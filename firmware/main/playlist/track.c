#include "playlist/track.h"
#include <string.h>
#include <stdlib.h>

bool track_copy(Track_t *dst, const Track_t *src) {
  if (dst == NULL || src == NULL) {
    return false;
  }

  dst->filename = malloc(strlen(src->filename) + 1);
  if (dst->filename == NULL) {
    return false;
  }
  strcpy((char *)dst->filename, src->filename);

  dst->id = src->id;
  dst->type = src->type;

  dst->date_intervals_size = src->date_intervals_size;
  if (src->date_intervals_size > 0) {
    dst->date_intervals = malloc(src->date_intervals_size * sizeof(DateInterval_t));
    if (dst->date_intervals == NULL) {
      free((void *)dst->filename);
      return false;
    }
    for (size_t i = 0; i < src->date_intervals_size; i++) {
      dst->date_intervals[i].begin_date = src->date_intervals[i].begin_date;
      dst->date_intervals[i].end_date = src->date_intervals[i].end_date;
      dst->date_intervals[i].schedule_seconds_size = src->date_intervals[i].schedule_seconds_size;

      if (src->date_intervals[i].schedule_seconds_size > 0) {
        size_t schedule_bytes_length = src->date_intervals[i].schedule_seconds_size * sizeof(int32_t);
        dst->date_intervals[i].schedule_seconds = malloc(schedule_bytes_length);
        if (dst->date_intervals[i].schedule_seconds == NULL) {
          free((void *)dst->filename);
          free(dst->date_intervals);
          return false;
        }
        memcpy(dst->date_intervals[i].schedule_seconds, src->date_intervals[i].schedule_seconds, schedule_bytes_length);
      }
    }
  }

  return true;
}

void track_free(Track_t *track) {
  if (track->filename != NULL) {
    free((void *)track->filename);
    track->filename = NULL;
  }
  if (track->date_intervals != NULL) {
    for (size_t i = 0; i < track->date_intervals_size; i++) {
      if (track->date_intervals[i].schedule_seconds != NULL) {
        free(track->date_intervals[i].schedule_seconds);
        track->date_intervals[i].schedule_seconds = NULL;
      }
    }
    free(track->date_intervals);
    track->date_intervals = NULL;
  }
  track->date_intervals_size = 0;
}
