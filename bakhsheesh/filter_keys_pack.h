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
void generateProper(uint64_t key, int x, uint8_t *key_state){
    memset(key_state, 0x00, 32);
    int q = 8;
    for(int i=0; i<4; i++){
        key_state[q*i + x] = (key >> (4*i)) & 0x0F;
    }
}
void get_diff(uint8_t *a, uint8_t *b, uint8_t *c, int grp1){
    memset(c, 0x00, 32);
    for (int i=0; i<4; i++){
        c[(4*grp1) + i] = a[(4*grp1) + i] ^ b[(4*grp1)  + i];
    }
}
struct Args{
    int grp;
    uint64_t start33;
    uint64_t end33;
    uint64_t start34;
    uint64_t end34;
    uint8_t *cips;
    uint8_t *fcips;
    vector<uint16_t> keys33;
    vector<uint16_t> keys34;
};
void *checkLastTwoPattern(void *args1){
    Args *args = (Args*) args1;

    uint8_t key34[32];
    uint8_t key33[32];

    uint8_t dip[32];
    uint8_t fdip[32]; 
    
    uint8_t diff[32];
    for(uint64_t key1=args->start33; key1<args->end33; key1++) {  //34th round key
        for(uint64_t key2=args->start34; key2<args->end34; key2++) { //33th round key
            generateProper(key1, args->grp, key33);
            generateProper(key2, args->grp, key34);
            
            /* cout << args->grp <<" :Testing key: "; */
            /* printf("0x%04X 0x%04X \n", key2, key1); */
            /* printreg(key33, 32); */
            /* printreg(key34, 32); */
            /* cout << "Start EXPs ------------------------------------------------\n"; */
            int flag = 1;
        
            for(int j=0; j<EXP; j++){
                memcpy(dip, args->cips + (32*j), 32);
                memcpy(fdip, args->fcips + (32*j), 32);
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
                
                get_diff(dip,fdip,diff, args->grp);
                /* printreg(diff, 32); */
            
                //first filter
                if  (check_alternating_bit_activeness(diff, args->grp) == 1 ){
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
                args->keys33.push_back(key1);
                args->keys34.push_back(key2);
            }
        }
    }
    free(args->cips);
    free(args->fcips);
    pthread_exit(NULL);
}

void test_w_key(uint8_t *cips, uint8_t *fcips, int grp, uint64_t key33, uint64_t key34){
    pthread_t thread_ids[1];
    Args thread_args[1];

    for(int i=0; i <1; i++){
        thread_args[i].grp = grp;
        thread_args[i].start33 = key33;
        thread_args[i].end33 = key33 + 1;
        thread_args[i].start34 = key34;
        thread_args[i].end34 = key34 + 1;
        thread_args[i].cips = (uint8_t *)malloc(EXP*32*sizeof(uint8_t));
        thread_args[i].fcips = (uint8_t *)malloc(EXP*32*sizeof(uint8_t));
        memcpy(thread_args[i].cips, cips, EXP*32*sizeof(uint8_t));
        memcpy(thread_args[i].fcips, fcips, EXP*32*sizeof(uint8_t));
    }

    for(int i=0; i < 1; i++){
        pthread_create(thread_ids + i, NULL, checkLastTwoPattern, (void*) (thread_args + i));
    }
    size_t total_size = 0;
    for(int i=0; i < 1; i++){
        pthread_join(thread_ids[i], NULL);
        total_size = + thread_args[i].keys33.size();
    }
    cout << total_size << endl;
}
//test with correct key
void test_key(uint8_t *cips, uint8_t *fcips, uint8_t *k34, uint8_t *k33){
    printreg(k33, 32);
    printreg(k34, 32);
    for(int grp=0; grp < GRP; grp++){
        uint32_t key33 = 0;
        uint32_t key34 = 0;
        for(int i=0; i<4; i++){
            key33 = (key33 << 4) | (k33[8*(3-i) + grp]);
            key34 = (key34 << 4) | (k34[8*(3-i) + grp]);
        }
        printf("0x%04X ", key33);
        printf("0x%04X ", key34);
        test_w_key(cips, fcips, grp, key33, key34);
    }
}
void for_each_group(vector<uint16_t> &keys33, vector<uint16_t> &keys34, 
                    uint8_t *cips, uint8_t *fcips, int grp){
    uint64_t sizeEachGroup = (1UL << 16);
    pthread_t thread_ids[THREAD];
    Args thread_args[THREAD];

    uint64_t data_each_thread = sizeEachGroup / THREAD;
    uint64_t data_last_thread = sizeEachGroup % THREAD;
    /* printf("%f \n", log(data_each_thread)/log(2)); */
    /* printf("%f \n", log(data_last_thread)/log(2)); */

    for(int i=0; i < THREAD; i++){
        thread_args[i].start34 = 0;
        thread_args[i].end34 = sizeEachGroup;
        if( i == (THREAD-1) ){
            thread_args[i].start33 = i * data_each_thread;
            thread_args[i].end33 = (i+1) * data_each_thread + data_last_thread;
        }
        else{
            thread_args[i].start33 = i * data_each_thread;
            thread_args[i].end33 = (i+1) * data_each_thread;
        }
        thread_args[i].cips = (uint8_t *)malloc(EXP*32*sizeof(uint8_t));
        thread_args[i].fcips = (uint8_t *)malloc(EXP*32*sizeof(uint8_t));
        memcpy(thread_args[i].cips, cips, EXP*32*sizeof(uint8_t));
        memcpy(thread_args[i].fcips, fcips, EXP*32*sizeof(uint8_t));
        
        thread_args[i].grp = grp;
    }
    for(int i=0; i < THREAD; i++){
        pthread_create(thread_ids + i, NULL, checkLastTwoPattern, (void*) (thread_args + i));
    }
    for(int i=0; i < THREAD; i++){
        pthread_join(thread_ids[i], NULL);
        for(auto j: thread_args[i].keys33){
            keys33.push_back(j);
        }
        for(auto j: thread_args[i].keys34){
            keys34.push_back(j);
        }
    }
}
vector<vector<uint8_t>> find3433Key(uint8_t *cips, uint8_t *fcips){
    vector<vector<uint16_t>> keys33;
    vector<vector<uint16_t>> keys34;
    for(int grp=0; grp<8; grp++){
        vector<uint16_t> tkeys33;
        vector<uint16_t> tkeys34;
        for_each_group(tkeys33, tkeys34, cips, fcips, grp);
        printf("Remaining %d => %ld %f\n", grp, tkeys33.size(), tkeys34.size(), log(tkeys33.size())/log(2));

        vector<uint16_t> keys33_0;
        vector<uint16_t> keys34_0;
        check_1_grp(tkeys33, tkeys34, grp, keys33_0, keys34_0);
        printf("Remaining %d => %ld %f\n", grp, keys33_0.size(), keys34_0.size(), log(keys33_0.size())/log(2));
        keys33.push_back(keys33_0);
        keys34.push_back(keys34_0);
    }
    //Check Two/Four groups
    vector<vector<uint64_t>> keys33_4grp; 
    vector<vector<uint64_t>> keys34_4grp;

    uint8_t grp2 = 0;
    uint8_t grp4 = 0;
    for(int i=0; i<8; i=i+4){
        int g1 = i;
        int g2 = i+1;
        int g3 = i+2;
        int g4 = i+3;

        vector<uint32_t> key33_01; 
        vector<uint32_t> key34_01; 
        check_2_grp(keys33[g1], keys34[g1], keys33[g2], keys34[g2], g1, g2, key33_01, key34_01);
        printf("Remaining %d%d => %ld %ld %f\n",g1,g2, key33_01.size(), key34_01.size(), log(key34_01.size())/log(2));
        keys33[g1].clear();
        keys33[g2].clear();
        keys34[g1].clear();
        keys34[g2].clear();
        
        vector<uint32_t> vkey33_01; 
        vector<uint32_t> vkey34_01; 
        check_2_grp(cips, fcips, key33_01, key34_01, vkey33_01, vkey34_01, grp2); grp2++;
        printf("Remaining %d%d => %ld %ld %f\n",g1,g2, vkey33_01.size(), vkey34_01.size(), log(vkey34_01.size())/log(2));

        vector<uint32_t> key33_23; 
        vector<uint32_t> key34_23; 
        check_2_grp(keys33[g3], keys34[g3], keys33[g4], keys34[g4], g3, g4, key33_23, key34_23);
        printf("Remaining %d%d => %ld %ld %f\n",g3,g4, key33_23.size(), key34_23.size(), log(key34_23.size())/log(2));
        keys33[g3].clear();
        keys33[g4].clear();
        keys34[g3].clear();
        keys34[g4].clear();

        vector<uint32_t> vkey33_23;
        vector<uint32_t> vkey34_23;
        check_2_grp(cips, fcips, key33_23, key34_23, vkey33_23, vkey34_23, grp2); grp2++;
        printf("Remaining %d%d => %ld %ld %f\n",g3,g4, vkey33_23.size(), vkey34_23.size(), log(vkey34_23.size())/log(2));
        
        vector<uint64_t> key33_0123; 
        vector<uint64_t> key34_0123;
        check_4_grp(vkey33_01, vkey34_01, vkey33_23, vkey34_23, g1, g2, g3, g4, key33_0123, key34_0123);
        printf("Remaining %d%d%d%d => %ld %ld %f\n",g1, g2, g3, g4, key33_0123.size(), key34_0123.size(), log(key34_0123.size())/log(2));

        vector<uint64_t> vkey33_0123; 
        vector<uint64_t> vkey34_0123;
        check_4_grp(cips, fcips, key33_0123, key34_0123, vkey33_0123, vkey34_0123, grp4); grp4++;
        printf("Remaining %d%d%d%d => %ld %ld %f\n",g1, g2, g3, g4, vkey33_0123.size(), vkey34_0123.size(), log(vkey34_0123.size())/log(2));
        key33_0123.clear();
        key34_0123.clear();

        keys33_4grp.push_back(vkey33_0123);
        keys34_4grp.push_back(vkey34_0123);

    }
    vector<vector<uint8_t>> full_key33;
    vector<vector<uint8_t>> full_key34;
    check_full_key(keys33_4grp[0], keys34_4grp[0], keys33_4grp[1], keys34_4grp[1], full_key33, full_key34);
    printf("Remaining => %ld %ld %f\n",full_key33.size(), full_key34.size(), log(full_key34.size())/log(2));
    return full_key34;
}


