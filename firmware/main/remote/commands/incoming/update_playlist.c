#include "remote/commands/incoming.h"
#include <stdio.h>
#include "pdjson.h"
#include <errno.h>
#include "esp_system.h"
#include "esp_log.h"
#include "util/tempfile.h"
#include "playlist/cloud/downloader.h"
#include "remote/commands/outgoing.h"

static const char *TAG = "update_playlist";

static bool extract_playlist(const char *src_file_path, const char *src_string, const char *dst_file_path);

bool update_playlist(IncomingCommandArgument_t *arg) {
  if (arg == NULL) {
    return false;
  }

  Tempfile_t *playlist = tempfile_create();
  if (playlist == NULL) {
    ESP_LOGE(TAG, "cannot create tempfile for playlist");
    return false;
  }

  tempfile_close(playlist); // don't need it open here

  bool ok = true;
  if (arg->data) {
    ok = extract_playlist(NULL, arg->data, playlist->path);
  } else if (arg->datafile) {
    ok = extract_playlist(arg->datafile->path, NULL, playlist->path);
  } else {
    ESP_LOGE(TAG, "neither datafile or data provided");
  }
  if (ok) {
    ok = cloud_downloader_start(playlist, arg->sequence);
    if (!ok) {
      ESP_LOGE(TAG, "error starting playlist download");
      AckCommandArgs_t a = {
        .sequence = arg->sequence,
        .message = "cannot start playlist download"
      };
      outgoing_command(ACK_FAIL, &a, NULL);
    }
  } else {
    ESP_LOGE(TAG, "error extracting playlist");
  }

  return ok;
}

static bool extract_playlist(const char *src_file_path, const char *src_string, const char *dst_file_path) {
  FILE *src_file = NULL;

  if (src_file_path != NULL) {
    src_file = fopen(src_file_path, "r");
    if (src_file == NULL) {
      return false;
    }
    setvbuf(src_file, NULL, _IOFBF, 1024);
  } else if (src_string != NULL) {
    src_file = NULL;
  } else {
    return false;
  }

  uint8_t buffer = 0;
  size_t string_read_pointer = 0;

  remove(dst_file_path);
  FILE *dst_file = fopen(dst_file_path, "a");
  if (dst_file == NULL) {
    return false;
  }

  bool found_first_brace_open = false;
  bool found_second_brace_open = false;
  int8_t nesting = 0;

  while (src_file == NULL ? ((buffer = src_string[string_read_pointer++]) != '\0') : (fread(&buffer, 1, 1, src_file) > 0)) {

    if (found_second_brace_open) {
      if (buffer == '{') {
        nesting++;
      } else if (buffer == '}') {
        nesting--;
      }

      if (nesting < 0) {
        fwrite("}", 1, 1, dst_file);
        break;
      } else {
        fwrite(&buffer, 1, 1, dst_file);
      }
    } else {
      if (buffer == '{') {
        if (found_first_brace_open) {
          found_second_brace_open = true;
          fwrite("{", 1, 1, dst_file);
        } else {
          found_first_brace_open = true;
        }
      }
    }
  }

  fclose(dst_file);
  if (src_file != NULL) {
    fclose(src_file);
  }
  return true;
}
