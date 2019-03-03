#include "remote/commands/incoming.h"
#include <stdio.h>
#include "pdjson.h"
#include <errno.h>
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include "clock/clock.h"
#include "remote/commands/outgoing.h"
#include "config/config.h"

static const char *TAG = "update_setting";

static const char *parse_json_timezone(json_stream *json);

bool update_setting(IncomingCommandArgument_t *arg) {
  if (arg == NULL) {
    return false;
  }

  bool ok = true;
  char *message = "";
  const char *tz = parse_json_timezone(&arg->json);
  if (tz != NULL && strlen(tz) > 1) {
    ok = clock_set_timezone(tz);
    if (ok) {
      message = "timezone set successfully";
      config_save_timezone(tz);
    }
  }

  AckCommandArgs_t a = {
    .sequence = arg->sequence,
    .message = message
  };
  if (ok) {
    outgoing_command(ACK_OK, &a, NULL);
  } else {
    outgoing_command(ACK_FAIL, &a, NULL);
  }

  return true;
}

static const char *parse_json_timezone(json_stream *json) {
  size_t len = 0;
  bool key_found = false;
  enum json_type t = JSON_ERROR;
  do {
    t = json_next(json);
    switch(t) {
      case JSON_STRING: {
        if (json_get_depth(json) == 1) {
          const char *s = json_get_string(json, &len);
          if (key_found) {
            return s;
          } else if (strcmp(s, "time_zone_tzinfo") == 0) {
            key_found = true;
          }
        }
        break;
      }

      default:
        break;
      case JSON_ERROR:
        ESP_LOGE(TAG, "json parse error: %s line %d pos %d", json_get_error(json), json_get_lineno(json), json_get_position(json));
        break;
    }

  } while (t != JSON_DONE && t != JSON_ERROR);
  return NULL;
}
