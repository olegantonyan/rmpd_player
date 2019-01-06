#ifndef __HTTP_H
#define __HTTP_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

int http_get(const char *uri, uint8_t *data, size_t data_max_length, size_t *content_length);

#endif
