/* sample2.c
 * demonstrate use of pthread_join to synchronize thread behavior 
 * (the potential exists for one thread to clobber the exit code of another
 * thread - we will fix that later)
 * 
 * $ gcc -o sample2 sample2.c -lpthread
 * $ ./sample2
 *
 *   This code uses double pointers. Review here:
 *
 *   int code = 5;
 *
 *   code [ 5 ]
 *        FAB88
 *
 *   int *ptr = &code;
 *
 *   ptr [FAB88]
 *        EEFFF
 * 
 *   int foo = *(&ptr);
 *
 *   (EEFFF) -> (FAB88) ->  5
 *
 *   *(&ptr) is type-equivalent to **int
 */

#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_THREADS 5 

/* use fib to make threads do some busy work and do a sanity check on join */
int fib(int n) {
  if (n==0) return 1;
  if (n==1) return 1;
  return fib(n-1) + fib(n-2);
}

int thread_exit_code = 0;
pthread_t tids[NUM_THREADS];

/* ------- THREAD_FUNCTION -------- */
void *printIt(void *thread_num) {
  /* pointers are longs so coerce to long */
  long tnum = (long)thread_num; 

  /* do some busy work */ 
  fib(20 + ((tnum*10)%7));

  printf("hello there from thread #%ld!\n",tnum);

  /* demonstrate that thread IDs are reused */
  pthread_t self = pthread_self();

  /* this statement is always true */
  if (pthread_equal(self,tids[0])) {
     thread_exit_code = tnum+99;
  }
  else {
     thread_exit_code = tnum+10;
  }
  pthread_exit(&thread_exit_code);
} 


int main( int argc, char *argv[])
{
   int rc;
   long t;
   for (t=0; t<NUM_THREADS; t++) {
      rc = pthread_create( &tids[t], NULL, printIt, (void *)t);
      if (rc) {
         printf("ERROR; return code from pthread_create() is %d\n",rc);
         exit(-1);
      }
      printf("just created thread #%ld\n",t);

      /* wait for thread t to terminate before creating the next one 
       * when you do this the thread ID is re-used! */
      int ret_code;
      int *ptr = &ret_code;
      rc = pthread_join(tids[t], (void **)&ptr);
      if (rc) {
         printf("ERROR; return code from pthread_join() is %d\n",rc);
         exit(-1);
      }
      printf("just joined with thread #%ld: thread's exit code: %d\n",t,*ptr);
   }
    
   /* now parent can exit its own thread */
   pthread_exit(NULL);
}
