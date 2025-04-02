//3||2||1||0
uint64_t combine_key(uint32_t key1){
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
struct Args2{
    uint64_t start;
    uint64_t end;

    uint64_t *cips;
    uint64_t *fcips;

    vector<uint16_t> keys26_01;
    vector<uint16_t> keys27_01;
    vector<uint16_t> keys26_23;
    vector<uint16_t> keys27_23;

    vector<uint64_t> keys26_0123;
    vector<uint64_t> keys27_0123;
};
void *combine_full_key(void *args1){
    Args2 *args = (Args2*) args1;
    uint64_t total = 0;
    for(uint64_t i=args->start; i<args->end; i++) {  
        uint32_t temp26_01 = args->keys26_01[i];
        uint32_t temp27_01 = args->keys27_01[i];
        for(int j=0; j<args->keys26_23.size(); j++){
            uint32_t temp26_23 = args->keys26_23[j];
            uint32_t temp27_23 = args->keys27_23[j];

            uint32_t key26 = (temp26_23 << 16) | temp26_01; //3||2||1||0
            uint32_t key27 = (temp27_23 << 16) | temp27_01;


            uint64_t temp26 = combine_key(key26);
            uint64_t temp27 = combine_key(key27);
            /* printreg(temp26, 32); */
            /* printreg(temp27, 32); */
            if (check_4_grp(args->cips, args->fcips, temp26, temp27)){
                args->keys26_0123.push_back(temp26);
                args->keys27_0123.push_back(temp27);
            }
        }
    }
    pthread_exit(NULL);
}
void combine_full_key_threaded(uint64_t *cips, uint64_t *fcips, 
                    vector<uint16_t> &keys26_01, vector<uint16_t> &keys27_01,
                    vector<uint16_t> &keys26_23, vector<uint16_t> &keys27_23,
                    vector<uint64_t> &keys26_0123, vector<uint64_t> &keys27_0123){
    uint64_t size = keys26_01.size();
    pthread_t thread_ids[THREAD];
    Args2 thread_args[THREAD];
    
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
        thread_args[i].keys26_01 = keys26_01;
        thread_args[i].keys27_01 = keys27_01;
        thread_args[i].keys26_23 = keys26_23;
        thread_args[i].keys27_23 = keys27_23;
        
        thread_args[i].cips = cips;
        thread_args[i].fcips = fcips;
    }
    for(int i=0; i < THREAD; i++){
        pthread_create(thread_ids + i, NULL, combine_full_key, (void*) (thread_args + i));
    }
    for(int i=0; i < THREAD; i++){
        pthread_join(thread_ids[i], NULL);
        for(auto j: thread_args[i].keys26_0123){
            keys26_0123.push_back(j);
        }
        for(auto j: thread_args[i].keys27_0123){
            keys27_0123.push_back(j);
        }
    }

}
void combine_keys(uint64_t *cips, uint64_t *fcips, 
        vector<vector<uint8_t>> &keys26, vector<vector<uint8_t>> &keys27,
        vector<uint64_t> &vkeys26, vector<uint64_t> &vkeys27){
    vector<uint16_t> keys26_01;
    vector<uint16_t> keys27_01;

    uint16_t key26 = 0x0000;
    uint16_t key27 = 0x0000;
    for(int a0=0; a0<keys26[0].size(); a0++){
        for(int a1=0; a1<keys26[1].size(); a1++){

            key26 = keys26[1][a1];
            key26 = (key26 << 8) | keys26[0][a0];  
            key27 = keys27[1][a1];
            key27 = (key27 << 8) | keys27[0][a0];  
            keys26_01.push_back(key26);
            keys27_01.push_back(key27);

        }
    }
    /* printf("Remaining 01 => %ld %ld %f\n", keys26_01.size(), keys27_01.size(), log(keys26_01.size())/log(2)); */
    vector<uint16_t> keys26_23;
    vector<uint16_t> keys27_23;
    for(int a2=0; a2<keys26[2].size(); a2++){
        for(int a3=0; a3<keys26[3].size(); a3++){

            key26 = keys26[3][a3];
            key26 = (key26 << 8) | keys26[2][a2];  
            key27 = keys27[3][a3];
            key27 = (key27 << 8) | keys27[2][a2];  
            keys26_23.push_back(key26);
            keys27_23.push_back(key27);

        }
    }
    /* printf("Remaining 23 => %ld %ld %f\n", keys26_23.size(), keys27_23.size(), log(keys26_23.size())/log(2)); */
    combine_full_key_threaded(cips, fcips, keys26_01, keys27_01, keys26_23, keys27_23, vkeys26, vkeys27);
    /* printf("Remaining 0123 => %ld %ld %f\n", vkeys26.size(), vkeys27.size(), log(vkeys26.size())/log(2)); */
}
