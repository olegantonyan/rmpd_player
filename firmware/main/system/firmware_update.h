#ifndef __FIRMWARE_UPDATE_H
#define __FIRMWARE_UPDATE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

bool firmware_update_start(const char *url, uint32_t sequence);
void firmware_update_confirm(bool success);

#endif
