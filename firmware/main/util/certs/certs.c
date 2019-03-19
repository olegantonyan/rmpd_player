#include "util/certs/certs.h"

extern const char certs_pem_start[] asm("_binary_certs_pem_start");
extern const char certs_pem_end[]   asm("_binary_certs_pem_end");

const char *certs() {
  return certs_pem_start;
}
