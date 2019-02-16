#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  void *data; // XXX: dynamically allocated
  uint32_t sequence;
} QueueMessage_t;

bool queue_put(const QueueMessage_t *msg, uint32_t timeout_ms);
bool queue_receive(QueueMessage_t *msg, uint32_t timeout_ms);

#endif
