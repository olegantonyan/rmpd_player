#ifndef __OUTGOING_H
#define __OUTGOING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum {
  NOW_PLAYING = 100,
  POWER_ON
} OutgoingCommand_t;

bool outgoing_command(OutgoingCommand_t cmd, void *args);

#endif
