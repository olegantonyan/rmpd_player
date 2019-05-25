#include "system/status.h"

static bool online = false;
static bool synchronizing = false;

void status_set_online(bool arg) {
  online = arg;
}

bool status_online() {
  return online;
}

void status_set_synchronizing(bool arg) {
  synchronizing = arg;
}

bool status_synchronizing() {
  return synchronizing;
}
