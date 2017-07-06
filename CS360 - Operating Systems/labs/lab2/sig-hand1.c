/* /examples/week02/sig-hand1.c
* 
*  A gentle introduction to the use of signal handling.
*  Uses signal handling to trap CTRL-C (SIGINT signal).
* 
*  Compile and execute. Hit CTRL-C. Note that with this signal handler, CTRL-C 
*  breaks you out of the loop but allows you to continue executing code 
*  until you safely exit.
*
*  Some notes: the use of keyword 'volatile' tells the compiler that the value
*  of the variable must be grabbed from its memory address and not from cache. 
*  This protects the value from strange things happening during a signal 
*  handler. The 'sig_stomic_t' type is an integer type that is accessed to 
*  read or write in a single atomic action (hence cannot be interrupted) 
*
*  The 'sa_mask' in the sigaction struct (shown below) is the mask of signals
*  that should be blocked during execution of the signal handler.
*
*          struct sigaction {
*               void     (*sa_handler)(int);
*               void     (*sa_sigaction)(int, siginfo_t *, void *);
*               sigset_t   sa_mask;
*               int        sa_flags;
*               void     (*sa_restorer)(void);
*         };
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/file.h>

/* use 'volatile sig_atomic_t' type for data you want to access in handlers */
volatile sig_atomic_t got_ctrlC = 0; 
sigset_t zeromask;

/* predefined signal handler function prototype is: void sigint_handler(int) */
void sigint_handler(int sig)
{
  got_ctrlC++;   /* the handler increments the flag to break out of loop */
}

int main(void)
{
   /* this sets stdout to not buffer */
   setvbuf(stdout, NULL, _IONBF, 0);

   struct sigaction sa; /* sa is a variable name of type struct sigaction */

   sa.sa_handler = sigint_handler;  /* this points to your handler */

   sa.sa_flags = SA_RESTART; /* restart interrupted system calls - set to 0
                              * if you do not want to do this */

   sigemptyset(&sa.sa_mask);  /* sa.sa_mask is the mask used in handler - you 
                               * do this manually if you use the signal syscall
                               * rather than sigaction; the mask is returned to 
                               * original value upon exit of the handler;
                               * an empty set mask means do not block any
                               * signals */

   sigaddset(&sa.sa_mask, SIGINT); /* block CTRL-C while in handler */

    /* link the signal handler to the SIGINT signal */ 
   if (sigaction(SIGINT, &sa, NULL) == -1) {  /* failed calls return -1 */ 
       perror("sigaction");   /* perror grabs the error and displays it */ 
       exit(1);
   }

   /* put yourself into sigsuspend(2) waiting for Ctrl-C */
   sigemptyset(&zeromask);   /* empty mask - do not block any signals */
   printf("HELP! I'm stuck in an infinite loop. Please hit Ctrl-C!\n");
   while (1) {
      sigsuspend(&zeromask); /* any signal will break you out of sigsuspend 
                              * but only sigint will break you out of the loop
                              */
      if (got_ctrlC)
          break;
   }
   printf("Thanks. I needed that. I safely made it outside the loop!\n");
   return 0;
}
