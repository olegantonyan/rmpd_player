#include "util/files.h"
#include <limits.h>     /* PATH_MAX */
#include <sys/stat.h> /* mkdir(2) */
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

size_t file_size(FILE *f) {
  fseek(f, 0, SEEK_END);
  size_t sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  return sz;
}

bool file_exists(const char *path) {
  return access(path, F_OK) != -1;
}

int mkdir_p(const char *path) {
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p;

    errno = 0;

    if (len > sizeof(_path)-1) {
      errno = ENAMETOOLONG;
      return -1;
    }
    strcpy(_path, path);

    for (p = _path + 1; *p; p++) {
      if (*p == '/') {
        /* Temporarily truncate */
        *p = '\0';

        if (mkdir(_path, S_IRWXU) != 0) {
          if (errno != EEXIST) {
            return -1;
          }
        }

        *p = '/';
      }
    }

    if (mkdir(_path, S_IRWXU) != 0) {
      if (errno != EEXIST) {
        return -1;
      }
    }

    return 0;
}

bool remove_directory(const char *path) {
  DIR *dp = opendir(path);
  if (dp == NULL) {
    return false;
  }

  while(true) {
    struct dirent *ep = readdir(dp);
    if (!ep) {
      break;
    }
    size_t newpath_len = strlen(path) + strlen(ep->d_name) + 4;
    char *newpath = malloc(newpath_len);
    snprintf(newpath, newpath_len, "%s/%s", path, ep->d_name);
    if (ep->d_type == DT_DIR) {
      if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0) {
        remove_directory(newpath);
        rmdir(newpath);
      }
    } else {
      remove(newpath);
    }
    free(newpath);
  }
  closedir(dp);

  return true;
}
