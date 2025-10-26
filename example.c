// example.c
#include"rcex.h"

#define F fflush(stdout)

int main(void);

void printbin(int8 * input, const int16 size){
    int16 i;
    int8 * p;

    assert(size > 0);

    for(i=size, p=input; i; i--, p++){
        if(!(i % 2)){  // if even
            printf(" ");
        }
        printf("%.02x", *p);
    }

    printf("\n");

    return;
}

int main(){
    Rcex * rc4;
    int16 skey, stext;
    int8 * key, * from, * encrypted, * decrypted;

    key = from = encrypted = decrypted = 0;
    skey = stext = 0;

    key = (int8*) "tomato";
    skey = strlen((char*)key);
    from = (int8*) "She said, Don't make others suffer for your personal hatered.";
    stext = strlen((char*)from);

    printf("Init encryption"); F;
    rc4 = rcexinit(key, skey);
    printf("\t done\n");

    printf("'%s'\t -> ", from);
    encrypted = rcexencrypt(rc4, from, stext);
    printbin(encrypted, stext);

    rcexuninit(rc4);

    
    printf("Init encryption"); F;
    rc4 = rcexinit(key, skey);  // Reinitialize RC4 with the same key before decrypting (same keystream ensures correct decryption)
    printf("\tdone\n");

    decrypted = rcexdecrypt(rc4, encrypted, stext);
    printf("        -> '%s'\n", decrypted);
    rcexuninit(rc4);

    free(encrypted);
    free(decrypted);

}