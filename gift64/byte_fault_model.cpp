#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cmath>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include <iostream>
#include <vector>

using namespace std;
#define EXP 32
#define ROUNDS 28
#define FROUND 24


#include "utility.h"
#include "gift64.h"

#include "filter_keys_second_step.h"
#include "find_25.h"
#include "combine_keys_advanced.h"
#include "filter_keys_pack.h"

void oracle(uint64_t *key, uint64_t *cips, uint64_t *fcips, uint64_t *rks){
    for(int i=0; i<EXP; i++){
        uint64_t msg = rand64();
        f_dual_enc(msg, key, FROUND, cips+i, fcips+i, rks);
    }
}

void attack(){
    uint64_t cips[EXP], fcips[EXP], key[2];
    randTwo64(key);
    printreg(key, 16);
    uint64_t rks[ROUNDS];
    oracle(key, cips, fcips, rks);
    printreg(key, 16);
    
    /* generate_round_keys(key, rks); */

    cout << "Given 25, 26, 27 :-------------------------------------"<<endl;
    printstate(rks[25]);
    printstate(rks[26]);
    /* printkey(rks[27]); */
    uint64_t temp1 = invPLayer(rks[27]);
    printstate(temp1);
    
    /* test_two_round_dec(cips, fcips, rks[26], temp1); */

    test_key(cips, fcips, rks[26], temp1);

    vector<uint64_t> keys25;
    vector<uint64_t> keys26;
    vector<uint64_t> keys27;
    find2627Key(cips, fcips, keys25, keys26, keys27);
    /* reduce_24(cips, fcips, rks[25], rks[26], temp1); */

    cout << "Given 25, 26, 27 :-------------------------------------"<<endl;
    printkey(rks[25]);
    printkey(rks[26]);
    /* printkey(rks[27]); */
    uint64_t temp = invPLayer(rks[27]);
    printkey(temp);

    cout << "Found 25, 26, 27 :-------------------------------------"<<endl;
    cout << keys25.size() <<" "<< keys26.size()<< " " << keys27.size() << endl;
    for(int i=0; i<keys25.size(); i++){
        printkey(keys25[i]);
    }
    cout << endl;

    for(int i=0; i<keys26.size(); i++){
        printkey(keys26[i]);
        printkey(keys27[i]);
    }

    cout << "\n Number of faults " << EXP << " Number of threads " << THREAD << endl;
    generateProper(0xFF, 0);
}
int main(){
    xx_initialize(SEED);
    printf("SEED: ");
    printreg(SEED, 32);
    printf("\n\n");
    attack();
    return 0;
}



