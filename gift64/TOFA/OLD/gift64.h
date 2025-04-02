#include "hal.h"
#include "simpleserial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int ROUNDS =28;
int NR_OF_SBOX =16;
int NIBBLE =4;
int NR_OF_BYTE =8;
int GRP =4;


int rc[28] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F, 0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B, 0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B};

int SB_table[16] = {0x1, 0xa, 0x4, 0xc, 0x6, 0xf, 0x3, 0x9, 0x2, 0xd, 0xb, 0x7, 0x5, 0x0, 0x8, 0xe};
int inv_SB_table[16] = {0xd, 0x0, 0x8, 0x6, 0x2, 0xc, 0x4, 0xb, 0xe, 0x7, 0x1, 0xa, 0x3, 0x9, 0xf, 0x5};

uint64_t circ_right_shift(uint64_t k, int n){
    uint64_t temp=(((k >> n) & 0xFFFF)|((k << (16 - n) )&0xFFFF))&0xFFFF;
    return temp;
}

void rotate_word(uint64_t *round_key_word){
    uint64_t temp1 = round_key_word[0];
    uint64_t temp2 = round_key_word[1];
    for(int i=2; i<8; i++){
        round_key_word[i-2] = round_key_word[i];
    }
    round_key_word[6] = temp1;
    round_key_word[7] = temp2;
}
void generate_round_keys(uint64_t key[2], uint64_t *round_keys){
    memset(round_keys, 0x00, ROUNDS*sizeof(uint64_t));
    
    uint64_t round_key_word[8];
    for(int i=0; i<4; i++){
        round_key_word[i] = (key[0] >> 16*i) & 0xFFFF; 
        round_key_word[4 + i] = (key[1] >> 16*i) & 0xFFFF; 
    }
    for(int r=0; r<ROUNDS; r++){
        round_keys[r] = (round_key_word[1] << 16) | round_key_word[0]; 
        
        rotate_word(round_key_word);
        round_key_word[6] = circ_right_shift(round_key_word[6], 12);
        round_key_word[7] = circ_right_shift(round_key_word[7],  2);
    }
}

uint64_t SBox(uint64_t msg){
    uint64_t cip = 0UL;
    uint64_t nibble;
    
    for(int i=0; i<NR_OF_SBOX; i++){
        nibble = SB_table[(msg >> 4*i)&0xf];
        cip = cip| (nibble << (4*i));
    }
    return cip;
}
int genPerm(int i) {
    return ((i >> 4) << 2) + (((3*((i & 0xf)>>2) + (i & 3)) & 3) << 4) + (i & 3);
}
uint64_t PLayer(uint64_t msg){
    uint64_t cip = 0UL;
    for(int i=0; i<64; i++){
        cip = cip | (((msg >> (63 - i)) & 1) << genPerm(63-i));
    }
    return cip;
}

uint64_t addRk(uint64_t msg, uint64_t key){
    uint64_t U,V, U1=0, V1=0;
    V = key&0xFFFF;
    U = (key>>16)&0xFFFF;
    for (int i=0; i<16; i++){
        V1 = (V1 << 4) | ((V >> (15-i)) & 0x01);
        U1 = (U1 << 4) | ((U >> (15-i)) & 0x01);
    }
    U1 = U1 << 1;
    
    msg = msg ^ U1 ^ V1;
    
    return msg;
}

uint64_t addRc(uint64_t msg, int r){
    uint64_t rcon = 0UL;
    int rcon_pos[6] = {23, 19, 15, 11, 7, 3};

	for(int i=0;i<6;i++){
		rcon = rcon | (((rc[r]>>i)&1) << rcon_pos[5-i]);
		}
	rcon = rcon|0x8000000000000000;	

	return (msg^rcon);
}

uint64_t enc_gift64(uint64_t msg, uint64_t key[2]){
    uint64_t round_keys[ROUNDS];
    //printreg(key, 16);
    generate_round_keys(key, round_keys);

    for(int r=0; r<ROUNDS; r++){
        msg = SBox(msg);
        msg = PLayer(msg);
        msg = addRk(msg, round_keys[r]);
        msg = addRc(msg, r);
    }
    return msg;
}
