#include "util/read_ahead.h"
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "read_ahead";

static void thread(void *arg);
static bool start(ReadAhead_t *ra);
static size_t read(ReadAhead_t *ra);

ReadAhead_t *read_ahead_init(FILE *file, size_t buffer_size) {
  ReadAhead_t *result = malloc(sizeof(ReadAhead_t));
  if (result == NULL) {
    ESP_LOGE(TAG, "cannot allocate memory for ReadAhead_t");
    return NULL;
  }
  result->buffer = malloc(buffer_size);
  if (result->buffer == NULL) {
    ESP_LOGE(TAG, "cannot allocate memory for buffer");
    free(result);
    return NULL;
  }
  result->buffer_size = buffer_size;
  result->file = file;
  result->bytes_read = 0;
  result->mutex = xSemaphoreCreateMutex();
  if (result->mutex == NULL) {
    ESP_LOGE(TAG, "cannot create mutex");
    free(result->buffer);
    free(result);
    return NULL;
  }

  result->sema = xSemaphoreCreateBinary();
  if (result->sema == NULL) {
    ESP_LOGE(TAG, "cannot create semaphore");
    vSemaphoreDelete(result->mutex);
    free(result->buffer);
    free(result);
    return NULL;
  }

  start(result);

  return result;
}

size_t read_ahead_next(ReadAhead_t *ra, uint8_t *buffer, size_t buffer_size) {
  if (buffer_size > ra->buffer_size) {
    return 0;
  }

  xSemaphoreTake(ra->sema, portMAX_DELAY);

  size_t result = 0;
  xSemaphoreTake(ra->mutex, portMAX_DELAY);
  result = ra->bytes_read;
  if (result > 0) {
    memcpy(buffer, ra->buffer, ra->bytes_read);
  }
  xSemaphoreGive(ra->mutex);

  start(ra);

  return result;
}

void read_ahead_deinit(ReadAhead_t *ra) {
  vSemaphoreDelete(ra->mutex);
  vSemaphoreDelete(ra->sema);
  free(ra->buffer);
  free(ra);
}

static void thread(void *arg) {
  ReadAhead_t *ra = (ReadAhead_t *)arg;
  read(ra);
  xSemaphoreGive(ra->sema);
  vTaskDelete(NULL);
}

static bool start(ReadAhead_t *ra) {
  return xTaskCreate(thread, TAG, 3000, (void *)ra, tskIDLE_PRIORITY, NULL) == pdPASS;
}

static size_t read(ReadAhead_t *ra) {
  xSemaphoreTake(ra->mutex, portMAX_DELAY);
  ra->bytes_read = fread(ra->buffer, 1, ra->buffer_size, ra->file);
  xSemaphoreGive(ra->mutex);
  return ra->bytes_read;
}
