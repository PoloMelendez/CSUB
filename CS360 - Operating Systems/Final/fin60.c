#include <stdio.h>
#include <stdlib.h>
#include <erno.h>

int status;

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

    child = fork();
    if( child < 0 ) {
        perror("fork: ");
        exit(0);
    }

    //child
    if( child == 0 )
    {
        sleep(secs);
        exit(secs);
    }

    //parent
    else
    {
        err = wait(&status);
        if(err < 0) perror("wait:\n");

        if(WIFEXITED(status))
            printf("Child exited with exit code %d.\n", WEXITSTATUS(status));
        else
            printf("Child killed by signal: %d.\n", WTERMSIG(status));

        exit(EXIT_SUCCESS);
    }
}

