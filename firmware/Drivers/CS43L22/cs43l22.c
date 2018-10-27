#include <stdio.h>
#include "cs43l22.h"

typedef enum {
  ID                 = 0x01,
  POWER_CTL1         = 0x02,
  POWER_CTL2         = 0x04,
  CLOCKING_CTL       = 0x05,
  INTERFACE_CTL1     = 0x06,
  INTERFACE_CTL2     = 0x07,
  PASSTHR_A_SELECT   = 0x08,
  PASSTHR_B_SELECT   = 0x09,
  ANALOG_ZC_SR_SETT  = 0x0A,
  PASSTHR_GANG_CTL   = 0x0C,
  PLAYBACK_CTL1      = 0x0D,
  MISC_CTL           = 0x0E,
  PLAYBACK_CTL2      = 0x0F,
  PASSTHR_A_VOL      = 0x14,
  PASSTHR_B_VOL      = 0x15,
  PCMA_VOL           = 0x1A,
  PCMB_VOL           = 0x1B,
  BEEP_FREQ_ON_TIME  = 0x1C,
  BEEP_VOL_OFF_TIME  = 0x1D,
  BEEP_TONE_CFG      = 0x1E,
  TONE_CTL           = 0x1F,
  MASTER_A_VOL       = 0x20,
  MASTER_B_VOL       = 0x21,
  HEADPHONE_A_VOL    = 0x22,
  HEADPHONE_B_VOL    = 0x23,
  SPEAKER_A_VOL      = 0x24,
  SPEAKER_B_VOL      = 0x25,
  CH_MIXER_SWAP      = 0x26,
  LIMIT_CTL1         = 0x27,
  LIMIT_CTL2         = 0x28,
  LIMIT_ATTACK_RATE  = 0x29,
  OVF_CLK_STATUS     = 0x2E,
  BATT_COMPENSATION  = 0x2F,
  VP_BATTERY_LEVEL   = 0x30,
  SPEAKER_STATUS     = 0x31,
  TEMPMONITOR_CTL    = 0x32,
  THERMAL_FOLDBACK   = 0x33,
  CHARGE_PUMP_FREQ   = 0x34
} CS43L22_REGISTER;

static CS43L22 config;
static bool initialized = false;

static void reset_cycle();
static void init_sequence();
static bool write_register(CS43L22_REGISTER reg, uint8_t data);
static uint8_t read_register(CS43L22_REGISTER reg);
static uint8_t volume_convert(uint8_t percents);

bool cs43l22_init(CS43L22 cs43l22) {
  if (initialized) {
    return false;
  }
  config = cs43l22;

  reset_cycle();
  init_sequence();


#if DEBUG
  printf("CS43L22 chip id/rev: 0x%X\n", read_register(ID));
#endif

  // beep
  write_register(BEEP_TONE_CFG, 0xC0);
  write_register(BEEP_FREQ_ON_TIME, 0x1C);

  cs43l22_set_master_volume(60);

  /*while(true) {
    uint16_t Istr[1];
    Istr[0] = 0;

    HAL_I2S_Transmit (config.i2s, Istr , 0x10, 10 );


  }*/

  initialized = true;
  return true;
}

void cs43l22_set_master_volume(uint8_t percents) {
  write_register(MASTER_A_VOL, volume_convert(percents));
  write_register(MASTER_B_VOL, volume_convert(percents));
}

#define AUDIODATA_SIZE                  2   /* 16-bits audio data size */
#define DMA_MAX_SZE                     0xFFFF
#define DMA_MAX(_X_)                (((_X_) <= DMA_MAX_SZE)? (_X_):DMA_MAX_SZE)
void cs43l22_play(uint16_t *buffer, size_t size) {
  //HAL_I2S_Transmit(config.i2s, buffer, size, 10);
  HAL_I2S_Transmit_DMA(config.i2s, buffer, DMA_MAX(size / AUDIODATA_SIZE));
}

// private

static uint8_t volume_convert(uint8_t percents) {
  uint8_t converted = 0;
  if (percents > 100) {
    converted = 255;
  } else {
    converted = percents * 255 / 100;
  }
  if (converted > 0xE6) {
    return converted - 0xE7;
  } else {
    return converted + 0x19;
  }
}

static void reset_cycle() {
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_RESET);
//  HAL_Delay(5);
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_SET);
}

static void init_sequence() {
  write_register(0x00, 0x99);               // requred init sequence
  write_register(0x47, 0x80);
  write_register(0x32, 0xBB);
  write_register(0x32, 0x3B);
  write_register(0x00, 0x00);

  write_register(POWER_CTL1, 0x9E);
  write_register(POWER_CTL2, 0xAA);         // output both
  write_register(CLOCKING_CTL, 0x81);
  write_register(INTERFACE_CTL1, 0x04);     // set the slave mode and the audio standard
  write_register(ANALOG_ZC_SR_SETT, 0x00);
  //write_register(MISC_CTL, 0x04);           // disable the digital soft ramp
  write_register(LIMIT_CTL1, 0x00);         // disable the limiter attack level
  write_register(TONE_CTL, 0b10001000);           // adjust bass and treble levels
  write_register(PCMA_VOL, 0x0A);           // adjust PCM volume level
  write_register(PCMB_VOL, 0x0A);
}

static bool write_register(CS43L22_REGISTER reg, uint8_t data) {
  return HAL_I2C_Mem_Write(config.i2c, config.address, reg, I2C_MEMADD_SIZE_8BIT, &data, sizeof(data), 10) == HAL_OK;
}

static uint8_t read_register(CS43L22_REGISTER reg) {
  uint8_t buf = 0x00;
  HAL_I2C_Mem_Read(config.i2c, config.address, reg, I2C_MEMADD_SIZE_8BIT, &buf, sizeof(buf), 10);
  return buf;
}
