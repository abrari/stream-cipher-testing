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

#include "matrix.h"
#define IVLEN_BIT 64

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

    for(i=0; i<num_p_vals; i++) {
        cov_p_vals[i] = diffusion_test(two_power(10), 256);
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


//    uint32_t num_p_vals = 1, i;
//    uint8_t key[KEYLEN] = {0};
//    uint8_t *keybits = malloc(KEYLEN_BIT);
//
//    printf("Menggunakan algoritme %s\n", ALGONAME);
//
//    //byte_to_bit_array(key, KEYLEN, keybits);
//
//    uint8_t iv[IVLEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
//
//    uint8_t *keystream;
//    uint32_t keystreamlen = 16;
//    uint8_t message[16] = {0};
//    keystream = generate_keystream(key, iv, 0, keystreamlen);
//    print_bytes(keystream, keystreamlen);
//    puts("");
//    free(keystream);
//    keystream = generate_keystream(key, iv, 1, keystreamlen);
//    print_bytes(keystream, keystreamlen);
//    puts("");
//    free(keystream);


    return 0;
}
