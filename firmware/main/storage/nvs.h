#ifndef __NVS_H
#define __NVS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool nvs_init();
bool nvs_read_string(const char *key, char *string, size_t max_length);
bool nvs_save_string(const char *key, char *string);

#endif
