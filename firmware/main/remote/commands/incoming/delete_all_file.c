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
#include "storage/sd.h"
#include "remote/commands/outgoing.h"
#include "playlist/offline/scheduler.h"
#include "playlist/cloud/scheduler.h"
#include "util/files.h"
#include "playlist/cloud/cleanup_files.h"
#include "playlist/cloud/downloader.h"

static const char *TAG = "delete_all_file";

bool delete_all_file(IncomingCommandArgument_t *arg) {
  if (arg == NULL) {
    return false;
  }

  ESP_LOGI(TAG, "started delete all files");

  offline_scheduler_deinit();
  cloud_downloader_stop();
  cloud_scheduler_deinit();
  cloud_cleanup_files_stop();

  bool ok = remove_directory(STORAGE_SD_MOUNTPOINT);

  AckCommandArgs_t a = {
    .sequence = arg->sequence,
    .message = "delete all files complete"
  };
  if (ok) {
    outgoing_command(ACK_OK, &a, NULL);
    ESP_LOGI(TAG, "done delete all files");
  } else {
    outgoing_command(ACK_FAIL, &a, NULL);
    ESP_LOGI(TAG, "error delete all files");
  }

  return true;
}
