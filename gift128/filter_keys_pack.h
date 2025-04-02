int check_alternating_sbox_activeness(uint8_t *diff){
    int flag = 1;
    for(int i=0; i<31; i++){
        if( ( (diff[i] != 0) && (diff[i+1] != 0) ) ){
            flag = 0;
        } 
    }
    return flag;
}
int check_alternating_bit_activeness(uint8_t *diff, int x){
    uint16_t p1 = 0x5555;
    uint16_t p2 = 0xAAAA;
    
    uint16_t diff1 = 0x0000;
    for(int i=0; i<4; i++){
        diff1 = (diff1 << 4) | diff[(4*x) + i]; 
    }
    if( ( (diff1 | p1) != p1 )  && ( (diff1 | p2) != p2 ) ){
            return 0;
    }
    return 1;
}

void get_diff(uint8_t *a, uint8_t *b, uint8_t *c, int grp1){
    memset(c, 0x00, 32);
    for (int i=0; i<4; i++){
        c[(4*grp1) + i] = a[(4*grp1) + i] ^ b[(4*grp1)  + i];
    }
}
void get_diff(uint8_t *a, uint8_t *b, uint8_t *c){
    memset(c, 0x00, 32);
    for (int i=0; i<32; i++){
        c[i] = a[i] ^ b[i];
    }
}
void generateProper(uint64_t key, int x, uint8_t *key_state){
    memset(key_state, 0x00, 32);
    int q = 8;
    //0, 8, 16, 24
    for(int i=0; i<4; i++){
        key_state[q*i + x] = ( ( (key >> (2*i)) & 0x03 ) << 1);
    }
}
void checkLastTwoPattern(uint8_t *cips, uint8_t *fcips, int grp,
                        uint32_t start38, uint32_t end38, uint32_t start39, uint32_t end39,
                        vector<uint8_t> &keys38, vector<uint8_t> &keys39){
    uint8_t key38[32];
    uint8_t key39[32];

    uint8_t dip[32];
    uint8_t fdip[32]; 
    
    uint8_t diff[32];
/* #ifdef PRINT */
/*     printf("Number of Faults = %d \n", EXP); */
/* #endif */
    for(uint64_t key1=start38; key1<end38; key1++) {  //40th round key
        for(uint64_t key2=start39; key2<end39; key2++) { //39th round key
            generateProper(key1, grp, key38);
            generateProper(key2, grp, key39);
            
            /* cout << grp <<" :Testing key: "; */
            /* printf("0x%04X 0x%04X \n", key1, key2); */
            /* printreg(key38, 32); */
            /* printreg(key39, 32); */
            /* cout << "Start EXPs ------------------------------------------------\n"; */
            int flag = 1;
        
            for(int j=0; j<EXP; j++){
                memcpy(dip, cips + (32*j), 32);
                memcpy(fdip,fcips + (32*j), 32);
                
                //last round inverse
                //We have P(X) + C + K and we guessed P^-1(K)
                //First compute P(X) + C + K + C  = P(X) + K
                //Then compute P^-1( P(X) + K ) + P^-1(K) = X
                addRc(dip, 39);
                addRc(fdip, 39);
                invPLayer(dip);
                invPLayer(fdip);
                addRk1(dip, key39);
                addRk1(fdip, key39);
                invSBox(dip);
                invSBox(fdip);
                
                //second last round inverse
                addRk1(dip, key38);
                addRk1(fdip, key38);
                addRc(dip, 38);
                addRc(fdip, 38);
                invPLayer(dip);
                invPLayer(fdip);
                invSBox(dip);
                invSBox(fdip);
                
                get_diff(dip,fdip,diff, grp);
                /* printreg(diff, 32); */
            
                //first filter
                if  (check_alternating_bit_activeness(diff, grp) == 1 ){
                    /* printf("filter 1 => \n"); */
                  //second filter
                  
                    invPLayer(diff);
                    /* printreg(diff, 32); */
                    if (check_alternating_sbox_activeness(diff) == 1){
                        /* printf("filter 2 => \n"); */
                        continue;
                    }
                    else{
                        flag = 0;
                        break;
                    }
                }
                else{
                    flag = 0;
                    break;
                }
            }
            if(flag == 1){
                /* cout << "found_one" << endl; */
                keys38.push_back(key1);
                keys39.push_back(key2);
            }
        }
    }
    /* printf("Remaining %d => %ld %ld %f\n", grp, keys38.size(), keys39.size(), log(keys38.size())/log(2)); */
    /* cout <<"\n-------------------------------------------------"<<endl; */
}

//test with correct key
void test_key(uint8_t *cips, uint8_t *fcips, uint8_t *k38, uint8_t *k39){
    printreg(k38, 32);
    printreg(k39, 32);
    for(int grp=0; grp < GRP; grp++){
        uint32_t key38 = 0;
        uint32_t key39 = 0;
        for(int i=0; i<4; i++){
            key38 = (key38 << 2) | ( (k38[8*(3-i) + grp]) >> 1) ;
            key39 = (key39 << 2) | ( (k39[8*(3-i) + grp]) >> 1 );
        }
        printf("0x%02X ", key38);
        printf("0x%02X \n", key39);
        vector<uint8_t> keys38;
        vector<uint8_t> keys39;
        checkLastTwoPattern(cips, fcips, grp, key38, key38+1, key39, key39+1, keys38, keys39);
    }
}


void find3839Key(uint8_t *cips, uint8_t *fcips, vector<vector<uint8_t> > &vkeys38, vector<vector<uint8_t> > &vkeys39){
    vector<vector<uint8_t> > keys38;
    vector<vector<uint8_t> > keys39;
    size_t size = (1 << 8);
    for(int grp=0; grp<8; grp++){
        vector<uint8_t> keys38_0;
        vector<uint8_t> keys39_0;
        checkLastTwoPattern(cips, fcips, grp, 0, size, 0, size, keys38_0, keys39_0);
        keys38.push_back(keys38_0);
        keys39.push_back(keys39_0);
    }
    combine_keys(cips, fcips, keys38, keys39, vkeys38, vkeys39);
}


