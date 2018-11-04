#ifndef __OS_H
#define __OS_H

#include "stm32f1xx_hal.h"
#include "clock/clock.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "timers.h"

inline void os_delay(uint32_t ticks) {
  vTaskDelay(ticks / portTICK_PERIOD_MS);
}

#endif
