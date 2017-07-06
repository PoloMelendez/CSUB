/* foo.c
 * let gcc produce the inline assembly that you need
 *
 * $ gcc -S foo.c
 * $ vi foo.S
 * $ gcc -o foo foo.c
 * $ ./foo
 * log 32 is 5.
 *
 */

#include <stdio.h>

int main()
{
  unsigned long long r = 1;
  unsigned long long num = 1;

  /* the compiler uses salq (shift arithmetic left) if 2^n operand is 
   * hard-coded 
   */  
  num = r * 8;

  /* the compiler uses shrq (shift right) if 2^n divisor is hard-coded */  
  num = r / 8;
  
  /* test shift left c syntax */
  int i = 5;
  num = num << i;

  /* use shifts to compute lg n */
  num = 32;
  int dividend = num;
  int log = 0;
  while (dividend > 1) {
    dividend = dividend >> 1;
    log++;
  }
  printf("log %llu is %d.\n",num,log); 

  return 0;
}


