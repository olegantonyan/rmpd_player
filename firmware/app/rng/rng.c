#include "rng/rng.h"

static RNG_HandleTypeDef *config = NULL;

bool rng_init(RNG_HandleTypeDef *rng) {
  config = rng;
  return true;
}

uint32_t rng_get() {
  if (!config) {
    return 0;
  }
  uint32_t result = 0;
  HAL_RNG_GenerateRandomNumber(config, &result);
  return result;
}
