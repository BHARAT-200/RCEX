#include "rcex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRIALS 1000000
#define POSITIONS 256

#define KEY_SIZE 32
#define NONCE_SIZE 16


static void random_bytes(int8 *buf, int size)
{
    FILE *f = fopen("/dev/urandom", "rb");

    if(!f)
    {
        perror("urandom");
        exit(1);
    }

    fread(buf,1,size,f);
    fclose(f);
}


int main()
{
    unsigned long long counts[POSITIONS][256] = {0};

    int8 key[KEY_SIZE];
    int8 nonce[NONCE_SIZE];


    printf("\nRCEX EARLY KEYSTREAM BIAS TEST\n");
    printf("==============================\n");

    printf("Trials: %d\n", TRIALS);
    printf("Positions tested: %d\n\n", POSITIONS);


    for(int t=0;t<TRIALS;t++)
    {

        random_bytes(key,KEY_SIZE);
        random_bytes(nonce,NONCE_SIZE);


        Rcex *ctx =
            rcexinit_nonce(
                key,
                KEY_SIZE,
                nonce,
                NONCE_SIZE
            );


        for(int p=0;p<POSITIONS;p++)
        {
            unsigned char b =
                (unsigned char)rcexbyte(ctx);

            counts[p][b]++;
        }


        rcexwipe(ctx);


        if((t+1)%100000==0)
            printf("%d / %d complete\n",
                t+1,TRIALS);
    }


    printf("\nRESULTS\n");
    printf("==============================\n");


    for(int p=0;p<POSITIONS;p++)
    {
        double expected =
            TRIALS / 256.0;


        double chi=0;


        for(int b=0;b<256;b++)
        {
            double diff =
                counts[p][b]-expected;

            chi +=
                (diff*diff)/expected;
        }


        printf(
            "Position %3d : Chi-square %.2f | byte0 %.4f%%\n",
            p,
            chi,
            (counts[p][0]*100.0)/TRIALS
        );
    }


    return 0;
}