// rcex.h
#pragma once
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<errno.h>

#define rc4decrypt(x,y)     rc4encrypt(x,y)


typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
struct s_rcex{
    int16 i, j, k;
    int8 s[256];
};

typedef struct s_rcex Rcex;


Rcex * rc4init(int8 *, int16);
int8 rc4byte(void);
int8 * rc4encrypt(int8 *, int16);