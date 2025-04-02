/* gist: This is my library functions for c programming. The functions works on the chunks of 64-bit */ 
/* memory. */
/* ----------------------------------------------------------------------------------------- */

/* functions: */
/*     (i)     uint64_t* mem_alloc(uint32_t size) */
/*     (ii)    void rand_alloc(uint64_t *msg, uint32_t size) */
/*     (iii)   void copy(uint64_t *x, uint64_t *y, uint32_t size) */    
/*     (iv)    void print(uint64_t *state, uint32_t size) */
/*     (v)     void circ_shift(uint64_t *msg, uint8_t pos, uint32_t size, char* str) */
/*     (vi)    void shift(uint64_t *msg, uint8_t pos, uint32_t size, char* str) */
/*     (vii)   void xr(uint64_t *a, uint64_t *b, uint32_t size) */
/*     (viii)  void insert(uint64_t *x, uint64_t msb, uint64_t lsb) */    
/*     (ix)    uint8_t check_eq(uint64_t *msg1, uint64_t *msg2, uint32_t size) */

/* ----------------------------------------------------------------------------------------- */


/* allocate the memory of 8-bit chunks */
uint8_t* mem_alloc8(uint32_t size){
    uint8_t *msg = (uint8_t*)calloc(size, sizeof(uint8_t));
    return msg;
}


/* /1* this creates the random msg of the given size *1/ */
/* void rand_alloc(uint64_t *msg, uint32_t size){ */
/*     for (uint16_t i=0; i<(size/64); i++){ */
/*         msg[i] = rand(); */
/*         msg[i] = (msg[i]<<32)|(rand()&0xffffffff); */
/*         } */
/*     } */   


/* /1* coping 64-bits of y to x *1/ */
void copy8(uint8_t *x, uint8_t *y, uint32_t size){    
    for (uint8_t i=0; i<size; i++){
        x[i] = y[i];
    }
}


/* printing the size/4 many eles of the state */
void print8(uint8_t *state, uint32_t size){
    for (int8_t i=size-1; i>=0; i--){
        printf("%x", state[i]);
        /* after 4 nibbles print space */
        if (((i%4) == 0) && (i>0)){
            printf(" ");
        }
    }   
    printf("\n");
    }


/* /1* right circular shift for 128-bit only *1/ */
/* void circ_shift(uint64_t *msg, uint8_t pos, uint32_t size, char* str){ */
/*     /1* if the shifting position is greater than the msg size *1/ */
/*     if (pos>size){ */
/*         printf("shifting position is greater than msg size!"); */
/*         abort(); */
/*         } */

/*     /1* for the left shift *1/ */
/*     if (str == "left"){ */
/*         uint16_t idx64 = pos/64; */
/*         uint16_t pos64 = pos%64; */

/*         uint64_t *cip = mem_alloc(size); */

/*         for (uint16_t i=0; i<size/64; i++){ */
/*             /1* if (i+idx64) exceeds the cell size, then it will circularly takes the cell values from 0th cell *1/ */
/*             if ((i + idx64) > ((size/64) - 1)){ */
/*                 cip[i] = msg[i+idx64 - (size/64)]; */

/*                 /1* if the shifting in teh cell is non-zero *1/ */
/*                 if (pos64 != 0){ */
/*                     cip[i] = cip[i]<<pos64; */
/*                     cip[i] |= (msg[i+idx64 - ((size/64) -1)] >> (64-pos64)); */
/*                     } */
/*                 continue; */
/*                 } */

/*             /1* i+idx64 th cell will be copied to ith cell *1/ */
/*             cip[i] = msg[i + idx64]; */

/*             /1* if the shifting val is non-zero within the cell, then shift in the cell *1/ */
/*             if (pos64 != 0){ */
/*                 cip[i] = cip[i]<<pos64; */

/*                 /1* if the next 64-bit is available then shift in the next also *1/ */
/*                 if ((i + idx64) < ((size/64) - 1)){ */
/*                     cip[i] |= (msg[i+1+idx64] >> (64-pos64)); */
/*                     } */

/*                 /1* if the (i+idx64) reaches its limit, it will take the bits from the 0th cell *1/ */ 
/*                 if ((i + idx64) == ((size/64) - 1)){ */
/*                     cip[i] |= (msg[0] >> (64-pos64)); */
/*                     }}} */
/*         copy(msg, cip, size); */
/*         free(cip); */
/*         } */

/*     /1* for the right shift *1/ */
/*     else if (str == "right"){ */
/*         uint16_t idx64 = pos/64; */
/*         uint16_t pos64 = pos%64; */

/*         uint64_t *cip = mem_alloc(size); */

/*         for (int16_t i=size/64 - 1; i>=0; i--){ */
/*             /1* for the circular shift if the target cell is lower than 0, then continue with the last cell *1/ */  
/*             if ((i - idx64) < 0){ */
/*                 cip[i] = msg[(size/64)-1 - i]; */

/*                 /1* if there is nothing to shift in the cell *1/ */
/*                 if (pos64 != 0){ */
/*                     cip[i] = cip[i]>>pos64; */
/*                     cip[i] |= (msg[size/64 - 1 - (idx64 - i)] << (64-pos64)); */
/*                     } */

/*                 continue; */
/*                 } */

/*             cip[i] = msg[i-idx64]; */
/*             if (pos64 != 0){ */
/*                 cip[i] = cip[i] >> pos64; */

/*                 /1* if the next 64-bit is available then shift in the next also *1/ */
/*                 if ((i-idx64) > 0){ */
/*                     cip[i] |= (msg[i-idx64 -1] << (64-pos64)); */
/*                     } */

/*                 /1* if (i-idx64) reaches at 0, then the next will be the last cell *1/ */
/*                 if ((i - idx64) == 0){ */
/*                     cip[i] |= (msg[size/64 - 1] << (64-pos64)); */
/*                     }}} */

/*         copy(msg, cip, size); */
/*         free(cip); */
/*         } */

/*     /1* if the size is not given properly. *1/ */
/*     else{ */
/*         printf("the shift direction is not properly given.\n"); */
/*         abort(); */
/*         }} */


/* /1* depending upon the direction, msg shifts to pos many bits *1/ */
/* void shift(uint64_t *msg, uint8_t pos, uint32_t size, char* str){ */
/*     /1* for the left shift *1/ */
/*     if (str == "left"){ */
/*         uint16_t idx64 = pos/64; */
/*         uint16_t pos64 = pos%64; */

/*         uint64_t *cip = mem_alloc(size); */

/*         for (uint16_t i=0; i<size/64; i++){ */
/*             /1* if the cell exceeds the last cell then break *1/ */
/*             if ((i + idx64) >= size/64){ */
/*                 break; */
/*                 } */

/*             cip[i] = msg[i + idx64]; */

/*             /1* if the pos in the cell is not 0 *1/ */
/*             if (pos64 != 0){ */
/*                 cip[i] = cip[i]<<pos64; */

/*                 /1* if the next 64-bit is available then shift in the next also *1/ */
/*                 if ((i + idx64) < (size/64 - 1)){ */
/*                     cip[i] |= (msg[i+1+idx64] >> (64-pos64)); */
/*                     }}} */
/*         copy(msg, cip, size); */
/*         free(cip); */
/*         } */

/*     /1* for the right shift *1/ */
/*     else if (str == "right"){ */
/*         uint16_t idx64 = pos/64; */
/*         uint16_t pos64 = pos%64; */

/*         uint64_t *cip = mem_alloc(size); */

/*         for (int16_t i=size/64 - 1; i>=0; i--){ */
/*             /1* if the lowest cell exceeds 0th cell, then break *1/ */
/*             if ((i - idx64) < 0){ */
/*                 break; */
/*                 } */

/*             cip[i] = msg[i-idx64]; */

/*             /1* if the shifting pos in the cell is 0 *1/ */
/*             if (pos64 != 0){ */
/*                 cip[i] = cip[i]>>pos64; */

/*                 /1* for the cell numbers greater than 0 *1/ */
/*                 if ((i - idx64) >= 1){ */
/*                     cip[i] |= (msg[i-1-idx64] << (64-pos64)); */
/*                     }}} */
/*         copy(msg, cip, size); */
/*         free(cip); */
/*         } */

/*     else{ */
/*         printf("shift direction is not properly given.\n"); */
/*         abort(); */
/*         }} */


/* /1* does xor of a, b (depending upon the size) and keep that in c *1/ */
/* void xr(uint64_t *a, uint64_t *b, uint32_t size){ */
/*     for (uint8_t i=0; i<(size/64); i++){ */
/*         a[i] ^= b[i]; */
/*         } */
/*     } */


/* insert 128-bit into 8-bit chunks of x */
void insert8(uint8_t *x, uint64_t msb, uint64_t lsb){    
    for (uint8_t i=0; i<16; i++){
        x[i] = (lsb >> 4*i)&0xf;
        x[i+16] = (msb >> 4*i)&0xf;
        }
    }


/* /1* checks whether the two msgs are equal or not *1/ */
/* uint8_t check_eq(uint64_t *msg1, uint64_t *msg2, uint32_t size){ */
/*     for (uint8_t i=0; i<(size/64); i++){ */
/*         if (msg1[i] != msg2[i]){ */
/*             return 0; */
/*             }} */

/*     return 1; */
/*     } */   



