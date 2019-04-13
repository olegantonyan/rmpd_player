#include "system/sysinfo.h"
#include "esp_ota_ops.h"
#include "esp_system.h"

void sysinfo_useragent(char *buffer, size_t length) {
  if (buffer == NULL) {
    return;
  }
  esp_chip_info_t ci;
  esp_chip_info(&ci);

  const esp_app_desc_t *d = esp_ota_get_app_description();

  snprintf(buffer, length, "rmpd-esp32 %s (%s %s, chip rev %d, build %s %s)", d->version, d->project_name, d->idf_ver, ci.revision, d->date, d->time);
}

const char *sysinfo_reset_reason() {
  const char *reset_reason = "";
  switch(esp_reset_reason()) {
    case ESP_RST_UNKNOWN:   reset_reason = "unknown"; break;    //!< Reset reason can not be determined
    case ESP_RST_POWERON:   reset_reason = "poweron"; break;     //!< Reset due to power-on event
    case ESP_RST_EXT:       reset_reason = "ext"; break;        //!< Reset by external pin (not applicable for ESP32)
    case ESP_RST_SW:        reset_reason = "sw"; break;         //!< Software reset via esp_restart
    case ESP_RST_PANIC:     reset_reason = "panic"; break;      //!< Software reset due to exception/panic
    case ESP_RST_INT_WDT:   reset_reason = "int_wdt"; break;    //!< Reset (software or hardware) due to interrupt watchdog
    case ESP_RST_TASK_WDT:  reset_reason = "task_wdt"; break;   //!< Reset due to task watchdog
    case ESP_RST_WDT:       reset_reason = "wdt"; break;        //!< Reset due to other watchdogs
    case ESP_RST_DEEPSLEEP: reset_reason = "deepsleep"; break;  //!< Reset after exiting deep sleep mode
    case ESP_RST_BROWNOUT:  reset_reason = "brownout"; break;   //!< Brownout reset (software or hardware)
    case ESP_RST_SDIO:      reset_reason = "sdio"; break;       //!< Reset over SDIO
  }
  return reset_reason;
}
