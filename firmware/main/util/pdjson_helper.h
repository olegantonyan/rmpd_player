#ifndef __PDJSON_HELPER_H
#define __PDJSON_HELPER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "pdjson.h"

typedef struct {
  json_stream *json;
  const char *current_key;
  size_t max_key_length;
} pdjson_helper_state_t;

typedef enum {
  PDJSON_HELPER_NULL,
  PDJSON_HELPER_STRING,
  PDJSON_HELPER_BOOL,
  PDJSON_HELPER_NUMBER
} pdjson_helper_value_type_t;

typedef union {
  const char *string;
  double number;
  bool boolean;
} pdjson_value_t;

typedef void (*pdjson_helper_callback_t)(const char *key, const pdjson_value_t *value, pdjson_helper_value_type_t type, void *ctx);

bool pdjson_helper_init(pdjson_helper_state_t *out_state, json_stream *json);
bool pdjson_helper_deinit(pdjson_helper_state_t *state);
void pdjson_helper_next(pdjson_helper_state_t *state, enum json_type found_type, pdjson_helper_callback_t callback, void *ctx);
bool pdjson_helper_reset(pdjson_helper_state_t *state);

#endif
