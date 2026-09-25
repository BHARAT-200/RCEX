#include "rcex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define TESTS       10000
#define DATA_SIZE   1024
#define KEY_SIZE    32
#define NONCE_SIZE  16

static unsigned long long count_changed_bits(
    const int8 *a,
    const int8 *b,
    int size
) {
    unsigned long long count = 0;

    for (int i = 0; i < size; i++) {
        unsigned char x = a[i] ^ b[i];

        while (x) {
            count += x & 1;
            x >>= 1;
        }
    }

    return count;
}

static void random_bytes(int8 *buf, int size) {
    FILE *f = fopen("/dev/urandom", "rb");

    if (!f) {
        perror("/dev/urandom");
        exit(EXIT_FAILURE);
    }

    if (fread(buf, 1, size, f) != (size_t)size) {
        fprintf(stderr, "Failed to read random bytes\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    fclose(f);
}

static double run_key_avalanche(void) {
    unsigned long long total_changed = 0;
    unsigned long long total_bits =
        (unsigned long long)TESTS * DATA_SIZE * 8;

    double min = 100.0;
    double max = 0.0;

    int8 key1[KEY_SIZE];
    int8 key2[KEY_SIZE];
    int8 nonce[NONCE_SIZE];
    int8 plaintext[DATA_SIZE];

    for (int t = 0; t < TESTS; t++) {

        random_bytes(key1, KEY_SIZE);
        random_bytes(nonce, NONCE_SIZE);
        random_bytes(plaintext, DATA_SIZE);

        memcpy(key2, key1, KEY_SIZE);

        /* Flip exactly one random key bit */
        int byte = rand() % KEY_SIZE;
        int bit  = rand() % 8;

        key2[byte] ^= (1 << bit);

        Rcex *a = rcexinit_nonce(
            key1, KEY_SIZE,
            nonce, NONCE_SIZE
        );

        Rcex *b = rcexinit_nonce(
            key2, KEY_SIZE,
            nonce, NONCE_SIZE
        );

        int8 *ca = rcexencrypt(a, plaintext, DATA_SIZE);
        int8 *cb = rcexencrypt(b, plaintext, DATA_SIZE);

        unsigned long long changed =
            count_changed_bits(ca, cb, DATA_SIZE);

        double percentage =
            ((double)changed / (DATA_SIZE * 8)) * 100.0;

        if (percentage < min)
            min = percentage;

        if (percentage > max)
            max = percentage;

        total_changed += changed;

        free(ca);
        free(cb);

        rcexwipe(a);
        rcexwipe(b);
    }

    double average =
        ((double)total_changed / total_bits) * 100.0;

    printf("\n");
    printf("========================================\n");
    printf("       RCEX KEY AVALANCHE TEST\n");
    printf("========================================\n");
    printf("Trials:             %d\n", TESTS);
    printf("Plaintext size:     %d bytes\n", DATA_SIZE);
    printf("Key size:            %d bytes\n", KEY_SIZE);
    printf("Nonce size:          %d bytes\n", NONCE_SIZE);
    printf("----------------------------------------\n");
    printf("Average changed:    %.4f%%\n", average);
    printf("Minimum:            %.4f%%\n", min);
    printf("Maximum:            %.4f%%\n", max);
    printf("Expected:           ~50.0%%\n");
    printf("========================================\n");

    return average;
}

static double run_nonce_avalanche(void) {
    unsigned long long total_changed = 0;
    unsigned long long total_bits =
        (unsigned long long)TESTS * DATA_SIZE * 8;

    double min = 100.0;
    double max = 0.0;

    int8 key[KEY_SIZE];
    int8 nonce1[NONCE_SIZE];
    int8 nonce2[NONCE_SIZE];
    int8 plaintext[DATA_SIZE];

    for (int t = 0; t < TESTS; t++) {

        random_bytes(key, KEY_SIZE);
        random_bytes(nonce1, NONCE_SIZE);
        random_bytes(plaintext, DATA_SIZE);

        memcpy(nonce2, nonce1, NONCE_SIZE);

        /* Flip exactly one random nonce bit */
        int byte = rand() % NONCE_SIZE;
        int bit  = rand() % 8;

        nonce2[byte] ^= (1 << bit);

        Rcex *a = rcexinit_nonce(
            key, KEY_SIZE,
            nonce1, NONCE_SIZE
        );

        Rcex *b = rcexinit_nonce(
            key, KEY_SIZE,
            nonce2, NONCE_SIZE
        );

        int8 *ca = rcexencrypt(a, plaintext, DATA_SIZE);
        int8 *cb = rcexencrypt(b, plaintext, DATA_SIZE);

        unsigned long long changed =
            count_changed_bits(ca, cb, DATA_SIZE);

        double percentage =
            ((double)changed / (DATA_SIZE * 8)) * 100.0;

        if (percentage < min)
            min = percentage;

        if (percentage > max)
            max = percentage;

        total_changed += changed;

        free(ca);
        free(cb);

        rcexwipe(a);
        rcexwipe(b);
    }

    double average =
        ((double)total_changed / total_bits) * 100.0;

    printf("\n");
    printf("========================================\n");
    printf("      RCEX NONCE AVALANCHE TEST\n");
    printf("========================================\n");
    printf("Trials:             %d\n", TESTS);
    printf("Plaintext size:     %d bytes\n", DATA_SIZE);
    printf("Key size:            %d bytes\n", KEY_SIZE);
    printf("Nonce size:          %d bytes\n", NONCE_SIZE);
    printf("----------------------------------------\n");
    printf("Average changed:    %.4f%%\n", average);
    printf("Minimum:            %.4f%%\n", min);
    printf("Maximum:            %.4f%%\n", max);
    printf("Expected:           ~50.0%%\n");
    printf("========================================\n");

    return average;
}

int main(void) {

    srand((unsigned int)time(NULL));

    printf("\nRCEX Avalanche Evaluation\n");

    run_key_avalanche();
    run_nonce_avalanche();

    printf("\nTesting complete.\n");

    return 0;
}