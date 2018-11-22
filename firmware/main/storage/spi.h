#ifndef __SPI_H
#define __SPI_H

#include <stdbool.h>
#include <stdint.h>

#define STORAGE_SPI_MOUNTPOINT "/spiffs"

bool spiffs_init();

#endif
