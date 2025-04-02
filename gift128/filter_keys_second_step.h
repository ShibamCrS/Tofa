int check_alternating_bit_activeness_4(uint8_t *diff, int x){
    uint64_t p1 = 0x55555555AAAAAAAAUL;
    uint64_t p2 = 0xAAAAAAAA55555555UL;

    uint64_t diff1 = 0x0000000000000000UL;
    for(int i=0; i<16; i++){
        diff1 = (diff1 << 4) | diff[(16*x) + i];
    }
    if( ( (diff1 | p1) != p1 )  && ( (diff1 | p2) != p2 ) ){
            return 0;
    }
    return 1;
}

void get_diff_4(uint8_t *a, uint8_t *b, uint8_t *c, int x){
    memset(c, 0x00, 32);
    for (int i=0; i<16; i++){
        c[(16*x) + i] = a[(16*x) + i] ^ b[(16*x) + i];
    }
}

int check_4_grp(uint8_t *cips, uint8_t *fcips, uint8_t *key38, uint8_t *key39, int grp){
    uint8_t dip[32];
    uint8_t fdip[32];

    uint8_t diff[32];
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

        get_diff_4(dip,fdip,diff, grp);
        /* printreg(diff, 32); */

        //first filter
        if  (check_alternating_bit_activeness_4(diff, grp) == 1 ){
            /* printf("filter 1 => \n"); */
                continue;
        }
        else{
            flag = 0;
            break;
        }
    }
    return flag;
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
int check_35(uint8_t *diff){
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


int check_8_grp(uint8_t *cips, uint8_t *fcips, uint8_t *key38, uint8_t *key39){
    uint8_t dip[32];
    uint8_t fdip[32];

    uint8_t diff[32];
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
        
        get_diff_8(dip,fdip,diff);
        /* printreg(diff, 32); */

        //first filter
        if  (check_alternating_bit_activeness_8(diff) == 1 ){
            /* printf("filter 1 => \n"); */
            /* continue; */

            uint8_t key39_copy[32];
            memcpy(key39_copy, key39, 32);
            PLayer(key39_copy);
            uint8_t key36[32];
            uint8_t key37[32];
            new_inv_key_schedule(key39_copy, key38, key37, key36);
            /* printreg(diff, 32); */
            
            //37th round inverse
            addRk1(dip, key37);
            addRk1(fdip, key37);
            addRc(dip, 37);
            addRc(fdip, 37);
            invPLayer(dip);
            invPLayer(fdip);
            invSBox(dip);
            invSBox(fdip);
            get_diff_8(dip,fdip,diff);
            /* printreg(diff, 32); */
            if(check_37(diff)){
                /* continue; */
                //36th round inverse
                addRk1(dip, key36);
                addRk1(fdip, key36);
                addRc(dip, 36);
                addRc(fdip, 36);
                invPLayer(dip);
                invPLayer(fdip);
                invSBox(dip);
                invSBox(fdip);
                get_diff_8(dip,fdip,diff);
                if(check_36(diff)){
                    #ifdef BIT_FAULT
                        uint8_t key35[32];
                        new_inv_key_schedule(key38, key37, key36, key35);
                        //36th round inverse
                        addRk1(dip, key35);
                        addRk1(fdip, key35);
                        addRc(dip, 35);
                        addRc(fdip, 35);
                        invPLayer(dip);
                        invPLayer(fdip);
                        invSBox(dip);
                        invSBox(fdip);
                        get_diff_8(dip,fdip,diff);
                        if(check_35(diff) == 1){
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
        else{
            flag = 0;
            break;
        }
    }
    return flag;
}

