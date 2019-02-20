#include "remote/commands/outgoing.h"
#include "remote/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "remote/queue.h"
#include <string.h>
#include <time.h>
#include "storage/sd.h"
#include "cJSON.h"

extern bool now_playing(OutgoingCommandArgument_t *arg);
extern bool power_on(OutgoingCommandArgument_t *arg);
extern bool track_end(OutgoingCommandArgument_t *arg);
extern bool track_begin(OutgoingCommandArgument_t *arg);
extern bool ack_ok(OutgoingCommandArgument_t *arg);
extern bool ack_fail(OutgoingCommandArgument_t *arg);

static const char *TAG = "outgoing_cmd";

static void base_command_fields(cJSON *json);

bool outgoing_command(OutgoingCommand_t cmd, void *args) {
  bool ok = false;
  OutgoingCommandArgument_t arg = {
    .json = cJSON_CreateObject(),
    .sequence = 0,
    .max_retries = 0,
    .args = args
  };

  switch(cmd) {
    case NOW_PLAYING:
      ok = now_playing(&arg);
      break;
    case POWER_ON:
      ok = power_on(&arg);
      break;
    case TRACK_BEGIN:
      ok = track_begin(&arg);
      break;
    case TRACK_END:
      ok = track_end(&arg);
      break;

    default:
      ESP_LOGE(TAG, "unknown command");
      ok = false;
      break;
  }

  if (ok) {
    base_command_fields(arg.json);

    QueueMessage_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.data = cJSON_PrintUnformatted(arg.json);
    msg.sequence = arg.sequence;
    msg.max_retries = arg.max_retries;
    ok = queue_put(&msg, 0);
    if (!ok) {
      free(msg.data); // no way we can handle this now
      msg.data = NULL;
    }
  } else {
    ESP_LOGE(TAG, "error executing command %d", cmd);
  }
  cJSON_Delete(arg.json);

  return ok;
}

static void base_command_fields(cJSON *json) {
  time_t now = time(NULL);
  struct tm timeinfo = { 0 };
  localtime_r(&now, &timeinfo);
  char time_buf[40] = { 0 };
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S%z", &timeinfo);
  cJSON_AddItemToObject(json, "localtime", cJSON_CreateString(time_buf));

  cJSON_AddItemToObject(json, "free_space", cJSON_CreateNumber(sd_bytes_free()));
}
