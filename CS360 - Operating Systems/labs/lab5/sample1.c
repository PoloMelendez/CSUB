/* sample1.c
 * demonstrates how to create threads 
 * 
 * $ gcc -o sample1 sample1.c -lpthread
 * $ ./sample1
*/

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>  
#include <stdlib.h>

#define NUM_THREADS 5 

sigset_t zeromask;

void *printIt(void *thread_num) {

  /* do the assigment into a long since pointers are 8 bytes */
  long tnum = (long)thread_num;

  /* demonstrate process wide syscall - it prevents all threads from joining*/
  if (tnum == 0) sleep(6);

  printf("hello there from thread #%ld!\n",tnum);
  pthread_exit(NULL);
} 

int main( int argc, char *argv[])
{
   sigemptyset(&zeromask);
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;
   for (t=0; t<NUM_THREADS; t++) {
      printf("creating thread %ld\n", t);

      /* pthread_create stuffs thread id into threads array */
      rc = pthread_create( &threads[t], NULL, printIt, (void *)t);
      if (rc) {
         printf("ERROR; return code from pthread_create() is %d\n",rc);
         exit(-1);
      }
   }
   for (t=0; t<NUM_THREADS; t++) {
      if (pthread_join(threads[t], NULL) < 0) {
          perror("pthread_join: ");
      }
      printf("joined thread %ld\n", t);
   } 
   /* parent can now exit its own thread */
   pthread_exit(NULL);
}


