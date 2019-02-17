#include "cJSON.h"
#include <time.h>
#include "audio/player.h"
#include "storage/sd.h"
#include "remote/queue.h"
#include "playlist/track.h"
#include "remote/commands/outgoing.h"

bool track_end(OutgoingCommandArgument_t *arg) {
  cJSON_AddItemToObject(arg->json, "command", cJSON_CreateString("track_end"));

  Track_t *track = (Track_t *)arg->args;
  cJSON *message = cJSON_CreateObject();
  cJSON_AddItemToObject(message, "filename", cJSON_CreateString(track->filename));
  cJSON_AddItemToObject(message, "id", cJSON_CreateNumber(track->id));

  cJSON_AddItemToObject(arg->json, "message", message);
  arg->max_retries = 50;

  return true;
}
