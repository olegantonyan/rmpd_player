#include "playlist/cloud/downloader.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "pdjson.h"
#include "util/files.h"
#include "util/tempfile.h"
#include "playlist/cloud/scheduler.h"
#include "remote/file_download.h"
#include "config/config.h"
#include "playlist/offline/scheduler.h"
#include "remote/commands/outgoing.h"
#include "playlist/cloud/traverse.h"
#include <dirent.h>
#include "playlist/cloud/cleanup_files.h"
#include "system/status.h"

static const char *TAG = "downloader";

static const EventBits_t RUNNING_BIT = BIT0;
static const EventBits_t STOP_BIT = BIT1;
static const EventBits_t STOPPED_BIT = BIT2;
static EventGroupHandle_t event_group = NULL;

static void thread(void *args);
static bool download_from_playlist(Tempfile_t *tmp_playlist, uint32_t sequence);
static bool extract_and_download(json_stream *json, uint32_t sequence);
static bool download_file(const char *url, const char *filename);
static void notify(uint32_t files_done, uint32_t sequence);
static bool is_stopping();

typedef struct {
  Tempfile_t *tmp_playlist;
  uint32_t sequence;
} ThreadArg_t;

bool cloud_downloader_start(Tempfile_t *tmp_playlist, uint32_t sequence) {
  if (event_group == NULL) {
    event_group = xEventGroupCreate();
    if (event_group == NULL) {
      ESP_LOGE(TAG, "cannot create event group");
      return false;
    }
  }
  if (cloud_downloader_is_running()) {
    ESP_LOGW(TAG, "download is already running. stopping it...");
    if (!cloud_downloader_stop()) {
      ESP_LOGE(TAG, "error stopping downloader");
      return false;
    }
  }
  xEventGroupClearBits(event_group, RUNNING_BIT);
  xEventGroupClearBits(event_group, STOP_BIT);
  xEventGroupClearBits(event_group, STOPPED_BIT);

  ThreadArg_t *ta = malloc(sizeof(ThreadArg_t)); // free in thread
  ta->tmp_playlist = tmp_playlist;
  ta->sequence = sequence;

  BaseType_t task_created = xTaskCreate(thread, TAG, 7500, (void *)ta, 5, NULL);
  if (pdPASS != task_created) {
    ESP_LOGE(TAG, "cannot create thread");
    return false;
  }
  return true;
}

bool cloud_downloader_is_running() {
  if (event_group == NULL) {
    return false;
  }
  return xEventGroupGetBits(event_group) & RUNNING_BIT;
}

bool cloud_downloader_stop() {
  if (!cloud_downloader_is_running()) {
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

static void thread(void *args) {
  xEventGroupSetBits(event_group, RUNNING_BIT);

  cloud_cleanup_files_stop();

  ESP_LOGI(TAG, "begin files download");

  status_set_synchronizing(true);

  ThreadArg_t *ta = (ThreadArg_t *)args;
  Tempfile_t *tmp_playlist = ta->tmp_playlist;
  uint32_t sequence = ta->sequence;

  bool ok = true;

  if (tmp_playlist == NULL) {
    ESP_LOGE(TAG, "no tempfile");
    ok = false;
  } else {
    if (mkdir_p(CLOUD_SCHEDULER_FILES_PATH) != 0) {
      ESP_LOGE(TAG, "cannot create subdir %s", CLOUD_SCHEDULER_FILES_PATH);
      ok = false;
    } else {
      offline_scheduler_deinit();
      if (download_from_playlist(tmp_playlist, sequence)) {
        cloud_cleanup_files_stop();
        cloud_scheduler_deinit();
        remove(CLOUD_SCHEDULER_PLAYLIST_PATH);
        if (tempfile_rename(tmp_playlist, CLOUD_SCHEDULER_PLAYLIST_PATH)) {
          ESP_LOGI(TAG, "playlist file saved to %s", CLOUD_SCHEDULER_PLAYLIST_PATH);
        } else {
          ESP_LOGE(TAG, "error saving playlist file: %s", strerror(errno));
          ok = false;
        }
        cloud_scheduler_init();
        cloud_cleanup_files_start(0);
      } else {
        ok = false;
      }
    }
  }

  if (is_stopping()) {
    ESP_LOGI(TAG, "stopped");
  } else {
    ESP_LOGI(TAG, "finish files download");
    if (ok) {
      AckCommandArgs_t a = {
        .sequence = sequence,
        .message = "files download complete"
      };
      outgoing_command(ACK_OK, &a, NULL);
    } else {
      AckCommandArgs_t a = {
        .sequence = sequence,
        .message = "files download error"
      };
      outgoing_command(ACK_FAIL, &a, NULL);
    }
  }

  free(args);
  status_set_synchronizing(false);
  xEventGroupClearBits(event_group, RUNNING_BIT);
  xEventGroupSetBits(event_group, STOPPED_BIT);
  vTaskDelete(NULL);
}

static bool download_from_playlist(Tempfile_t *tmp_playlist, uint32_t sequence) {
  json_stream json;
  tempfile_open(tmp_playlist, "r");
  json_open_stream(&json, tmp_playlist->file);
  bool ok = extract_and_download(&json, sequence);
  json_close(&json);
  return ok;
}

static bool download_file(const char *url, const char *filename) {
  char *full_url = NULL;
  bool malloced_full_url = false;
  if (strncmp(url, "http", 4) == 0) { // full url
    full_url = (char *)url;
  } else { // relative
    const char *server_url = config_server_url();
    size_t full_url_len = strlen(url) + strlen(server_url) + 25;
    full_url = malloc(full_url_len);
    full_url[0] = '\0';
    if (url[0] == '/') {
      snprintf(full_url, full_url_len, "%s%s", server_url, url);
    } else {
      snprintf(full_url, full_url_len, "%s/%s", server_url, url);
    }
    malloced_full_url = true;
  }


  size_t filepath_permanent_len = strlen(filename) + strlen(CLOUD_SCHEDULER_FILES_PATH) + 5;
  char *filepath_permanent = malloc(filepath_permanent_len);
  if (filepath_permanent == NULL) {
    return false;
  }
  snprintf(filepath_permanent, filepath_permanent_len, "%s/%s", CLOUD_SCHEDULER_FILES_PATH, filename);

  size_t filepath_temp_len = strlen(filename) + strlen(TEMPFILE_DIR) + 5;
  char *filepath_temp = malloc(filepath_temp_len);
  if (filepath_permanent == NULL) {
    return false;
  }
  snprintf(filepath_temp, filepath_temp_len, "%s/%s", TEMPFILE_DIR, filename);


  bool ok = true;
  if (!file_exists(filepath_permanent)) {
    int result = 0;
    while (true) {
      result = file_download_start(full_url, filepath_temp, 4096, is_stopping);
      if (is_stopping()) {
        break;
      }
      if (result < 200 || result > 299) {
        ESP_LOGW(TAG, "error downloading file (status %d), retrying...", result);
        vTaskDelay(pdMS_TO_TICKS(1000));
      } else {
        break;
      }
    }

    if (is_stopping()) {
      remove(filepath_temp);
      ok = false;
    } else {
      rename(filepath_temp, filepath_permanent);
      ESP_LOGI(TAG, "downloaded file %s", filepath_permanent);
    }
  } else {
    ESP_LOGI(TAG, "file %s already exists, skipping", filepath_permanent);
  }

  free(filepath_permanent);
  free(filepath_temp);

  if (malloced_full_url) {
    free(full_url);
  }
  return ok;
}

static void notify(uint32_t files_done, uint32_t sequence) {
  UpdatePlaylistCommandArgs_t a = {
    .sequence = sequence,
    .files_done = files_done
  };
  outgoing_command(UPDATE_PLAYLIST, &a, NULL);
}

static bool extract_and_download(json_stream *json, uint32_t sequence) {
  uint32_t files_done = 0;
  size_t len = 0;
  enum json_type t = JSON_ERROR;
  bool url_found = false;
  bool filename_found = false;

  char url[2048] = { 0 };
  char filename[256] = { 0 };

  do {
    t = json_next(json);
    switch(t) {
      case JSON_STRING: {
        const char *s = json_get_string(json, &len);
        if (url_found) {
          url[0] = '\0';
          if (len > sizeof(url)) {
            ESP_LOGE(TAG, "too long url %s: %d", s, len);
            return false;
          }
          strncpy(url, s, len);
          url_found = false;
        } else if (strcmp(s, "url") == 0) {
          url_found = true;
        } else if (filename_found) {
          filename[0] = '\0';
          if (len > sizeof(filename)) {
            ESP_LOGE(TAG, "too long filename %s: %d", s, len);
            return false;
          }
          strncpy(filename, s, len);
          filename_found = false;
        } else if (strcmp(s, "filename") == 0) {
          filename_found = true;
        }

        if (strlen(url) > 0 && strlen(filename) > 0) {
          if(!download_file(url, filename)) {
            ESP_LOGE(TAG, "error downloading file from %s", url);
            return false;
          }

          memset(url, 0, sizeof(url));
          memset(filename, 0, sizeof(filename));
          url_found = false;
          filename_found = false;
          files_done++;
          notify(files_done, sequence);
        }

        break;
      }
      case JSON_OBJECT_END:
        memset(url, 0, sizeof(url));
        memset(filename, 0, sizeof(filename));
        url_found = false;
        filename_found = false;
        break;

      default:
        break;
      case JSON_ERROR:
        ESP_LOGE(TAG, "json parse error: %s line %d pos %d", json_get_error(json), json_get_lineno(json), json_get_position(json));
        return false;
        break;
    }

  } while (t != JSON_DONE && t != JSON_ERROR && !is_stopping());

  return true;
}
