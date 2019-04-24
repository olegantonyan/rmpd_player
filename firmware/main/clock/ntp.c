#include "clock/ntp.h"

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "esp_sntp.h"
#include "clock/ds3231.h"
#include "clock/clock.h"
#include "wifi/wifi.h"

static const char *TAG = "ntp";

static void thread(void * args);

bool ntp_init() {
  return xTaskCreate(thread, TAG, 3000, NULL, 5, NULL) == pdPASS;
}

static void thread(void * args) {
  wifi_wait_connected(portMAX_DELAY);

  ESP_LOGI(TAG, "initializing ntp");

  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "0.ru.pool.ntp.org");
  sntp_setservername(1, "time.google.com");
  sntp_setservername(2, "time-a-g.nist.gov ");
  sntp_setservername(3, "time-b-g.nist.gov");
  sntp_setservername(4, "time-c-g.nist.gov");
  sntp_setservername(5, "time-d-g.nist.gov");
  sntp_setservername(6, "time-d-g.nist.gov");
  sntp_setservername(7, "2.opensuse.pool.ntp.org");
  sntp_setservername(8, "utcnist.colorado.edu ");
  sntp_setservername(9, "time.windows.com");
  //sntp_set_time_sync_notification_cb(time_sync_notification_cb);
  sntp_init();

  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
    vTaskDelay(pdMS_TO_TICKS(1146));
  }

  time_t now = time(NULL);
  struct tm timeinfo = { 0 };
  localtime_r(&now, &timeinfo);
  if (!ds3231_set_time(&timeinfo)) {
    ESP_LOGI(TAG, "error setting RTC time");
  }
  ESP_LOGI(TAG, "time synchronized: %s", ctime(&now));

  sntp_stop();
  vTaskDelete(NULL);
}
