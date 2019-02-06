#include "remote/gate.h"
#include "util/http.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "remote_gate";

static void thread(void *args);

bool gate_init() {
  // TODO
  return true;
}

static void thread(void *args) {
  while(true) {


  }
}
