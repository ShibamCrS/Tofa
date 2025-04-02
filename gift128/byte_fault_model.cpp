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

#define EXP 6
#define ROUND 40
#define FROUND 35

void print_vector(vector<uint8_t> &A){
    for(int a=0; a<A.size(); a++){
        printf("%X ",A[A.size() - 1 - a]);
    }
    printf("\n");
}
void print_vector(vector<uint32_t> &A){
    for(auto a: A){
        printf("0x%08X, ",a);
    }
    printf("\n");
}
void addRk1(uint8_t *state, uint8_t *key){
    for (int i=0; i<32; i++){
        state[i] = state[i] ^ key[i];
    }
}
#include "check_36_37.h"
#include "filter_keys_second_step.h"
#include "combine_keys_advanced.h"
#include "filter_keys_pack.h"

void oracle1(uint8_t *key, uint8_t *cips, uint8_t *fcips,int *OrFaultLocations, uint8_t **rks) {
/* Harvested data 3.23, 11/04/2024 ........................................*/
    /* uint8_t pHarsvested[EXP][32] = { */
    /*     {0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 }, */
    /*     {1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 }, */ 
    /*     {2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 }, */
    /*     {3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 }}; */

    /* uint8_t cipsHarvested[EXP][32]  = { */
    /*     {2 ,5 ,1 ,0 ,14 ,14 ,9 ,0 ,4 ,8 ,6 ,4 ,15 ,10 ,6 ,4 ,3 ,9 ,10 ,5 ,15 ,11 ,13 ,6 ,10 ,1 ,4 ,2 ,2 ,2 ,4 ,8 }, */ 
    /*     {8 ,13 ,3 ,6 ,11 ,6 ,9 ,5 ,9 ,2 ,12 ,3 ,3 ,9 ,8 ,8 ,6 ,14 ,1 ,14 ,7 ,5 ,14 ,15 ,2 ,8 ,8 ,0 ,11 ,8 ,1 ,6}, */
    /*     {14 ,8 ,1 ,9 ,14 ,1 ,15 ,10 ,9 ,8 ,4 ,1 ,12 ,5 ,2 ,8 ,4 ,3 ,9 ,7 ,6 ,10 ,13 ,10 ,10 ,8 ,13 ,8 ,13 ,12 ,14 ,14}, */
    /*     {0 ,14 ,10 ,14 ,14 ,15 ,14 ,8 ,7 ,11 ,2 ,14 ,13 ,2 ,11 ,4 ,13 ,4 ,11 ,10 ,5 ,12 ,11 ,15 ,4 ,7 ,2 ,10 ,5 ,0 ,14 ,2}}; */

    /* uint8_t fcipsHarvested[EXP][32] = { */
    /*      {3 ,15 ,10 ,0 ,11 ,12 ,13 ,0 ,0 ,15 ,7 ,2 ,15 ,2 ,4 ,2 ,3 ,1 ,2 ,4 ,7 ,1 ,6 ,6 ,3 ,9 ,9 ,0 ,8 ,5 ,10 ,9}, */
    /*      {15 ,1 ,2 ,7 ,9 ,3 ,11 ,13 ,7 ,2 ,15 ,3 ,6 ,2 ,8 ,1 ,11 ,15 ,6 ,8 ,13 ,3 ,4 ,8 ,3 ,10 ,14 ,12 ,11 ,12 ,5 ,10}, */
    /*      {15 ,8 ,8 ,12 ,7 ,5 ,14 ,2 ,9 ,13 ,6 ,15 ,4 ,1 ,0 ,11 ,15 ,8 ,15 ,15 ,10 ,10 ,13 ,7 ,12 ,6 ,5 ,9 ,9 ,15 ,14 ,8}, */
    /*      {2 ,10 ,11 ,8 ,8 ,10 ,13 ,15 ,3 ,7 ,2 ,6 ,5 ,2 ,3 ,12 ,5 ,4 ,11 ,11 ,12 ,8 ,2 ,3 ,4 ,4 ,10 ,1 ,6 ,2 ,7 ,3 }}; */

    /* uint8_t pHarsvested[EXP][32] = {{0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15},{1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0},{2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1},{3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,},{4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3}}; */

 /* uint8_t cipsHarvested[EXP][32]= {{2 ,5 ,1 ,0 ,14 ,14 ,9 ,0 ,4 ,8 ,6 ,4 ,15 ,10 ,6 ,4 ,3 ,9 ,10 ,5 ,15 ,11 ,13 ,6 ,10 ,1 ,4 ,2 ,2 ,2 ,4 ,8},{8 ,13 ,3 ,6 ,11 ,6 ,9 ,5 ,9 ,2 ,12 ,3 ,3 ,9 ,8 ,8 ,6 ,14 ,1 ,14 ,7 ,5 ,14 ,15 ,2 ,8 ,8 ,0 ,11 ,8 ,1 ,6},{14 ,8 ,1 ,9 ,14 ,1 ,15 ,10 ,9 ,8 ,4 ,1 ,12 ,5 ,2 ,8 ,4 ,3 ,9 ,7 ,6 ,10 ,13 ,10 ,10 ,8 ,13 ,8 ,13 ,12 ,14 ,14},{0 ,14 ,10 ,14 ,14 ,15 ,14 ,8 ,7 ,11 ,2 ,14 ,13 ,2 ,11 ,4 ,13 ,4 ,11 ,10 ,5 ,12 ,11 ,15 ,4 ,7 ,2 ,10 ,5 ,0 ,14 ,2},{7 ,14 ,14 ,8 ,10 ,9 ,10 ,15 ,6 ,6 ,3 ,9 ,11 ,10 ,15 ,1 ,4 ,5 ,7 ,10 ,6 ,5 ,10 ,0 ,5 ,15 ,7 ,2 ,13 ,13 ,9 ,5 }}; */

/* uint8_t fcipsHarvested[EXP][32] = {{3 ,15 ,10 ,0 ,11 ,12 ,13 ,0 ,0 ,15 ,7 ,2 ,15 ,2 ,4 ,2 ,3 ,1 ,2 ,4 ,7 ,1 ,6 ,6 ,3 ,9 ,9 ,0 ,8 ,5 ,10 ,9},{15 ,1 ,2 ,7 ,9 ,3 ,11 ,13 ,7 ,2 ,15 ,3 ,6 ,2 ,8 ,1 ,11 ,15 ,6 ,8 ,13 ,3 ,4 ,8 ,3 ,10 ,14 ,12 ,11 ,12 ,5 ,10},{15 ,8 ,8 ,12 ,7 ,5 ,14 ,2 ,9 ,13 ,6 ,15 ,4 ,1 ,0 ,11 ,15 ,8 ,15 ,15 ,10 ,10 ,13 ,7 ,12 ,6 ,5 ,9 ,9 ,15 ,14 ,8},{2 ,10 ,11 ,8 ,8 ,10 ,13 ,15 ,3 ,7 ,2 ,6 ,5 ,2 ,3 ,12 ,5 ,4 ,11 ,11 ,12 ,8 ,2 ,3 ,4 ,4 ,10 ,1 ,6 ,2 ,7 ,3},{6 ,6 ,14 ,12 ,11 ,5 ,10 ,13 ,0 ,7 ,7 ,9 ,11 ,11 ,15 ,1 ,4 ,7 ,7 ,10 ,14 ,4 ,10 ,0 ,12 ,11 ,6 ,0 ,4 ,13 ,9 ,4}}; */

    uint8_t pHarsvested[32][32] = {{0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15},{1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0},{2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1},{3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2},{4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3},{5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4},{6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15 ,0 ,1 ,2 ,3 ,4 ,5}};
       uint8_t  cipsHarvested[32][32]= {{2 ,5 ,1 ,0 ,14 ,14 ,9 ,0 ,4 ,8 ,6 ,4 ,15 ,10 ,6 ,4 ,3 ,9 ,10 ,5 ,15 ,11 ,13 ,6 ,10 ,1 ,4 ,2 ,2 ,2 ,4 ,8},{8 ,13 ,3 ,6 ,11 ,6 ,9 ,5 ,9 ,2 ,12 ,3 ,3 ,9 ,8 ,8 ,6 ,14 ,1 ,14 ,7 ,5 ,14 ,15 ,2 ,8 ,8 ,0 ,11 ,8 ,1 ,6},{14 ,8 ,1 ,9 ,14 ,1 ,15 ,10 ,9 ,8 ,4 ,1 ,12 ,5 ,2 ,8 ,4 ,3 ,9 ,7 ,6 ,10 ,13 ,10 ,10 ,8 ,13 ,8 ,13 ,12 ,14 ,14},{0 ,14 ,10 ,14 ,14 ,15 ,14 ,8 ,7 ,11 ,2 ,14 ,13 ,2 ,11 ,4 ,13 ,4 ,11 ,10 ,5 ,12 ,11 ,15 ,4 ,7 ,2 ,10 ,5 ,0 ,14 ,2},{7 ,14 ,14 ,8 ,10 ,9 ,10 ,15 ,6 ,6 ,3 ,9 ,11 ,10 ,15 ,1 ,4 ,5 ,7 ,10 ,6 ,5 ,10 ,0 ,5 ,15 ,7 ,2 ,13 ,13 ,9 ,5},{15 ,11 ,15 ,6 ,3 ,0 ,12 ,6 ,13 ,9 ,1 ,4 ,3 ,11 ,9 ,9 ,12 ,8 ,12 ,14 ,2 ,13 ,1 ,11 ,9 ,8 ,12 ,0 ,0 ,10 ,7 ,10},{10 ,6 ,8 ,1 ,2 ,13 ,5 ,2 ,8 ,15 ,9 ,11 ,10 ,3 ,11 ,11 ,8 ,9 ,5 ,3 ,12 ,1 ,2 ,5 ,0 ,14 ,15 ,7 ,8 ,7 ,4 ,9}};

uint8_t fcipsHarvested[32][32] = {{3 ,15 ,10 ,0 ,11 ,12 ,13 ,0 ,0 ,15 ,7 ,2 ,15 ,2 ,4 ,2 ,3 ,1 ,2 ,4 ,7 ,1 ,6 ,6 ,3 ,9 ,9 ,0 ,8 ,5 ,10 ,9},{15 ,1 ,2 ,7 ,9 ,3 ,11 ,13 ,7 ,2 ,15 ,3 ,6 ,2 ,8 ,1 ,11 ,15 ,6 ,8 ,13 ,3 ,4 ,8 ,3 ,10 ,14 ,12 ,11 ,12 ,5 ,10},{15 ,8 ,8 ,12 ,7 ,5 ,14 ,2 ,9 ,13 ,6 ,15 ,4 ,1 ,0 ,11 ,15 ,8 ,15 ,15 ,10 ,10 ,13 ,7 ,12 ,6 ,5 ,9 ,9 ,15 ,14 ,8},{2 ,10 ,11 ,8 ,8 ,10 ,13 ,15 ,3 ,7 ,2 ,6 ,5 ,2 ,3 ,12 ,5 ,4 ,11 ,11 ,12 ,8 ,2 ,3 ,4 ,4 ,10 ,1 ,6 ,2 ,7 ,3},{6 ,6 ,14 ,12 ,11 ,5 ,10 ,13 ,0 ,7 ,7 ,9 ,11 ,11 ,15 ,1 ,4 ,7 ,7 ,10 ,14 ,4 ,10 ,0 ,12 ,11 ,6 ,0 ,4 ,13 ,9 ,4},{4 ,5 ,3 ,1 ,8 ,12 ,3 ,1 ,15 ,3 ,14 ,7 ,12 ,1 ,6 ,1 ,14 ,6 ,14 ,9 ,0 ,0 ,7 ,3 ,12 ,7 ,5 ,6 ,4 ,12 ,7 ,2},{8 ,0 ,12 ,9 ,13 ,5 ,0 ,4 ,4 ,3 ,7 ,2 ,1 ,3 ,9 ,7 ,1 ,3 ,0 ,1 ,10 ,9 ,14 ,14 ,3 ,10 ,13 ,2 ,10 ,6 ,12 ,8}};
 
    uint8_t msg2[32];
    uint8_t msg1[32];
    for(int i=0; i<EXP; i++){
        generate_random_state(msg1);
        memcpy(msg2, msg1, 32);
        
        //both fault and fault free
        OrFaultLocations[i] = dual_enc_byte(ROUND, msg1, msg2, key, FROUND, rks);
        memcpy(cips + (i*32),  cipsHarvested[i], 32);
        memcpy(fcips + (i*32), fcipsHarvested[i], 32);
    }
}
void oracle(uint8_t *key, uint8_t *cips, uint8_t *fcips,int *OrFaultLocations, uint8_t **rks){
    uint8_t msg1[32];
    uint8_t msg2[32];
    for(int i=0; i<EXP; i++){
        generate_random_state(msg1);
        memcpy(msg2, msg1, 32);
        
        //both fault and fault free
        OrFaultLocations[i] = dual_enc_byte(ROUND, msg1, msg2, key, FROUND, rks);
        memcpy(cips + (i*32), msg1, 32);
        memcpy(fcips + (i*32), msg2, 32);
    }
}
void get_round_keys(uint8_t *key, uint8_t **rks){
    cout << "Round Keys :" << endl;
    for(int i=0; i<ROUND; i++){
        uint8_t bits[128];
        memset(bits, 0x00, 128);
        uint8_t key_bits[128];
        to_bits(key, key_bits);
        int kbc=0;  //key_bit_counter
        for (int i=0; i<32; i++){
            bits[4*i+1] ^= key_bits[kbc];
            bits[4*i+2] ^= key_bits[kbc+64];
            kbc++;
        }
        from_bits(bits, rks[i]);
        if(i == (ROUND-1)){
            invPLayer(rks[i]);    
        }
        Key_update(key);
        printreg(rks[i], 32);
    }
}
void print_rks(uint8_t **rks){
    for(int i=0; i<ROUND; i++){
        printreg(rks[i], 32);
    }
}

void attack(){
    uint8_t cips[EXP*32], fcips[EXP*32];
    uint8_t key[32] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF, 0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};
    generate_random_state(key);
    printreg(key, 32);
    uint8_t **rks = (uint8_t **)malloc(ROUND*sizeof(uint8_t *));
    for(int i=0; i<ROUND; i++){
        rks[i] = (uint8_t *)malloc(32*sizeof(uint8_t));
    }
    cout <<"-------------------------------------\n";
    int OrFaultLocations[EXP];
    oracle(key, cips, fcips, OrFaultLocations, rks);
    printf("-------------------Master Key-------------------------------\n");
    printreg(key, 32);
    cout <<"-------------------------------------\n";
    /* print_list(cips, EXP, 32); */
    /* print_list(fcips, EXP, 32); */
    
    cout <<"-------------------------------------\n";
        /* get_round_keys(key, rks); */
    cout <<"-------------------------------------\n";
    /* invPLayer(rks[39]); */
    cout <<"-------------------------------------\n";

    /* test_key(cips, fcips, rks[38], rks[39]); */
    vector<vector<uint8_t> > keys38;
    vector<vector<uint8_t> > keys39;
    find3839Key(cips, fcips, keys38, keys39);

    cout << "Given 36, 37, 38, 39 :-------------------------------------"<<endl;
    printreg(rks[36], 32);
    printreg(rks[37], 32);
    printreg(rks[38], 32);
    invPLayer(rks[39]);
    printreg(rks[39], 32);

    cout << "Found 38, 39 :-------------------------------------"<<endl;
    cout << keys38.size() <<" "<< keys39.size()<<endl;
    for(int i=0; i<keys38.size(); i++){
        print_vector(keys38[i]);
        print_vector(keys39[i]);
    }

    cout << "Original Master Key: ";
    printreg(key, 32);
    cout << "Found Master Key:    ";
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
        printreg(k0, 32);
        if(memcmp(k0, key, 32) == 0) {
            printf("Match \n");
        }
    }

    for(int i=0; i<ROUND; i++){
        free(rks[i]);
    }
    free(rks);
    cout << "\n Number of faults " << EXP << " Number of threads " << THREAD << " Remaining Keys " << keys38.size() << endl;
}

void test(){
uint8_t msg[32] = {0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0, 0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0};
    uint8_t key[32] = {0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0, 0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0};
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
    srand(time(NULL));
    xx_initialize(SEED);
    printf("SEED: ");
    printreg(SEED, 32);
    printf("\n\n");
    test();
    attack();
    return 0;
}



