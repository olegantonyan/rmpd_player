#ifndef __PLAYER_H
#define __PLAYER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool player_init();
bool player_start(const char *fname, bool async);
bool player_stop();
bool player_get_now_playing(char *buffer, size_t length);

#endif
