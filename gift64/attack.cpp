#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include <iostream>
#include <vector>
#include <random>

using namespace std;

#if NOCYTHON == 1
#define PRINT
#endif

int EXP = 8;
#define ROUNDS 28
#define FROUND 24


#include "utility.h"
#include "gift64.h"

#include "filter_keys_second_step.h"
#include "find_25.h"
#include "combine_keys_advanced.h"
#include "filter_keys_pack.h"


void pop_rks(uint64_t *rks, uint64_t *mkey){
    generate_round_keys(mkey, rks);
    for(int i=0; i<ROUNDS; i++){
        uint64_t key = rks[i];
        uint64_t U,V, U1=0, V1=0;
        V = key&0xFFFF;
        U = (key>>16)&0xFFFF;
        for (int i=0; i<16; i++){
            V1 = (V1 << 4) | ((V >> (15-i)) & 0x01);
            U1 = (U1 << 4) | ((U >> (15-i)) & 0x01);
        }
        U1 = U1 << 1;
        rks[i] = U1 ^ V1;
    }
}

int attack(uint64_t cips[], uint64_t fcips[], uint64_t key[2], int noEXP){
    struct timespec start, end;
    long long time_meter, start_ms, end_ms;

    uint64_t rks[ROUNDS];
    pop_rks(rks, key);

    EXP = noEXP;
    vector<uint64_t> keys25;
    vector<uint64_t> keys26;
    vector<uint64_t> keys27;
    clock_gettime(CLOCK_MONOTONIC, &start);
    uint64_t rem = find2627Key(cips, fcips, keys25, keys26, keys27);
    clock_gettime(CLOCK_MONOTONIC, &end);
    start_ms = (start.tv_sec * 1000LL) + (start.tv_nsec / 1000000LL);
    end_ms   = (end.tv_sec * 1000LL)   + (end.tv_nsec   / 1000000LL);
    /* time_meter = ((double)(end.tv_nsec - start.tv_nsec)); */
    time_meter = end_ms - start_ms;

    int success = 0;
    uint64_t remKeys = -1;
    int flag25 = 0, flag26 = 0, flag27 = 0;
    for(int i=0; i<keys26.size(); i++){
        if(keys26[i] == rks[26]){
            flag26 = 1;
        }
        if(keys27[i] == invPLayer(rks[27])){
            flag27 = 1;
        }
    }
    for(int i=0; i<keys25.size(); i++){
        if(keys25[i] == rks[25]){
            flag25 = 1;
        }
    }
    if((flag25 == 1) && (flag26 == 1) && (flag27 == 1)) {
            success = 1;
#ifdef PRINT
            printf("Match \n");
#endif
    }
#ifdef PRINT
            cout << flag25 <<" " << flag26 << " " << flag27 << endl;
cout << "\n Number of faults " << EXP << " Number of threads " <<
        THREAD << " Remaining Keys " << rem << endl;
        printf("Elapsed time : %lld mss %lld s\n", time_meter, time_meter / 1000LL);
#endif
    //if the original key is in the sugested key list, we say a success. In that case we return
    //the number of remaining keys. Otherwise, we return -1
    if(success == 1) {
        remKeys = rem;
    }
    else {
        remKeys = -1;
    }
    return remKeys;
}


/**********************************ONLY FOR TESTING*************************************/
uint64_t randomMsg64(){
    std::random_device rd; // Seed with a real random value, if available                              
    std::mt19937_64 eng(rd()); // A 64-bit Mersenne Twister random number generator                    
    std::uniform_int_distribution<uint64_t> distr;                                                     
                                                                                                        
    uint64_t random_value = distr(eng); // Generate a random 64-bit integer    

    return random_value;
}

void oracle(uint64_t *key, uint64_t *cips, uint64_t *fcips, int noEXP){
    srand(time(NULL));
    EXP = noEXP;
    uint64_t msg = 0UL;
    for(int i=0; i<EXP; i++){
        /* msg = (uint64_t)rand() & 0xFFFFFFFFUL; */
        /* msg = (msg << 32) & (rand() & 0xFFFFFFFFUL); */
        msg = randomMsg64();
        /* printf("%ld \n", msg); */
        byte_fault_simulation(msg, key, FROUND, cips+i, fcips+i);
    }
}
void testAttack(){
    int noEXP = 32;
    EXP = noEXP;
    uint64_t cips[EXP], fcips[EXP], key[2];
    randTwo64(key);
    printreg(key, 16);
    uint64_t rks[ROUNDS];
    oracle(key, cips, fcips, EXP);
    printreg(key, 16);

    int rem = attack(cips, fcips, key, noEXP);
    cout << "\n Number of faults " << EXP << " Number of threads " << 
        THREAD << " Remaining Keys " << rem << endl;
}
/***************************************************************************************/

void test_enc(){
    uint64_t msg = 0x0123456789ABCDEFUL;
    uint64_t key[2] = {0x0123456789ABCDEFUL, 0x0123456789ABCDEFUL};
    printreg(&msg, 8);
    printreg(&key[0], 8);
    printreg(&key[1], 8);
    uint64_t cip;
    cip = enc_gift64(msg, key);
    printf("-----------------------------------------------\n");
    printreg(&cip, 8);
}
#if NOCYTHON == 1
int main(){
    srand(time(NULL));
    xx_initialize(SEED);
    printf("SEED: ");
    printreg(SEED, 32);
    printf("\n\n");
    testAttack();
    test_enc();
    return 0;
}
#endif


