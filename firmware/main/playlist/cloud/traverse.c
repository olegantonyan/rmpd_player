#include "playlist/cloud/traverse.h"
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "util/pdjson_helper.h"

static const char *TAG = "traverse";

static bool traverse_playlist_json(json_stream *json, void *ctx, bool (*callback)(const Track_t *track, void *ctx));
static void on_parse(const char *key, const pdjson_value_t *value, pdjson_helper_value_type_t type, void *ctx);
static bool is_equal(const char *one, const char *two);

typedef struct {
  bool (*callback)(const Track_t *track, void *ctx);
  void *ctx;
  Track_t track;
  bool stop_flag;
} parser_context_t;

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

  parser_context_t context = {
    .callback = callback,
    .ctx = ctx,
    .stop_flag = false
  };
  context.track.filename = NULL;
  context.track.id = 0;

  bool ok = true;

  do {
    t = json_next(json);
    switch(t) {
      case JSON_ARRAY_END:
      case JSON_OBJECT_END:
        pdjson_helper_reset(&state);
        break;

      default:
        pdjson_helper_next(&state, t, on_parse, &context);
        break;

      case JSON_ERROR:
        ESP_LOGE(TAG, "json parse error: %s line %d pos %d", json_get_error(json), json_get_lineno(json), json_get_position(json));
        ok = false;
    }

    taskYIELD();
  } while (t != JSON_DONE && t != JSON_ERROR && !context.stop_flag);

  pdjson_helper_deinit(&state);

  return ok;
}

static void on_parse(const char *key, const pdjson_value_t *value, pdjson_helper_value_type_t type, void *ctx) {
  parser_context_t *context = (parser_context_t *)ctx;

  if (is_equal(key, "filename") && type == PDJSON_HELPER_STRING) {
    context->track.filename = value->string;
  } else if (is_equal(key, "media_item_id") && type == PDJSON_HELPER_NUMBER) {
    context->track.id = (int64_t)value->number;
  }

  if (context->track.id != 0 && context->track.filename != NULL) {
    if (!context->callback(&context->track, context->ctx)) {
      context->stop_flag = true;
    }
    context->track.id = 0;
    context->track.filename = NULL;
  }
}

static bool is_equal(const char *one, const char *two) {
  if (one == NULL || two == NULL) {
    return false;
  }
  return strcmp(one, two) == 0;
}
