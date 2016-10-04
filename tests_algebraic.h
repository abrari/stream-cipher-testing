#include <stdint.h>

#ifndef STREAM_CIPHER_TESTS_ALGEBRAIC_H
#define STREAM_CIPHER_TESTS_ALGEBRAIC_H

#define KEYLEN  16
#define IVLEN   8
#define IVLEN_BIT (IVLEN * 8)

double linear_span_test(uint8_t key[], int m, int N);

#endif //STREAM_CIPHER_TESTS_ALGEBRAIC_H
