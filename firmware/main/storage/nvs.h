#ifndef __NVS_H
#define __NVS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool nvs_read_string(const char *key, char *string, size_t max_length);
bool nvs_save_string(const char *key, char *string);

bool nvs_read_uint8(const char *key, uint8_t *value);
bool nvs_save_uint8(const char *key, uint8_t value);

bool nvs_read_uint32(const char *key, uint32_t *value);
bool nvs_save_uint32(const char *key, uint32_t value);

#endif
