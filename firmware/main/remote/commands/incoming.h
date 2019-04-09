#ifndef __INCOMING_H
#define __INCOMING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "pdjson.h"
#include "util/tempfile.h"

typedef struct {
  json_stream json;      // either data or datafile will be present depending on size
  const char *data;   //
  Tempfile_t *datafile;  //
  uint32_t sequence;
} IncomingCommandArgument_t;

bool incoming_init();
bool incoming_command(const char *data, Tempfile_t *datafile, uint32_t sequence);

#endif
