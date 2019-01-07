#ifndef __AUTH_H
#define __AUTH_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "esp_http_server.h"

bool auth_check(httpd_req_t *req);

#endif
