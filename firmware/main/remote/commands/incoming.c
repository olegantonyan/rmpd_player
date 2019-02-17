#include "remote/commands/incoming.h"
#include "cJSON.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>

extern bool update_setting(const IncomingCommandArgument_t *arg);

static const char *TAG = "incoming_cmd";

static bool execute(const char *command, const cJSON *json, uint32_t sequence);

bool incoming_command(const char *data, uint32_t sequence) {
  cJSON *json = cJSON_Parse(data);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      ESP_LOGE(TAG, "json parse error: %s", error_ptr);
    }
    return false;
  }

  const cJSON *command = cJSON_GetObjectItemCaseSensitive(json, "command");
  if (cJSON_IsString(command) && (command->valuestring != NULL)) {
    return execute(command->valuestring, json, sequence);
  }
  return true;
}

static bool execute(const char *command, const cJSON *json, uint32_t sequence) {
  bool ok = false;
  IncomingCommandArgument_t arg = {
    .json = json,
    .sequence = sequence
  };

  if (strcmp(command, "update_setting") == 0) {
    ok = update_setting(&arg);
  } else {
    ESP_LOGW(TAG, "unknown/unsupported command '%s'", command);
    ok = false;
  }

  return ok;
}
