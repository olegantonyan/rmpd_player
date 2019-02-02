#ifndef __STREAM_H
#define __STREAM_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/event_groups.h"

#define STREAM_BUFFER_MAX_CHUNKS 12

typedef struct {
  int socket;
  RingbufHandle_t buffer;
  EventGroupHandle_t event_group;
  size_t read_chunk_size;
} stream_t;

bool stream_start(const char *url, size_t read_chunk_size, stream_t *out);
bool stream_stop(stream_t *stream);
size_t stream_read(const stream_t *stream, uint8_t *buffer, size_t buffer_size);
bool stream_probe(const char *url);

#endif
