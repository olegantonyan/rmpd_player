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

static const char *TAG = "ntp";

static void thread(void * args);

bool ntp_init() {
  return xTaskCreate(thread, "ntp", 4096, NULL, 10, NULL) == pdPASS;
}

static void thread(void * args) {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();

  time_t now = 0;
  struct tm timeinfo = { 0 };

  while(timeinfo.tm_year < (2018 - 1900)) {
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    time(&now);
    localtime_r(&now, &timeinfo);
    //ESP_LOGI(TAG, "time: %s", ctime(&now));
  }
  ESP_LOGI(TAG, "time synchronized: %s", ctime(&now));
  vTaskDelete(NULL);
}
