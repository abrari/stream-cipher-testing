#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "matrix.h"
#include "salsa20/salsa20.h"

#define KEYLEN  16
#define IVLEN   8
#define IVLEN_BIT (IVLEN * 8)

typedef unsigned char byte;

void print_bytes(byte *bytes, unsigned int length) {
    unsigned int i;
    for (i = 0; i < length; ++i) {
        printf("%02x", bytes[i]);
    }
}

void print_bits(byte *bits, unsigned int length) {
    unsigned int i;
    for (i = 0; i < length; ++i) {
        if (bits[i] == 0 || bits[i] == 1)
            printf("%d", bits[i]);
    }
}

void byte_to_bit_array(/*IN*/ byte *bytes, unsigned int bytelength, /*OUT*/ byte *bits) {

    unsigned int i, j;
    byte b;
    for (i = 0; i < bytelength; ++i) {
        b = bytes[i];
        for (j = 0; j < 8; ++j) {
            bits[i * 8 + j] = (b & (1 << (7 - j))) != 0;
        }
    }

}

void bit_to_byte_array(/*IN*/ byte *bits, unsigned int bitslength, /*OUT*/ byte *bytes) {

    unsigned int i, j;
    byte b;
    for (i = 0; i < bitslength / 8; ++i) {
        b = 0;
        for (j = 0; j < 8; ++j) {
            b += bits[i * 8 + j] << (7 - j);
        }
        bytes[i] = b;
    }

}

byte *generate_random_bytes(unsigned int length) {
    byte *bytes = malloc(length);
    unsigned int i;
    for (i = 0; i < length; ++i) {
        bytes[i] = rand() % 256;
    }
    return bytes;
}

byte *generate_random_bits(unsigned int bitlength) {

    assert(bitlength % 8 == 0);

    byte *bytes = generate_random_bytes(bitlength / 8);
    byte *bits  = malloc(bitlength);
    byte_to_bit_array(bytes, bitlength / 8, bits);
    free(bytes);

    return bits;

}

void xor_bytes(/*IN*/ byte *b1, /*IN*/ byte *b2, /*OUT*/ byte *result, unsigned int length) {

    unsigned int i;
    for (i = 0; i < length; ++i) {
        result[i] = b1[i] ^ b2[i];
    }

}

int main() {

    srand(time(NULL));

    int m = 5;
    int _2m = (1u << m);
    int N = 100;
    int R[N];

    int num_fullrank = 0, num_onelessfullrank = 0, num_lowrank = 0;
    float prob_fullrank, prob_onelessfullrank, prob_lowrank;

    uint8_t key[KEYLEN] = {0};

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
        int j, k, mask;
        for (j = 0; j < _2m; ++j) {
            IV[j] = calloc(IVLEN_BIT, sizeof(byte *));
            for (k = 0; k < m; ++k) {
                mask = (j >> k) & 1;
                if (mask == 1) {
                    xor_bytes(alpha[k], IV[j], IV[j], IVLEN_BIT);
                }
            }
        }

        // generate keystream for each generated IV
        byte **Z = malloc(_2m * sizeof(byte *));
        byte *IV_byte;
        uint8_t *keystream_byte;
        int keystream_byte_len = 1u << (m - 3);     // 2^(m-3)

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

        //    printf("Alpha: \n");
        //    display_matrix(m, IVLEN_BIT, alpha);
        //    printf("IV: \n");
        //    display_matrix(_2m, IVLEN_BIT, IV);
        //    printf("M: \n");
        //    display_matrix(_2m, _2m, Z);

        // calculate rank of M (or Z)
        R[i] = computeRank(_2m, _2m, Z);
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

    printf("R  = m   : %2d (%.5f)\n", num_fullrank, prob_fullrank);
    printf("R  = m-1 : %2d (%.5f)\n", num_onelessfullrank, prob_onelessfullrank);
    printf("R <= m-2 : %2d (%.5f)\n", num_lowrank, prob_lowrank);

    return 0;
}













