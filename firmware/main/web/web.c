#include "web/web.h"
#include "storage/spi.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "config/config.h"
#include "wifi/wifi.h"
#include "audio/player.h"
#include "util/files.h"
#include "audio/player.h"

static const char *TAG = "web";

static void send_file(FILE* f, httpd_req_t *req);
static esp_err_t root_get_handler(httpd_req_t *req);
static esp_err_t settings_get_handler(httpd_req_t *req);
static esp_err_t settings_post_handler(httpd_req_t *req);
static esp_err_t volume_post_handler(httpd_req_t *req);
static esp_err_t status_get_handler(httpd_req_t *req);
static httpd_handle_t start_webserver();
static void render_settings(httpd_req_t *req);

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

static esp_err_t settings_get_handler(httpd_req_t *req) {
  render_settings(req);
  return ESP_OK;
}

static esp_err_t volume_post_handler(httpd_req_t *req) {
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
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  player_set_volume(volume->valueint);

  httpd_resp_send(req, "", strlen("")); // TODO render status maybe?

  free(buffer);
  return ESP_OK;
}

static esp_err_t settings_post_handler(httpd_req_t *req) {
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
  httpd_resp_set_type(req, "application/json");

  cJSON *root = cJSON_CreateObject();

  char now_playing[800] = { 0 };
  if (player_get_now_playing(now_playing, sizeof(now_playing))) {
    cJSON_AddItemToObject(root, "now_playing", cJSON_CreateString(now_playing));
  } else {
    cJSON_AddItemToObject(root, "now_playing", cJSON_CreateString("nothing"));
  }
  cJSON_AddItemToObject(root, "percent_pos", cJSON_CreateNumber(player_get_position_percents()));
  cJSON_AddItemToObject(root, "volume", cJSON_CreateNumber(config_volume()));
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
  return ESP_OK;
}

static esp_err_t root_get_handler(httpd_req_t *req) {
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

  ESP_LOGI(TAG, "starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) != ESP_OK) {
    ESP_LOGI(TAG, "error starting server!");
    return NULL;
  }
  httpd_register_uri_handler(server, &settings_get);
  httpd_register_uri_handler(server, &settings_post);
  httpd_register_uri_handler(server, &volume_post);
  httpd_register_uri_handler(server, &status_get);
  httpd_register_uri_handler(server, &root);
  return server;
}
