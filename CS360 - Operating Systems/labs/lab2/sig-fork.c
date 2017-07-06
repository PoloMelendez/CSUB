/*   /examples/week02/sig-fork.c     
 *
 *   Demonstrate using strace to trace signals from child to parent: 
 *      SIGCHLD (a signal from child to parent)
 *      SIGTERM (this exit signal can be blocked unlike kill -9) 
 *
 *   The child kills its parent, becomes an orphan and is inherited by init
 *   (PID 1). The child then tries to kill init and then finally kills itself.
 *   Note that the orphan cannot be stopped by Ctrl-C since init blocks Ctrl-C.
 *
 *   NOTE: THIS IS AN EXAMPLE OF A BADLY BEHAVING PROGRAM. 
 *
 *   Compile 
 *
 *     $ gcc -o sig-fork sig-fork.c
 *
 *   Trace the signals passed between the kernel and the processes:
 * 
 *    $ strace -e trace=signal -f ./sig-fork  2> out    # -f means trace child
 *    $ vi out   # view the strace output file
 */
 
#include <sys/wait.h>    /* make sure you include this system header */
#include <stddef.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>      /* for perror */
 
static int secs = 1;   
pid_t me;
 
int main()
{
    int i, err, status;
    pid_t parent, child;  /* pid_t is POSIX interface */
 
    /* if you want to ignore signals from your children - signal() is a quick 
     * and dirty way;  we are not doing this because we want the child to send
     * a signal to kill its parent. AND, signal is no longer the accepted
     * method to do signal handling - use sigaction instead! 
     * FYI: you may see signal in legacy system code but do NOT USE IT!
     *
     * signal(SIGCHLD, SIG_IGN);  
     */ 

     parent = getpid(); /* the child and parent will both see this value */  
    /* fork a child - fork returns PID of the child to the parent 
     *  and 0 to the child or -1 to the  parent on failure 
     */
    child = fork(); 
 
    /* I AM THE PARENT */
    if (child != 0) {
       me = getpid();   /* variable me is in user space of parent & child */
       printf("Parent: %d\n",me);
       printf("I am the parent and I am going into an infinite sleep...\n");
       while (1) {
          sleep(secs);
       }
       printf("I am the parent and I never make it here...\n");
       exit (EXIT_SUCCESS);
    }
 
    /* I AM THE CHILD */
    if (child == 0)  { 
       me = getpid();   /* child has its own copy of variable me */ 
       printf("Child: %d\n",me);
       fflush(stdout);   /* flush the buffer for stdout */

       write(1,"Child sending SIGUSR1 to parent.\n",33);  
       err = kill(parent,SIGUSR1);  
       if (err < 0) {
           perror("Error sending SIGUSR1\n");
       }

       write(1,"Child sending SIGTERM to parent.\n",33);
       err = kill(parent,SIGTERM);  
       if (err < 0) {
           perror("Error sending SIGTERM\n");
       }

       sleep(1);

       /* child is inherited by init  - */
       parent = getppid();   /* get parent PID */
       printf("\nMy new parent is: %d\n", parent);
       fflush(stdout);   /* you must flush the buffer for stdout */

       err = kill(parent,SIGTERM);
       if (err < 0) 
          perror("SIGTERM ");

       /* child goes into infinite sleep - waiting for Ctrl-C */
       printf("I am the child and I am going into an infinite sleep...\n");
       printf("Hit Ctrl-C or kill -9 %d to terminate me!\n",me);
       while (1) 
          sleep(1);
 
       exit(0);
    }
}
