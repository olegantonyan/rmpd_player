#ifndef __FILES_H
#define __FILES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

bool string_ends_with(const char *str, const char *suffix);
size_t file_size(FILE *f);

#endif
