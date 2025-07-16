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

#define EXP 16
#define ROUND 35
#define FROUND 30

void print_vector(vector<uint8_t> &A){
    for(auto a: A){
        printf("0x%02X, ",a);
    }
    printf("\n");
}
void print_vector(vector<uint32_t> &A){
    for(auto a: A){
        printf("0x%08X, ",a);
    }
    printf("\n");
}
#include "check_31_32.h"
#include "filter_keys_second_step.h"
#include "key_relations_pack.h"
#include "filter_keys_pack.h"

void oracle(uint8_t *key, uint8_t *cips, uint8_t *fcips,int *OrFaultLocations){
    uint8_t msg1[32];
    uint8_t msg2[32];
    for(int i=0; i<EXP; i++){
        generate_random_state(msg1);
        memcpy(msg2, msg1, 32);
        
        //both fault and fault free
        OrFaultLocations[i] = dual_enc_byte(ROUND, msg1, msg2, key, FROUND);
        memcpy(cips + (i*32), msg1, 32);
        memcpy(fcips + (i*32), msg2, 32);
    }
}
void get_round_keys(uint8_t *key1, uint8_t **rks){
    uint8_t key[32];
    memcpy(key, key1, 32);
    cout << "Round Keys :" << endl;
    for(int i=0; i<ROUND; i++){
        Key_update(key);
        if(i == (ROUND-1)){
            invPLayer(key);
        }
        memcpy(rks[i], key, 32);
        /* printreg(rks[i], 32); */
    }
}
void get_master_key(vector<uint8_t> &k34, uint8_t *mk){
    for(int j=0; j<32; j++){
        mk[j] = k34[j];
    }
    for(int i=0; i<ROUND; i++){
        Key_update_dec(mk);
    }
}
void get_master_key(uint8_t *k34, uint8_t *mk){
    for(int j=0; j<32; j++){
        mk[j] = k34[j];
    }
    for(int i=0; i<ROUND; i++){
        Key_update_dec(mk);
    }
}
void attack(){
    uint8_t cips[EXP*32], fcips[EXP*32], key[32];
    generate_random_state(key);
    printreg(key, 32);
    cout <<"-------------------------------------\n";
    int OrFaultLocations[EXP];
    oracle(key, cips, fcips, OrFaultLocations);
    printreg(key, 32);
    cout <<"-------------------------------------\n";
    /* print_list(cips, EXP, 32); */
    /* print_list(fcips, EXP, 32); */
    
    cout <<"-------------------------------------\n";
    uint8_t **rks = (uint8_t **)malloc(ROUND*sizeof(uint8_t *));
    for(int i=0; i<ROUND; i++){
        rks[i] = (uint8_t *)malloc(32*sizeof(uint8_t));
    }
    cout << "Master Key : ";
    printreg(key, 32);
    get_round_keys(key, rks);
    cout <<"-------------------------------------\n";
    /* test_key(cips, fcips, rks[34], rks[33]); */
    /* test_key2(cips, fcips, rks[34], rks[33]); */
    /* test_key4(cips, fcips, rks[34], rks[33]); */
    /* test_key8(cips, fcips, rks[34]); */
    //step1
    vector<vector<uint8_t> > keys34;
    keys34 = find3433Key(cips, fcips);
    printreg(rks[33], 32);
    printreg(rks[34], 32);
    cout << "Original Master Key: ";
    uint8_t k0[32];
    PLayer(rks[34]);
    get_master_key(rks[34], k0);
    printreg(k0, 32);

    cout << "Original Master Key: ";
    printreg(key, 32);
    cout << "Found Master Key   : ";
    for(auto k: keys34){
        uint8_t k34[32];
        for(int j=0; j<32; j++){
            k34[j] = k[j];
        }
        PLayer(k34);
        uint8_t k0[32];
        get_master_key(k34, k0);
        printreg(k0, 32);
    }
    for(int i=0; i<ROUND; i++){
        free(rks[i]);
    }
    free(rks);
    cout << "\n Number of faults " << EXP << " Number of threads " << THREAD << endl;
}

void test_bahsheesh(){
    uint8_t key[32] ={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    uint8_t msg[32] = {0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4, 0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4, 0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4, 0x4,0x4,};
    
    printreg(msg, 32);
    printreg(key, 32);
    enc(35, msg, key);
    printf("-----------------------------------------------\n");
    printreg(msg, 32);
    printreg(key, 32);
    printf("-----------------------------------------------\n");
    printreg(msg, 32);
    printreg(key, 32);
    dec(35, msg, key);
    printf("-----------------------------------------------\n");
    printreg(msg, 32);
    printreg(key, 32);
    printf("-----------------------------------------------\n");
}
int main(){
    xx_initialize(SEED);
    printf("SEED: ");
    printreg(SEED, 32);
    printf("\n\n");
    test_bahsheesh();
    attack();
    return 0;
}



