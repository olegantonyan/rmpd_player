#include "remote/commands/incoming.h"
#include <stdio.h>
#include <errno.h>
#include "esp_system.h"
#include <dirent.h>
#include "esp_log.h"
#include <string.h>
#include "remote/commands/outgoing.h"
#include "config/config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "storage/sd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "remote/commands/outgoing.h"

static const char *TAG = "delete_all_file";

static bool remove_directory(const char *path);

bool delete_all_file(IncomingCommandArgument_t *arg) {
  if (arg == NULL) {
    return false;
  }

  ESP_LOGI(TAG, "started delete all files");

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

static bool remove_directory(const char *path) {
  DIR *dp = opendir(path);
  if (dp == NULL) {
    ESP_LOGE(TAG, "error opening directory %s: %s", path, strerror(errno));
    return false;
  }

  while(true) {
    struct dirent *ep = readdir(dp);
    if (!ep) {
      break;
    }
    size_t newpath_len = strlen(path) + strlen(ep->d_name) + 4;
    char *newpath = malloc(newpath_len);
    snprintf(newpath, newpath_len, "%s/%s", path, ep->d_name);
    if (ep->d_type == DT_DIR) {
      if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0) {
        remove_directory(newpath);
      }
    } else {
      remove(newpath);
    }
    free(newpath);
    taskYIELD();
  }
  closedir(dp);

  return true;
}
