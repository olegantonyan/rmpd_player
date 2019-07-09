#ifndef __TRACK_H
#define __TRACK_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef enum {
  TRACK_BACKGROUND = 0,
  TRACK_ADVERTISING
} TrackType_t;

typedef struct {
  int32_t begin_date;
  int32_t end_date;
  int32_t *schedule_seconds;
  uint16_t schedule_seconds_size;
} DateInterval_t;

typedef struct {
  const char *filename;
  int64_t id;
  TrackType_t type:1;
  DateInterval_t *date_intervals;
  uint16_t date_intervals_size;
} Track_t;

bool track_copy(Track_t *dst, const Track_t *src);
void track_free(Track_t *track);
const char *track_type_to_string(const Track_t *track);

#endif
