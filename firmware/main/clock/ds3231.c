#include "clock/ds3231.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include <string.h>
#include <sys/time.h>
#include "clock/clock.h"

#define SCL_GPIO 26
#define SDA_GPIO 27
#define I2C_MASTER_NUM  0
#define DS3231_ADDRESS 0x68
#define DS3231_STAT_OSCILLATOR 0x80
#define DS3231_STAT_32KHZ      0x08
#define DS3231_STAT_BUSY       0x04
#define DS3231_STAT_ALARM_2    0x02
#define DS3231_STAT_ALARM_1    0x01
#define DS3231_CTRL_OSCILLATOR    0x80
#define DS3231_CTRL_SQUAREWAVE_BB 0x40
#define DS3231_CTRL_TEMPCONV      0x20
#define DS3231_CTRL_ALARM_INTS    0x04
#define DS3231_CTRL_ALARM2_INT    0x02
#define DS3231_CTRL_ALARM1_INT    0x01
#define DS3231_ALARM_WDAY   0x40
#define DS3231_ALARM_NOTSET 0x80
#define DS3231_ADDR_TIME    0x00
#define DS3231_ADDR_ALARM1  0x07
#define DS3231_ADDR_ALARM2  0x0B
#define DS3231_ADDR_CONTROL 0x0E
#define DS3231_ADDR_STATUS  0x0F
#define DS3231_ADDR_AGING   0x10
#define DS3231_ADDR_TEMP    0x11
#define DS3231_12HOUR_FLAG  0x40
#define DS3231_12HOUR_MASK  0x1F
#define DS3231_PM_FLAG      0x20
#define DS3231_MONTH_MASK   0x1F

static const char *TAG = "ds3231";

static uint8_t bcd2dec(uint8_t val);
static uint8_t dec2bcd(uint8_t val);
static bool i2c_read(uint8_t reg, uint8_t *buffer, size_t size);
static bool i2c_write(uint8_t reg, uint8_t *data, size_t size);
static void set_system_time(time_t secs);
static void start_condition();

static SemaphoreHandle_t mutex = NULL;

bool ds3231_init() {
  start_condition();

  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = SDA_GPIO;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num = SCL_GPIO;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = 100000;
  i2c_param_config(I2C_MASTER_NUM, &conf);
  esp_err_t ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "i2c master driver init error: %s", esp_err_to_name(ret));
    return false;
  }
  mutex = xSemaphoreCreateMutex();
  if (mutex == NULL) {
    ESP_LOGE(TAG, "cannot create mutex");
    return false;
  }
  time_t now = ds3231_get_time();
  if (now > 30) {
    ESP_LOGI(TAG, "set hw clock to %s", ctime(&now));
    set_system_time(now);
  }
  return true;
}

time_t ds3231_get_time() {
  struct tm timeinfo = { 0 };
  uint8_t data[7] = { 0 };

  if (!i2c_read(DS3231_ADDR_TIME, data, sizeof(data))) {
    return 0;
  }

  timeinfo.tm_sec = bcd2dec(data[0]);
  timeinfo.tm_min = bcd2dec(data[1]);
  if (data[2] & DS3231_12HOUR_FLAG) {
    timeinfo.tm_hour = bcd2dec(data[2] & DS3231_12HOUR_MASK) - 1  ;
    if (data[2] & DS3231_PM_FLAG) {
      timeinfo.tm_hour += 12;
    }
  } else {
    timeinfo.tm_hour = bcd2dec(data[2]);
  }
  timeinfo.tm_wday = bcd2dec(data[3]) - 1;
  timeinfo.tm_mday = bcd2dec(data[4]);
  timeinfo.tm_mon  = bcd2dec(data[5] & DS3231_MONTH_MASK) - 1;
  timeinfo.tm_year = bcd2dec(data[6]) + 100;
  timeinfo.tm_isdst = -1;

  return mktime(&timeinfo);
}

bool ds3231_set_time(struct tm *timeinfo) {
  uint8_t data[7] = { 0 };
  data[0] = dec2bcd(timeinfo->tm_sec);
  data[1] = dec2bcd(timeinfo->tm_min);
  data[2] = dec2bcd(timeinfo->tm_hour);
  data[3] = dec2bcd(timeinfo->tm_wday + 1);
  data[4] = dec2bcd(timeinfo->tm_mday);
  data[5] = dec2bcd(timeinfo->tm_mon + 1);
  data[6] = dec2bcd(timeinfo->tm_year - 100);

  return i2c_write(DS3231_ADDR_TIME, data, sizeof(data));
}

static uint8_t bcd2dec(uint8_t val) {
  return (val >> 4) * 10 + (val & 0x0F);
}

static uint8_t dec2bcd(uint8_t val) {
  return ((val / 10) << 4) + (val % 10);
}

static bool i2c_read(uint8_t reg, uint8_t *buffer, size_t size) {
  if (size == 0 || !buffer) {
    return false;
  }

  xSemaphoreTake(mutex, portMAX_DELAY);
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (DS3231_ADDRESS << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
  i2c_master_write_byte(cmd, reg, I2C_MASTER_ACK);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (DS3231_ADDRESS << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
  i2c_master_read(cmd, buffer, size, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));

  i2c_cmd_link_delete(cmd);
  xSemaphoreGive(mutex);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "i2c master read error: %s", esp_err_to_name(ret));
    return false;
  }
  return true;
}

static bool i2c_write(uint8_t reg, uint8_t *data, size_t size) {
  if (size == 0 || !data) {
    return false;
  }

  xSemaphoreTake(mutex, portMAX_DELAY);
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (DS3231_ADDRESS << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
  i2c_master_write_byte(cmd, reg, I2C_MASTER_ACK);
  i2c_master_write(cmd, data, size, I2C_MASTER_ACK);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));

  i2c_cmd_link_delete(cmd);
  xSemaphoreGive(mutex);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "i2c master write error: %s", esp_err_to_name(ret));
    return false;
  }
  return true;
}

static void set_system_time(time_t secs) {
  struct timeval tv = { .tv_sec = secs, .tv_usec = 0 };
  settimeofday(&tv, NULL);
}

static void start_condition() {
  gpio_set_direction(SCL_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(SDA_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_level(SDA_GPIO, 0);
  gpio_set_level(SCL_GPIO, 1);
  vTaskDelay(pdMS_TO_TICKS(5));
}
