#include <stdio.h>
#include "cs43l22.h"

static CS43L22 config;
static bool initialized = false;

static void reset_cycle();
static void init_sequence();
static bool i2c_write_register(uint8_t reg, uint8_t data);
static uint8_t i2c_read_register(uint8_t reg);

bool cs43l22_init(CS43L22 cs43l22) {
  if (initialized) {
    return false;
  }
  config = cs43l22;

  reset_cycle();
  init_sequence();

  printf("CS43L22 chip id: 0x%x\n", i2c_read_register(0x01));
  printf("CS43L22 power ctl 1: 0x%x\n", i2c_read_register(0x02));
  printf("CS43L22 power ctl 2: 0x%x\n", i2c_read_register(0x04));

  initialized = true;
  return true;
}

// private

static void reset_cycle() {
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_SET);
}

static void init_sequence() {
  i2c_write_register(0x00, 0x99);
  i2c_write_register(0x47, 0x80);
  i2c_write_register(0x32, 0x80);
  i2c_write_register(0x32, 0x00);
  i2c_write_register(0x00, 0x00);
  i2c_write_register(0x06, 0x00); // not sure
  i2c_write_register(0x02, 0x9E);
}

static bool i2c_write_register(uint8_t reg, uint8_t data) {
  uint8_t buf[2] = { reg, data };
  return HAL_I2C_Master_Transmit(config.i2c, config.address, buf, sizeof(buf), 10) == HAL_OK;
}

static uint8_t i2c_read_register(uint8_t reg) {
  uint8_t buf = 0x00;
  HAL_I2C_Master_Receive(config.i2c, config.address, &buf, sizeof(buf), 10);
  return buf;
}
