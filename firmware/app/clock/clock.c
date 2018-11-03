#include "clock/clock.h"

static uint32_t hal_read_rtc(); //HACK: HAL's rtc implementation sucks, and RTC_ReadTimeCounter is private. copy-pasted function here

static RTC_HandleTypeDef *rtc = NULL;

void clock_init(RTC_HandleTypeDef *rtc_) {
  rtc = rtc_;
}

time_t clock_gettime() {
  if (!rtc) {
    return 0;
  }
  return hal_read_rtc(rtc);
}

time_t time(time_t *t) {
   time_t seconds_since_epoch = clock_gettime();
   if (t) {
     *t = seconds_since_epoch;
   }
   return seconds_since_epoch;
}

// private

// copy-pasted RTC_ReadTimeCounter
static uint32_t hal_read_rtc(RTC_HandleTypeDef* hrtc)
{
  uint16_t high1 = 0U, high2 = 0U, low = 0U;
  uint32_t timecounter = 0U;

  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  low   = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  { /* In this case the counter roll over during reading of CNTL and CNTH registers,
       read again CNTL register then return the counter value */
    timecounter = (((uint32_t) high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  { /* No counter roll over during reading of CNTL and CNTH registers, counter
       value is equal to first value of CNTL and CNTH */
    timecounter = (((uint32_t) high1 << 16U) | low);
  }

  return timecounter;
}
