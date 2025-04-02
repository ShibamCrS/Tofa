#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gift64.h"
#include "hal.h"
#include "simpleserial.h"




uint64_t key_copy[2] = {0x0123456789ABCDEFUL, 0x0123456789ABCDEFUL};
uint8_t msg_copy[16];
uint64_t msg;

uint8_t get_key(uint8_t* k)
{
	//ignore the input k and use pre-defined key

	return 0x00;
}

void convert_word_to_char(uint64_t a, int nrof_byte){
    int i;
    uint64_t p;
    for(i=0; i < nrof_byte; i++){
    	p=a>>4*i;
        msg_copy[i]=(p & 0xF); //uint8_t c[4+8];

    }
}
void convert_char_to_word(uint8_t* pt, int nrof_byte){
    int i;
    msg=0;
    for(i=0; i < nrof_byte; i++){
        msg=(msg<<4)+(pt[i] & 0xF); 
    }
}
uint8_t get_pt(uint8_t* pt)
{
    uint64_t cip;

    convert_char_to_word(pt,16);
    
    trigger_high();
    cip =enc_gift64(msg, key_copy);
    trigger_low();
    
    convert_word_to_char(cip,16);
    
    simpleserial_put('r', 16, msg_copy);

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
