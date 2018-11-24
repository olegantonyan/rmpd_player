#include "storage/nvs.h"

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"

//static const char *TAG = "nvs";

static nvs_handle open();
static void close(nvs_handle handle);

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
  nvs_handle h = open();
  esp_err_t ret = nvs_get_str(h, key, string, &max_length);
  close(h);
  return ret == ESP_OK;
}

bool nvs_save_string(const char *key, char *string) {
  nvs_handle h = open();
  esp_err_t ret = nvs_set_str(h, key, string);
  close(h);
  return ret == ESP_OK;
}

static nvs_handle open() {
  nvs_handle h;
  ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &h));
  return h;
}

static void close(nvs_handle handle) {
  nvs_close(handle);
}
