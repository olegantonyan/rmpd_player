#include "playlist/cloud/advertising.h"
#include <stdio.h>
#include <string.h>
#include "pdjson.h"
#include "esp_system.h"
#include "esp_log.h"
#include "playlist/cloud/traverse.h"
#include "util/datetime.h"

static const char *TAG = "advertising";

static Track_t *ads[ADVERTISING_MAX_TRACKS] = { NULL };

static bool on_file_parse_callback(const Track_t *track, void *_ctx);
static void deinit();

bool advertising_init(const char *playlist_file_path) {
  deinit();
  ESP_LOGI(TAG, "initializing");

  FILE *f = fopen(playlist_file_path, "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "error opening playlist file");
    return false;
  }
  json_stream json;
  json_open_stream(&json, f);
  traverse_playlist(&json, NULL, on_file_parse_callback);
  json_close(&json);
  fclose(f);

/*
  for (size_t j = 0; j < ADVERTISING_MAX_TRACKS; j++) {
    Track_t *track = ads[j];
    if (track != NULL) {
      for (size_t i = 0; i < track->date_intervals_size; i++) {
        ESP_LOGI(TAG, "ad: %s interval begin %d end %d", track->filename, (int)track->date_intervals[i].begin_date, (int)track->date_intervals[i].end_date);
        for (size_t j = 0; j < track->date_intervals[i].schedule_seconds_size; j++) {
          ESP_LOGI(TAG, " ---- at %d", (int)track->date_intervals[i].schedule_seconds[j]);
        }
      }
    }
  }
*/
  return true;
}

Track_t *advertising_now() {
  int32_t date_now = datetime_current_date_int();
  int32_t time_now = datetime_current_time_int();

  for (size_t j = 0; j < ADVERTISING_MAX_TRACKS; j++) {
    Track_t *track = ads[j];
    if (track == NULL || track->date_intervals == NULL) {
      continue;
    }

    for (size_t i = 0; i < track->date_intervals_size; i++) {
      if (track->date_intervals[i].begin_date <= date_now && date_now <= track->date_intervals[i].end_date) {
        for (size_t k = 0; k < track->date_intervals[i].schedule_seconds_size; k++) {
          if (track->date_intervals[i].schedule_seconds != NULL) {
            if (track->date_intervals[i].schedule_seconds[k] == time_now) {
              return track;
            }
          }
        }
      }
    }

  }
  return NULL;
}

void advertising_deinit() {
  ESP_LOGI(TAG, "de-initializing");
  deinit();
}

static void deinit() {
  for (size_t i = 0; i < ADVERTISING_MAX_TRACKS; i++) {
    if (ads[i] != NULL) {
      track_free(ads[i]);
      free(ads[i]);
      ads[i] = NULL;
    }
  }
  memset(ads, 0, sizeof(ads));
}

static bool on_file_parse_callback(const Track_t *track, void *_ctx) {
  if (track->type != TRACK_ADVERTISING) {
    return true;
  }

  for (size_t i = 0; i < ADVERTISING_MAX_TRACKS; i++) {
    if (ads[i] == NULL) {
      ads[i] = malloc(sizeof(Track_t));
      track_copy(ads[i], track);
      break;
    }
  }

  return true;
}
