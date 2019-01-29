#include "playlist/stream_scheduler.h"

#include <string.h>

static struct {
  bool initialized;
  uint16_t list[STREAM_SCHEDULER_MAX_STREAMS];
  uint8_t max;
} p = { false, { 0 }, 0 };

void stream_scheduler_init() {
  memset(&p, 0, sizeof(p));
  p.initialized = true;
}

void stream_scheduler_deinit() {
  p.initialized = false;
}

uint8_t stream_scheduler_max() {
  return p.max;
}

bool stream_scheduler_add_stream(uint16_t index) {
  if (!p.initialized) {
    return false;
  }
  if (p.max >= STREAM_SCHEDULER_MAX_STREAMS) {
    return false;
  }
  p.list[p.max++] = index;
  return true;
}

bool stream_scheduler_is_possible() {
  // TODO
  return true;
}
