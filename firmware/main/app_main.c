#include "wifi/wifi.h"
#include "web/web.h"
#include "clock/ntp.h"
#include "storage/spi.h"
#include "storage/sd.h"
#include "audio/scheduler.h"
#include "clock/ds3231.h"

void app_main() {
  ds3231_init();
  spiffs_init();
  sd_init();
  wifi_init();
  ntp_init();
  web_init();
  scheduler_start();
}
