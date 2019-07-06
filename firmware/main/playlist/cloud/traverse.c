#include "playlist/cloud/traverse.h"
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "util/pdjson_helper.h"
#include "util/strings.h"

static const char *TAG = "traverse";

typedef struct {
  Track_t track;

  struct {
    uint8_t filename:1;
    uint8_t id:1;
    uint8_t type:1;
    uint8_t schedule_intervals:1;
  } found;

  /*struct {
    uint8_t schedule_intervals:1;
    uint8_t date_interval:1;
    uint8_t schedule:1;
  } searching;*/
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
  do {
    t = json_next(json);
    switch(t) {
      case JSON_ARRAY_END:
      case JSON_OBJECT:
      case JSON_OBJECT_END:
        pdjson_helper_reset(&state);
        break;

      default:
        pdjson_helper_next(&state, t, on_parse, &context);
        break;

      case JSON_ERROR:
        ESP_LOGE(TAG, "json parse error: %s line %d pos %d", json_get_error(json), json_get_lineno(json), json_get_position(json));
        ok = false;
        break;
    }

    if (ok && track_ready(&context)) {
      if (!callback(&context.track, ctx)) {
        break;
      }
      memset(&context.found, 0, sizeof(context.found));
      free((void *)context.track.filename);
      context.track.filename = NULL;
    }

    taskYIELD();
  } while (t != JSON_DONE && t != JSON_ERROR);

  pdjson_helper_deinit(&state);

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
  } /*else if (string_is_equal(key, "schedule_intervals")) {
    context->searching.schedule_intervals = 1;
  } else if (string_is_equal(key, "date_interval")) {
    context->searching.date_interval = 1;
  } else if (string_is_equal(key, "schedule")) {
    context->searching.schedule = 1;
  }*/
}

static bool track_ready(const ParserContext_t *context) {
  return (
    context->found.id &&
    context->found.filename &&
    context->found.type &&
    (context->track.type == TRACK_BACKGROUND || (context->track.type == TRACK_ADVERTISING/* && context.found.schedule_intervals*/))
  );
}
