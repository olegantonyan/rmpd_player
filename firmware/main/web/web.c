#include "web/web.h"
#include "fs/spi.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"

static const char *TAG = "web";

static void send_file(FILE* f, httpd_req_t *req);
static bool string_ends_with(const char *str, const char *suffix);

static esp_err_t root_get_handler(httpd_req_t *req) {
  if(strcmp("/", req->uri) == 0) {
    FILE* f = fopen(FS_SPI_MOUNTPOINT "/index.html", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "cannot open index.html");
        httpd_resp_set_status(req, "<h1>" HTTPD_404 "</h1>");
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, HTTPD_404, strlen(HTTPD_404));
        return ESP_OK;
    }
    httpd_resp_set_type(req, "text/html");
    send_file(f, req);
    fclose(f);
    return ESP_OK;
  }

  size_t fname_size = HTTPD_MAX_URI_LEN + strlen(FS_SPI_MOUNTPOINT);
  char *fname = malloc(fname_size);
  memset(fname, 0, fname_size);
  strcpy(fname, FS_SPI_MOUNTPOINT);
  strcat(fname, "/");
  strncat(fname, req->uri + 1, fname_size - strlen(fname));
  FILE* f = fopen(fname, "r");
  if(f == NULL) {
    httpd_resp_set_status(req, HTTPD_404);
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, HTTPD_404, strlen(HTTPD_404));
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
  } else {
    httpd_resp_set_type(req, "application/octet-stream");
  }
  send_file(f, req);
  fclose(f);
  free(fname);
  return ESP_OK;
}

httpd_uri_t root = {
    .uri       = "/*",
    .method    = HTTP_GET,
    .handler   = root_get_handler
};

static void send_file(FILE* f, httpd_req_t *req) {
  static const size_t HTTP_CHUNK_SIZE = 1024;
  char *buffer = malloc(HTTP_CHUNK_SIZE);
  size_t bytes_read = 0;
  while ((bytes_read = fread(buffer, 1, HTTP_CHUNK_SIZE, f)) > 0) {
    httpd_resp_send_chunk(req, buffer, bytes_read);
  }
  httpd_resp_send_chunk(req, NULL, 0);
  free(buffer);
}

static bool string_ends_with(const char *str, const char *suffix) {
  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);
  return (str_len >= suffix_len) && (0 == strcmp(str + (str_len - suffix_len), suffix));
}

httpd_handle_t start_webserver() {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  ESP_LOGI(TAG, "starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) != ESP_OK) {
    ESP_LOGI(TAG, "error starting server!");
    return NULL;
  }
  httpd_register_uri_handler(server, &root);
  return server;
}

void stop_webserver(httpd_handle_t server) {
    httpd_stop(server);
}

bool web_init() {
  return start_webserver() != NULL;
}
