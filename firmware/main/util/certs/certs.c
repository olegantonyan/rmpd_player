#include "util/certs/certs.h"
#include "config/config.h"

extern const char certs_pem_start[] asm("_binary_certs_pem_start");
extern const char certs_pem_end[]   asm("_binary_certs_pem_end");

const char *certs() {
  if (config_disable_tls_certs_verification()) {
    return NULL;
  }
  return certs_pem_start;
}
