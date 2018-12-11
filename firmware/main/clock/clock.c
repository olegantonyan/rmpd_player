#include "clock/clock.h"
#include "clock/ds3231.h"
#include "clock/ntp.h"

bool clock_init() {
  return ds3231_init() && ntp_init();
}
