#ifndef __URL_H
#define __URL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define URL_HOST_MAXLEN 256
#define URL_PROTOCOL_MAXLEN 10
#define URL_PATH_MAXLEN 256
#define URL_PORT_MAXLEN 6
#define URL_USERNAME_MAXLEN 32
#define URL_PASSWORD_MAXLEN 32

typedef struct {
  char host[URL_HOST_MAXLEN];
  char protocol[URL_PROTOCOL_MAXLEN];
  char path[URL_PATH_MAXLEN];
  char port[URL_PORT_MAXLEN];
  char username[URL_USERNAME_MAXLEN];
  char password[URL_PASSWORD_MAXLEN];
} url_t;

bool url_parse(const char *uri, url_t *addr);

#endif
