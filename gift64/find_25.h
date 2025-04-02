//3||2||1||0
uint64_t combine_key_25(uint32_t key1){
    uint64_t qr_grp[4][4] = {{0, 4, 8, 12},
                           {1, 5, 9, 13},
                           {2, 6, 10, 14},
                           {3, 7, 11, 15}};

    uint64_t proper_key = 0x0000000000000000UL;
    for(int g=0; g<4; g++){
        uint64_t key = (key1 >> (g*8)) & 0xFFUL;
        for(int i=0; i<4; i++){
            proper_key |= ( ( (key  >> (2*i) ) & 0x0000000000000003UL ) << (qr_grp[g][i]*4) ) ;
        }
    }
    /* printstate(proper_key); */
    return proper_key;
}
struct Args25{
    uint64_t start;
    uint64_t end;

    uint64_t *cips;
    uint64_t *fcips;

    vector<uint16_t> keys25_01;
    vector<uint16_t> keys25_23;

    vector<uint64_t> keys25_0123;

    uint64_t key26;
    uint64_t key27;
};
void *combine_full_key_25(void *args1){
    Args25 *args = (Args25*) args1;
    uint64_t total = 0;
    for(uint64_t i=args->start; i<args->end; i++) {  
        uint32_t temp25_01 = args->keys25_01[i];
        for(int j=0; j<args->keys25_23.size(); j++){
            uint32_t temp25_23 = args->keys25_23[j];
            
            uint32_t key25 = (temp25_23 << 16) | temp25_01; //3||2||1||0

            uint64_t temp25 = combine_key_25(key25);
            /* cout << " -------25--------------" << endl; */
            /* printreg(&temp25, 16); */
            if (check_4_grp_25(args->cips, args->fcips, temp25, args->key26, args->key27)){
                /* cout << " -------25--------------" << endl; */
                /* printreg(&temp25, 16); */
                args->keys25_0123.push_back(temp25);
            }
        }
    }
    pthread_exit(NULL);
}
void combine_full_key_threaded(uint64_t *cips, uint64_t *fcips, 
                    vector<uint16_t> &keys25_01, vector<uint16_t> &keys25_23,
                    vector<uint64_t> &keys25_0123, uint64_t key26, uint64_t key27){
    uint64_t size = keys25_01.size();
    pthread_t thread_ids[THREAD];
    Args25 thread_args[THREAD];
    
    uint64_t data_each_thread = size / THREAD;
    uint64_t data_last_thread = size % THREAD;

    /* printf("%d %f \n",data_each_thread, log(data_each_thread)/log(2)); */
    /* printf("%d %f \n", data_last_thread, log(data_last_thread)/log(2)); */

    for(int i=0; i < THREAD; i++){
        if( i == (THREAD-1) ){
            thread_args[i].start = i * data_each_thread;
            thread_args[i].end = (i+1) * data_each_thread + data_last_thread;
        }
        else{
            thread_args[i].start = i * data_each_thread;
            thread_args[i].end = (i+1) * data_each_thread;
        } 
        thread_args[i].keys25_01 = keys25_01;
        thread_args[i].keys25_23 = keys25_23;
        thread_args[i].key26 = key26;
        thread_args[i].key27 = key27;
        
        thread_args[i].cips = cips;
        thread_args[i].fcips = fcips;
    }
    for(int i=0; i < THREAD; i++){
        pthread_create(thread_ids + i, NULL, combine_full_key_25, (void*) (thread_args + i));
    }
    for(int i=0; i < THREAD; i++){
        pthread_join(thread_ids[i], NULL);
        for(auto j: thread_args[i].keys25_0123){
            keys25_0123.push_back(j);
        }
    }

}
void combine_keys(uint64_t *cips, uint64_t *fcips, 
        vector<vector<uint8_t>> &keys25,  vector<uint64_t> &vkeys25, 
        uint64_t key26, uint64_t key27){
    
    vector<uint16_t> keys25_01;
    uint16_t key25 = 0x0000;
    for(int a0=0; a0<keys25[0].size(); a0++){
        for(int a1=0; a1<keys25[1].size(); a1++){

            key25 = keys25[1][a1];
            key25 = (key25 << 8) | keys25[0][a0];  
            keys25_01.push_back(key25);

        }
    }
    /* printf("Remaining 01 => %ld %ld %f\n", keys25_01.size(), log(keys25_01.size())/log(2)); */
    vector<uint16_t> keys25_23;
    for(int a2=0; a2<keys25[2].size(); a2++){
        for(int a3=0; a3<keys25[3].size(); a3++){

            key25 = keys25[3][a3];
            key25 = (key25 << 8) | keys25[2][a2];  
            keys25_23.push_back(key25);
        }
    }
    /* printf("Remaining 23 => %ld %ld %f\n", keys25_23.size(), log(keys25_23.size())/log(2)); */
    combine_full_key_threaded(cips, fcips, keys25_01, keys25_23, vkeys25, key26, key27);
    /* printf("Remaining 0123 => %ld %ld %f\n", vkeys25.size(), log(vkeys25.size())/log(2)); */
}
int check_alternating_bit_activeness25(uint64_t diff){
    if( ((diff | 0x000b000d000e0007) == 0x000b000d000e0007) ||
        ((diff | 0x0007000b000d000e) == 0x0007000b000d000e) ||
        ((diff | 0x008600430029001c) == 0x008600430029001c) ||
        ((diff | 0x009400c200610038) == 0x009400c200610038) ||

        ((diff | 0x00b000d000e00070) == 0x00b000d000e00070) ||
        ((diff | 0x007000b000d000e0) == 0x007000b000d000e0) ||
        ((diff | 0x08600430029001c0) == 0x08600430029001c0) ||
        ((diff | 0x09400c2006100380) == 0x09400c2006100380) ||

        ((diff | 0x0b000d000e000700) == 0x0b000d000e000700) ||
        ((diff | 0x07000b000d000e00) == 0x07000b000d000e00) ||
        ((diff | 0x8600430029001c00) == 0x8600430029001c00) ||
        ((diff | 0x9400c20061003800) == 0x9400c20061003800) ||

        ((diff | 0xb000d000e0007000) == 0xb000d000e0007000) ||
        ((diff | 0x7000b000d000e000) == 0x7000b000d000e000)){
           return 1;
        }
    return 0;
}
/* int check_24(uint64_t cip, uint64_t fcip){ */
/*     uint64_t P0 = 0b000011111111; */
/*     uint64_t P1 = 0b000111111110; */
/*     uint64_t P2 = 0b001111111100; */
/*     uint64_t P3 = 0b011111111000; */
/*     uint64_t P4 = 0b111111110000; */
/*     for(uint8_t key=0; key < (1<<6); key++){ */
/*         uint64_t x = cip ^ ( (key & 0x03) ^ ( ((key>>2) & 0x03) << 4 ) ^ ( ((key>>4) & 0x03) << 6 ) ); */  
/*         x =( inv_SB_table[x&0x0F]|(inv_SB_table[(x>>4)&0x0F]<<4)|(inv_SB_table[(x>>8) & 0x0F] << 8)); */
        
/*         uint64_t y = fcip ^ ( (key & 0x03) ^ ( ((key>>2) & 0x03) << 4 ) ^ ( ((key>>4) & 0x03) << 6 ) ); */  
/*         y =( inv_SB_table[y&0x0F]|(inv_SB_table[(y>>4)&0x0F]<<4)|(inv_SB_table[(y>>8) & 0x0F] << 8) ); */

/*         uint64_t diff = x ^ y; */
/*         if( ((diff | P0) == P0) || ((diff | P0) == P0) || ((diff | P0) == P0) || ((diff | P0) == P0) ){ */         
/*             continue; */
/*         } */
/*         else{ */
/*             return 0; */
/*         } */

/*     } */
/*     return 1; */

/* } */
//0x51 => 01 01 00 01 
uint64_t generateProper25(uint64_t key, int grp){
    uint64_t qr_grp1[4][4] = {{0, 4, 8, 12},
                           {1, 5, 9, 13},
                           {2, 6, 10, 14},
                           {3, 7, 11, 15}};
    
    uint64_t proper_key = 0x0000000000000000UL;
        
    proper_key |= ( (key        & 0x0000000000000003UL) << (qr_grp1[grp][0]*4) ) ;
    proper_key |= ( ((key >> 2) & 0x0000000000000003UL) << (qr_grp1[grp][1]*4) ) ;
    proper_key |= ( ((key >> 4) & 0x0000000000000003UL) << (qr_grp1[grp][2]*4) ) ;
    proper_key |= ( ((key >> 6) & 0x0000000000000003UL) << (qr_grp1[grp][3]*4) ) ;
    /* printstate(proper_key); */
    return proper_key;
}

/* THIS FUNCTION MAY HAVE SOME BUG */
void checkLastTwoPattern(uint64_t *cips, uint64_t *fcips, int grp,
                        uint64_t start25, uint64_t end25, uint64_t key26, uint64_t key27,
                        vector<uint8_t> &keys25){
    uint64_t key25 = 0UL;
    uint64_t dip = 0UL;
    uint64_t fdip = 0UL; 
    

    for(uint64_t key1=start25; key1<end25; key1++) { //25th round key
        key25 = generateProper25(key1, grp);
        /* cout << "-----------------------------------------"<< endl; */
        
        /* cout << grp <<" :Testing key: "; */
        /* printf("0x%04X 0x%04X \n", key1, key2); */
        /* printkey(key26); */
        /* printkey(key27); */
        /* cout << "Start EXPs ------------------------------------------------\n"; */
        int flag = 1;
    
        for(int j=0; j<EXP; j++){
            dip = cips[j];
            fdip = fcips[j];

            //last round inverse
            //We have P(X) + C + K and we guessed P^-1(K)
            //First compute P(X) + C + K + C  = P(X) + K
            //Then compute P^-1( P(X) + K ) + P^-1(K) = X
            dip = addRc(dip, 27);
            fdip = addRc(fdip, 27);
            dip = invPLayer(dip);
            fdip = invPLayer(fdip);
            dip = dip ^ key27;
            fdip = fdip ^ key27;
            dip = invSBox(dip);
            fdip = invSBox(fdip);

            //second last round inverse
            dip = dip ^ key26;
            fdip = fdip ^ key26;
            dip = addRc(dip, 26);
            fdip = addRc(fdip, 26);
            dip = invPLayer(dip);
            fdip = invPLayer(fdip);
            dip = invSBox(dip);
            fdip = invSBox(fdip);
            
            dip = dip ^ key25;
            fdip = fdip ^ key25;
            dip = addRc(dip, 25);
            fdip = addRc(fdip, 25);
            dip = invPLayer(dip);
            fdip = invPLayer(fdip);
            dip = invSBox(dip);
            fdip = invSBox(fdip);
            
            uint64_t mask = (0x000000000000FFFFUL << (16*grp));
            uint64_t diff = ( (dip & mask) ^ (fdip & mask) );
 
            //first filter
            if  (check_alternating_bit_activeness25(diff) == 1 ){
                continue;
            }
            else{
                flag = 0;
                break;
            }
        }
        if(flag == 1){
            /* cout << "found_one" << endl; */
            keys25.push_back(key1);
        }
    }
    /* printf("Remaining %d => %ld %f\n", grp, keys25.size(), log(keys25.size())/log(2)); */
    /* cout <<"\n-------------------------------------------------"<<endl; */
}
uint64_t reduce_24(uint64_t *cips, uint64_t *fcips, uint64_t key25, uint64_t key26, uint64_t key27){

    uint64_t r24 = 0UL;
    uint64_t dip = 0UL;
    uint64_t fdip = 0UL; 
    

    /* printstate(key25); */
    /* printstate(key26); */
    /* printstate(key27); */
    int flag = 1;
    for(int j=0; j<EXP; j++){
        dip = cips[j];
        fdip = fcips[j];

        //last round inverse
        //We have P(X) + C + K and we guessed P^-1(K)
        //First compute P(X) + C + K + C  = P(X) + K
        //Then compute P^-1( P(X) + K ) + P^-1(K) = X
        dip = addRc(dip, 27);
        fdip = addRc(fdip, 27);
        dip = invPLayer(dip);
        fdip = invPLayer(fdip);
        dip = dip ^ key27;
        fdip = fdip ^ key27;
        dip = invSBox(dip);
        fdip = invSBox(fdip);

        //second last round inverse
        dip = dip ^ key26;
        fdip = fdip ^ key26;
        dip = addRc(dip, 26);
        fdip = addRc(fdip, 26);
        dip = invPLayer(dip);
        fdip = invPLayer(fdip);
        dip = invSBox(dip);
        fdip = invSBox(fdip);
        
        dip = dip ^ key25;
        fdip = fdip ^ key25;
        dip = addRc(dip, 25);
        fdip = addRc(fdip, 25);
        dip = invPLayer(dip);
        fdip = invPLayer(fdip);
        dip = invSBox(dip);
        fdip = invSBox(fdip);
        
        dip = invPLayer(dip);
        fdip = invPLayer(fdip);
        uint64_t diff = dip ^ fdip;
        /* printstate(diff); */
            /* int shift = __builtin_ctzl(diff); */
            /* diff = diff >> shift; */
        int msb =63 -  __builtin_clzl(diff);
        int lsb = __builtin_ctzl(diff);
        /* cout << msb << " " << lsb << endl; */
        msb = msb / 4;
        lsb = lsb / 4;
        /* cout << msb << " " << lsb << endl; */
        if( (msb - lsb) > 1){
            /* cout << "checking " << endl; */
            //checking for lsb
            for(uint64_t i=0; i<4; i++){
                uint64_t key = (i << (4*lsb));
                uint64_t x = inv_SB_table[( (dip ^ key) >> (4*lsb)) & 0xF];
                uint64_t y = inv_SB_table[( (fdip ^ key) >> (4*lsb)) & 0xF];
                if ( (x^y) & 0x01 == 0x01){
                    r24++;
                }
            }
            //checking for msb
            for(uint64_t i=0; i<4; i++){
                uint64_t key = (i << (4*msb));
                uint64_t x = inv_SB_table[( (dip ^ key) >> (4*msb)) & 0xF];
                uint64_t y = inv_SB_table[( (fdip ^ key) >> (4*msb)) & 0xF];
                if ( (x^y) & 0x01 == 0x01){
                    r24++;
                }
            }
        }
    }
    /* cout <<"\n-------------------------------------------------"<<endl; */
    printf("R24 => %ld %f\n", r24, log(r24)/log(2));
    uint64_t size24 = 4294967296UL - r24;
    printf("Remaining 24 => %ld %lf\n", size24, log2(size24));
    return size24;
}
uint64_t find25key(uint64_t *cips, uint64_t *fcips, 
        uint64_t key26, uint64_t key27, vector<uint64_t> &vkeys25){
    
    vector<vector<uint8_t>> keys25;
    int size = (1 << 8);
    for(int grp=0; grp<4; grp++){
        vector<uint8_t> keys25_0;
        checkLastTwoPattern(cips, fcips, grp, 0, size, key26, key27, keys25_0);
        /* printf("%ld \n", keys25_0.size()); */
        keys25.push_back(keys25_0);
    }
    /* printf("25 => %ld %ld %ld %ld\n", keys25[0].size(), keys25[1].size(), keys25[2].size(), keys25[3].size()); */
    combine_keys(cips, fcips, keys25, vkeys25, key26, key27);
    /* printf("Remaining 25 => %ld %f\n", vkeys25.size(), log(vkeys25.size())/log(2)); */
    uint64_t total = 0Ul;
    return vkeys25.size();
}

