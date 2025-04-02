#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void printreg_to_file(const void *a, int nrof_byte, FILE *fp){
    int i;
    unsigned char *f = (unsigned char *)a;
    for(i=0; i < nrof_byte; i++){
        unsigned char bb = f[nrof_byte - 1 - i];
        fprintf(fp, "%01X %01X ",((bb >> 4) & 0xF), (bb & 0xF) ); //uint8_t c[4+8];
    }
    fprintf(fp, "\n");
}
void printreg(const void *a, int nrof_byte){
    printreg_to_file(a, nrof_byte, stdout);
}

#define ROUNDS 28
#define NR_OF_SBOX 16
#define NIBBLE 4
#define NR_OF_BYTE 8
#define GRP 4

#include "gift64.h"

int main(){
    uint64_t msg = 0x0123456789ABCDEFUL;
    uint64_t key[2] = {0x0123456789ABCDEFUL, 0x0123456789ABCDEFUL};
    printreg(&msg, 8);
    printreg(&key[0], 8);
    printreg(&key[1], 8);
    uint64_t cip;
    cip = enc_gift64(msg, key);
    printf("-----------------------------------------------\n");
    printreg(&cip, 8);
}
