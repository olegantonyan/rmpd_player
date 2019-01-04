#include "audio/stream.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include <errno.h>

static const char *TAG = "stream";

typedef struct {
  const char *host;
  const char *protocol;
  const char *path;
  const char *port;
} stream_addr_t;

static stream_addr_t parse_uri(const char *uri);
static int open_socket(stream_addr_t *addr);

bool stream_start(const char *url, stream_t *out) {
  ESP_LOGI(TAG, "starting stream %s", url);
  stream_addr_t stream_addr = parse_uri(url);
  int sock = -1;
  do {
    sock = open_socket(&stream_addr);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  } while(sock < 0);
  out->socket = sock;

  return true;
}

bool stream_stop(stream_t *stream) {
  ESP_LOGI(TAG, "stopping stream");
  close(stream->socket);
  return true;
}

size_t stream_read(stream_t *stream, uint8_t *buffer, size_t buffer_size) {
  int bytes = read(stream->socket, buffer, buffer_size);
  if (bytes < 0) {
    return 0;
  }
  return bytes;
}

static stream_addr_t parse_uri(const char *uri) {
  stream_addr_t addr = {
    .host = "us4.internet-radio.com",
    .protocol = "http",
    .path = "/",
    .port = "8258",
  };
  return addr;
}

static int open_socket(stream_addr_t *stream_addr) {
  if (strcmp(stream_addr->protocol, "http") != 0) {
    ESP_LOGE(TAG, "protocol %s is not supported", stream_addr->protocol);
    return -1;
  }

  const struct addrinfo hints = {
    .ai_family = AF_INET,
    .ai_socktype = SOCK_STREAM,
  };
  struct addrinfo *res;
  int err = getaddrinfo(stream_addr->host, stream_addr->port, &hints, &res);
  if(err != 0) {
    ESP_LOGE(TAG, "dns lookup failed err: %d", err);
    return -1;
  }

  int sock = socket(res->ai_family, res->ai_socktype, 0);
  if(sock < 0) {
    ESP_LOGE(TAG, "error allocating socket");
    freeaddrinfo(res);
    return -1;
  }

  if(connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
    ESP_LOGE(TAG, "socket connect failed: %s", strerror(errno));
    close(sock);
    freeaddrinfo(res);
    return -1;
  }
  freeaddrinfo(res);

  const char *request_template = "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n";
  size_t request_size = strlen(request_template) + strlen(stream_addr->path) + strlen(stream_addr->host) + 13;
  char *request = malloc(request_size);
  snprintf(request, request_size, request_template, stream_addr->path, stream_addr->host);
  if (write(sock, request, strlen(request)) < 0) {
    ESP_LOGE(TAG, "socket write failed");
    free(request);
    close(sock);
    return -1;
  }
  free(request);

  struct timeval receiving_timeout = {
    .tv_sec = 5,
    .tv_usec = 0,
  };
  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0) {
    ESP_LOGE(TAG, "failed to set socket receiving timeout");
    close(sock);
    return -1;
  }
  return sock;
}
