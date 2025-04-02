void print_bits(uint8_t *state){
  printf(" ");
  for(int i = 0; i < 128; i++){
      printf("%d", state[127 - i]);
      if(i%4 == 3){
        printf(" ");
      }
  }
  printf("\n");
}
void from_bits(uint8_t *A, vector<uint8_t> &B){
    //convert bit-wise variables into nibble-wise variables
    for(int i = 0; i < 32; i++){
        //0 is LSB and 3 is MSB in each nibble
        B[i]  = (A[(4 * i)]         );
        B[i] ^= (A[(4 * i) + 1] << 1);
        B[i] ^= (A[(4 * i) + 2] << 2);
        B[i] ^= (A[(4 * i) + 3] << 3);
    }
}
int check_key(uint8_t *A, uint8_t *B){
    int filter_size = 128;
    for(int i=0; i<128; i++){
        if( (A[i] == 2) || (B[i] == 2) ){
            filter_size -= 1;
            continue;
        }  
        else{
            if(A[i] != B[i]){
                return 0;
            }
        }
    }
    /* cout << "Filter Size: "<< filter_size << endl; */
    return 1;
}
void get_Bits(uint16_t a, int grp, uint8_t *key){
    int q = 8; 
    /* printf("%d 0x%08X \n", grp1, a0); */
    /* printf("%d 0x%08X \n", grp2, b0); */
    for(int i=0; i<128; i++){
        key[i] = 2; //2 => unknown
    }

    for(int i=0; i<4; i++){
        uint8_t na = (a >> (4*i)) & 0xF;
        for(int j=0; j<4; j++){
            key[4*( (q*i) + grp) + j] = (na >> j) & 1;
        }    
    }
}
void check_1_grp(vector<uint16_t> &keys33, vector<uint16_t> &keys34,
                 int grp, vector<uint16_t> &valid_keys33, vector<uint16_t> &valid_keys34){
    for(int i=0; i<keys33.size(); i++){
        uint16_t a1 = keys33[i];
        uint16_t a0 = keys34[i];

        uint8_t key33[128];
        uint8_t key34[128];
        uint8_t key33_rotate[128];
        uint8_t key34_permuted[128];

        get_Bits(a1, grp, key33);
        get_Bits(a0, grp, key34);
        //rotate key33
        for(int i = 0; i < 127; i++){
            key33_rotate[i] = key33[i + 1];
        }
        key33_rotate[127] = key33[0];

        //permute key34
        for(int i = 0; i < 128; i++){
            key34_permuted[PermBits[i]] = key34[i];
        }
        /* print_bits(key33_rotate); */
        /* print_bits(key34_permuted); */

        if(check_key(key33_rotate, key34_permuted)){
            /* cout << "valid "<<endl; */
            /* print_bits(key33_rotate); */
            /* print_bits(key34_permuted); */
            valid_keys33.push_back(a1);
            valid_keys34.push_back(a0);
        }
    }
}
void get_Bits(uint32_t k1, uint32_t k0, int grp1, int grp2, uint8_t *key){
    int q = 8; 
    /* printf("%d 0x%08X \n", grp1, a0); */
    /* printf("%d 0x%08X \n", grp2, b0); */
    for(int i=0; i<128; i++){
        key[i] = 2; //2 => unknown
    }

    for(int i=0; i<4; i++){
        uint8_t na = (k0 >> (4*i)) & 0xF;
        uint8_t nb = (k1 >> (4*i)) & 0xF;
        for(int j=0; j<4; j++){
            key[4*( (q*i) + grp1 ) + j] = (na >> j) & 1;
            key[4*( (q*i) + grp2 ) + j] = (nb >> j) & 1;
        }    
    }
}
struct Args2{
    vector<uint16_t> keys33_0;
    vector<uint16_t> keys34_0;
    vector<uint16_t> keys33_1;
    vector<uint16_t> keys34_1;
    uint64_t start;
    uint64_t end;
    int grp1;
    int grp2;
    vector<uint32_t> keys33_01;
    vector<uint32_t> keys34_01;
};
void *check_2_grp(void *args1){
    Args2 *args = (Args2*) args1;
    for(uint64_t a=args->start; a<args->end; a++){
        /* cout << a << endl; */
        uint32_t k33_0 = args->keys33_0[a];
        uint32_t k34_0 = args->keys34_0[a];
        for(int b=0; b<args->keys33_1.size(); b++){
            uint32_t k33_1 = args->keys33_1[b];
            uint32_t k34_1 = args->keys34_1[b];

            uint8_t key33[128];
            uint8_t key34[128];
            uint8_t key33_rotate[128];
            uint8_t key34_permuted[128];

            get_Bits(k33_1, k33_0, args->grp1, args->grp2, key33);
            get_Bits(k34_1, k34_0, args->grp1, args->grp2, key34);
            //rotate key33
            for(int i = 0; i < 127; i++){
                key33_rotate[i] = key33[i + 1];
            }
            key33_rotate[127] = key33[0];

            //permute key34
            for(int i = 0; i < 128; i++){
                key34_permuted[PermBits[i]] = key34[i];
            }
            /* print_bits(key34_permuted); */
            /* print_bits(key33_rotate); */

            if(check_key(key33_rotate, key34_permuted)){
                /* cout << "valid "<<endl; */
                /* print_bits(key33_rotate); */
                /* print_bits(key34_permuted); */
                uint32_t k33 = (k33_1 << 16) | k33_0;
                uint32_t k34 = (k34_1 << 16) | k34_0;
                args->keys33_01.push_back(k33);
                args->keys34_01.push_back(k34);
                /* print_vector(a33); */
                /* print_vector(a34); */
            }
        }
    }
    pthread_exit(NULL);
}
void check_2_grp(vector<uint16_t> &keys33_0, vector<uint16_t> &keys34_0,
                 vector<uint16_t> &keys33_1, vector<uint16_t> &keys34_1,
                 int grp1, int grp2,
                 vector<uint32_t> &keys33_01, vector<uint32_t> &keys34_01){

    uint64_t sizeEachGroup = keys33_0.size();
    pthread_t thread_ids[THREAD];
    Args2 thread_args[THREAD];

    uint64_t data_each_thread = sizeEachGroup / THREAD;
    uint64_t data_last_thread = sizeEachGroup % THREAD;

    /* printf("%d %f \n", data_each_thread, log(data_each_thread)/log(2)); */
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
        thread_args[i].keys33_0 = keys33_0;
        thread_args[i].keys34_0 = keys34_0;
        thread_args[i].keys33_1 = keys33_1;
        thread_args[i].keys34_1 = keys34_1;
        thread_args[i].grp1 = grp1;
        thread_args[i].grp2 = grp2;
    }
    for(int i=0; i < THREAD; i++){
        pthread_create(thread_ids + i, NULL, check_2_grp, (void*) (thread_args + i));
    }
    for(int i=0; i < THREAD; i++){
        pthread_join(thread_ids[i], NULL);
        for(auto j: thread_args[i].keys33_01){
            keys33_01.push_back(j);
        }
        for(auto j: thread_args[i].keys34_01){
            keys34_01.push_back(j);
        }
    }
}
void get_Bits(uint32_t k01, uint32_t k23, int grp0, int grp1, int grp2, int grp3, uint8_t *key){
    int q = 8;
    for(int i=0; i<128; i++){
        key[i] = 2; //2 => unknown
    }

    for(int i=0; i<4; i++){
        uint8_t k0 = (k01 >> (4*i)) & 0xF;
        uint8_t k1 = ( k01 >> (16 + (4*i)) ) & 0xF;
        uint8_t k2 = (k23 >> (4*i)) & 0xF;
        uint8_t k3 = (k23 >> (16 + (4*i)) ) & 0xF;
        for(int j=0; j<4; j++){
            key[4*( (q*i) + grp0 ) + j] = (k0 >> j) & 1;
            key[4*( (q*i) + grp1 ) + j] = (k1 >> j) & 1;
            key[4*( (q*i) + grp2 ) + j] = (k2 >> j) & 1;
            key[4*( (q*i) + grp3 ) + j] = (k3 >> j) & 1;
        }
    }
}
struct Args4{
    vector<uint32_t> keys33_01;
    vector<uint32_t> keys34_01;
    vector<uint32_t> keys33_23;
    vector<uint32_t> keys34_23;
    uint64_t start;
    uint64_t end;
    int grp1;
    int grp2;
    int grp3;
    int grp4;
    vector<uint64_t> keys33_0123;
    vector<uint64_t> keys34_0123;
};
void *check_4_grp(void *args1){
    Args4 *args = (Args4 *) args1;
    for(uint64_t a=args->start; a<args->end; a++){
        /* cout << a << endl; */
        uint32_t k33_01 = args->keys33_01[a];
        uint32_t k34_01 = args->keys34_01[a];
        for(int b=0; b<args->keys33_23.size(); b++){
            uint32_t k33_23 = args->keys33_23[b];
            uint32_t k34_23 = args->keys34_23[b];
            
            uint8_t key33[128];
            uint8_t key34[128];
            uint8_t key33_rotate[128];
            uint8_t key34_permuted[128];
            
            get_Bits(k33_01, k33_23, args->grp1, args->grp2, args->grp3, args->grp4, key33);
            get_Bits(k34_01, k34_23, args->grp1, args->grp2, args->grp3, args->grp4, key34);
            
            /* print_bits(key33); */
            /* print_bits(key34); */
            //rotate key33
            for(int i = 0; i < 127; i++){
                key33_rotate[i] = key33[i + 1];
            }
            key33_rotate[127] = key33[0];
            /* print_bits(key33_rotate); */

            //permute key34
            for(int i = 0; i < 128; i++){
                key34_permuted[PermBits[i]] = key34[i];
            }
            /* print_bits(key34_permuted); */
            if(check_key(key33_rotate, key34_permuted)){
                /* cout << "valid "<<endl; */
                /* print_bits(key33_rotate); */
                /* print_bits(key34_permuted); */
                uint64_t k33 = k33_23 & 0xFFFFFFFFFFFFFFFF;
                k33 = (k33 << 32) | k33_01;
                uint64_t k34 = k34_23 & 0xFFFFFFFFFFFFFFFF;
                k34 = (k34 << 32) | k34_01;
                
                args->keys33_0123.push_back(k33);
                args->keys34_0123.push_back(k34);
                /* print_vector(a33); */
                /* print_vector(a34); */
            }
        }
    }
    pthread_exit(NULL);
}

void check_4_grp(vector<uint32_t> &keys33_01, vector<uint32_t> &keys34_01, 
                 vector<uint32_t> &keys33_23, vector<uint32_t> &keys34_23, 
                 int grp1, int grp2, int grp3, int grp4,
            vector<uint64_t> &keys33_0123, vector<uint64_t> &keys34_0123){

    uint64_t sizeEachGroup = keys33_01.size();
    pthread_t thread_ids[THREAD];
    Args4 thread_args[THREAD];

    uint64_t data_each_thread = sizeEachGroup / THREAD;
    uint64_t data_last_thread = sizeEachGroup % THREAD;
    /* printf("%d %f \n", data_each_thread, log(data_each_thread)/log(2)); */
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
        thread_args[i].keys33_01 = keys33_01;
        thread_args[i].keys34_01 = keys34_01;
        thread_args[i].keys33_23 = keys33_23;
        thread_args[i].keys34_23 = keys34_23;
        
        thread_args[i].grp1 = grp1;
        thread_args[i].grp2 = grp2;
        thread_args[i].grp3 = grp3;
        thread_args[i].grp4 = grp4;
        
        thread_args[i].keys33_0123 = keys33_0123;
        thread_args[i].keys34_0123 = keys34_0123;
    }
    for(int i=0; i < THREAD; i++){
        pthread_create(thread_ids + i, NULL, check_4_grp, (void*) (thread_args + i));
    }
    for(int i=0; i < THREAD; i++){
        pthread_join(thread_ids[i], NULL);
        for(auto j: thread_args[i].keys33_0123){
            keys33_0123.push_back(j);
        }
        for(auto j: thread_args[i].keys34_0123){
            keys34_0123.push_back(j);
        }
    }
}
void get_Bits(uint64_t k0123, uint64_t k4567, uint8_t *key){
    int q = 8;
    for(int i=0; i<128; i++){
        key[i] = 2; //2 => unknown
    }

    for(int i=0; i<4; i++){
        uint8_t k0 = (k0123 >> (4*i)) & 0xF;
        uint8_t k1 = (k0123 >> (16 + (4*i)) ) & 0xF;
        uint8_t k2 = (k0123 >> (32 + (4*i)) ) & 0xF;
        uint8_t k3 = (k0123 >> (48 + (4*i)) ) & 0xF;

        uint8_t k4 = (k4567 >> (4*i)) & 0xF;
        uint8_t k5 = (k4567 >> (16 + (4*i)) ) & 0xF;
        uint8_t k6 = (k4567 >> (32 + (4*i)) ) & 0xF;
        uint8_t k7 = (k4567 >> (48 + (4*i)) ) & 0xF;
        
        for(int j=0; j<4; j++){
            key[4*( (q*i) +  0) + j] = (k0 >> j) & 1;
            key[4*( (q*i) +  1) + j] = (k1 >> j) & 1;
            key[4*( (q*i) +  2) + j] = (k2 >> j) & 1;
            key[4*( (q*i) +  3) + j] = (k3 >> j) & 1;
            
            key[4*( (q*i) +  4) + j] = (k4 >> j) & 1;
            key[4*( (q*i) +  5) + j] = (k5 >> j) & 1;
            key[4*( (q*i) +  6) + j] = (k6 >> j) & 1;
            key[4*( (q*i) +  7) + j] = (k7 >> j) & 1;
        }
    }
}
struct Args8{
    vector<uint64_t> keys33_0123;
    vector<uint64_t> keys34_0123;
    vector<uint64_t> keys33_4567;
    vector<uint64_t> keys34_4567;
    uint64_t start;
    uint64_t end;
    vector<vector<uint8_t>> keys33;
    vector<vector<uint8_t>> keys34;
};
void *check_full_key(void *args1){
    Args8 *args = (Args8 *) args1;
    for(uint64_t a=args->start; a<args->end; a++){
        /* cout << a << endl; */
        uint64_t k33_0123 = args->keys33_0123[a];
        uint64_t k34_0123 = args->keys34_0123[a];
        for(int b=0; b<args->keys33_4567.size(); b++){
            uint64_t k33_4567 = args->keys33_4567[b];
            uint64_t k34_4567 = args->keys34_4567[b];

            uint8_t key33[128];
            uint8_t key34[128];
            uint8_t key33_rotate[128];
            uint8_t key34_permuted[128];
            

            get_Bits(k33_0123, k33_4567, key33);
            get_Bits(k34_0123, k34_4567, key34);
            
            /* print_bits(key33); */
            /* print_bits(key34); */
            //rotate key33
            for(int i = 0; i < 127; i++){
                key33_rotate[i] = key33[i + 1];
            }
            key33_rotate[127] = key33[0];

            //permute key34
            for(int i = 0; i < 128; i++){
                key34_permuted[PermBits[i]] = key34[i];
            }
            /* print_bits(key33_rotate); */
            /* print_bits(key34_permuted); */
            if(check_key(key33_rotate, key34_permuted)){
                for(int j=0; j<128; j++){
                    if(key33[j] == 2){
                        key33[j] = 0;
                    }
                    if(key34[j] == 2){
                        key34[j] = 0;
                    }
                }
                vector<uint8_t> a33(32);
                vector<uint8_t> a34(32);
                from_bits(key33, a33);
                from_bits(key34, a34);
                args->keys33.push_back(a33);
                args->keys34.push_back(a34);
            }
        }
    }
    pthread_exit(NULL);
}
void check_full_key(vector<uint64_t> &keys33_0123, vector<uint64_t> &keys34_0123, 
                    vector<uint64_t> &keys33_4567, vector<uint64_t> &keys34_4567, 
                    vector<vector<uint8_t>> &keys33, vector<vector<uint8_t>> &keys34){

    uint64_t sizeEachGroup = keys33_0123.size();
    pthread_t thread_ids[THREAD];
    Args8 thread_args[THREAD];

    uint64_t data_each_thread = sizeEachGroup / THREAD;
    uint64_t data_last_thread = sizeEachGroup % THREAD;
    /* printf("%d %f \n", data_each_thread, log(data_each_thread)/log(2)); */
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
        thread_args[i].keys33_0123 = keys33_0123;
        thread_args[i].keys34_0123 = keys34_0123;
        thread_args[i].keys33_4567 = keys33_4567;
        thread_args[i].keys34_4567 = keys34_4567;
        
        thread_args[i].keys33 = keys33;
        thread_args[i].keys34 = keys34;
    }
    for(int i=0; i < THREAD; i++){
        pthread_create(thread_ids + i, NULL, check_full_key, (void*) (thread_args + i));
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
