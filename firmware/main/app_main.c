#include "wifi/wifi.h"
#include "web/web.h"
#include "storage/spi.h"
#include "storage/sd.h"
#include "playlist/scheduler.h"
#include "clock/clock.h"

void app_main() {
  spiffs_init();
  wifi_init();
  clock_init();
  sd_init();
  web_init();
  scheduler_init();
}
