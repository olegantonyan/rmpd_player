#include "clock/ntp.h"

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/apps/sntp.h"

bool ntp_init() {

  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();

  time_t now = 0;
  struct tm timeinfo = { 0 };

  while(timeinfo.tm_year < (2016 - 1900)) {
    ESP_LOGI("NTP", "Waiting for system time to be set...");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    time(&now);

    time_t current_time = time(NULL);
    printf("posix time: %s", ctime(&current_time));

    localtime_r(&now, &timeinfo);
  }

  return true;
}
