#include "remote/file_download.h"
#include "util/certs/certs.h"
#include "esp_http_client.h"
#include "util/url.h"
#include "config/config.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "system/sysinfo.h"

static const char *TAG = "file_download";

static esp_err_t http_event_handle(esp_http_client_event_t *evt);

/*
need at least 6kb stack
vTaskDelay(pdMS_TO_TICKS(30000));
//wifi_wait_connected(100000);

remove("/sdcard/download.aaa");
int res = file_download_start("http://192.168.1.3:3000/rails/active_storage/blobs/eyJfcmFpbHMiOnsibWVzc2FnZSI6IkJBaHBWdz09IiwiZXhwIjpudWxsLCJwdXIiOiJibG9iX2lkIn19--875f1583cf8ad0aa2d9cdfb39bc25147ec484b3b/09%20The%20Immaculate%20Deception.mp3", "/sdcard/download.aa");
ESP_LOGI(TAG, "STATUS %d", res);
*/
int file_download_start(const char *url, const char *download_path) {
  esp_http_client_config_t config = {
    .event_handler = http_event_handle,
    .url = url,
    .timeout_ms = 6000,
    .method = HTTP_METHOD_GET,
    .cert_pem = certs(),
    .max_redirection_count = 4,
    .disable_auto_redirect = false,
    .user_data = (void *)download_path,
    .buffer_size = 2048
  };
  ESP_LOGI(TAG, "request to %s", config.url);
  esp_http_client_handle_t client = esp_http_client_init(&config);

  char ua[128] = { 0 };
  sysinfo_useragent(ua, sizeof(ua));
  esp_http_client_set_header(client, "User-Agent", ua);

  esp_err_t err = esp_http_client_perform(client);
  int status = -1;
  if (err == ESP_OK) {
    status = esp_http_client_get_status_code(client);
  } else {
    ESP_LOGW(TAG, "http status get request failed: %s", esp_err_to_name(err));
  }
  esp_http_client_cleanup(client);
  return status;
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
      const char *filepath = (const char *)evt->user_data;
      FILE *f = fopen(filepath, "ab+");
      fwrite(evt->data, evt->data_len, 1, f);
      fflush(f);
      fclose(f);
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
