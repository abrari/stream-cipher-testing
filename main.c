#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <memory.h>

#include "stream_cipher.h"

#include "tests_algebraic.h"
#include "tests_randommapping.h"
#include "tests_correlation.h"

#include "bytes.h"
#include "stats.h"

#define two_power(n) (1u << (n))

#include "sfmt/SFMT.h"

sfmt_t mtrand;

void test_gen_keystream(uint8_t *key, uint8_t *iv) {
    printf("Keystream dengan algoritme %s:\n", ALGONAME);

    uint8_t *keystream;
    uint32_t keystreamlen = 16;
    uint8_t message[16] = {0};
    keystream = generate_keystream(key, iv, 0, keystreamlen);
    puts("index 0");
    print_bytes(keystream, keystreamlen);
    puts("");
    free(keystream);
    keystream = generate_keystream(key, iv, 1, keystreamlen);
    puts("index 1");
    print_bytes(keystream, keystreamlen);
    puts("");
    free(keystream);
}

int main() {

    sfmt_init_gen_rand(&mtrand, time(NULL));

    setvbuf(stdout, NULL, _IONBF, 0);

    const int num_p_vals = 100;
    const int N = 100;
    int i;
    double p_vals[num_p_vals], sum_p_vals;

    uint8_t *key = generate_random_bytes(KEYLEN);
    uint8_t iv[IVLEN]   = {'C', 'O', 'M', 'P', 'U', 'T', 'E', 'R'};

    double *cov_p_vals, total_cov_p_vals = 0;
    cov_p_vals = (double*) malloc(sizeof(double)*num_p_vals);

/*

algebraic tests:
    double linear_span_test(uint8_t *key, int N, int m);
    double diffusion_test(int N, int L);

correlation tests:
    double key_keystream_correlation_test(uint8_t *IV, int m);
    double iv_keystream_correlation_test(uint8_t *key, int m);
    double frame_correlation_test(unsigned int l, unsigned int m);

randommapping tests:
    double rho_test(uint8_t *key, uint32_t r, uint32_t l);
    double coverage_test(uint8_t *key, uint32_t r, uint32_t l);
    double dpcoverage_test(uint8_t *key, uint32_t r, uint32_t l, uint32_t k);

*/

    for(i=0; i<num_p_vals; i++) {
        cov_p_vals[i] = coverage_test(key, 100, 12);
        total_cov_p_vals += cov_p_vals[i];
        printf("%d: P-Val = %f\n", i+1, cov_p_vals[i]);
    }

    printf("Average P-Val = %f\n", total_cov_p_vals/num_p_vals);
    double alpha = 0.01;
    double pc = prop_under_alpha(cov_p_vals, num_p_vals, alpha);
    printf("Proportion P-Val <= %f = %f \n", alpha, pc);
    double ks = KStest(cov_p_vals, num_p_vals);
    printf("KS test P-Val = %f\n", ks);
    double cs = chi_sq_uniform(cov_p_vals, num_p_vals, 5);
    printf("ChiSq test P-Val = %f\n", cs);

    // test_gen_keystream(key, iv);

    return 0;
}
