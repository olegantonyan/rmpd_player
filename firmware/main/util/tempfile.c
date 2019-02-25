#include "util/tempfile.h"
#include <sys/stat.h>
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include <string.h>
#include <dirent.h>

Tempfile_t *tempfile_create() {
  struct stat st = { 0 };
  if(stat(TEMPFILE_DIR, &st) < 0) {
    if(mkdir(TEMPFILE_DIR, 0777) != 0) {
      return false;
    }
    f_chmod(TEMPFILE_DIR, AM_HID, AM_HID);
  }
  Tempfile_t *tf = malloc(sizeof(Tempfile_t));
  if (tf == NULL) {
    return NULL;
  }
  size_t pathlen = strlen(TEMPFILE_DIR) + 28;
  tf->path = malloc(pathlen);
  snprintf(tf->path, pathlen - 1, "%s/%u.tmp", TEMPFILE_DIR, esp_random());

  if (tempfile_open(tf, "ab+")) {
    return tf;
  }
  free(tf->path);
  free(tf);
  return NULL;
}

bool tempfile_remove(Tempfile_t *tf) {
  if (tf == NULL || tf->path == NULL || tf->file == NULL) {
    return false;
  }
  tempfile_close(tf);
  bool ok = remove(tf->path) == 0;
  free(tf->path);
  free(tf);
  return ok;
}

bool tempfile_close(Tempfile_t *tf) {
  if (tf == NULL || tf->file == NULL) {
    return false;
  }
  fflush(tf->file);
  return fclose(tf->file) == 0;
}

bool tempfile_open(Tempfile_t *tf, const char *mode) {
  if (tf == NULL || tf->path == NULL) {
    return false;
  }
  tf->file = fopen(tf->path, mode);
  return tf->file != NULL;
}

void tempfile_init() {
  DIR* dp = opendir(TEMPFILE_DIR);
  if (dp == NULL) {
    return;
  }

  char path[PATH_MAX + 1] = { 0 };
  struct dirent* ep;
  while((ep = readdir(dp))) {
    if (ep->d_type == DT_REG ) {
      memset(path, 0, sizeof(path));
      snprintf(path, sizeof(path), "%s/%s", TEMPFILE_DIR, ep->d_name);
      remove(path);
    }
  }
  closedir(dp);
}
