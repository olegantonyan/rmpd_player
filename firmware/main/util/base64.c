#include "util/base64.h"

#include <string.h>
#include <stdlib.h>

static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static bool is_base64(char c);
static inline char value(char c);

size_t base64_decode(char *dest, const char *src, size_t srclen) {
  *dest = '\0';
  if(*src == '\0') {
    return 0;
  }
  char *p = dest;
  do {
    char a = value(src[0]);
    char b = value(src[1]);
    char c = value(src[2]);
    char d = value(src[3]);
    *p++ = (a << 2) | (b >> 4);
    *p++ = (b << 4) | (c >> 2);
    *p++ = (c << 6) | d;
    if(!is_base64(src[1])) {
      p -= 2;
      break;
    } else if(!is_base64(src[2])) {
      p -= 2;
      break;
    } else if(!is_base64(src[3])) {
      p--;
      break;
    }
    src += 4;
    while(*src && (*src == 13 || *src == 10)) {
      src++;
    }
  } while(srclen -= 4);
  *p = 0;
  return p - dest;
}

size_t base64_encode(char *encoded, const char *string, size_t len) {
    int i = 0;
    char *p = encoded;
    for (i = 0; i < len - 2; i += 3) {
      *p++ = table[(string[i] >> 2) & 0x3F];
      *p++ = table[((string[i] & 0x3) << 4) |
                      ((int) (string[i + 1] & 0xF0) >> 4)];
      *p++ = table[((string[i + 1] & 0xF) << 2) |
                      ((int) (string[i + 2] & 0xC0) >> 6)];
      *p++ = table[string[i + 2] & 0x3F];
    }
    if (i < len) {
    *p++ = table[(string[i] >> 2) & 0x3F];
    if (i == (len - 1)) {
        *p++ = table[((string[i] & 0x3) << 4)];
        *p++ = '=';
    }
    else {
        *p++ = table[((string[i] & 0x3) << 4) |
                        ((int) (string[i + 1] & 0xF0) >> 4)];
        *p++ = table[((string[i + 1] & 0xF) << 2)];
    }
    *p++ = '=';
    }

    *p++ = '\0';
    return p - encoded;
}

static bool is_base64(char c) {
  return c && strchr(table, c) != NULL;
}

static inline char value(char c) {
  const char *p = strchr(table, c);
  if(p != NULL) {
    return p - table;
  } else {
    return 0;
  }
}
