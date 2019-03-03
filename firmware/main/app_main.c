#include "wifi/wifi.h"
#include "web/web.h"
#include "storage/sd.h"
#include "playlist/offline/scheduler.h"
#include "clock/clock.h"
#include "remote/control.h"
#include "util/tempfile.h"
#include "system/firmware_update.h"

/*
#include "pdjson.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
void test_json() {
  FILE *f = fopen("/sdcard/test.json", "r");

  json_stream json;
  json_open_stream(&json, f);

  size_t len = 0;

  enum json_type t = JSON_ERROR;
  do {
    t = json_next(&json);
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
        printf("JSON_STRING: %s\n", json_get_string(&json, &len));
        break;
      case JSON_NUMBER:
        printf("JSON_NUMBER: %f\n", json_get_number(&json));
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
        printf("JSON_ERROR: %s\n", json_get_error(&json));
        break;
    }

  } while (t != JSON_DONE && t != JSON_ERROR);

  json_close(&json);
  fclose(f);
}*/

void app_main() {
  // mind order of initialization
  wifi_init();
  clock_init();
  sd_init();
  tempfile_init();
  web_init();
  scheduler_init();
  remote_control_init();
  firmware_update_confirm(true); // TODO check if everything is ok
}
