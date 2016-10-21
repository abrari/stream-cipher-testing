#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

#include "bytes.h"
#include "stats.h"
#include "stream_cipher.h"

#include "sfmt/SFMT.h"

#include "tests_randommapping.h"

uint32_t find_kzeroes(uint32_t k, uint8_t *bytestream, uint32_t bytestreamlen, uint32_t *numfound) {
    uint32_t curpos = 0, bitstreamlen, zeroes = 0, found = 0;
    uint32_t numleft = k - *numfound;

    bitstreamlen = bytestreamlen * 8;

    if (*numfound) {
        while (!isset_bit(bytestream, curpos) && numleft && (curpos < bitstreamlen)) {
            numleft--;
            curpos++;
        }

        if (numleft == 0) {
            *numfound = k;
            found = 1;
        }
        else if (isset_bit(bytestream, curpos))
            *numfound = 0;
        else if (curpos == bitstreamlen) {
            *numfound = k - numleft;
            return 0;
        }
    }


    while (!found && (curpos < bitstreamlen)) {
        if (isset_bit(bytestream, curpos)) zeroes = 0;
        else zeroes++;

        if (zeroes == k) found = 1;

        curpos++;
    }

    *numfound = zeroes;

    if (found) return curpos;
    else return 0;
}

double rho_test(uint8_t *key, uint32_t r, uint32_t l) {
    assert(l == 15 || l == 20);
    float e_prob[5], e_freq[5];;
    uint32_t bin[5], o_freq[5] = {0};
    uint32_t i, j, k, temp;

    switch (l) {
        case 15:
            bin[0] = 122;
            bin[1] = 184;
            bin[2] = 246;
            bin[3] = 325;
            bin[4] = 32768;
            e_prob[0] = 0.201906;
            e_prob[1] = 0.200448;
            e_prob[2] = 0.199904;
            e_prob[3] = 0.198270;
            e_prob[4] = 0.199472;
            break;
        case 20:
            bin[0] = 685;
            bin[1] = 1036;
            bin[2] = 1386;
            bin[3] = 1838;
            bin[4] = 1048576;
            e_prob[0] = 0.200258;
            e_prob[1] = 0.200124;
            e_prob[2] = 0.199400;
            e_prob[3] = 0.200518;
            e_prob[4] = 0.199700;
            break;
        default:
            return -1;
    }

    for (i = 0; i < 5; i++)
        e_freq[i] = e_prob[i] * r;

    uint8_t ivpos[IVLEN * 8] = {0};
    int *lpos = calloc(l, sizeof(int));

    for (i = 0; i < l; i++) {
        do {
            temp = sfmt_genrand_uint32(&mtrand) % (IVLEN * 8);
        } while (ivpos[temp]);
        ivpos[temp] = 1;
        lpos[i] = temp;
        //printf("%d ", temp);
    }
    //printf("\n");

    for (i = 0; i < r; i++) {
        //printf("RUN #%d\n", i);
        uint8_t *cov = calloc(two_power(l), sizeof(uint8_t));
        uint32_t rho = 0, repeated = 0;

        uint8_t *iv;
        iv = generate_random_bytes(IVLEN);

        do {
            uint32_t keystreamlen = 3;
            uint8_t *keystream; // = calloc(keystreamlen, sizeof(uint8_t));
            keystream = generate_keystream(key, iv, 0, keystreamlen);
            //HC128(key, iv, message, keystream, messagelen);

            for (k = 0; k < l; k++) {
                if (isset_bit(keystream, k)) set_bit(iv, lpos[k]);
                else unset_bit(iv, lpos[k]);
            }

            uint32_t kspos;

            if (15 == l)
                kspos = (keystream[0] << 7) + (keystream[1] >> 1);
            else
                kspos = (keystream[0] << 12) + (keystream[1] << 4) + (keystream[2] >> 4);

            //print_bytes(keystream, keystreamlen);
            //printf("\n");
            //printf("%d\n", kspos);

            rho++;

            if (0 == cov[kspos]) cov[kspos] = 1;
            else repeated = 1;

            //free(message);
            free(keystream);
        } while (!repeated);

        j = 0;
        while (rho > bin[j])
            j++;
        o_freq[j]++;

        free(iv);
        free(cov);
    }

//    for(i=0; i<5; i++)
//        printf("%d\t%f\t%f\n", i, e_freq[i], (float)o_freq[i]);

    free(lpos);

    return chi_sq_pval(4, chi_sq(5, o_freq, e_freq));
}

double coverage_test(uint8_t *key, uint32_t r, uint32_t l) {
    assert(l == 12 || l == 14);
    float e_prob[5], e_freq[5];
    uint32_t bin[5], o_freq[5] = {0};
    uint32_t i, j, k, mask, temp;

    switch (l) {
        case 12:
            bin[0] = 2572;
            bin[1] = 2584;
            bin[2] = 2594;
            bin[3] = 2606;
            bin[4] = 4096;
            e_prob[0] = 0.199139;
            e_prob[1] = 0.204674;
            e_prob[2] = 0.197856;
            e_prob[3] = 0.203225;
            e_prob[4] = 0.195106;
            break;
        case 14:
            bin[0] = 10323;
            bin[1] = 10346;
            bin[2] = 10367;
            bin[3] = 10390;
            bin[4] = 16384;
            e_prob[0] = 0.201591;
            e_prob[1] = 0.195966;
            e_prob[2] = 0.207519;
            e_prob[3] = 0.195253;
            e_prob[4] = 0.199671;
            break;
        default:
            return -1;
    }

    for (i = 0; i < 5; i++)
        e_freq[i] = e_prob[i] * r;

    uint8_t ivpos[IVLEN_BIT] = {0};
    uint32_t *lpos = calloc(l, sizeof(uint32_t));

    for (i = 0; i < l; i++) {
        do {
            temp = sfmt_genrand_uint32(&mtrand) % (IVLEN_BIT);
        } while (ivpos[temp]);
        ivpos[temp] = 1;
        lpos[i] = temp;
        //printf("%d ", temp);
    }
    //printf("\n");

    for (i = 0; i < r; i++) {
        //printf("RUN #%d\n", i);
        uint8_t *cov = calloc(two_power(l), sizeof(uint8_t));
        uint32_t cov_freq = 0;

        uint8_t *iv;
        iv = generate_random_bytes(IVLEN);

        for (j = 0; j < two_power(l); j++) {
            //print_bytes(iv, IVLEN);
            mask = two_power(l - 1);
            for (k = 0; k < l; k++) {
                if (j & mask) set_bit(iv, lpos[k]);
                else unset_bit(iv, lpos[k]);
                //printf("%d\n", mask);
                mask >>= 1;
            }

            //print_bytes(iv, IVLEN);
            //printf("\n");
            //getch();
            uint32_t keystreamlen = 2;
            uint8_t *keystream; //calloc(keystreamlen, sizeof(uint8_t));
            keystream = generate_keystream(key, iv, 0, keystreamlen);

            uint32_t kspos;
            kspos = (keystream[0] << (l - 8)) + (keystream[1] >> (16 - l));
            //print_bytes(keystream, keystreamlen);
            //printf("\n");
            //printf("%d\n", kspos);
            if (0 == cov[kspos]) {
                cov[kspos] = 1;
                cov_freq++;
            }
            free(keystream);
        }

        j = 0;
        while (cov_freq > bin[j])
            j++;
        o_freq[j]++;

        free(iv);
        free(cov);
    }

//    for(i=0; i<5; i++)
//        printf("%d\t%f\t%d\n", i, e_freq[i], o_freq[i]);

    free(lpos);

    return chi_sq_pval(4, chi_sq(5, o_freq, e_freq));
}

double dpcoverage_test(uint8_t *key, uint32_t r, uint32_t l, uint32_t k) {
    assert(l == 12 || l == 14);
    float e_prob[5], e_freq[5];
    uint32_t bin[5], o_freq[5] = {0};
    uint32_t i, j, m, mask, temp;

    switch (l) {
        case 12:
            bin[0] = 2572;
            bin[1] = 2584;
            bin[2] = 2594;
            bin[3] = 2606;
            bin[4] = 4096;
            e_prob[0] = 0.199139;
            e_prob[1] = 0.204674;
            e_prob[2] = 0.197856;
            e_prob[3] = 0.203225;
            e_prob[4] = 0.195106;
            break;
        case 14:
            bin[0] = 10323;
            bin[1] = 10346;
            bin[2] = 10367;
            bin[3] = 10390;
            bin[4] = 16384;
            e_prob[0] = 0.201591;
            e_prob[1] = 0.195966;
            e_prob[2] = 0.207519;
            e_prob[3] = 0.195253;
            e_prob[4] = 0.199671;
            break;
        default:
            return -1;
    }

    for (i = 0; i < 5; i++)
        e_freq[i] = e_prob[i] * r;

    uint8_t ivpos[IVLEN_BIT] = {0};
    int *lpos = calloc(l, sizeof(int));

    for (i = 0; i < l; i++) {
        do {
            temp = sfmt_genrand_uint32(&mtrand) % (IVLEN_BIT);
        } while (ivpos[temp]);
        ivpos[temp] = 1;
        lpos[i] = temp;
        //printf("%d ", temp);
    }
    //printf("\n");

    for (i = 0; i < r; i++) {
        //printf("RUN #%d\n", i);
        int *cov = calloc(two_power(l), sizeof(int));
        int cov_freq = 0;

        uint8_t *iv;
        iv = generate_random_bytes(IVLEN);

        for (j = 0; j < two_power(l); j++) {
            //print_bytes(iv, IVLEN);
            mask = two_power(l - 1);
            for (m = 0; m < l; m++) {
                if (j & mask) set_bit(iv, lpos[m]);
                else unset_bit(iv, lpos[m]);
                //printf("%d\n", mask);
                mask >>= 1;
            }

            uint32_t keystreamlen = 1000, prevkeystreamlen = 0;
            uint8_t *keystream = NULL;
            uint32_t startlpos, numfound = 0;

            do {
                if (keystream) free(keystream);
                keystream = generate_keystream(key, iv, prevkeystreamlen, keystreamlen);
                startlpos = find_kzeroes(k, keystream, keystreamlen, &numfound);
                prevkeystreamlen += keystreamlen;
            } while (!startlpos);

            uint32_t p, q;
            p = startlpos / 8;
            q = startlpos % 8;

            uint32_t kspos;

            if (startlpos == keystreamlen * 8) {
                free(keystream);
                keystream = generate_keystream(key, iv, prevkeystreamlen, 2);

                kspos = (keystream[0] << (l - 8 + q)) + (keystream[1] >> (16 - l - q));
            }
            else if ((startlpos + l) > (keystreamlen * 8)) {

                if (p == (keystreamlen-1)) {
                    kspos = keystream[p] << (l - 8 + q);

                    free(keystream);
                    keystream = generate_keystream(key, iv, prevkeystreamlen, 2);

                    if ((l + q) <= 16)
                        kspos += (keystream[0] >> (16 - l - q));
                    else
                        kspos += (keystream[0] << (l + q - 16)) + (keystream[1] >> (24 - l - q));
                }
                else {
                    kspos = (keystream[p] << (l - 8 + q)) + (keystream[p+1] << (l + q - 16));

                    free(keystream);
                    keystream = generate_keystream(key, iv, prevkeystreamlen, 1);
                    kspos += (keystream[0] >> (24 - l - q));
                }
            }
            else {
                if ((l + q) <= 16)
                    kspos = (keystream[p] << (l - 8 + q)) + (keystream[p + 1] >> (16 - l - q));
                else
                    kspos = (keystream[p] << (l - 8 + q)) + (keystream[p + 1] << (l + q - 16)) +
                            (keystream[p + 2] >> (24 - l - q));
            }

            //printf("%u\t%u\n", startlpos, kspos);

            //print_bytes(keystream, keystreamlen);
            //printf("\n");
            //printf("%d\n", kspos);
            if (0 == cov[kspos]) {
                cov[kspos] = 1;
                cov_freq++;
            }
            free(keystream);
        }

        j = 0;
        while (cov_freq > bin[j])
            j++;
        o_freq[j]++;

        free(iv);
        free(cov);
    }

//    for (i = 0; i < 5; i++)
//        printf("%d\t%f\t%f\n", i, e_freq[i], (float) o_freq[i]);

    free(lpos);

    return chi_sq_pval(4, chi_sq(5, o_freq, e_freq));
}