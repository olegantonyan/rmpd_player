#include "audio/scheduler.h"
#include "audio/player.h"

#include <string.h>
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
#include "util/files.h"

static const char *TAG = "scheduler";

static void scheduler_thread(void * args);
static void recurse_dir(const char *path, uint8_t depth);

bool scheduler_init() {
  if (!player_init()) {
    ESP_LOGE(TAG, "error initializing player");
    return false;
  }

  return xTaskCreate(scheduler_thread, "scheduler", 4096, NULL, 6, NULL) == pdPASS;
}

bool scheduler_next() {
  ESP_LOGI(TAG, "next");
  return true;
}

bool scheduler_prev() {
  ESP_LOGI(TAG, "prev");
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
  while(true) {
    recurse_dir(STORAGE_SD_MOUNTPOINT, 0);
    taskYIELD();
  }
}

static void recurse_dir(const char *path, uint8_t depth) {
  if (depth > 20) {
    ESP_LOGW(TAG, "directory recurse max depth reached");
    return;
  }
  DIR *dp = opendir(path);
  if (dp == NULL) {
    ESP_LOGE(TAG, "error opening directory %s", path);
  } else {
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
        recurse_dir(newpath, depth + 1);
        free(newpath);
      } else if (string_ends_with(ep->d_name, ".mp3")) {
        size_t name_len = strlen(path) + strlen(ep->d_name) + 10;
        char *fullname = malloc(name_len);
        snprintf(fullname, name_len, "%s/%s", path, ep->d_name);
        ESP_LOGD(TAG, "starting '%s'", fullname);
        player_start(fullname, false);
        free(fullname);
      } else {
        ESP_LOGD(TAG, "'%s' is not mp3", ep->d_name);
      }
      taskYIELD();
    }
    closedir(dp);
  }
}
