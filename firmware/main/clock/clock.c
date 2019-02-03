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
#include "cJSON.h"

static const char *TAG = "clock";

extern const char zones_json_start[] asm("_binary_zones_json_start");
extern const char zones_json_end[]   asm("_binary_zones_json_end");

static bool clock_set_timezone(const char *tzname);

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

  cJSON *json = cJSON_Parse(clock_zones());
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
    } else {
      ESP_LOGE(TAG, "json parse error");
    }
    return false;
  }
  bool ok = false;
  const cJSON *zone = cJSON_GetObjectItemCaseSensitive(json, tzname);
  if (cJSON_IsString(zone) && (zone->valuestring != NULL)) {
    ESP_LOGI(TAG, "timezone value: '%s'", zone->valuestring);
    setenv("TZ", zone->valuestring, 1);
    tzset();
    ok = true;
  } else {
    ESP_LOGE(TAG, "cannot get timezone value from zones.json");
  }

  cJSON_Delete(json);
  return ok;

  /*bool ok = false;
  char line[255];
  while(fgets(line, sizeof(line), f) != NULL) {
    if (line[0] == '{' || line[0] == '}') {
      continue;
    }
    string_chomp(line);
    char *tzname_quoted = malloc(strlen(tzname) + 13);
    snprintf(tzname_quoted, strlen(tzname) + 2, "\"%s\"", tzname);
    if (strncmp(tzname_quoted, line, strlen(tzname_quoted)) == 0) {
      line[strlen(line) - 2] = '\0'; // remove ",
      char *tzvalue = line + strlen(tzname_quoted) + 3;
      ESP_LOGI(TAG, "timezone value: '%s'", tzvalue);
      setenv("TZ", tzvalue, 1);
      tzset();
      free(tzname_quoted);
      ok = true;
      break;
    }
    free(tzname_quoted);
  }
  */
}
