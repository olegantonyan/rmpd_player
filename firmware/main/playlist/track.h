#ifndef __TRACK_H
#define __TRACK_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  TRACK_BACKGROUND = 0,
  TRACK_ADVERTISING
} TrackType_t;

typedef struct {
  const char *filename;
  int64_t id;
  TrackType_t type:1;
} Track_t;

#endif
