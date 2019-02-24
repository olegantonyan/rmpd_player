#include "util/sysinfo.h"
#include "esp_ota_ops.h"

void sysinfo_useragent(char *buffer, size_t length) {
  if (buffer == NULL) {
    return;
  }
  const esp_app_desc_t *d = esp_ota_get_app_description();
  snprintf(buffer, length, "rmpd %s (%s, FreeRTOS, ESP32, ESP-IDF %s, build %s %s)", d->version, d->project_name, d->idf_ver, d->date, d->time);
}
