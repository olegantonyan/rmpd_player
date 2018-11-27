#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

char *config_wifi_ssid();
char *config_wifi_pass();

bool config_save_wifi_ssid(const char * arg);
bool config_save_wifi_pass(const char * arg);

#endif
