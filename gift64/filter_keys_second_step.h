int check_alternating_bit_activeness_4(int64_t diff){
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
int check_4_grp(uint64_t *cips, uint64_t *fcips, uint64_t key26, uint64_t key27){
    uint64_t dip;
    uint64_t fdip;
    uint64_t diff;

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

        uint64_t diff = dip ^ fdip;

        //first filter
        if  (check_alternating_bit_activeness_4(diff) == 1 ){
            continue;
        }
        else{
            flag = 0;
            break;
        }
    }
    /* cout << "Flag "<< flag << endl; */
    return flag;
}
int check_alternating_bit_activeness_4_25(uint64_t diff){
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
int check_4_grp_25(uint64_t *cips, uint64_t *fcips, uint64_t key25, uint64_t key26, uint64_t key27){
    uint64_t dip;
    uint64_t fdip;
    uint64_t diff;

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

        uint64_t diff = dip ^ fdip;

        //first filter
        if  (check_alternating_bit_activeness_4_25(diff) == 1 ){
            continue;
        }
        else{
            flag = 0;
            break;
        }
    }
    /* cout << "Flag "<< flag << endl; */
    return flag;
}
