#include "clock/clock.h"
#include "clock/ds3231.h"
#include "clock/ntp.h"

#include "esp_log.h"
#include "esp_system.h"
#include <string.h>
#include <sys/time.h>
#include "config/config.h"
#include "storage/spi.h"
#include "util/files.h"
#include "util/strings.h"

static const char *TAG = "clock";

static bool clock_set_timezone(const char *tzname);

bool clock_init() {
  return ds3231_init() && ntp_init();
}

bool clock_set_timezone_from_config() {
  return clock_set_timezone(config_timezone());
}

static bool clock_set_timezone(const char *tzname) {
  ESP_LOGI(TAG, "set timezone to %s", tzname);

  FILE* f = fopen(STORAGE_SPI_MOUNTPOINT "/zones.json", "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "cannot open zones.json");
    return false;
  }

  bool ok = false;
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
  fclose(f);

  return ok;
}
