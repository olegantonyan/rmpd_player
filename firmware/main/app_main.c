#include "wifi/wifi.h"
#include "web/web.h"
#include "storage/spi.h"
#include "storage/sd.h"
#include "playlist/scheduler.h"
#include "clock/clock.h"



/*
#include "util/http.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
void tread(void * args) {
  wifi_wait_connected(portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(1000));
  while (true) {
    static uint8_t buf[4096] = { 0 };
    size_t len = 0;

    int s = http_get("https://server.slon-ds.ru/", buf, sizeof(buf), &len);
    printf("status:%d\n%s\n", s, buf);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
*/

void app_main() {
  spiffs_init();
  wifi_init();
  clock_init();
  sd_init();
  web_init();
  scheduler_init();
  //xTaskCreate(tread, "ololo", 10000, NULL, 5, NULL);
}
