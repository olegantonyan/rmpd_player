#include "remote/queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define QUEUE_LENGTH 16

static bool queue_init();

static QueueHandle_t q = NULL;

bool queue_put(const QueueMessage_t *msg, uint32_t timeout_ms) {
  queue_init();

  uint32_t ticks = pdMS_TO_TICKS(timeout_ms);
  if (timeout_ms == portMAX_DELAY) {
    ticks = portMAX_DELAY;
  }

  return xQueueSendToBack(q, msg, ticks) == pdPASS;
}

bool queue_receive(QueueMessage_t *msg, uint32_t timeout_ms) {
  queue_init();

  uint32_t ticks = pdMS_TO_TICKS(timeout_ms);
  if (timeout_ms == portMAX_DELAY) {
    ticks = portMAX_DELAY;
  }

  return xQueueReceive(q, msg, ticks) == pdPASS;
}

static bool queue_init() {
  if (q != NULL) {
    return true;
  }
  q = xQueueCreate(QUEUE_LENGTH, sizeof(QueueMessage_t));
  return q != NULL;
}
