#include "cmsis_os.h"
#include <stdio.h>

#include "audio/audio.h"
#include "cs43l22.h"
#include "rng/rng.h"

static AudioConfig config;
static osThreadId thread_handle;
static osSemaphoreId sema;

static void thread_task(void const * args);
static bool codec_write_register(uint8_t reg, uint8_t data);
static uint8_t codec_read_register(uint8_t reg);
static void codec_reset();

static uint16_t buf[2048] = { 0 }; // TODO remove

bool audio_init(AudioConfig cfg) {
  config = cfg;

  CS43L22 codec;
  codec.codec_write_register = codec_write_register;
  codec.codec_read_register = codec_read_register;
  codec.codec_reset = codec_reset;
  bool ok = cs43l22_init(codec);
  if(!ok) {
    printf("Error initializing audio codec\n");
    return false;
  }

  uint8_t codec_id = cs43l22_id();
  printf("CS43L22 chip id/rev: 0x%X\n", codec_id);
  if (codec_id != 0xE3) {
    printf("Error: wrong CS43L22 chip id\n");
    return false;
  }

  osSemaphoreDef(sema);
  sema = osSemaphoreCreate(osSemaphore(sema), 1);

  osThreadDef(thread_task, thread_task, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(thread_task), NULL);

  // TODO remove
  for (size_t i = 0 ; i < sizeof(buf) / 2; i++) {
    if (i % 2 != 0) {
      buf[i] = 0;
    } else {
      buf[i] = rng_get();
    }
  }
  //audio_play(buf, sizeof(buf) / 2);
  //

  return true;
}

void audio_set_volume(uint8_t percents) {
  cs43l22_set_master_volume(percents);
}

void audio_transfer_complete_callback() {
  osSemaphoreRelease(sema);
}

// private

static void thread_task(void const * args) {
  while(true) {
    osSemaphoreWait(sema, osWaitForever);
    audio_play(buf, sizeof(buf) / 2); // TODO remove
  }
}

static bool codec_write_register(uint8_t reg, uint8_t data) {
  return HAL_I2C_Mem_Write(config.i2c, config.address, reg, I2C_MEMADD_SIZE_8BIT, &data, sizeof(data), 10) == HAL_OK;
}

static uint8_t codec_read_register(uint8_t reg) {
  uint8_t buf = 0x00;
  HAL_I2C_Mem_Read(config.i2c, config.address, reg, I2C_MEMADD_SIZE_8BIT, &buf, sizeof(buf), 10);
  return buf;
}

static void codec_reset() {
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_SET);
}

bool audio_play(uint16_t *buffer, size_t size) {
  if(size > 0xFFFF) { // max dma size
    return false;
  }
  return HAL_I2S_Transmit_DMA(config.i2s, buffer, size / 2) == HAL_OK; // 2 bytes per channel
}
