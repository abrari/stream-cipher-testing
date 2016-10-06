#include <stdint.h>

#ifndef STREAM_CIPHER_TESTS_RANDOMMAPPING_H
#define STREAM_CIPHER_TESTS_RANDOMMAPPING_H

double rho_test(uint8_t *key, int r, int l);
double coverage_test(uint8_t *key, int r, int l);
double dpcoverage_test(uint8_t *key, int r, int l, int k);

#endif //STREAM_CIPHER_TESTS_RANDOMMAPPING_H
