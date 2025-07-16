//Sbox
const uint8_t sbox[16] = {3, 0, 6, 13, 11, 5, 8, 14, 12, 15, 9, 2, 4, 10, 7, 1};
const uint8_t invsbox[16] = {1, 15, 11, 0, 12, 5, 2, 14, 6, 10, 13, 4, 8, 3, 7, 9};

//bit permutation
const uint8_t PermBits[128] = {0, 33, 66, 99, 96, 1, 34, 67, 64, 97, 2, 35, 32, 65, 98, 3, 4, 37, 70, 103, 100, 5, 38, 71, 68, 101, 6, 39, 36, 69, 102, 7, 8, 41, 74, 107, 104, 9, 42, 75, 72, 105, 10, 43, 40, 73, 106, 11, 12, 45, 78, 111, 108, 13, 46, 79, 76, 109, 14, 47, 44, 77, 110, 15, 16, 49, 82, 115, 112, 17, 50, 83, 80, 113, 18, 51, 48, 81, 114, 19, 20, 53, 86, 119, 116, 21, 54, 87, 84, 117, 22, 55, 52, 85, 118, 23, 24, 57, 90, 123, 120, 25, 58, 91, 88, 121, 26, 59, 56, 89, 122, 27, 28, 61, 94, 127, 124, 29, 62, 95, 92, 125, 30, 63, 60, 93, 126, 31};
const uint8_t PermBitsInv[128] = {0, 5, 10, 15, 16, 21, 26, 31, 32, 37, 42, 47, 48, 53, 58, 63, 64, 69, 74, 79, 80, 85, 90, 95, 96, 101, 106, 111, 112, 117, 122, 127, 12, 1, 6, 11, 28, 17, 22, 27, 44, 33, 38, 43, 60, 49, 54, 59, 76, 65, 70, 75, 92, 81, 86, 91, 108, 97, 102, 107, 124, 113, 118, 123, 8, 13, 2, 7, 24, 29, 18, 23, 40, 45, 34, 39, 56, 61, 50, 55, 72, 77, 66, 71, 88, 93, 82, 87, 104, 109, 98, 103, 120, 125, 114, 119, 4, 9, 14, 3, 20, 25, 30, 19, 36, 41, 46, 35, 52, 57, 62, 51, 68, 73, 78, 67, 84, 89, 94, 83, 100, 105, 110, 99, 116, 121, 126, 115};

// round constants
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
void invSBox(uint8_t *state){
    //invSBox
    for(int i=0; i<32; i++){
    	state[i] = invsbox[state[i]];
    }
}
void PLayer(uint8_t *state){
    uint8_t tmp[128];
    uint8_t bits[128];
    to_bits(state, tmp);
    for(int i = 0; i < 128; i++){
        bits[PermBits[i]] = tmp[i];
    }
    from_bits(bits, state);
}
void invPLayer(uint8_t *state){
    uint8_t tmp[128];
    uint8_t bits[128];
    to_bits(state, tmp);
    for(int i = 0; i < 128; i++){
        bits[PermBitsInv[i]] = tmp[i];
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
void Key_update_dec(uint8_t *key){
    uint8_t tmp[128];
    uint8_t buf[128];
    to_bits(key, tmp);
    //rotation
    for(int i = 0; i < 127; i++){
        buf[i+1] = tmp[i];
    }
    buf[0] = tmp[127];
    from_bits(buf, key);
}

void enc(int rounds, uint8_t *state, uint8_t *key){
    uint8_t key_copy[32];
    memcpy(key_copy, key, 32);
    /* Display_state_nibble(state); */
    //whitening key
    addRk(state, key);
    /* Display_state_nibble(state); */
    //round function
    for(int r=0; r<rounds; r++){
        SBox(state);
        PLayer(state);
        addRc(state, r);
        Key_update(key);
        addRk(state, key);
    }
    memcpy(key, key_copy, 32);
}
void dec(int rounds, uint8_t *state, uint8_t *key){
    //set key
    for(int i = 0; i < rounds; i++){
        Key_update(key);
    }
    /* Display_state_nibble(state); */
    //round function
    for(int r=0; r<rounds; r++){
        addRk(state, key);
        Key_update_dec(key);
        addRc(state, rounds - 1 - r);
        invPLayer(state);
        invSBox(state);
        /* Display_state_nibble(state); */
    }
    //whitening key
    addRk(state, key);
}
void byte_fault_simulation(int rounds, uint8_t *state1, uint8_t *state2, uint8_t *key, int fr){
    int f_bit_index; 
    int offset;
    uint64_t fault_value;
    uint8_t fault_state[32];
    uint8_t key_copy[32];
    uint8_t diff[32];
    memcpy(key_copy, key, 32);
    //whitening key
    addRk(state1, key);
    addRk(state2, key);
    fr = fr -1;
    for(int r=0; r<rounds; r++){
        Key_update(key);
        if(r == (rounds-1)){
            invPLayer(key);
        }
        SBox(state1);
        if(r != (rounds - 1)){
            PLayer(state1);
            addRc(state1, r);
        }
        addRk(state1, key);

        SBox(state2);
        if(r != (rounds - 1)){
            PLayer(state2);
            addRc(state2, r);
        }
        addRk(state2, key);

        if(r == fr){
            f_bit_index = rand() % 56;
            offset = rand() % 2;

            fault_value = rand() & 0x00000000000000FFUL;
            while(fault_value == 0){
                fault_value = rand() & 0x00000000000000FFUL;
            }

            /* printf("%d %d %016LX \n", f_bit_index, offset, fault_value); */
            fault_value = fault_value << f_bit_index;

            memset(fault_state, 0x00, 32);
            if(offset > 0){
                offset = 16;
            }
            for(int i=0; i<16; i++){
                fault_state[offset + i] = (fault_value >> (4*i)) & 0x0F;
            }
            xor_sum_32(state2, fault_state, state2);
            /* printf("%d %d %016LX \n", f_bit_index, offset, fault_value); */
            printreg(fault_state, 32);
        }
        xor_sum_32(state1, state2, diff);
        /* printreg(diff, 32); */
    }
    memcpy(key, key_copy, 32);
}
int fenc(int rounds, uint8_t *state, uint8_t *key, int fr){
    int f_nibble_index; 
    uint8_t key_copy[32];
    memcpy(key_copy, key, 32);
    /* Display_state_nibble(state); */
    //whitening key
    addRk(state, key);
    for(int r=0; r<rounds; r++){
        SBox(state);
        PLayer(state);
        addRc(state, r);
        Key_update(key);
        addRk(state, key);
        if(r == fr){
            f_nibble_index = rand64() % 32;
            uint64_t fault_value = rand64() & 0xF;
            while(fault_value == 0){
                fault_value = rand64() & 0xF;
            }
            state[f_nibble_index] = state[f_nibble_index] ^ fault_value;
        }
    }
    memcpy(key, key_copy, 32);
    return f_nibble_index;
}
int dual_enc_bit(int rounds, uint8_t *state1, uint8_t *state2, uint8_t *key, int fr){
    int f_bit_index;
    uint8_t key_copy[32];
    uint8_t diff[32];
    memcpy(key_copy, key, 32);
    //whitening key
    addRk(state1, key);
    addRk(state2, key);
    for(int r=0; r<rounds; r++){
        Key_update(key);
        if(r == (rounds-1)){
            invPLayer(key);
        }
        SBox(state1);
        if(r != (rounds - 1)){
            PLayer(state1);
            addRc(state1, r);
        }
        addRk(state1, key);

        SBox(state2);
        if(r != (rounds - 1)){
            PLayer(state2);
            addRc(state2, r);
        }
        addRk(state2, key);
        if(r == fr){
            f_bit_index = rand64() % 128;
            int f_nibble_index = f_bit_index/4;
            int index = f_bit_index % 4;
            uint8_t fault_value = (0x01 << index);
            state2[f_nibble_index] = state2[f_nibble_index] ^ fault_value;
            printf("%d %d %016LX \n",f_bit_index, f_nibble_index, fault_value);
        }
        xor_sum_32(state1, state2, diff);
        /* printreg(diff, 32); */
    }
    /* cout << "--------------------------------------------------------------\n"; */
    memcpy(key, key_copy, 32);
    return f_bit_index;
}
int dual_enc_nibble(int rounds, uint8_t *state1, uint8_t *state2, uint8_t *key, int fr){
    int f_nibble_index; 
    uint8_t key_copy[32];
    uint8_t diff[32];
    memcpy(key_copy, key, 32);
    //whitening key
    addRk(state1, key);
    addRk(state2, key);
    for(int r=0; r<rounds; r++){
        Key_update(key);
        if(r == (rounds-1)){
            invPLayer(key);
        }
        SBox(state1);
        if(r != (rounds - 1)){
            PLayer(state1);
            addRc(state1, r);
        }
        addRk(state1, key);

        SBox(state2);
        if(r != (rounds - 1)){
            PLayer(state2);
            addRc(state2, r);
        }
        addRk(state2, key);

        if(r == fr){
            f_nibble_index = rand64() % 32;
            uint64_t fault_value = rand64() & 0xF;
            while(fault_value == 0){
                fault_value = rand64() & 0xF;
            }
            state2[f_nibble_index] = state2[f_nibble_index] ^ fault_value;
        }
        xor_sum_32(state1, state2, diff);
        /* printreg(diff, 32); */
    }
    /* cout << "--------------------------------------------------------------\n"; */
    memcpy(key, key_copy, 32);
    return f_nibble_index;
}
int dual_enc_byte(int rounds, uint8_t *state1, uint8_t *state2, uint8_t *key, int fr){
    int f_bit_index; 
    int offset;
    uint64_t fault_value;
    uint8_t fault_state[32];
    uint8_t key_copy[32];
    uint8_t diff[32];
    memcpy(key_copy, key, 32);
    //whitening key
    addRk(state1, key);
    addRk(state2, key);
    fr = fr -1;
    for(int r=0; r<rounds; r++){
        Key_update(key);
        if(r == (rounds-1)){
            invPLayer(key);
        }
        SBox(state1);
        if(r != (rounds - 1)){
            PLayer(state1);
            addRc(state1, r);
        }
        addRk(state1, key);

        SBox(state2);
        if(r != (rounds - 1)){
            PLayer(state2);
            addRc(state2, r);
        }
        addRk(state2, key);

        if(r == fr){
            f_bit_index = rand64() % 56;
            offset = rand64() % 2;

            fault_value = rand64() & 0x00000000000000FFUL;
            while(fault_value == 0){
                fault_value = rand64() & 0x00000000000000FFUL;
            }

            /* printf("%d %d %016LX \n", f_bit_index, offset, fault_value); */
            fault_value = fault_value << f_bit_index;

            memset(fault_state, 0x00, 32);
            if(offset > 0){
                offset = 16;
            }
            for(int i=0; i<16; i++){
                fault_state[offset + i] = (fault_value >> (4*i)) & 0x0F;
            }
            xor_sum_32(state2, fault_state, state2);
            /* printf("%d %d %016LX \n", f_bit_index, offset, fault_value); */
            printreg(fault_state, 32);
        }
        xor_sum_32(state1, state2, diff);
        /* printreg(diff, 32); */
    }
    memcpy(key, key_copy, 32);
    return f_bit_index;
}
