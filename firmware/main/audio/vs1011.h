#ifndef __VS1011_H
#define __VS1011_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

bool vs1011_init();
void vs1011_play(FILE *fp, void (*callback)(uint32_t poistion, uint32_t total));
void vs1011_stop();
void vs1011_transient_mute(bool mute);
uint16_t vs1011_set_volume(uint8_t percents);
uint16_t vs1011_set_bass_freqlimit(uint8_t value); // 2-15          //  20Hz - 150Hz
uint16_t vs1011_set_bass_amplitude(uint8_t value); // 0-15, 0=off   //  0dB  - 15dB
uint16_t vs1011_set_treble_freqlimit(uint8_t value); // 0-15        //  0KHz - 15KHz
uint16_t vs1011_set_treble_amplitude(int8_t value); // -8-7, 0=off  // -12dB - 10.5dB

#endif
