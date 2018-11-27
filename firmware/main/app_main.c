#include "wifi/wifi.h"
#include "web/web.h"
#include "clock/ntp.h"
#include "storage/spi.h"

void app_main() {
  spiffs_init();
  wifi_init();
  ntp_init();
  web_init();
}
