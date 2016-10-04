#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "tests_algebraic.h"

int main() {

    srand(time(NULL));

    uint8_t key[KEYLEN] = {0};
    int m = 5, N = 100;

    linear_span_test(key, m, N);

    return 0;
}
