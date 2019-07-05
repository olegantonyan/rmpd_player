#ifndef __TRACK_H
#define __TRACK_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  TRACK_BACKGROUND = 1,
  TRACK_ADVERTISING
} TrackType_t;

typedef struct {
  const char *filename;
  int64_t id;
  TrackType_t type;
  uint16_t begin_time;
  uint16_t end_time;
} Track_t;

#endif
