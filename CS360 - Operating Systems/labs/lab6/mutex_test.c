/* mutex_test.c
 * introduction to pthread mutex primitives
 *
 * Mutex objects are intended to serve as a low-level primitive from which
 * other  thread  synchronization  functions  can  be  built. As such, the
 * implementation of mutexes should be as efficient as possible, and  this
 * has ramifications on the features available at the interface. 
 *
 * int n; flag = 1;
 * n = (flag) ? 10 : 20; # C semantics for ternary conditional operator '?':
 * meaning: if flag is T then n is assigned value 10 otherwise value 20.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
typedef struct {
    char thread_no;
    pthread_mutex_t *needle;
    pthread_t thread;
    int fail;
} thread_struct;
 
int running = 4;  /* not using this at the moment */

/*--------------------------*/
/*     the thread function  */
/*-------------------------*/
void *thread_func(void *t) 
{
    int s = (1 + (rand() % 3));
    sleep(s); /* add a little non-deterministic behavior */
    thread_struct *tptr = t;
    pthread_mutex_t *needle = tptr->needle;

    int failed = 1;
    do {
        failed = pthread_mutex_trylock(needle); /* lock else return -1 */
        if (!failed) {
            write(1,&tptr->thread_no,1);
            write(1," : got the needle!\n",20);
            fflush(stdout); 
            pthread_mutex_unlock( needle);
            running--;
        }
    } while (failed && running);

   return 0;
}
 
int main()
{
    pthread_mutex_t needle;  
    thread_struct threads[2]; /* two threads want to use the one needle */
    thread_struct * tmp;      /* pass a struct to the thread function */ 
    int i;
    int failed;

    /* create a mutex for a needle to ensure exclusive use 
       the mutex is unlocked to begin with */ 
    failed = pthread_mutex_init(&needle, NULL);
    if (failed) {
         printf("Failed to initialize mutex.");
         exit(1);
    }

    /* create two threads that will share the needle */ 
    char ch;
    for (i=0;i<2; i++) {
        tmp = &threads[i];
        tmp->needle = &needle;
        tmp->thread_no = (i==0) ? '1': '2';
        tmp->fail = pthread_create( &tmp->thread, NULL, thread_func, tmp);
    }

    sleep(2);
 
    for(i=0; i < 2; i++) {
        tmp = &threads[i];
        if ( !tmp->fail && pthread_join( tmp->thread, NULL) ) {
            exit(1);
        }
        printf("joined with thread %d\n",i);
    }
}
