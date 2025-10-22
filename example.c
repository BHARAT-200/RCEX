// example.c
#include"rcex.h"

#define F fflush(stdout)

int main(void);

int main(){
    Rcex * rc4;
    int16 skey, stext;
    char * key, * from, * encrypted, * decrypted;

    key = "tomato";
    skey = strlen(key);
    from = "She said, Don't make others suffer for your personal hatered.";
    stext = strlen(from);

    printf("Init encryption"); F;
    rc4 = rc4init(key, skey);
    printf("done\n");

    printf("'%s'\n -> ", from);
    encrypted = rc4encrypt(from, stext);


}