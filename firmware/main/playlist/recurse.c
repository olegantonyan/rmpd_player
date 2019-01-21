#include "playlist/recurse.h"

#include <dirent.h>
#include "util/strings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>

static const char *TAG = "recurse";

uint32_t recurse_dir(const char *path, uint8_t depth, uint16_t *index, void (*callback)(const char *fullname, uint16_t index), bool (*match_func)(const char *fname)) {
  if (depth > RECURSE_MAX_DEPTH) {
    ESP_LOGW(TAG, "directory recurse max depth reached");
    return 0;
  }

  DIR *dp = opendir(path);
  if (dp == NULL) {
    ESP_LOGE(TAG, "error opening directory %s: %s", path, strerror(errno));
    return 0;
  }

  uint32_t files_in_dir = 0;
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
        files_in_dir += recurse_dir(newpath, depth + 1, index, callback, match_func);
      }
    } else if (match_func != NULL && match_func(newpath)) {
      if (callback != NULL && index != NULL) {
        callback(newpath, *index);
      }
      files_in_dir++;
      if (index != NULL) {
        (*index)++;
      }
    }
    free(newpath);
    taskYIELD();
  }
  closedir(dp);

  return files_in_dir;
}
