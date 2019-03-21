#include "web/auth.h"

#include <string.h>
#include "esp_system.h"
#include "util/base64.h"
#include "config/config.h"
#include <sys/socket.h>

static void reject(httpd_req_t *req);
static bool skip_auth(httpd_req_t *req);

bool auth_check(httpd_req_t *req) {
  if (skip_auth(req)) {
    return true;
  }

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
  base64_decode(buf, base64, strlen(base64));

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
  httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm = \"slon-ds player\"");
  httpd_resp_set_status(req, "401");
  const char *txt = "Unauthorized";
  httpd_resp_send(req, txt, strlen(txt));
}

static bool skip_auth(httpd_req_t *req) {
  int new_fd = httpd_req_to_sockfd(req);
  char ip[INET6_ADDRSTRLEN] = { 0 };
  struct sockaddr_in6 addr_to;
  socklen_t addr_to_len = sizeof(addr_to);
  getsockname(new_fd, (struct sockaddr *)&addr_to, &addr_to_len);
  inet_ntop(AF_INET, &addr_to.sin6_addr.un.u32_addr[3], ip, sizeof(ip));

  config_ip_addr_t ap_ad = config_ap_static_ip();
  char ap_ip[INET6_ADDRSTRLEN] = { 0 };
  snprintf(ap_ip, sizeof(ap_ip), "%d.%d.%d.%d", ap_ad.oct1, ap_ad.oct2, ap_ad.oct3, ap_ad.oct4);

  return strncmp(ap_ip, ip, INET6_ADDRSTRLEN) == 0;
}
