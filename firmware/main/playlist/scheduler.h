#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#define SCHEDULER_MAX_MEDIAFILES 65534

bool scheduler_init();
bool scheduler_next();
bool scheduler_prev();
bool scheduler_set_random(bool arg);
bool scheduler_random();
bool scheduler_mediafile_match_func(const char *fname);

#endif
