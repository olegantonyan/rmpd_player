#include "remote/gate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi/wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "remote_gate";

static void thread(void *_args);

bool gate_init() {
  BaseType_t task_created = xTaskCreate(thread, "remote_gate", 8192, NULL, 5, NULL);
  if (pdPASS != task_created) {
    ESP_LOGE(TAG, "cannot create thread");
    return false;
  }
  return true;
}

static void thread(void *_args) {
  wifi_wait_connected(portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(541));

  while (true) {

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
