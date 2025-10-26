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

export Rcex * rcexinit(int8 * key, int16 size){
    // Initialize RCEX context and perform key scheduling
    int16 tmp1, tmp2;
    Rcex * p = (Rcex*) malloc(sizeof(struct s_rcex));
    int32 n;

    if(!p){
        printf("Error: memory allocation failed for Rcex\n");
        exit(EXIT_FAILURE);
    }
    
    zero(p->s, 256);
    p->i=p->j=p->k = 0;
    tmp1=tmp2 = 0;
    
    for(p->i = 0; p->i < 256; p->s[p->i] = p->i, p->i++);  // Initialize S array

    // Key Scheduling Algorithm (KSA)
    for(p->i = 0; p->i < 256; p->i++){
        tmp1 = p->i % size;
        tmp2 = p->j + p->s[p->i] + key[tmp1];
        p->j = tmp2 % 256;  // j = (j + s[i] + key[i mod size]) mod 256

        tmp1 = p->s[p->i];  // swap
        p->s[p->i] = p->s[p->j];
        p->s[p->j] = tmp1;
    }

    p->i=p->j=p->k = 0; 
    rcexwhitewash(n, p);  // Discard first bytes to remove key bias

    return p;
}

int8 rcexbyte(Rcex * p){  // create 1 byte of keystream, will be used to encrypt and decrypt
    int16 tmp1, tmp2;

    p->i = (p->i + 1) % 256;
    p->j = (p->j + p->s[p->i]) % 256;

    tmp1 = p->s[p->i];  // swap
    p->s[p->i] = p->s[p->j];
    p->s[p->j] = tmp1;

    // Select keystream byte
    tmp2 = (p->s[p->i] + p->s[p->j]) % 256;
    p->k = p->s[tmp2];

    return p->k;
}

export int8 * rcexencrypt(Rcex * p, int8 * cleartext, int16 size){ 
    // XOR each plaintext byte with one byte of keystream
    int8 * ciphertext;
    int16 x;

    ciphertext = (int8*) malloc(size + 1);
    if(!ciphertext){
        printf("Error: memory allocation failed for ciphertext\n");
        exit(EXIT_FAILURE);
    }

    for(x = 0; x < size; x++){
        ciphertext[x] = cleartext[x] ^ rcexbyte(p);  // Ciphertext = cleartext XOR keystream
    }

    return ciphertext;

}