#ifndef __DATETIME_H
#define __DATETIME_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>

int32_t datetime_string_to_seconds_int(const char *string);
int32_t datetime_date_string_to_date_int(const char *string);
int32_t datetime_current_date_int();
int32_t datetime_current_time_int();

#endif
