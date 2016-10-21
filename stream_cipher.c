#include "stream_cipher.h"

uint8_t *generate_keystream(uint8_t *key, uint8_t *iv, uint32_t idx, uint32_t kslen) {
    uint8_t *ks = (uint8_t *) calloc(kslen, sizeof(uint8_t));

#if STREAM_CIPHER == SC_SALSA20
    s20_crypt(key, S20_KEYLEN_128, iv, idx, ks, kslen);

#elif STREAM_CIPHER == SC_HC128
    if (idx) {
            uint8_t *ksfull = (uint8_t*)calloc(kslen+idx, sizeof(uint8_t));

            HC128(key, iv, ksfull, ksfull, kslen+idx);
            memcpy(ks, ksfull+idx, kslen);
            free(ksfull);
        }
        else HC128(key, iv, ks, ks, kslen);

#elif STREAM_CIPHER == SC_GRAIN128
    ECRYPT_ctx ctx;
        ECRYPT_init();
        ECRYPT_keysetup(&ctx,key,KEYLEN_BIT,IVLEN_BIT);
        ECRYPT_ivsetup(&ctx,iv);

        if (idx) {
            uint8_t *ksfull = (uint8_t*)calloc(kslen+idx, sizeof(uint8_t));

            ECRYPT_keystream_bytes(&ctx,ksfull,kslen+idx);
            memcpy(ks, ksfull+idx, kslen);
            free(ksfull);
        }
        else ECRYPT_keystream_bytes(&ctx,ks,kslen);

#elif STREAM_CIPHER == SC_MICKEY
    ECRYPT_ctx ctx;
    ECRYPT_init();
    ECRYPT_keysetup(&ctx, key, KEYLEN_BIT, IVLEN_BIT);
    ECRYPT_ivsetup(&ctx, iv);

    if (idx) {
        uint8_t *ksfull = (uint8_t *) calloc(kslen + idx, sizeof(uint8_t));

        ECRYPT_keystream_bytes(&ctx, ksfull, kslen + idx);
        memcpy(ks, ksfull + idx, kslen);
        free(ksfull);
    }
    else {
        ECRYPT_keystream_bytes(&ctx, ks, kslen);
    }

#else
    return NULL;
#endif // STREAM_CIPHER

    return ks;
}
