#ifndef __SD_H
#define __SD_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define STORAGE_SD_MOUNTPOINT "/sdcard"

bool sd_init();
size_t sd_bytes_free();

#endif
