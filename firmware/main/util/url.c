#include "util/url.h"

#include <string.h>
#include <stdio.h>

#include "yuarel.h"

bool url_parse(const char *uri, url_t *addr) {
  memset(addr, 0, sizeof(url_t));
  
  /*if (sscanf(uri, "%9[^:]://%255[^:]:%5[^/]%255[^\n]", addr->protocol, addr->host, addr->port, addr->path) == 4) {
  } else if (sscanf(uri, "%9[^:]://%255[^/]%255[^\n]", addr->protocol, addr->host, addr->path) == 3) {
  } else if (sscanf(uri, "%9[^:]://%255[^:]:%5[^\n]", addr->protocol, addr->host, addr->port) == 3) {
  } else if (sscanf(uri, "%9[^:]://%255[^\n]", addr->protocol, addr->host) == 2) {
  } else {
    return false;
  }*/

  struct yuarel url;
  if (yuarel_parse(&url, (char *)uri) < 0) {
		return false;
	}
  if (url.scheme != NULL) {
    strncpy(addr->protocol, url.scheme, URL_PROTOCOL_MAXLEN - 1);
  }
  if (url.host != NULL) {
    strncpy(addr->host, url.host, URL_HOST_MAXLEN - 1);
  }
  if (url.path != NULL) {
    addr->path[0] = '/';
    strncpy(addr->path + 1, url.path, URL_PATH_MAXLEN - 2);
  }
  if (url.username != NULL) {
    strncpy(addr->username, url.username, URL_USERNAME_MAXLEN - 1);
  }
  if (url.password != NULL) {
    strncpy(addr->password, url.password, URL_PASSWORD_MAXLEN - 1);
  }
  snprintf(addr->port, URL_PORT_MAXLEN, "%d", url.port);

  if (strcmp(addr->port, "") == 0 || strcmp(addr->port, "0") == 0) {
    if (strcmp(addr->protocol, "https") == 0) {
      strncpy(addr->port, "443", URL_PORT_MAXLEN - 1);
    } else if (strcmp(addr->protocol, "http") == 0) {
      strncpy(addr->port, "80", URL_PORT_MAXLEN -1) ;
    }
  }
  if (strcmp(addr->path, "") == 0) {
    strcpy(addr->path, "/");
  }

  return true;
}
