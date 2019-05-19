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
#include "playlist/cloud/traverse.h"
#include "pdjson.h"
#include "audio/player.h"
#include "remote/commands/outgoing.h"

static const char *TAG = "cloud_sched";

static void scheduler_thread(void *args);
static void on_file_parse_callback(const Track_t *track, void *ctx);
static bool is_stopping();

static const EventBits_t STOP_BIT = BIT0;
static const EventBits_t STOPPED_BIT = BIT1;
static EventGroupHandle_t event_group = NULL;

bool cloud_scheduler_is_enabled() {
  return file_exists(CLOUD_SCHEDULER_PLAYLIST_PATH);
}

bool cloud_scheduler_init() {
  ESP_LOGI(TAG, "initializing");
  if (!cloud_scheduler_is_enabled()) {
    return false;
  }

  event_group = xEventGroupCreate();
  if (event_group == NULL) {
    ESP_LOGE(TAG, "cannot create event group");
    return false;
  }

  return xTaskCreate(scheduler_thread, TAG, 4096, NULL, 6, NULL) == pdPASS;
}

bool cloud_scheduler_deinit() {
  ESP_LOGI(TAG, "de-initializing");
  if (event_group == NULL) {
    return false;
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
  xEventGroupSetBits(event_group, STOPPED_BIT);
  vTaskDelete(NULL);
}

static void on_file_parse_callback(const Track_t *track, void *_ctx) {
  if (is_stopping()) {
    return;
  }

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
