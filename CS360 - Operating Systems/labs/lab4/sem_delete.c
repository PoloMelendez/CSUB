/* filename: sem_delete.c

  Demonstrate removing semaphores. You can also remove by:

     $ ipcs
     $ ipcrm -s {semaphore id}  

  Normally, the process that creates the semaphore set should remove it. If, as 
  in sem_test.c, you don't know which process will finish first, you need to 
  write a separate program to remove the semaphore. */ 

#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h> /* For strerror(3c) */
#include <errno.h> /* For errno */
#include <stdio.h>
#include <stdlib.h>  /* exit(3) */

int main (int argc, char **argv) 
{               
   int semid; /* semaphore identifier */
   if ( argc == 2 ) {       
      semid = atoi(argv[1]);  
      printf("semaphore id: %d\n",semid);
   } else {
      printf("USAGE: sem_rem <semaphore id>\n");
      exit(EXIT_FAILURE);
   }
                 
   if ((semctl(semid, 0, IPC_RMID)) < 0) {
       perror("semctrl IPC_RMID");
       exit(EXIT_FAILURE);
   } else {
       puts("semaphore removed");
       system("ipcs -s");
   } 
   
   exit(EXIT_SUCCESS);
}
