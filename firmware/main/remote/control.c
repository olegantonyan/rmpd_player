#include "remote/control.h"
#include "remote/gate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
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

static SemaphoreHandle_t sema = NULL;

static void thread(void *_args);

bool remote_control_init() {
  outgoing_command(POWER_ON, NULL, NULL);

  if (!gate_init()) {
    ESP_LOGE(TAG, "error initializing remote gate");
    return false;
  }

  sema = xSemaphoreCreateBinary();

  BaseType_t task_created = xTaskCreate(thread, TAG, 2400, NULL, 5, NULL);
  if (pdPASS != task_created) {
    ESP_LOGE(TAG, "cannot create thread");
    return false;
  }

  return true;
}

void remote_control_force_cycle() {
  if (sema != NULL) {
    xSemaphoreGive(sema);
  }
}

static void thread(void *_args) {
  while (true) {
    if (sema != NULL) {
      xSemaphoreTake(sema, pdMS_TO_TICKS(20000));
    } else {
      vTaskDelay(pdMS_TO_TICKS(20000));
    }

    outgoing_command(NOW_PLAYING, NULL, NULL);
    taskYIELD();
  }
}
