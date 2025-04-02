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
#include "gift128.h"

int EXP = 16;
#define ROUND 40
#define FROUND 35

/* #define CYTHON 1 */

#if NOCYTHON == 1
#define PRINT
#endif

void addRk1(uint8_t *state, uint8_t *key){
    for (int i=0; i<32; i++){
        state[i] = state[i] ^ key[i];
    }
}
#include "check_36_37.h"
#include "filter_keys_second_step.h"
#include "combine_keys_advanced.h"
#include "filter_keys_pack.h"



int attack(uint8_t cips[], uint8_t fcips[], uint8_t key[], int noEXP){
    struct timespec start, end;
    long long time_meter, start_ms, end_ms;

    EXP = noEXP;
    vector<vector<uint8_t> > keys38;
    vector<vector<uint8_t> > keys39;

    clock_gettime(CLOCK_MONOTONIC, &start);
    find3839Key(cips, fcips, keys38, keys39);
    clock_gettime(CLOCK_MONOTONIC, &end);
    start_ms = (start.tv_sec * 1000LL) + (start.tv_nsec / 1000000LL);
    end_ms   = (end.tv_sec * 1000LL)   + (end.tv_nsec   / 1000000LL);
    /* time_meter = ((double)(end.tv_nsec - start.tv_nsec)); */
    time_meter = end_ms - start_ms;

    int success = 0;
    uint64_t remKeys = -1;
    for(int i=0; i<keys38.size(); i++){
        uint8_t k38[32];
        uint8_t k39[32];
        for(int j=0; j<32; j++){
            k38[j] = keys38[i][j];
            k39[j] = keys39[i][j];
        }
        PLayer(k39);
        uint8_t k0[32];
        inv_master_key_schedule(k39, k38, k0);
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
        THREAD << " Remaining Keys " << keys38.size() << endl;
        printf("Elapsed time : %lld mss %lld s\n", time_meter, time_meter / 1000LL);
#endif
    //if the original key is in the sugested key list, we say a success. In that case we return
    //the number of remaining keys. Otherwise, we return -1
    if(success == 1) {
        remKeys = keys38.size();
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
    int noEXP = 4;
    EXP = noEXP;
    uint8_t cips[EXP*32], fcips[EXP*32];
    uint8_t key[32] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF, 0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};
    generate_random_state(key);

    oracle(key, cips, fcips, noEXP);
    printf("-------------------Master Key-------------------------------\n");
    printreg(key, 32);
    printf("------------------------------------------------------------\n");
    int rem = attack(cips, fcips, key, noEXP);
    cout << "\n Number of faults " << EXP << " Number of threads " << 
        THREAD << " Remaining Keys " << rem << endl;
}
/***************************************************************************************/

#if NOCYTHON == 1
int main(){
    srand(time(NULL));
    xx_initialize(SEED);
    printf("SEED: ");
    printreg(SEED, 32);
    printf("\n\n");
    testAttack();
    return 0;
}
#endif


