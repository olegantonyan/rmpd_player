#include "remote/commands/incoming.h"
#include <stdio.h>
#include "pdjson.h"
#include <errno.h>
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include "remote/commands/outgoing.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "reboot";

static void sent_callback(bool ok);

bool reboot(IncomingCommandArgument_t *arg) {
  if (arg == NULL) {
    return false;
  }

  ESP_LOGI(TAG, "rebooting...");
  AckCommandArgs_t a = {
    .sequence = arg->sequence,
    .message = "rebooting..."
  };
  outgoing_command(ACK_OK, &a, sent_callback);

  return true;
}

static void sent_callback(bool ok) {
  vTaskDelay(pdMS_TO_TICKS(10000));
  esp_restart();
}
