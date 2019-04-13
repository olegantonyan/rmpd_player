#ifndef __OFFLINE_SCHEDULER_H
#define __OFFLINE_SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#define OFFLINE_SCHEDULER_MAX_MEDIAFILES 65534 // NOTE: 65535 is used as blank list item in stream scheduler, @see BLANK_LIST_ITEM

bool offline_scheduler_init();
bool offline_scheduler_next();
bool offline_scheduler_prev();
bool offline_scheduler_set_random(bool arg);
bool offline_scheduler_random();
bool offline_scheduler_mediafile_match_func(const char *fname);
void offline_scheduler_deinit();

#endif
