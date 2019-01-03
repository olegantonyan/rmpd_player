#include "web/web.h"
#include "storage/spi.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "config/config.h"
#include "wifi/wifi.h"
#include "audio/player.h"
#include "util/files.h"
#include "audio/player.h"
#include "audio/scheduler.h"
#include <tcpip_adapter.h>

static const char *TAG = "web";

static void send_file(FILE* f, httpd_req_t *req);
static esp_err_t root_get_handler(httpd_req_t *req);
static esp_err_t settings_get_handler(httpd_req_t *req);
static esp_err_t settings_post_handler(httpd_req_t *req);
static esp_err_t tone_get_handler(httpd_req_t *req);
static esp_err_t tone_post_handler(httpd_req_t *req);
static esp_err_t volume_post_handler(httpd_req_t *req);
static esp_err_t status_get_handler(httpd_req_t *req);
static esp_err_t playback_post_handler(httpd_req_t *req);
static esp_err_t reboot_post_handler(httpd_req_t *req);
static esp_err_t system_get_handler(httpd_req_t *req);
static httpd_handle_t start_webserver();
static void render_settings(httpd_req_t *req);
static void render_status(httpd_req_t *req);

static httpd_uri_t root = {
  .uri       = "/*",
  .method    = HTTP_GET,
  .handler   = root_get_handler
};

static httpd_uri_t settings_get = {
  .uri       = "/api/settings.json",
  .method    = HTTP_GET,
  .handler   = settings_get_handler
};

static httpd_uri_t settings_post = {
  .uri       = "/api/settings.json",
  .method    = HTTP_POST,
  .handler   = settings_post_handler
};

static httpd_uri_t status_get = {
  .uri       = "/api/status.json",
  .method    = HTTP_GET,
  .handler   = status_get_handler
};

static httpd_uri_t volume_post = {
  .uri       = "/api/volume.json",
  .method    = HTTP_POST,
  .handler   = volume_post_handler
};

static httpd_uri_t tone_post = {
  .uri       = "/api/tone.json",
  .method    = HTTP_POST,
  .handler   = tone_post_handler
};

static httpd_uri_t tone_get = {
  .uri       = "/api/tone.json",
  .method    = HTTP_GET,
  .handler   = tone_get_handler
};

static httpd_uri_t playback_post = {
  .uri       = "/api/playback.json",
  .method    = HTTP_POST,
  .handler   = playback_post_handler
};

static httpd_uri_t reboot_post = {
  .uri       = "/api/reboot.json",
  .method    = HTTP_POST,
  .handler   = reboot_post_handler
};

static httpd_uri_t system_get = {
  .uri       = "/api/system.json",
  .method    = HTTP_GET,
  .handler   = system_get_handler
};

bool web_init() {
  return start_webserver() != NULL;
}

static void render_settings(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();

  char *ssid = config_wifi_ssid();
  char *pass = config_wifi_pass();

  cJSON_AddItemToObject(root, "wifi_ssid", cJSON_CreateString(ssid));
  cJSON_AddItemToObject(root, "wifi_pass", cJSON_CreateString(pass));

  char* json = malloc(1024);

  if(cJSON_PrintPreallocated(root, json, 1024, 0)) {
    httpd_resp_send(req, json, strlen(json));
  } else {
    ESP_LOGE(TAG, "failed to build json response");
    httpd_resp_send_500(req);
  }

  free(json);
  cJSON_Delete(root);
}

static void render_status(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();

  char now_playing[800] = { 0 };
  if (player_get_now_playing(now_playing, sizeof(now_playing))) {
    cJSON_AddItemToObject(root, "now_playing", cJSON_CreateString(now_playing));
  } else {
    cJSON_AddItemToObject(root, "now_playing", cJSON_CreateString(""));
  }
  cJSON_AddItemToObject(root, "percent_pos", cJSON_CreateNumber(player_get_position_percents()));
  cJSON_AddItemToObject(root, "volume", cJSON_CreateNumber(config_volume()));
  cJSON_AddItemToObject(root, "random", cJSON_CreateBool(scheduler_random()));
  time_t now = time(NULL);
  struct tm timeinfo = { 0 };
  localtime_r(&now, &timeinfo);
  char time_buf[80] = { 0 };
  strftime(time_buf, sizeof(time_buf), "%H:%M:%S %d-%m-%Y %Z", &timeinfo);
  cJSON_AddItemToObject(root, "time", cJSON_CreateString(time_buf));

  char* json = malloc(1024);

  if(cJSON_PrintPreallocated(root, json, 1024, 0)) {
    httpd_resp_send(req, json, strlen(json));
  } else {
    ESP_LOGE(TAG, "failed to build json response");
    httpd_resp_send_500(req);
  }

  free(json);
  cJSON_Delete(root);
}

static esp_err_t settings_get_handler(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "Connection", "close");
  render_settings(req);
  return ESP_OK;
}

static esp_err_t volume_post_handler(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "Connection", "close");
  if (req->content_len > 128) {
    ESP_LOGE(TAG, "too big request body %d", req->content_len);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char *buffer = malloc(req->content_len + 4);
  int ret = httpd_req_recv(req, buffer, req->content_len);
  if (ret <= 0) {
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  buffer[ret] = '\0';

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
    }
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const cJSON *volume = cJSON_GetObjectItemCaseSensitive(json, "volume");
  if (!cJSON_IsNumber(volume)) {
    ESP_LOGE(TAG, "error getting volume from json");
    free(buffer);
    cJSON_Delete(json);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  player_set_volume(volume->valueint);
  free(buffer);
  cJSON_Delete(json);

  render_status(req);
  return ESP_OK;
}

static esp_err_t settings_post_handler(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "Connection", "close");
  bool ok = true;

  if (req->content_len > 2048) {
    ESP_LOGE(TAG, "too big request body %d", req->content_len);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char *buffer = malloc(req->content_len + 4);
  int ret = httpd_req_recv(req, buffer, req->content_len);
  if (ret <= 0) {
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  buffer[ret] = '\0';

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
      ok = false;
      goto exit;
    }
  } else {
    const cJSON *ssid = cJSON_GetObjectItemCaseSensitive(json, "wifi_ssid");
    if (!cJSON_IsString(ssid) || (ssid->valuestring == NULL)) {
      ok = false;
      goto exit;
    }
    const cJSON *pass = cJSON_GetObjectItemCaseSensitive(json, "wifi_pass");
    if (!cJSON_IsString(pass) || (pass->valuestring == NULL)) {
      ok = false;
      goto exit;
    }
    if (strlen(ssid->valuestring) > 31 || strlen(pass->valuestring) > 63) {
      ok = false;
      goto exit;
    }
    ok = config_save_wifi_ssid(ssid->valuestring) && config_save_wifi_pass(pass->valuestring);
  }

exit:
  free(buffer);
  cJSON_Delete(json);
  if (ok) {
    render_settings(req);
    wifi_reconfig();
    return ESP_OK;
  } else {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
}

static esp_err_t status_get_handler(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "Connection", "close");
  render_status(req);
  return ESP_OK;
}

static esp_err_t tone_get_handler(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "Connection", "close");
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();

  cJSON_AddItemToObject(root, "bass_frequency_limit", cJSON_CreateNumber(config_bass_freqlimit()));
  cJSON_AddItemToObject(root, "bass_amplitude", cJSON_CreateNumber(config_bass_amplitude()));
  cJSON_AddItemToObject(root, "treble_frequency_limit", cJSON_CreateNumber(config_treble_freqlimit()));
  cJSON_AddItemToObject(root, "treble_amplitude", cJSON_CreateNumber(config_treble_amplitude()));

  char* json = malloc(1024);

  if(cJSON_PrintPreallocated(root, json, 1024, 0)) {
    httpd_resp_send(req, json, strlen(json));
  } else {
    ESP_LOGE(TAG, "failed to build json response");
    httpd_resp_send_500(req);
  }

  free(json);
  cJSON_Delete(root);

  return ESP_OK;
}

static esp_err_t tone_post_handler(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "Connection", "close");
  if (req->content_len > 256) {
    ESP_LOGE(TAG, "too big request body %d", req->content_len);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char *buffer = malloc(req->content_len + 4);
  int ret = httpd_req_recv(req, buffer, req->content_len);
  if (ret <= 0) {
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  buffer[ret] = '\0';

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
    }
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const cJSON *bass_frequency_limit = cJSON_GetObjectItemCaseSensitive(json, "bass_frequency_limit");
  if (cJSON_IsNumber(bass_frequency_limit)) {
    player_set_bass_freqlimit(bass_frequency_limit->valueint);
  }
  const cJSON *bass_amplitude = cJSON_GetObjectItemCaseSensitive(json, "bass_amplitude");
  if (cJSON_IsNumber(bass_amplitude)) {
    player_set_bass_amplitude(bass_amplitude->valueint);
  }
  const cJSON *treble_frequency_limit = cJSON_GetObjectItemCaseSensitive(json, "treble_frequency_limit");
  if (cJSON_IsNumber(treble_frequency_limit)) {
    player_set_treble_freqlimit(treble_frequency_limit->valueint);
  }
  const cJSON *treble_amplitude = cJSON_GetObjectItemCaseSensitive(json, "treble_amplitude");
  if (cJSON_IsNumber(treble_amplitude)) {
    player_set_treble_amplitude(treble_amplitude->valueint);
  }
  free(buffer);
  cJSON_Delete(json);

  render_status(req);
  return ESP_OK;
}

static esp_err_t playback_post_handler(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "Connection", "close");
  if (req->content_len > 128) {
    ESP_LOGE(TAG, "too big request body %d", req->content_len);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char *buffer = malloc(req->content_len + 4);
  int ret = httpd_req_recv(req, buffer, req->content_len);
  if (ret <= 0) {
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  buffer[ret] = '\0';

  cJSON *json = cJSON_Parse(buffer);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
    }
    free(buffer);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const cJSON *action = cJSON_GetObjectItemCaseSensitive(json, "action");
  if (cJSON_IsString(action) && (action->valuestring != NULL)) {
    if(strcmp(action->valuestring, "next") == 0) {
      scheduler_next();
    } else if(strcmp(action->valuestring, "prev") == 0) {
      scheduler_prev();
    }
  }
  const cJSON *random = cJSON_GetObjectItemCaseSensitive(json, "random");
  if (cJSON_IsTrue(random)) {
    scheduler_set_random(true);
  } else if (cJSON_IsFalse(random)) {
    scheduler_set_random(false);
  }
  free(buffer);
  cJSON_Delete(json);

  render_status(req);
  return ESP_OK;
}

static esp_err_t reboot_post_handler(httpd_req_t *req) {
  esp_restart();
  return ESP_OK;
}

static esp_err_t system_get_handler(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "Connection", "close");
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "heap_free", cJSON_CreateNumber(esp_get_free_heap_size()));
  cJSON_AddItemToObject(root, "heap_free_min", cJSON_CreateNumber(esp_get_minimum_free_heap_size()));

  uint8_t mac[6] = { 0 };
  esp_efuse_mac_get_default(mac);
  char mac_str[20] = { 0 };
  snprintf(mac_str, sizeof(mac_str), "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  cJSON_AddItemToObject(root, "mac_addr", cJSON_CreateString(mac_str));

  tcpip_adapter_ip_info_t ip_info;
  char ip[128] = { 0 };
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info);
  ip4addr_ntoa_r(&ip_info.ip, ip, sizeof(ip));
  cJSON_AddItemToObject(root, "ip_addr", cJSON_CreateString(ip));

  const char *reset_reason = "";
  switch(esp_reset_reason()) {
    case ESP_RST_UNKNOWN:   reset_reason = "unknown"; break;    //!< Reset reason can not be determined
    case ESP_RST_POWERON:   reset_reason = "poweron"; break;     //!< Reset due to power-on event
    case ESP_RST_EXT:       reset_reason = "ext"; break;        //!< Reset by external pin (not applicable for ESP32)
    case ESP_RST_SW:        reset_reason = "sw"; break;         //!< Software reset via esp_restart
    case ESP_RST_PANIC:     reset_reason = "panic"; break;      //!< Software reset due to exception/panic
    case ESP_RST_INT_WDT:   reset_reason = "int_wdt"; break;    //!< Reset (software or hardware) due to interrupt watchdog
    case ESP_RST_TASK_WDT:  reset_reason = "task_wdt"; break;   //!< Reset due to task watchdog
    case ESP_RST_WDT:       reset_reason = "wdt"; break;        //!< Reset due to other watchdogs
    case ESP_RST_DEEPSLEEP: reset_reason = "deepsleep"; break;  //!< Reset after exiting deep sleep mode
    case ESP_RST_BROWNOUT:  reset_reason = "brownout"; break;   //!< Brownout reset (software or hardware)
    case ESP_RST_SDIO:      reset_reason = "sdio"; break;       //!< Reset over SDIO
  }
  cJSON_AddItemToObject(root, "reset_reason", cJSON_CreateString(reset_reason));

  esp_chip_info_t ci;
  esp_chip_info(&ci);
  cJSON_AddItemToObject(root, "chip_revision", cJSON_CreateNumber(ci.revision));

  char* json = malloc(1024);
  if(cJSON_PrintPreallocated(root, json, 1024, 0)) {
    httpd_resp_send(req, json, strlen(json));
  } else {
    ESP_LOGE(TAG, "failed to build json response");
    httpd_resp_send_500(req);
  }

  free(json);
  cJSON_Delete(root);

  return ESP_OK;
}

static esp_err_t root_get_handler(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "Connection", "close");
  if(strcmp("/", req->uri) == 0) {
    FILE* f = fopen(STORAGE_SPI_MOUNTPOINT "/index.html", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "cannot open index.html");
        httpd_resp_send_404(req);
        return ESP_OK;
    }
    httpd_resp_set_type(req, "text/html");
    send_file(f, req);
    fclose(f);
    return ESP_OK;
  }

  size_t fname_size = HTTPD_MAX_URI_LEN + strlen(STORAGE_SPI_MOUNTPOINT);
  char *fname = malloc(fname_size + 4);
  memset(fname, 0, fname_size);
  strcpy(fname, STORAGE_SPI_MOUNTPOINT);
  strcat(fname, "/");
  strncat(fname, req->uri + 1, fname_size - strlen(fname));
  FILE* f = fopen(fname, "r");
  if(f == NULL) {
    httpd_resp_send_404(req);
    free(fname);
    return ESP_OK;
  }

  if(string_ends_with(fname, ".html")) {
    httpd_resp_set_type(req, "text/html");
  } else if(string_ends_with(fname, ".css")) {
    httpd_resp_set_type(req, "text/css");
  } else if(string_ends_with(fname, ".js")) {
    httpd_resp_set_type(req, "application/javascript");
  } else if(string_ends_with(fname, ".ico")) {
    httpd_resp_set_type(req, "image/x-icon");
  } else if(string_ends_with(fname, ".svg")) {
    httpd_resp_set_type(req, "image/svg+xml");
  } else {
    httpd_resp_set_type(req, "application/octet-stream");
  }
  send_file(f, req);
  fclose(f);
  free(fname);
  return ESP_OK;
}

static void send_file(FILE* f, httpd_req_t *req) {
  static const size_t HTTP_CHUNK_SIZE = 1024;
  char *buffer = malloc(HTTP_CHUNK_SIZE);
  if(file_size(f) <= HTTP_CHUNK_SIZE) {
    size_t bytes_read = fread(buffer, 1, HTTP_CHUNK_SIZE, f);
    httpd_resp_send(req, buffer, bytes_read);
  } else {
    size_t bytes_read = 0;
    while ((bytes_read = fread(buffer, 1, HTTP_CHUNK_SIZE, f)) > 0) {
      httpd_resp_send_chunk(req, buffer, bytes_read);
    }
    httpd_resp_send_chunk(req, NULL, 0);
  }
  free(buffer);
}

static httpd_handle_t start_webserver() {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 15;

  ESP_LOGI(TAG, "starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) != ESP_OK) {
    ESP_LOGI(TAG, "error starting server!");
    return NULL;
  }
  httpd_register_uri_handler(server, &settings_get);
  httpd_register_uri_handler(server, &settings_post);
  httpd_register_uri_handler(server, &volume_post);
  httpd_register_uri_handler(server, &status_get);
  httpd_register_uri_handler(server, &tone_get);
  httpd_register_uri_handler(server, &tone_post);
  httpd_register_uri_handler(server, &playback_post);
  httpd_register_uri_handler(server, &reboot_post);
  httpd_register_uri_handler(server, &system_get);
  httpd_register_uri_handler(server, &root);
  return server;
}
