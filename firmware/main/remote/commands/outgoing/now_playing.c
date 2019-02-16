#include "cJSON.h"
#include "remote/queue.h"
#include "audio/player.h"

bool now_playing(cJSON *json, uint32_t *sequence, const void *args) {
  char now_playing[256] = { 0 };

  if (player_get_now_playing(now_playing, sizeof(now_playing))) {
    cJSON_AddItemToObject(json, "message", cJSON_CreateString(now_playing));
  } else {
    cJSON_AddItemToObject(json, "message", cJSON_CreateString("nothing"));
  }
  return true;
}
