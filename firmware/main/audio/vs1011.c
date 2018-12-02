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

#define VS_SO_GPIO 19
#define VS_SI_GPIO 23
#define VS_SCLK_GPIO 18
#define VS_XRESET_GPIO 21
#define VS_DREQ_GPIO 22
#define VS_XCS_GPIO 5
#define VS_XDCS_GPIO 17
#define VS_XTAL_FREQ 12000000 // 12288000
#define VS_MAX_CHUNK_SIZE 32

static spi_device_handle_t data_spi;
static spi_device_handle_t command_spi;

static void reset();
static void write_sci(uint8_t addr, uint16_t data);
static uint16_t read_sci(uint8_t addr);
static void write_sdi(const uint8_t *buffer, size_t length);
static bool dreq();
static void wait_for_dreq();
static void bus_init();
static bool codec_init();

void vs1011_play(FILE *fp) {

}

bool vs1011_init() {
  bus_init();
  return codec_init();
}

static void reset() {
  gpio_set_level(VS_XRESET_GPIO, 0);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  gpio_set_level(VS_XRESET_GPIO, 1);
}

static void write_sci(uint8_t addr, uint16_t data) {
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
  t.cmd = 2;
  t.addr = addr;
  t.length = sizeof(data) * 8;
  t.tx_data[0] = data >> 8;
  t.tx_data[1] = data & 0xFF;
  wait_for_dreq();
  ESP_ERROR_CHECK(spi_device_polling_transmit(command_spi, &t));
}

static uint16_t read_sci(uint8_t addr) {
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
  t.cmd = 3;
  t.addr = addr;
  t.length = sizeof(uint16_t) * 8;
  wait_for_dreq();
  ESP_ERROR_CHECK(spi_device_polling_transmit(command_spi, &t));
  uint16_t result = (t.rx_data[0] << 8) | t.rx_data[1];
  return result;
}

static void write_sdi(const uint8_t *buffer, size_t length) {
  if (length > VS_MAX_CHUNK_SIZE) {
    return;
  }
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.length = length;
  t.tx_buffer = buffer;
  wait_for_dreq();
  ESP_ERROR_CHECK(spi_device_polling_transmit(data_spi, &t));
}

static bool dreq() {
  return gpio_get_level(VS_DREQ_GPIO) == 1;
}

static void wait_for_dreq() {
  while(!dreq()) {
    taskYIELD();
  }
}

static void bus_init() {
  gpio_set_direction(VS_XRESET_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(VS_DREQ_GPIO, GPIO_MODE_INPUT);

  reset();

  spi_bus_config_t bus_cfg = {
    .miso_io_num = VS_SO_GPIO,
    .mosi_io_num = VS_SI_GPIO,
    .sclk_io_num = VS_SCLK_GPIO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = VS_MAX_CHUNK_SIZE
  };
  ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &bus_cfg, 1));

  spi_device_interface_config_t data_cfg = {
    .command_bits = 0,
    .address_bits = 0,
    .clock_speed_hz = 6000000,
    .mode = 0,
    .spics_io_num = VS_XDCS_GPIO,
    .queue_size = 7,
  };
  ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &data_cfg, &data_spi));

  spi_device_interface_config_t command_cfg = {
    .command_bits = 8,
    .address_bits = 8,
    .clock_speed_hz = 1000000,
    .mode = 0,
    .spics_io_num = VS_XCS_GPIO,
    .queue_size = 1,
  };
  ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &command_cfg, &command_spi));
}

static bool codec_init() {
  /* Start initialization with a dummy read, which makes sure our
     microcontoller chips selects and everything are where they
     are supposed to be and that VS10xx's SCI bus is in a known state. */
  read_sci(SCI_MODE);

  /* First real operation is a software reset. After the software
     reset we know what the status of the IC is. You need, depending
     on your application, either set or not set SM_SDISHARE. See the
     Datasheet for details. */
  write_sci(SCI_MODE, SM_SDINEW | SM_SDISHARE | SM_TESTS | SM_RESET);

  /* A quick sanity check: write to two registers, then test if we
   get the same results. Note that if you use a too high SPI
   speed, the MSB is the most likely to fail when read again. */
  write_sci(SCI_AICTRL1, 0xABAD);
  write_sci(SCI_AICTRL2, 0x7E57);
  if (read_sci(SCI_AICTRL1) != 0xABAD || read_sci(SCI_AICTRL2) != 0x7E57) {
    ESP_LOGE(TAG, "VS1011 SPI communication failure");
    return false;
  }
  write_sci(SCI_AICTRL1, 0);
  write_sci(SCI_AICTRL2, 0);

  /* Note: code SS_VER=2 is used for both VS1002 and VS1011e */
  const uint16_t CHIP_NUMBER[16] = {
    1001, 1011, 1011, 1003, 1053, 1033, 1063, 1103,
    0, 0, 0, 0, 0, 0, 0, 0
  };
  /* Check VS10xx type */
  uint16_t chip_id = (read_sci(SCI_STATUS) >> 4) & 0x0F;
  if (CHIP_NUMBER[chip_id]) {
    ESP_LOGI(TAG, "chip version: %d", CHIP_NUMBER[chip_id]);
    if (CHIP_NUMBER[chip_id] != 1011) {
      ESP_LOGE(TAG, "incorrect chip");
      return false;
    }
  } else {
    ESP_LOGE(TAG, "unknown VS10xx SCI_MODE field SS_VER = %d", chip_id);
    return false;
  }

  /* Set the clock. Until this point we need to run SPI slow so that
   we do not exceed the maximum speeds mentioned in
   Chapter SPI Timing Diagram in the Datasheet. */
  write_sci(SCI_CLOCKF, HZ_TO_SCI_CLOCKF(VS_XTAL_FREQ));

  /* Set volume level at -6 dB of maximum */
  write_sci(SCI_VOL, 0x0C0C);

  ESP_LOGI(TAG, "initialization sequence completed");

  return true;
}
