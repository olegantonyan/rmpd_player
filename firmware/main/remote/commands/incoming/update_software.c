#include "remote/commands/incoming.h"
#include <stdio.h>
#include "pdjson.h"
#include <errno.h>
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include "remote/commands/outgoing.h"
#include "config/config.h"
#include "system/firmware_update.h"
#include "util/url.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "update_software";

static const char *parse_json_distribution_url(json_stream *json);

bool update_software(IncomingCommandArgument_t *arg) {
  if (arg == NULL) {
    return false;
  }

  const char *distribution_url = parse_json_distribution_url(&arg->json);
  if (distribution_url == NULL || strlen(distribution_url) < 2) {
    return false;
  }

  url_t url_parsed;
  char *distribution_url_copy = malloc(strlen(distribution_url) + 1);
  distribution_url_copy[0] = '\0';
  strcpy(distribution_url_copy, distribution_url);
  bool parsed = url_parse(distribution_url_copy, &url_parsed);
  free(distribution_url_copy);
  if (!parsed) {
    return false;
  }

  char *full_url = NULL;
  bool malloced = false;
  if (url_parsed.host != NULL && strlen(url_parsed.host) > 1) { // full url
    full_url = (char *)distribution_url;
  } else {
    full_url = malloc(strlen(distribution_url) + strlen(config_server_url()) + 56);
    full_url[0] = '\0';
    strcat(full_url, config_server_url());
    strcat(full_url, distribution_url);
    malloced = true;
  }

  bool ok = false;
  if (full_url != NULL) {
    ESP_LOGI(TAG, "start firmware upgrade from url: %s", full_url);
    ok = firmware_update_start(full_url);
  }
  if (malloced) {
    free(full_url);
  }

  // TODO: add rollback and send ack after restart
  AckCommandArgs_t a = {
    .sequence = arg->sequence,
    .message = "firmware upgrade started"
  };
  if (ok) {
    outgoing_command(ACK_OK, &a);
  } else {
    outgoing_command(ACK_FAIL, &a);
  }

  return true;
}

static const char *parse_json_distribution_url(json_stream *json) {
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
          } else if (strcmp(s, "distribution_url") == 0) {
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
