/* This code is modified from terminal.c and process.c in the CS376 code by 
 * Marc Thomas
 *
 * This program shows how to manipulate signal handlers and how to use the
 * select() function to do event-driven I/O. 
 *
 * See the following man pages for more information:
 *    man 7 signal
 *    man signal
 *    man sigprocmask
 *    man sigsetops
 *    man select
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include <signal.h>
#include <sys/select.h>

#define SELECT_TIMEOUT   30
#define ULIMIT          255

int ctrlc_hit = 0;   /* Counter of how often ^C has been caught */

/* Signal handler for CTRL-C */
void ctrlc_handler(int dummy);

/* Function to process and show the current signal mask */
void show_signals();

int main(int argc, char *argv[], char *envp[])
{
  int i, pid, bytesread, ret;
  char buf[256];

  /* The following are for blocking certain signals */
  sigset_t intrmask, oldmask;

  /* The following variables are for the select() function */
  fd_set readmask, writemask, exceptmask;
  struct timeval timeout;  /* contains tv_sec and tv_usec member variables */

  printf("\nCurrent Signal Mask: ");
  show_signals();

  /* Block SIGCHLD and SIGQUIT by first zeroing out the masks, then adding
   * SIGCHLD and SIGQUIT to the intrmask and then calling sigprocmask to
   * block those signals */
  sigemptyset(&intrmask);
  sigemptyset(&oldmask);
  sigaddset(&intrmask, SIGCHLD);
  sigaddset(&intrmask, SIGQUIT);
  sigprocmask(SIG_BLOCK, &intrmask, &oldmask);

  /* Create a signal handler for CTRL-C which is SIGINT */
  signal(SIGINT, ctrlc_handler);

  printf("\nNew Signal Mask: ");
  show_signals();

  /* Enter select() loop. This will wait for activity on specified file 
   * descriptors or timeout if no activity is detected. select() watches for:
   *     Read activity (readmask)
   *     Write activity (writemask)
   *     Exceptions (exceptmask)
   * select() is a blocking function. You will remain at the select() function
   * call until there is activity or the function times out.
   */
  pid = getpid();
  printf("\nProcess ID: %d\n", pid);
  printf("\nEnter a string at the prompt or wait for timeout.\n");
  printf("Enter just a period on a line by itself to exit.\n\n");
  printf("Try entering CTRL-C, CTRL-D and other signals at the prompt.\n");
  printf("Also open a second terminal and try sending signals via kill, such as:\n");
  printf("    kill -SIGCHLD %d\n", pid);
  printf("    kill -SIGQUIT %d\n", pid);
  printf("    kill -SIGINT %d\n", pid);
  printf("    kill -SIGHUP %d\n", pid);
  printf("    kill -SIGTERM %d\n", pid);
  printf("NOTE: Some of the above will terminate the program. Restart to try more.\n");
  printf("\n");
  while(1)
  {
    printf("enter a line> ");
    fflush(stdout);
    bzero(buf, sizeof(buf)); /* Clear out the buffer */

    /* The first step to using select is to zero out all the file descriptors
     * in each mask */
    FD_ZERO(&readmask);
    FD_ZERO(&writemask);
    FD_ZERO(&exceptmask);

    /* The second step is to set all the file descriptors you wish to watch
     * for each mask. Recall that the file descriptor for stdin is 0 and the
     * file descriptor for stdout is 1. For a socket, the file descriptor is
     * the integer returned by the socket() function */
    FD_SET(0, &readmask);    /* activiate stdin read */
    FD_SET(0, &exceptmask);   /* activiate stdin exception */

    /* The third step is to set the timeout values. This is how long select()
     * will wait for activity before giving up and returning */
    timeout.tv_sec = SELECT_TIMEOUT;    /* seconds */
    timeout.tv_usec = 0;                /* microseconds */

    /* Now we are ready to call select(). We will wait here until something
     * happens or the function times out */
    ret = select(ULIMIT, &readmask, &writemask, &exceptmask, &timeout);

    /* Now process the return value from timeout to see what happened */
    if(ret == 0)  /* timeout */
    {
      printf(" timeout!\n");
    }
    else if(ret < 0)  /* error or signal interupt */
    {
      if(errno == EINTR)
      {
        printf("Signal interupted select!\n");
      }
      else
      {
        printf("Exception on select(), exiting loop...\n");
        break;
      }
    }
    /* Check if there was an exception on stdin */
    else if(FD_ISSET(0, &exceptmask))
    {
      printf("Exception on select(), exiting loop...\n");
      break;
    }
    /* Check if there's data on stdin waiting to be read */
    else if(FD_ISSET(0, &readmask))
    {
      bytesread = read(0, buf, sizeof(buf));

      /* Check for the period to end the connection and also convert any
       * newlines into null characters */
      for(i = 0 ; i < (sizeof(buf) - 1) ; i++)
      {
        if( (buf[i] == '\n') || (buf[i] == '\0') )
          break;
      }
      if(buf[i] == '\n')
        buf[i] = '\0';  /* get rid of newline */


      if(buf[0] == '.')    /* end of stream */
      {
        printf("Exiting select() loop at user's request...\n");
        break; /* Exit out of loop */
      }
      else 
      {
        printf("You typed: %s\n", buf);
      }
    }
    fflush(stdout);
  }

  if(ctrlc_hit == 0)
  {
    printf("Please rerun the program and remember to hit CTRL-C at least once.\n");
  }

  return 0;
}

/* Increment a counter every time ^C is hit while the program is running */
void ctrlc_handler(int dummy)
{
  ctrlc_hit++;
  printf("ctrlc_handler: ^C hit %d times\n", ctrlc_hit);
}

/* Show the current signal mask and any important signals set */
void show_signals()
{
  sigset_t current_mask; /* structure to store current signal mask */
  int i;

  /* Zero out the signal mask */
  sigemptyset(&current_mask);

  /* Since the second argument is NULL, sigprocmask() will retrieve the
   * current signal mask and store it in current_mask */
  sigprocmask(SIG_BLOCK, NULL, &current_mask);

  /* The sigismember() function takes a signal set and an integer and sees if
   * that signal has been set in the current process. Look at the first 32
   * signals even though many systems have more than 32 signals. The integer
   * to symbolic name translation can be found in 'man 7 signal' */
  for(i = 1 ; i < 32 ; i++)
  {
    if(sigismember(&current_mask, i))
      printf("1");
    else
      printf("0");
  }

  /* The following are the symbolic names of some important signals. Again
   * refer to 'man 7 signal' for a list of symbolic names */
  if(sigismember(&current_mask, SIGHUP))
    printf(" HUP");
  if(sigismember(&current_mask, SIGINT))
    printf(" INT");
  if(sigismember(&current_mask, SIGQUIT))
    printf(" QUIT");
  if(sigismember(&current_mask, SIGSEGV))
    printf(" SEGV");
  if(sigismember(&current_mask, SIGALRM))
    printf(" ALRM");
  if(sigismember(&current_mask, SIGPROF))
    printf(" PROF");
  if(sigismember(&current_mask, SIGVTALRM))
    printf(" VTALRM");
  if(sigismember(&current_mask, SIGTERM))
    printf(" TERM");
  if(sigismember(&current_mask, SIGPOLL))
    printf(" POLL");
  if(sigismember(&current_mask, SIGCHLD))
    printf(" CHLD");
  if(sigismember(&current_mask, SIGUSR1))
    printf(" USR1");
  if(sigismember(&current_mask, SIGUSR2))
    printf(" USR2");

  printf("\n");
}
