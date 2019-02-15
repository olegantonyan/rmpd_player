#include "remote/commands/outgoing/now_playing.h"
#include "cJSON.h"
#include <time.h>
#include "audio/player.h"
#include "storage/sd.h"

char *now_playing(void *_args) {
  cJSON *root = cJSON_CreateObject();

  time_t now = time(NULL);
  struct tm timeinfo = { 0 };
  localtime_r(&now, &timeinfo);
  char time_buf[40] = { 0 };
  strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S%z", &timeinfo);
  cJSON_AddItemToObject(root, "localtime", cJSON_CreateString(time_buf));

  cJSON_AddItemToObject(root, "command", cJSON_CreateString("now_playing"));

  char now_playing[256] = { 0 };
  if (player_get_now_playing(now_playing, sizeof(now_playing))) {
    cJSON_AddItemToObject(root, "message", cJSON_CreateString(now_playing));
  } else {
    cJSON_AddItemToObject(root, "message", cJSON_CreateString(""));
  }

  cJSON_AddItemToObject(root, "free_space", cJSON_CreateNumber(sd_bytes_free()));

  char *result = cJSON_Print(root);
  cJSON_Delete(root);
  return result;
}
