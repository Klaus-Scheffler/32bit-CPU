#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


//https://cs.stackexchange.com/questions/80668/simple-algorithm-for-ieee-754-division-on-8-bit-cpu


uint32_t idiv (uint32_t N, uint32_t D)
{
  uint32_t Q = 0;
  uint32_t R = 0;
  int i;

  for (i = 31; i >= 0; --i) {
    Q = Q << 1;
    R = R << 1;

    R |= (N >> 31) & 1;
    N = N << 1;

    if (D <= R) {
      R = R - D;
      Q = Q | 1;
    }
  }

  printf("Q = %08x, R = %08x\n", Q, R);
  return Q;
}

	

int main () {
	
uint32_t f1 = 200;
uint32_t f2 = 2;
uint32_t f3;

f3 = f1/f2;

  printf("\nf1: %08x, f2: %08x, f3: %08x\n",f1,f2,f3);
  
f3 = idiv(f1,f2);
  
}
