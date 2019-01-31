#ifndef __STREAM_SCHEDULER_H
#define __STREAM_SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#define STREAM_SCHEDULER_MAX_STREAMS 128

void stream_scheduler_init();
void stream_scheduler_deinit();
bool stream_scheduler_add_stream(uint16_t index);
bool stream_scheduler_is_known_stream(uint16_t index);
uint8_t stream_scheduler_max();
bool stream_scheduler_is_possible();

#endif
