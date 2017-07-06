/* This code is derived from the CMPS 376 networking code by Marc Thomas
 *
 * The purpose of this code is to demonstrate a simple daemon (server) which
 * can handle connections from multiple clients. It spawns a child process
 * for each client it handles.
 *
 * Starting this program:
 *
 * $ s_daemon
 *
 * It will display the port number it has received from the system.
 *
 * Connecting to this program:
 *
 * $ telnet sleipnir [port_number]
 *
 * Do not use vcsend since the simple handshake has been removed from this
 * server. 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>      /* for gethostname() and pipe functions */
#include <arpa/inet.h>   /* for IP address structures and functions */

#include <signal.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/wait.h>

#define SELECT_TIMEOUT   30
#define ULIMIT          255
#define MAX_CHILDREN      4    /* Alter this to accept more/less clients */

/* This structure keeps track of the children */
struct _off
{
  int opid;     // PID of the child process handling the client
  int msgsock;  // The socket being used to communicate with this client
};

/* Make the array of children global so terminate functions can see it */
struct _off offspring[MAX_CHILDREN];

/* Also make the listening and child sockets global for the same reason */
int sock;
int child_sock;

/* This variable is used to pass environmental information to the simple
 * shell process when it is created */
char *envp_to_offspring[16];

// The CTRL-C, SIGHUP and SIGTERM signal handlers will call this function
void parent_terminate();
void kill_child();

// This function is called when the child process terminates
void child_terminate();

// This function is called when the child process is forked
void dialog_with_client();

int main(int argc, char *argv[], char *envp[])
{
  int msgsock;  /* Sockets are integer file descriptors on Linux */
  struct sockaddr_in  name, caller;

  int i, length, ret, k;

  /* Set up the environmental variables for the simple shell process */
  if(getenv("PATH") != (char *)NULL)
  {
    envp_to_offspring[0] = (char *)malloc(strlen(getenv("PATH")) + 32);
    strcpy(envp_to_offspring[0], "PATH=");
    strcat(envp_to_offspring[0], getenv("PATH"));
    envp_to_offspring[1] = (char *)NULL;
  }

  /* Set up the signal handlers */
  signal(SIGQUIT, SIG_IGN);  /* disable core dumps */
  signal(SIGCHLD, child_terminate);  /* Child has terminated */
  signal(SIGHUP, parent_terminate);  /* Requested parent to terminate */
  signal(SIGTERM, parent_terminate);
  signal(SIGINT, parent_terminate);

  /* Zero out the offspring structure array */
  for(i = 0; i < MAX_CHILDREN; i++)
  {
    offspring[i].opid = 0;
    offspring[i].msgsock = -1;
  }

  /* Create the listen socket. This is a TCP socket, so use SOCK_STREAM 
   * Exit if the socket cannot be created */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0) 
  {
    perror("daemon: socket() failed. ");
    return -1;
  }

  /* Bind the socket to an IP address and port. We will use the IP address
   * INADDR_ANY, which tells the system to assign the IP address, and the
   * port number 0, which tells the system to assign a random port number.
   *
   * First we have to set the fields in the sockaddr_in object "name" and then
   * we can call bind(). Again, exit if bind() fails. */
  name.sin_family = AF_INET;         /* TCP/IP family */
  name.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY = assigned by system */
  name.sin_port = htons(0);           /* 0 = assigned by system */
  ret = bind(sock,(struct sockaddr *)&name,sizeof name);
  if(ret < 0)
  {
    perror("daemon: bind() failed. ");
    return -1;
  }

  /* In order to use vcsend to send data to this program, we need to know
   * what port number the system just assigned this program. So this segment
   * calls getsockname() to update the sockaddr_in object "name" with the
   * system assigned values and then print that info to the screen. */
  length = sizeof name;
  ret = getsockname(sock, (struct sockaddr *)&name, (socklen_t *)&length);
  if(ret < 0)
  {
    perror("daemon: getsockname() failed. ");
    return -1;
  }

  sleep(1);  /* pause for clean screen display */
  printf("\ndaemon: process id: %d ", (int)getpid());
  printf("\ndaemon: IP address: %d.%d.%d.%d",
        (ntohl(name.sin_addr.s_addr) & 0xff000000) >> 24,
        (ntohl(name.sin_addr.s_addr) & 0x00ff0000) >> 16,
        (ntohl(name.sin_addr.s_addr) & 0x0000ff00) >>  8,
        (ntohl(name.sin_addr.s_addr) & 0x000000ff));
  printf("\ndaemon: port number: %hu", ntohs(name.sin_port));
  printf("\n");
  fflush(stdout);

  /* Now we will call listen() and wait for a client to connect. The
   * accept() function will block until there is a client or an error. 
   * This is now a loop since we will accept more than one client. */

  listen(sock, MAX_CHILDREN+1);  /* Allow MAX_CHILDREN+1 connections */
  while(1)
  {
    printf("\ndaemon: waiting for connections.\n");
    fflush(stdout);

    k = sizeof caller;
    msgsock = accept(sock, (struct sockaddr *)&caller, (socklen_t *)&k);

    /* We only reach this point when there is an error or a client. We can 
     * check the value of msgsock (the data socket) to see which has happened */

    if(msgsock < 0)
    {
      perror("daemon: accept() failed. ");
      continue;  /* Go back to listen/accept and try again */
    }

    printf("\ndaemon: Valid connection received.\n");
    printf("daemon: Searching for free child slot...\n");
    fflush(stdout);

    for(i = 0; i < MAX_CHILDREN; i++)
    {
      if(offspring[i].opid == 0) break;  /* free slot */
    }
    if(i >= MAX_CHILDREN)
    {
      printf("daemon: No free slots. Connection refused.\n");
      fflush(stdout);
      close(msgsock);  /* This will cause the client socket to close too */
      continue;  /* Go back to blocking on listen */
    }

    /* Now have a free child slot 'i'. Try to fork the child process for this
     * client request. */
    child_sock = msgsock;
    offspring[i].msgsock = msgsock;
    offspring[i].opid = fork();

    /* fork() returns a negative on failure, a positive to the parent process
     * and 0 to the child process. */
    if(offspring[i].opid < 0)
    {
      printf("daemon: error on fork(). Exiting program...\n");
      fflush(stdout);
      close(msgsock);
      close(sock);
      exit(1);
    }

    if(offspring[i].opid > 0)
    {
      printf("daemon: spawned child %d.\n", offspring[i].opid);
      fflush(stdout);
      /* The parent now goes back to blocking on listen */
      continue;
    }

    /* Child process calls the dialog_with_client function */
    if(offspring[i].opid == 0)
    {
      dialog_with_client();
      break;
    }
  }

  printf("Process %d is exiting.\n", getpid());

  return 0;
}

/* When we have been told to exit, we have to go through each child process
 * and terminate them as well. This includes closing the data socket */
void parent_terminate()
{
  int i, opid, ostatus;

  /* First turn off SIGCHLD handler since we know each child is going to exit */
  signal(SIGCHLD, SIG_IGN);

  printf("daemon: Termination signal received...\n");
  printf("daemon: Telling each child process to terminate.\n");
  fflush(stdout);

  for(i = 0; i < MAX_CHILDREN; i++)
  {
    if(offspring[i].opid > 0)  /* slot is in use */
    {
      kill(offspring[i].opid, SIGTERM);  /* Tell child to exit */
      opid = wait(&ostatus);  /* wait for child to exit */
      printf("daemon: terminated child %d\n", opid);
      fflush(stdout);
    }
  }

  printf("daemon: Closing listening socket and exiting.\n");
  fflush(stdout);
  close(sock);
  exit(0);  /* Exit without error */
}

/* This is the handler for when the child receives a SIGTERM generated by
 * parent_terminate */
void kill_child()
{
  printf("child: Received request to terminate. Closing socket.\n");
  fflush(stdout);
  close(child_sock);
  exit(0); /* end the child process */
}

/* When a child terminates, SIGCHLD is generated and we can find out which
 * child exited by calling the wait() function. We just need to clear the
 * slot used by that child so it is available to other clients */
void child_terminate()
{
  int i, opid, ostatus;
  
  opid = wait(&ostatus);
  printf("daemon: %d child exited, zeroing slot for child.\n", opid);
  fflush(stdout);

  for(i = 0; i < MAX_CHILDREN; i++)
  {
    if(offspring[i].opid == opid)
    {
      /* POSIX requires both child and parent process close the data socket
       * before it will release the TCP connection */
      close(offspring[i].msgsock);
      offspring[i].opid = 0;
      offspring[i].msgsock = -1;
    }
  }
}

void dialog_with_client()
{
  char buf[2048];
  int pipe_to_shell[2];
  int pipe_from_shell[2];
  int shell_pid;
  int ret, sret, size;

  /* The following variables are for the select() function */
  fd_set readmask, writemask, exceptmask;
  struct timeval timeout;  /* contains tv_sec and tv_usec member variables */

  size = sizeof(buf) - 1;

  /* set up signal handlers */
  signal(SIGCHLD, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGHUP, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, kill_child);

  /* set up socket */
  bzero(buf, sizeof(buf)); /* Clear out the buffer */
  sprintf(buf, "child: Preparing shell for communication...\r\n");
  send(child_sock, buf, strlen(buf), 0);

  /* Create the pipes to/from the simple shell program. The shell program
   * will have a limited number of commands the user can give since it is
   * unauthenticated. */
  ret = pipe(pipe_to_shell);
  if(ret < 0)  /* Error making pipe file descriptors */
  {
    printf("child: can not create pipe to shell.\n");
    fflush(stdout);
    goto finish;
  }

  ret = pipe(pipe_from_shell);
  if(ret < 0)  /* Error making pipe file descriptors */
  {
    printf("child: can not create pipe from shell.\n");
    fflush(stdout);
    goto finish;
  }

  /* Now fork the shell process and hook up the pipes */
  shell_pid = fork();
  if(shell_pid < 0)  /* error */
  {
    printf("child: error forking simple shell process.\n");
    fflush(stdout);
    goto finish;
  }
  else if(shell_pid == 0)  /* child process for shell */
  {
    /* First set up the pipes back to the client process */
    dup2(pipe_to_shell[0], 0);   /* Hook to stdin */
    dup2(pipe_from_shell[1], 1); /* Hook to stdout */
    close(pipe_to_shell[1]);     /* unused */
    close(pipe_from_shell[0]);   /* unused */

    /* Now spawn the simple shell program */
    ret = execle("s_sh", "s_sh", NULL, envp_to_offspring);
    if(ret < 0)  /* Very rare if executable file exists. */
    {
      printf("child: error spawning simple shell program.\n");
      fflush(stdout);
      close(pipe_to_shell[0]);
      close(pipe_from_shell[1]);
      exit(-1);
    }
  }
  else if(shell_pid > 0)  /* This is the process that handles the socket */
  {
    printf("child: simple shell is spawned, communicating with client.\n");
  }

  close(pipe_to_shell[0]);    /* unused by socket handler */
  close(pipe_from_shell[1]);

  while(1)
  {
    bzero(buf, sizeof(buf)); /* Clear out the buffer */

    /* The first step to using select is to zero out all the file descriptors
     * in each mask */
    FD_ZERO(&readmask);
    FD_ZERO(&writemask);
    FD_ZERO(&exceptmask);

    /* The second step is to set all the file descriptors you wish to watch
     * for each mask. Recall that the file descriptor for stdin is 0 and the
     * file descriptor for stdout is 1. For a socket, the file descriptor is
     * the integer returned by the socket() function. For a pipe, the file
     * descriptor is either index 0 or 1 of the pipe array. */
    FD_SET(pipe_from_shell[0], &readmask);    /* activiate pipe read */
    FD_SET(pipe_from_shell[0], &exceptmask);  /* activiate pipe exception */
    FD_SET(child_sock, &readmask);   /* watch socket for arriving data */
    FD_SET(child_sock, &exceptmask); /* watch socket for exceptions */

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
      /* No message this time so our screen doesn't get cluttered */
    }
    else if(ret < 0)  /* error or signal interupt */
    {
      printf("child: Exception on select(), exiting loop...\n");
      break;
    }
    /* Check if there was an exception on stdin */
    else if(FD_ISSET(pipe_from_shell[0], &exceptmask))
    {
      printf("child: Exception on pipe, exiting loop...\n");
      break;
    }
    /* Check if there was an exception on socket */
    else if(FD_ISSET(child_sock, &exceptmask))
    {
      printf("child: Exception on socket, exiting loop...\n");
      break;
    }
    /* Check if the simple shell program has sent us data to send to the
     * client socket */
    else if(FD_ISSET(pipe_from_shell[0], &readmask))
    {
      /* Retrieve data from the shell program */
      ret = read(pipe_from_shell[0], &buf[0], size);
      if(ret < 0)
      {
        printf("child: error reading data from simple shell.\n");
        fflush(stdout);
        break;
      }
      if(ret == 0)
      {
        printf("child: simple shell has exited.\n");
        fflush(stdout);
        break;
      }

      ret = send(child_sock, buf, ret, 0);

      if(ret < 0)
      {
        printf("child: error sending data to client. ");
        fflush(stdout);
        break;  /* Exit out of loop */
      }
    }
    /* Check if there's data on the socket */
    else if(FD_ISSET(child_sock, &readmask))
    {
      /* recv() will block until the client sends information, the client
       * closes the connection or an error occurs on the data socket. */
      ret = recv(child_sock, buf, size, 0);
      if(ret < 0)
      {
        printf("child: recv() failed. ");
        fflush(stdout);
        break;
      }
      if(ret == 0)
      {
        printf("child: client has closed connection.\n");
        fflush(stdout);
        break;
      }

      /* Send the data from the client to the simple shell process */
      sret = write(pipe_to_shell[1], buf, ret);
      if(sret < ret)  /* couldn't send all the data to the shell */
      {
        printf("child: error sending data to simple shell.\n");
        fflush(stdout);
        break;
      }
    }
  }

finish:
  /* To finish the client communication, we need to close the pipes and the
   * sockets and then exit */
  printf("child: closing pipes and socket\n");

  close(pipe_to_shell[1]);
  close(pipe_from_shell[0]);
  close(child_sock);    /* close data socket */
}
