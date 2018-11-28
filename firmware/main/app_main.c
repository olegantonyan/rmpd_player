#include "wifi/wifi.h"
#include "web/web.h"
#include "clock/ntp.h"
#include "storage/spi.h"
#include "storage/sd.h"

void app_main() {
  spiffs_init();
  sd_init();
  wifi_init();
  ntp_init();
  web_init();
}
