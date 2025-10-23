// rcex.c
#include"rcex.h"


void zero(int8 *str, int16 size) {
    int8 *p;
    int16 n;

    for (n=0, p=str; n<size; n++, p++){
        *p = 0;
    }

    return;
}

Rcex * rc4init(int8 * key, int16 size){
    int16 tmp1, tmp2;
    Rcex * p = (Rcex*) malloc(sizeof(struct s_rcex));
    if(!p){
        printf("Error: memory allocation failed for Rcex\n");
        exit(EXIT_FAILURE);
    }
    
    zero(p->s, 256);
    p->i=p->j=p->k = 0;
    tmp1=tmp2 = 0;

    for(p->i = 0; p->i < 256; p->s[p->i] = p->i, p->i++);

    for(p->i = 0; p->i < 256; p->i++){
        tmp1 = p->i % size;
        tmp2 = p->j + p->s[p->i] + key[tmp1];
        p->j = tmp2 % 256;  // j = (j + s[i] + key(i mod size)) mod 256

        tmp1 = p->s[p->i];  // swap
        p->s[p->i] = p->s[p->j];
        p->s[p->j] = tmp1;
    }
    p->i=p->j=p->k = 0;

    return p;
}

