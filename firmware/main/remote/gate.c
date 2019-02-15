#include "remote/gate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi/wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include "util/certs/certs.h"
#include "config/config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_http_client.h"
#include "util/url.h"

static const char *TAG = "remote_gate";

#define MAX_RECEIVE_DATA_LENGTH 1500

typedef struct {
  uint32_t sequence;
  char data[MAX_RECEIVE_DATA_LENGTH];
  size_t length;
} response_t;

static void thread(void *_args);
static int http_post_cmd(const char *send_data, size_t send_data_len, uint32_t send_seq, response_t *response);

bool gate_init() {
  BaseType_t task_created = xTaskCreate(thread, "remote_gate", 6000 + MAX_RECEIVE_DATA_LENGTH, NULL, 5, NULL);
  if (pdPASS != task_created) {
    ESP_LOGE(TAG, "cannot create thread");
    return false;
  }
  return true;
}

#include "remote/commands/outgoing/now_playing.h"
static void thread(void *_args) {
  wifi_wait_connected(portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(541)); // for some reasons, probably

  while (true) {

    response_t recv;
    memset(&recv, 0, sizeof(response_t));
    char *json = now_playing(NULL);
    if (json != NULL) {
      int status = http_post_cmd(json, strlen(json), 0, &recv);
      if (status >= 200 && status < 300) {
        printf("data: %s\n", recv.data);
        printf("stat: %d\n", status);
        printf("seq:  %d\n", recv.sequence);
        printf("len:  %d\n", recv.length);
      }
      free(json);
    };

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

esp_err_t http_event_handle(esp_http_client_event_t *evt) {
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
      //printf("%s : %s\n", evt->header_key, evt->header_value);
      if (strcmp(evt->header_key, "X-Sequence-Number") == 0) {
        response_t *r = (response_t *)evt->user_data;
        r->sequence = atoi(evt->header_value);
      }
      break;
    case HTTP_EVENT_ON_DATA:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      //if (!esp_http_client_is_chunked_response(evt->client)) {
      //    printf("%.*s\n", evt->data_len, (char*)evt->data);
      //}
      response_t *r = (response_t *)evt->user_data;
      if (r->length + evt->data_len >= MAX_RECEIVE_DATA_LENGTH) {
        ESP_LOGE(TAG, "no space left in receive buffer for incomming data");
      } else {
        memcpy(r->data + r->length, evt->data, evt->data_len);
        r->length += evt->data_len;
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

static int http_post_cmd(const char *send_data, size_t send_data_len, uint32_t send_seq, response_t *response) {
  url_t *url_parsed = malloc(sizeof(url_t));
  char server_addr[96] = { 0 };
  strncpy(server_addr, config_server_url(), sizeof(server_addr) - 1);
  if (!url_parse(server_addr, url_parsed)) {
    free(url_parsed);
    ESP_LOGE(TAG, "server url parse error");
    return -1;
  }
  char url[300] = { 0 };
  snprintf(url, sizeof(url), "%s://%s:%s@%s:%s/deviceapi/status", url_parsed->protocol, config_deviceid(), config_server_password(), url_parsed->host, url_parsed->port);
  free(url_parsed);

  esp_http_client_config_t config = {
    .event_handler = http_event_handle,
    .url = url,
    .timeout_ms = 4000,
    .method = HTTP_METHOD_POST,
    .cert_pem = certs[0],
    .auth_type = HTTP_AUTH_TYPE_BASIC,
    .max_redirection_count = 3,
    .disable_auto_redirect = false,
    .user_data = response
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_post_field(client, send_data, send_data_len);
  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_http_client_set_header(client, "Accept", "application/json");
  esp_http_client_set_header(client, "User-Agent", "TODO: set me ESP32");
  char seqs[20] = { 0 };
  itoa(send_seq, seqs, 10);
  esp_http_client_set_header(client, "X-Sequence-Number", seqs);

  esp_err_t err = esp_http_client_perform(client);
  int status = -1;
  if (err == ESP_OK) {
    status = esp_http_client_get_status_code(client);
  } else {
    ESP_LOGW(TAG, "http status post request failed: %s", esp_err_to_name(err));
  }
  esp_http_client_cleanup(client);
  return status;
}
