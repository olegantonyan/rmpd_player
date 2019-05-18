#ifndef __RANDOM_H
#define __RANDOM_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define RANDOM_MAX_TRAIL 32

void random_init(uint16_t max);
uint16_t random_next();
void random_reset();

#endif
