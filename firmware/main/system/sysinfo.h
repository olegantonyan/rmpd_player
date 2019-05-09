#ifndef __SYSINFO_H
#define __SYSINFO_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

void sysinfo_useragent(char *buffer, size_t length);
const char *sysinfo_reset_reason();
void sysinfo_sta_ip_addr(char *buffer, size_t length);

#endif
