#include "clock/ds3231.h"
#include "esp_log.h"
#include "driver/i2c.h"

#define SCL_GPIO 26
#define SDA_GPIO 27
#define I2C_MASTER_NUM  0

static const char *TAG = "ds3231";

static uint8_t bcd2dec(uint8_t val);
static uint8_t dec2bcd(uint8_t val);

bool ds3231_init() {

}

time_t ds3231_get_time() {

}

bool ds3231_set_time(time_t time) {

}

static uint8_t bcd2dec(uint8_t val) {
  return (val >> 4) * 10 + (val & 0x0f);
}

static uint8_t dec2bcd(uint8_t val) {
  return ((val / 10) << 4) + (val % 10);
}
