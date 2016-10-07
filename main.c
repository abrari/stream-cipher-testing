#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "tests_algebraic.h"
#include "tests_randommapping.h"
#include "tests_correlation.h"

#define KEYLEN      16
#define IVLEN       8

#define two_power(n) (1u << (n))

int main() {

    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);

    const int num_p_vals = 100;
    const int N = 100;
    int i, j;
    double p_vals[num_p_vals], sum_p_vals;

    uint8_t key[KEYLEN] = {'C', 'O', 'M', 'P', 'U', 'T', 'E', 'R', ' ', 'S', 'C', 'I', 'E', 'N', 'C', 'E'};
    uint8_t iv[IVLEN]   = {'C', 'O', 'M', 'P', 'U', 'T', 'E', 'R'};

    printf("Coverage test\n\t");
    sum_p_vals = 0.0;
    for (i = 0; i < num_p_vals; ++i) {
        if (i % 10 == 0) printf("%d%% ", i);
        p_vals[i] = coverage_test(key, N, 12);
        sum_p_vals += p_vals[i];
    }
    printf("\n\taverage p-value = %f\n", sum_p_vals / num_p_vals);

    printf("Rho test\n\t");
    sum_p_vals = 0.0;
    for (i = 0; i < num_p_vals; ++i) {
        if (i % 10 == 0) printf("%d%% ", i);
        p_vals[i] = rho_test(key, N, 15);
        sum_p_vals += p_vals[i];
    }
    printf("\n\taverage p-value = %f\n", sum_p_vals / num_p_vals);

    printf("DP-coverage test\n\t");
    sum_p_vals = 0.0;
    for (i = 0; i < num_p_vals; ++i) {
        if (i % 10 == 0) printf("%d%% ", i);
        p_vals[i] = dpcoverage_test(key, N, 12, 10);
        sum_p_vals += p_vals[i];
    }
    printf("\n\taverage p-value = %f\n", sum_p_vals / num_p_vals);

    printf("Linear span test\n\t");
    sum_p_vals = 0.0;
    for (i = 0; i < num_p_vals; ++i) {
        if (i % 10 == 0) printf("%d%% ", i);
        p_vals[i] = linear_span_test(key, N, 5);
        sum_p_vals += p_vals[i];
    }
    printf("\n\taverage p-value = %f\n", sum_p_vals / num_p_vals);

    printf("Diffusion test\n\t");
    sum_p_vals = 0.0;
    for (i = 0; i < num_p_vals; ++i) {
        if (i % 10 == 0) printf("%d%% ", i);
        p_vals[i] = diffusion_test(1024, 256);
        sum_p_vals += p_vals[i];
    }
    printf("\n\taverage p-value = %f\n", sum_p_vals / num_p_vals);

    printf("Key/keystream correlation test\n\t");
    sum_p_vals = 0.0;
    for (i = 0; i < num_p_vals; ++i) {
        if (i % 10 == 0) printf("%d%% ", i);
        p_vals[i] = key_keystream_correlation_test(iv, two_power(20));
        sum_p_vals += p_vals[i];
    }
    printf("\n\taverage p-value = %f\n", sum_p_vals / num_p_vals);

    printf("IV/keystream correlation test\n\t");
    sum_p_vals = 0.0;
    for (i = 0; i < num_p_vals; ++i) {
        if (i % 10 == 0) printf("%d%% ", i);
        p_vals[i] = iv_keystream_correlation_test(key, two_power(20));
        sum_p_vals += p_vals[i];
    }
    printf("\n\taverage p-value = %f\n", sum_p_vals / num_p_vals);

    printf("Frame correlation test\n\t");
    sum_p_vals = 0.0;
    for (i = 0; i < num_p_vals; ++i) {
        if (i % 10 == 0) printf("%d%% ", i);
        p_vals[i] = frame_correlation_test(256, two_power(20));
        sum_p_vals += p_vals[i];
    }
    printf("\n\taverage p-value = %f\n", sum_p_vals / num_p_vals);

    return 0;
}
