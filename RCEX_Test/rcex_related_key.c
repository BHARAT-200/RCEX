#include "rcex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TRIALS 100000

#define KEY_SIZE 32
#define NONCE_SIZE 16
#define PLAINTEXT_SIZE 1024


void random_bytes(int8 *buf, int size)
{
    FILE *f = fopen("/dev/urandom", "rb");

    if(!f)
    {
        perror("urandom");
        exit(1);
    }

    fread(buf, 1, size, f);
    fclose(f);
}


int count_bits(unsigned char x)
{
    int c = 0;

    while(x)
    {
        c += x & 1;
        x >>= 1;
    }

    return c;
}


int main()
{
    unsigned long long total_changed = 0;

    int min = PLAINTEXT_SIZE * 8;
    int max = 0;


    int8 key1[KEY_SIZE];
    int8 key2[KEY_SIZE];

    int8 nonce[NONCE_SIZE];

    int8 plaintext[PLAINTEXT_SIZE];


    printf("\n");
    printf("=====================================\n");
    printf("       RCEX RELATED KEY TEST\n");
    printf("=====================================\n");

    printf("Trials: %d\n", TRIALS);
    printf("Key size: %d bytes\n", KEY_SIZE);
    printf("Nonce size: %d bytes\n", NONCE_SIZE);
    printf("Plaintext: %d bytes\n", PLAINTEXT_SIZE);
    printf("-------------------------------------\n");


    for(int t = 0; t < TRIALS; t++)
    {

        random_bytes(key1, KEY_SIZE);
        random_bytes(nonce, NONCE_SIZE);
        random_bytes(plaintext, PLAINTEXT_SIZE);


        // create key2 = key1 with exactly 1 flipped bit

        memcpy(key2, key1, KEY_SIZE);

        int byte = rand() % KEY_SIZE;
        int bit  = rand() % 8;

        key2[byte] ^= (1 << bit);



        Rcex *ctx1 =
            rcexinit_nonce(
                key1,
                KEY_SIZE,
                nonce,
                NONCE_SIZE
            );


        Rcex *ctx2 =
            rcexinit_nonce(
                key2,
                KEY_SIZE,
                nonce,
                NONCE_SIZE
            );


        int8 *cipher1 =
            rcexencrypt(
                ctx1,
                plaintext,
                PLAINTEXT_SIZE
            );


        int8 *cipher2 =
            rcexencrypt(
                ctx2,
                plaintext,
                PLAINTEXT_SIZE
            );



        int changed = 0;


        for(int i = 0; i < PLAINTEXT_SIZE; i++)
        {
            changed +=
                count_bits(
                    cipher1[i] ^ cipher2[i]
                );
        }


        total_changed += changed;


        if(changed < min)
            min = changed;


        if(changed > max)
            max = changed;



        free(cipher1);
        free(cipher2);


        rcexwipe(ctx1);
        rcexwipe(ctx2);



        if((t + 1) % 10000 == 0)
        {
            printf("%d / %d complete\n",
                t + 1,
                TRIALS);
        }

    }



    unsigned long long total_bits =
        (unsigned long long)TRIALS *
        PLAINTEXT_SIZE *
        8;



    printf("\n");
    printf("=====================================\n");
    printf("RESULTS\n");
    printf("=====================================\n");


    printf(
        "Average changed bits: %.4f%%\n",
        ((double)total_changed /
        total_bits) * 100.0
    );


    printf(
        "Minimum: %.4f%%\n",
        ((double)min /
        (PLAINTEXT_SIZE * 8)) * 100.0
    );


    printf(
        "Maximum: %.4f%%\n",
        ((double)max /
        (PLAINTEXT_SIZE * 8)) * 100.0
    );


    printf("-------------------------------------\n");
    printf("Expected: ~50%%\n");


    return 0;
}