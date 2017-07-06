					
				/* pipesend: this program opens a pipe
		and forks(Unix)/spawns(NT) an offspring process. The parent 
		closes the RECEIVE end of the pipe and the offspring closes 
		the WRITE end of the pipe. The parent then prompts for a 
		stream of information (entered line by line) from the 
		terminal which is then sent to the offspring process through 
		the pipe. The offspring process also uses the screen to 
		display the stream of characters it receives. The default 
		invocation of this program is:

		$ pipesend

		When a line beginning with "." is typed, the parent 
		sends a SIGTERM(Unix)/SENTINEL(NT) to the offspring 
		telling it to exit gracefully. The parent then calls
		WAIT_PID until it receives confirmation of the exit
		(otherwise we would leave a "Zombie" process). The
		parent then exits.

			Note that one can use spawn() (albeit with
		complications) in place of Unix fork()/exec() for all
		descriptors which are RECOGNIZED by the Windows NT 
		kernel (unfortunately network sockets are NOT 
		recognized by the Windows NT kernel -- they exist only
		in the mind of WINSOCKx.DLL). This program has
		extensive conditional code (not handled by "header.h")
		essentially because of major operating system differences
		with regard to process control.

		Makefiles are provided as follows:

		makefile.unx		DecUnix (alpha) or Ultrix (VAX)
		makefile.lnx		Red Hat Linux
		makefile.vc5		Windows NT with Winsock2 DLL
		makefile.sun	 	Sun Solaris 7/8

					Marc Thomas		*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>		/* for signal() */
#include <errno.h>

#if defined(_WIN95) || defined(_WINNT)

#include <io.h>			/* for _pipe */
#include <fcntl.h>		/* for O_BINARY */
#include <process.h>
#define	CREATE_PIPE(x)	_pipe(x,1024,O_BINARY)
#define	WAIT_PID(p,s,o)	_cwait(s,p,o)
#define	WAIT_FLAGS	_WAIT_CHILD
#define	EXIT_PLEA	"SENTINEL"
#define	DESC_EXTERNALIZE(d,s,b)	itoa(d,s,b);
#else

#include <sys/wait.h>		/* for wait() */
#define	CREATE_PIPE(x)	 pipe(x)
#define	WAIT_PID(p,s,o)	 waitpid(p,s,o)
#define	WAIT_FLAGS	0
#define	EXIT_PLEA	"SIGTERM"
#define	DESC_EXTERNALIZE(d,s,b)	
#endif

/* ------------------------------------------------------------------ */

#include "header.h"	/* OUR header which handles MOST of the
			WinSock/32-bit Unix/64-bit Unix Differences */
#include "startup.h"	/* globals for main module set by the
			code startup.c */
#include "diagnost.h"	/* OUR dump_xxxx() routines and other
			diagnostics */

/* ------------------------------------------------------------------ */

#define	BUFFER_SIZE	2048

int	pipedesc[2];	/* This is an array for two pipe descripters.
			   pipedesc[0] is the file descripter for the
			   READing side of the pipe.
			   pipedesc[1] is the file descripter for the
			   WRITing side of the pipe.  */
int	readchan;	/* global variable for nice_exit() */

			/* function Prototypes */
#if defined(_NOPROTO)

int	nice_exit();
#else

void	nice_exit(int dummy);
#endif

/* ------------------------------------------------------------------ */
#if defined(_NOPROTO)
int main(argc, argv, envp)
int	argc;	
char	*argv[],*envp[];
#else
int main(int argc, char *argv[], char *envp[])
#endif
	{
	int	i, length,size, am_offspring;
	int	opid, ostatus, pret;
	char	chrline[80], buf[BUFFER_SIZE];
	int	writechan, numread, numwritten;
	int	pipedesc[2];	/* This is an array for two pipe 
			descripters.  pipedesc[0] is the file descripter 
			for the READing side of the pipe.  pipedesc[1] 
			is the file descripter for the WRITing side of 
			the pipe.  */
#if defined(_WIN95) || defined(_WINNT)
	char	readchan_string[64];
#endif

#include "startup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix startup. Main modules
			ALSO require the inclusion of 
				header.h
				startup.h */

/* INITIALIZE AND CHECK COMMAND LINE ********************************** */
	am_offspring = 0;
	size = sizeof(buf) - 1;		/* leave room for '\0' */
	for (i = 1 ; i < argc ; i++)
		{
				/* this is only needed by Windows NT.
			Since we lack a fork() we have to use spawn()
			and pass BOTH the fact that the spawned process 
			is an OFFSPRING and the pipe READ handle via the 
			command-line arguments, e.g.

			pipesend -child {pipe read descriptor}
								 */
		if (strcmp(argv[i], "-child") == 0)
			{
			i++;
			if (i >= argc)
				{
				fprintf(stderr,
		"\nerror: expected pipe READ handle after \"-child\" \n");
				fflush(stderr);
				return(-1);
				}
			readchan = atoi(argv[i]);
			am_offspring = 1;
			break;
			}
		}  /* end of for */

/* OPEN PIPE BEFORE fork()/spawn() ********************************** */
	if (!am_offspring)
		{
		pret = CREATE_PIPE(pipedesc);  /* pipedesc[0] = read, 
						  pipedesc[1] = write */
		if (pret < 0)
			{
			perror("pipe: ");
			return(-2);
			}
		readchan  = pipedesc[0];       /* copy pipe descriptors */
				/* convert to string using base 10 (this 
				is ONLY needed by Windows NT) for 
				spawnl() below) */
		DESC_EXTERNALIZE(readchan,readchan_string,10)
		writechan = pipedesc[1];
		}

/* FORK/SPAWN OFFSPRING RECEIVER PROCESS ****************************** */

#if defined(_WIN95) || defined(_WINNT)

	if(!am_offspring)
		{
				/* parameters for spawnl() are as
				follows:

				mode_integer	usually P_NOWAIT
				command_name	usually argv[0] with
						optional path
				argv[0]		first argument
				argv[1]		second argument
				..
				argv[n-1]
				NULL		end arguments with a
						NULL pointer */
		fprintf(stdout,
  "\nexecuting spawnl(P_NOWAIT,\"%s\",\"%s\",\"%s\",\"%s\",NULL).. ",
				argv[0], argv[0], "-child",
				readchan_string);
		fflush(stdout);
						/* we could also use
				"spawnlp" here. the difference is that
				spawnlp would also look in $PATH for
				command_name */
		opid = spawnl(P_NOWAIT, argv[0], argv[0], "-child",
			readchan_string, NULL);
		if (opid < 0)
			{
			fprintf(stderr,
  "\nerror: spawnl(P_NOWAIT,\"%s\",\"%s\",\"%s\",\"%s\",NULL) failed.\n",
				argv[0], argv[0], "-child", 
				readchan_string);
			fflush(stderr);
			return(-3);
			}
		goto parent;			/* we are the parent,
				the offspring has been spawned, it
				started execution AT LINE 0, and then
				started parsing its command line 
				arguments for

				-child {pipe read descriptor} */
		}
	else
		{
		printf("\nReceiver: process id: %d, waiting at pipe. \n",
				getpid());

			/* read loop */
		while(1)
		  {
		  numread = read(readchan, buf, size);
		  if (numread <= 0)
			{
			fprintf(stderr,
		    "\nerror: read() returned %d, exiting..\n", numread);
			return(-4);
			}
			/* scan for SENTINEL (char with 8th bit set) 
			if found, call nice_exit() to terminate
			gracefully. */
		  for (i = 0 ; i < numread ; i++)
			if ((buf[i] & 0x80) != 0) 
				nice_exit(0);
		  buf[numread + 1] = '\0';
		  printf("\n        received-->%s",buf);
		  fflush(stdout);
		  }
		}  /* end of OFFSPRING code */
#else
	fprintf(stdout, "\nexecuting fork()");
	fflush(stdout);			
					/* fork(), unlike spawn(), makes a
				copy of the parent's data segment and
				descriptor table. Execution in the case 
				of the offspring begins at NEXT LINE after 
				the fork() */

	if((opid = fork()) == 0)	/* fork() RETURNS 0 to OFFSPRING */
		{
		am_offspring = 1;	/* we are the OFFSPRING process */

		close(writechan);	/* close WRITING side of the pipe */
		signal(SIGTERM, nice_exit);	/* handle termination
				request by the procedure nice_exit() */
		printf("\nReceiver: process id: %d, waiting at pipe. \n",
				(int)getpid());

			/* read loop - offspring leaves this loop
			only by receiving the SIGTERM signal which
			invokes the nice_exit() signal handler. */
		while(1)
		  {
		  numread = read(readchan, buf, size);
		  if (numread <= 0)
			{
			fprintf(stderr,
		    "\nerror: read() returned %d, exiting..\n", numread);
			return(-4);
			}
		  buf[numread + 1] = '\0';
		  printf("\n        received-->%s",buf);
		  fflush(stdout);
		  }
		}  /* end of OFFSPRING code */

	/* fall through to PARENT code (fork() returned offspring OPID
	to the parent and this is non-zero) */
#endif

#if defined(_WIN95) || defined(_WINNT)
parent:
#endif
	close(readchan);		/* close READing side of the pipe */
		
		/* write loop */
	sleep(2);			/* pause for clean screen display */
	printf("\nStart entering a stream of characters at prompt;");
	printf(" type .<cr> to stop program. \n");
loop:	printf("\nenter line> ");

/* *** OLD *** gets() is dangerous
	gets(chrline);		even though it converts '\n' to \0' */

	fgets(chrline, sizeof(chrline) - 1, stdin);
	for (i = 0 ; i < sizeof(chrline) - 1 ; i++)
		{
		if ( (chrline[i] == '\n') || (chrline[i] == '\0') )
			break;
		}
	if (chrline[i] == '\n')
		chrline[i] = '\0'; 	/* get rid of newline */

	if (chrline[0] == '.')		/* user indicates end of stream */
		{
#if defined(_WIN95) || defined(_WINNT)
		printf("\nSender: sending process %d a character with the",
			opid); 
		printf(" 8th bit set as SENTINEL..\n");
		chrline[0] = vaxunsign(0x00ff);
		write(writechan, chrline, 1);
#else
		printf("\nSender: sending process %d ",opid); 
		printf("a SIGTERM, which requests termination..");
		fflush(stdout);
		kill(opid, SIGTERM);	/* politefully request termination
					of offspring RECEIVING process */
#endif
		fflush(stdout);
		close(writechan);
					/* wait for offspring 
				RECEIVING process to terminate. This is
				important so that we don't leave a ZOMBIE
				in the process table. */
		opid = WAIT_PID(opid,&ostatus,WAIT_FLAGS); 
		printf("\nSender: returned from waitpid() with ostatus=%d, ",
			ostatus);
		printf("also exiting.. \n");
		fflush(stdout);

#include "cleanup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */
		return(0);
		}  /* end of if (chrline[0] == '.') */

		/* user indicates end of stream */
	length = strlen(chrline) + 1;
	numwritten = write(writechan, chrline, length);
	sleep(2);	/* may need to increase this for clean display
			if system is heavily loaded. */
	goto loop;
	}  /* end of main */

/* -------------------------------------------------------------- */

#if defined(_NOPROTO)
int nice_exit()
#else
void nice_exit(int dummy)
#endif
			/* Under Unix SIGTERM sends the offspring to 
		this routine in order to exit gracefully. Under Windows
		NT the receiving process has to watch for a SENTINEL
		and CALL this routine if one is found. */
	{
	printf(
	  "\n\n        ********************************************** ");
	printf(
	  "\n        received-->%s, requesting end of session. ", EXIT_PLEA);
	printf(
	  "\n        ********************************************** \n");
	fflush(stdout);
	close(readchan);
	printf(
	  "\nReceiver: pipe closed, now ending session and exiting. \n");
	exit(0);
	}  /* end of nice_exit */

