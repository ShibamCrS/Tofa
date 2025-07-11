#include "xoshiro256plusplus.h"
#define THREAD 128
#define NR_OF_SBOX 32
#define NIBBLE 4
#define GRP 8

uint64_t SEED[4];

void printreg_to_file(const void *a, int nrof_byte){
    int i;
    unsigned char *f = (unsigned char *)a;
    for(i=0; i < nrof_byte; i++){
        printf("%X ",(unsigned char) f[nrof_byte - 1 - i]); //uint8_t c[4+8];
    }
    printf("\n");
}
void printreg(const void *a, int nrof_byte){
    printreg_to_file(a, nrof_byte);
}

void print_list(uint8_t *a, int size_of_list, int size_of_el){
    for(int i=0; i<size_of_list; i++){
        printreg(a + (i*32), 32);
    }
    printf("\n");
}
uint64_t rand64(){
    uint64_t random64 = xx_next(SEED);
    return random64;
}
void generate_random_state(uint8_t *random_data){
    uint64_t random64 = xx_next(SEED);
    for(int i=0; i<16; i++){
        random_data[i] = rand() & 0x0F; //(random64 >> (4*i)) & 0x0F; 
    }
    random64 = xx_next(SEED);
    for(int i=0; i<16; i++){
        random_data[16 + i] = rand() & 0x0F; //(random64 >> (4*i)) & 0x0F; 
    }

}
void reverse16byte(uint8_t *a){
    uint8_t temp[16];
    for(int i=0; i<16; i++){
        temp[i] = a[15-i];
    }
    memcpy(a, temp, 16);
}
int mem_cmp(const void *a, const void *b, uint32_t nrof_byte){
    if(memcmp(a, b, nrof_byte) == 0){
        return 1;
    }
    else{
        return 0;
    }
}

void xor_sum_32(uint8_t *a, uint8_t *b, uint8_t *c){
    for (int i=0; i<32; i++){
        c[i] = a[i] ^ b[i];
    }
}
void print_arr(int *arr, int size){
    for (int i=0; i<size; i++){
        printf("%d ",arr[i]);
    }
    printf("\n");
}
void print_arr_int64(int64_t *arr, int size){
    for (int i=0; i<size; i++){
        printf("%ld ",(long int)arr[i]);
    }
    printf("\n");
}
void print_arr_float(float *arr, int size){
    for (int i=0; i<size; i++){
        printf("%f ",arr[i]);
    }
    printf("\n");
}
void print_arr_double(double *arr, int size){
    for (int i=0; i<size; i++){
        printf("%lf ",arr[i]);
    }
    printf("\n");
}

void save_array_double(FILE *fp, double *data, uint64_t size){
    uint64_t written = 0UL;
    written = fwrite(data, sizeof(double), size, fp);
    if (written < size){
        printf("Error during writing to file !");
    }
    fclose(fp);
}

void read_array_double(FILE *fp, double *data, uint64_t size){
    uint64_t read = 0;
    read = fread(data, sizeof(double), size, fp);
    
    if (read < size){
        printf("Read %ld ", (unsigned long int)read);
        printf("Error during reading from file !\n");
    }
    fclose(fp);
}


void save_array(FILE *fp, float *data, uint64_t size){
    uint64_t written = 0UL;
    written = fwrite(data, sizeof(float), size, fp);
    if (written < size){
    printf("Error during writing to file !");
    }
}

void read_array(FILE *fp, float *data, uint64_t size){
    uint64_t read = 0;
    read = fread(data, sizeof(float), size, fp);
    
    if (read < size){
        printf("Read %ld", (unsigned long int)read);
        printf("Error during reading from file !");
    }
}
