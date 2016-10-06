#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bytes.h"

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

unsigned int isset_bit(byte *b, int pos) {
    return (b[pos/8] & (128 >> (pos%8)));
}

void set_bit(byte *b, int pos) {
    b[pos/8] |= (128 >> (pos%8));
}

void unset_bit(byte *b, int pos) {
    b[pos/8] &= 255 - two_power(7 - (pos%8));
}

void flip_bit(byte *b, int pos) {
    b[pos/8] ^= (128 >> (pos%8));
}

unsigned int hamming_weight(byte *b, unsigned int length) {
    unsigned int w = 0;
    unsigned int i, x;
    for (i = 0; i < length; ++i) {
        // http://stackoverflow.com/a/14010273/1198623
        x = b[i];
        x = (x & 0x55) + (x >> 1 & 0x55);
        x = (x & 0x33) + (x >> 2 & 0x33);
        x = (x & 0x0f) + (x >> 4 & 0x0f);
        w += x;
    }
    return w;
}