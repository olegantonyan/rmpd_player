#include "playlist/cloud/traverse.h"
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "traverse";

static bool traverse_playlist_json(json_stream *json, void *ctx, void (*callback)(const Track_t *track, void *ctx));

bool traverse_playlist(json_stream *playlist,  void *ctx, void (*callback)(const Track_t *track, void *ctx)) {
  if (callback == NULL) {
    return false;
  }
  return traverse_playlist_json(playlist, ctx, callback);
}

static bool traverse_playlist_json(json_stream *json, void *ctx, void (*callback)(const Track_t *track, void *ctx)) {
  enum json_type t = JSON_ERROR;
  bool filename_found = false;
  bool id_found = false;

  char filename[256] = { 0 };
  int64_t id = 0;

  do {
    t = json_next(json);
    switch(t) {
      case JSON_NUMBER: {
        double num = json_get_number(json);
        if (id_found) {
          id = (int64_t)num;
          id_found = false;
        }

        break;
      }
      case JSON_STRING: {
        size_t len = 0;
        const char *s = json_get_string(json, &len);
        if (strcmp(s, "media_item_id") == 0) {
          id_found = true;
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

        break;
      }
      case JSON_OBJECT_END:
        id = 0;
        memset(filename, 0, sizeof(filename));
        id_found = false;
        filename_found = false;
        break;

      default:
        break;
      case JSON_ERROR:
        ESP_LOGE(TAG, "json parse error: %s line %d pos %d", json_get_error(json), json_get_lineno(json), json_get_position(json));
        return false;
        break;
    }

    if (id != 0 && strlen(filename) > 0) {
      const Track_t track = {
        .filename = (const char *)filename,
        .id = id
      };
      callback(&track, ctx);

      id = 0;
      memset(filename, 0, sizeof(filename));
      id_found = false;
      filename_found = false;
    }

    taskYIELD();
  } while (t != JSON_DONE && t != JSON_ERROR);

  return true;
}
