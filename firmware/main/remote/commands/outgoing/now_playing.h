#ifndef __NOW_PLAYING_H
#define __NOW_PLAYING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "remote/commands/outgoing.h"

OutgoingCommandResult_t now_playing(void *args);

#endif
