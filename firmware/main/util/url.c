#include "util/url.h"

#include <string.h>
#include <stdio.h>

bool url_parse(const char *uri, url_t *addr) {
  memset(addr, 0, sizeof(url_t));
  if (sscanf(uri, "%9[^:]://%255[^:]:%5[^/]%255[^\n]", addr->protocol, addr->host, addr->port, addr->path) == 4) {
  } else if (sscanf(uri, "%9[^:]://%255[^/]%255[^\n]", addr->protocol, addr->host, addr->path) == 3) {
  } else if (sscanf(uri, "%9[^:]://%255[^:]:%5[^\n]", addr->protocol, addr->host, addr->port) == 3) {
  } else if (sscanf(uri, "%9[^:]://%255[^\n]", addr->protocol, addr->host) == 2) {
  } else {
    return false;
  }

  if (strcmp(addr->port, "") == 0) {
    if (strcmp(addr->protocol, "https") == 0) {
      strcpy(addr->port, "443");
    } else if (strcmp(addr->protocol, "http") == 0) {
      strcpy(addr->port, "80");
    }
  }
  if (strcmp(addr->path, "") == 0) {
    strcpy(addr->path, "/");
  }

  return true;
}
