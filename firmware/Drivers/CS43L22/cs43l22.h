#ifndef __CS43L22_H
#define __CS43L22_H

#include <stdint.h>

#include "stm32f4xx_hal.h"

typedef struct {
  bool (*codec_write_register)(uint8_t reg, uint8_t data);
  uint8_t (*codec_read_register)(uint8_t reg);
  void (*codec_reset)();
} CS43L22;

bool cs43l22_init(CS43L22 cs43l22);
void cs43l22_set_master_volume(uint8_t percents);
uint8_t cs43l22_id();

#endif // __CS43L22_H
