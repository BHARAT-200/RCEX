#include "rcex.h"

int main(int argc, char **argv) {

    if (argc != 4) {
        printf("Usage: %s <input> <output> <key>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "rb");
    FILE *out = fopen(argv[2], "wb");

    if (!in || !out) {
        printf("Error: could not open files.\n");
        return 1;
    }

    unsigned char *key = (unsigned char *)argv[3];
    Rcex *ctx = rcexinit(key, strlen((char *)key));

    unsigned char buffer[4096];
    int read;

    while ((read = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        unsigned char *encrypted = rcexencrypt(ctx, buffer, read);
        fwrite(encrypted, 1, read, out);
        free(encrypted);
    }

    rcexuninit(ctx);
    fclose(in);
    fclose(out);

    printf("Done \n");
    return 0;
}