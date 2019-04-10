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
#include "playlist/cloud/scheduler.h"
#include "remote/file_download.h"
#include "config/config.h"
#include "playlist/offline/scheduler.h"

static const char *TAG = "downloader";

static const EventBits_t RUNNING_BIT = BIT0;
static EventGroupHandle_t event_group = NULL;

static void thread(void *args);
static bool download_from_playlist(Tempfile_t *tmp_playlist);
static bool extract_and_download(json_stream *json);
static bool download_file(const char *url, const char *filename);

bool cloud_downloader_start(Tempfile_t *tmp_playlist) {
  if (event_group == NULL) {
    event_group = xEventGroupCreate();
    if (event_group == NULL) {
      ESP_LOGE(TAG, "cannot create event group");
      return false;
    }
  }
  if (cloud_downloader_is_running()) {
    ESP_LOGE(TAG, "download is already running");
    return false;
  }

  BaseType_t task_created = xTaskCreate(thread, TAG, 8192, (void *)tmp_playlist, 15, NULL);
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

static void thread(void *args) {
  xEventGroupSetBits(event_group, RUNNING_BIT);

  ESP_LOGI(TAG, "begin files download");
  // TODO update_playlist command outgoing

  Tempfile_t *tmp_playlist = (Tempfile_t *)args;
  if (tmp_playlist == NULL) {
    ESP_LOGE(TAG, "no tempfile");
  } else {
    if (mkdir_p(CLOUD_SCHEDULER_FILES_PATH) != 0) {
      ESP_LOGE(TAG, "cannot create subdir %s", CLOUD_SCHEDULER_FILES_PATH);
    } else {
      scheduler_suspend();
      if (download_from_playlist(tmp_playlist)) {
        file_copy(tmp_playlist->path, CLOUD_SCHEDULER_PLAYLIST_PATH);
      }
      tempfile_remove(tmp_playlist);
    }
  }

  ESP_LOGI(TAG, "finish files download");
  // TODO ack

  xEventGroupClearBits(event_group, RUNNING_BIT);
  vTaskDelete(NULL);
}

static bool download_from_playlist(Tempfile_t *tmp_playlist) {
  json_stream json;
  tempfile_open(tmp_playlist, "r");
  json_open_stream(&json, tmp_playlist->file);
  bool ok = extract_and_download(&json);
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

  size_t pathlen = strlen(filename) + strlen(CLOUD_SCHEDULER_FILES_PATH) + 5;
  char *buffer = malloc(pathlen);
  if (buffer == NULL) {
    return false;
  }
  snprintf(buffer, pathlen, "%s/%s", CLOUD_SCHEDULER_FILES_PATH, filename);
  remove(buffer); // remove file if it exists
  int result = file_download_start(full_url, buffer, 16384);

  bool ok = true;
  if (result < 200 || result > 299) {
    ESP_LOGE(TAG, "error downloading file from %s to %s", full_url, buffer);
    ok = false;
  } else {
    ESP_LOGI(TAG, "downloaded file %s", buffer);
  }
  free(buffer);
  if (malloced_full_url) {
    free(full_url);
  }
  return ok;
}

static bool extract_and_download(json_stream *json) {
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

  } while (t != JSON_DONE && t != JSON_ERROR);

  return true;
}
