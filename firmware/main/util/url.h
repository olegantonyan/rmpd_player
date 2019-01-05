#ifndef __URL_H
#define __URL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  char host[256];
  char protocol[10];
  char path[256];
  char port[6];
} url_t;

bool url_parse(const char *uri, url_t *addr);

#endif
