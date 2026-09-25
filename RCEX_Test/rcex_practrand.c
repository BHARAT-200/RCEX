/*
 * Emit the raw RCEX keystream as binary data for PractRand.
 *
 * Build beside rcex.c and rcex.h:
 *   gcc -std=c11 -O2 -Wall -Wextra -I. rcex_practrand.c rcex.c -o rcex_practrand
 *
 * Usage:
 *   ./rcex_practrand "test key" ["test nonce"] | /path/to/RNG_test stdin8
 *
 * Arguments are C strings, so this adapter is intended for ordinary text test
 * values. It emits no text on stdout; stdout is exclusively raw bytes.
 */
#include "rcex.h"

#include <limits.h>

static int valid_length(const char *value, const char *name, size_t *length) {
    *length = strlen(value);
    if (*length == 0) {
        fprintf(stderr, "%s must not be empty\n", name);
        return 0;
    }
    if (*length > USHRT_MAX) {
        fprintf(stderr, "%s is too long (maximum is %u bytes)\n", name,
                (unsigned)USHRT_MAX);
        return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    const char *key;
    const char *nonce = NULL;
    size_t key_length;
    size_t nonce_length = 0;
    Rcex *ctx;
    int8 block[16384];

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <key> [nonce]\n", argv[0]);
        return 2;
    }

    key = argv[1];
    if (!valid_length(key, "key", &key_length)) {
        return 2;
    }

    if (argc == 3) {
        nonce = argv[2];
        if (!valid_length(nonce, "nonce", &nonce_length)) {
            return 2;
        }
        /* rcexinit_nonce() uses an unsigned-short combined length. */
        if (key_length + nonce_length > USHRT_MAX) {
            fprintf(stderr, "key plus nonce must be at most %u bytes\n",
                    (unsigned)USHRT_MAX);
            return 2;
        }
        ctx = rcexinit_nonce((int8 *)key, (int16)key_length,
                             (int8 *)nonce, (int16)nonce_length);
    } else {
        ctx = rcexinit((int8 *)key, (int16)key_length);
    }

    /* rcexinit*() has already dropped RCEX's first 5,000 stream bytes. */
    for (;;) {
        size_t i;
        for (i = 0; i < sizeof block; ++i) {
            block[i] = rcexbyte(ctx);
        }
        if (fwrite(block, 1, sizeof block, stdout) != sizeof block) {
            /* A closed PractRand pipe normally arrives here or as SIGPIPE. */
            rcexwipe(ctx);
            return ferror(stdout) ? 1 : 0;
        }
    }
}
