#include "audio/vs1011.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "storage/sd.h"

static const char *TAG = "player";

bool player_init() {
  bool vs_ok = vs1011_init();
  if (!vs_ok) {
    ESP_LOGE(TAG, "cannot initialize audio driver");
    return false;
  }



  ESP_LOGI(TAG, "plaing file");
  FILE *f = fopen(STORAGE_SD_MOUNTPOINT   "/Yamaha-V50-Synbass-1-C2.wav", "rb");
  if (f == NULL) {
    ESP_LOGE(TAG, "failed to open file for reading");
    return false;
  }

  vs1011_play(f);
  ESP_LOGI(TAG, "done plaing file");

  fclose(f);


  return true;
}
