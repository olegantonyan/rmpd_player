#include "util/pdjson_helper.h"
#include <stdlib.h>
#include <string.h>

static void free_key(pdjson_helper_state_t *state);

bool pdjson_helper_init(pdjson_helper_state_t *out_state, json_stream *json) {
  if (out_state == NULL) {
    return false;
  }
  out_state->json = json;
  out_state->current_key = NULL;
  out_state->max_key_length = 256;
  return true;
}

bool pdjson_helper_deinit(pdjson_helper_state_t *state) {
  if (state == NULL) {
    return false;
  }
  if (state->current_key != NULL) {
    free_key(state);
  }
  return true;
}

bool pdjson_helper_reset(pdjson_helper_state_t *state) {
  if (state == NULL) {
    return false;
  }
  if (state->current_key != NULL) {
    free_key(state);
  }
  return true;
}

void pdjson_helper_next(pdjson_helper_state_t *state, enum json_type found_type, pdjson_helper_callback_t callback, void *ctx) {
  pdjson_value_t value;

  switch(found_type) {
    case JSON_NUMBER:
      if (state->current_key == NULL) {
        break;
      }
      value.number = json_get_number(state->json);
      callback(state->current_key, &value, PDJSON_HELPER_NUMBER, ctx);
      free_key(state);
      break;

    case JSON_STRING: {
      size_t len = 0;
      const char *s = json_get_string(state->json, &len);
      if (s == NULL) {
        break;
      }
      if (state->current_key == NULL) { // this is a key
        if (len < state->max_key_length) {
          state->current_key = malloc(len + 1);
          strncpy((char *)state->current_key, s, len);
        }
      } else { // this is a string value
        value.string = s;
        callback(state->current_key, &value, PDJSON_HELPER_STRING, ctx);
        free_key(state);
      }
      break;
    }

    case JSON_TRUE:
    case JSON_FALSE:
      if (state->current_key == NULL) {
        break;
      }
      value.boolean = found_type == JSON_TRUE;
      callback(state->current_key, &value, PDJSON_HELPER_BOOL, ctx);
      free_key(state);
      break;

    case JSON_NULL:
      if (state->current_key == NULL) {
        break;
      }
      callback(state->current_key, NULL, PDJSON_HELPER_NULL, ctx);
      free_key(state);
      break;
    default:
      break;
  }
}

static void free_key(pdjson_helper_state_t *state) {
  if (state->current_key == NULL) {
    return;
  }
  free((void *)state->current_key);
  state->current_key = NULL;
}
