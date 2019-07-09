#include "playlist/cloud/scheduler.h"
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "util/files.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "playlist/cloud/traverse.h"
#include "pdjson.h"
#include "audio/player.h"
#include "remote/commands/outgoing.h"
#include "playlist/cloud/advertising.h"

static const char *TAG = "cloud_sched";

static void scheduler_thread(void *args);
static bool on_file_parse_callback(const Track_t *track, void *ctx);
static bool is_stopping();
static void seconds_timer_callback(TimerHandle_t tmr);
static void play(const Track_t *track);

static const EventBits_t STOP_BIT = BIT0;
static const EventBits_t STOPPED_BIT = BIT1;
static EventGroupHandle_t event_group = NULL;
static TaskHandle_t thread_handle = NULL;
static TimerHandle_t seconds_timer = NULL;
static const Track_t *next_track_override = NULL; // TODO replace with Queue

bool cloud_scheduler_is_enabled() {
  return file_exists(CLOUD_SCHEDULER_PLAYLIST_PATH);
}

bool cloud_scheduler_init() {
  ESP_LOGI(TAG, "initializing");
  if (!cloud_scheduler_is_enabled()) {
    return false;
  }

  if (event_group == NULL) {
    event_group = xEventGroupCreate();
    if (event_group == NULL) {
      ESP_LOGE(TAG, "cannot create event group");
      return false;
    }
  }
  xEventGroupClearBits(event_group, STOP_BIT);
  xEventGroupClearBits(event_group, STOPPED_BIT);
  thread_handle = NULL;

  if (seconds_timer == NULL) {
    seconds_timer = xTimerCreate("seconds_timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, seconds_timer_callback);
    if (seconds_timer == NULL) {
      ESP_LOGE(TAG, "cannot create timer");
      return false;
    }
  }
  if (xTimerIsTimerActive(seconds_timer)) {
    xTimerStop(seconds_timer, portMAX_DELAY);
  } else {
    xTimerStart(seconds_timer, portMAX_DELAY);
  }

  return xTaskCreate(scheduler_thread, TAG, 3000, NULL, 6, &thread_handle) == pdPASS;
}

bool cloud_scheduler_deinit() {
  ESP_LOGI(TAG, "de-initializing");
  if (event_group == NULL) {
    return false;
  }
  if (thread_handle == NULL) {
    return true;
  }
  if (seconds_timer != NULL) {
    if (xTimerIsTimerActive(seconds_timer)) {
      xTimerStop(seconds_timer, portMAX_DELAY);
    }
  }

  player_stop();
  xEventGroupSetBits(event_group, STOP_BIT);
  bool ok = xEventGroupWaitBits(event_group, STOPPED_BIT, pdTRUE,	pdFALSE, portMAX_DELAY) & STOPPED_BIT;
  vTaskDelay(pdMS_TO_TICKS(50)); // HACK to "ensure" thread has died
  return ok;
}

static bool is_stopping() {
  if (event_group == NULL) {
    return false;
  }
  return xEventGroupGetBits(event_group) & STOP_BIT;
}

static void scheduler_thread(void *args) {

  if (!advertising_init(CLOUD_SCHEDULER_PLAYLIST_PATH)) {
    ESP_LOGE(TAG, "error initializing advertising");
  }

  while (!is_stopping()) {

    FILE *f = fopen(CLOUD_SCHEDULER_PLAYLIST_PATH, "r");
    if (f == NULL) {
      ESP_LOGE(TAG, "error opening playlist file");
      break;
    }
    json_stream json;
    json_open_stream(&json, f);
    traverse_playlist(&json, NULL, on_file_parse_callback);
    json_close(&json);
    fclose(f);

    taskYIELD();
  }

  advertising_deinit();

  xEventGroupSetBits(event_group, STOPPED_BIT);
  thread_handle = NULL;
  vTaskDelete(NULL);
}

static bool on_file_parse_callback(const Track_t *track, void *_ctx) {
  if (is_stopping()) {
    return false;
  }

  if (next_track_override != NULL) {
    play(next_track_override);
    next_track_override = NULL;
  }
  
  if (track->type != TRACK_BACKGROUND) {
    return true;
  }

  play(track);
  return true;
}

static void play(const Track_t *track) {
  size_t fullpath_len = strlen(CLOUD_SCHEDULER_FILES_PATH) + strlen(track->filename) + 4;
  char *fullpath = malloc(fullpath_len);
  snprintf(fullpath, fullpath_len, "%s/%s", CLOUD_SCHEDULER_FILES_PATH, track->filename);
  if (file_exists(fullpath)) {
    outgoing_command(TRACK_BEGIN, (void *)track, NULL);
    player_start(fullpath, NULL);
    outgoing_command(TRACK_END, (void *)track, NULL);
  } else {
    ESP_LOGE(TAG, "no such file: %s", fullpath);
  }
  free(fullpath);
}

static void seconds_timer_callback(TimerHandle_t tmr) {
  const Track_t *ad_now = advertising_now();
  if (ad_now == NULL) {
    return;
  }
  time_t t = time(NULL);
  ESP_LOGI(TAG, "time to interrupt for ad: %s [%s]", ad_now->filename, ctime(&t));
  next_track_override = ad_now;
  player_stop();
}
