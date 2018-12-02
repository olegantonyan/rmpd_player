#include "audio/vs1011.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "player";

bool player_init() {
  bool vs_ok = vs1011_init();
  if (!vs_ok) {
    ESP_LOGE(TAG, "cannot initialize audio driver");
  }

  return true;
}
