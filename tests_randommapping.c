#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bytes.h"
#include "salsa20/salsa20.h"

#define KEYLEN  16
#define IVLEN   8

void random_bytes(unsigned char *bytes, unsigned int length) {
    unsigned int i;
    for (i = 0; i < length; ++i) {
        bytes[i] = rand() % 256;
    }
}

unsigned int find_kzeroes(unsigned int k, uint8_t *bytestream, unsigned int bytestreamlen, unsigned int *numfound) {
    unsigned int curbyte = 0, curpos = 0, bitstreamlen, zeroes = 0, found = 0;
    unsigned int numleft = k - *numfound;

    bitstreamlen = bytestreamlen * 8;

    if (numfound) {
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
        if (isset_bit(bytestream, curpos))
            zeroes = 0;
        else
            zeroes++;

        if (zeroes == k)
            found = 1;

        curpos++;
    }

    *numfound = zeroes;

    if (found)
        return curpos;
    else
        return 0;
}

double chi_sq(int n, float observed[], float expected[]) {
    double cs = 0;
    int i;
    for (i = 0; i < n; i++) {
        cs += (((double) observed[i] - (double) expected[i]) * ((double) observed[i] - (double) expected[i]))
              / (double) expected[i];
    }
    return cs;
}

static double igf(double S, double Z) {
    if (Z < 0.0) {
        return 0.0;
    }
    double Sc = (1.0 / S);
    Sc *= pow(Z, S);
    Sc *= exp(-Z);

    double Sum = 1.0;
    double Nom = 1.0;
    double Denom = 1.0;
    int I;

    for (I = 0; I < 200; I++) {
        Nom *= Z;
        S++;
        Denom *= S;
        Sum += (Nom / Denom);
    }

    return Sum * Sc;
}

double chisqr(int Dof, double Cv) {
    if (Cv < 0 || Dof < 1) {
        return 0.0;
    }
    double K = ((double) Dof) * 0.5;
    double X = Cv * 0.5;
    if (Dof == 2) {
        return exp(-1.0 * X);
    }

    double PValue = igf(K, X);
    if (isnan(PValue) || isinf(PValue) || PValue <= 1e-8) {
        return 1e-14;
    }

    PValue /= tgamma(K);

    return (1.0 - PValue);
}

double rho_test(uint8_t *key, int r, int l) {
    float e_prob[5], e_freq[5], o_freq[5] = {0};
    int bin[5];
    int i, j, k, mask, temp;

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

    uint8_t iv[IVLEN];
    uint8_t ivpos[IVLEN * 8] = {0};
    int *lpos = calloc(l, sizeof(int));

    int kspos;

    for (i = 0; i < l; i++) {
        do {
            temp = rand() % (IVLEN * 8);
        } while (ivpos[temp]);
        ivpos[temp] = 1;
        lpos[i] = temp;
        //printf("%d ", temp);
    }
    //printf("\n");

    for (i = 0; i < r; i++) {
        //printf("RUN #%d\n", i);
        int *cov = calloc(two_power(l), sizeof(int));
        unsigned int rho = 0, repeated = 0;

        random_bytes(iv, IVLEN);

        do {
            uint32_t keystreamlen = 3;
            uint8_t *keystream = calloc(keystreamlen, sizeof(uint8_t));
            s20_crypt(key, S20_KEYLEN_128, iv, 0, keystream, keystreamlen);

            for (k = 0; k < l; k++) {
                if (isset_bit(keystream, k)) set_bit(iv, lpos[k]);
                else unset_bit(iv, lpos[k]);
            }

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

            free(keystream);
        } while (!repeated);

        j = 0;
        while (rho > bin[j])
            j++;
        o_freq[j]++;

        free(cov);
    }

//    for(i=0; i<5; i++)
//        printf("%d\t%f\t%f\n", i, e_freq[i], (float)o_freq[i]);

    free(lpos);

    return chisqr(4, chi_sq(5, o_freq, e_freq));
}

double coverage_test(uint8_t *key, int r, int l) {
    float e_prob[5], e_freq[5], o_freq[5] = {0};
    int bin[5];
    int i, j, k, mask, temp;

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

    uint8_t iv[IVLEN];
    uint8_t ivpos[IVLEN * 8] = {0};
    int *lpos = calloc(l, sizeof(int));

    int kspos;

    for (i = 0; i < l; i++) {
        do {
            temp = rand() % (IVLEN * 8);
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

        random_bytes(iv, IVLEN);

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
            uint8_t *keystream = calloc(keystreamlen, sizeof(uint8_t));
            s20_crypt(key, S20_KEYLEN_128, iv, 0, keystream, keystreamlen);

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

        free(cov);
    }

//    for(i=0; i<5; i++)
//        printf("%d\t%f\t%f\n", i, e_freq[i], (float)o_freq[i]);

    free(lpos);

    return chisqr(4, chi_sq(5, o_freq, e_freq));
}

double dpcoverage_test(uint8_t *key, int r, int l, int k) {
    float e_prob[5], e_freq[5], o_freq[5] = {0};
    int bin[5];
    int i, j, m, mask, temp;

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

    uint8_t iv[IVLEN];
    uint8_t ivpos[IVLEN * 8] = {0};
    int *lpos = calloc(l, sizeof(int));

    int kspos;

    for (i = 0; i < l; i++) {
        do {
            temp = rand() % (IVLEN * 8);
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

        random_bytes(iv, IVLEN);

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
            unsigned int startlpos, numfound = 0;

            do {
                if (keystream) free(keystream);
                keystream = calloc(keystreamlen, sizeof(uint8_t));
                s20_crypt(key, S20_KEYLEN_128, iv, prevkeystreamlen, keystream, keystreamlen);
                startlpos = find_kzeroes(k, keystream, keystreamlen, &numfound);
                prevkeystreamlen += keystreamlen;
            } while (!startlpos);

            // Belum menangani kasus sisa bit di keystream kurang dari l

            unsigned int p, q;
            p = startlpos / 8;
            q = startlpos % 8;

            if ((l + q) <= 16)
                kspos = (keystream[p] << (l - 8 + q)) + (keystream[p + 1] >> (16 - l - q));
            else
                kspos = (keystream[p] << (l - 8 + q)) + (keystream[p + 1] << (l + q - 16)) +
                        (keystream[p + 2] >> (24 - l - q));


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

        free(cov);
    }

//    for (i = 0; i < 5; i++)
//        printf("%d\t%f\t%f\n", i, e_freq[i], (float) o_freq[i]);

    free(lpos);

    return chisqr(4, chi_sq(5, o_freq, e_freq));
}
