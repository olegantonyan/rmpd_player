#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#define SCHEDULER_MAX_MEDIAFILES 65534 // NOTE: 65535 is used as blank list item in stream scheduler, @see BLANK_LIST_ITEM

bool scheduler_init();
bool scheduler_next();
bool scheduler_prev();
bool scheduler_set_random(bool arg);
bool scheduler_random();
bool scheduler_mediafile_match_func(const char *fname);
void scheduler_stop_process();

#endif
