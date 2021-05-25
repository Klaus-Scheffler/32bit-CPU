#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


//https://cs.stackexchange.com/questions/80668/simple-algorithm-for-ieee-754-division-on-8-bit-cpu


uint32_t imul (uint32_t N, uint32_t D)
{
{

    uint32_t productRegisterLower = N;
    uint32_t productRegisterUpper = 0;
    int n;

    for (n = 0; n < 32; n++) {
        if (productRegisterLower & 1 == 1) {
            productRegisterUpper += D;
        }
        productRegisterLower >>= 1;
        productRegisterLower |= productRegisterUpper << 31;
        productRegisterUpper >>= 1;
    }

    printf("Result: %d\n", productRegisterLower);
    printf("Result: %08x\n", productRegisterUpper);
}
}

	

int main () {
	
uint32_t f1 = 0x80000000;
uint32_t f2 = 0x80000002;
uint32_t f3;

f3 = f1*f2;

  printf("\nf1: %08x, f2: %08x, f3: %08x\n",f1,f2,f3);
  
f3 = imul(f1,f2);
  
}
