#include <stdint.h>

#ifndef STREAM_CIPHER_TESTS_ALGEBRAIC_H
#define STREAM_CIPHER_TESTS_ALGEBRAIC_H

double linear_span_test(uint8_t key[], int m, int N);
double diffusion_test(int N, int L);

#endif //STREAM_CIPHER_TESTS_ALGEBRAIC_H
