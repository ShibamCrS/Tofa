
/* If the print details is true then only all the state values will print */
char *print_details = "false";
/* char* print_details = "true"; */

#define no_of_rounds 28
#define state_size 64


/* right shift within 16-bit only */
uint16_t circ_right_shift(uint16_t x, int pos){		
	return ((x>>pos)|(x<<(16-pos)));
	}							


void generate_round_keys(uint64_t *round_key){	
	for(uint8_t round=0; round<no_of_rounds; round++){
        uint16_t k[8] = {0};
	
		for(int i=0;i<8;i++){
            uint8_t half = 0;
            /* the lsb 64-bits are in the 1th half */
            if (i<4){
                half = 1;
                }

            /* taking 16-bit keys into k[i]'s. k = k[7]||k[6]||...||k[0] */
			k[i] = (round_key[2*round + half] >> 16*(i%4))&0xffff;
			}

        /* making the keys for the next round */
        insert(&round_key[2*(round+1)], 0x0, circ_right_shift(k[1],2)&0xffff);
        shift(&round_key[2*(round+1)], 16, 128, "left");
        round_key[2*(round+1) + 1] |= circ_right_shift(k[0], 12)&0xffff;

		for(int i=7;i>=2;i--){
            shift(&round_key[2*(round+1)], 16, 128, "left");
			round_key[2*(round+1) + 1] |= k[i]&0xffff;
            }}
    
    /* for printing purpose */
    if (print_details == "true"){
        printf("********************************************************************************\n");
        printf("round keys:\n");
        printf("********************************************************************************\n");

        for(uint8_t round=0; round<=no_of_rounds; round++){
            printf("for round %d:\t", round);
            print(&round_key[2*round], 128);
            }}}


void sbox(uint64_t *msg){
    /* sbox table of baksheesh cipher */
    uint8_t sbox_table[16] = {0x1, 0xa, 0x4, 0xc, 0x6, 0xf, 0x3, 0x9, 0x2, 0xd, 0xb, 0x7, 0x5, 0x0, 0x8, 0xe};

    uint64_t cip = 0;

    /* applying sbox on each nibble */
    for(int8_t i=15;i>=0;i--){
        uint8_t nibble = sbox_table[(msg[0] >> 4*i)&0xf];;
        cip = (cip << 4)|(nibble&0xf);
        }

    /* storing the modified msg in cip */
    msg[0] = cip;
    
    /* for printing purpose */
    if (print_details == "true"){
        printf("after sbox:\t");
        print(msg, state_size);
        }
    }   

 
void p_layer(uint64_t *msg){
    /* permutation table of baksheesh same as gift */
    uint8_t perm_table[64] = { 0, 17, 34, 51, 48, 1, 18, 35, 32, 49, 2, 19, 16, 33, 50, 3, 
                               4, 21, 38, 55, 52, 5, 22, 39, 36, 53, 6, 23, 20, 37, 54, 7,	
                               8, 25, 42, 59, 56, 9, 26, 43, 40, 57, 10, 27, 24, 41, 58, 11, 
                               12, 29, 46, 63, 60, 13, 30, 47, 44, 61, 14, 31, 28, 45, 62, 15};

    /* storing the permutation in cip */
    uint64_t cip = 0;

    for(uint8_t i=0; i<64; i++){
        cip |= (((msg[0] >> i)&1) << perm_table[i]);
        }

    msg[0] = cip;

    /* for printing purpose */
    if (print_details == "true"){
        printf("after p-layer:\t");
        print(msg, state_size);
        }}


void add_round_key(uint64_t *msg, uint64_t *key){
    /* taking the corresponding 16-bits in u and v */
	uint16_t u = (key[1]>>16)&0xffff;
    uint16_t v = key[1]&0xffff;

	uint64_t u1=0, 
             v1=0;

    /* making u1 and v1 from u and v of the round key */
	for(uint8_t i=0;i<16;i++){
		v1 = (v1 << 4) | ((v>>(15-i))&1);
		u1 = (u1 << 4) | ((u>>(15-i))&1);
		}
	u1 = u1<<1; 

    /* xoring u1 and v1 with the msg */
    xr(msg, &u1, 64);
    xr(msg, &v1, 64);
    
    /* for printing purpose */
    if (print_details == "true"){
        printf("after add rk:\t");
        print(msg, state_size);
        }
	}


void add_rc(uint64_t *msg, uint8_t round_number){
    /* round constants of gift-64 */
    uint8_t rc[28] = {  0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F, 0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 
                        0x1D, 0x3A, 0x35, 0x2B, 0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B};

    /* round constant positions */
	uint8_t rcon_pos[6] = {23, 19, 15, 11, 7, 3};

	uint64_t rcon = 0;
	for(int i=0;i<6;i++){
		rcon = rcon | ((uint32_t)((rc[round_number]>>i)&1) << rcon_pos[5-i]);
		}
	rcon = rcon|0x8000000000000000;	

    xr(msg, &rcon, 64);

    /* for printing purpose */
    if (print_details == "true"){
        printf("after add rk:\t");
        print(msg, state_size);
        }}


void oracle(uint64_t *msg, uint64_t *key){
    /* allocating mem for round keys */
    uint64_t *round_key = mem_alloc(128*(no_of_rounds + 1));

    /* coping the original key into 0-th round key */
    copy(&round_key[0], key, 128);
	generate_round_keys(round_key);
		
    /* round function */
	for(uint8_t round=0; round < no_of_rounds; round++){
        /* for printing purpose */
        if (print_details == "true"){
            printf("\n");
            printf("********************************************************************************\n");
            printf("for round %d:\n", round);
            printf("********************************************************************************\n");
            }

		sbox(msg);
		p_layer(msg);	
		add_round_key(msg, &round_key[2*round]);
		add_rc(msg, round);
		}

    if (print_details == "true"){
        printf("********************************************************************************\n");
        }}
	
