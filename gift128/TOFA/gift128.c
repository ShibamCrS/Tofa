#include <stdio.h>
#include <stdint.h>
#include "hal.h"
#include "simpleserial.h"
//Sbox
const uint8_t sbox[16] = {1,10, 4,12, 6,15, 3, 9, 2,13,11, 7, 5, 0, 8,14};
const uint8_t invsbox[16] = {13, 0, 8, 6, 2,12, 4,11,14, 7, 1,10, 3, 9,15, 5};

//X0 <- x00, X1 <- x01,...., X4 <- x10, X5 <- x11,... etc.
//127,126,125,124,          .....        7,6,5,4,  3,2,1,0
void to_bits(uint8_t *A, uint8_t *B){
    for(int i = 0; i < 32; i++){
        for(int j = 0; j < 4; j++){
        B[(i * 4) + j] = (A[i] >> j) & 0x1;
        }
    }
}
void from_bits(uint8_t *A, uint8_t *B){
    //convert bit-wise variables into nibble-wise variables
    for(int i = 0; i < 32; i++){
        //0 is LSB and 3 is MSB in each nibble
        B[i]  = (A[(4 * i)]         );
        B[i] ^= (A[(4 * i) + 1] << 1);
        B[i] ^= (A[(4 * i) + 2] << 2);
        B[i] ^= (A[(4 * i) + 3] << 3);
    }
}
void SBox(uint8_t *state){
    //SBox
    for(int i=0; i<32; i++){
    	state[i] = sbox[state[i]];
    }
}

int genPerm(int i) {
    return ((i >> 4) << 2) + (((3*((i & 0xf)>>2) + (i & 3)) & 3) << 5) + (i & 3);
}
void PLayer(uint8_t *state){
    uint8_t tmp[128];
    uint8_t bits[128];
    to_bits(state, tmp);
    for(int i = 0; i < 128; i++){
        bits[genPerm(i)] = tmp[i];
    }
    from_bits(bits, state);
}
void Key_update(uint8_t *key){
    uint8_t temp_key[32];
    //entire key>>32
    for(int i=0; i<32; i++){
        temp_key[i] = key[(i+8)%32];
    }
    for(int i=0; i<24; i++) key[i] = temp_key[i];
    //k0>>12
    key[24] = temp_key[27];
    key[25] = temp_key[24];
    key[26] = temp_key[25];
    key[27] = temp_key[26];
    //k1>>2
    key[28] = ((temp_key[28]&0xc)>>2) ^ ((temp_key[29]&0x3)<<2);
    key[29] = ((temp_key[29]&0xc)>>2) ^ ((temp_key[30]&0x3)<<2);
    key[30] = ((temp_key[30]&0xc)>>2) ^ ((temp_key[31]&0x3)<<2);
    key[31] = ((temp_key[31]&0xc)>>2) ^ ((temp_key[28]&0x3)<<2);
}
void addRk(uint8_t *state, uint8_t *key){
    uint8_t bits[128];
    uint8_t key_bits[128];
    to_bits(key, key_bits);
    to_bits(state, bits);
    int kbc=0;  //key_bit_counter
    for (int i=0; i<32; i++){
        bits[4*i+1] ^= key_bits[kbc];
        bits[4*i+2] ^= key_bits[kbc+64];
        kbc++;
    }
    from_bits(bits, state);
}

void addRc(uint8_t *state, uint8_t RC){
    uint8_t bits[128];
    to_bits(state, bits);
    //add constant
    bits[3] ^= RC & 0x1;
    bits[7] ^= (RC>>1) & 0x1;
    bits[11] ^= (RC>>2) & 0x1;
    bits[15] ^= (RC>>3) & 0x1;
    bits[19] ^= (RC>>4) & 0x1;
    bits[23] ^= (RC>>5) & 0x1;
    bits[127] ^= 1;
    from_bits(bits, state);
}
uint8_t updateConst(uint8_t cc){
    return ((cc << 1) ^ ( ((cc >> 5) & 0x01) ^ ((cc >> 4) & 0x01) ^ 1) );
}
void enc(int rounds, uint8_t *state, uint8_t *key){
    uint8_t rconst = 0x00;
    for(int r=0; r<rounds; r++){
        if(r==35) trigger_high();
        SBox(state);
        if(r==35) trigger_low();
        PLayer(state);
        rconst = updateConst(rconst);
        addRc(state, rconst);
        addRk(state, key);
        Key_update(key);
        
        
    }
}
