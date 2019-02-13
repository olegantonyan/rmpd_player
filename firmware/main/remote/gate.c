#include "remote/gate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi/wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include "util/certs/certs.h"
#include "config/config.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "esp_http_client.h"
#include "util/url.h"

static const char *TAG = "remote_gate";

static void thread(void *_args);

static bool now_playing_json(char *buffer, size_t length);
static int http_post_cmd(const char *send_data, size_t send_data_len, char *receive_data, size_t receive_data_len, uint32_t seq);

bool gate_init() {
  BaseType_t task_created = xTaskCreate(thread, "remote_gate", 9000, NULL, 5, NULL);
  if (pdPASS != task_created) {
    ESP_LOGE(TAG, "cannot create thread");
    return false;
  }
  return true;
}

static void thread(void *_args) {
  wifi_wait_connected(portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(541));

  while (true) {

    char *json = malloc(1024);
    char *recv = malloc(1024);
    if (now_playing_json(json, 1024)) {
      http_post_cmd(json, strlen(json), recv, 1024, 0);
    };
    free(json);
    free(recv);

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

static bool now_playing_json(char *buffer, size_t length) {
  if (NULL == buffer) {
    return false;
  }

  cJSON *root = cJSON_CreateObject();

  time_t now = time(NULL);
  struct tm timeinfo = { 0 };
  localtime_r(&now, &timeinfo);
  char time_buf[40] = { 0 };
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S%z", &timeinfo);
  cJSON_AddItemToObject(root, "localtime", cJSON_CreateString(time_buf));

  cJSON_AddItemToObject(root, "command", cJSON_CreateString("now_playing"));

  cJSON_AddItemToObject(root, "message", cJSON_CreateString("playing something..."));

  cJSON_AddItemToObject(root, "free_space", cJSON_CreateNumber(100500));

  bool ok = false;
  if(cJSON_PrintPreallocated(root, buffer, length, 0)) {
    ok = true;
  }
  cJSON_Delete(root);
  return ok;
}

static int http_post_cmd(const char *send_data, size_t send_data_len, char *receive_data, size_t receive_data_len, uint32_t seq) {
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
    .url = url,
    .timeout_ms = 4000,
    .method = HTTP_METHOD_POST,
    .cert_pem = certs[0],
    .auth_type = HTTP_AUTH_TYPE_BASIC,
    .max_redirection_count = 3,
    .disable_auto_redirect = false
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_post_field(client, send_data, send_data_len);
  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_http_client_set_header(client, "Accept", "application/json");
  esp_http_client_set_header(client, "User-Agent", "TODO: set me ESP32");
  char seqs[20] = { 0 };
  itoa(seq, seqs, 10);
  esp_http_client_set_header(client, "X-Sequence-Number", seqs);

  esp_err_t err = esp_http_client_perform(client);

  int status = 0;
  do {
    if (err != ESP_OK) {
      ESP_LOGW(TAG, "http status post request failed: %s", esp_err_to_name(err));
      break;
    }

    status = esp_http_client_get_status_code(client);
    if (esp_http_client_is_chunked_response(client)) {
      ESP_LOGE(TAG, "chunked response is not supported");
      break;
    } else {
      int bytes = esp_http_client_read(client, receive_data, receive_data_len);
      size_t content_length = esp_http_client_get_content_length(client);
      if (content_length > receive_data_len) {
        ESP_LOGW(TAG, "content length %d > expected %d", content_length, receive_data_len);
      }
      ESP_LOGD(TAG, "non-chunked, status:%d content_length:%d bytes_read:%d", status, content_length, bytes);
    }
  } while(false);

  esp_http_client_cleanup(client);
  return status;
}
