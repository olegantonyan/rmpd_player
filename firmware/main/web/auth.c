#include "web/auth.h"

#include <string.h>
#include "esp_system.h"
#include "util/base64.h"
#include "config/config.h"

static void reject(httpd_req_t *req);

bool auth_check(httpd_req_t *req) {
  char buf[256] = { 0 };
  if (httpd_req_get_hdr_value_str(req, "Authorization", buf, sizeof(buf)) != ESP_OK) {
    reject(req);
    return false;
  }

  char base64[241] = { 0 };
  if (sscanf(buf, "Basic %249[^\n]", base64) != 1) {
    reject(req);
    return false;
  }
  base64_decode(buf, base64, strlen(buf));

  char login[16] = { 0 };
  char password[16] = { 0 };
  if (sscanf(buf, "%15[^:]:%15[^\n]", login, password) != 2) {
    reject(req);
    return false;
  }

  if (strcmp(login, config_web_login()) != 0 || strcmp(password, config_web_password()) != 0) {
    reject(req);
    return false;
  }


  return true;
}

static void reject(httpd_req_t *req) {
  httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm = \"default admin/1234\"");
  httpd_resp_set_status(req, "401");
  const char *txt = "Unauthorized";
  httpd_resp_send(req, txt, strlen(txt));
}
