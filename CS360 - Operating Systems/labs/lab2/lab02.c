//Polo Melendez
//CMPS360
//Lab02
//Parent sends signals to child halt, resume, and eventually stop action.
//Signal handler is made to intercept CTRL-C signals and manually exit the child
//and allow the parent to finish.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

static int stuff = 15;

int status;

pid_t parent,child;

//volatile sig_atomic_t got_ctrlC = 0;

void siginit_handler(int sig)
{
    /* Child */
    if(child == 0)
    {
        write(1, "\nChild: got CTRL-C. \n", 21);
        _exit(33);
    }
    /* Parent */
    else
    {
        write(1, "\nParent: got CTRL-C. \n", 22);
        //_exit(33); instead if you want handler to end parent immediately
        return;
    }
}

int main()
{
    int i = 1;

    struct sigaction sa;

    sa.sa_handler = siginit_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);

    if(sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    child = fork();

    if (child < 0) {
        perror("fork: ");
        exit(0);
    }

    /* CHILD */
    if (child == 0)   
    {
        while(1)
        {
            printf("%d\n",i);
            sleep(1);
            i++;
        }           
    }
    /* PARENT */
    else
    {
        sleep(4);
        kill(child, SIGSTOP);
        sleep(4);
        kill(child, SIGCONT);
        sleep(4);
        kill(child, SIGTERM);

        wait(&status);

        if (WIFEXITED(status)) {
            printf("child exited normally with code, status=%d\n",WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("child terminated by signal %d\n", WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("child stopped by signal %d\n", WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            printf("child continued\n");
        }  
    }
}
