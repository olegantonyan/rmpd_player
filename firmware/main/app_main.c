#include "wifi/wifi.h"
#include "web/web.h"
#include "storage/spi.h"
#include "storage/sd.h"
#include "audio/scheduler.h"
#include "clock/clock.h"

void app_main() {
  wifi_init();
  clock_init();
  spiffs_init();
  sd_init();
  web_init();
  scheduler_init();
}
