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
#include "remote/http.h"
#include "remote/queue.h"
#include "remote/commands/outgoing.h"

static const char *TAG = "remote_ctl";

void remote_control_start() {
  if (!gate_init()) {
    ESP_LOGE(TAG, "error initializing remote gate");
    return;
  }

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(20000));
    outgoing_command(NOW_PLAYING, NULL);
  }
