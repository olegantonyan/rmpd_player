#include "wifi/wifi.h"
#include "web/web.h"
#include "storage/spi.h"
#include "storage/sd.h"
#include "playlist/scheduler.h"
#include "clock/clock.h"
#include "remote/control.h"

void app_main() {
  spiffs_init();
  wifi_init();
  clock_init();
  sd_init();
  web_init();
  scheduler_init();

  remote_control_start(); // should never return 
}
