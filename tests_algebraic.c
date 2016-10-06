#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "bytes.h"
#include "stats.h"
#include "salsa20/salsa20.h"

#define KEYLEN      16
#define KEYLEN_BIT  (KEYLEN * 8)
#define IVLEN       8
#define IVLEN_BIT   (IVLEN * 8)

double linear_span_test(uint8_t key[], int m, int N) {

    unsigned int _2m = (1u << m);
    unsigned int R[N];

    int num_fullrank = 0, num_onelessfullrank = 0, num_lowrank = 0;
    float prob_fullrank, prob_onelessfullrank, prob_lowrank;

    int i;
    for (i = 0; i < N; ++i) {

        int a;
        int independent = 0;
        int ranktest;

        // generate random, linearly independent binary vectors
        byte **alpha = malloc(m * sizeof(byte *));
        do {
            for (a = 0; a < m; ++a) {
                alpha[a] = generate_random_bits(IVLEN_BIT);
            }
            ranktest = computeRank(m, IVLEN_BIT, alpha);
            if (ranktest == m) independent = 1;
            else {
                for (a = 0; a < m; ++a) free(alpha[a]);
            }
        }
        while (!independent);

        // generate all linear combinations of alpha
        byte **IV = malloc(_2m * sizeof(byte *));
        int j, k, bit;
        for (j = 0; j < _2m; ++j) {
            IV[j] = calloc(IVLEN_BIT, sizeof(byte *));
            for (k = 0; k < m; ++k) {
                bit = (j >> k) & 1;
                if (bit == 1) {
                    xor_bytes(alpha[k], IV[j], IV[j], IVLEN_BIT);
                }
            }
        }

        // get keystream for each generated IV
        byte **Z = malloc(_2m * sizeof(byte *));
        byte *IV_byte;
        uint8_t *keystream_byte;
        unsigned int keystream_byte_len = 1u << (m - 3);     // 2^(m-3)

        for (j = 0; j < _2m; ++j) {
            IV_byte = malloc(IVLEN);
            keystream_byte = calloc(keystream_byte_len, sizeof(uint8_t));

            bit_to_byte_array(IV[j], IVLEN_BIT, IV_byte);
            s20_crypt(key, S20_KEYLEN_128, IV_byte, 0, keystream_byte, keystream_byte_len);

            Z[j] = malloc(_2m);
            byte_to_bit_array(keystream_byte, keystream_byte_len, Z[j]);

            free(IV_byte);
            free(keystream_byte);
        }

        // calculate rank of M (or Z)
        R[i] = computeRank(_2m, _2m, Z);

        //    printf("Alpha: \n");
        //    display_matrix(m, IVLEN_BIT, alpha);
        //    printf("IV: \n");
        //    display_matrix(_2m, IVLEN_BIT, IV);
        //    printf("M: \n");
        //    display_matrix(_2m, _2m, Z);
        //    printf("R[%d] = %d\n", i, R[i]);

        delete_matrix(m, alpha);
        delete_matrix(_2m, IV);
        delete_matrix(_2m, Z);

        if (R[i] == _2m) num_fullrank++;
        else if (R[i] == _2m - 1) num_onelessfullrank++;
        else if (R[i] <= _2m - 2) num_lowrank++;

    }

    prob_fullrank = (float) num_fullrank / N;
    prob_onelessfullrank = (float) num_onelessfullrank / N;
    prob_lowrank = (float) num_lowrank / N;

    // printf("R  = m   : %2d (%.5f)\n", num_fullrank, prob_fullrank);
    // printf("R  = m-1 : %2d (%.5f)\n", num_onelessfullrank, prob_onelessfullrank);
    // printf("R <= m-2 : %2d (%.5f)\n", num_lowrank, prob_lowrank);

    double chisq = pow((double) num_fullrank - 0.2888 * N, 2) / (0.2888 * N) \
                 + pow((double) num_onelessfullrank - 0.5776 * N, 2) / (0.5776 * N) \
                 + pow((double) num_lowrank - 0.1336 * N, 2) / (0.1336 * N);
    double pval  = exp(-chisq / 2);

    // printf("chisq = %lf\n", chisq);
    // printf("p-val = %lf\n", pval);

    return pval;

}

double diffusion_test(int N, int L) {
    float e_prob[5] = {0.199405, 0.189855, 0.221481, 0.189855, 0.199405};
    int bin[5] = {498, 507, 516, 525, 1024}, LB = L>>3;
    float e_freq[5], o_freq[5] = {0};

    if (N != 1024) return -1;

    unsigned i, j, m;

    for (i = 0; i < 5; i++)
        e_freq[i] = e_prob[i] * ((KEYLEN_BIT+IVLEN_BIT)*L);

    byte *key, *iv, *keystream1, *keystream2;
    int **M;
    byte *diff_bits = calloc(L, sizeof(uint8_t));

    M = create_integer_matrix(KEYLEN_BIT+IVLEN_BIT, L);

    for(i=0; i<N; i++) {
        key = generate_random_bytes(KEYLEN);
        iv = generate_random_bytes(IVLEN);

        uint8_t *keystream1 = calloc(LB, sizeof(uint8_t));
        s20_crypt(key, S20_KEYLEN_128, iv, 0, keystream1, LB);

        for(j=0; j<(KEYLEN_BIT+IVLEN_BIT); j++) {

            // flip the current bit;
            if (j<KEYLEN_BIT) {
                flip_bit(key, j);
            }
            else {
                flip_bit(iv, j-KEYLEN_BIT);
            }

            uint8_t *keystream2 = calloc(LB, sizeof(uint8_t));
            s20_crypt(key, S20_KEYLEN_128, iv, 0, keystream2, LB);

            uint8_t *diff = calloc(LB, sizeof(uint8_t));
            xor_bytes(keystream1, keystream2, diff, LB);

            byte_to_bit_array(diff, LB, diff_bits);

            for(m=0; m<L; m++) {
                M[j][m] += diff_bits[m];
            }

            // flip the bit to restore its original value;
            if (j<KEYLEN_BIT) {
                flip_bit(key, j);
            }
            else {
                flip_bit(iv, j-KEYLEN_BIT);
            }

            free(diff);
            free(keystream2);
        }

        free(keystream1);
        free(iv);
        free(key);

    }

    for(i=0; i<(KEYLEN_BIT+IVLEN_BIT); i++) {
        for(j=0; j<L; j++) {
            m = 0;
            while (M[i][j] > bin[m])
                m++;
            o_freq[m]++;

        }

    }

    for (i = 0; i < 5; i++)
        printf("%d\t%f\t%f\n", i, e_freq[i], o_freq[i]);

    return chisqr(4, chi_sq(5, o_freq, e_freq));
}
