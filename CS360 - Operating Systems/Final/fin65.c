#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

void sigchild_handler(int);

int status, outfd;

int main(int argc, char *argv[], char *envp[])
{
    int secs, err;

    if( argc == 2 ) {
        secs = atoi(argv[1]);
    }
    else
    {
        printf("Usage:  ./fin60 nsecs\n");
        exit(EXIT_SUCCESS);
    }

    pid_t child;

    //Signal Handling
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    child = fork();
    if( child < 0 ) {
        perror("fork: ");
        exit(0);
    }

    //child
    if( child == 0 )
    {
        outfd = open("log", O_CREAT | O_WRONLY | O_TRUNC , 0644);
        sleep(secs);
        write(outfd, "Child exited!\n",14);
        close(outfd);
        exit(secs);
    }

    //parent
    else
    {
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_UNBLOCK, &mask, NULL);

        err = wait(&status);
        if(err < 0) perror("wait:\n");


        if(WIFEXITED(status))
            printf("Child exited normally with exit code %d.\n", WEXITSTATUS(status));
        else
            printf("Child killed by signal: %d.\n", WTERMSIG(status));

        exit(EXIT_SUCCESS);
    }
}

