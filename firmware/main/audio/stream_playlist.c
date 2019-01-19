#include "audio/stream_playlist.h"

#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "util/strings.h"
#include "util/http.h"

static const char *TAG = "stream_playlist";

static bool fetch_playlist_by_url(const char *url, uint8_t *buffer, size_t buffer_size);

bool stream_playlist_parse_file(const char *fname, char *result, size_t result_length) {
  bool ok = false;
  FILE *f = fopen(fname, "r");
  size_t line_max_length = result_length + 12;
  char *line = malloc(line_max_length);

  while(fgets(line, line_max_length, f) != NULL) {
    if (line[0] == '#') {
      continue;
    }
    string_chomp(line);
    if (strncmp(line, "http", 4) == 0) {
      strncpy(result, line, result_length);
      ok = true;
      break;
    }
    if (strncmp(line, "File1=", 6) == 0) { // pls format
      strncpy(result, &line[6], result_length);
      ok = true;
      break;
    }
  }

  if (ok && (strstr(result, ".pls") != NULL)) { // bullshit format - link to a pls file instead of stream itself
    ESP_LOGI(TAG, "fetching actual playlist from %s", result);
    const size_t buffer_size = 1024;
    uint8_t *buffer = malloc(buffer_size);
    if (fetch_playlist_by_url(result, buffer, buffer_size)) {
      buffer[buffer_size - 1] = '\0'; // just in case
      char *saveptr = NULL;
      char *delims = "\n";
      char *tok = strtok_r((char *)buffer, delims, &saveptr);
      bool done = false;
      while (tok != NULL) {
        if (strncmp(tok, "File1=", 6) == 0) { // pls format
          string_chomp(tok);
          strncpy(result, &tok[6], result_length);
          done = true;
          break;
        }
        tok = strtok_r(NULL, delims, &saveptr);
      }
      ok = done;
    }
    free(buffer);
  }

  free(line);
  fclose(f);
  return ok;
}

static bool fetch_playlist_by_url(const char *url, uint8_t *buffer, size_t buffer_size) {
  bool ok = false;
  size_t content_length = 0;
  int response_status = 0;
  uint8_t retries = 50;
  do {
    response_status = http_get(url, buffer, buffer_size, &content_length);
    if (response_status == 200 || response_status == 201) {
      ok = true;
      break;
    }
    if (content_length > buffer_size) {
      ESP_LOGE(TAG, "expected max %d bytes, got %d", buffer_size, content_length);
      ok = false;
      break;
    }
  } while(retries-- > 0);
  return ok;
}

bool stream_is_stream_playlist(const char *fname) {
  if (!string_ends_with(fname, ".m3u") && !string_ends_with(fname, ".pls")) {
    return false;
  }

  FILE *f = fopen(fname, "r");
  const size_t line_max_length = 512;
  char *line = malloc(line_max_length);

  bool result = false;
  while(fgets(line, line_max_length, f) != NULL) {
    if (line[0] == '#') {
      continue;
    }
    string_chomp(line);
    if (strncmp(line, "http", 4) == 0) {
      result = true;
      break;
    }
    if (strncmp(line, "File1=", 6) == 0) { // pls format
      result = true;
      break;
    }
  }
  free(line);
  return result;
}
