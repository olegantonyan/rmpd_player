#include "system/firmware_update.h"
#include "esp_http_client.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_log.h"
#include "util/certs/certs.h"
#include "remote/commands/outgoing.h"
#include "storage/nvs.h"
#include "config/config.h"
#include "system/sysinfo.h"
#include "playlist/cloud/cleanup_files.h"
#include "playlist/cloud/downloader.h"

static const char *TAG = "firmware_update";

typedef struct {
  bool ok;
  esp_ota_handle_t handle;
} OTA_Context_t;

static void remove_nvs_sequence_callback(bool ok);
static esp_err_t http_event_handle(esp_http_client_event_t *evt);
static bool ota_start(const char *url);

bool firmware_update_start(const char *url, uint32_t sequence) {
  cloud_downloader_stop();
  cloud_cleanup_files_stop();

  bool result = ota_start(url);

  if (result) {
    ESP_LOGI(TAG, "successful upgrade, restarting, sequence %u", sequence);
    nvs_save_uint32("fw_upgdare_seq", sequence);
    config_save_disable_tls_certs_verification(false);
    esp_restart();
  } else {
    ESP_LOGE(TAG, "upgrade failed");
  }
  return result == ESP_OK;
}

void firmware_update_confirm(bool success) {
  char *message = NULL;
  if (success) {
    esp_ota_mark_app_valid_cancel_rollback();
    const esp_app_desc_t *d = esp_ota_get_app_description();
    message = (char *)d->version;
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
    outgoing_command(ACK_OK, &a, remove_nvs_sequence_callback);
  } else {
    outgoing_command(ACK_FAIL, &a, remove_nvs_sequence_callback);
  }
}

static void remove_nvs_sequence_callback(bool ok) {
  if (!ok) {
    return;
  }
  ESP_LOGI(TAG, "upgrade acked");
  nvs_save_uint32("fw_upgdare_seq", 0);
}

static bool ota_start(const char *url) {
  bool ok = false;

  esp_err_t err = ESP_OK;

  esp_http_client_handle_t client = NULL;

  do {
    ESP_LOGI(TAG, "starting ota");

    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == NULL) {
      ESP_LOGE(TAG, "passive ota partition not found");
      break;
    }
    ESP_LOGI(TAG, "writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);

    OTA_Context_t ctx;
    ctx.ok = true;
    ctx.handle = 0;

    if (esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ctx.handle) != ESP_OK) {
      ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
      break;
    }
    ESP_LOGI(TAG, "esp_ota_begin succeeded");
    ESP_LOGI(TAG, "please wait...");

    esp_http_client_config_t config = {
      .event_handler = http_event_handle,
      .url = url,
      .timeout_ms = 30000,
      .method = HTTP_METHOD_GET,
      .cert_pem = certs(),
      .max_redirection_count = 4,
      .disable_auto_redirect = false,
      .user_data = (void *)&ctx,
      .buffer_size = 2048,
      .buffer_size_tx = 2048
    };
    ESP_LOGI(TAG, "request to %s", config.url);
    client = esp_http_client_init(&config);

    char ua[128] = { 0 };
    sysinfo_useragent(ua, sizeof(ua));
    esp_http_client_set_header(client, "User-Agent", ua);

    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
      int status = esp_http_client_get_status_code(client);
      ESP_LOGI(TAG, "request status code %d", status);
      if (200 <= status && status < 300 && ctx.ok) {
        err = esp_ota_end(ctx.handle);
        if (err != ESP_OK) {
          ESP_LOGE(TAG, "esp_ota_end failed: %s", esp_err_to_name(err));
          break;
        }

        ESP_LOGI(TAG, "setting boot partition");
        err = esp_ota_set_boot_partition(update_partition);
        if (err != ESP_OK) {
          ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
          break;
        }
        ESP_LOGI(TAG, "esp_ota_set_boot_partition succeeded");
        ok = true;
      }
    } else {
      ESP_LOGE(TAG, "http get request failed: %s", esp_err_to_name(err));
      break;
    }

  } while(false);

  esp_http_client_cleanup(client);

  return ok;
}

static esp_err_t http_event_handle(esp_http_client_event_t *evt) {
  switch(evt->event_id) {
    case HTTP_EVENT_ERROR:
      ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER %s = %s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

      int status = esp_http_client_get_status_code(evt->client);
      if (300 <= status && status < 599) {
        break;
      }

      OTA_Context_t *ctx = (OTA_Context_t *)evt->user_data;
      esp_err_t ota_write_err = esp_ota_write(ctx->handle, evt->data, evt->data_len);
      if (ota_write_err != ESP_OK) {
        ESP_LOGE(TAG, "OTA write error: %s", esp_err_to_name(ota_write_err));
        ctx->ok = false;
      }
      break;
    case HTTP_EVENT_ON_FINISH:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
      break;
  }
  return ESP_OK;
}
