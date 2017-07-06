/*  /examples/week02/perror.c
 *
 * Demonstrate use of system call error handling facilities with perror
 *
 * The include file errno.h lets you call systems level error handling routines.
 * In particular, perror(), which accesses the global variable errno that holds
 * the error code of the last failed system call. See /usr/include/sys/errno.h
 * for predefined error codes. Here are a few:
 *
 *   #define EPERM  1  // not owner 
 *   #define ENOENT 2  // no such file or directory
 *   #define ESRCH  3  // no such process
 *   #define EINTR  4  // interrupted system call 
 *   #define EIO    5  // I/O error 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/file.h>

int main(void)
{
    /* demonstrate how perror works on failed calls by trying to open a
       file that does not exist */

    printf("\nDemonstrate perror().\n");

    printf("First try to open a non-existent file:\n");

    int fd = open("nofile.txt", O_WRONLY);  /* force a file open error */
    if ( fd == -1) 
        perror("file open error: "); /* this string prefaces the error msg */
    
    printf("Next try to open a file that does exist:\n");
    fd = open("Makefile", O_WRONLY);  /* open a file that does exist */
    if ( fd == -1)  
        perror("File Open Error: "); /* the string can be anything you wish */
    else  {
       printf("Makefile opened successfully.\n");
       close(fd);  /* close the file */
    } 
    return 0;
}
