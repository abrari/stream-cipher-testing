#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bytes.h"
#include "stats.h"
#include "salsa20/salsa20.h"

#define KEYLEN  16
#define IVLEN   8

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
        uint8_t *keystream = calloc(keystreamlen, sizeof(uint8_t));

        s20_crypt(key, S20_KEYLEN_128, IV, 0, keystream, keystreamlen);
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

    return chisqr(4, chi_sq(5, o_freq, e_freq));

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
        uint8_t *keystream = calloc(keystreamlen, sizeof(uint8_t));

        s20_crypt(key, S20_KEYLEN_128, IV, 0, keystream, keystreamlen);
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

    return chisqr(4, chi_sq(5, o_freq, e_freq));

}