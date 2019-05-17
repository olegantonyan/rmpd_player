#include "remote/commands/incoming.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>

extern bool update_setting(IncomingCommandArgument_t *arg);
extern bool update_playlist(IncomingCommandArgument_t *arg);
extern bool update_software(IncomingCommandArgument_t *arg);
extern bool delete_all_file(IncomingCommandArgument_t *arg);

static const char *TAG = "incoming_cmd";

static const char *parse_command(json_stream *json);
static bool execute(const char *command, IncomingCommandArgument_t *arg);

bool incoming_command(const char *data, Tempfile_t *datafile, uint32_t sequence) {
  IncomingCommandArgument_t arg = {
    .sequence = sequence,
    .datafile = datafile,
    .data = data
  };
  bool in_file = false;
  if (datafile != NULL && datafile->file != NULL) {
    ESP_LOGI(TAG, "received data in file %s", datafile->path);
    tempfile_open(datafile, "r"); // was closed before
    json_open_stream(&arg.json, datafile->file);
    in_file = true;
    arg.data = NULL;
  } else if(data != NULL && strcmp("{}", data) != 0) {
    ESP_LOGD(TAG, "received data: %s", data);
    json_open_string(&arg.json, data);
    in_file = false;
    arg.datafile = NULL;
  } else {
    return false;
  }
  json_close(&arg.json);

  bool ok = false;
  const char *command = parse_command(&arg.json);
  if (command != NULL) {
    ESP_LOGD(TAG, "received command %s", command);

    if (in_file) {
      fseek(datafile->file, 0, SEEK_SET);
      json_open_stream(&arg.json, datafile->file);
    } else {
      json_open_string(&arg.json, data);
    }
    ok = execute(command, &arg);
    json_close(&arg.json);
  }

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

static bool execute(const char *command, IncomingCommandArgument_t *arg) {
  if (strcmp(command, "update_setting") == 0) {
    return update_setting(arg);
  } else if (strcmp(command, "update_playlist") == 0) {
    return update_playlist(arg);
  } else if (strcmp(command, "update_software") == 0) {
    return update_software(arg);
  } else if (strcmp(command, "delete_all_file") == 0) {
    return delete_all_file(arg);
  }

  return false;
}
