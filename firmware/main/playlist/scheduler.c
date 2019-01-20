#include "playlist/scheduler.h"
#include "audio/player.h"
#include "audio/random.h"
#include "playlist/recurse.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "storage/sd.h"
#include <dirent.h>
#include "util/strings.h"
#include "config/config.h"

static const char *TAG = "scheduler";

static struct s_state {
  SemaphoreHandle_t mutex;
  uint16_t current;
  uint16_t next;
  uint16_t total;
} state = { NULL, 0, 0, 0 };

static void scheduler_thread(void * args);
static void play(const char *path);
static void stop();
static void on_medifile_callback(const char *path, uint16_t index);
static bool mediafile_match_func(const char *fname);

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

static void scheduler_thread(void * args) {
  DIR *dp = opendir(STORAGE_SD_MOUNTPOINT);
  if (dp == NULL) {
    ESP_LOGE(TAG, "cannot open root directory");
    vTaskDelete(NULL);
    return;
  }
  closedir(dp);

  uint32_t total_mediafiles = recurse_dir(STORAGE_SD_MOUNTPOINT, 0, NULL, NULL, mediafile_match_func);
  ESP_LOGI(TAG, "total media files: %u", total_mediafiles);
  if (total_mediafiles > 65535) {
    ESP_LOGE(TAG, "too many media files");
    vTaskDelete(NULL);
    return;
  }
  xSemaphoreTake(state.mutex, portMAX_DELAY);
  state.total = (uint16_t)total_mediafiles;
  random_init(state.total - 1);
  if (scheduler_random()) {
    state.next = random_next();
  }
  xSemaphoreGive(state.mutex);

  while(true) {
    uint16_t index = 0;
    recurse_dir(STORAGE_SD_MOUNTPOINT, 0, &index, on_medifile_callback, mediafile_match_func);
    taskYIELD();
    random_reset();
  }
}

static bool mediafile_match_func(const char *fname) {
  return string_ends_with(fname, ".mp3") || string_ends_with(fname, ".m3u") || string_ends_with(fname, ".pls");
}

static void on_medifile_callback(const char *path, uint16_t index) {
  ESP_LOGD(TAG, "index %u next_index %u : %s", index, state.next, path);
  if (index == state.next) {
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
    play(path);
  }
}

static void play(const char *path) {
  ESP_LOGD(TAG, "starting '%s'", path);
  player_start(path, false);
}

static void stop() {
  ESP_LOGD(TAG, "stopping");
  player_stop();
}
