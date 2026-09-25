#include "rcex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define KEY_SIZE 32
#define NONCE_SIZE 16

#define OUTPUT_SIZE 60000


void print_result(
    const char *name,
    int8 *data,
    int size
)
{

    unsigned long counts[256] = {0};

    unsigned long long ones[8] = {0};


    for(int i = 0; i < size; i++)
    {
        unsigned char b = data[i];

        counts[b]++;

        for(int j = 0; j < 8; j++)
        {
            if(b & (1 << j))
                ones[j]++;
        }
    }



    double expected =
        (double)size / 256.0;


    double chi = 0;


    for(int i = 0; i < 256; i++)
    {
        double diff =
            counts[i] - expected;

        chi +=
            (diff * diff) / expected;
    }



    printf("\n");
    printf("=====================================\n");
    printf("%s\n", name);
    printf("=====================================\n");


    printf("Bytes tested: %d\n", size);


    printf("\nByte frequency:\n");

    printf("Expected: %.2f\n", expected);


    int min = 0;
    int max = 0;


    for(int i=1;i<256;i++)
    {
        if(counts[i] < counts[min])
            min=i;

        if(counts[i] > counts[max])
            max=i;
    }


    printf(
        "Lowest: 0x%02X -> %lu\n",
        min,
        counts[min]
    );


    printf(
        "Highest: 0x%02X -> %lu\n",
        max,
        counts[max]
    );


    printf(
        "Chi-square: %.2f\n",
        chi
    );



    printf("\nBit frequency:\n");


    for(int i=0;i<8;i++)
    {

        double percent =
            ((double)ones[i] /
            (size*8.0))*100.0;


        printf(
            "Bit %d : %.4f%% ones\n",
            i,
            percent
        );
    }


}



void run_test(
    const char *name,
    int8 *key,
    int8 *nonce
)
{

    int8 plaintext[OUTPUT_SIZE];

    memset(
        plaintext,
        0,
        OUTPUT_SIZE
    );


    Rcex *ctx =
        rcexinit_nonce(
            key,
            KEY_SIZE,
            nonce,
            NONCE_SIZE
        );


    int8 *out =
        rcexencrypt(
            ctx,
            plaintext,
            OUTPUT_SIZE
        );


    print_result(
        name,
        out,
        OUTPUT_SIZE
    );


    free(out);

    rcexwipe(ctx);

}



void fill_pattern(
    int8 *buf,
    int size,
    unsigned char value
)
{
    memset(
        buf,
        value,
        size
    );
}



int main()
{

    int8 key[KEY_SIZE];
    int8 nonce[NONCE_SIZE];


    printf("\n");
    printf("=====================================\n");
    printf("       RCEX WEAK KEY TEST\n");
    printf("=====================================\n");



    /*
        Weak keys
    */


    memset(key,0,KEY_SIZE);
    memset(nonce,0,NONCE_SIZE);

    run_test(
        "ZERO KEY + ZERO NONCE",
        key,
        nonce
    );



    memset(key,0xFF,KEY_SIZE);

    run_test(
        "FF KEY + ZERO NONCE",
        key,
        nonce
    );



    for(int i=0;i<KEY_SIZE;i++)
        key[i]=0xAA;


    run_test(
        "AA PATTERN KEY",
        key,
        nonce
    );



    for(int i=0;i<KEY_SIZE;i++)
        key[i]=0x55;


    run_test(
        "55 PATTERN KEY",
        key,
        nonce
    );



    memset(key,0,KEY_SIZE);

    key[0]=1;


    run_test(
        "SINGLE BIT KEY",
        key,
        nonce
    );



    /*
        Weak nonces
    */


    memset(key,0xAA,KEY_SIZE);

    memset(nonce,0xFF,NONCE_SIZE);


    run_test(
        "AA KEY + FF NONCE",
        key,
        nonce
    );



    printf("\n");
    printf("Weak key testing complete\n");


    return 0;
}
