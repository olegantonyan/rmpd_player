#ifndef __STRINGS_H
#define __STRINGS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool string_ends_with(const char *str, const char *suffix);
void string_chomp(char *line);

#endif
