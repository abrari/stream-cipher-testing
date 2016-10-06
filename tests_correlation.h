#include <stdint.h>

#ifndef STREAM_CIPHER_TESTS_CORRELATION_H
#define STREAM_CIPHER_TESTS_CORRELATION_H

double key_keystream_correlation_test(uint8_t *IV, int m);
double iv_keystream_correlation_test(uint8_t *key, int m);

#endif //STREAM_CIPHER_TESTS_CORRELATION_H
