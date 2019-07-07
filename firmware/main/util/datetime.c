#include "util/datetime.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int32_t datetime_string_to_seconds_int(const char *string) {
  if (NULL == string) {
    return -1;
  }
  unsigned int hours = 0;
  unsigned int minutes = 0;
  unsigned int seconds = 0;

  if (sscanf(string, "%u:%u:%u", &hours, &minutes, &seconds) != 3) {
    return -1;
  }

  return hours * 3600 + minutes * 60 + seconds;
}

int32_t datetime_date_string_to_date_int(const char *string) {
  /*if (NULL == string) {
    return 0;
  }

  unsigned int year = 0;
  unsigned int month = 0;
  unsigned int day = 0;

  if (sscanf(string, "%u.%u.%u", &day, &month, &year) != 3) {
    return -1;
  }
  return year * 1000 + month * 40 + day;*/
  if (NULL == string) {
    return 0;
  }

  unsigned int year = 0;
  unsigned int month = 0;
  unsigned int day = 0;

  if (sscanf(string, "%u.%u.%u", &day, &month, &year) != 3) {
    return -1;
  }
  char buf[10] = { 0 };
  snprintf(buf, sizeof buf, "%u%u%u", day, month, year);
  return atoi(buf);

}

int32_t datetime_current_date_int() {
  time_t current = time(NULL);
  struct tm t = { 0 };
  localtime_r(&current, &t);
  char buf[12] = { 0 };
  strftime(buf, sizeof(buf) - 1, "%d.%m.%Y", &t);
  return datetime_date_string_to_date_int(buf);
}

int32_t datetime_current_time_int() {
  time_t current = time(NULL);
  struct tm t = { 0 };
  localtime_r(&current, &t);
  return t.tm_hour * 3600 + t.tm_min * 60 + t.tm_sec;
}
