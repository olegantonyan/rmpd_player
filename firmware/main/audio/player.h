#ifndef __PLAYER_H
#define __PLAYER_H

#include <stdbool.h>
#include <stdint.h>

bool player_init();
bool player_start(const char *fname);
bool player_stop();

#endif
