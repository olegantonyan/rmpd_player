#include "storage/nvs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "nvs";

static nvs_handle open();
static void close(nvs_handle handle);
static bool nvs_init();

static bool initialized = false;
static SemaphoreHandle_t mutex = NULL;

bool nvs_read_string(const char *key, char *string, size_t max_length) {
  nvs_init();
  xSemaphoreTake(mutex, portMAX_DELAY);
  nvs_handle h = open();
  esp_err_t ret = nvs_get_str(h, key, string, &max_length);
  close(h);
  xSemaphoreGive(mutex);
  if (ret != ESP_OK) {
    ESP_LOGD(TAG, "failed to read %s from nvs: %s", key, esp_err_to_name(ret));
  }
  return ret == ESP_OK;
}

bool nvs_save_string(const char *key, char *string) {
  nvs_init();
  xSemaphoreTake(mutex, portMAX_DELAY);
  nvs_handle h = open();
  esp_err_t ret = nvs_set_str(h, key, string);
  close(h);
  xSemaphoreGive(mutex);
  if (ret != ESP_OK) {
    ESP_LOGW(TAG, "failed to write %s to nvs: %s", key, esp_err_to_name(ret));
  }
  return ret == ESP_OK;
}


bool nvs_read_uint8(const char *key, uint8_t *value) {
  nvs_init();
  xSemaphoreTake(mutex, portMAX_DELAY);
  nvs_handle h = open();
  esp_err_t ret = nvs_get_u8(h, key, value);
  close(h);
  xSemaphoreGive(mutex);
  if (ret != ESP_OK) {
    ESP_LOGD(TAG, "failed to read %s from nvs: %s", key, esp_err_to_name(ret));
  }
  return ret == ESP_OK;
}

bool nvs_save_uint8(const char *key, uint8_t value) {
  nvs_init();
  xSemaphoreTake(mutex, portMAX_DELAY);
  nvs_handle h = open();
  esp_err_t ret = nvs_set_u8(h, key, value);
  close(h);
  xSemaphoreGive(mutex);
  if (ret != ESP_OK) {
    ESP_LOGW(TAG, "failed to write %s to nvs: %s", key, esp_err_to_name(ret));
  }
  return ret == ESP_OK;
}

static nvs_handle open() {
  nvs_handle h;
  esp_err_t ret = nvs_open("storage", NVS_READWRITE, &h);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "failed to open nvs: %s", esp_err_to_name(ret));
  }
  ESP_ERROR_CHECK(ret);
  return h;
}

static void close(nvs_handle handle) {
  nvs_close(handle);
}

static bool nvs_init() {
  if (initialized) {
    return true;
  }
  mutex = xSemaphoreCreateMutex();
  if (mutex == NULL) {
    ESP_LOGE(TAG, "cannot create mutex");
    return false;
  }
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "failed to init nvs: %s", esp_err_to_name(ret));
  }
  ESP_ERROR_CHECK(ret);
  initialized = ret == ESP_OK;
  return initialized;
}
