#ifndef __DS3231_H
#define __DS3231_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

bool ds3231_init();
time_t ds3231_get_time();
bool ds3231_set_time(struct tm *timeinfo);

#endif
