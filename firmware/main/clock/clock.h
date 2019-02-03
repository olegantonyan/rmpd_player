#ifndef __CLOCK_H
#define __CLOCK_H

#include <stdbool.h>
#include <stdint.h>

bool clock_init();
bool clock_set_timezone_from_config();
const char *clock_zones();

#endif
