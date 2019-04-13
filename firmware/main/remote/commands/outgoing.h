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
  ACK_FAIL,
  UPDATE_PLAYLIST
} OutgoingCommand_t;

typedef void (*outgoing_command_callback_t)(bool sent_ok);

typedef struct {
  cJSON *json;                   // In & Out
  uint32_t sequence;             // Out
  uint16_t max_retries;          // Out
  const void *args;              // In
} OutgoingCommandArgument_t;

typedef struct {
  const char *message;
  uint32_t sequence;
} AckCommandArgs_t;

typedef struct {
  uint32_t files_done;
  uint32_t sequence;
} UpdatePlaylistCommandArgs_t;

bool outgoing_command(OutgoingCommand_t cmd, void *args, outgoing_command_callback_t callback);

#endif
