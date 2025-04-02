//Sbox
const uint8_t sbox[16] = {3, 0, 6, 13, 11, 5, 8, 14, 12, 15, 9, 2, 4, 10, 7, 1};
const uint8_t invsbox[16] = {1, 15, 11, 0, 12, 5, 2, 14, 6, 10, 13, 4, 8, 3, 7, 9};

const uint8_t RC[35] = {2, 33, 16, 9, 36, 19, 40, 53, 26, 13, 38, 51, 56, 61, 62, 31, 14, 7, 34, 49, 24, 45, 54, 59, 28, 47, 22, 43, 20, 11, 4, 3, 32, 17, 8};
// tap positions for round contants
const uint8_t TP[6] = {8, 13, 19, 35, 67, 106};
void Display_state_nibble(uint8_t *state){

  for(int i = 0; i < 32; i++){
    printf("%x", state[32 - i - 1]);
  }
  printf("\n");

}
void Display_state_bit(uint8_t *state){
  printf(" ");
  for(int i = 0; i < 32; i++){
    for(int j = 0; j < 4; j++){
      printf("%x", ((state[32 - i - 1] >> (3-j)) & 0x1));
    }
    printf(" ");
  }
  printf("\n");
}

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
void addRc(uint8_t *state, int r){
    uint8_t buf[128];
    to_bits(state, buf);
    buf[TP[0]] ^= (RC[r]     ) & 0x1;
    buf[TP[1]] ^= (RC[r] >> 1) & 0x1;
    buf[TP[2]] ^= (RC[r] >> 2) & 0x1;
    buf[TP[3]] ^= (RC[r] >> 3) & 0x1;
    buf[TP[4]] ^= (RC[r] >> 4) & 0x1;
    buf[TP[5]] ^= (RC[r] >> 5) & 0x1;
    from_bits(buf, state);
}
void addRk(uint8_t *state, uint8_t *key){
    for(int i = 0; i < 32; i++){
        state[i] ^= key[i];
    }
}
void Key_update(uint8_t *key){
    uint8_t tmp[128];
    uint8_t buf[128];
    to_bits(key, tmp);
    //rotation
    for(int i = 0; i < 127; i++){
        buf[i] = tmp[i + 1];
    }
    buf[127] = tmp[0];
    from_bits(buf, key);
}
void enc(int rounds, uint8_t *state, uint8_t *key){
    uint8_t key_copy[32];
    memcpy(key_copy, key, 32);
    addRk(state, key);
    for(int r=0; r<rounds; r++){
        SBox(state);
        PLayer(state);
        addRc(state, r);
        Key_update(key);
        addRk(state, key);
    }
    memcpy(key, key_copy, 32);
}
