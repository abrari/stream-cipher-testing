#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "tests_algebraic.h"
#include "tests_randommapping.h"
#include "tests_correlation.h"

#include "stats.h"
#include <math.h>

#define KEYLEN      16
#define IVLEN       8

#define two_power(n) (1u << (n))

int main() {

    srand(time(NULL));
    int num_p_vals = 20, i, j;
    uint8_t key[KEYLEN] = {0xA0, 0xF1, 0x02, 0x0A, 0x0C, 0x0F};
    uint8_t iv[IVLEN] = {0};

    printf("%f", frame_correlation_test(256, two_power(20)));

    return 0;
}
