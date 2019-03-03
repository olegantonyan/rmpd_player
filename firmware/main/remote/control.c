#include "remote/control.h"
#include "remote/gate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "remote/commands/outgoing.h"
#include "remote/http.h"
#include "remote/queue.h"

static const char *TAG = "remote_ctl";

static void thread(void *_args);

bool remote_control_init() {
  if (!gate_init()) {
    ESP_LOGE(TAG, "error initializing remote gate");
    return false;
  }

  BaseType_t task_created = xTaskCreate(thread, TAG, 3000, NULL, 5, NULL);
  if (pdPASS != task_created) {
    ESP_LOGE(TAG, "cannot create thread");
    return false;
  }

  outgoing_command(POWER_ON, NULL, NULL);
  return true;
}

static void thread(void *_args) {
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(20000));
    outgoing_command(NOW_PLAYING, NULL, NULL);
  }
}
