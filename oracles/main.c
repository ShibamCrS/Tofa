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

#include "gift128.h"
#include "gift128_bitsliced.h"

int main(){
    uint8_t msg[32] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};
    uint8_t key[32] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};
    uint8_t key_copy[32];
    uint8_t msg_copy[32];
    printreg(msg, 32);
    printreg(key, 32);
    memcpy(key_copy, key, 32);
    memcpy(msg_copy, key, 32);
    enc(40, msg_copy, key_copy);
    printf("-----------------------------------------------\n");
    printreg(msg_copy, 32);

    printf("-----------------BitSliced-----------------------\n");
    uint8_t P[16] = {0x00};
    uint8_t K[16] = {0x00};
    /* 0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10}; */
    /* uint8_t K[16] = {0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10}; */
    uint8_t C[16];
    giftb128(P, K, C);
    printreg(K, 16);
    printreg(K, 16);
    printreg(C, 16);
}
