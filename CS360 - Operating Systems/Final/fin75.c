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

union {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buff;
} my_semun;

struct sembuf sops[2];

int status, outfd, mqid, semid, shmid, sem_value;
char* shared;
pid_t child;

int main(int argc, char *argv[], char *envp[])
{
    int secs, err, ret, received;
    char input[100] = "";
    char buf[100] = "";
    struct shmid_ds shmbuf;

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
        fprintf(stderr, "Usage:  ./fin60 nsecs\n");
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
    //sa.sa_flags = SA_RESTART;

    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaction(SIGINT, &sa, NULL);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    //Shared Memory
    shmid = shmget(ipckey, 100, IPC_CREAT | 0666);

    //Semophore
    semid = semget(ipckey, 1, 0666 | IPC_CREAT);
    if(semid < 0) {
        perror("semget: ");
        _exit(1);
    }
    my_semun.val = 0;
    sem_value = semctl(semid, 0, SETVAL, my_semun);
    sops[0].sem_num = 0;
    sops[0].sem_flg = SEM_UNDO;
    sops[1].sem_num = 0;
    sops[1].sem_flg = SEM_UNDO;

    //Fork
    child = fork();
    if( child < 0 ) {
        perror("fork: ");
        exit(0);
    }

    //child
    if( child == 0 )
    {
        shared = shmat(shmid, (void *) 0, 0);
        sops[0].sem_op = -1;
        semop(semid, sops, 1);

        outfd = open("log", O_CREAT | O_WRONLY | O_TRUNC , 0644);
        sleep(secs);
        
        //Receiving Message
        received = msgrcv(mqid, &mymsg, sizeof(mymsg), 0, 0);
        if(received > 0)
            write(outfd, mymsg.text, strlen(mymsg.text));
        
        //Shared Memory To Log
        shmctl(shmid, IPC_STAT, &shmbuf);
        write(outfd, shared, shmbuf.shm_segsz);  

        write(outfd, "Child exited!\n",14);

        shmdt(shared);

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

        //Shared Memory
        strcpy(buf, "Type up to 80 characters followed by [Enter].\n");
        write(1,buf, strlen(buf));
        read(0,input,80);
        shared = shmat(shmid, (void *) 0, 0);

        //Semaphore
        sops[0].sem_op = 0;
        semop(semid, sops, 1);
        strcpy(shared, input);
        sops[0].sem_op = +1;
        semop(semid, sops, 1);

        err = wait(&status);
        if(err < 0) perror("wait:\n");
        
        msgctl(mqid, IPC_RMID, NULL);
        shmdt(shared);
        shmctl(shmid, IPC_RMID, 0);
        semctl(semid, 0, IPC_RMID);


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
        shmdt(shared);
        shmctl(shmid, IPC_RMID, 0);
        semctl(semid, 0, IPC_RMID);
        msgctl(mqid, IPC_RMID, NULL);
        _exit(-1);
    }
}
