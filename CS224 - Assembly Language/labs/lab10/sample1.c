/* sample1.c
 * demonstrate gcc inline x86 assembly
 * this program assigns out the value of in
 * 
 * general form:
 * asm(code : output operand list : input operand list : clobber list); 
 */

#include <stdio.h>

int main(){

  unsigned int in = 99;  /* this is our input variable - referred to by %1 */ 
  unsigned int out = 0;  /* this is our output variable - referred to by %0 */

  /* gcc uses ATT syntax so 'mov src dst' moves src to dst 
   * %0 refers to the output operand
   * %1 refers to the input operand
   * "r" means use any available GPR for reading
   * "=r" means use any available GPR for writing
   */
  asm ("mov %1, %%eax;  \
             mov %%eax, %0;"  /* %0 refers to output variable */
             : "=r" (out)   /* output */
             : "r" (in)     /* input */
             : "%eax"     /* cobbered register -tells gcc eax is overwritten */
             );

  printf("in: %ld out: %ld\n", in, out);

  /* in this second example there is no input register and there is 1 output */
  asm ("mov $5, %%eax; \
        add %%eax, %%eax; \
        mov %%eax, %0;" : "=r" (out) : /*empty input*/ : "%eax" );

  printf("5 + 5 = %ld.\n", out);

  /* in this third example there are two input registers and 1 output */
  asm ("mov $5, %%eax; \
        add %%eax, %%eax; \
        mov %%eax, %0;" : "=r" (out) : /*empty input*/ : "%eax" );

  printf("5 + 5 = %ld.\n", out);


  /* in this example we store the value in %0 to memory */
  int x;
  int *loc = &x; 
  /* sidt is store content of interrupt descriptor table register to %0 */
  asm("sidt %0\n" : /* empty output*/ :"m"(loc));
  printf("value in memory is: %d.\n", x);

  return 0;

}
