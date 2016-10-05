#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "tests_algebraic.h"
#include "tests_randommapping.h"

int main() {

    srand(time(NULL));

    uint8_t key[KEYLEN] = {0};
    int m = 5, N = 100;

    linear_span_test(key, m, N);

    int num_p_vals = 100, i;
    double cov_p_vals[num_p_vals], total_cov_p_vals = 0;

    for(i=0; i<num_p_vals; i++) {
        cov_p_vals[i] = rho_test(key, 100, 15);
        total_cov_p_vals += cov_p_vals[i];
        printf("Coverage %d: P-Val = %f\n", i+1, cov_p_vals[i]);
    }

    printf("Coverage Average P-Val = %f\n", total_cov_p_vals/num_p_vals);

    return 0;
}
