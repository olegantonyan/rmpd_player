#include "cJSON.h"
#include <time.h>
#include "audio/player.h"
#include "storage/sd.h"
#include "remote/queue.h"
#include "remote/commands/outgoing.h"
#include "system/sysinfo.h"

bool power_on(OutgoingCommandArgument_t *arg) {
  cJSON_AddItemToObject(arg->json, "command", cJSON_CreateString("power_on"));
  cJSON_AddItemToObject(arg->json, "message", cJSON_CreateString(sysinfo_reset_reason()));
  arg->max_retries = 15;
  return true;
}
