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
    rc4 = rc4init(key, skey);
    printf("\ndone\n");

    printf("'%s'\n -> ", from);
    // encrypted = rc4encrypt(from, stext);
    printbin(key, skey);


    printf("\nrc4 after inti:-"); F;
    printbin(rc4->s, skey);

    for(int i=0; i<skey; printf("s->[%d] = %d\n", i, rc4->s[i]), i++);

}