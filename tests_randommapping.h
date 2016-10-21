#include <stdint.h>

#ifndef STREAM_CIPHER_TESTS_RANDOMMAPPING_H
#define STREAM_CIPHER_TESTS_RANDOMMAPPING_H

double rho_test(uint8_t *key, uint32_t r, uint32_t l);
double coverage_test(uint8_t *key, uint32_t r, uint32_t l);
double dpcoverage_test(uint8_t *key, uint32_t r, uint32_t l, uint32_t k);

#endif //STREAM_CIPHER_TESTS_RANDOMMAPPING_H
