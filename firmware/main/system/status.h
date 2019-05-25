#ifndef __STATUS_H
#define __STATUS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

void status_set_online(bool arg);
bool status_online();

void status_set_synchronizing(bool arg);
bool status_synchronizing();

#endif
