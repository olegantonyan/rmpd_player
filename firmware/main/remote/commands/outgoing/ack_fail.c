#include "cJSON.h"
#include "remote/queue.h"
#include "remote/commands/outgoing.h"

bool ack_fail(OutgoingCommandArgument_t *arg) {
  cJSON_AddItemToObject(arg->json, "command", cJSON_CreateString("ack_fail"));

  if (arg->args != NULL) {
    const AckCommandArgs_t *args = (const AckCommandArgs_t *)arg->args;
    cJSON_AddItemToObject(arg->json, "message", cJSON_CreateString(args->message));
    arg->sequence = args->sequence;
  } else {
    cJSON_AddItemToObject(arg->json, "message", cJSON_CreateString(""));
  }

  return true;
}
