#include "playlist/cloud/scheduler.h"
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "cloud_sched";

bool cloud_scheduler_is_enabled() {
  return false;
}

bool cloud_scheduler_init() {
  return false;
}
