#include "remote/file_download.h"
#include "util/certs/certs.h"
#include "esp_http_client.h"
#include "util/url.h"
#include "config/config.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <unistd.h>
#include "storage/sd.h"
#include <string.h>
#include "system/sysinfo.h"

static const char *TAG = "file_download";

typedef struct {
  FILE *file;
  uint8_t *buffer;
  size_t max_size;
  size_t bytes_in_buffer;
  bool stop_flag;
  SemaphoreHandle_t received_sema;
  SemaphoreHandle_t written_sema;
} FileWriterArg_t;

static esp_err_t http_event_handle(esp_http_client_event_t *evt);
static void file_write_thread(void *args);

/*
need at least 6kb stack
vTaskDelay(pdMS_TO_TICKS(30000));
//wifi_wait_connected(100000);

remove("/sdcard/download.aaa");
int res = file_download_start("http://192.168.1.3:3000/rails/active_storage/blobs/eyJfcmFpbHMiOnsibWVzc2FnZSI6IkJBaHBWdz09IiwiZXhwIjpudWxsLCJwdXIiOiJibG9iX2lkIn19--875f1583cf8ad0aa2d9cdfb39bc25147ec484b3b/09%20The%20Immaculate%20Deception.mp3", "/sdcard/download.aa");
ESP_LOGI(TAG, "STATUS %d", res);
*/
int file_download_start(const char *url, const char *download_path, size_t buffer_size) {
  FileWriterArg_t f;
  f.file = fopen(download_path, "ab+");
  f.buffer = malloc(buffer_size);
  f.max_size = buffer_size;
  f.received_sema = xSemaphoreCreateBinary();
  f.written_sema = xSemaphoreCreateBinary();
  f.stop_flag = false;
  xSemaphoreGive(f.written_sema);

  int status = -1;
  do {
    BaseType_t task_created = xTaskCreate(file_write_thread, "dl_file_write", 2048, (void *)&f, 15, NULL);
    if (pdPASS != task_created) {
      ESP_LOGE(TAG, "cannot create file writer thread thread");
      break;
    }

    esp_http_client_config_t config = {
      .event_handler = http_event_handle,
      .url = url,
      .timeout_ms = 6000,
      .method = HTTP_METHOD_GET,
      .cert_pem = certs(),
      .max_redirection_count = 4,
      .disable_auto_redirect = false,
      .user_data = (void *)&f,
      .buffer_size = buffer_size,
      .buffer_size_tx = 2048
    };
    ESP_LOGI(TAG, "request to %s", config.url);
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
      ESP_LOGE(TAG, "cannot initialize http client");
      break;
    }

    char ua[128] = { 0 };
    sysinfo_useragent(ua, sizeof(ua));
    esp_http_client_set_header(client, "User-Agent", ua);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
      status = esp_http_client_get_status_code(client);
    } else {
      ESP_LOGW(TAG, "http get request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
  } while(false);

  f.stop_flag = true;
  xSemaphoreGive(f.received_sema);
  xSemaphoreTake(f.written_sema, portMAX_DELAY);
  fflush(f.file);
  fclose(f.file);
  free(f.buffer);
  vSemaphoreDelete(f.received_sema);
  vSemaphoreDelete(f.written_sema);

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
      FileWriterArg_t *f = (FileWriterArg_t *)evt->user_data;
      if(evt->data_len > f->max_size) {
        ESP_LOGE(TAG, "received more bytes than we can save to file");
      } else {
        xSemaphoreTake(f->written_sema, portMAX_DELAY);
        f->bytes_in_buffer = evt->data_len;
        memcpy(f->buffer, evt->data, evt->data_len);
        xSemaphoreGive(f->received_sema);
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

static void file_write_thread(void *args) {
  FileWriterArg_t *f = (FileWriterArg_t *)args;

  while(true) {
    xSemaphoreTake(f->received_sema, portMAX_DELAY);
    if(f->stop_flag) {
      break;
    }
    
    bool taken = sd_global_lock_acquire(1500);
    write(fileno(f->file), f->buffer, f->bytes_in_buffer);
    //fwrite(f->buffer, f->bytes_in_buffer, 1, f->file);
    if (taken) {
      sd_global_lock_release();
    }

    xSemaphoreGive(f->written_sema);
  }

  xSemaphoreGive(f->written_sema);
  vTaskDelete(NULL);
}
