#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gift64.h"
#include "hal.h"
#include "simpleserial.h"




uint8_t msg_copy[16],msg1_copy[16];
uint8_t K[32] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

uint8_t get_key(uint8_t* k)
{
	//ignore the input k and use pre-defined key

	return 0x00;
}


uint8_t get_pt(uint8_t* pt)
{

    for(int i=0; i < 16; i++){
        msg_copy[i]=pt[i]; 
    }
    
    //trigger_high();
    enc64(msg_copy,K,28,1);
    //trigger_low();
    
    for(int i=0; i < 16; i++){
        msg1_copy[15-i]=msg_copy[i]; 
    }
    
    
    simpleserial_put('r', 16, msg1_copy);

   return 0x00;
}

uint8_t reset(uint8_t* x)
{
    // Reset key here if needed
	return 0x00;
}

int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();
    
    
	simpleserial_init();
    simpleserial_addcmd('k', 16,  get_key);
    simpleserial_addcmd('p', 16,  get_pt);
    simpleserial_addcmd('x',  0,   reset);
    while(1)
        simpleserial_get();
  
}
