#ifndef STREAM_CIPHER_H
#define STREAM_CIPHER_H

#include <stdint.h>
#include <stdlib.h>

#define SC_SALSA20     0
#define SC_HC128       1
#define SC_GRAIN128    2
#define SC_MICKEY      3

#define STREAM_CIPHER SC_SALSA20 // 0 = Salsa20, 1 = HC-128, 2 = Grain 128, 3 = Mickey v2-128

#if STREAM_CIPHER == SC_SALSA20
#define KEYLEN      16
    #define IVLEN       8
    #define ALGONAME    "Salsa20"
    #include "salsa20/salsa20.h"
#elif STREAM_CIPHER == SC_HC128
#define KEYLEN      16
    #define IVLEN       16
    #define ALGONAME    "HC-128"
    #include "hc128/hc128_opt32.h"
#elif STREAM_CIPHER == SC_GRAIN128
#define KEYLEN      16
    #define IVLEN       12
    #define ALGONAME    "Grain-128"
    #include "grain128/ecrypt-sync.h"
#elif STREAM_CIPHER == SC_MICKEY
    #define KEYLEN      10
    #define IVLEN       10
    #define ALGONAME    "Mickey"
    #include "mickey/ecrypt-sync.h"
#endif // STREAM_CIPHER

#define KEYLEN_BIT  (KEYLEN * 8)
#define IVLEN_BIT   (IVLEN * 8)

uint8_t *generate_keystream(uint8_t *key, uint8_t *iv, uint32_t idx, uint32_t kslen);

#endif // STREAM_CIPHER_H
