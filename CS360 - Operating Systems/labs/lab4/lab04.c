/* lab04.c
 * 
 * This file is a simplified version of lab04.c from week 03. If you do not
 * understand how shared memory works, review the solution to week03 lab and 
 * the week 03 examples. Your job in lab 04 is to add semaphores to protect the 
 * critical sections of code in this file.  You know things have gone awry
 * at line 4:
 *  
 *    $ gcc lab04.c -o lab04
 *    $./lab04
 *    parent set 2, reads: 1
 *    child  set 1, reads: 4
 *    parent set 4, reads: 3
 *    child  set 3, reads: 3
 *
 * The parent and child should write the same value they set. The problem 
 * is that each process sleeps after changing shared. The sleep forces 
 * things to be out of order; while one process is sleeping the other 
 * process modifies the segment. To fix this problem the critical 3 lines of 
 * code show below needs to be protected by a semaphore.
 * 
 *       *shared=3;  
 *       sleep(2);   
 *       printf("child  set 3, reads: %d\n", *shared);
 *
 * Your job is to add the semaphore.
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sem.h>

union {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} my_semun;


int status, semid;

int main(void) {

    pid_t child;
    int shmid, n_waiting, sem_value;

    struct sembuf sops[2];
    int nsems = 1;
    semid = semget(IPC_PRIVATE, nsems, 0666 | IPC_CREAT);
    if(semid < 0) {
        perror("semget: ");
        _exit(1);
    }
    my_semun.val = 0;
    sem_value = semctl(semid, 0, SETVAL, my_semun);

    shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);

    sops[0].sem_num = 0;
    sops[0].sem_flg = SEM_UNDO;
    sops[1].sem_num = 0;
    sops[1].sem_flg = SEM_UNDO;

    int* shared = shmat(shmid, (void *) 0, 0);
    child = fork();

    if (child == 0) { /* CHILD */

        sops[0].sem_op = 0;
        semop(semid, sops, 1);


        *shared=1;   /* modify shared memory segment */
        printf("child  set 1, reads: %d\n", *shared);
        fflush(stdout);

        sops[0].sem_op = +1;
        semop(semid, sops, 1);

        sops[0].sem_op = 0;
        semop(semid, sops, 1);

        *shared=3;   /* modify shared memory segment */
        printf("child  set 3, reads: %d\n", *shared);
        fflush(stdout);

        sops[0].sem_op = +1;
        semop(semid, sops, 1);

        shmdt(shared);
        exit(0);
    }
    else { /* PARENT */

        sops[0].sem_op = -1;
        sops[1].sem_op = +1;
        semop(semid, sops, 2);

        *shared=2;  /* modify shared memory segment */
        printf("parent set 2, reads: %d\n", *shared);
        fflush(stdout);

        sops[0].sem_op = -1;
        semop(semid, sops, 1);

        sops[0].sem_op = -1;
        sops[1].sem_op = +1;
        semop(semid, sops, 2);

        *shared=4;  
        printf("parent set 4, reads: %d\n", *shared);
        fflush(stdout);

        sops[0].sem_op = -1;
        semop(semid, sops, 1);

        wait(&status); /* wait for the child to die */
        shmdt(shared);

        shmctl(shmid, IPC_RMID, 0);

        if(child != 0) {
            int ret = semctl(semid, 0, IPC_RMID);
            if(ret < 0)
                perror("Semctl: ");
            exit(EXIT_SUCCESS);
        }
        else
            exit(0);
    }
}
