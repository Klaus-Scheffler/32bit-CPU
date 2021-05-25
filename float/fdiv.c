#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


//https://cs.stackexchange.com/questions/80668/simple-algorithm-for-ieee-754-division-on-8-bit-cpu

#define SIGN_MASK     (0x80000000)  /* sign bit of IEEE-754 binary32 format */
#define EXPO_MASK     (0x7f800000)  /* exponent bits of IEEE-754 binary32 */
#define MANT_MASK     (0x007fffff)  /* significand bits of IEEE-754 binary32 */
#define MANT_INTBIT   (0x00800000)  /* where integer bit would be if stored */
#define QNAN_BIT      (0x00400000)  /* distinguishes QNaN from SNaN */
#define INFTY         (0x7f800000)  /* infinity */
#define INDEFINITE    (0xffc00000)  /* special QNaN */
#define MAX_NORM_EXPO (254)         /* maximum biased exponent for normals */
#define MIN_NORM_EXPO (1)           /* minimum biased exponent for normals */
#define EXPO_BIAS     (127)         /* exponent bias IEEE-754 binary32 */
#define MANT_BITS     (24)          /* effective # of mantissa bits binary32 */
#define QUOT_BITS     (MANT_BITS+1) /* one extra bit for rounding */
#define EXPO_SHFT     (MANT_BITS-1) /* integer bit is implicit */
#define ROUND_BIT     (1)           /* mask for round bit in 25-bit quotient */
#define MANT_LSB      (2)           /* mask for significand lsb in quotient */

uint32_t fp32_div_kernel (uint32_t a, uint32_t b)
{
    uint32_t r, x, y, sign;
    uint16_t expo_x, expo_y, expo_res;
    uint8_t i, odd, rnd, sticky;

    /* extract biased exponents and sign bits */
    expo_x = (a & EXPO_MASK) >> EXPO_SHFT;
    expo_y = (b & EXPO_MASK) >> EXPO_SHFT;
    sign = (a ^ b) & SIGN_MASK;

    if ((expo_x >= MIN_NORM_EXPO) && (expo_x <= MAX_NORM_EXPO) &&
        (expo_y >= MIN_NORM_EXPO) && (expo_y <= MIN_NORM_EXPO)) { /* fast path */
divide:
        /* extract significands */
        x = (a & MANT_MASK) | MANT_INTBIT;
        y = (b & MANT_MASK) | MANT_INTBIT;
        /* compute exponent of result */
        expo_res = expo_x - expo_y + EXPO_BIAS;
        /* dividend may not be smaller than divisor: normalize */
        if (x < y) {
            x = x << 1;
            expo_res--;
        }
        /* generate quotient one bit at at time */
        r = 0;
        for (i = 0; i < QUOT_BITS; i++) {
            r = r << 1;
            if (x >= y) {
                x = x - y;
                r = r | 1;
            }
            x = x << 1;
        }
        /* OR remainder bits into sticky bit */
        sticky = (x != 0);
        printf("x: %08x sticky: %08x\n",x,sticky);
        if ((expo_res >= MIN_NORM_EXPO) && 
            (expo_res <= MAX_NORM_EXPO)) { /* normal, may overflow to infinity*/
            /* extract round and lsb bits */
            rnd = (r & ROUND_BIT);
            odd = (r & MANT_LSB) != 0;
            /* remove round bit from quotient and round to-nearest-even */
            r = (r >> 1) + (rnd & (sticky | odd));
            /* combine exponent and significand */
            r = ((uint32_t)expo_res << EXPO_SHFT) + (r - MANT_INTBIT);
        } else if ((int16_t)expo_res > MAX_NORM_EXPO) { // overflow: infinity
            r = INFTY;
        } else { /* underflow: result is zero, subnormal, or smallest normal */
            uint8_t shift = (uint8_t)(1 - expo_res);
            /* clamp shift count */
            if (shift > QUOT_BITS) shift = QUOT_BITS;
            /* OR shifted-off bits of significand into sticky bit */
            sticky = sticky | ((r & ~(~0 << shift)) != 0);
            /* denormalize significand */
            r = r >> shift;
            /* extract round and lsb bits */
            rnd = (r & ROUND_BIT);
            odd = (r & MANT_LSB) != 0;
            /* remove round bit from quotient and round to-nearest-even */
            r = (r >> 1) + (rnd & (sticky | odd));
        }
        /* combine sign bit with combo of exponent and significand */
        r = r | sign;
    } else { /* slow path */
        /* take absolute value of arguments */
        x = a & ~SIGN_MASK;
        y = b & ~SIGN_MASK;
        /* if dividend is a NaN, convert that NaN into a QNaN and return it */
        if (x > INFTY) return a | QNAN_BIT;
        /* if divisor is a NaN, convert that NaN into a QNaN and return it */
        if (y > INFTY) return b | QNAN_BIT;
        /* dividend and divisor are both zero or infinity: invalid operation */
        if (((x == 0) && (y == 0)) || 
            ((x == INFTY) && (y == INFTY))) return INDEFINITE;
        /* 0/y or x/INF -> 0 */
        if ((x == 0) || (y == INFTY)) return sign;
        /* x/0 or INF/y -> INF */
        if ((y == 0) || (x == INFTY)) return sign | INFTY;
        /* if dividend is a subnormal, normalize it */
        if (expo_x == 0) {
            expo_x++;
            do {
                a = a + a;
                expo_x--;
            } while (!(a & MANT_INTBIT));
        }
        /* if divisor is a subnormal, normalize it */
        if (expo_y == 0) {
            expo_y++;
            do {
                b = b + b;
                expo_y--;
            } while (!(b & MANT_INTBIT));
        }
        /* now that dividend and divisor are normalized, do the division */
        goto divide;
    }
    return r;
}
	

int main () {
	
float f1 = - 2000.1f;
float f2 = -0.00345f;
float f3;
int ov;
unsigned int exponent1,exponent2,exponent3,mantissa1,mantissa2,mantissa3;

f3 = f1/f2;
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
  
  x3 = fp32_div_kernel(x1,x2);
  exponent3 = x3 & 0x7f800000;
  exponent3 >>=23;
  mantissa3 = 0x800000 | (x3 & 0x7fffff);
  printf("\nfmul: %x, exponent3: %03d, mantissa3: %08x\n",x3,exponent3,mantissa3);
  
}
