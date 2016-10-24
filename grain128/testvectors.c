/*
 * REFERENCE IMPLEMENTATION OF STREAM CIPHER GRAIN-128
 *
 * Filename: testvectors.c
 *
 * Author:
 * Martin Hell
 * Dept. of Information Technology
 * P.O. Box 118
 * SE-221 00 Lund, Sweden,
 * email: martin@it.lth.se
 *
 * Synopsis:
 *    Generates testvectors from the reference implementation of Grain-128.
 *
 */


#include <stdio.h>
#include <string.h>
#include "ecrypt-sync.h"

/*  GENERATE TEST VECTORS  */

void printData(u8 *key, u8 *IV, u8 *ks) {
	u32 i;
	printf("\n\nkey:        ");
	for (i=0;i<16;++i) printf("%02x",key[i]);
	printf("\nIV :        ");
	for (i=0;i<12;++i) printf("%02x",IV[i]);
	printf("\nkeystream:  ");
	for (i=0;i<16;++i) printf("%02x",ks[i]);
}

void testvectors() {
	
	ECRYPT_ctx ctx;
	u8 key1[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		IV1[12] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	    ks[16];

	u8 key2[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0},
		IV2[12] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x12,0x34,0x56,0x78};
	
	ECRYPT_keysetup(&ctx,key1,128,96);
	ECRYPT_ivsetup(&ctx,IV1);
	ECRYPT_keystream_bytes(&ctx,ks,16);
	printData(key1,IV1,ks);

	ECRYPT_keysetup(&ctx,key2,128,96);
	ECRYPT_ivsetup(&ctx,IV2);
	ECRYPT_keystream_bytes(&ctx,ks,16);
	printData(key2,IV2,ks);

	printf("\n");

}

int main(int argc, char **argv) {	
	printf("---REFERENCE IMPLEMENTATION OF GRAIN-128---\n");
	testvectors();
	return 0;
}


