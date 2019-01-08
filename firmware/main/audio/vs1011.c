#include "audio/vs1011.h"
#include "audio/vs10xx_uc.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
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
#define VS_MUTE_GPIO 16
#define VS_XTAL_FREQ 12000000 // 12288000
#define VS_MAX_CHUNK_SIZE 32

typedef enum {
  af_unknown,
  af_riff,
  af_mp1,
  af_mp2,
  af_mp3,
} audio_format_t;

static spi_device_handle_t data_spi;
static spi_device_handle_t command_spi;
static SemaphoreHandle_t mutex = NULL;
static SemaphoreHandle_t dreq_sema = NULL;

static void reset();
static void write_sci(uint8_t addr, uint16_t data);
static uint16_t read_sci(uint8_t addr);
static void write_sdi(const uint8_t *buffer, size_t length);
static void wait_for_dreq();
static void bus_init();
static bool codec_init();
static void IRAM_ATTR dreq_isr(void *arg);
static audio_format_t audio_format();

void vs1011_play(size_t (*read_func)(uint8_t *buffer, size_t buffer_size, void *ctx),
                 size_t total_size,
                 void *ctx,
                 void (*callback)(audio_info_t ai)
               ) {
  if (read_func == NULL) {
    return;
  }

  size_t bytes_in_buffer = 0;
  uint32_t pos = 0;
  static uint8_t buffer[VS1011_BUFFER_SIZE] = { 0 };
  memset(buffer, 0, sizeof(buffer));

  write_sci(SCI_DECODE_TIME, 0); // Reset DECODE_TIME
  write_sci(SCI_DECODE_TIME, 0); // twice according to datasheet

  if (callback != NULL) {
    audio_info_t ai = {
      .total = total_size,
      .position = pos,
      .decode_time = read_sci(SCI_DECODE_TIME),
    };
    callback(ai);
  }

  write_sdi(buffer, 2); // according to faq: Send at least one (preferably two) byte containing zero to SDI.

  vs1011_transient_mute(false);

  while ((bytes_in_buffer = read_func(buffer, sizeof(buffer), ctx)) > 0) {
    uint8_t *buf_play = buffer;
    while (bytes_in_buffer) {
      size_t i = MIN(VS_MAX_CHUNK_SIZE, bytes_in_buffer);
      write_sdi(buf_play, i);
      buf_play += i;
      bytes_in_buffer -= i;
      pos += i;
    }

    if (callback != NULL) {
      audio_info_t ai = {
        .total = total_size,
        .position = pos,
        .decode_time = read_sci(SCI_DECODE_TIME),
      };
      callback(ai);
    }

    //uint16_t sample_rate = read_sci(SCI_AUDATA);
    //uint16_t h1 = read_sci(SCI_HDAT1); // format
    //uint16_t h0 = read_sci(SCI_HDAT0); // format
    //printf("%uKiB %1ds %d    H0: 0x%X H1: 0x%X\n", pos / 1024, read_sci(SCI_DECODE_TIME), sample_rate, h0, h1);
  }
  vs1011_transient_mute(true);

  audio_format_t af = audio_format();
  if (af != af_mp3 && af != af_unknown) {
    write_sci(SCI_MODE, read_sci(SCI_MODE) | SM_OUTOFWAV);
    memset(buffer, 0, sizeof(buffer));
    for(uint32_t i = 0; i < 512; i++) {
      write_sdi(buffer, VS_MAX_CHUNK_SIZE);
      if (!(read_sci(SCI_MODE) & SM_OUTOFWAV)) {
        break;
      }
    }
  }

  memset(buffer, 0, sizeof(buffer));
  for(uint32_t i = 0; i < 64; i++) {
    write_sdi(buffer, VS_MAX_CHUNK_SIZE);
  }

  if (callback != NULL) {
    audio_info_t ai = {
      .total = 0,
      .position = 0,
      .decode_time = 0,
    };
    callback(ai);
  }

  /* If SM_OUTOFWAV is on at this point, there is some weirdness going
   on. Reset the IC just in case. */
  if (read_sci(SCI_MODE) & SM_OUTOFWAV) {
    codec_init();
  }
}

void vs1011_transient_mute(bool mute) {
  gpio_set_level(VS_MUTE_GPIO, mute ? 0 : 1);
}

bool vs1011_init() {
  mutex = xSemaphoreCreateMutex();
  if (mutex == NULL) {
    ESP_LOGE(TAG, "cannot create mutex");
    return false;
  }
  dreq_sema = xSemaphoreCreateBinary();
  if (dreq_sema == NULL) {
    ESP_LOGE(TAG, "cannot create semaphore");
    return false;
  }
  bus_init();
  return codec_init();
}

uint16_t vs1011_set_volume(uint8_t percents) {
  if(percents > 100) {
    percents = 100;
  }
  uint8_t scaled_percents = 50 * percents / 100 + 50; // scale: 50 - min, 100 - max; vs1011 has weirg volume range
  if(scaled_percents > 100) { // just in case
    scaled_percents = 100;
  }

  // 0 max, 254 min
  uint16_t value = 254 - scaled_percents * 254 / 100;
  uint16_t reg = value + value * 256;
  write_sci(SCI_VOL, reg);
  return reg;
}

uint16_t vs1011_set_bass_freqlimit(uint8_t value) { // 2-15
  if (value < 2) {
    value = 2;
  }
  if (value > 15) {
    value = 15;
  }
  uint16_t current = read_sci(SCI_BASS);
  uint16_t reg = (current & ~SB_FREQLIMIT_MASK) | ((value << SB_FREQLIMIT_B) & SB_FREQLIMIT_MASK);
  write_sci(SCI_BASS, reg);
  return reg;
}

uint16_t vs1011_set_bass_amplitude(uint8_t value) {  // 0-15, 0=off
  if (value > 15) {
    value = 15;
  }
  uint16_t current = read_sci(SCI_BASS);
  uint16_t reg = (current & ~SB_AMPLITUDE_MASK) | ((value << SB_AMPLITUDE_B) & SB_AMPLITUDE_MASK);
  write_sci(SCI_BASS, reg);
  return reg;
}

uint16_t vs1011_set_treble_freqlimit(uint8_t value) { // 0-15
  if (value > 15) {
    value = 15;
  }
  uint16_t current = read_sci(SCI_BASS);
  uint16_t reg = (current & ~ST_FREQLIMIT_MASK) | ((value << ST_FREQLIMIT_B) & ST_FREQLIMIT_MASK);
  write_sci(SCI_BASS, reg);
  return reg;
}

uint16_t vs1011_set_treble_amplitude(int8_t value) { // -8-7, 0=off
  if (value < -8) {
    value = -8;
  }
  if (value > 7) {
    value = 7;
  }
  uint16_t current = read_sci(SCI_BASS);
  uint16_t reg = (current & ~ST_AMPLITUDE_MASK) | ((value << ST_AMPLITUDE_B) & ST_AMPLITUDE_MASK);
  write_sci(SCI_BASS, reg);
  return reg;
}

static void reset() {
  gpio_set_level(VS_XRESET_GPIO, 0);
  taskYIELD();
  gpio_set_level(VS_XRESET_GPIO, 1);
}

static void write_sci(uint8_t addr, uint16_t data) {
  xSemaphoreTake(mutex, portMAX_DELAY);
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
  wait_for_dreq(); // once dreq is high again we're done
  xSemaphoreGive(mutex);
}

static uint16_t read_sci(uint8_t addr) {
  xSemaphoreTake(mutex, portMAX_DELAY);
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
  t.cmd = 3;
  t.addr = addr;
  t.length = sizeof(uint16_t) * 8;
  wait_for_dreq();
  ESP_ERROR_CHECK(spi_device_transmit(command_spi, &t));
  uint16_t result = (t.rx_data[0] << 8) | t.rx_data[1];
  xSemaphoreGive(mutex);
  return result;
}

static void write_sdi(const uint8_t *buffer, size_t length) {
  if (length > VS_MAX_CHUNK_SIZE) {
    return;
  }
  xSemaphoreTake(mutex, portMAX_DELAY);
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.length = length * 8;
  t.tx_buffer = buffer;
  wait_for_dreq();
  ESP_ERROR_CHECK(spi_device_transmit(data_spi, &t));
  wait_for_dreq(); // once dreq is high again we're done
  xSemaphoreGive(mutex);
}

static audio_format_t audio_format() {
  uint16_t h1 = read_sci(SCI_HDAT1);
  if (h1 == 0x7665) {
    return af_riff;
  } else if ((h1 & 0xFFE6) == 0xFFE2) {
    return af_mp3;
  } else if ((h1 & 0xFFE6) == 0xFFE4) {
    return af_mp2;
  } else if ((h1 & 0xFFE6) == 0xFFE6) {
    return af_mp1;
  }
  return af_unknown;
}

static void wait_for_dreq() {
  if (gpio_get_level(VS_DREQ_GPIO) == 1) {
    return;
  }
  xSemaphoreTake(dreq_sema, portMAX_DELAY);
}

static void IRAM_ATTR dreq_isr(void *_arg) {
  BaseType_t hpt;
  xSemaphoreGiveFromISR(dreq_sema, &hpt);
  if (hpt == pdTRUE) {
    portYIELD_FROM_ISR();
  }
}

static void bus_init() {
  gpio_set_direction(VS_XRESET_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_direction(VS_MUTE_GPIO, GPIO_MODE_OUTPUT);

  gpio_set_direction(VS_DREQ_GPIO, GPIO_MODE_INPUT);
  gpio_intr_enable(VS_DREQ_GPIO);
  gpio_set_intr_type(VS_DREQ_GPIO, GPIO_PIN_INTR_POSEDGE);
  gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
  gpio_isr_handler_add(VS_DREQ_GPIO, dreq_isr, (void *)VS_DREQ_GPIO);


  vs1011_transient_mute(true);

  reset();

  spi_bus_config_t bus_cfg = {
    .miso_io_num = VS_SO_GPIO,
    .mosi_io_num = VS_SI_GPIO,
    .sclk_io_num = VS_SCLK_GPIO,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .flags = SPICOMMON_BUSFLAG_MASTER,
    .max_transfer_sz = VS_MAX_CHUNK_SIZE
  };
  ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &bus_cfg, 1));

  spi_device_interface_config_t data_cfg = {
    .command_bits = 0,
    .address_bits = 0,
    .clock_speed_hz = 4000000,
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
  write_sci(SCI_MODE, SM_SDINEW | SM_RESET); // SM_TESTS // SM_DACT

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

  ESP_LOGI(TAG, "initialization sequence completed");

  return true;
}
