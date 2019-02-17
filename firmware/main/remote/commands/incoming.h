#ifndef __INCOMING_H
#define __INCOMING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

typedef struct {
  const cJSON *json;
  uint32_t sequence;
} IncomingCommandArgument_t;

bool incoming_command(const char *data, uint32_t sequence);

#endif
