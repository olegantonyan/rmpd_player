#include "config/config.h"
#include "storage/nvs.h"

#define MAX_WIFI_SSID_LENGTH 32
#define MAX_WIFI_PASS_LENGTH 64

char *config_wifi_ssid() {
  static char buffer[MAX_WIFI_SSID_LENGTH] = {0};
  bool ok = nvs_read_string("wifi_ssid", buffer, sizeof(buffer));
  if (ok) {
    return buffer;
  }
  return NULL;
}

char *config_wifi_pass() {
  static char buffer[MAX_WIFI_PASS_LENGTH] = {0};
  bool ok = nvs_read_string("wifi_pass", buffer, sizeof(buffer));
  if (ok) {
    return buffer;
  }
  return NULL;
}

bool config_save_wifi_ssid(const char * arg) {
  return nvs_save_string("wifi_ssid", (char *)arg);
}

bool config_save_wifi_pass(const char * arg) {
  return nvs_save_string("wifi_pass", (char *)arg);
}

uint8_t config_volume() {
  uint8_t value = 0;
  if (!nvs_read_uint8("volume", &value)) {
    return 100;
  }
  if (value > 100) {
    return 100;
  }
  return value;
}

bool config_save_volume(uint8_t arg) {
  return nvs_save_uint8("volume", arg);
}
