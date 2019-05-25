#include "remote/http.h"
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
#include <unistd.h>

static const char *TAG = "remote_http";

static esp_err_t http_event_handle(esp_http_client_event_t *evt);

int http_post_cmd(const char *send_data, size_t send_data_len, uint32_t send_seq, http_response_t *response) {
  url_t *url_parsed = malloc(sizeof(url_t));
  char server_addr[64] = { 0 };
  strncpy(server_addr, config_server_url(), sizeof(server_addr) - 1);
  if (!url_parse(server_addr, url_parsed)) {
    free(url_parsed);
    ESP_LOGE(TAG, "server url parse error");
    return -1;
  }
  char url[128] = { 0 };
  snprintf(url, sizeof(url), "%s://%s:%s@%s:%s/deviceapi/status", url_parsed->protocol, config_deviceid(), config_server_password(), url_parsed->host, url_parsed->port);
  free(url_parsed);

  esp_http_client_config_t config = {
    .event_handler = http_event_handle,
    .url = url,
    .timeout_ms = 10000,
    .method = HTTP_METHOD_POST,
    .cert_pem = certs(),
    .auth_type = HTTP_AUTH_TYPE_BASIC,
    .max_redirection_count = 3,
    .disable_auto_redirect = false,
    .user_data = response
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_post_field(client, send_data, send_data_len);
  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_http_client_set_header(client, "Accept", "application/json");


  char ua[128] = { 0 };
  sysinfo_useragent(ua, sizeof(ua));
  esp_http_client_set_header(client, "User-Agent", ua);

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
      if (strcmp(evt->header_key, "X-Sequence-Number") == 0) {
        http_response_t *r = (http_response_t *)evt->user_data;
        r->sequence = atoi(evt->header_value);
      }
      break;
    case HTTP_EVENT_ON_DATA:
      ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      http_response_t *r = (http_response_t *)evt->user_data;

      if(r->datafile == NULL) {
        if (r->length + evt->data_len >= REMOTE_HTTP_MAX_RECEIVE_DATA_LENGTH) {
          ESP_LOGD(TAG, "no space left in receive buffer for incomming data, dumping to filesystem");
          r->datafile = tempfile_create();
          if (r->datafile == NULL || r->datafile->file == NULL) {
            ESP_LOGE(TAG, "error creating tempfile for incomming data dump");
            r->datafile = NULL;
          } else {
            fwrite(r->data, r->length, 1, r->datafile->file);
            fwrite(evt->data, evt->data_len, 1, r->datafile->file);
            //write(fileno(r->datafile->file), r->data, r->length);
            //write(fileno(r->datafile->file), evt->data, evt->data_len);
            tempfile_close(r->datafile);
          }
        } else {
          memcpy(r->data + r->length, evt->data, evt->data_len);
          r->length += evt->data_len;
        }
      } else {
        tempfile_open(r->datafile, "ab+");
        fwrite(evt->data, evt->data_len, 1, r->datafile->file);
        //write(fileno(r->datafile->file), evt->data, evt->data_len);
        tempfile_close(r->datafile);
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
