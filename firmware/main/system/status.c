#include "system/status.h"

static bool online = false;

void status_set_online(bool arg) {
  online = arg;
}

bool status_online() {
  return online;
}
