#include <stdio.h>
#include <stdlib.h>


//https://www.h-schmidt.net/FloatConverter/IEEE754.html
//https://gregstoll.com/~gregstoll/floattohex/

int fmul(int a1, int a2) {
	
	unsigned int ah,al,bh,bl,ahbh,ahblalbh,albl,c,exponent1,exponent2,exponent3,mantissa1,mantissa2,a3;
	unsigned long long int c1,c2,c3;
	int ov,sign;


  exponent1 = a1 & 0x7f800000;
  exponent1 >>=23;
  exponent2 = a2 & 0x7f800000;
  exponent2 >>=23;

  mantissa1 = 0x800000 | (a1 & 0x7fffff);
  mantissa2 = 0x800000 | (a2 & 0x7fffff);
  
  sign = (a1&0x80000000) ^ (a2&0x80000000);
  
  ah = mantissa1; ah >>=16;
  al = mantissa1 & 0xffff;
  bh = mantissa2; bh >>=16;
  bl = mantissa2 & 0xffff;
 
  ahbh = ah*bh;
  ahblalbh = ah*bl + al*bh;
  albl = al*bl;
  c1 = ahbh; c1<<=32;
  c2 = ahblalbh; c2<<=16;
  c3 = albl;
  c1 = c1 + c2 + c3;
  if (c1&0x800000) ov=1; else ov=0;
  c1>>=23; c = c1;
  if (ov) c++;
  exponent3 = exponent1 + exponent2 - 127;
  if (c & 0x1000000) {exponent3++; c>>=1;}
  a3 = (c&0x7fffff) + (exponent3<<23);
  if (sign) a3 = (a3 | 0x80000000);
  return a3;
}
	

int main () {
	
float f1 = -2000.1f;
float f2 = -0.00345f;
float f3;
int ov;
unsigned int exponent1,exponent2,exponent3,mantissa1,mantissa2,mantissa3;

f3 = f1*f2;
// get address of float, cast as pointer to int, reference
int x1 = *((int *)&f1);
int x2 = *((int *)&f2);
int x3 = *((int *)&f3);
// get address of int, cast as pointer to float, reference
float g1 = *((float *)&x1);
float g2 = *((float *)&x2);
float g3 = *((float *)&x3);
printf("%f %f %08x\n",f1,g1,x1);
printf("%f %f %08x\n",f2,g2,x2);
printf("%f %f %08x\n",f3,g3,x3);


  exponent1 = x1 & 0x7f800000;
  exponent1 >>=23;
  exponent2 = x2 & 0x7f800000;
  exponent2 >>=23;
  exponent3 = x3 & 0x7f800000;
  exponent3 >>=23;

  mantissa1 = 0x800000 | (x1 & 0x7fffff);
  mantissa2 = 0x800000 | (x2 & 0x7fffff);
  mantissa3 = 0x800000 | (x3 & 0x7fffff);

  printf("\nfor x: %x, exponent1: %03d, mantissa1: %08x\n",x1,exponent1,mantissa1);
  printf("for x: %x, exponent2: %03d, mantissa2: %08x\n",x2,exponent2,mantissa2);
  printf("for x: %x, exponent3: %03d, mantissa3: %08x\n",x3,exponent3,mantissa3);
  
  x3 = fmul(x1,x2);
  exponent3 = x3 & 0x7f800000;
  exponent3 >>=23;
  mantissa3 = 0x800000 | (x3 & 0x7fffff);
  printf("\nfmul: %x, exponent3: %03d, mantissa3: %08x\n",x3,exponent3,mantissa3);
  
}
