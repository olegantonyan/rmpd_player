#include "cJSON.h"
#include "remote/queue.h"
#include "audio/player.h"
#include "remote/commands/outgoing.h"

bool now_playing(CommandArgument_t *arg) {
  cJSON_AddItemToObject(arg->json, "command", cJSON_CreateString("now_playing"));

  char now_playing[256] = { 0 };
  if (player_get_now_playing(now_playing, sizeof(now_playing))) {
    cJSON_AddItemToObject(arg->json, "message", cJSON_CreateString(now_playing));
  } else {
    cJSON_AddItemToObject(arg->json, "message", cJSON_CreateString("nothing"));
  }
  return true;
}
