#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  void *data; // XXX: dynamically allocated
  uint32_t sequence;
  uint16_t max_retries;
  void (*callback)(bool);
} QueueMessage_t;

bool queue_put(const QueueMessage_t *msg, uint32_t timeout_ms);
bool queue_receive(QueueMessage_t *msg, uint32_t timeout_ms);

#endif
