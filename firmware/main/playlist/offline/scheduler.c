#include "playlist/offline/scheduler.h"
#include "audio/player.h"
#include "playlist/offline/random.h"
#include "playlist/offline/recurse.h"
#include "audio/stream_playlist.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "storage/sd.h"
#include "util/strings.h"
#include "config/config.h"
#include "playlist/stream_scheduler.h"
#include "wifi/wifi.h"
#include "remote/commands/outgoing.h"
#include "playlist/track.h"

static const char *TAG = "scheduler";

static struct s_state {
  SemaphoreHandle_t mutex;
  uint16_t current;
  uint16_t next;
  uint16_t total;
} state = { NULL, 0, 0, 0 };

static void scheduler_thread(void * args);
static bool play(const char *path);
static void stop();
static void on_medifile_callback(const char *path, uint16_t index);
static void mediafile_enum_func(const char *fname, uint16_t index);

bool scheduler_init() {
  if (!player_init()) {
    ESP_LOGE(TAG, "error initializing player");
    return false;
  }
  state.mutex = xSemaphoreCreateMutex();
  if (state.mutex == NULL) {
    ESP_LOGE(TAG, "cannot create mutex");
    return false;
  }

  scheduler_set_random(config_random());

  return xTaskCreate(scheduler_thread, "scheduler", 4096, NULL, 6, NULL) == pdPASS;
}

bool scheduler_random() {
  return config_random();
}

bool scheduler_set_random(bool arg) {
  ESP_LOGI(TAG, "set random %d", (int)arg);

  if (arg != config_random()) {
    config_save_random(arg);
  }
  return true;
}

bool scheduler_next() {
  ESP_LOGD(TAG, "next");
  xSemaphoreTake(state.mutex, portMAX_DELAY);
  if (state.current >= (state.total - 1)) {
    state.next = 0;
  } else {
    state.next = state.current + 1;
  }
  // this will force skip to the next track in scheduler_thread
  stop();
  xSemaphoreGive(state.mutex);
  return true;
}

bool scheduler_prev() {
  ESP_LOGD(TAG, "prev");
  xSemaphoreTake(state.mutex, portMAX_DELAY);
  if (state.current == 0) {
    state.next = state.total - 1;
  } else {
    state.next = state.current - 1;
  }
  // this will force skip to the next track in scheduler_thread
  stop();
  xSemaphoreGive(state.mutex);
  return true;
}

bool scheduler_mediafile_match_func(const char *fname) {
  return string_ends_with(fname, ".mp3") || stream_playlist_is_stream(fname);
}

static void scheduler_thread(void * args) {
  DIR *dp = opendir(STORAGE_SD_MOUNTPOINT);
  if (dp == NULL) {
    ESP_LOGE(TAG, "cannot open root directory");
    vTaskDelete(NULL);
    return;
  }
  closedir(dp);

  if (!stream_scheduler_init(STORAGE_SD_MOUNTPOINT)) {
    ESP_LOGE(TAG, "cannot initialize stream scheduler");
  }

  uint16_t index = 0;
  uint32_t total_mediafiles = recurse_dir(STORAGE_SD_MOUNTPOINT, 0, &index, mediafile_enum_func, scheduler_mediafile_match_func);
  ESP_LOGI(TAG, "total media files: %u", total_mediafiles);
  if (total_mediafiles > SCHEDULER_MAX_MEDIAFILES) {
    ESP_LOGE(TAG, "too many media files");
  }

  if (!stream_scheduler_start()) {
    ESP_LOGE(TAG, "cannot start stream scheduler");
  }

  if (total_mediafiles > 0 && total_mediafiles < SCHEDULER_MAX_MEDIAFILES) {
    xSemaphoreTake(state.mutex, portMAX_DELAY);
    state.total = (uint16_t)total_mediafiles;
    random_init(state.total - 1);
    if (scheduler_random()) {
      state.next = random_next();
    }
    xSemaphoreGive(state.mutex);

    if (stream_scheduler_max() > 0) {
      wifi_wait_connected(5000);
    }

    while(true) {
      index = 0;
      recurse_dir(STORAGE_SD_MOUNTPOINT, 0, &index, on_medifile_callback, scheduler_mediafile_match_func);
      taskYIELD();
      random_reset();
    }
  }

  stream_scheduler_deinit();

  vTaskDelete(NULL);
}

static void mediafile_enum_func(const char *fname, uint16_t index) {
  if (stream_playlist_is_stream(fname)) {
    ESP_LOGI(TAG, "adding %s as a stream with index %d", fname, index);
    stream_scheduler_add_stream(index);
  }
}

static void on_medifile_callback(const char *path, uint16_t index) {
  ESP_LOGD(TAG, "index %u next_index %u : %s", index, state.next, path);
  if (index != state.next) {
    return;
  }
  xSemaphoreTake(state.mutex, portMAX_DELAY);
  state.current = index;
  if (scheduler_random()) {
    state.next = random_next();
  } else {
    if (state.next >= (state.total - 1)) {
      state.next = 0;
    } else {
      state.next = state.current + 1;
    }
  }
  xSemaphoreGive(state.mutex);

  if (stream_scheduler_is_known_stream(index)) {
    if (stream_scheduler_is_alive(index)) { // play stream here if you can
      if (play(path)) {
        ESP_LOGD(TAG, "stream finished successfuly");
        stream_scheduler_mark_alive(index);
      } else {
        ESP_LOGD(TAG, "stream finished with error");
        stream_scheduler_mark_dead(index);
      }
    }
  } else if (!stream_scheduler_any_alive_streams()) { // play a file if there are no alive streams
    play(path);
  }
}

static bool play(const char *path) {
  ESP_LOGD(TAG, "starting '%s'", path);

  Track_t track = { // offline scheduler knows nothing about track ids
    .filename = path,
    .id = 0
  };
  outgoing_command(TRACK_BEGIN, &track, NULL);
  bool ok = player_start(path, stream_scheduler_force_probe); // force stream probe after any error regardless of retries #
  outgoing_command(TRACK_END, &track, NULL);

  return ok;
}

static void stop() {
  ESP_LOGD(TAG, "stopping");
  player_stop();
}
