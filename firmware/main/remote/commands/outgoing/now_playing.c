#include "cJSON.h"
#include "remote/queue.h"
#include "audio/player.h"
#include "remote/commands/outgoing.h"
#include <stdio.h>
#include <string.h>

bool now_playing(OutgoingCommandArgument_t *arg) {
  cJSON_AddItemToObject(arg->json, "command", cJSON_CreateString("now_playing"));

  char now_playing[322] = { 0 };
  if (player_get_now_playing(now_playing, sizeof(now_playing))) {
    char percents[10] = { 0 };
    snprintf(percents, sizeof(percents), " (%d%%)", player_get_position_percents());
    strncat(now_playing, percents, sizeof(now_playing) - sizeof(percents) -1);
    cJSON_AddItemToObject(arg->json, "message", cJSON_CreateString(now_playing));
  } else {
    cJSON_AddItemToObject(arg->json, "message", cJSON_CreateString(""));
  }
  return true;
}
