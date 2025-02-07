#include <stdio.h>
#include <string.h>

int matches_at_equal_signia(const char * str ,const char * str1){
    // return 1 if str1 matches till '=' sign
    if (strlen(str)<=strlen(str1)){
        return 0;
    }
    int c ;
    int d ;
    for (int i =0;i<strlen(str1);i++){
        c= str[i];
        d= str1[i];
        if (c!=d){
            return 0;
        }
    }
    if (str[strlen(str1)]!='='){
        return 0;
    }
    return 1;
}

int hash(const char * str){
    int hash = 5381;
    int c;
    while ((c = *str++)!='\0'){
        hash = ((hash << 5) + hash ) + c;
    }
    return hash<0 ? -1*hash : hash;
}