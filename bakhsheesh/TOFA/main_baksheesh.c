#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void printreg_to_file(const void *a, int nrof_byte, FILE *fp){
    int i;
    unsigned char *f = (unsigned char *)a;
    for(i=0; i < nrof_byte; i++){
        fprintf(fp, "%X ",(unsigned char) f[nrof_byte - 1 - i]); //uint8_t c[4+8];
    }
    fprintf(fp, "\n");
}
void printreg(const void *a, int nrof_byte){
    printreg_to_file(a, nrof_byte, stdout);
}

#include "baksheesh.h"

int main(){
    uint8_t key[32] ={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    uint8_t msg[32] = {0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4, 0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4, 0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4, 0x4,0x4,};
    uint8_t key_copy[32];
    uint8_t msg_copy[32];
    printreg(msg, 32);
    printreg(key, 32);
    memcpy(key_copy, key, 32);
    memcpy(msg_copy, msg, 32);
    enc(35, msg_copy, key_copy);
    printf("-----------------------------------------------\n");
    printreg(msg_copy, 32);
}
