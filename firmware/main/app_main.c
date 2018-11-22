#include "wifi/wifi.h"
#include "web/web.h"
#include "clock/ntp.h"
#include "storage/spi.h"
#include "storage/nvs.h"

void app_main() {
  nvs_init();
  spiffs_init();

  wifi_init();
  ntp_init();
  web_init();
}
