#ifndef __TEMPFILE_H
#define __TEMPFILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include "storage/sd.h"

#define TEMPFILE_DIR (STORAGE_SD_MOUNTPOINT "/.tmp")

typedef struct {
  FILE *file;
  char *path;
} Tempfile_t;

void tempfile_init();
bool tempfile_create(Tempfile_t *tf);
bool tempfile_remove(Tempfile_t *tf);
bool tempfile_close(Tempfile_t *tf);
bool tempfile_open(Tempfile_t *tf);

#endif
