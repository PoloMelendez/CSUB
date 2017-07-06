#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

int status;

void siginit_handler(int);

int main(void) {

    int ret, received;
    char pathname[128];
    getcwd(pathname,128);
    strcat(pathname,"/foo");    /* make sure file foo is in your directory*/

    int ipckey=ftok(pathname, 21);   
    int shmid; /* shared memory segment id - visible to both parent and child */

    shmid = shmget(ipckey, sizeof(int), IPC_CREAT | 0666); 

    pid_t child;

    //Create queues
    key_t ipckey2;
    int mqid, myNum;
    struct { long type; char text[100]; } mymsg;

    myNum = 18; /* arbitrary number + pathname creates a unique key */

    getcwd(pathname,128);
    strcat(pathname,"/foo");    /* make sure file foo is in your directory*/
    printf("%s \n",pathname);
    ipckey2 = ftok(pathname, myNum);

    if (ipckey2 == -1) {
        perror("ipckey error: ");
        exit(EXIT_FAILURE);
    }
    else
        printf("My ipc key is %d\n", ipckey2);

    mqid = msgget(ipckey2, IPC_CREAT | 0666);

    if ( mqid < 0 ) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    printf("Message queue identifier is %d\n", mqid);

    //Signal Handling
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);

    child = fork(); 
    if (child == 0) { /* child  */

        struct sigaction sa;
        sa.sa_handler = siginit_handler;
        sa.sa_flags = SA_RESTART;
        sigaction(SIGUSR1, &sa, NULL);

        int* shared = shmat(shmid, (void *) 0, 0);

        printf("\t\tchild  pointer: %p\n", shared); 

        *shared=1;
        printf("child  set 1, reads: %d\n", *shared);
        *shared=3;
        sleep(2);
        printf("child  set 3, reads: %d\n", *shared);

        //message
        received = msgrcv(mqid, &mymsg, sizeof(mymsg), 0, 0); /* blocks on queue */
        if (received > 0) {
            printf("%s Bytes: %d\n", mymsg.text, received);
        }

        //remove queue
        ret = msgctl(mqid, IPC_RMID, NULL);
        if (ret < 0)
            perror("msgctl: ");

        shmdt(shared);
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        exit(0);

    } else { /* Parent */

        kill(child, SIGUSR1);
        int *shared = shmat(shmid, (void *) 0, 0);

        printf("\t\tParent pointer: %p\n", shared); 
        *shared=2;
        printf("parent set 2, reads: %d\n", *shared);
        sleep(1);
        *shared=4;  
        printf("parent set 4, reads: %d\n", *shared);
        sleep(5);

        //message
        memset(mymsg.text, 0, 100);
        strcpy(mymsg.text, "Hello, world!");
        mymsg.type = 1;
        msgsnd(mqid, &mymsg, sizeof(mymsg), 0);
        memset(mymsg.text, 0, 100);
        strcpy(mymsg.text, "Hello, again!");
        mymsg.type = 1;
        msgsnd(mqid, &mymsg, sizeof(mymsg), 0);

        wait(&status);
        shmdt(shared);
        shmctl(shmid, IPC_RMID, 0);
    }
}

void siginit_handler(int sig)
{
    if(sig == SIGUSR1)
        write(1, "\nI finally got the signal!\n",27);
    return;
}
