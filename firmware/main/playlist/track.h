#ifndef __TRACK_H
#define __TRACK_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  const char *filename;
  uint32_t id;
} Track_t;

#endif