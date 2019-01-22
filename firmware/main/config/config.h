#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  uint8_t oct1;
  uint8_t oct2;
  uint8_t oct3;
  uint8_t oct4;
} config_ip_addr_t;

char *config_wifi_ssid();
char *config_wifi_pass();
bool config_save_wifi_ssid(const char * arg);
bool config_save_wifi_pass(const char * arg);

uint8_t config_volume();
bool config_save_volume(uint8_t arg);

uint8_t config_bass_freqlimit();
uint8_t config_bass_amplitude();
uint8_t config_treble_freqlimit();
int8_t config_treble_amplitude();
bool config_save_bass_freqlimit(uint8_t arg);
bool config_save_bass_amplitude(uint8_t arg);
bool config_save_treble_freqlimit(uint8_t arg);
bool config_save_treble_amplitude(int8_t arg);

bool config_random();
bool config_save_random(bool arg);

char *config_web_login();
char *config_web_password();

char *config_deviceid();
char *config_ap_password();

config_ip_addr_t config_ap_static_ip();

char *config_timezone();
bool config_save_timezone(const char *arg);

uint8_t config_stream_retries();

#endif
