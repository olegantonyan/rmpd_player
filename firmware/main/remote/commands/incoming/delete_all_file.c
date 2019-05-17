#include "remote/commands/incoming.h"
#include <stdio.h>
#include <errno.h>
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include "remote/commands/outgoing.h"
#include "config/config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "delete_all_file";

bool delete_all_file(IncomingCommandArgument_t *arg) {
  if (arg == NULL) {
    return false;
  }

  ESP_LOGI(TAG, "started delete all files");

  return true;
}
