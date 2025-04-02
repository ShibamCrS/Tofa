void combine_key(uint8_t *k, uint8_t *fk){
    int q = 8;
    //0, 8, 16, 24
    for(int i=0; i<8; i++){
        for(int j=0; j<4; j++){            
            fk[q*j + i] = ( ( (k[i] >> (2*j)) & 0x03 ) << 1);
        }
    }

}
void combine_half_key(uint32_t k, uint8_t *fk, int grp4567 = 0){
    memset(fk, 0x00, 32);
    int q = 8;
    //0, 8, 16, 24
    for(int i=0; i<4; i++){
        uint8_t na = (k  >> (8*i) ) & 0xFF;
        for(int j=0; j<4; j++){
            if(grp4567 == 0){ //0123
                fk[q*j + i] = ( ( (na >> (2*j)) & 0x03 ) << 1);
            }
            else{  //4567
                fk[q*j + i + 4] = ( ( (na >> (2*j)) & 0x03 ) << 1);
            }
        }
    }

}
void get_rkeys(uint64_t K, uint8_t *key, vector<uint8_t> &vkey){
    for(int i=0; i<8; i++){
        for(int j=0; j<4; j++){
            uint8_t nb = ( ( (K >> (8*i)) >> (2*j) ) & 0x03 ) << 1;
            key[8*j + i] = nb;
            vkey[8*j + i] = nb;
        }
    }
}
struct Args{
    uint64_t start;
    uint64_t end;

    uint8_t *cips;
    uint8_t *fcips;

    vector<uint32_t> keys38_0123;
    vector<uint32_t> keys39_0123;
    vector<uint32_t> keys38_4567;
    vector<uint32_t> keys39_4567;

    vector<vector<uint8_t> > keys38;
    vector<vector<uint8_t> > keys39;
};
void *combine_full_key(void *args1){
    Args *args = (Args*) args1;
    uint64_t total = 0;
    for(uint64_t i=args->start; i<args->end; i++) {  
        uint32_t temp38_0123 = args->keys38_0123[i];
        uint32_t temp39_0123 = args->keys39_0123[i];
        for(int j=0; j<args->keys38_4567.size(); j++){
            uint32_t temp38_4567 = args->keys38_4567[j];
            uint32_t temp39_4567 = args->keys39_4567[j];

            uint64_t full38 = 0x0000000000000000UL;
            uint64_t full39 = 0x0000000000000000UL;

            full38 = full38 | temp38_4567;
            full38 = (full38<<32) | temp38_0123;

            full39 = full39 | temp39_4567;
            full39 = (full39<<32) | temp39_0123;
            
            uint8_t key38[32]; 
            uint8_t key39[32]; 
            vector<uint8_t> vkey38(32);
            vector<uint8_t> vkey39(32);
            get_rkeys(full38, key38, vkey38);
            get_rkeys(full39, key39, vkey39);
            
            if(check_8_grp(args->cips, args->fcips, key38, key39)){
                args->keys38.push_back(vkey38);
                args->keys39.push_back(vkey39);
            }
        }
    }
    pthread_exit(NULL);
}
void combine_full_key_threaded(uint8_t *cips, uint8_t *fcips, 
                    vector<uint32_t> &keys38_0123, vector<uint32_t> &keys39_0123,
                    vector<uint32_t> &keys38_4567, vector<uint32_t> &keys39_4567,
                    vector<vector<uint8_t> > &keys38, vector<vector<uint8_t> > &keys39){

    uint64_t size = keys38_0123.size();
    pthread_t thread_ids[THREAD];
    Args thread_args[THREAD];

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
        thread_args[i].keys38_0123 = keys38_0123;
        thread_args[i].keys39_0123 = keys39_0123;
        thread_args[i].keys38_4567 = keys38_4567;
        thread_args[i].keys39_4567 = keys39_4567;
        
        thread_args[i].cips = cips;
        thread_args[i].fcips = fcips;
    }
    for(int i=0; i < THREAD; i++){
        pthread_create(thread_ids + i, NULL, combine_full_key, (void*) (thread_args + i));
    }
    uint64_t total=0;
    for(int i=0; i < THREAD; i++){
        pthread_join(thread_ids[i], NULL);
        for(auto j: thread_args[i].keys38){
            total++;
            keys38.push_back(j);
        }
        for(auto j: thread_args[i].keys39){
            keys39.push_back(j);
        }
    }
#ifdef PRINT    
    printf("Remaining => %ld %f\n", total, log(total)/log(2));
#endif
}
struct Args2{
    uint64_t start;
    uint64_t end;

    uint8_t *cips;
    uint8_t *fcips;

    vector<uint16_t> keys38_01;
    vector<uint16_t> keys39_01;
    vector<uint16_t> keys38_23;
    vector<uint16_t> keys39_23;

    vector<uint32_t> keys38_0123;
    vector<uint32_t> keys39_0123;

    int grp;
};
void *combine_half_key(void *args1){
    Args2 *args = (Args2*) args1;
    uint64_t total = 0;
    for(uint64_t i=args->start; i<args->end; i++) {  
        uint32_t temp38_01 = args->keys38_01[i];
        uint32_t temp39_01 = args->keys39_01[i];
        for(int j=0; j<args->keys38_23.size(); j++){
            uint32_t temp38_23 = args->keys38_23[j];
            uint32_t temp39_23 = args->keys39_23[j];

            uint32_t key38 = (temp38_23 << 16) | temp38_01;
            uint32_t key39 = (temp39_23 << 16) | temp39_01;


            uint8_t temp38[32];
            uint8_t temp39[32];
            combine_half_key(key38, temp38, args->grp);
            combine_half_key(key39, temp39, args->grp);
            /* printreg(temp38, 32); */
            /* printreg(temp39, 32); */
            if (check_4_grp(args->cips, args->fcips, temp38, temp39, args->grp)){
                args->keys38_0123.push_back(key38);
                args->keys39_0123.push_back(key39);
            }
        }
    }
    pthread_exit(NULL);
}
void combine_half_key_threaded(uint8_t *cips, uint8_t *fcips, 
                    vector<uint16_t> &keys38_01, vector<uint16_t> &keys39_01,
                    vector<uint16_t> &keys38_23, vector<uint16_t> &keys39_23,
                    vector<uint32_t> &keys38_0123, vector<uint32_t> &keys39_0123, int grp){
    uint64_t size = keys38_01.size();
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
        thread_args[i].keys38_01 = keys38_01;
        thread_args[i].keys39_01 = keys39_01;
        thread_args[i].keys38_23 = keys38_23;
        thread_args[i].keys39_23 = keys39_23;
        
        thread_args[i].cips = cips;
        thread_args[i].fcips = fcips;
        
        thread_args[i].grp = grp;
    }
    for(int i=0; i < THREAD; i++){
        pthread_create(thread_ids + i, NULL, combine_half_key, (void*) (thread_args + i));
    }
    uint64_t total=0;
    for(int i=0; i < THREAD; i++){
        pthread_join(thread_ids[i], NULL);
        for(auto j: thread_args[i].keys38_0123){
            total++;
            keys38_0123.push_back(j);
        }
        for(auto j: thread_args[i].keys39_0123){
            keys39_0123.push_back(j);
        }
    }

}
void combine_keys(uint8_t *cips, uint8_t *fcips, vector<vector<uint8_t> > &keys38, vector<vector<uint8_t> > &keys39,
                 vector<vector<uint8_t> > &vkeys38, vector<vector<uint8_t> > &vkeys39){
    vector<uint16_t> keys38_01;
    vector<uint16_t> keys39_01;

    uint16_t key38 = 0x0000;
    uint16_t key39 = 0x0000;
    for(int a0=0; a0<keys38[0].size(); a0++){
        for(int a1=0; a1<keys38[1].size(); a1++){

            key38 = keys38[1][a1];
            key38 = (key38 << 8) | keys38[0][a0];  
            key39 = keys39[1][a1];
            key39 = (key39 << 8) | keys39[0][a0];  
            keys38_01.push_back(key38);
            keys39_01.push_back(key39);

        }
    }
#ifdef PRINT    
    printf("Remaining 01 => %ld %ld %f\n", keys38_01.size(), keys39_01.size(), log(keys38_01.size())/log(2));
#endif
    vector<uint16_t> keys38_23;
    vector<uint16_t> keys39_23;
    for(int a2=0; a2<keys38[2].size(); a2++){
        for(int a3=0; a3<keys38[3].size(); a3++){

            key38 = keys38[3][a3];
            key38 = (key38 << 8) | keys38[2][a2];  
            key39 = keys39[3][a3];
            key39 = (key39 << 8) | keys39[2][a2];  
            keys38_23.push_back(key38);
            keys39_23.push_back(key39);

        }
    }
#ifdef PRINT    
    printf("Remaining 23 => %ld %ld %f\n", keys38_23.size(), keys39_23.size(), log(keys38_23.size())/log(2));
#endif
    vector<uint32_t> keys38_0123;
    vector<uint32_t> keys39_0123;
    combine_half_key_threaded(cips, fcips, keys38_01, keys39_01, keys38_23, keys39_23, keys38_0123, keys39_0123, 0);
#ifdef PRINT    
    printf("Remaining 0123 => %ld %ld %f\n", keys38_0123.size(), keys39_0123.size(), log(keys38_0123.size())/log(2));
#endif

    vector<uint16_t> keys38_45;
    vector<uint16_t> keys39_45;
    for(int a4=0; a4<keys38[4].size(); a4++){
        for(int a5=0; a5<keys38[5].size(); a5++){

            key38 = keys38[5][a5];
            key38 = (key38 << 8) | keys38[4][a4];  
            key39 = keys39[5][a5];
            key39 = (key39 << 8) | keys39[4][a4];  
            keys38_45.push_back(key38);
            keys39_45.push_back(key39);
        }
    }
#ifdef PRINT    
    printf("Remaining 45 => %ld %ld %f\n", keys38_45.size(), keys39_45.size(), log(keys38_45.size())/log(2));
#endif
    vector<uint16_t> keys38_67;
    vector<uint16_t> keys39_67;
    for(int a6=0; a6<keys38[6].size(); a6++){
        for(int a7=0; a7<keys38[7].size(); a7++){

            key38 = keys38[7][a7];
            key38 = (key38 << 8) | keys38[6][a6];  
            key39 = keys39[7][a7];
            key39 = (key39 << 8) | keys39[6][a6];  
            keys38_67.push_back(key38);
            keys39_67.push_back(key39);
        }
    }
#ifdef PRINT    
    printf("Remaining 67 => %ld %ld %f\n", keys38_67.size(), keys39_67.size(), log(keys38_67.size())/log(2));
#endif
    vector<uint32_t> keys38_4567;
    vector<uint32_t> keys39_4567;
    combine_half_key_threaded(cips, fcips, keys38_45, keys39_45, keys38_67, keys39_67, keys38_4567, keys39_4567, 1);
#ifdef PRINT    
    printf("Remaining 4567 => %ld %ld %f\n", keys38_4567.size(), keys39_4567.size(), log(keys38_4567.size())/log(2));
#endif
    
    combine_full_key_threaded(cips, fcips, keys38_0123, keys39_0123, keys38_4567, keys39_4567, vkeys38, vkeys39);
}
