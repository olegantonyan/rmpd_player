#include "cJSON.h"
#include <time.h>
#include "audio/player.h"
#include "storage/sd.h"
#include "remote/queue.h"
#include "remote/commands/outgoing.h"

bool power_on(CommandArgument_t *arg) {
  cJSON_AddItemToObject(arg->json, "command", cJSON_CreateString("power_on"));
  arg->max_retries = 15;
  return true;
}
