/*  cs360/Code/readwrite.c
 *  demonstrates low-level calls to read / write
 *  and file streams
 *
 *  Usage: ./readwrite <infile> <outfile> <bytes> 
 *
 *  Example:
 *     $ ./readwrite in out 20   # copy 20 bytes from in to out 
 *
 *  Demonstrates 
 *    o  use of system calls open(), read(), write()
 *    o  reading/writing to files or stdin/stdout 
 *    o  use of command-line arguments
 *
 *  To compile:   
 *      $ gcc -o readwrite readwrite.c
 *  
 *  For cmdline arguments:
 *   argv[0] holds name of executable; argv[1] holds first cmdline arg, etc.
 *   argc holds the number of cmdline args 
 * 
 *  You need to use read(),write() since printf and scanf are not atomic calls 
 *  Use printf for debugging purposes only.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEBUG 1     /* flip this if you want verbose output */
#define MAXSZ 1024 /* this is the maximum number of bytes you can read */

int stdinYES = 0;  
char *infile;   /* the input filename */
char *outfile;  /* the output filename */
int num_bytes;  /* the number of bytes you want to copy */
int infd;       /* file descriptor for infile */
int outfd;      /* file descriptor for outfile */
int errno;      /* this global is set by the kernel for any system call */

void cleanup();

int main( int argc, char * argv[])
{
  char buf[MAXSZ+1];  /* buf[] is for reads/writes - add one byte to hold \0 */
  int sz;

  if ( argc == 4) {  

     /* malloc grabs memory from heap */
     infile = (char *) malloc(sizeof(argv[1]));  

     strncpy(infile,argv[1],sizeof(argv[1]));  /* use strncpy to control size */
     if (strncmp(infile,"stdin",5)== 0) 
         stdinYES = 1;

     outfile = (char *) malloc(sizeof(argv[2]));  
     strcpy(outfile,argv[2]);

     /* convert a string to an integer */
     num_bytes = atoi(argv[3]);

     /* convert an int to a string to display it with write */
     char str[10];
     sprintf(str,"%d\0",num_bytes);
     write(1,strcat(str,"\n"),strlen(str)+1);
   }

   else {
     printf("\nUsage: ./readwrite [infile | stdin] outfile bytes\n");
     exit(EXIT_SUCCESS);
   } 

   /* quick and dirty way to prompt user */
   char chin[1];
   write(1,"Continue? [y|n]",15);
   read(0,chin,1);
   if (chin[0]=='n')
      exit(EXIT_SUCCESS);
   if (chin[0]=='y')
      write(1,"Are you sure?\n",15);

  /*  The open() system call returns a file descriptor. The various flags 
   *  control how the file is opened. O_CREAT creates the file if it does not 
   *  exist; if the file does exist O_TRUNC sets its size to 0; O_WRONLY
   *  opens the file in write only mode; 0644 sets file permissions to 644; 
   *  another useful flag is O_APPEND which appends to an existing file
   */

  /* open the output file for writing - truncate if already created */

  outfd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC , 0644);
  if (outfd < 0) {
     perror("open outfile");
     cleanup();
  }

  /* open the input file for reading unless stdin flag is set */
  if (stdinYES) 
     infd = 0;
  else {
     infd = open(infile, O_RDONLY);
     if (infd < 0) { 
        perror("open infile");
        fprintf(stderr,"Global errno is set: %d\n", errno);
        cleanup();
     }
  }

  /* display information */

  struct stat fileStat;
  if (fstat(infd, &fileStat) == -1)
      perror("fstat: ");

  printf("OutFile Name:  \t%s\n",outfile);
  printf("OutFile Handle:\t%d\n",outfd);
  printf("Infile Name:   \t%s\n",infile);
  printf("Infile Handle: \t%d\n",infd);
  printf("Infile Size:   \t%d bytes\n",fileStat.st_size);
  printf("Infile Links:  \t%d\n",fileStat.st_nlink);
  printf("Infile inode:  \t%d\n",fileStat.st_ino);
  printf("Permissions:   \t");
  printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
  printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
  printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
  printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
  printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
  printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
  printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
  printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
  printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
  printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
  printf("\n");

  if (num_bytes > MAXSZ) num_bytes = MAXSZ; 
  sz = read(infd, buf, num_bytes);  /* returns the number of bytes read */
  if (DEBUG) printf("ACTUAL BYTES READ: %d\n",sz);
  if (sz < 0) { 
     perror("read");
  } 
  buf[sz] = '\0';   /* always mark the end of a string with \0 */

  if (DEBUG) printf("\n%s\nWe just read %d bytes.\n",buf,sz);

  if (write(outfd,buf,sz) < 0)   {  /* write to output file */
     perror("write");
  } 

  if (close(infd) < 0) { 
     perror("close infile");
  } 
  if (close(outfd) < 0) { 
     perror("close outfile");
  } 

  free(infile);  /* free what you allocate to prevent memory leaks */ 
  free(outfile);

  /****************************
   * DEMONSTRATE FILE STREAMS *
   ****************************/

  FILE *in;
  char c;
  if (in = fopen("foo", "rt"))
  {
    /* use fscanf for reading */
    for (c; !feof(in); fscanf(in, "%c", &c))
             fprintf(stderr,"%c", c);
    fprintf(stderr,"\n");
    fclose(in);
  }

  exit(EXIT_SUCCESS);
}

void cleanup() {
  close(infd);
  close(outfd);
  free(infile);  
  free(outfile);
  exit(EXIT_FAILURE);
}
