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

static const char *TAG = "scheduler";

static void scheduler_thread(void * args);
static bool string_ends_with(const char *str, const char *suffix);

bool scheduler_start() {
  player_init();

  return xTaskCreate(scheduler_thread, "scheduler", 4096, NULL, 5, NULL) == pdPASS;
}

static void scheduler_thread(void * args) {
  while(true) {

    DIR *dp = opendir(STORAGE_SD_MOUNTPOINT);
    if (dp == NULL) {
      ESP_LOGE(TAG, "error opening directory");
    } else {
      while(true) {
        struct dirent *ep = readdir(dp);
        if (!ep) {
          break;
        }

        if (string_ends_with(ep->d_name, ".mp3")) {
          char fullname[1024] = { 0 };
          snprintf(fullname, sizeof(fullname), "%s/%s", STORAGE_SD_MOUNTPOINT, ep->d_name);
          ESP_LOGD(TAG, "starting '%s'", fullname);
          player_start(fullname, false);
        } else {
          ESP_LOGD(TAG, "'%s' is not mp3", ep->d_name);
        }
        taskYIELD();
      }
      closedir(dp);
    }

    taskYIELD();
  }
}

static bool string_ends_with(const char *str, const char *suffix) {
  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);
  return (str_len >= suffix_len) && (0 == strcmp(str + (str_len - suffix_len), suffix));
}
