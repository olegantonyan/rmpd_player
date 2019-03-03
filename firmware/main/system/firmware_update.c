#include "system/firmware_update.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_log.h"
#include "util/certs/certs.h"
#include "remote/commands/outgoing.h"
#include "storage/nvs.h"

static const char *TAG = "firmware_update";

bool firmware_update_start(const char *url, uint32_t sequence) {
  esp_http_client_config_t config = {
    .url = url,
    .cert_pem = certs(0),
  };
  esp_err_t result = esp_https_ota(&config);

  if (result == ESP_OK) {
    ESP_LOGI(TAG, "successful upgrade, restarting, sequence %u", sequence);
    nvs_save_uint32("fw_upgdare_seq", sequence);
    esp_restart();
  } else {
    ESP_LOGE(TAG, "upgrade failed: %s", esp_err_to_name(result));
  }
  return result == ESP_OK;
}

void firmware_update_confirm(bool success) {
  char *message = "";
  if (success) {
    esp_ota_mark_app_valid_cancel_rollback();
    message = "firmware update successful";
  } else {
    esp_ota_mark_app_invalid_rollback_and_reboot();
    message = "firmware update failed, rolling back";
  }

  uint32_t sequence = 0;
  bool read_ok = nvs_read_uint32("fw_upgdare_seq", &sequence);
  if (!read_ok || sequence == 0) {
    return;
  }
  ESP_LOGI(TAG, "ack sucessful update, sequence %u", sequence);

  AckCommandArgs_t a = {
    .sequence = sequence,
    .message = message
  };
  if (success) {
    outgoing_command(ACK_OK, &a);
  } else {
    outgoing_command(ACK_FAIL, &a);
  }
}
