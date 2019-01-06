#include "web/auth.h"

#include <string.h>
#include "esp_system.h"

bool auth_check(httpd_req_t *req, auth_t *pwd) {
  char buf[256] = { 0 };
  if (httpd_req_get_hdr_value_str(req, "Authorization", buf, sizeof(buf)) != ESP_OK) {
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm = \"default admin/1234\"");
    httpd_resp_set_status(req, "401");
    const char *txt = "Unauthorized";
    httpd_resp_send(req, txt, strlen(txt));
    return false;
  }

  return true;
}
