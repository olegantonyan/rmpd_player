#include "playlist/cloud/traverse.h"
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "util/pdjson_helper.h"
#include "util/strings.h"
#include "util/datetime.h"

static const char *TAG = "traverse";

typedef struct {
  Track_t track;

  struct {
    uint8_t filename:1;
    uint8_t id:1;
    uint8_t type:1;
    uint8_t schedule_intervals:1;
  } found;

  struct {
    uint8_t schedule_intervals:1;
    uint8_t schedule:1;
  } searching;
} ParserContext_t;

static bool traverse_playlist_json(json_stream *json, void *ctx, bool (*callback)(const Track_t *track, void *ctx));
static void on_parse(const char *key, const pdjson_value_t *value, pdjson_helper_value_type_t type, void *ctx);
static bool track_ready(const ParserContext_t *context);

bool traverse_playlist(json_stream *playlist,  void *ctx, bool (*callback)(const Track_t *track, void *ctx)) {
  if (callback == NULL) {
    return false;
  }
  return traverse_playlist_json(playlist, ctx, callback);
}

static bool traverse_playlist_json(json_stream *json, void *ctx, bool (*callback)(const Track_t *track, void *ctx)) {
  enum json_type t = JSON_ERROR;

  pdjson_helper_state_t state;
  pdjson_helper_init(&state, json);

  ParserContext_t context;
  memset(&context, 0, sizeof(context));

  bool ok = true;
  bool callback_was_called = false;
  do {
    t = json_next(json);
    switch(t) {
      case JSON_ERROR:
        ESP_LOGE(TAG, "json parse error: %s line %d pos %d", json_get_error(json), json_get_lineno(json), json_get_position(json));
        ok = false;
        break;

      case JSON_OBJECT:
        if (context.searching.schedule_intervals) {
          //printf("JSON_OBJECT %d\n", context.track.date_intervals_size);
          context.track.date_intervals_size++;
          context.track.date_intervals = realloc(context.track.date_intervals, sizeof(DateInterval_t) * (context.track.date_intervals_size + 1));
          context.track.date_intervals[context.track.date_intervals_size - 1].schedule_seconds_size = 0;
          context.track.date_intervals[context.track.date_intervals_size - 1].schedule_seconds = NULL;
        }
        pdjson_helper_reset(&state);
        break;

      case JSON_OBJECT_END:
        if (callback_was_called) {
          callback_was_called = false;
          memset(&context.found, 0, sizeof(context.found));
          track_free(&context.track);
        }
        pdjson_helper_reset(&state);
        break;

      case JSON_ARRAY:
        pdjson_helper_reset(&state);
        break;

      case JSON_ARRAY_END:
        if (context.searching.schedule) {
          context.searching.schedule = 0;
        } else if (context.searching.schedule_intervals) {
          context.searching.schedule_intervals = 0;
          context.found.schedule_intervals = 1;
        }
        pdjson_helper_reset(&state);
        break;

      case JSON_STRING: {
        size_t len = 0;
        const char *s = json_get_string(json, &len);
        if (context.searching.schedule) {
          size_t interval_index = context.track.date_intervals_size - 1;
          if (context.track.date_intervals != NULL) {
            context.track.date_intervals[interval_index].schedule_seconds_size++;

            size_t new_size = sizeof(int32_t) * (context.track.date_intervals[interval_index].schedule_seconds_size + 1);
            context.track.date_intervals[interval_index].schedule_seconds = realloc(context.track.date_intervals[interval_index].schedule_seconds, new_size);

            size_t schedule_index = context.track.date_intervals[interval_index].schedule_seconds_size - 1;
            context.track.date_intervals[interval_index].schedule_seconds[schedule_index] = 0;
            int32_t val = datetime_string_to_seconds_int(s);
            if (val >= 0) {
              context.track.date_intervals[interval_index].schedule_seconds[schedule_index] = val;
            }
          }
          break;
        } else if (string_is_equal(s, "schedule_intervals")) {
          context.searching.schedule_intervals = 1;
          break;
        } else  if (string_is_equal(s, "schedule")) {
          context.searching.schedule = 1;
          break;
        }
      }
      default:
        pdjson_helper_next(&state, t, on_parse, &context);
        break;
    }

    if (!callback_was_called && ok && track_ready(&context)) {
      bool contin = callback(&context.track, ctx);

      memset(&context.found, 0, sizeof(context.found));
      track_free(&context.track);

      callback_was_called = true;

      if (!contin) {
        break;
      }
    }

    taskYIELD();
  } while (t != JSON_DONE && t != JSON_ERROR);

  pdjson_helper_deinit(&state);

  track_free(&context.track);

  return ok;
}

static void on_parse(const char *key, const pdjson_value_t *value, pdjson_helper_value_type_t type, void *ctx) {
  ParserContext_t *context = (ParserContext_t *)ctx;

  if (string_is_equal(key, "filename") && type == PDJSON_HELPER_STRING) {
    if (context->track.filename != NULL) {
      free((void *)context->track.filename);
    }
    context->track.filename = malloc(strlen(value->string) + 1);
    strcpy((char *)context->track.filename, value->string);
    context->found.filename = 1;
  } else if (string_is_equal(key, "media_item_id") && type == PDJSON_HELPER_NUMBER) {
    context->track.id = (int64_t)value->number;
    context->found.id = 1;
  } else if (string_is_equal(key, "type") && type == PDJSON_HELPER_STRING) {
    if (string_is_equal("background", value->string)) {
      context->track.type = TRACK_BACKGROUND;
      context->found.type = 1;
    } else if (string_is_equal("advertising", value->string)) {
      context->track.type = TRACK_ADVERTISING;
      context->found.type = 1;
    }
  }

  if (context->searching.schedule_intervals) {
    if (type == PDJSON_HELPER_STRING) {
      if (string_is_equal(key, "begin")) {
        int32_t begin = datetime_date_string_to_date_int(value->string);
        //printf("  begin %s\n", value->string);
        if (begin > 0 && context->track.date_intervals != NULL && context->track.date_intervals_size > 0) {
          context->track.date_intervals[context->track.date_intervals_size - 1].begin_date = begin;
        }
      } else if (string_is_equal(key, "end")) {
        int32_t end = datetime_date_string_to_date_int(value->string);
        //printf("    end %s\n", value->string);
        if (end > 0 && context->track.date_intervals != NULL && context->track.date_intervals_size > 0) {
          context->track.date_intervals[context->track.date_intervals_size - 1].end_date = end;
        }
      }
    }
  }
}

static bool track_ready(const ParserContext_t *context) {
  return (
    context->found.id &&
    context->found.filename &&
    context->found.type &&
    (context->track.type == TRACK_BACKGROUND || (context->track.type == TRACK_ADVERTISING && context->found.schedule_intervals))
  );
}
