/*  lab01.c     
 *  A badly written example of fork(2)  - the child dies before the parent
 *  and the parent does not harvest the child's exit code (the child becomes
 *  a zombie). To fix it, you must add an exit in the child, a wait in the
 *  parent, and a status variable. 
 * 
 *  Compile and run with strace, logging stderr to outfile:
 *
 *    $ gcc -o lab01 lab01.c
 *    $ strace -f ./lab01  2> outfile        
 *
 *  Let it run without interruption (15 seconds) and view the outfile.
 *
 *  Run it again,
 *
 *     $ ./lab01
 *
 * and from another terminal, run top on your username:
 *
 *    $ top -u [username]    # note the 'Z' when the child dies
 *
 * Notes: The <unistd.h> header file defines the POSIX API 
 */
#include <stdio.h>
#include <stdlib.h>

static int stuff = 15;

int status;

int main()
{
   pid_t parent,child;  /* following POSIX standards use pid_t */
   int i;

   /* do this if you want to identify yourself later */
   parent = getpid(); 

   /* display info before the fork */
   printf("BEFORE FORK:\nuserid %d, effective userid %d", getuid(), geteuid()); 
   printf("pid: %d, parent pid: %d\n", getpid(), getppid());
   printf("Value of stuff at address %p is %d\n", &stuff, stuff); 

   /* clone yourself - fork returns PID of the child to the parent 
   * and 0 to the child or -1 to the parent on failure */

   child = fork();
   printf("child: %d\n", child);
   if (child < 0) {
       perror("fork: ");
       exit(0);
   }

   /* CHILD */
   if (child == 0)   
   {
      stuff = 1; 
      printf("CHILD: Value of stuff at address %p is %d\n", &stuff, stuff); 
      printf("Child Info: uid %d , euid %d ", getuid(), geteuid()); 
      printf("pid: %d ppid: %d \n", getpid(), getppid());
      for ( i=1; i <=3; i++) 
      { 
         printf("stuff: %d\n",stuff);
         stuff = stuff + 2;
         sleep(2);   
      }
      exit(33);
   }
   /* PARENT */
   else
   {
      wait(&status);
      stuff = 2;
      printf("PARENT: Value of stuff at address %p is %d\n", &stuff, stuff); 
      for ( i=1; i <=3; i++) { 
         printf("stuff: %d\n",stuff);
         stuff = stuff + 2;
         sleep(1); 
      } 
      sleep(10);
      if(WIFEXITED(status))
          printf("exited, status=%d\n", WEXITSTATUS(status));
      exit (EXIT_SUCCESS);
   }
}
