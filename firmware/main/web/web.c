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

static const char *TAG = "web";

static void send_file(FILE* f, httpd_req_t *req);
static bool string_ends_with(const char *str, const char *suffix);
static size_t file_size(FILE *f);
static void set_common_headers(httpd_req_t *req);
static esp_err_t root_get_handler(httpd_req_t *req);
static httpd_handle_t start_webserver();

static httpd_uri_t root = {
  .uri       = "/*",
  .method    = HTTP_GET,
  .handler   = root_get_handler
};

bool web_init() {
  return start_webserver() != NULL;
}

static esp_err_t root_get_handler(httpd_req_t *req) {
  set_common_headers(req);

  if(strcmp("/", req->uri) == 0) {
    FILE* f = fopen(STORAGE_SPI_MOUNTPOINT "/index.html", "r");
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

  size_t fname_size = HTTPD_MAX_URI_LEN + strlen(STORAGE_SPI_MOUNTPOINT);
  char *fname = malloc(fname_size);
  memset(fname, 0, fname_size);
  strcpy(fname, STORAGE_SPI_MOUNTPOINT);
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

static bool string_ends_with(const char *str, const char *suffix) {
  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);
  return (str_len >= suffix_len) && (0 == strcmp(str + (str_len - suffix_len), suffix));
}

static size_t file_size(FILE *f) {
  fseek(f, 0, SEEK_END);
  size_t sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  return sz;
}

static void set_common_headers(httpd_req_t *req) {
  time_t now = time(NULL);
  struct tm timeinfo = { 0 };
  localtime_r(&now, &timeinfo);
  if (timeinfo.tm_year >= (2018 - 1900)) {
    char buffer[64] = { 0 };
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &timeinfo);
    httpd_resp_set_hdr(req, "Date", buffer);
  }

  httpd_resp_set_hdr(req, "Server", "esp_http_server");
}

static httpd_handle_t start_webserver() {
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
