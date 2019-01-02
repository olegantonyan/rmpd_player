#ifndef __STREAM_H
#define __STREAM_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool stream_start(const char *url);
bool stream_stop();
size_t stream_read(uint8_t *buffer, size_t buffer_size);

#endif
