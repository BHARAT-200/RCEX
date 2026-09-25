#include "rcex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TRIALS 10000

#define KEY_SIZE 32
#define NONCE_SIZE 16

#define OBSERVE 4096
#define PREDICT 1024



void random_bytes(int8 *buf, int size)
{
    FILE *f = fopen("/dev/urandom","rb");

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

    unsigned long long correct = 0;
    unsigned long long total = 0;


    int8 key[KEY_SIZE];
    int8 nonce[NONCE_SIZE];


    int8 plaintext[OBSERVE + PREDICT];


    printf("\n");
    printf("=====================================\n");
    printf("     RCEX PREDICTION TEST\n");
    printf("=====================================\n");

    printf("Trials: %d\n",TRIALS);
    printf("Observe: %d bytes\n",OBSERVE);
    printf("Predict: %d bytes\n",PREDICT);



    for(int t=0;t<TRIALS;t++)
    {

        random_bytes(
            key,
            KEY_SIZE
        );


        random_bytes(
            nonce,
            NONCE_SIZE
        );


        memset(
            plaintext,
            0,
            OBSERVE+PREDICT
        );


        Rcex *ctx =
            rcexinit_nonce(
                key,
                KEY_SIZE,
                nonce,
                NONCE_SIZE
            );


        int8 *stream =
            rcexencrypt(
                ctx,
                plaintext,
                OBSERVE+PREDICT
            );


        /*
            Simple attacker:
            predicts next byte as previous byte
        */


        for(int i=OBSERVE;i<OBSERVE+PREDICT;i++)
        {

            unsigned char prediction =
                stream[i-1];


            if(prediction == stream[i])
                correct++;


            total++;

        }


        free(stream);

        rcexwipe(ctx);


        if((t+1)%1000==0)
        {
            printf(
                "%d / %d complete\n",
                t+1,
                TRIALS
            );
        }

    }



    printf("\n");
    printf("=====================================\n");
    printf("RESULTS\n");
    printf("=====================================\n");


    printf(
        "Correct predictions: %llu\n",
        correct
    );


    printf(
        "Total predictions: %llu\n",
        total
    );


    printf(
        "Accuracy: %.6f%%\n",
        ((double)correct /
        total)*100.0
    );


    printf("-------------------------------------\n");

    printf(
        "Random guessing expected: %.6f%%\n",
        100.0/256.0
    );


    return 0;
}