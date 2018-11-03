#ifndef __CLOCK_H
#define __CLOCK_H

#include "stm32f1xx_hal.h"
#include <time.h>

time_t time(time_t *t); // override standard function

void clock_init(RTC_HandleTypeDef *rtc);
time_t clock_gettime();

#endif
