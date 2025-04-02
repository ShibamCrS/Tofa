int check_alternating_bit_activeness(uint64_t diff){
    if( ((diff | 0x8888444422221111) == 0x8888444422221111) ||
        ((diff | 0x9999cccc66663333) == 0x9999cccc66663333) ||
        ((diff | 0x1111888844442222) == 0x1111888844442222) ||
        ((diff | 0x33339999cccc6666) == 0x33339999cccc6666) ||

        ((diff | 0x33339999cccc6666) == 0x33339999cccc6666) ||
        ((diff | 0x2222111188884444) == 0x2222111188884444) ||
        ((diff | 0x666633339999cccc) == 0x666633339999cccc) ||
        ((diff | 0x4444222211118888) == 0x4444222211118888)){
        return 1;
    }
    return 0;
}
uint64_t properToRoundKeys(uint64_t proper_key){
    uint64_t U = 0UL ,V = 0UL;
    for(int i=15; i>=0; i--){
        uint64_t nibb = (proper_key >> (4*i)) & 0x3;
        uint64_t nibb0 = nibb & 0x1;
        uint64_t nibb1 = (nibb>>1) & 0x1;

        V = (V<<1) | nibb0;
        U = (U << 1) | nibb1;
    }
    uint64_t round_key = (U << 16) | V;
    return round_key;
}
//0x51 => 01 01 00 01 
uint64_t generateProper(uint64_t key, int grp){
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
void checkLastTwoPattern(uint64_t *cips, uint64_t *fcips, int grp,
                        uint64_t start26, uint64_t end26, uint64_t start27, uint64_t end27,
                        vector<uint8_t> &keys26, vector<uint8_t> &keys27){
    uint64_t key26 = 0UL;
    uint64_t key27 = 0Ul;

    uint64_t dip = 0UL;
    uint64_t fdip = 0UL; 
    

    for(uint64_t key1=start26; key1<end26; key1++) {  //26th round key
        for(uint64_t key2=start27; key2<end27; key2++) { //27th round key
            key26 = generateProper(key1, grp);
            key27 = generateProper(key2, grp);
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

                uint64_t mask = (0x000000000000FFFFUL << (16*grp));
                uint64_t diff = ( (dip & mask) ^ (fdip & mask) );
            
                //first filter
                if  (check_alternating_bit_activeness(diff) == 1 ){
                    continue;
                }
                else{
                    flag = 0;
                    break;
                }
            }
            if(flag == 1){
                /* cout << "found_one" << endl; */
                keys26.push_back(key1);
                keys27.push_back(key2);
            }
        }
    }
    /* printf("Remaining %d => %ld %ld %f\n", grp, keys26.size(), keys27.size(), log(keys26.size())/log(2)); */
    /* cout <<"\n-------------------------------------------------"<<endl; */
}

//test with correct key
void test_key(uint64_t *cips, uint64_t *fcips, uint64_t k26, uint64_t k27){
    printstate(k26);
    printstate(k27);
    for(int grp=0; grp < 4; grp++){
        uint64_t key26 = 0UL;
        uint64_t key27 = 0UL;
        for(int i=0; i<4; i++){
            key26 = (key26 << 2) | ( ( k26 >> (16*(3-i) + 4*grp) ) & 0x3 );
            key27 = (key27 << 2) | ( ( k27 >> (16*(3-i) + 4*grp) ) & 0x3 );
        }
        /* printf("0x%02X ", key26); */
        /* printf("0x%02X \n", key27); */
        vector<uint8_t> keys26;
        vector<uint8_t> keys27;
        checkLastTwoPattern(cips, fcips, grp, key26, key26+1, key27, key27+1, keys26, keys27);
    }
}


uint64_t find2627Key(uint64_t *cips, uint64_t *fcips, 
                vector<uint64_t> &vkeys25, vector<uint64_t> &vkeys26, vector<uint64_t> &vkeys27){
    vector<vector<uint8_t>> keys26;
    vector<vector<uint8_t>> keys27;
    int size = (1 << 8);
    for(int grp=0; grp<4; grp++){
        vector<uint8_t> keys26_0;
        vector<uint8_t> keys27_0;
        checkLastTwoPattern(cips, fcips, grp, 0, size, 0, size, keys26_0, keys27_0);
        keys26.push_back(keys26_0);
        keys27.push_back(keys27_0);
    }
    vector<uint64_t> keys26_t;
    vector<uint64_t> keys27_t;
    /* combine_keys(cips, fcips, keys26, keys27, keys26_t, keys27_t); */
    combine_keys(cips, fcips, keys26, keys27, vkeys26, vkeys27);
    uint64_t total_remaining = 0UL;
    for(int i=0; i<vkeys26.size(); i++){
        uint64_t key26 = vkeys26[i];
        uint64_t key27 = vkeys27[i];
        vector<uint64_t> keys25;
        uint64_t size25 = find25key(cips, fcips, key26, key27, keys25);
        total_remaining = total_remaining + size25 ;
        for(auto a: keys25){
            vkeys25.push_back(a);
        }
    }
    return total_remaining;
    /* printf("Total Remaining => %ld %f\n", total_remaining, log(total_remaining)/log(2)); */
}
