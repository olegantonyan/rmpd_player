#include "playlist/random.h"

#include <string.h>
#include "esp_system.h"
#include "esp_log.h"

static uint16_t rng();
static bool is_in_trail(uint16_t num);

static uint16_t maximum = 0;
static uint16_t trail[RANDOM_MAX_TRAIL] = { 0 };
static size_t trail_index = 0;

void random_init(uint16_t max) {
  maximum = max;
}

void random_reset() {
  memset(trail, 0, sizeof(trail));
  trail_index = 0;
}

uint16_t random_next() {
  uint16_t value = 0;
  uint8_t tries = 200;
  do {
    value = rng();
  } while(is_in_trail(value) && tries-- > 0 );
  trail[trail_index++] = value;
  if (trail_index >= RANDOM_MAX_TRAIL) {
    random_reset();
  }
  return value;
}

static uint16_t rng() {
  if (maximum == 0) {
    return 0;
  }
  return esp_random() % maximum;
}

static bool is_in_trail(uint16_t num) {
  for(size_t i = 0; i < RANDOM_MAX_TRAIL; i++) {
    if (num == trail[i]) {
      return true;
    }
  }
  return false;
}
