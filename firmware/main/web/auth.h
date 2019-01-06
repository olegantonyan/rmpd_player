#ifndef __AUTH_H
#define __AUTH_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "esp_http_server.h"

typedef struct {
  char login[16];
  char password[16];
} auth_t;

bool auth_check(httpd_req_t *req, auth_t *pwd);

#endif
