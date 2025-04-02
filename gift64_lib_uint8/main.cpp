
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

#include "oracle.h"


int main(){
    srand((unsigned) time (NULL));

    /* giving the plaintext */
    uint8_t P[16] = {0xd, 0x7, 0xa, 0x8, 0xb, 0x9, 0xa, 0x7, 0x2, 0x7, 0x7, 0xc, 0x0, 0x5, 0x4, 0xc};
    uint8_t K[32] = {0x7, 0xe, 0x4, 0x4, 0x0, 0x5, 0x7, 0xc, 0xf, 0xf, 0x6, 0xf, 0x9, 0xf, 0x1, 0xa,
                     0x3, 0x1, 0x7, 0x2, 0xc, 0xb, 0x6, 0xb, 0xe, 0x1, 0x3, 0x7, 0x1, 0x9, 0xd, 0xb};

    /* for printing purpose */
    /* printing the plaintext */
    cout<<"Plaintext = ";
    for (int i=0; i<16; i++){
    	P[15-i]=15-i;
        cout<<hex<<(int)P[15-i]<<dec;
        cout<<"  ";
    }
    cout<<endl;

    /* printing the key */
    cout<<"masterkey = ";
    for (int i=0; i<32;i++){
    	K[31-i]=i%16;//(31-i)%16;
        cout<<(int)K[31-i]<<dec;
        cout<<",  ";
    }
    cout<<endl<<endl;

    enc64(P,K,28,1);

    cout<<"Ciphertext = ";
    for (int i=0; i<16; i++){
        cout<<hex<<(int)P[15-i]<<dec;
        if(i%2==1) cout<<" ";
    }
    cout<<endl<<endl;
    exit(0);

}

