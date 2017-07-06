/* /examples/week02/sigmask.c
 *
 *  Demonstrate sleep(3) and the use of SIGINT, SIGSTOP, SIGCONT and SIGTERM 
 *  and setting a signal mask to block all signals.
 *
 *  Compile
 *
 *      $ gcc -o sigmask sigmask.c
 *      $ ./sigmask   # hit CTRL-C
 *  
 *  TEST 1.
 *  Open another terminal window. Run top in that window:
 *
 *       $ top -u [username] -d .5    # -d specifies .5 second delay 
 *
 *  We want to view all CPUs. Hit 'f'. Arrow down and over to the P (Last Used 
 *  CPU) field. Hit space to toggle P on. Hit 'q'. Now view the process state 
 *  column (S). The ./sig1 and top processes move from R=running to S=sleeping. 
 *  The S is a little misleading. Your other processes are not sleeping at all
 *  times. Top is taking a sampling of CPU cycles. It looks like top or ./sig1
 *  are running continuously. Other processes on the shared CPUs are in fact in
 *  state R but the time slice is so quick that you do not see it.  
 *
 *  Return to the ./sig1 window. Hit CTRL-C. Ctrl-C generates a SIGINT signal
 *  to the process linked to the tty/keyboard that you hit Ctrl-C on. Note in 
 *  top that the process silently dies. You could also hit 'k' from top and
 *  specify to PID. Hit 'q' to exit top.
 *   
 *  TEST 2.
 *  Execute sig1 in the background. Make sure you redirect output to /dev/null
 *  so ./sig1 doesn't grab the terminal. In this run you will manually generate
 *  some signals with the kill shell command to your background process. 
 *
 *  Note what happens to the status column (S) in top. The status will change 
 *  from S (sleep) to T (traced or stopped), then back to S and then disappear.
 *
 *       $ ./sig1 > /dev/null &     
 *       [1] 32359                  # (your PID will differ from 3259) 
 *
 *       $ kill -STOP 32360         # stop but do not terminate the process
 *       $ kill -CONT 32360         # continue the process
 *       continued
 *       $ kill -TERM 32360         # terminate the process
 *       [1]+  Terminated ./sig1
 *       $
 *
 */

#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   /* this header file is for the POSIX interface */
#include <string.h>

int main(int argc, char*argv[])
{
    
    /* block all signals just for fun */
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    printf("Hit Ctrl-C\n");
    sleep(5);  /* note that nothing happens for 5 seconds! */

    char buf[100];
    strcpy(buf,"Made it inside the loop.\n");
    while (1) {

       write(1,buf,strlen(buf));  /* use write - printf is buffered */
       fib(40);   /* do some busy work for a few seconds */
       
       /* now unblock signals */
       sigprocmask(SIG_UNBLOCK, &mask, NULL);  /* now Ctrl-C is processed */
    }

    printf("This code is never executed.\n");
    return 0;
}

/* handy busy-work routine */
int fib(int n) {
  if (n==0) return 1;
  if (n==1) return 1;
  return fib(n-1) + fib(n-2);
}
