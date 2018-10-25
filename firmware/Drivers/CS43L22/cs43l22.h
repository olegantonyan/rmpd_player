#ifndef __CS43L22_H
#define __CS43L22_H

#include <stdint.h>

#include "stm32f4xx_hal.h"

typedef struct {
  I2C_HandleTypeDef *i2c;
  I2S_HandleTypeDef *i2s;
  uint8_t address;
  uint16_t reset_pin;
  GPIO_TypeDef *reset_port;
} CS43L22;

bool cs43l22_init(CS43L22 cs43l22);
void cs43l22_set_master_volume(uint8_t percents);

#endif // __CS43L22_H
