#ifndef __SD_H
#define __SD_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define STORAGE_SD_MOUNTPOINT "/sdcard"

bool sd_init();
uint64_t sd_bytes_free();
bool sd_global_lock_acquire(uint32_t ms);
bool sd_global_lock_release();

#endif
