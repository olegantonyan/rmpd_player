#include "remote/commands/incoming.h"
#include <stdio.h>
#include "pdjson.h"
#include <errno.h>
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "update_playlist";

static void test_json(json_stream *json) {
  size_t len = 0;
  enum json_type t = JSON_ERROR;
  do {
    t = json_next(json);
    switch(t) {
      case JSON_DONE:
        printf("JSON_DONE\n");
        break;
      case JSON_OBJECT:
        printf("JSON_OBJECT\n");
        break;
      case JSON_OBJECT_END:
        printf("JSON_OBJECT_END\n");
        break;
      case JSON_ARRAY:
        printf("JSON_ARRAY\n");
        break;
      case JSON_ARRAY_END:
        printf("JSON_ARRAY_END\n");
        break;
      case JSON_STRING:
        printf("JSON_STRING: %s\n", json_get_string(json, &len));
        break;
      case JSON_NUMBER:
        printf("JSON_NUMBER: %f\n", json_get_number(json));
        break;
      case JSON_TRUE:
        printf("JSON_TRUE\n");
        break;
      case JSON_FALSE:
        printf("JSON_FALSE\n");
        break;
      case JSON_NULL:
        printf("JSON_NULL\n");
        break;

      default:
      case JSON_ERROR:
        ESP_LOGE(TAG, "json parse error: %s line %d pos %d", json_get_error(json), json_get_lineno(json), json_get_position(json));
        break;
    }

  } while (t != JSON_DONE && t != JSON_ERROR);
}


bool update_playlist(IncomingCommandArgument_t *arg) {
  if (arg == NULL) {
    return false;
  }

  printf("\n\n update_playlist %d\n\n", arg->sequence);
  // TODO run in thread

  test_json(&arg->json);

  return true;
}
