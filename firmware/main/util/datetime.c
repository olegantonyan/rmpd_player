#include "util/datetime.h"

int32_t datetime_string_to_seconds(const char *string) {
  if (NULL == string) {
    return -1;
  }
  struct tm tm;
  if (strptime(string, "%H:%M:%S", &tm) == NULL) {
    return -1;
  }
  return (int32_t)mktime(&tm);
}
