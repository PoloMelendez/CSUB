/* lab10.c 
 * a program that computes a^b and log_b(a^b) LOTS of times
 * values 'a' and 'b' are read in from the keyboard
 *
 * modify to optimize if base is 2 by using shift left and shift right
 */
#include <stdio.h>
#include <stdlib.h>

#define ITERATIONS 4000


/* change this flag to 0 and recompile to compare times */
#define OPTIMIZE 1 

int main(int argc, char **argv){

   if (argc < 3) {
      printf("Usage: ./lab10 <base> <exp>\n");
      exit(0);
   }
      
   unsigned long long base = atoi(argv[1]);
   unsigned long long exp  = atoi(argv[2]);
   int i, j;
   unsigned long long exponential = 1;

   exponential = 1;

   /* repeatedly compute the exponential to get substantial numbers for time */ 
   for (i=1; i<=ITERATIONS; i++) {
      exponential = 1;

      /* check if we can optimize */
      if (base==2 && OPTIMIZE) {

         /*  %0 = exponential (output); %1 = base (input);  %2 = exp (input) */
         asm ("movq $1, %0\n\t"\
              "movq %2, %%rcx\n\t" \
              "sal %%cl,%0\n\t" :"=r"(exponential):"r"(base),"r"(exp):"%rcx" ); 
      }
      else {
        /* brute force it */
        for (j=1; j<=exp; j++)  
          exponential = base*exponential;
     }
   } 
   printf("%d^%d is %llu\n",base,exp,exponential);

   int log = 0;
   unsigned long long dividend = exponential;

   /* repeatedly compute the log to get substantial numbers for time */ 
   for (i=1; i<=ITERATIONS; i++) {
      log = 0;
      dividend = exponential;

      while (dividend >= base) {
        if(base != 2 || !OPTIMIZE) {
            dividend = dividend / base;
        }
        else {
            asm("sar $1, %0\n\t" :"=r"(dividend):"0"(dividend));
        }
        log++;
      }
   }
   printf("%llu log %d is %llu\n",exponential,base,log);

   return 0;
}
