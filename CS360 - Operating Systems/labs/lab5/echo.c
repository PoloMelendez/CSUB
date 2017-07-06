/* filename: echo.c
 *
 *  Demonstrate classic producer/consumer problem; the parent thread (the 
 *  producer) reads one character at a time from a file into a shared variable
 *         volatile int ichar;  
 *  The consumer copies the value to this variable
 *         volatile int ochar = 'a'; 
 *  The consumer then echos it to stdout and to a file. With no synchronization,
 *  the consumer will stutter or completely miss characters. The output is also
 *  not deterministic - output varies across executions. 
 *
 *  This is a problem of concurrency. When two threads access shared resources 
 *  without synchronization it is a race condition as to which thread gets 
 *  there first or next. Without synchronization, the consumer may read a value
 *  multiple times (empty buffer) or the producer overwrite a value before the 
 *  consumer had a chance to read it. 
 *
 *      $ gcc -o echo echo.c -lpthread   # link in POSIX pthread library  
 *      $ ./echo 
*/

#include <pthread.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/sem.h>

union {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} p_sem, c_sem;

int retval, sem_id;

/* thread function prototypes */
void *consumer(void * dummy); 

   /*  A note on the differences between forks and threads. Variables inside 
   main and above the fork are visiable to a forked child but not shared after 
   the fork. Threads have the same visibility as functions; a thread has no 
   visability unless the variable is global. These globals are not only 
   visible but shared by all threads since threads share user space.
   */

const int LIMIT = 50;      /* for testing just read 50 chars from the file */
int count = 0;
volatile int ichar = 'i';  /* incoming char */
volatile int ochar = 'o';  /* outgoing char */

struct sembuf psops[2];
struct sembuf csops[2];
int pSem_value, cSem_value;

int main(void) 
{
    c_sem.val = 0;
    p_sem.val = 1;
    sem_id = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);
    pSem_value = semctl(sem_id, 0, SETVAL, p_sem);
    cSem_value = semctl(sem_id, 1, SETVAL, c_sem);
    
    psops[0].sem_num = 0;
    psops[0].sem_flg = SEM_UNDO;
    psops[1].sem_num = 0;
    psops[1].sem_flg = SEM_UNDO;

    csops[0].sem_num = 1;
    csops[0].sem_flg = SEM_UNDO;
    csops[1].sem_num = 1;
    csops[1].sem_flg = SEM_UNDO;


   FILE * inf;
   pthread_t C;  /* main is the producer; C is the consumer */
   int dummy;
   /* Create a thread that executes this function:
          void *consumer(void * dummy);  */
   if (  pthread_create(&C, NULL,  consumer, (void *)&dummy) != 0 )
      printf("Error creating thread\n");

   ochar = 'p';
   pid_t tid = syscall(SYS_gettid);      
   printf("producer thread pid: %d tid: %d \n",getpid(), tid);
   inf = fopen("poem.in", "r");
   if (inf == NULL ) {
      fprintf(stderr, "error opening input file.\n");
      exit(1);
   }
   /* PRODUCER */
   while (1)
   {
       psops[0].sem_op = -1;
       semop(sem_id, psops, 1);

       ochar = fgetc(inf);  /* this line is the critical code */

       csops[0].sem_op = +1;
       semop(sem_id, csops, 1);

       if (count == LIMIT)
           break;
        count++;
   }
   if ((pthread_join(C, (void*)&retval)) < 0) 
      perror("pthread_join");
   else  
      printf("joined thread1 exit code: %d\n",retval);
   fclose(inf);
   
   semctl(sem_id, 0, IPC_RMID);

   exit(EXIT_SUCCESS);
   return 0;
}

/* CONSUMER THREAD */
void *consumer(void *dummy)
{
   pid_t tid = syscall(SYS_gettid);      
   printf("consumer thread pid: %d tid: %d \n",getpid(), tid);
   FILE * outf;
   outf = fopen("poem.out", "w");
   if (outf == NULL) {
      fprintf(stderr, "error opening output file.\n");
      exit(1);
   }
   while (1)
   {
       csops[0].sem_op = -1;
       semop(sem_id, csops, 1);

       putc(ochar,stdout); 
       fflush(stdout);
       fputc(ochar,outf);

       psops[0].sem_op = +1;
       semop(sem_id, psops, 1);

       if (count == LIMIT) 
           break;
   }
   putc('\n',stdout); 
   fflush(stdout);
   fclose(outf);  

   pthread_exit((void*)EXIT_SUCCESS);
}
