#include "wifi/wifi.h"
#include "web/web.h"
#include "clock/ntp.h"
#include "fs/spi.h"

void app_main() {
  //Initialize NVS
  /*esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  */
  spiffs_init();

  wifi_init();
  ntp_init();
  web_init();
}
