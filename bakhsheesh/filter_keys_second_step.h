int check_alternating_bit_activeness_2(uint32_t diff){
    uint32_t p1 = 0x55555555;
    uint32_t p2 = 0xAAAAAAAA;
    if( ( (diff | p1) != p1 )  && ( (diff | p2) != p2 ) ){
            return 0;
    }
    return 1;
}

uint32_t get_diff_2(uint8_t *a, uint8_t *b, int x){
    uint32_t diff = 0x00000000;
    for (int i=0; i<8; i++){
        diff = (diff << 4) | (a[(8*x) + 7 - i] ^ b[(8*x) + 7 - i]);
    }
    return diff;
}
void generateProper2(uint64_t key, int x, uint8_t *key_state){
    memset(key_state, 0x00, 32);
    int q = 8;
    for(int j=0; j<2; j++){
        uint16_t na = (key >> (16*j)) & 0xFFFF;
        for(int i=0; i<4; i++){
            key_state[q*i + 2*x + j]     = (na >> (4*i) ) & 0x0F;
        }
    }
}
void check_2_grp(uint8_t *cips, uint8_t *fcips, vector<uint32_t> &keys33, vector<uint32_t> &keys34, 
                vector<uint32_t> &vkeys33, vector<uint32_t> &vkeys34, int grp){
    uint8_t key33[32];
    uint8_t key34[32];

    uint8_t dip[32];
    uint8_t fdip[32];

    uint32_t diff;
    /* cout << " =>" << grp <<" :Testing key check_2_grp: \n"; */
    for(int i=0; i<keys33.size(); i++){

        uint32_t key1 = keys33[i];
        uint32_t key2 = keys34[i];
        generateProper2(key1, grp, key33);
        generateProper2(key2, grp, key34);
        
        /* cout << grp <<" :Testing key: "; */
        /* printf("0x%04X 0x%04X \n", key1, key2); */
        /* printreg(key33, 32); */
        /* printreg(key34, 32); */
        /* cout << "Start EXPs ------------------------------------------------\n"; */
        int flag = 1;
        for(int j=0; j<EXP; j++){
            memcpy(dip, cips + (32*j), 32);
            memcpy(fdip, fcips + (32*j), 32);
            /* printreg(dip, 32); */
            /* printreg(fdip, 32); */

            //last round inverse
            addRk(dip, key34);
            addRk(fdip, key34);
            invSBox(dip);
            invSBox(fdip);

            //second last round inverse
            addRk(dip, key33);
            addRk(fdip, key33);
            addRc(dip, 33);
            addRc(fdip, 33);
            invPLayer(dip);
            invPLayer(fdip);
            invSBox(dip);
            invSBox(fdip);

            diff = get_diff_2(dip,fdip,grp);
            /* printreg(&diff, 4); */
            //first filter
            if  (check_alternating_bit_activeness_2(diff) == 1 ){
                /* printf("First filter pass \n"); */
                continue;
            }
            else{
                flag = 0;
                break;
            }
        }   
        if(flag == 1){
            vkeys33.push_back(key1);
            vkeys34.push_back(key2);
        }
    }
}
int check_alternating_bit_activeness_4(uint64_t diff){
    uint64_t p1 = 0x55555555AAAAAAAAUL;
    uint64_t p2 = 0xAAAAAAAA55555555UL;
    if( ( (diff | p1) != p1 )  && ( (diff | p2) != p2 ) ){
            return 0;
    }
    return 1;
}

uint32_t get_diff_4(uint8_t *a, uint8_t *b, int x){
    uint64_t diff = 0x0000000000000000UL;
    for (int i=0; i<16; i++){
        diff = (diff << 4) | (a[(16*x) + 15 - i] ^ b[(16*x) + 15 - i]);
    }
    return diff;
}
void generateProper4(uint64_t key, int x, uint8_t *key_state){
    memset(key_state, 0x00, 32);
    int q = 8;
    for(int j=0; j<4; j++){
        uint16_t na = (key >> (16*j)) & 0xFFFF;
        for(int i=0; i<4; i++){
            key_state[q*i + 4*x + j]     = (na >> (4*i) ) & 0x0F;
        }
    }
}
void check_4_grp(uint8_t *cips, uint8_t *fcips, vector<uint64_t> &keys33, vector<uint64_t> &keys34, 
                vector<uint64_t> &vkeys33, vector<uint64_t> &vkeys34, int grp){
    uint8_t key33[32];
    uint8_t key34[32];

    uint8_t dip[32];
    uint8_t fdip[32];

    uint64_t diff;
    for(int i=0; i<keys33.size(); i++){

        uint64_t key1 = keys33[i];
        uint64_t key2 = keys34[i];
        generateProper4(key1, grp, key33);
        generateProper4(key2, grp, key34);
        
        /* cout << grp <<" :Testing key: "; */
        /* printreg(key33, 32); */
        /* printreg(key34, 32); */
        /* cout << "Start EXPs ------------------------------------------------\n"; */

        int flag = 1;
        for(int j=0; j<EXP; j++){
            memcpy(dip, cips + (32*j), 32);
            memcpy(fdip, fcips + (32*j), 32);
            /* printreg(dip, 32); */
            /* printreg(fdip, 32); */

            //last round inverse
            addRk(dip, key34);
            addRk(fdip, key34);
            invSBox(dip);
            invSBox(fdip);

            //second last round inverse
            addRk(dip, key33);
            addRk(fdip, key33);
            addRc(dip, 33);
            addRc(fdip, 33);
            invPLayer(dip);
            invPLayer(fdip);
            invSBox(dip);
            invSBox(fdip);

            diff = get_diff_4(dip,fdip,grp);
            /* printreg(&diff, 4); */
            //first filter
            if  (check_alternating_bit_activeness_4(diff) == 1 ){
                continue;
            }
            else{
                printf("4grp First filter Fail \n");
                flag = 0;
                break;
            }
        }   
        if(flag == 1){
            vkeys33.push_back(key1);
            vkeys34.push_back(key2);
        }
    }
}
int check_alternating_bit_activeness_8(uint8_t *diff){
    uint64_t p1 = 0x55555555AAAAAAAAUL;
    uint64_t p2 = 0xAAAAAAAA55555555UL;
    //p1p1 | p2p2

    uint64_t diff1 = 0x0000000000000000UL;
    uint64_t diff2 = 0x0000000000000000UL;
    for(int i=0; i<16; i++){
        diff1 = (diff1 << 4) | diff[i];
        diff2 = (diff2 << 4) | diff[16 + i];
    }
    if( ( ((diff1|p1)==p1)&&((diff2|p1)==p1))  || (((diff1|p2)==p2)&&((diff2|p2)==p2))) {
            return 1;
    }
    return 0;
}

void get_diff_8(uint8_t *a, uint8_t *b, uint8_t *c){
    memset(c, 0x00, 32);
    for (int i=0; i<32; i++){
        c[i] = a[i] ^ b[i];
    }
}
int check_30(uint8_t *diff){
    uint64_t diff1 = 0x0000000000000000UL;
    uint64_t diff2 = 0x0000000000000000UL;
    for(int i=0; i<16; i++){
        diff1 = (diff1 << 4) | diff[15 - i];
        diff2 = (diff2 << 4) | diff[16 + 15 - i];
    }
    int count1 = __builtin_popcountll(diff1);
    int count2 = __builtin_popcountll(diff2);
    if( ( (count1 == 1) && (count2 == 0) ) || ( (count1 == 0) && (count2 == 1) ) ){
        return 1;
    }
    return 0;
}


int check_8_grp(uint8_t *cips, uint8_t *fcips, uint8_t *key34){
    uint8_t dip[32];
    uint8_t fdip[32];

    uint8_t diff[32];
    int flag = 1;
    for(int j=0; j<EXP; j++){
        uint8_t key[32];
        memcpy(key, key34, 32);

        memcpy(dip, cips + (32*j), 32);
        memcpy(fdip,fcips + (32*j), 32);
        
        //last round inverse
        addRk(dip, key);
        addRk(fdip, key);
        invSBox(dip);
        invSBox(fdip);
        
        PLayer(key);
        Key_update_dec(key);
        /* printreg(key, 32); */
        //second last round inverse
        addRk(dip, key);
        addRk(fdip, key);
        addRc(dip, 33);
        addRc(fdip, 33);
        invPLayer(dip);
        invPLayer(fdip);
        invSBox(dip);
        invSBox(fdip);
        
        Key_update_dec(key);
        addRk(dip, key);
        addRk(fdip, key);
        addRc(dip, 32);
        addRc(fdip, 32);
        invPLayer(dip);
        invPLayer(fdip);
        invSBox(dip);
        invSBox(fdip);

        get_diff_8(dip,fdip,diff);
        /* printreg(diff, 32); */
        if(check_32(diff)){
            /* cout << "Pass 32" << endl; */
            Key_update_dec(key);
            addRk(dip, key);
            addRk(fdip, key);
            addRc(dip, 31);
            addRc(fdip, 31);
            invPLayer(dip);
            invPLayer(fdip);
            invSBox(dip);
            invSBox(fdip);
            get_diff_8(dip,fdip,diff);
            if(check_31(diff)){
                /* cout << "Pass 31" << endl; */
                #ifdef BIT_FAULT
                    Key_update_dec(key);
                    addRk(dip, key);
                    addRk(fdip, key);
                    addRc(dip, 30);
                    addRc(fdip, 30);
                    invPLayer(dip);
                    invPLayer(fdip);
                    invSBox(dip);
                    invSBox(fdip);
                    get_diff_8(dip,fdip,diff);
                    if(check_30(diff) == 1){
                        continue;
                    }
                    else{
                        flag = 0;
                        break;
                    }
                #else
                    continue;
                #endif
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
    return flag;
}
//test with correct key
void test_key2(uint8_t *cips, uint8_t *fcips, uint8_t *k34, uint8_t *k33){
    printreg(k33, 32);
    printreg(k34, 32);
    
    for(int grp=0; grp < 4; grp++){
        vector<uint32_t> keys33;
        vector<uint32_t> keys34;
        
        uint32_t key33 =0;
        uint32_t key34 =0;

        for(int j=0; j<2; j++){
            for(int i=0; i<4; i++){
                key33 = (key33 << 4) | k33[8*(3-i) + 2*grp + (1-j)];
                key34 = (key34 << 4) | k34[8*(3-i) + 2*grp + (1-j)];
            }
        }
        printf("0x%08X ", key33);
        printf("0x%08X \n", key34);

        keys33.push_back(key33);
        keys34.push_back(key34);

        vector<uint32_t> vkeys33;
        vector<uint32_t> vkeys34;
        check_2_grp(cips, fcips, keys33, keys34, vkeys33, vkeys34, grp);
        printf("0x%08X ", vkeys33[0]);
        printf("0x%08X \n", vkeys34[0]);
    }
}
void test_key4(uint8_t *cips, uint8_t *fcips, uint8_t *k34, uint8_t *k33){
    printreg(k33, 32);
    printreg(k34, 32);
    
    for(int grp=0; grp < 2; grp++){
        vector<uint64_t> keys33;
        vector<uint64_t> keys34;
        
        uint64_t key33 =0000000000000000UL;
        uint64_t key34 =0000000000000000UL;
        for(int j=0; j<4; j++){
            for(int i=0; i<4; i++){
                key33 = (key33 << 4) | k33[8*(3-i) + 4*grp + (3-j)];
                key34 = (key34 << 4) | k34[8*(3-i) + 4*grp + (3-j)];
            }
        }
        printf("0x%016LX ", key33);
        printf("0x%016LX \n", key34);

        keys33.push_back(key33);
        keys34.push_back(key34);

        vector<uint64_t> vkeys33;
        vector<uint64_t> vkeys34;
        check_4_grp(cips, fcips, keys33, keys34, vkeys33, vkeys34, grp);
        printf("0x%016LX ", vkeys33[0]);
        printf("0x%016LX \n", vkeys34[0]);
    }
}
void test_key8(uint8_t *cips, uint8_t *fcips, uint8_t *k34){
    printreg(k34, 32);
    int flag = check_8_grp(cips, fcips, k34);
    printf("Flag = %d\n", flag);
}
