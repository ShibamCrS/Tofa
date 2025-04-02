#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include <iostream>
#include <vector>

using namespace std;
#include "utility.h"
#include "baksheesh.h"

int EXP = 16;
#define ROUND 35
#define FROUND 30

/* #define CYTHON */

#if NOCYTHON == 1
#define PRINT
#endif

void addRk1(uint8_t *state, uint8_t *key){
    for (int i=0; i<32; i++){
        state[i] = state[i] ^ key[i];
    }
}
#include "cw_data.h"
#include "check_31_32.h"
#include "filter_keys_second_step.h"
#include "key_relations_pack.h"
#include "filter_keys_pack.h"


void get_master_key(uint8_t *k34, uint8_t *mk){
    for(int j=0; j<32; j++){
        mk[j] = k34[j];
    }
    for(int i=0; i<ROUND; i++){
        Key_update_dec(mk);
    }
}
int attack(uint8_t cips[], uint8_t fcips[], uint8_t key[], int noEXP){
    struct timespec start, end;
    long long time_meter, start_ms, end_ms;

    EXP = noEXP;
    vector<vector<uint8_t> > keys34;

    clock_gettime(CLOCK_MONOTONIC, &start);
    keys34 = find3433Key(cips, fcips);
    clock_gettime(CLOCK_MONOTONIC, &end);
    /* time_meter = ((double)(end.tv_nsec - start.tv_nsec)); */
    start_ms = (start.tv_sec * 1000LL) + (start.tv_nsec / 1000000LL);
    end_ms   = (end.tv_sec * 1000LL)   + (end.tv_nsec   / 1000000LL);
    time_meter = end_ms - start_ms;
    
    int success = 0;
    uint64_t remKeys = -1;
    for(auto k: keys34){
        uint8_t k34[32];
        for(int j=0; j<32; j++){
            k34[j] = k[j];
        }
        PLayer(k34);
        uint8_t k0[32];
        get_master_key(k34, k0);
        if(memcmp(k0, key, 32) == 0) {
            success = 1;
#ifdef PRINT
            printf("Match \n");
            cout << "Original Master Key: ";
            printreg(key, 32);
            cout << "Found Master Key:    ";
            printreg(k0, 32);
#endif
        }

    }
#ifdef PRINT
cout << "\n Number of faults " << EXP << " Number of threads " <<
        THREAD << " Remaining Keys " << keys34.size() << endl;
        printf("Elapsed time : %lld mss %lld s\n", time_meter, time_meter / 1000LL);
#endif
    //if the original key is in the sugested key list, we say a success. In that case we return
    //the number of remaining keys. Otherwise, we return -1
    if(success == 1) {
        remKeys = keys34.size();
    }
    else {
        remKeys = -1;
    }
    return remKeys;
}


/**********************************ONLY FOR TESTING*************************************/
void oracle(uint8_t *key, uint8_t *cips, uint8_t *fcips, int noEXP){
    srand(time(NULL));
    EXP = noEXP;
    uint8_t msg1[32];
    uint8_t msg2[32];
    /* printf("EXP = %d\n", EXP); */
    for(int i=0; i<EXP; i++){
        generate_random_state(msg1);
        memcpy(msg2, msg1, 32);
        /* printreg(msg1, 32); */
        //both fault and fault free
        byte_fault_simulation(ROUND, msg1, msg2, key, FROUND);
        memcpy(cips + (i*32), msg1, 32);
        memcpy(fcips + (i*32), msg2, 32);
    }
}
void testAttack(){
    int noEXP = 10;
    EXP = noEXP;
    uint8_t cips[EXP*32], fcips[EXP*32];
    uint8_t key[32] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF, 0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};
    generate_random_state(key);

    printreg(key, 32);
    oracle(key, cips, fcips, noEXP);
    
    
    printf("-------------------Master Key-------------------------------\n");
    printreg(key, 32);
    printf("------------------------------------------------------------\n");
    /* int rem = attack(cips_r, fcips_r, key, noEXP); */
    int rem = attack(cips, fcips, key, noEXP);
    cout << "\n Number of faults " << EXP << " Number of threads " << 
        THREAD << " Remaining Keys " << rem << endl;
}
/***************************************************************************************/

#if NOCYTHON == 1
int main(){
    srand(time(NULL));
    xx_initialize(SEED);
    /* printf("SEED: "); */
    /* printreg(SEED, 32); */
    printf("\n\n");
    testAttack();
    return 0;
}
#endif


