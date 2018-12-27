#ifndef __PLAYER_H
#define __PLAYER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool player_init();
bool player_start(const char *fname, bool async);
bool player_stop();
bool player_get_now_playing(char *buffer, size_t length);
uint8_t player_get_position_percents();
void player_set_volume(uint8_t percents);
void player_set_bass_freqlimit(uint8_t value); // 2-15          //  20Hz - 150Hz
void player_set_bass_amplitude(uint8_t value); // 0-15, 0=off   //  0dB  - 15dB
void player_set_treble_freqlimit(uint8_t value); // 0-15        //  0KHz - 15KHz
void player_set_treble_amplitude(int8_t value); // -8-7, 0=off  // -12dB - 10.5dB

#endif
