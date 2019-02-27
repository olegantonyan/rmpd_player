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

  snprintf(buffer, length, "rmpd-esp32 %s (%s, esp-idf %s, chip rev %d, build %s %s)", d->version, d->project_name, d->idf_ver, ci.revision, d->date, d->time);
}
