#ifndef __WIFI_H
#define __WIFI_H

#include <stdbool.h>
#include <stdint.h>

bool wifi_init();
bool wifi_reconfig();
bool wifi_is_connected();

#endif
