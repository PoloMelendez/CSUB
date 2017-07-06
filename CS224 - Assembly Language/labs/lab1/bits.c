/* filename: bits.c
   a program to demonstrate how integers are stored
 
   note: GNU doesn't seem to use __UINT_MAX__ - worry about this bug later...
   the max size of unsigned int is  __INT_MAX__  

    $ gcc bits.c        */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void bitPrint(unsigned int n, int numBits);
void bitPrintf(float * n, int numBits);
char *btostr(unsigned int n, int numBits);
char *btostrf(float n, int numBits);

int main()
{
  int num2;
  int num;
  float x;
  int u;
  u = sizeof(int);
  printf("Size of int is: %d bytes\n",u); 
  u = sizeof(float);
  printf("Size of float is: %d bytes\n",u); 
  u = sizeof(unsigned long long);
  printf("Size of unsigned long long is: %d bytes\n",u); 
  u = sizeof(unsigned int);
  printf("Size of unsigned int is: %d bytes\n",u); 
  u = sizeof(long int);
  printf("Size of long int is: %d bytes\n\n",u);

  x = 8.0;
  printf("\nfloat x = %f\n", x);
  bitPrintf(&x, 32);
  x = .15625;
  printf("\nfloat x = %f\n", x);
  bitPrintf(&x, 32);
  x = 5.1;
  printf("\nfloat x = %f\n", x);
  bitPrintf(&x, 32);
 
  num = __INT_MAX__;
  num2 = num + 1;
  printf("\n\nnum = __INT_MAX__: %d\n",num);
  bitPrint(num, 32);

  // will flip to the largest negative number possible
  printf("\nnum2 = num+1: %d \n", num2);
  bitPrint(num2, 32);
  printf("\n ");
  printf("\nnum + num2: %d \n",num + num2);


/* results:
num = __INT_MAX__: 2147483647
01111111111111111111111111111111 
num2 = num+1: -2147483648 
10000000000000000000000000000000 

*/


  // negative integers are stored in two's complement - 
  // e.g. the logical complement of the positive number + 1
  num2 = -__INT_MAX__; 
  printf("\nnum2=-_INT_MAX: %d \n",num2);
  bitPrint(num2, 32);
  printf("\n ");

  num2 = 3;
  printf("\nint=3: %d \n",num2);
  bitPrint(num2, 32);
  num2 = -3;
  printf("\nint=-3: %d \n",num2);
  bitPrint(num2, 32);
  printf("\n ");

}

void bitPrintf(float * fptr, int numBits)
{
  /* bitwise operators require integral operands, so do 
     some casting to print floating point numbers */ 

    int * iptr;
    iptr = (int *)fptr;
    char *tmp;
    printf("%s ", tmp = btostr( *iptr, numBits));
    free(tmp);
}

char *btostr(unsigned int n, int numBits)
{
    unsigned int m;
    char *result, *t;
    t = (char *) malloc(numBits+1);
    result = t;

    m = 1<<(numBits-1);
    while(m) {
       *t++ = (n & m) ? '1' : '0';
       m = m >> 1;
    }
    *t = '\0';
    return result;
}

void bitPrint(unsigned int n, int numBits)
{
    char *tmp;
    printf("%s ", tmp = btostr(n, numBits));
    free(tmp);
}

/* sample output:

num = __INT_MAX__: 2147483647
01111111111111111111111111111111
num = num++: -2147483648
10000000000000000000000000000001

num2=-_INT_MAX: -2147483647
10000000000000000000000000000001

int=1: 1
00000000000000000000000000000001
int=-1: -1
11111111111111111111111111111111

Why twos complement?

Easy to compute (just flip the bits)
and easy to perform + operation on
(no need to check sign bit):

1 + -1:
0 0 0 0 0 0 0 1 = 1
1 1 1 1 1 1 1 1 = -1
0 0 0 0 0 0 0 0 = 0

In the IEEE standard for floating point numbers,
numbers are represented in binary. The exponent is 
biased by 127 (2^7 - 1). The significand s is 1 + 
fraction, thus 1 <= s <= 2. 

Examples.
x = 8.000000
8 is 2^3.

Sign exponent  fraction
     76543210 
0    10000010  00000000000000000000000
-    --------  -----------------------
1     8 bits    23 bits

The exponent is 130 - 127 = 3 
s is 1.0
This number is 1.0 * 2^3.

x = 8.250000
0    10000010  00001000000000000000000
The exponent is 130 - 127 = 3.
The fraction is 1/32 = 0.03125
This number is 1.03125 * 2^3.

x = .15625;
0    01111100  01000000000000000000000
-    --------  -----------------------
exponent = 64+32+16+8+4 = 124; 124 - 127 = -3.
fraction: .01 (binary) = .25 (decimal)
This number is 1.25 * 2^-3 = 0.15625

float x = 5.100000
sign exponent  fraction
0    10000001  01000110011001100110011

exponent 129 - 127 = 2.
fraction (an infinite recurring binary number) is .275 decimal
This number is 2^2 * 1.275 = 5.1

*/
