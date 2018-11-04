#ifndef __LEDS_H
#define __LEDS_H

#include "stm32f1xx_hal.h"

typedef enum {
  LEDS_POWER,
  LEDS_NETWORK,
  LEDS_PLAYER
} Leds_t;

typedef struct {
  GPIO_TypeDef *port;
  uint32_t pin;
} Led_config_t;

typedef struct {
  Led_config_t power;
  Led_config_t network;
  Led_config_t player;
} Leds_config_t;

void leds_init(Leds_config_t cfg);
void leds_on(Leds_t led);
void leds_off(Leds_t led);
void leds_blink(Leds_t led);

#endif
