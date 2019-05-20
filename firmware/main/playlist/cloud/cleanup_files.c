#include "playlist/cloud/cleanup_files.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "pdjson.h"
#include "util/files.h"
#include "playlist/cloud/scheduler.h"
#include "playlist/cloud/traverse.h"
#include <dirent.h>
#include "playlist/track.h"
#include <errno.h>

static const char *TAG = "cleanup_files";

static const EventBits_t STOP_BIT = BIT0;
static const EventBits_t STOPPED_BIT = BIT1;
static EventGroupHandle_t event_group = NULL;
static TaskHandle_t thread_handle = NULL;

typedef struct {
  const char *target_filename;
  bool found;
} CleanupNonPlaylistContext_t;

static void doit(const char *playlist_path, const char *mediafiles_path, const char *playlist_filename);
static bool callback(const Track_t *track, void *ctx);
static void thread(void *arg);
static bool is_stopping();

bool cloud_cleanup_files_start() {
  ESP_LOGI(TAG, "starting");
  if (cloud_cleanup_files_is_running()) {
    if (!cloud_cleanup_files_stop()) {
      ESP_LOGE(TAG, "cannot stop running process");
      return false;
    }
  }

  event_group = xEventGroupCreate();
  if (event_group == NULL) {
    ESP_LOGE(TAG, "cannot create event group");
    return false;
  }

  return xTaskCreate(thread, TAG, 3000, NULL, tskIDLE_PRIORITY, &thread_handle) == pdPASS;
}

bool cloud_cleanup_files_is_running() {
  return thread_handle != NULL;
}

bool cloud_cleanup_files_stop() {
  ESP_LOGI(TAG, "stopping");

  if (event_group == NULL || !cloud_cleanup_files_is_running()) {
    return true;
  }

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

static void thread(void *arg) {
  ESP_LOGI(TAG, "started");
  doit(CLOUD_SCHEDULER_PLAYLIST_PATH, CLOUD_SCHEDULER_FILES_PATH, CLOUD_SCHEDULER_PLAYLIST_FILENAME);
  ESP_LOGI(TAG, "finished");

  thread_handle = NULL;
  xEventGroupSetBits(event_group, STOPPED_BIT);
  vTaskDelete(NULL);
}

static void doit(const char *playlist_path, const char *mediafiles_path, const char *playlist_filename) {
  DIR *dp = opendir(mediafiles_path);
  if (dp == NULL) {
    ESP_LOGE(TAG, "error opening directory %s: %s", mediafiles_path, strerror(errno));
    return;
  }

  while(!is_stopping()) {
    struct dirent *ep = readdir(dp);
    if (!ep) {
      break;
    }

    if (ep->d_type == DT_REG && strcmp(ep->d_name, playlist_filename) != 0) {

      FILE *f = fopen(playlist_path, "r");
      if (f == NULL) {
        ESP_LOGE(TAG, "error opening playlist file to cleanup nonplaylist files");
        return;
      }
      json_stream json;
      CleanupNonPlaylistContext_t ctx = {
        .target_filename = (const char *)ep->d_name,
        .found = false
      };
      json_open_stream(&json, f);
      bool ok = traverse_playlist(&json, (void *)&ctx, callback);
      json_close(&json);
      fclose(f);

      if (ok && !ctx.found) {
        ESP_LOGI(TAG, "removing non-playlist file %s", ep->d_name);
        size_t newpath_len = strlen(mediafiles_path) + strlen(ep->d_name) + 4;
        char *newpath = malloc(newpath_len);
        snprintf(newpath, newpath_len, "%s/%s", mediafiles_path, ep->d_name);
        remove(newpath);
        free(newpath);
      }

    }
    taskYIELD();
  }

  closedir(dp);
}

static bool callback(const Track_t *track, void *context) {
  if (is_stopping()) {
    return false;
  }

  CleanupNonPlaylistContext_t *ctx = (CleanupNonPlaylistContext_t *)context;
  if (strcmp(ctx->target_filename, track->filename) == 0) {
    ctx->found = true;
  }
  taskYIELD();
  return true;
}
