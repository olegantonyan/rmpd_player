#include "cJSON.h"
#include <time.h>
#include "audio/player.h"
#include "storage/sd.h"
#include "remote/queue.h"
#include "remote/commands/outgoing.h"
#include "playlist/track.h"

bool track_begin(OutgoingCommandArgument_t *arg) {
  cJSON_AddItemToObject(arg->json, "command", cJSON_CreateString("track_begin"));

  Track_t *track = (Track_t *)arg->args;
  cJSON *message = cJSON_CreateObject();
  cJSON_AddItemToObject(message, "filename", cJSON_CreateString(track->filename));
  cJSON_AddItemToObject(message, "id", cJSON_CreateNumber(track->id));
  cJSON_AddItemToObject(message, "type", cJSON_CreateString(track_type_to_string(track)));

  cJSON_AddItemToObject(arg->json, "message", message);
  arg->max_retries = 50;

  return true;
}
