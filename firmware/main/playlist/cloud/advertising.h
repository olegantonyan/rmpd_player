#ifndef __ADEVERTISING_H
#define __ADEVERTISING_H

#include <stdbool.h>
#include <stdint.h>
#include "playlist/track.h"

#define ADVERTISING_MAX_TRACKS 20

bool advertising_init(const char *playlist_file_path);
void advertising_deinit();
Track_t *advertising_now();

#endif
