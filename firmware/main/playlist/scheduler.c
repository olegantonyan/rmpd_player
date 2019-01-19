#include "playlist/scheduler.h"
#include "audio/player.h"
#include "audio/random.h"

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
static uint32_t recurse_dir(const char *path, uint8_t depth, uint16_t *index, void (*callback)(const char *fullname, uint16_t index));
static void play(const char *path);
static void stop();
static void on_medifile_callback(const char *path, uint16_t index);

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

  uint32_t total_mediafiles = recurse_dir(STORAGE_SD_MOUNTPOINT, 0, NULL, NULL);
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
    recurse_dir(STORAGE_SD_MOUNTPOINT, 0, &index, on_medifile_callback);
    taskYIELD();
    random_reset();
  }
}

static uint32_t recurse_dir(const char *path, uint8_t depth, uint16_t *index, void (*callback)(const char *fullname, uint16_t index)) {
  if (depth > 10) {
    ESP_LOGW(TAG, "directory recurse max depth reached");
    return 0;
  }

  DIR *dp = opendir(path);
  if (dp == NULL) {
    ESP_LOGE(TAG, "error opening directory %s: %s", path, strerror(errno));
    return 0;
  }

  uint32_t files_in_dir = 0;
  while(true) {
    struct dirent *ep = readdir(dp);
    if (!ep) {
      break;
    }
    if (ep->d_type == DT_DIR) {
      if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) {
        continue;
      }
      size_t newpath_len = strlen(path) + strlen(ep->d_name) + 10;
      char *newpath = malloc(newpath_len);
      snprintf(newpath, newpath_len, "%s/%s", path, ep->d_name);
      files_in_dir += recurse_dir(newpath, depth + 1, index, callback);
      free(newpath);
    } else if (string_ends_with(ep->d_name, ".mp3") || string_ends_with(ep->d_name, ".m3u") || string_ends_with(ep->d_name, ".pls")) {
      size_t name_len = strlen(path) + strlen(ep->d_name) + 10;
      char *fullname = malloc(name_len);
      snprintf(fullname, name_len, "%s/%s", path, ep->d_name);
      if (callback != NULL && index != NULL) {
        callback(fullname, *index);
      }
      files_in_dir++;
      if (index != NULL) {
        (*index)++;
      }
      free(fullname);
    } else {
      ESP_LOGD(TAG, "'%s' is not supported media file", ep->d_name);
    }
    taskYIELD();
  }
  closedir(dp);

  return files_in_dir;
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
