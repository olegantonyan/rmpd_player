#ifndef _RNG_H
#define _RNG_H

#include <stdint.h>

#include "stm32f4xx_hal.h"

bool rng_init(RNG_HandleTypeDef *rng);
uint32_t rng_get();

#endif
