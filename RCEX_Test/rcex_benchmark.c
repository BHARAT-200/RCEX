#include "rcex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define KEY_SIZE 32
#define NONCE_SIZE 16

#define TEST_SIZE (100 * 1024 * 1024)   // 100 MB


double get_time()
{
    struct timespec t;

    clock_gettime(
        CLOCK_MONOTONIC,
        &t
    );

    return t.tv_sec +
        t.tv_nsec / 1000000000.0;
}



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

    printf("\n");
    printf("=====================================\n");
    printf("        RCEX PERFORMANCE TEST\n");
    printf("=====================================\n");


    printf("Buffer size: %d MB\n",
        TEST_SIZE/(1024*1024));


    int8 key[KEY_SIZE];
    int8 nonce[NONCE_SIZE];


    random_bytes(key,KEY_SIZE);
    random_bytes(nonce,NONCE_SIZE);



    int8 *data =
        malloc(TEST_SIZE);


    if(!data)
    {
        printf("Memory allocation failed\n");
        return 1;
    }


    memset(
        data,
        0xAA,
        TEST_SIZE
    );



    /*
        Key setup benchmark
    */

    double start =
        get_time();


    Rcex *ctx =
        rcexinit_nonce(
            key,
            KEY_SIZE,
            nonce,
            NONCE_SIZE
        );


    double end =
        get_time();



    printf("\n");
    printf("Key setup time: %.6f ms\n",
        (end-start)*1000);



    /*
        Encryption benchmark
    */


    start =
        get_time();


    int8 *encrypted =
        rcexencrypt(
            ctx,
            data,
            TEST_SIZE
        );


    end =
        get_time();



    double enc_time =
        end-start;


    double mb =
        TEST_SIZE /
        (1024.0*1024.0);



    printf("\nEncryption:\n");

    printf(
        "Time: %.4f seconds\n",
        enc_time
    );


    printf(
        "Speed: %.2f MB/s\n",
        mb / enc_time
    );



    free(encrypted);


    rcexwipe(ctx);



    free(data);


    printf("\n");
    printf("Benchmark complete\n");


    return 0;
}