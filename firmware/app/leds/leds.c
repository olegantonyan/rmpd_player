#include "leds/leds.h"
#include "os.h"

static void write(Leds_t led, uint_fast8_t state);
static void thread(void *params);
static Led_config_t *get_led_config(Leds_t led);

static Leds_config_t config;
static uint8_t blinks[3] = { 0 };

void leds_init(Leds_config_t cfg) {
  config = cfg;

  write(LEDS_POWER, 1);
  write(LEDS_NETWORK, 0);
  write(LEDS_PLAYER, 0);

  TaskHandle_t handle = NULL;
  xTaskCreate(thread, "leds", 32, NULL, tskIDLE_PRIORITY, &handle);
}

void leds_on(Leds_t led) {
  write(led, 1);
}

void leds_off(Leds_t led) {
  write(led, 0);
  blinks[led] = 0;
}

void leds_blink(Leds_t led) {
  blinks[led] = 1;
}

// private

static void thread(void *params) {
  while(true) {
    for (size_t i = 0; i < sizeof(blinks); i++) {
      if (blinks[i] == 1) {
        Led_config_t *c = get_led_config(i);
        if (!c) {
          continue;
        }
        write(i, HAL_GPIO_ReadPin(c->port, c->pin) == GPIO_PIN_SET ? 0 : 1);
      }
    }
    os_delay(500);
  }
}

static Led_config_t *get_led_config(Leds_t led) {
  Led_config_t *c = NULL;
  switch(led) {
    case LEDS_POWER:
      c = &config.power;
      break;
    case LEDS_NETWORK:
      c = &config.network;
      break;
    case LEDS_PLAYER:
      c = &config.player;
      break;
    default:
      break;
  }
  return c;
}

static void write(Leds_t led, uint_fast8_t state) {
  Led_config_t *c = get_led_config(led);
  if (!c) {
    return;
  }
  HAL_GPIO_WritePin(c->port, c->pin, state == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
