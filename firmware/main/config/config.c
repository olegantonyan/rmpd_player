#include "config/config.h"
#include "storage/nvs.h"

#define MAX_WIFI_SSID_LENGTH 32
#define MAX_WIFI_PASS_LENGTH 64
#define MAX_TIMEZONE_LENGTH 32

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

uint8_t config_bass_freqlimit() {
  uint8_t value = 0;
  if (!nvs_read_uint8("bass_freqlim", &value)) {
    return 0;
  }
  return value;
}

uint8_t config_bass_amplitude() {
  uint8_t value = 0;
  if (!nvs_read_uint8("bass_amp", &value)) {
    return 0;
  }
  return value;
}

uint8_t config_treble_freqlimit() {
  uint8_t value = 0;
  if (!nvs_read_uint8("treb_freqlim", &value)) {
    return 0;
  }
  return value;
}

int8_t config_treble_amplitude() {
  uint8_t value = 0;
  if (!nvs_read_uint8("treb_amp", &value)) {
    return 0;
  }
  return (int8_t)value;
}

bool config_save_bass_freqlimit(uint8_t arg) {
  return nvs_save_uint8("bass_freqlim", arg);
}

bool config_save_bass_amplitude(uint8_t arg) {
  return nvs_save_uint8("bass_amp", arg);
}

bool config_save_treble_freqlimit(uint8_t arg) {
  return nvs_save_uint8("treb_freqlim", arg);
}

bool config_save_treble_amplitude(int8_t arg) {
  return nvs_save_uint8("treb_amp", (uint8_t)arg);
}

bool config_random() {
  uint8_t value = 0;
  if (!nvs_read_uint8("random", &value)) {
    return false;
  }
  return value == 0 ? false : true;
}

bool config_save_random(bool arg) {
  return nvs_save_uint8("random", arg ? 1 : 0);
}

char *config_web_login() {
  char *t = "admin";
  return t;
}

char *config_web_password() {
  return config_ap_password();
}

char *config_deviceid() {
  static char buffer[10] = {0};
  bool ok = nvs_read_string("deviceid", buffer, sizeof(buffer));
  if (ok) {
    return buffer;
  }
  return "vanillaesp";
}

char *config_server_password() {
  static char buffer[16] = {0};
  bool ok = nvs_read_string("server_pass", buffer, sizeof(buffer));
  if (ok) {
    return buffer;
  }
  return "vanillaesppass";
}

char *config_server_url() {
  static char buffer[32] = {0};
  bool ok = nvs_read_string("server_url", buffer, sizeof(buffer));
  if (ok) {
    return buffer;
  }
//return "http://192.168.1.3:3000";
  return "https://server.slon-ds.ru/";
}

char *config_ap_password() {
  return "12345678";
}

config_ip_addr_t config_ap_static_ip() {
  config_ip_addr_t t = {
    .oct1 = 10,
    .oct2 = 10,
    .oct3 = 0,
    .oct4 = 1
  };
  return t;
}

char *config_timezone() {
  static char buffer[MAX_TIMEZONE_LENGTH] = {0};
  bool ok = nvs_read_string("timezone", buffer, sizeof(buffer));
  if (ok) {
    return buffer;
  }
  return "Europe/Kaliningrad";
}

bool config_save_timezone(const char *arg) {
  return nvs_save_string("timezone", (char *)arg);
}

uint8_t config_stream_retries() {
  return 3;
}
