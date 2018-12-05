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

#endif
