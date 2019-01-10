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
#include "lwip/apps/sntp.h"
#include "clock/ds3231.h"
#include "clock/clock.h"

static const char *TAG = "ntp";

static void thread(void * args);

static QueueHandle_t queue = NULL;

// https://github.com/espressif/esp-idf/pull/1668/files
void sntp_setsystemtime(u32_t t) {
  xQueueSend(queue, &t, portMAX_DELAY);
}
void sntp_setsystemtime_us(u32_t t, u32_t us) {
  (void)us;
  sntp_setsystemtime(t);
}

bool ntp_init() {
  queue = xQueueCreate(1, sizeof(uint32_t));
  if (queue == NULL) {
    ESP_LOGE(TAG, "cannot create queue");
    return false;
  }

  return xTaskCreate(thread, "ntp", 4096, NULL, 10, NULL) == pdPASS;
}

static void thread(void * args) {
  vTaskDelay(pdMS_TO_TICKS(5000)); // hack

  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_setservername(1, "time.google.com");
  sntp_setservername(2, "time-a-g.nist.gov ");
  sntp_setservername(3, "time-b-g.nist.gov");
  sntp_setservername(4, "time-c-g.nist.gov");
  sntp_setservername(5, "time-d-g.nist.gov");
  sntp_setservername(6, "time-d-g.nist.gov");
  sntp_setservername(7, "2.opensuse.pool.ntp.org");
  sntp_setservername(8, "utcnist.colorado.edu ");
  sntp_setservername(9, "time.windows.com");
  sntp_init();

  uint32_t result;
  if(xQueueReceive(queue, &result, portMAX_DELAY)) {
    struct timeval tv = { .tv_sec = result, .tv_usec = 0 };
    settimeofday(&tv, NULL);
    clock_set_timezone_from_config();

    time_t now = time(NULL);
    struct tm timeinfo = { 0 };
    localtime_r(&now, &timeinfo);
    if (!ds3231_set_time(&timeinfo)) {
      ESP_LOGI(TAG, "error setting RTC time");
    }
    ESP_LOGI(TAG, "time synchronized: %s", ctime(&now));
  }

  sntp_stop();
  vTaskDelete(NULL);
}
