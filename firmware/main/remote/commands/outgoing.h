#ifndef __OUTGOING_H
#define __OUTGOING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum {
  NOW_PLAYING = 100
} OutgoingCommand_t;

typedef struct {
  char *data;
  uint32_t sequence;
  uint32_t queue_timeout;
} OutgoingCommandResult_t;

bool outgoing_command(OutgoingCommand_t cmd, void *args);

#endif
