#include "remote/gate.h"
#include "util/http.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi/wifi.h"
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "remote_gate";

static void thread(void *_args);

bool gate_init() {
  /*BaseType_t task_created = xTaskCreate(thread, "remote_gate", 8192, NULL, 5, NULL);
  if (pdPASS != task_created) {
    ESP_LOGE(TAG, "cannot create thread");
    return false;
  }*/
  return true;
}

static void thread(void *_args) {
  wifi_wait_connected(portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(541));

  while (true) {
    static uint8_t buf[8192] = { 0 };
    size_t len = 0;

    int s = http_get("https://server.slon-ds.ru/", buf, sizeof(buf), &len);
    printf("status:%d bytes:%d\n%s\n", s, len, buf);

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
