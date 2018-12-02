#include "audio/vs1011.h"
#include "audio/vs10xx_uc.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "vs1011";

static spi_device_handle_t data_spi;
static spi_device_handle_t command_spi;

static void reset();
static void write_sci(uint8_t addr, uint16_t data);
static uint16_t read_sci(uint8_t addr);
static bool dreq();

bool vs1011_init() {
  return true;
  ESP_LOGI(TAG, "initialize vs1011");

  gpio_set_direction(21, GPIO_MODE_OUTPUT);
  gpio_set_direction(22, GPIO_MODE_INPUT);

  ESP_LOGI(TAG, "reset vs1011");

  reset();

  spi_bus_config_t bus_cfg = {
    .miso_io_num=19,
    .mosi_io_num=23,
    .sclk_io_num=18,
    .quadwp_io_num=-1,
    .quadhd_io_num=-1,
    .max_transfer_sz=32
  };

  spi_device_interface_config_t data_cfg = {
    .command_bits = 8,
    .address_bits = 8,
    .clock_speed_hz=4000,
    .mode=0,
    .spics_io_num=17,
    .queue_size=7,
  };

  spi_device_interface_config_t command_cfg = {
    .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
    .command_bits = 8,
    .address_bits = 8,
    .clock_speed_hz=4000,
    .mode=0,
    .spics_io_num=7,
    .queue_size=1,
  };

  ESP_LOGI(TAG, "initialize spi bus");

  ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &bus_cfg, 1));
  //ESP_LOGI(TAG, "done initialize spi bus");
  //ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &data_cfg, &data_spi));
  //ESP_LOGI(TAG, "done add spi device 1");
  ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &command_cfg, &command_spi));
  //ESP_LOGI(TAG, "done add spi device 2");

  read_sci(SCI_MODE);
  write_sci(SCI_MODE, SM_SDINEW|SM_SDISHARE|SM_TESTS|SM_RESET);

  write_sci(SCI_AICTRL1, 0xABAD);
  write_sci(SCI_AICTRL2, 0x7E57);
  if (read_sci(SCI_AICTRL1) != 0xABAD || read_sci(SCI_AICTRL2) != 0x7E57) {
    ESP_LOGI(TAG, "oops :( vs1011");
  }
  ESP_LOGI(TAG, "done vs1011");
  return true;
}

static void reset() {
  gpio_set_level(21, 0);
  vTaskDelay(10);
  gpio_set_level(21, 1);
}

static void write_sci(uint8_t addr, uint16_t data) {
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.cmd = 2;
  t.addr = addr;
  t.length = sizeof(data);
  t.tx_data[0] = data >> 8;
  t.tx_data[1] = data & 0xFF;
  while(!dreq());
  ESP_ERROR_CHECK(spi_device_polling_transmit(command_spi, &t));
}

static uint16_t read_sci(uint8_t addr) {
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.cmd = 3;
  t.addr = addr;
  t.length = sizeof(uint16_t);
  while(!dreq());
  ESP_ERROR_CHECK(spi_device_polling_transmit(command_spi, &t));
  return t.rx_data[0] << 8 | t.tx_data[1];
}

static bool dreq() {
  return gpio_get_level(22) == 1;
}
