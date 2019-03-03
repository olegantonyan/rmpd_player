#include "util/certs/certs.h"

extern const char letsencrypt1_pem_start[] asm("_binary_letsencrypt1_pem_start");
extern const char letsencrypt1_pem_end[]   asm("_binary_letsencrypt1_pem_end");

extern const char letsencrypt2_pem_start[] asm("_binary_letsencrypt2_pem_start");
extern const char letsencrypt2_pem_end[]   asm("_binary_letsencrypt2_pem_end");

static const char *h[CERTS_TOTAL] = { letsencrypt1_pem_start, letsencrypt2_pem_start };

const char *certs(size_t index) {
  if (index >= CERTS_TOTAL) {
    return NULL;
  }
  return h[index];
}
