#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gift128.c"
#include "hal.h"
#include "simpleserial.h"


uint8_t key[32] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};

uint8_t key_copy[32];
uint8_t msg_copy[32];

uint8_t get_key(uint8_t* k)
{
	//ignore the input k and use pre-defined key

	return 0x00;
}

uint8_t get_pt(uint8_t* pt)
{
    
    for(int j=0;j<32;j++)
        msg_copy[j]=pt[j];
    
    for(int j=0;j<32;j++)
        key_copy[j]=key[j];
        
    //trigger_high();

    enc(40, msg_copy, key_copy);

    //trigger_low();
    
    simpleserial_put('r', 32, msg_copy);

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
    simpleserial_addcmd('p', 32,  get_pt);
    simpleserial_addcmd('x',  0,   reset);
    while(1)
        simpleserial_get();
  
}
