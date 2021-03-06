#ifndef __BASE64_H
#define __BASE64_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

size_t base64_decode(char *dest, const char *src, size_t srclen);
size_t base64_encode(char *encoded, const char *string, size_t len);

#endif
