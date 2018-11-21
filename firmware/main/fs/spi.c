#include "fs/spi.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "spiffs";

bool spiffs_init() {
  ESP_LOGI(TAG, "initializing SPIFFS");

  esp_vfs_spiffs_conf_t conf = {
    .base_path = FS_SPI_MOUNTPOINT,
    .partition_label = NULL,
    .max_files = 30,
    .format_if_mount_failed = false
  };

  // Use settings defined above to initialize and mount SPIFFS filesystem.
  // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK) {
      if (ret == ESP_FAIL) {
          ESP_LOGE(TAG, "failed to mount the filesystem");
      } else if (ret == ESP_ERR_NOT_FOUND) {
          ESP_LOGE(TAG, "failed to find SPIFFS partition");
      } else {
          ESP_LOGE(TAG, "failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
      }
      return false;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(NULL, &total, &used);
  if (ret != ESP_OK) {
      ESP_LOGE(TAG, "failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
  } else {
      ESP_LOGI(TAG, "partition size: total: %d, used: %d", total, used);
  }

  return true;
}
