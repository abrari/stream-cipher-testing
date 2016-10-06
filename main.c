#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "tests_algebraic.h"
#include "tests_randommapping.h"

int main() {

    srand(time(NULL));
    int num_p_vals = 20, i, j;
    uint8_t key[KEYLEN] = {0xA0, 0xF1, 0x02, 0x0A, 0x0C, 0x0F};
    uint8_t bitkey[KEYLEN_BIT];

//    byte_to_bit_array(key, KEYLEN, bitkey);
//
//    for(j=0; j<16; j++) {
//        printf("%d", bitkey[j]);
//    }
//    printf("\n");
//
//    for(i=0; i<16; i++) {
//        flip_bit(key, i);
//        byte_to_bit_array(key, KEYLEN, bitkey);
//        for(j=0; j<16; j++) {
//            printf("%d", bitkey[j]);
//        }
//        printf("\n");
//    }

//    // LINEAR SPAN
//    double lspan_p_vals[num_p_vals], total_lspan_p_vals = 0;
//
//    for(i=0; i<num_p_vals; i++) {
//        int m = 5, N = 100;
//        lspan_p_vals[i] = linear_span_test(key, m, N);
//        total_lspan_p_vals += lspan_p_vals[i];
//        printf("Linearspan %d: P-Val = %f\n", i+1, lspan_p_vals[i]);
//    }
//
//    printf("Linearspan Average P-Val = %f\n", total_lspan_p_vals/num_p_vals);
//
    // RHO
//    double cov_p_vals[num_p_vals], total_cov_p_vals = 0;
//
//    for(i=0; i<num_p_vals; i++) {
//        cov_p_vals[i] = dpcoverage_test(key, 10, 12, 10);
//        total_cov_p_vals += cov_p_vals[i];
//        printf("Rho %d: P-Val = %f\n", i+1, cov_p_vals[i]);
//    }
//
//    printf("Rho Average P-Val = %f\n", total_cov_p_vals/num_p_vals);

    // Diffusion
    double diff_p_vals[num_p_vals], total_diff_p_vals = 0;

    for(i=0; i<num_p_vals; i++) {
        diff_p_vals[i] = diffusion_test(1024, 128);
        total_diff_p_vals += diff_p_vals[i];
        printf("Diffusion %d: P-Val = %f\n", i+1, diff_p_vals[i]);
    }

    printf("Diffusion Average P-Val = %f\n", total_diff_p_vals/num_p_vals);

//    printf("%f\n", chisqr(4, 13.2767));

    return 0;
}
