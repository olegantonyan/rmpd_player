#ifndef __STREAM_SCHEDULER_H
#define __STREAM_SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#define STREAM_SCHEDULER_MAX_STREAMS 64

bool stream_scheduler_init(const char *storage_path);
bool stream_scheduler_start();
void stream_scheduler_deinit();
bool stream_scheduler_add_stream(uint16_t index);
bool stream_scheduler_is_known_stream(uint16_t index);
uint8_t stream_scheduler_max();
bool stream_scheduler_any_alive_streams();
bool stream_scheduler_is_alive(uint16_t index);
void stream_scheduler_mark_dead(uint16_t index);
void stream_scheduler_mark_alive(uint16_t index);
void stream_scheduler_force_probe();

#endif
