#include "remote/commands/incoming.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>

extern bool update_setting(const IncomingCommandArgument_t *arg);
extern bool update_playlist(const IncomingCommandArgument_t *arg);

static const char *TAG = "incoming_cmd";

static const char *parse_command(json_stream *json);
static bool execute(const char *command, const IncomingCommandArgument_t *arg);

bool incoming_command(const char *data, Tempfile_t *datafile, uint32_t sequence) {
  IncomingCommandArgument_t arg = {
    .sequence = sequence,
    .datafile = datafile
  };
  if (datafile != NULL) {
    ESP_LOGD(TAG, "received data in file %s", datafile->path);
    tempfile_open(datafile, "r"); // was closed before
    json_open_stream(&arg.json, datafile->file);
    /*char b[64] = { 0 };
    size_t i = fread(b, 1, 63, datafile->file);
    if (i > 0) {
      printf("data read: %s\n", b);
    } else {
      printf("read %d\n", i);
    }*/
  } else if(data != NULL && strcmp("{}", data) != 0) {
    ESP_LOGD(TAG, "received data: %s", data);
    json_open_string(&arg.json, data);
  } else {
    return true;
  }

  bool ok = false;
  const char *command = parse_command(&arg.json);
  if (command != NULL) {
    ESP_LOGD(TAG, "received command %s", command);
    json_reset(&arg.json);
    ok = execute(command, &arg);
  }
  json_close(&arg.json);
  tempfile_remove(arg.datafile);
  return ok;
}

static const char *parse_command(json_stream *json) {
  size_t len = 0;
  enum json_type t = JSON_ERROR;
  bool key_found = false;
  do {
    t = json_next(json);
    switch(t) {
      case JSON_STRING: {
        if (json_get_depth(json) == 1) {
          const char *s = json_get_string(json, &len);
          if (key_found) {
            return s;
          } else if (strcmp(s, "command") == 0) {
            key_found = true;
          }
        }
        break;
      }
      case JSON_ERROR:
        ESP_LOGE(TAG, "json parse error: %s line %d pos %d", json_get_error(json), json_get_lineno(json), json_get_position(json));
        break;
      default:
        break;
    }
  } while (t != JSON_DONE && t != JSON_ERROR);
  return NULL;
}

static bool execute(const char *command, const IncomingCommandArgument_t *arg) {
  if (strcmp(command, "update_setting") == 0) {
    return update_setting(arg);
  } else if (strcmp(command, "update_playlist") == 0) {
    return update_playlist(arg);
  }
  return false;
}
