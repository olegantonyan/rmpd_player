#ifndef _AUDIO_H
#define _AUDIO_H

#include <stdint.h>

#include "stm32f4xx_hal.h"

typedef struct {
  I2C_HandleTypeDef *i2c;
  I2S_HandleTypeDef *i2s;
  uint8_t address;
  uint16_t reset_pin;
  GPIO_TypeDef *reset_port;
} AudioConfig;

bool audio_init(AudioConfig cfg);
bool audio_play(uint16_t *buffer, size_t size);
void audio_transfer_complete_callback();
void audio_set_volume(uint8_t percents);

#endif
