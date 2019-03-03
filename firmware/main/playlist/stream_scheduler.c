#include "playlist/stream_scheduler.h"
#include "audio/stream.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "playlist/offline/recurse.h"
#include "wifi/wifi.h"
#include "audio/stream_playlist.h"
#include "playlist/offline/scheduler.h"

static const char *TAG = "stream_sched";

static void thread(void * args);
static void on_medifile_callback(const char *path, uint16_t index);
static bool list_add(uint16_t *list, uint16_t value);
static bool list_remove(uint16_t *list, uint16_t value);
static bool list_contains(uint16_t *list, uint16_t value);
static uint8_t list_size(uint16_t *list);

#define BLANK_LIST_ITEM UINT16_MAX

static struct {
  bool initialized;
  uint16_t list[STREAM_SCHEDULER_MAX_STREAMS];
  uint16_t dead[STREAM_SCHEDULER_MAX_STREAMS];
  SemaphoreHandle_t mutex;
  const char *storage_path;
  SemaphoreHandle_t sema;
} p = { false, { 0 },  { 0 }, NULL, NULL, NULL };


bool stream_scheduler_init(const char *storage_path) {
  for (size_t i = 0; i < STREAM_SCHEDULER_MAX_STREAMS; i++) {
    p.list[i] = BLANK_LIST_ITEM;
    p.dead[i] = BLANK_LIST_ITEM;
  }
  p.mutex = xSemaphoreCreateMutex();
  if (p.mutex == NULL) {
    ESP_LOGE(TAG, "cannot create mutex");
    stream_scheduler_deinit();
    return false;
  }
  p.sema = xSemaphoreCreateBinary();
  if (p.sema == NULL) {
    ESP_LOGE(TAG, "cannot create semaphore");
    stream_scheduler_deinit();
    return false;
  }
  if (storage_path == NULL) {
    ESP_LOGE(TAG, "null storage path");
    stream_scheduler_deinit();
    return false;
  }
  p.storage_path = storage_path;
  p.initialized = true;
  return true;
}

bool stream_scheduler_start() {
  return xTaskCreate(thread, TAG, 4096, NULL, 5, NULL) == pdPASS;
}

void stream_scheduler_deinit() {
  p.initialized = false;
  if (p.mutex != NULL) {
    vSemaphoreDelete(p.mutex);
    p.mutex = NULL;
  }
  if (p.sema != NULL) {
    vSemaphoreDelete(p.sema);
    p.sema = NULL;
  }
}

uint8_t stream_scheduler_max() {
  return list_size(p.list);
}

void stream_scheduler_force_probe() {
  if (p.sema != NULL) {
    xSemaphoreGive(p.sema);
  }
}

bool stream_scheduler_add_stream(uint16_t index) {
  if (!p.initialized) {
    return false;
  }
  return list_add(p.list, index);
}

bool stream_scheduler_is_known_stream(uint16_t index) {
  if (!p.initialized) {
    return false;
  }
  return list_contains(p.list, index);
}

bool stream_scheduler_is_alive(uint16_t index) {
  if (!p.initialized) {
    return false;
  }
  return !list_contains(p.dead, index);
}

void stream_scheduler_mark_dead(uint16_t index) {
  list_add(p.dead, index);
}

void stream_scheduler_mark_alive(uint16_t index) {
  list_remove(p.dead, index);
}

bool stream_scheduler_any_alive_streams() {
  if (!p.initialized) {
    return false;
  }
  if (stream_scheduler_max() == 0) {
    return false;
  }
  return list_size(p.dead) == 0;
}

static void thread(void * args) {
  ESP_LOGI(TAG, "started stream scheduler thread, known streams: %d", stream_scheduler_max());

  wifi_wait_connected(portMAX_DELAY);
  uint16_t index = 0;
  do {
    index = 0;
    recurse_dir(p.storage_path, 0, &index, on_medifile_callback, scheduler_mediafile_match_func);

    if (stream_scheduler_any_alive_streams()) {
      xSemaphoreTake(p.sema, pdMS_TO_TICKS(1 * 3600 * 1000));
    } else {
      xSemaphoreTake(p.sema, pdMS_TO_TICKS(10 * 1000));
    }
  } while(p.initialized);
  ESP_LOGI(TAG, "stopping stream scheduler thread");
  vTaskDelete(NULL);
}

static void on_medifile_callback(const char *path, uint16_t index) {
  if (!stream_scheduler_is_known_stream(index)) {
    return;
  }
  if (!wifi_is_connected()) {
    stream_scheduler_mark_dead(index);
    return;
  }
  char url[512] = { 0 };
  if (!stream_playlist_parse_file(path, url, sizeof(url))) {
    ESP_LOGW(TAG, "error parsing playlist file '%s'", path);
    stream_scheduler_mark_dead(index);
    return;
  }
  if (stream_probe(url)) {
    stream_scheduler_mark_alive(index);
    ESP_LOGI(TAG, "stream in file %s seems to be alive", path);
  } else {
    stream_scheduler_mark_dead(index);
    ESP_LOGI(TAG, "stream in file %s seems to be dead", path);
  }
}

static bool list_add(uint16_t *list, uint16_t value) {
  bool result = false;
  xSemaphoreTake(p.mutex, portMAX_DELAY);
  for (size_t i = 0; i < STREAM_SCHEDULER_MAX_STREAMS; i++) {
    if (list[i] == BLANK_LIST_ITEM) {
      list[i] = value;
      result = true;
      break;
    }
  }
  xSemaphoreGive(p.mutex);
  return result;
}

static bool list_remove(uint16_t *list, uint16_t value) {
  bool result = false;
  xSemaphoreTake(p.mutex, portMAX_DELAY);
  for (size_t i = 0; i < STREAM_SCHEDULER_MAX_STREAMS; i++) {
    if (list[i] == value) {
      list[i] = BLANK_LIST_ITEM;
      result = true;
    }
  }
  xSemaphoreGive(p.mutex);
  return result;
}

static bool list_contains(uint16_t *list, uint16_t value) {
  bool result = false;
  xSemaphoreTake(p.mutex, portMAX_DELAY);
  for (size_t i = 0; i < STREAM_SCHEDULER_MAX_STREAMS; i++) {
    if (list[i] == value) {
      result = true;
      break;
    }
  }
  xSemaphoreGive(p.mutex);
  return result;
}

static uint8_t list_size(uint16_t *list) {
  uint8_t result = 0;
  xSemaphoreTake(p.mutex, portMAX_DELAY);
  for (size_t i = 0; i < STREAM_SCHEDULER_MAX_STREAMS; i++) {
    if (list[i] != BLANK_LIST_ITEM) {
      result++;
    }
  }
  xSemaphoreGive(p.mutex);
  return result;
}
