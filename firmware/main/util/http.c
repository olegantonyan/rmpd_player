#include "util/http.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <errno.h>

static const char *TAG = "http";

static esp_err_t http_event_handle(esp_http_client_event_t *evt);

int http_get(const char *uri, uint8_t *data, size_t data_max_length, size_t *content_length) {
  esp_http_client_config_t config = {
   .url = uri,
   .event_handler = http_event_handle,
   .timeout_ms = 5000,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_err_t err = esp_http_client_perform(client);

  int status = 0;
  if (err == ESP_OK) {
    int bytes = esp_http_client_read(client, (char *)data, data_max_length);
    *content_length = esp_http_client_get_content_length(client);
    status = esp_http_client_get_status_code(client);
    ESP_LOGD(TAG, "status:%d content_length:%d bytes_read:%d", status, *content_length, bytes);
  } else {
    ESP_LOGW(TAG, "http get request failed: %s", esp_err_to_name(err));
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
      ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER");
      //printf("%.*s", evt->data_len, (char*)evt->data);
      break;
    case HTTP_EVENT_ON_DATA:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      /*if (!esp_http_client_is_chunked_response(evt->client)) {
        //printf("%.*s", evt->data_len, (char*)evt->data);
      } else {
        ESP_LOGW(TAG, "chunked response is not supported");
      }*/
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
