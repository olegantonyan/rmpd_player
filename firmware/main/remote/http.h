#ifndef __REMOTE_HTTP_H
#define __REMOTE_HTTP_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "util/tempfile.h"

#define REMOTE_HTTP_MAX_RECEIVE_DATA_LENGTH 2048

typedef struct {
  uint32_t sequence;
  char data[REMOTE_HTTP_MAX_RECEIVE_DATA_LENGTH];
  size_t length;
  Tempfile_t *datafile;
} http_response_t;

int http_post_cmd(const char *send_data, size_t send_data_len, uint32_t send_seq, http_response_t *response);

#endif
