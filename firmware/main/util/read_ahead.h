#ifndef __READ_AHEAD_H
#define __READ_AHEAD_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

typedef struct {
  uint8_t *buffer;
  size_t buffer_size;
  size_t bytes_read;
  FILE *file;
  SemaphoreHandle_t mutex;
  SemaphoreHandle_t sema;
} ReadAhead_t;

ReadAhead_t *read_ahead_init(FILE *file, size_t buffer_size);
size_t read_ahead_next(ReadAhead_t *ra, uint8_t *buffer, size_t buffer_size);
void read_ahead_deinit(ReadAhead_t *ra);

#endif
