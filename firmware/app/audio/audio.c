#include "cmsis_os.h"
#include <stdio.h>

#include "audio/audio.h"
#include "cs43l22.h"
#include "rng/rng.h"
#include "mp3dec.h"

extern const char mp3_data[];
#define MP3_SIZE	687348

static AudioConfig config;
static osThreadId thread_handle;
static osSemaphoreId sema;

static void thread_task(void const * args);
static bool codec_write_register(uint8_t reg, uint8_t data);
static uint8_t codec_read_register(uint8_t reg);
static void codec_reset();

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

  return true;
}

void audio_set_volume(uint8_t percents) {
  cs43l22_set_master_volume(percents);
}

void audio_transfer_complete_callback() {
  //printf("interrupt...\n");
  osSemaphoreRelease(sema);
}

bool audio_play(uint16_t *buffer, size_t size) {
  if(size > 0xFFFF) { // max dma size
    return false;
  }
  //int i = HAL_I2S_Transmit_DMA(config.i2s, buffer, size / 2); // 2 bytes per channel
  ///printf("hal %i\n", i);
  //return i == HAL_OK;
  return HAL_I2S_Transmit(config.i2s, buffer, size / 2, 100) == HAL_OK; // 2 bytes per channel
}

// private

static void thread_task(void const * args) {
  static uint16_t buffer[4096];

  for(size_t i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++) {
    buffer[i] = rng_get();
  }

  int offset, err;
	int outOfData = 0;
	static const char *read_ptr = mp3_data;
	static int bytes_left = MP3_SIZE;


  MP3FrameInfo mp3FrameInfo;
  HMP3Decoder hMP3Decoder = MP3InitDecoder();

  while(true) {

    uint16_t *samples = buffer;

    offset = MP3FindSyncWord((unsigned char*)read_ptr, bytes_left);
  	bytes_left -= offset;

  	if (bytes_left <= 10000) {
  		read_ptr = mp3_data;
  		bytes_left = MP3_SIZE;
  		offset = MP3FindSyncWord((unsigned char*)read_ptr, bytes_left);
  	}

  	read_ptr += offset;
  	err = MP3Decode(hMP3Decoder, (unsigned char**)&read_ptr, &bytes_left, samples, 0);

  	if (err) {
  		/* error occurred */
  		switch (err) {
  		case ERR_MP3_INDATA_UNDERFLOW:
  			outOfData = 1;
        printf("ERR_MP3_INDATA_UNDERFLOW\n");
  			break;
  		case ERR_MP3_MAINDATA_UNDERFLOW:
  			/* do nothing - next call to decode will provide more mainData */
        printf("ERR_MP3_MAINDATA_UNDERFLOW\n");
  			break;
  		case ERR_MP3_FREE_BITRATE_SYNC:
  		default:
  			outOfData = 1;
        printf("ERR_MP3_FREE_BITRATE_SYNC || default\n");
  			break;
  		}
  	} else {
  		/* no error */
  		MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
  	}

  	if (!outOfData) {
      //printf("1\n");

  		audio_play(samples, mp3FrameInfo.outputSamps);
            //printf("2\n");
      //osSemaphoreWait(sema, osWaitForever);
      //osDelay(10);

      //printf("3\n");
  	}

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
