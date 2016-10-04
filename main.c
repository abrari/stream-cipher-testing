#include <stdio.h>
#include <stdlib.h>
#include "salsa20/salsa20.h"

void print_bytes(unsigned char *bytes, unsigned int length) {
    unsigned int i;
    for (i = 0; i < length; ++i) {
        printf("%02x", bytes[i]);
    }
}

void byte_to_bit_array(/*IN*/ unsigned char *bytes, unsigned int bytelength, /*OUT*/ unsigned char* bits) {

    unsigned int i, j;
    unsigned char b;
    for (i = 0; i < bytelength; ++i) {
        b = bytes[i];
        for (j = 0; j < 8; ++j) {
            bits[i * 8 + j] = (b & (1 << (7 - j))) != 0;
        }
    }

}

int main() {

    uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    uint8_t iv[8]   = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

    // encrypt null bytes to get the raw keystream
    uint32_t keystreamlen = 10000;
    uint8_t  *keystream   = calloc(keystreamlen, sizeof(uint8_t));

    s20_crypt(key, S20_KEYLEN_128, iv, 0, keystream, keystreamlen);

    print_bytes(keystream, keystreamlen);

    free(keystream);

    return 0;
}