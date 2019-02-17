#include "util/tempfile.h"
#include <sys/stat.h>
#include "esp_system.h"
#include "esp_vfs_fat.h"

bool tempfile_create(Tempfile_t *tf) {
  struct stat st = { 0 };
  if(stat(TEMPFILE_DIR, &st) < 0) {
    if(mkdir(TEMPFILE_DIR, 0777) != 0) {
      return false;
    }
    f_chmod(TEMPFILE_DIR, AM_HID, AM_HID);
  }
  tf->path = malloc(PATH_MAX);
  snprintf(tf->path, PATH_MAX - 1, "%s/%u.tmp", TEMPFILE_DIR, esp_random());

  return tempfile_open(tf);
}

bool tempfile_remove(Tempfile_t *tf) {
  if (tf == NULL || tf->path == NULL || tf->file == NULL) {
    return false;
  }
  tempfile_close(tf);
  bool ok = remove(tf->path) == 0;
  free(tf->path);
  return ok;
}

bool tempfile_close(Tempfile_t *tf) {
  if (tf == NULL || tf->file == NULL) {
    return false;
  }
  fflush(tf->file);
  return fclose(tf->file) == 0;
}

bool tempfile_open(Tempfile_t *tf) {
  if (tf == NULL || tf->path == NULL) {
    return false;
  }
  tf->file = fopen(tf->path, "ab+");
  return tf->file != NULL;
}

void tempfile_init() {
  // TODO clean the directory
}
