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

#ifndef MIN
  #define MIN(a,b) (((a)<(b))?(a):(b))
#endif

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
static uint8_t dreq();
static void wait_for_dreq();
static void bus_init();
static bool codec_init();

void vs1011_play(FILE *fp) {

//  uint8_t test[8] = { 0x53, 0xEF, 0x63, 126, 0,0,0,0};
//  write_sdi(test, sizeof(test));
//  return;


  size_t bytes_in_buffer = 0;
  size_t pos = 0;
  static uint8_t file_buffer[2048] = { 0 };

//  write_sci(SCI_DECODE_TIME, 0);         // Reset DECODE_TIME

  write_sdi(file_buffer, 2); // according to faq: Send at least one (preferably two) byte containing zero to SDI.

  while ((bytes_in_buffer = fread(file_buffer, 1, sizeof(file_buffer), fp)) > 0) {
    uint8_t *buf_play = file_buffer;
    while (bytes_in_buffer) {
      size_t i = MIN(VS_MAX_CHUNK_SIZE, bytes_in_buffer);
      write_sdi(buf_play, i);
      buf_play += i;
      bytes_in_buffer -= i;
      pos += i;
    }

    uint16_t sample_rate = read_sci(SCI_AUDATA);;
    uint16_t h1 = read_sci(SCI_HDAT1); // format
    uint16_t h0 = read_sci(SCI_HDAT0); // format
    printf("%uKiB %1ds %d    H0: 0x%X H1: 0x%X\n", pos / 1024, read_sci(SCI_DECODE_TIME), sample_rate, h0, h1);
  }

  /* Earlier we collected endFillByte. Now, just in case the file was
   broken, or if a cancel playback command has been given, write
   lots of endFillBytes. */
  memset(file_buffer, 0, sizeof(file_buffer));
  for (size_t i = 0; i < sizeof(file_buffer); i += VS_MAX_CHUNK_SIZE) {
    write_sdi(file_buffer, VS_MAX_CHUNK_SIZE);
  }

  /* If SM_OUTOFWAV is on at this point, there is some weirdness going
   on. Reset the IC just in case. */
  if (read_sci(SCI_MODE) & SM_OUTOFWAV) {
    codec_init();
  }
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
  ESP_ERROR_CHECK(spi_device_transmit(command_spi, &t));
}

static uint16_t read_sci(uint8_t addr) {
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
  t.cmd = 3;
  t.addr = addr;
  t.length = sizeof(uint16_t) * 8;
  wait_for_dreq();
  ESP_ERROR_CHECK(spi_device_transmit(command_spi, &t));
  uint16_t result = (t.rx_data[0] << 8) | t.rx_data[1];
  return result;
}

static void write_sdi(const uint8_t *buffer, size_t length) {
  if (length > VS_MAX_CHUNK_SIZE) {
    return;
  }
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.length = length * 8;
  t.tx_buffer = buffer;
  wait_for_dreq();
  //gpio_set_level(VS_XDCS_GPIO, 0);
  ESP_ERROR_CHECK(spi_device_transmit(data_spi, &t));
  //gpio_set_level(VS_XDCS_GPIO, 1);
}

static uint8_t dreq() {
  return gpio_get_level(VS_DREQ_GPIO);
}

static void wait_for_dreq() {
  while(dreq() == 0) {
    taskYIELD();
  }
}

static void bus_init() {
  gpio_set_direction(VS_XRESET_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(VS_DREQ_GPIO, GPIO_MODE_INPUT);

//gpio_set_direction(VS_XDCS_GPIO, GPIO_MODE_OUTPUT);
//gpio_set_level(VS_XDCS_GPIO, 1);

  reset();

  spi_bus_config_t bus_cfg = {
    .miso_io_num = VS_SO_GPIO,
    .mosi_io_num = VS_SI_GPIO,
    .sclk_io_num = VS_SCLK_GPIO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .flags = SPICOMMON_BUSFLAG_MASTER,
    //.max_transfer_sz = VS_MAX_CHUNK_SIZE
  };
  ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &bus_cfg, 1));

  spi_device_interface_config_t data_cfg = {
    .command_bits = 0,
    .address_bits = 0,
    .clock_speed_hz = 2000000,
    .mode = 0,
    .spics_io_num = VS_XDCS_GPIO,
    .queue_size = 1,
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
  write_sci(SCI_MODE, SM_SDINEW | SM_TESTS | SM_RESET); // SM_TESTS // SM_DACT

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
//  write_sci(SCI_VOL, 0x0C0C);
//  write_sci(SCI_VOL, 0); // max vol

  ESP_LOGI(TAG, "initialization sequence completed");

  return true;
}
