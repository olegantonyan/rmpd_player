#ifndef __STREAM_H
#define __STREAM_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  int socket;
} stream_t;

bool stream_start(const char *url, stream_t *out);
bool stream_stop(stream_t *stream);
size_t stream_read(stream_t *stream, uint8_t *buffer, size_t buffer_size);

#endif
