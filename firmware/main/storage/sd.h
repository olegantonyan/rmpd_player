#ifndef __SD_H
#define __SD_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define STORAGE_SD_MOUNTPOINT "/sdcard"

bool sd_init();
uint64_t sd_bytes_free();

#endif
