#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

bool scheduler_init();
bool scheduler_next();
bool scheduler_prev();
bool scheduler_set_random(bool arg);

#endif
