#include "remote/commands/outgoing.h"
#include "remote/commands/outgoing/now_playing.h"
#include "remote/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "remote/queue.h"
#include <string.h>

static const char *TAG = "outgoing_cmd";

bool outgoing_command(OutgoingCommand_t cmd, void *args) {
  OutgoingCommandResult_t result;
  memset(&result, 0, sizeof(result));

  switch(cmd) {
    case NOW_PLAYING:
      result = now_playing(args);
      break;
    default:
      ESP_LOGE(TAG, "unknown command");
      break;
  }

  if (result.data == NULL) {
    ESP_LOGE(TAG, "error executing command %d", cmd);
    return false;
  }
  QueueMessage_t msg;
  memset(&msg, 0, sizeof(msg));
  msg.data = result.data;
  msg.sequence = result.sequence;
  bool ok = queue_put(&msg, result.queue_timeout);
  if (!ok) {
    free(result.data); // no way we can handle this now
    msg.data = NULL;
  }
  return ok;
}
