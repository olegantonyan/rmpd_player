#include <stdio.h>
#include "cs43l22.h"

static CS43L22 config;
static bool initialized = false;

static void reset_cycle();
static void init_sequence();
static bool i2c_write_register(uint8_t reg, uint8_t data);
static uint8_t i2c_read_register(uint8_t reg);

/* AUDIO FREQUENCY */
#define AUDIO_FREQUENCY_192K          ((uint32_t)192000)
#define AUDIO_FREQUENCY_96K           ((uint32_t)96000)
#define AUDIO_FREQUENCY_48K           ((uint32_t)48000)
#define AUDIO_FREQUENCY_44K           ((uint32_t)44100)
#define AUDIO_FREQUENCY_32K           ((uint32_t)32000)
#define AUDIO_FREQUENCY_22K           ((uint32_t)22050)
#define AUDIO_FREQUENCY_16K           ((uint32_t)16000)
#define AUDIO_FREQUENCY_11K           ((uint32_t)11025)
#define AUDIO_FREQUENCY_8K            ((uint32_t)8000)

/** CS43l22 Registers  ***/
#define   CS43L22_REG_ID                  0x01
#define   CS43L22_REG_POWER_CTL1          0x02
#define   CS43L22_REG_POWER_CTL2          0x04
#define   CS43L22_REG_CLOCKING_CTL        0x05
#define   CS43L22_REG_INTERFACE_CTL1      0x06
#define   CS43L22_REG_INTERFACE_CTL2      0x07
#define   CS43L22_REG_PASSTHR_A_SELECT    0x08
#define   CS43L22_REG_PASSTHR_B_SELECT    0x09
#define   CS43L22_REG_ANALOG_ZC_SR_SETT   0x0A
#define   CS43L22_REG_PASSTHR_GANG_CTL    0x0C
#define   CS43L22_REG_PLAYBACK_CTL1       0x0D
#define   CS43L22_REG_MISC_CTL            0x0E
#define   CS43L22_REG_PLAYBACK_CTL2       0x0F
#define   CS43L22_REG_PASSTHR_A_VOL       0x14
#define   CS43L22_REG_PASSTHR_B_VOL       0x15
#define   CS43L22_REG_PCMA_VOL            0x1A
#define   CS43L22_REG_PCMB_VOL            0x1B
#define   CS43L22_REG_BEEP_FREQ_ON_TIME   0x1C
#define   CS43L22_REG_BEEP_VOL_OFF_TIME   0x1D
#define   CS43L22_REG_BEEP_TONE_CFG       0x1E
#define   CS43L22_REG_TONE_CTL            0x1F
#define   CS43L22_REG_MASTER_A_VOL        0x20
#define   CS43L22_REG_MASTER_B_VOL        0x21
#define   CS43L22_REG_HEADPHONE_A_VOL     0x22
#define   CS43L22_REG_HEADPHONE_B_VOL     0x23
#define   CS43L22_REG_SPEAKER_A_VOL       0x24
#define   CS43L22_REG_SPEAKER_B_VOL       0x25
#define   CS43L22_REG_CH_MIXER_SWAP       0x26
#define   CS43L22_REG_LIMIT_CTL1          0x27
#define   CS43L22_REG_LIMIT_CTL2          0x28
#define   CS43L22_REG_LIMIT_ATTACK_RATE   0x29
#define   CS43L22_REG_OVF_CLK_STATUS      0x2E
#define   CS43L22_REG_BATT_COMPENSATION   0x2F
#define   CS43L22_REG_VP_BATTERY_LEVEL    0x30
#define   CS43L22_REG_SPEAKER_STATUS      0x31
#define   CS43L22_REG_TEMPMONITOR_CTL     0x32
#define   CS43L22_REG_THERMAL_FOLDBACK    0x33
#define   CS43L22_REG_CHARGE_PUMP_FREQ    0x34

#define CODEC_STANDARD                0x04
#define I2S_STANDARD                  I2S_STANDARD_PHILIPS


bool cs43l22_init(CS43L22 cs43l22) {
  if (initialized) {
    return false;
  }
  config = cs43l22;

  reset_cycle();
  init_sequence();



  printf("CS43L22 chip id: 0x%x\n", i2c_read_register(CS43L22_REG_ID));
  printf("CS43L22 power ctl 1: 0x%x\n", i2c_read_register(0x02));
  printf("CS43L22 power ctl 2: 0x%x\n", i2c_read_register(0x04));

  printf("0x05: 0x%x\n", i2c_read_register(0x05));


  // beep
  i2c_write_register(CS43L22_REG_BEEP_TONE_CFG, 0xC0);
  i2c_write_register(CS43L22_REG_BEEP_FREQ_ON_TIME, 0x1C);

  while(true) {
    uint16_t Istr[1];
    Istr[0] = 0;

    HAL_I2S_Transmit (config.i2s, Istr , 0x10, 10 );
  }

  initialized = true;
  return true;
}

// private

static void reset_cycle() {
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_RESET);
  HAL_Delay(5);
  HAL_GPIO_WritePin(config.reset_port, config.reset_pin, GPIO_PIN_SET);
}

static void init_sequence() {
  i2c_write_register(0x00, 0x99);
  i2c_write_register(0x47, 0x80);
  i2c_write_register(0x32, 0xBB);
  i2c_write_register(0x32, 0x3B);
  i2c_write_register(0x00, 0x00);
  //i2c_write_register(0x06, 0x00); // not sure
  i2c_write_register(0x02, 0x9E);

  i2c_write_register(0x04, 0xAA); // output both

  i2c_write_register(CS43L22_REG_CLOCKING_CTL, 0x81);

  /* Set the Slave Mode and the audio Standard */
  i2c_write_register(CS43L22_REG_INTERFACE_CTL1, CODEC_STANDARD);


  i2c_write_register(CS43L22_REG_ANALOG_ZC_SR_SETT, 0x00);
  /* Disable the digital soft ramp */
  i2c_write_register(CS43L22_REG_MISC_CTL, 0x04);
  /* Disable the limiter attack level */
  i2c_write_register(CS43L22_REG_LIMIT_CTL1, 0x00);
  /* Adjust Bass and Treble levels */
  i2c_write_register(CS43L22_REG_TONE_CTL, 0x0F);
  /* Adjust PCM volume level */
  i2c_write_register(CS43L22_REG_PCMA_VOL, 0x0A);
  i2c_write_register(CS43L22_REG_PCMB_VOL, 0x0A);
}

static bool i2c_write_register(uint8_t reg, uint8_t data) {
  return HAL_I2C_Mem_Write(config.i2c, config.address, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 10) == HAL_OK;
}

static uint8_t i2c_read_register(uint8_t reg) {
  uint8_t buf = 0x00;
  HAL_I2C_Mem_Read(config.i2c, config.address, reg, I2C_MEMADD_SIZE_8BIT, &buf, 1, 10);
  return buf;
}
