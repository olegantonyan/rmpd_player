#ifndef __OUTGOING_H
#define __OUTGOING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

typedef enum {
  NOW_PLAYING = 100,
  POWER_ON,
  TRACK_BEGIN,
  TRACK_END,
  ACK_OK,
  ACK_FAIL
} OutgoingCommand_t;

typedef struct {
  cJSON *json;          // In & Out
  uint32_t sequence;    // Out
  uint16_t max_retries; // Out
  const void *args;     // In
} OutgoingCommandArgument_t;

typedef struct {
  const char *message;
  uint32_t sequence;
} AckCommandArgs_t;

bool outgoing_command(OutgoingCommand_t cmd, void *args);

#endif
