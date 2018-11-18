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

static const char *TAG = "clock/ntp";

static void thread(void * args);

bool ntp_init() {
  TaskHandle_t handle = NULL;
  return xTaskCreate(thread, "ntp", 1024, NULL, tskIDLE_PRIORITY, &handle) == pdPASS;
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
  }
  ESP_LOGI(TAG, "time synchronized: %s", ctime(&now));
  vTaskDelete(NULL);
}
