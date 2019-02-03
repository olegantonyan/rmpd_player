#ifndef __CERTS_H
#define __CERTS_H

extern const char letsencrypt1_pem_start[] asm("_binary_letsencrypt1_pem_start");
extern const char letsencrypt1_pem_end[]   asm("_binary_letsencrypt1_pem_end");

extern const char letsencrypt2_pem_start[] asm("_binary_letsencrypt2_pem_start");
extern const char letsencrypt2_pem_end[]   asm("_binary_letsencrypt2_pem_end");

#define CERTS_TOTAL 2
const char *certs[CERTS_TOTAL] = { letsencrypt1_pem_start, letsencrypt2_pem_start };

#endif
