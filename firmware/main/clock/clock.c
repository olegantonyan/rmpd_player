#include "clock/clock.h"
#include "clock/ds3231.h"
#include "clock/ntp.h"
#include "esp_log.h"
#include "esp_system.h"
#include <string.h>
#include <sys/time.h>
#include "config/config.h"
#include "util/files.h"
#include "util/strings.h"
#include "pdjson.h"

static const char *TAG = "clock";

extern const char zones_json_start[] asm("_binary_zones_json_start");
extern const char zones_json_end[]   asm("_binary_zones_json_end");

static bool clock_set_timezone(const char *tzname);
static const char *parse_json(json_stream *json, const char *key);

bool clock_init() {
  return ds3231_init() && ntp_init();
}

bool clock_set_timezone_from_config() {
  return clock_set_timezone(config_timezone());
}

const char *clock_zones() {
  return zones_json_start;
}

static bool clock_set_timezone(const char *tzname) {
  ESP_LOGI(TAG, "set timezone to %s", tzname);

  bool ok = false;
  json_stream json;
  json_open_string(&json, clock_zones());
  const char *tzvalue = parse_json(&json, tzname);

  if (tzvalue != NULL) {
    ESP_LOGI(TAG, "timezone value: '%s'", tzvalue);
    setenv("TZ", tzvalue, 1);
    tzset();
    ok = true;
  }
  json_close(&json);
  return ok;
}

static const char *parse_json(json_stream *json, const char *key) {
  size_t len = 0;
  bool key_found = false;
  enum json_type t = JSON_ERROR;
  do {
    t = json_next(json);
    switch(t) {
      case JSON_STRING: {
        if (json_get_depth(json) == 1) {
          const char *s = json_get_string(json, &len);
          if (key_found) {
            return s;
          } else if (strcmp(s, key) == 0) {
            key_found = true;
          }
        }
        break;
      }

      default:
        break;
      case JSON_ERROR:
        ESP_LOGE(TAG, "json parse error: %s line %d pos %d", json_get_error(json), json_get_lineno(json), json_get_position(json));
        break;
    }

  } while (t != JSON_DONE && t != JSON_ERROR);
  return NULL;
}
