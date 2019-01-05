#include "util/strings.h"

#include <string.h>

bool string_ends_with(const char *str, const char *suffix) {
  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);
  return (str_len >= suffix_len) && (0 == strcmp(str + (str_len - suffix_len), suffix));
}

void string_chomp(char *line) {
  for (int i = strlen(line) - 1; i > 0; i--) {
    if (line[i] == '\r' || line[i] == '\n') {
      line[i] = '\0';
    }
  }
}
