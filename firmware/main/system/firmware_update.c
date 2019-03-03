#include "system/firmware_update.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_system.h"
#include "esp_log.h"
#include "util/certs/certs.h"

static const char *TAG = "firmware_update";

bool firmware_update_start(const char *url) {
  esp_http_client_config_t config = {
    .url = url,
    .cert_pem = certs(0),
  };
  esp_err_t result = esp_https_ota(&config);

  if (result == ESP_OK) {
    ESP_LOGI(TAG, "successful upgrade, restarting");
    esp_restart();
  } else {
    ESP_LOGE(TAG, "upgrade failed: %s", esp_err_to_name(result));
  }
  return result == ESP_OK;
}
