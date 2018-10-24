#include "cs43l22.h"

static CS43L22 config;
static bool initialized = false;

static void reset_cycle();
static void init_sequence();

bool cs43l22_init(CS43L22 cs43l22) {
  if (initialized) {
    return false;
  }
  config = cs43l22;

  reset_cycle();
  init_sequence();

  initialized = true;
  return true;
}

// private

static void reset_cycle() {
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_SET);
}

static void init_sequence() {

}
