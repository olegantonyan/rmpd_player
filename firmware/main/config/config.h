#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

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

#endif
