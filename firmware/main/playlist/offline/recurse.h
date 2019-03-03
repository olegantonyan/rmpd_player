#ifndef __RECURSE_H
#define __RECURSE_H

#include <stdbool.h>
#include <stdint.h>

#define RECURSE_MAX_DEPTH 10

uint32_t recurse_dir(const char *path, uint8_t depth, uint16_t *index, void (*callback)(const char *fullname, uint16_t index), bool (*match_func)(const char *fname));

#endif
