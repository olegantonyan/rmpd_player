#include "audio/stream.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/ringbuf.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include <errno.h>

static const char *TAG = "stream";

static const EventBits_t EOS_BIT = BIT0;
static const EventBits_t STOP_BIT = BIT1;

typedef struct {
  char host[256];
  char protocol[10];
  char path[256];
  char port[6];
} stream_addr_t;

static bool parse_uri(const char *uri, stream_addr_t *result);
static int open_socket(stream_addr_t *addr);
static void thread(void *params);

bool stream_start(const char *url, size_t read_chunk_size, stream_t *out) {
  ESP_LOGI(TAG, "starting %s", url);

  stream_addr_t stream_addr;
  if (!parse_uri(url, &stream_addr)) {
    ESP_LOGE(TAG, "error parsing url '%s'", url);
    return false;
  }
  if (strcmp(stream_addr.protocol, "http") != 0) {
    ESP_LOGE(TAG, "protocol %s is not supported", stream_addr.protocol);
    return false;
  }

  int sock = -1;
  do {
    sock = open_socket(&stream_addr);
    if (sock >= 0) {
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(3000));
  } while(sock < 0);
  out->socket = sock;
  out->read_chunk_size = read_chunk_size;
  out->buffer = xRingbufferCreate(read_chunk_size * 4, RINGBUF_TYPE_BYTEBUF);
  if (out->buffer == NULL) {
    ESP_LOGE(TAG, "cannot create ring buffer");
    return false;
  }
  out->event_group = xEventGroupCreate();
  if (out->event_group == NULL) {
    ESP_LOGE(TAG, "cannot create event group");
    return false;
  }

  return xTaskCreate(thread, "stream", 2048, out, 5, NULL) == pdPASS;
}

bool stream_stop(stream_t *stream) {
  ESP_LOGI(TAG, "stopping");

  xEventGroupSetBits(stream->event_group, STOP_BIT);

  uint8_t dummy = 0;
  xRingbufferReceiveUpTo(stream->buffer, (size_t *)&dummy, 0, sizeof(dummy)); // force read if reader thread already stopped
  xRingbufferSend(stream->buffer, &dummy, sizeof(dummy), 0); // force read from the stream if there's someone waiting for it

  xEventGroupWaitBits(stream->event_group, EOS_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

  close(stream->socket);
  vRingbufferDelete(stream->buffer);
  vEventGroupDelete(stream->event_group);
  stream->buffer = NULL;
  stream->event_group = NULL;
  stream->socket = -1;

  ESP_LOGI(TAG, "stopped");

  return true;
}

size_t stream_read(stream_t *stream, uint8_t *buffer, size_t buffer_size) {
  if (xEventGroupGetBits(stream->event_group) & EOS_BIT) {
    return 0;
  }

  size_t bytes = 0;
  uint8_t *data = (uint8_t *)xRingbufferReceiveUpTo(stream->buffer, &bytes, pdMS_TO_TICKS(5000), buffer_size);
  if (data != NULL) {
    memcpy(buffer, data, bytes);
    vRingbufferReturnItem(stream->buffer, (void *)data);
  } else {
    ESP_LOGW(TAG, "buffer undeflow");
  }

  return bytes;
}

static bool parse_uri(const char *uri, stream_addr_t *addr) {
  if (sscanf(uri, "%9[^:]://%255[^:]:%5[^/]%255[^\n]", addr->protocol, addr->host, addr->port, addr->path) == 4) {
  } else if (sscanf(uri, "%9[^:]://%255[^/]%255[^\n]", addr->protocol, addr->host, addr->path) == 3) {
  } else if (sscanf(uri, "%9[^:]://%255[^:]:%5[^\n]", addr->protocol, addr->host, addr->port) == 3) {
  } else if (sscanf(uri, "%9[^:]://%255[^\n]", addr->protocol, addr->host) == 2) {
  } else {
    return false;
  }

  if (strcmp(addr->port, "") == 0) {
    if (strcmp(addr->protocol, "https") == 0) {
      strcpy(addr->port, "443");
    } else if (strcmp(addr->protocol, "http") == 0) {
      strcpy(addr->port, "80");
    }
  }
  if (strcmp(addr->path, "") == 0) {
    strcpy(addr->path, "/");
  }

  ESP_LOGI(TAG, "protocol:%s host:%s port:%s path:%s", addr->protocol, addr->host, addr->port, addr->path);

  return true;
}

static int open_socket(stream_addr_t *stream_addr) {
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
  if (send(sock, request, strlen(request), 0) < 0) {
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

static void thread(void *params) {
  stream_t *stream = (stream_t *)params;

  ESP_LOGI(TAG, "started");

  uint8_t *chunk_buf = malloc(stream->read_chunk_size);
  int bytes = -1;
  do {
    bytes = recv(stream->socket, chunk_buf, stream->read_chunk_size, MSG_WAITALL);
    if (bytes <= 0 || (xEventGroupGetBits(stream->event_group) & STOP_BIT)) {
      break;
    }
    if (xRingbufferSend(stream->buffer, chunk_buf, bytes, pdMS_TO_TICKS(5000)) == pdFALSE) {
      break;
    }
    if (xEventGroupGetBits(stream->event_group) & STOP_BIT) {
      break;
    }
  } while(bytes > 0);
  free(chunk_buf);

  ESP_LOGI(TAG, "end of stream");

  xEventGroupSetBits(stream->event_group, EOS_BIT);
  vTaskDelete(NULL);
}
