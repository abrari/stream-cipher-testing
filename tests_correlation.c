#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bytes.h"
#include "stats.h"
#include "stream_cipher.h"

double key_keystream_correlation_test(uint8_t *IV, int m) {

    float e_prob[5], e_freq[5], o_freq[5] = {0};
    int bin[5];
    int i, j;

    if (KEYLEN == 10 /*byte*/) {
        bin[0] = 35;
        bin[1] = 38;
        bin[2] = 41;
        bin[3] = 44;
        bin[4] = 80;
        e_prob[0] = 0.157153;
        e_prob[1] = 0.211624;
        e_prob[2] = 0.262446;
        e_prob[3] = 0.211624;
        e_prob[4] = 0.157153;
    } else if (KEYLEN == 16 /*byte*/) {
        bin[0] = 58;
        bin[1] = 62;
        bin[2] = 65;
        bin[3] = 69;
        bin[4] = 128;
        e_prob[0] = 0.165468;
        e_prob[1] = 0.230035;
        e_prob[2] = 0.208994;
        e_prob[3] = 0.230035;
        e_prob[4] = 0.165468;
    } else {
        return -1.0;
    }

    for (i = 0; i < 5; i++)
        e_freq[i] = e_prob[i] * m;

    for (i = 0; i < m; ++i) {
        unsigned int w;
        uint8_t *key = generate_random_bytes(KEYLEN);
        uint32_t keystreamlen = KEYLEN;
        uint8_t *keystream = generate_keystream(key, IV, 0, keystreamlen);

        xor_bytes(key, keystream, keystream, KEYLEN);

        w = hamming_weight(keystream, KEYLEN);

        free(key);
        free(keystream);

        j = 0;
        while (w > bin[j])
            j++;
        o_freq[j]++;
    }

    // for (i = 0; i < 5; i++)
    //    printf("%d\t%f\t%f\n", i, e_freq[i], o_freq[i]);

    return chi_sq_pval(4, chi_sq(5, o_freq, e_freq));

}

double iv_keystream_correlation_test(uint8_t *key, int m) {

    float e_prob[5], e_freq[5], o_freq[5] = {0};
    int bin[5];
    int i, j;

    if (IVLEN == 8 /*byte*/) {
        bin[0] = 28;
        bin[1] = 30;
        bin[2] = 33;
        bin[3] = 35;
        bin[4] = 64;
        e_prob[0] = 0.190866;
        e_prob[1] = 0.163124;
        e_prob[2] = 0.292019;
        e_prob[3] = 0.163124;
        e_prob[4] = 0.190866;
    } else if (IVLEN == 10 /*byte*/) {
        bin[0] = 35;
        bin[1] = 38;
        bin[2] = 41;
        bin[3] = 44;
        bin[4] = 80;
        e_prob[0] = 0.157153;
        e_prob[1] = 0.211624;
        e_prob[2] = 0.262446;
        e_prob[3] = 0.211624;
        e_prob[4] = 0.157153;
    } else if (IVLEN == 16 /*byte*/) {
        bin[0] = 58;
        bin[1] = 62;
        bin[2] = 65;
        bin[3] = 69;
        bin[4] = 128;
        e_prob[0] = 0.165468;
        e_prob[1] = 0.230035;
        e_prob[2] = 0.208994;
        e_prob[3] = 0.230035;
        e_prob[4] = 0.165468;
    } else {
        return -1.0;
    }

    for (i = 0; i < 5; i++)
        e_freq[i] = e_prob[i] * m;

    for (i = 0; i < m; ++i) {
        unsigned int w;
        uint8_t *IV = generate_random_bytes(IVLEN);
        uint32_t keystreamlen = IVLEN;
        uint8_t *keystream = generate_keystream(key, IV, 0, keystreamlen);

        xor_bytes(IV, keystream, keystream, IVLEN);

        w = hamming_weight(keystream, IVLEN);

        free(IV);
        free(keystream);

        j = 0;
        while (w > bin[j])
            j++;
        o_freq[j]++;
    }

    // for (i = 0; i < 5; i++)
    //    printf("%d\t%f\t%f\n", i, e_freq[i], o_freq[i]);

    return chi_sq_pval(4, chi_sq(5, o_freq, e_freq));

}

// http://pastebin.com/PWdxg6ky

double frame_correlation_test(unsigned int l, unsigned int m) {   // l is in bits

    float e_prob[5], e_freq[5], o_freq[5] = {0};
    int bin[5];

    if (m == two_power(20)) {
        bin[0] = 523857;
        bin[1] = 524158;
        bin[2] = 524417;
        bin[3] = 524718;
        bin[4] = 1048576;
        e_prob[0] = 0.200224;
        e_prob[1] = 0.199937;
        e_prob[2] = 0.199677;
        e_prob[3] = 0.199937;
        e_prob[4] = 0.200224;
    } else {
        calculate_bins(m, bin, e_prob);
    }

    int i;
    for (i = 0; i < 5; i++)
        e_freq[i] = e_prob[i] * l;


    uint8_t *key = generate_random_bytes(KEYLEN);
    uint8_t *iv  = generate_random_bytes(IVLEN);

    unsigned int *w = calloc(l, sizeof(unsigned int));

    int j, k;
    for (i = 0; i < m; ++i) {
        uint32_t keystreamlen = l / 8;

        uint8_t *keystream = generate_keystream(key, iv, 0, keystreamlen);
        uint8_t *keystream_bit = malloc(l);

        byte_to_bit_array(keystream, keystreamlen, keystream_bit);

        for (j = 0; j < l; ++j) {
            w[j] += keystream_bit[j];
        }

        // print_bytes(iv, IVLEN); printf("\n");
        // print_bytes(keystream, keystreamlen); printf("\n");

        inc_byte(iv, IVLEN-1);
        free(keystream);
        free(keystream_bit);
    }

    // chi-square testing of w

    for (j = 0; j < l; ++j) {
        k = 0;
        while (w[j] > bin[k])
            k++;
        o_freq[k]++;
    }

    free(key);
    free(iv);

    return chi_sq_pval(4, chi_sq(5, o_freq, e_freq));

}









