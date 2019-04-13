#include "remote/commands/outgoing.h"
#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "cJSON.h"

bool update_playlist_progress(OutgoingCommandArgument_t *arg) {
  cJSON_AddItemToObject(arg->json, "command", cJSON_CreateString("update_playlist"));

  const UpdatePlaylistCommandArgs_t *args = (UpdatePlaylistCommandArgs_t *)arg->args;

  cJSON *message = cJSON_CreateObject();
  cJSON_AddItemToObject(message, "files_done", cJSON_CreateNumber(args->files_done));

  cJSON_AddItemToObject(arg->json, "message", message);
  arg->sequence = args->sequence;

  return true;
}
