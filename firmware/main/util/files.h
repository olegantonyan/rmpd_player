#ifndef __FILES_H
#define __FILES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

size_t file_size(FILE *f);
bool file_copy(const char *src, const char *dst);
int mkdir_p(const char *path);
bool file_exists(const char *path);

#endif
