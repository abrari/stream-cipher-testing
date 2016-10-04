#ifndef STREAM_CIPHER_BYTES_H
#define STREAM_CIPHER_BYTES_H

typedef unsigned char byte;

void print_bytes(byte *bytes, unsigned int length);
void print_bits(byte *bits, unsigned int length);
void byte_to_bit_array(/*IN*/ byte *bytes, unsigned int bytelength, /*OUT*/ byte *bits);
void bit_to_byte_array(/*IN*/ byte *bits, unsigned int bitslength, /*OUT*/ byte *bytes);
byte * generate_random_bytes(unsigned int length);
byte * generate_random_bits(unsigned int bitlength);
void xor_bytes(/*IN*/ byte *b1, /*IN*/ byte *b2, /*OUT*/ byte *result, unsigned int length);

#endif //STREAM_CIPHER_BYTES_H
