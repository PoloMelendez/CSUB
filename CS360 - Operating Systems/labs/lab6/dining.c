/*  dining.c
 *  a solution to the dining philosophers problem using mutexes 
 *
 *  code makes use of the C ternary conditional operator '?', which is
 *  a shorthand if-else statement:
 *
 *   n = (flag==1) ? 10 : 20;  
 *   meaning: if flag is 1 then n is assigned value 10 otherwise value 20.
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* setup a structure to pass to the thread function */ 
typedef struct philData {
  int phil_id;  /* philosopher number */
  pthread_mutex_t *fork_lft, *fork_rgt;
  const char *name;
  pthread_t thread_id;
  int fail;
} Philosopher;

const char *nameList[] = {"Kant", "Hegel", "Plato", "Aristotle", "Decartes"};

/* use this to count times each philosopher eats to ensure no starvation */
int cnt[5];

/* thread function */ 
void *PhilPhunction(void *p) {
  int eat_cnt;
  int turn_cnt = 5;  /* number of times each phil gets to eat */
  Philosopher *phil = p;
  int failed;
  int tries_left;
  pthread_mutex_t *fork_lft, *fork_rgt, *fork_tmp;
 
  while (turn_cnt > 0) {
      fib(40);  /* slow output down a bit */
      fork_lft = phil->fork_lft;
      fork_rgt = phil->fork_rgt;
      tries_left = 2;   /* try twice before being forceful */

      do {
        /* block on left fork grab */
        failed = pthread_mutex_lock(fork_lft);

       /* if you made it here you know you have the left fork so try right */

       /* mutex_trylock() returns zero if lock acquired else error code;
        * mutex_trylock does not block - mutex_lock blocks and returns
        * id of owner of the lock  - this essentially implements "Try
        * nicely twice before forcefully grabbing it 
        */ 

        failed = (tries_left>0)? pthread_mutex_trylock(fork_rgt)
                               : pthread_mutex_lock(fork_rgt);

        if (failed) {
            pthread_mutex_unlock(fork_lft);
            fork_tmp = fork_lft;
            fork_lft = fork_rgt;
            fork_rgt = fork_tmp;
            tries_left -= 1;
        }
      } while(failed && turn_cnt);
 
      if (!failed) {
        cnt[phil->phil_id]++; 
        eat_cnt = cnt[phil->phil_id];
        printf("%9s (%hu) taking turn %d\n",phil->name,phil->thread_id,eat_cnt);
        fflush(stdout);
        pthread_mutex_unlock(fork_rgt);
        pthread_mutex_unlock(fork_lft);
        turn_cnt --;
      }
  }
}

 
int main( )  {
  int i,failed;
  /* initialize count array */
  for (i = 0; i<= 4; i++) 
     cnt[0] = 0;
  pthread_mutex_t forks[5];
  Philosopher philosophers[5];
  Philosopher *phil;
 
  for (i=0;i<5; i++) {
      /* initialize the mutexes - NULL flag means the defaults are used */
      failed = pthread_mutex_init(&forks[i], NULL);
      if (failed) {
        printf("Failed to initialize mutexes.");
        exit(1);
      }
  }
 
  for ( i = 0; i < 5; i++ ) {
      phil = &philosophers[i];
      phil->name = nameList[i];
      phil->phil_id = i;
      phil->fork_lft = &forks[i];
      phil->fork_rgt = &forks[(i+1) % 5];
      phil->fail = pthread_create( &phil->thread_id, NULL, PhilPhunction, phil);
  }

  /* join all the threads with main thread */ 
  for( i=0; i < 5; i++ ) {
      phil = &philosophers[i];
      if ( !phil->fail && pthread_join( phil->thread_id, NULL) ) {
        printf("error joining thread for %s", phil->name);
        exit(1);
      }
      else {
        printf("Joined %s who ate %d times.\n", phil->name,cnt[phil->phil_id]);
      }
  }
  return 0;
}
 

int fib( int n) {
  if (n==0) return 0;
  if (n==1) return 1;
  return fib(n-1) + fib(n-2);
}

