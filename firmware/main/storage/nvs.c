#include "storage/nvs.h"

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

bool nvs_init() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  return ret == ESP_OK;
}

bool nvs_read_string(const char *key, char *string, size_t max_length) {
  return true;
}

bool nvs_save_string(const char *key, char *string) {
  return true;
}
