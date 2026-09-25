#include "rcex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TRIALS          100000
#define BYTES_PER_TRIAL 4096

#define KEY_SIZE       32
#define NONCE_SIZE     16

#define TOTAL_BYTES \
    ((unsigned long long)TRIALS * BYTES_PER_TRIAL)

static void random_bytes(int8 *buf, int size)
{
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

int main(void)
{
    unsigned long long byte_count[256] = {0};
    unsigned long long bit_count[8] = {0};

    int8 key[KEY_SIZE];
    int8 nonce[NONCE_SIZE];

    printf("\n");
    printf("============================================\n");
    printf("          RCEX KEYSTREAM BIAS TEST\n");
    printf("============================================\n");

    printf("Trials:              %d\n", TRIALS);
    printf("Bytes per trial:     %d\n", BYTES_PER_TRIAL);
    printf("Total bytes:         %llu\n", TOTAL_BYTES);
    printf("Key size:            %d bytes\n", KEY_SIZE);
    printf("Nonce size:          %d bytes\n", NONCE_SIZE);
    printf("--------------------------------------------\n");

    /*
     * Generate many independent RCEX streams.
     *
     * rcexinit_nonce() performs the key+nonce initialization
     * and rcexinit() already discards RCEX's first 5000 bytes.
     */
    for (int t = 0; t < TRIALS; t++) {

        random_bytes(key, KEY_SIZE);
        random_bytes(nonce, NONCE_SIZE);

        Rcex *ctx = rcexinit_nonce(
            key,
            KEY_SIZE,
            nonce,
            NONCE_SIZE
        );

        if (!ctx) {
            fprintf(stderr, "RCEX initialization failed\n");
            return EXIT_FAILURE;
        }

        for (int n = 0; n < BYTES_PER_TRIAL; n++) {

            unsigned char b =
                (unsigned char)rcexbyte(ctx);

            /* Byte frequency */
            byte_count[b]++;

            /* Individual bit frequencies */
            for (int bit = 0; bit < 8; bit++) {
                if (b & (1 << bit))
                    bit_count[bit]++;
            }
        }

        rcexwipe(ctx);

        if ((t + 1) % 1000 == 0) {
            printf("Completed %d / %d trials\n",
                   t + 1, TRIALS);
        }
    }

    /*
     * --------------------------------------------------------
     * BYTE FREQUENCY ANALYSIS
     * --------------------------------------------------------
     */

    double expected =
        (double)TOTAL_BYTES / 256.0;

    double chi_square = 0.0;

    unsigned long long min_count = byte_count[0];
    unsigned long long max_count = byte_count[0];

    int min_byte = 0;
    int max_byte = 0;

    for (int i = 0; i < 256; i++) {

        double diff =
            (double)byte_count[i] - expected;

        chi_square +=
            (diff * diff) / expected;

        if (byte_count[i] < min_count) {
            min_count = byte_count[i];
            min_byte = i;
        }

        if (byte_count[i] > max_count) {
            max_count = byte_count[i];
            max_byte = i;
        }
    }

    printf("\n");
    printf("============================================\n");
    printf("           BYTE FREQUENCY RESULTS\n");
    printf("============================================\n");

    printf("Expected count/byte: %.2f\n", expected);

    printf(
        "Least frequent:     0x%02X -> %llu (%.6f%%)\n",
        min_byte,
        min_count,
        ((double)min_count / TOTAL_BYTES) * 100.0
    );

    printf(
        "Most frequent:      0x%02X -> %llu (%.6f%%)\n",
        max_byte,
        max_count,
        ((double)max_count / TOTAL_BYTES) * 100.0
    );

    printf("Expected frequency:  0.390625%%\n");

    printf("--------------------------------------------\n");
    printf("Chi-square:          %.4f\n", chi_square);
    printf("Degrees freedom:     255\n");

    /*
     * Approximate critical values for df=255.
     *
     * These are useful reference points:
     *
     * ~293 -> p ≈ 0.05
     * ~310 -> p ≈ 0.01
     *
     * We don't claim this alone proves randomness.
     */
    if (chi_square < 293.25)
        printf("Chi-square result:   Within ~95%% range\n");
    else if (chi_square < 310.46)
        printf("Chi-square result:   Above ~95%% threshold\n");
    else
        printf("Chi-square result:   HIGH / suspicious\n");

    /*
     * --------------------------------------------------------
     * BIT FREQUENCY ANALYSIS
     * --------------------------------------------------------
     */

    printf("\n");
    printf("============================================\n");
    printf("             BIT FREQUENCY RESULTS\n");
    printf("============================================\n");

    unsigned long long total_bits =
        TOTAL_BYTES * 8ULL;

    for (int bit = 0; bit < 8; bit++) {

        double ones =
            ((double)bit_count[bit] /
             (double)TOTAL_BYTES) * 100.0;

        double zeros = 100.0 - ones;

        printf(
            "Bit %d:  ones = %.6f%%   zeros = %.6f%%\n",
            bit,
            ones,
            zeros
        );
    }

    printf("--------------------------------------------\n");
    printf("Expected: ones ≈ 50%%, zeros ≈ 50%%\n");

    /*
     * --------------------------------------------------------
     * SUMMARY
     * --------------------------------------------------------
     */

    printf("\n");
    printf("============================================\n");
    printf("                 SUMMARY\n");
    printf("============================================\n");

    printf("Total bytes tested: %llu\n", TOTAL_BYTES);
    printf("Total bits tested:  %llu\n", total_bits);
    printf("Chi-square:         %.4f\n", chi_square);

    printf("\nBias test complete.\n");
    printf("============================================\n");

    return 0;
}