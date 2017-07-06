#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

void sigint_handler(int);

int status, outfd, mqid;
pid_t child;

int main(int argc, char *argv[], char *envp[])
{
    int secs, err, ret, received, shmid;

    //foo pathname ipckey
    char pathname[128];
    getcwd(pathname,128);
    strcat(pathname,"foo");
    int ipckey = ftok(pathname, 21);

    if( argc == 2 ) {
        secs = atoi(argv[1]);
    }
    else
    {
        fprintf(stderr, "Usage:  ./fin75 nsecs\n");
        exit(EXIT_SUCCESS);
    }

    //Message Que
    struct { long type; char text[100]; } mymsg;
    mqid = msgget(ipckey, IPC_CREAT | 0666);
    if (mqid < 0) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    //Signal Handling
    sigset_t mask;
    
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = SA_RESTART;

    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaction(SIGINT, &sa, NULL);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

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
        
        //Receiving Message
        received = msgrcv(mqid, &mymsg, sizeof(mymsg), 0, 0);
        if(received > 0)
            write(outfd, mymsg.text, strlen(mymsg.text));
        write(outfd, "Child exited!\n",14);
        close(outfd);
        exit(secs);
    }

    //parent
    else
    {
        //Sending Message
        memset(mymsg.text, 0, 100);
        strcpy(mymsg.text, "Arbitrary Message!\n");
        mymsg.type = 1;
        msgsnd(mqid, &mymsg, sizeof(mymsg), 0);

        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_UNBLOCK, &mask, NULL);

        err = wait(&status);
        if(err < 0) perror("wait:\n");
        
        ret = msgctl(mqid, IPC_RMID, NULL);

        if(WIFEXITED(status))
            printf("Child exited normally with exit code %d.\n", WEXITSTATUS(status));
        else
            printf("Child killed by signal: %d.\n", WTERMSIG(status));

        exit(EXIT_SUCCESS);
    }
}

void sigint_handler(int sig)
{
    if(child == 0)
    {
        outfd = open("log", O_CREAT | O_WRONLY | O_TRUNC , 0644);
        write(outfd, "Ctrl-C received and handled by child\n", 37);
        write(outfd, "Exiting from handler with code 77\n", 34);
        close(outfd);
        _exit(77);
    }
    else
    {
        write(1, "\nCtrl-C received and handled by parent\n", 39);
        msgctl(mqid, IPC_RMID, NULL);
        return;
    }
}
