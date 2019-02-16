#include "remote/commands/outgoing.h"
#include "remote/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "remote/queue.h"
#include <string.h>
#include <time.h>
#include "storage/sd.h"
#include "cJSON.h"

extern bool now_playing(cJSON *json, uint32_t *sequence, const void *args);
extern bool power_on(cJSON *json, uint32_t *sequence, const void *args);

static const char *TAG = "outgoing_cmd";

static void base_command_fields(cJSON *json);

bool outgoing_command(OutgoingCommand_t cmd, void *args) {
  bool ok = false;
  cJSON *json = cJSON_CreateObject();
  const char *cmd_name = NULL;
  uint32_t sequence = 0;

  switch(cmd) {
    case NOW_PLAYING:
      ok = now_playing(json, &sequence, args);
      cmd_name = "now_playing";
      break;
    case POWER_ON:
      ok = power_on(json, &sequence, args);
      cmd_name = "power_on";
      break;

    default:
      ESP_LOGE(TAG, "unknown command");
      break;
  }

  if (ok) {
    base_command_fields(json);
    cJSON_AddItemToObject(json, "command", cJSON_CreateString(cmd_name));

    QueueMessage_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.data = cJSON_PrintUnformatted(json);
    msg.sequence = sequence;
    ok = queue_put(&msg, 0);
    if (!ok) {
      free(msg.data); // no way we can handle this now
      msg.data = NULL;
    }
  } else {
    ESP_LOGE(TAG, "error executing command %d", cmd);
  }
  cJSON_Delete(json);

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
