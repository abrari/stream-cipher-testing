#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "tests_algebraic.h"
#include "tests_randommapping.h"

int main() {

    srand(time(NULL));
    int num_p_vals = 100, i;
    uint8_t key[KEYLEN] = {0};

    // LINEAR SPAN
    double lspan_p_vals[num_p_vals], total_lspan_p_vals = 0;

    for(i=0; i<num_p_vals; i++) {
        int m = 5, N = 100;
        lspan_p_vals[i] = linear_span_test(key, m, N);
        total_lspan_p_vals += lspan_p_vals[i];
        printf("Linearspan %d: P-Val = %f\n", i+1, lspan_p_vals[i]);
    }

    printf("Linearspan Average P-Val = %f\n", total_lspan_p_vals/num_p_vals);

    // RHO
    double cov_p_vals[num_p_vals], total_cov_p_vals = 0;

    for(i=0; i<num_p_vals; i++) {
        cov_p_vals[i] = rho_test(key, 100, 15);
        total_cov_p_vals += cov_p_vals[i];
        printf("Coverage %d: P-Val = %f\n", i+1, cov_p_vals[i]);
    }

    printf("Coverage Average P-Val = %f\n", total_cov_p_vals/num_p_vals);

    return 0;
}
