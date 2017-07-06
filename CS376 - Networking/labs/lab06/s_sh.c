
/* s_ksh:	This is a small shell which is able to run under 
		Windows NT (and its prototype, OS/2, with some mod's) 
		or under Unix/Linux (POSIX support is best here). 

	     I. If it is invoked from the COMMAND LINE via

		s_sh [cr]

		then it expects to take its input from (what it thinks is) 
		the terminal console (and no_redirect = 1). Of course, if 
		it has been fork/exec'd under Unix it cannot actually tell 
		whether or not stdin (descriptor 0) and stdout (descriptor 1) 
		are actually connected to a terminal console until it calls 
		isatty(), in which case, if it is not talking to a terminal 
		console then no_redirect = 0.

	    II. If it is spawned by a parent such as Windows NT as 
		 
	  	spawnlp(P_NOWAIT, "s_sh.exe", "s_sh.exe", 
			"-R", tosh_str, "-W", fromsh_str, NULL); 
	
		this is EQUIVALENT to a command line invocation of the
		following form:

		s_sh.exe -R [pipe_readdesc] -W [pipe_writedesc] [cr]

		and it uses dup2() to (manually) remap these descriptors
		to the descriptors for stdin (0) and stdout (1). In this
		case it uses the two pipes as a primitive type of
		pseudo-terminal to accept commands and return results.

		Currently this shell shows the mechanism of parsing 
		commands, searching the environmental PATH variable, 
		matching wildcards (which modifies the command params),
		and execution.

						Marc Thomas           

		This shell supports only the following features, but does 
		show you what it is doing as it operates: 

		i.    It will parse a command line into tokens assuming
		      that they are separated by whitespace (i.e. spaces, 
		      tabs, or a combination of these). A token is allowed
		      to contain a space if it is quoted (e.g. "hi there").
		ii.   It will allow use of one wildcard (* or ?) on the 
		      command line, and will use fnmatch() to expand these 
		      arguments.
		iii.  It has the following INTERNAL commands: cd, dir, envp, 
		      help, pwd, tty, history, ! [repeat num], jobs, and exit.
		iv.   It will try to find EXTERNAL commands making use of 
		      the PATH environmental variable and it will get the 
		      st_mode of the (supposedly) executable file together
		      with the uid and gid.
		v.    It uses a fork()..execve() sequence of calls to execute 
		      EXTERNAL commands under Unix and a spawnve()} call to
		      execute commands under Windows NT.
		vi.   It allows modification of its own environment from
		      the command line (e.g. "TERM= vt100" redefines the
		      value of the environmental variable "TERM").
		vii.  It does allow backgrounding a job using the ampersand
		      token '&' at the end of the command line. It also
		      supports a "jobs" command.

		Note that the following features (common to most shells) 
		are NOT supported (although some are syntactically recognized):

		viii. It will not allow MORE THAN ONE wildcard character 
		      (* or ?) per command line.
		ix.   It does not support redirection of input or output, 
		      using, for example, the  '>' and '<' tokens.
		x.    It does not support the use of '$' in a command
		      line to refer to the contents of an environmental
		      variable (e.g. "echo $PATH").
		xi.   Although it does maintain a command history and 
		      allow re-execution with the "! [num]" internal
		      command, it does not allow editing current or 
		      previous command lines (c.f.  the "set -o vi" option 
		      in csh, ksh, and bash).
		xii.  It does not support piping the output of one program 
		      to the input of another, using, for example, the
		      '|' token.


		The operation of the main loop of this shell can be 
		blocked out as follows:

		while(1) {
		call getcmd() 
			which reads from descriptor 0 into

		    raw_cmdline[last_cmd][] = "ls -C *.h > tmpfile\0"
				cmdbuf[]    = "ls -C *.h > tmpfile\0"
				no_wildcards = 1

			and then calls 
			parse_cmdbuf()
				which parses line and sets

		    raw_cmdline[last_cmd][] = "ls -C *.h > tmpfile\0"
				cmdbuf[]    = "ls\0-C\0*.h\0>\0tmpfile\0"
				cmd[0] --------^   ^   ^    ^  ^
				cmd[1] ------------^   ^    ^  ^
				cmd[2] ----------------^    ^  ^
				cmd[3] ---------------------^  ^
				cmd[4] ------------------------^
				cmd[5] = NULL
				no_wildcards = 0
				first_wildcard_index = 2
				redirect_stdin = 0
				infile[] = ""
				redirect_stdout = 1
				outfile[] = "tmpfile"

		    if the command is a "! [num]" and the number given
		    is in the history queue, then getcmd() retrieves the
		    old command line, substitutes it in cmdbuf[] and
		    calls parse_cmdbuf() again.

		report bad command line grammar and/or feature not yet
			supported and continue

		check for the "exit" command, if found, exit

		check if the command is an "internal" command, if so,
			execute and continue

		assume command is "external", use the "PATH="
			environmental variable to try to find it, and,
			if found, stat() it

		if there were wildcards on the command line expand them,
			and show the expansions, by calling expand_wcard(),
			for example:

			cmd[0] -> "/bin/ls"
			cmd[1] -> "-C"
			cmd[2] -> "*.h"		(wildcard_index = 2)
			cmd[3] -> NULL

			is converted to:

			cmdexp[0] -> "/bin/ls"
			cmdexp[1] -> "-C"
			cmdexp[2] -> "startup.h"	
			cmdexp[3] -> "cleanup.h"	
			cmdexp[4] -> "specio.h"	
			cmdexp[5] -> NULL 

			return(matches=3)

		execute the command with fork..exec (Unix) or spawnve
			(Windows NT), if successful, record opid in
			history queue
		} 

									*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>		/* for stat(),lstat() */
#include <string.h>
#include <signal.h>		/* for signal() */
#include <errno.h>

#if defined(_WIN95) || defined(_WINNT)

#include <io.h>			/* for _pipe */
#include <fcntl.h>		/* for O_BINARY */
#include <process.h>
#include <direct.h>		/* for _getcwd */

#define DIRSEP '\\'
#define OPFLAG '/'		/* e.g.   ... /V /S          */
#define PATHSEP ';'		/* in "PATH=" statement      */
#define	EXEC_SUFFIX ".exe"

#define GET_CWD		_getcwd
#define	CREATE_PIPE(x)	_pipe(x,1024,O_BINARY)
#define	WAIT_PID(p,s,o)	_cwait(s,p,o)
#define	WAIT_FLAGS	_WAIT_CHILD
#define	EXIT_PLEA	"SENTINEL"
#define	DESC_EXTERNALIZE(d,s,b)	itoa(d,s,b);
#define DESC_INTERNALIZE(s)	atoi(s);

#define	PREEXEC_MESSAGE		"args before spawnve.."
#define	EXEC_MESSAGE		"starting spawnve.."
#define	ENDSHELL_MESSAGE	"returning to WinNT-OS/2.."
#else

#include <dirent.h>		/* POSIX version */
#include <sys/wait.h>		/* for wait() */

#define DIRSEP '/'
#define OPFLAG '-'		/* e.g.   ... /V /S          */
#define PATHSEP ':'		/* in "PATH=" statement      */
#define	EXEC_SUFFIX ""

#define GET_CWD		 getcwd
#define	CREATE_PIPE(x)	 pipe(x)
#define	WAIT_PID(p,s,o)	 waitpid(p,s,o)
#define	WAIT_FLAGS	0
#define	EXIT_PLEA	"SIGTERM"
#define	DESC_EXTERNALIZE(d,s,b)	
#define DESC_INTERNALIZE(s)	atoi(s);

#define	PREEXEC_MESSAGE		"args before fork/execve.."
#define	EXEC_MESSAGE		"starting fork/execve.."
#define	ENDSHELL_MESSAGE	"returning to Unix.."
#endif

#if defined(__alpha) || defined(__linux)  || defined(__sun) 

#include <unistd.h>
#include <fnmatch.h>
sigset_t	zeromask, intrmask, oldmask;

#elif defined(__bsd)

#define	SIGCLD SIGCHLD

#include <unistd.h>
#include <fnmatch.h>
sigset_t	zeromask, intrmask, oldmask;

#endif

/* ------------------------------------------------------------------ */

/* #include "header.h"	 OUR header which handles MOST of the
			WinSock/32-bit Unix/64-bit Unix Differences */
/* #include "startup.h"	 globals for main module set by the
			code startup.c */
/* #include "diagnost.h"	 OUR dump_xxxx() routines and other
			diagnostics */

/* ------------------------------------------------------------------ */

			/* function Prototypes */
#if defined(_NOPROTO)

void	child_exit();
int	help_internal();
int	hist_internal();
int	proc_internal();
int	dir_internal();
int	getcmd();
int	parse_cmdbuf();
int	drop_arg();
int	expand_wcard();
int	get_st_mode();
#else

void	child_exit(int dummy);
int	help_internal(void);
int	hist_internal(void);
int	proc_internal(void);
int	dir_internal(void);
int	getcmd(void);	
int	parse_cmdbuf(char cline[], char *cargv[], int *nowildcard_flag,
		int *first_wildcard_index, 
		int *in_redir, char in_redir_file[],
		int *out_redir, char out_redir_file[]);
int	drop_arg(char *cmdargv[], int drop_index);
int	expand_wcard(char *cmdto[], char *cmdfrom[], int wildcard_index);
int	get_st_mode(struct stat *sptr, char str[]);
#endif

/* ------------------------------------------------------------------ */

			/* set limits on lines, arguments and tokens */
#define	MAX_PATHL    128
#define MAX_LINEL   1024
#define MAX_ARGS      64
#define	MAX_ARGLEN    64
#define HISTORY_LINES 16
#define	PROCESS_LINES  3
#define	ERROR_MESSAGE 80
#define	MAX_ENVPSTR  512

			/* GLOBALS used by getcmd(), parse_cmdbuf() */
int	last_cmd;		/* point to which index in raw_cmdline[][]
				   was filled last */
int	command_num;		/* commands are numbered starting with 1 */
char	raw_cmdline[HISTORY_LINES][MAX_LINEL];	  /* raw command lines */
int	num_cmdline[HISTORY_LINES];
int	pid_cmdline[HISTORY_LINES];	/* "external" commands should
				   execute with positive opid, 0 means 
				   command executed as "internal", other
				   special numbers are as follows: */
#define	UNSUPPORT_ERR	-3
#define	SYNTAX_ERR	-2
#define	EXEC_FAILED	-1

struct	_process_table {
	int	opid;			/* 0 = "unused slot" */
	int	num;			/* copy of num_cmdline */
	char	cmdline[MAX_LINEL];	/* copy of raw_cmdline[] */
	int	status;			/* for expansion */
} proc[PROCESS_LINES];
	

int	no_wildcards;		/* set by getcmd() */
int	wc_index;		/* set if wildcards */
int	redirect_stdin;		/* set if input has been redirected by
				   the '<' token */
char	infile[MAX_PATHL];
int	redirect_stdout;	/* set if output has been redirected by
				   the '>' token */
char	outfile[MAX_PATHL];

int	no_waitslot;		/* used between parse_cmdbuf and successful
				   execution of "&'d" command to hold next
				   free process slot */
int	no_waitflag;		/* 0 for general command, 1 if '&'
				   token is used to background command */
int	last_SIGCLD;		/* pid of last offspring which terminated
				   causing SIGCLD -> child_exit()
				   which called last_SIGCLD = wait() */
int	inline_opid;		/* this is 0 except during waitpid() (NT)
				   or sigsuspend() (Unix) calls for inline 
				   commands. It is reset either

				   under a normal (non-signal, non-error) 
				   return of waitpid() (NT), or

				   when SIGCLD -> child_exit()
				   and the last_SIGCLD matches inline_opid */ 

char	cmdbuf[MAX_LINEL];	/* buffer for command line that user will
			type. this is filled by getcmd() and modified in
			the process of parsing. */
char	*cmd[MAX_ARGS];	/* array for pointers to tokens which are
			command line arguments (note that a pointer
			to a token is the same as a pointer to char).
			these pointers will actually point to
			sub-strings in cmdbuf[]. */
char	*cmdexp[MAX_ARGS];	/* these are only used when expanding
			arguments with wildcards */
char	cmdpath[MAX_PATHL];	/* string for FULL PATH to cmd[0] */
char	cmdmode[32];		/* string for permission display  */
char	syntax_error[ERROR_MESSAGE];	/* utility routines will return
			negative values and leave message here on
			syntax and grammar errors */

				/* some GLOBALS peculiar to PC files */
#define ATTR	 0x0037		/* find ANY file type */
#define NOR_ATTR 0x0000		/* find NORMAL files only */
#define	READ_ONLY	0x0001
#define	HIDDEN		0x0002
#define	SYSTEM		0x0004
#define	DIRECT	 	0x0010
#define	ARCHIVE		0x0020

char	searchpath[MAX_PATHL];	/* string (with wildcards) to search for */
char	cbasename[9];		/* DOS/OS2/WinNT cbasename */
char	extension[4];		/* DOS/OS2/WinNT extension */

			/* GLOBALS used by DESC_INTERNALIZE */
int	pipe_readfrom;		/* "-R" command line option */
int	pipe_writeto;		/* "-W" command line option */
int	no_redirect;		/* set if terminal console I/O */
char	console_tty[MAX_ARGLEN];/* if terminal console I/O then
				   this is the tty name */

/* --------------------------------------------------------------- */

#if defined(_NOPROTO)
	main(argc, argv, envp)		
	int	argc;
	char	*argv[],*envp[];
#else
int	main(int argc, char *argv[], char *envp[])
#endif
	{
	char	cwdbuf[128], *cptr, *tempenvp;
	int	i, j, k, ret, count;
	int	opid, termstat;
	struct	stat	statbuf;	/* for stat(),lstat() */

/* #include "startup.c"		 We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix startup. Main modules
			ALSO require the inclusion of 
				header.h
				startup.h */

/* ********** CHECK COMMAND LINE INVOCATION ********************** */

	no_redirect = 1;	/* default is from terminal console */
	console_tty[0] = '\0';	/* no name yet */

#if defined(_WIN95) || defined(_WINNT)

				/* Under WinNT we have to parse command 
				line to find out our invocation status */
	for (i = 1 ; i < argc ; i++)
	    {
	    if (strcmp(argv[i], "-R") == 0) 
		{
		/* WinNT: *** REDIRECT *** stdin FROM PIPE */
		if ((i + 1) >= argc)
			{
			fprintf(stderr,
			  "\nmain: expected pipedesc after \"-R\" \n");
			fflush(stderr);
			return(-1);
			}
		i++;
		pipe_readfrom = DESC_INTERNALIZE(argv[i])
		if (dup2(pipe_readfrom, 0) < 0)
			{
			perror("dup2(pipe_readfrom, 0): ");
			fflush(stderr);
			exit(-1);
			}
		/* record the fact that we have MANUALLY redirected I/O */
		no_redirect = 0;
		}
	    else if (strcmp(argv[i], "-W") == 0) 
		{
		/* WinNT: *** REDIRECT *** stdout TO PIPE */
		if ((i + 1) >= argc)
			{
			fprintf(stderr,
			  "\nmain: expected pipedesc after \"-W\" \n");
			fflush(stderr);
			return(-1);
			}
		i++;
		pipe_writeto = DESC_INTERNALIZE(argv[i])
		if (dup2(pipe_writeto, 1) < 0)
			{
			perror("dup2(pipe_writeto, 1): ");
			fflush(stderr);
			exit(-1);
			}
		/* record the fact that we have MANUALLY redirected I/O */
		no_redirect = 0;
		}

	   }  /* end of for (i = 1 ; i < argc ; i++) */
#else
	if (isatty(0))		/* Unix has the calls isatty(), ttyname() */
		{
		no_redirect = 1;
		strncpy(console_tty, ttyname(0), sizeof(console_tty) - 1);
		}
	else
		{
		no_redirect = 0;
		}
#endif

/* ********** DO SOME INITIALIZATIONS ************************** */

	for (j = 0 ; j < MAX_ARGS ; j++) 
		cmdexp[j] = (char *)NULL;  /* memory unallocated */
	for (j = 0 ; j < HISTORY_LINES ; j++)
		{
		raw_cmdline[j][0] = '\0';
		num_cmdline[j] = 0;	   /* no command yet */
		pid_cmdline[j] = 0;
		}
	last_cmd = HISTORY_LINES - 1;
	command_num = 0;	/* numbering will start at 1 */
	for (j = 0 ; j < PROCESS_LINES ; j++)
		{
		proc[j].opid = 0;	   /* available slot */
		proc[j].num = 0;
		proc[j].cmdline[0] = '\0';
		proc[j].status = 0;
		}

/* ********** SET SOME SIGNAL HANDLERS ************************* */

#if defined(_WIN95) || defined(_WINNT)

#else
	sigemptyset(&zeromask);
	sigemptyset(&intrmask);
	sigemptyset(&oldmask);
	sigaddset(&intrmask, SIGCLD);	/* we will only watch
			for SIGCLD at this time */

	signal(SIGCLD, child_exit);	/* this will warn
			us to update our tables since if a child 
			process dies or is terminated we will be 
			sent a SIGCLD signal */
#endif

/* ********** START MAIN COMMAND LINE LOOP: ******************** */

	while(1)			/* repeat forever */
	  	{
		fflush(stdout);
		printf("\r\ns_sh> ");	/* put out prompt */
		fflush(stdout);
		last_cmd++;		/* go to new line in history */
		if (last_cmd == HISTORY_LINES)
			last_cmd = 0;
		command_num++;		/* set the command number */
		num_cmdline[last_cmd] = command_num;
		pid_cmdline[last_cmd] = EXEC_FAILED;	/* regard it 
			as "failed" until we are able to execute it either 
			internally or externally */
		syntax_error[0] = '\0';	/* reset message */
		no_waitflag = 0;	/* normal in-line command unless
			we find out otherwise */
main_loop:
		count = getcmd();	/* call getcmd() to make copy
			of raw command in history queue and fill
			the array of string pointers cmd[]. note that
			cmdbuf[] contains the objects of the pointers
			in cmd[]. */

/* ********** CHECK FOR BAD GRAMMAR AND/OR UNSUPPORTED ************* */

		if (raw_cmdline[last_cmd][0] == '\0')
			{
			fflush(stdout);
			printf("\rs_sh> ");	
			fflush(stdout);
		    	goto main_loop;	/* user probably just hit [CR] */
			}
		if (count < 0)
			{
			pid_cmdline[last_cmd] = SYNTAX_ERR;
			printf("\r\n  error: %s\r\n", syntax_error);
			continue;	/* with while(1) */
			}
		if ( (count < 1) || (cmd[0] == NULL) )
			{
			pid_cmdline[last_cmd] = SYNTAX_ERR;
					/* give user some options */
			help_internal();
			continue;	/* with while(1) */
			}  

/* ********** CHECK FOR REDIRECTION TOKENS ON COMMAND LINE ********* */

		if (redirect_stdin) 
			{
			printf(
	"\r\n  redirection of stdin (file=\"%s\") not yet supported ",
				infile);
			}
		if (redirect_stdout) 
			{
			printf(
	"\r\n  redirection of stdout (file=\"%s\") not yet supported ",
				outfile);
			}
		if ( (redirect_stdin) || (redirect_stdout) )
			{
			pid_cmdline[last_cmd] = UNSUPPORT_ERR;
			i = 0;
			printf("\r\n  redirected raw_cmd[]: ");
			while (cmd[i] != NULL)
				{
				printf("\"%s\" ", cmd[i]);
				i++;
				}
			printf("\r\n");
			continue;	/* with while(1) */
			}

/* ********** CHECK FOR BACKGROUNDING TOKEN ON COMMAND LINE ******** */

		if ( (count > 1) &&
			(strcmp(cmd[count - 1], "&") == 0) )
			{
#if defined(_WIN95) || defined(_WINNT)

			pid_cmdline[last_cmd] = UNSUPPORT_ERR;
			printf(
  "\r\n  backgrounding of jobs with \"&\" not supported under NT ");
			no_waitslot = PROCESS_LINES;
#else
				/* check if we have a free slot
				in the process table */

			for (no_waitslot = 0 ; no_waitslot < PROCESS_LINES ;
					no_waitslot++)
				{
				if (proc[no_waitslot].opid == 0) break;
				}
			if (no_waitslot < PROCESS_LINES)
				{
				proc[no_waitslot].num = command_num;
				strcpy(proc[no_waitslot].cmdline,
					&raw_cmdline[last_cmd][0]);
				no_waitflag = 1;	/* systems "go" */
				}
			else
				{
			printf(
  "\r\n  backgrounding of jobs not possible - all process slots are full ");
				}
#endif
			drop_arg(cmd, count - 1);
			i = 0;
			printf("\r\n  backgrounded raw_cmd[]: ");
			while (cmd[i] != NULL)
				{
				printf("\"%s\" ", cmd[i]);
				i++;
				}
			printf("\r\n");
			if (no_waitslot >= PROCESS_LINES)
				continue; 	/* with while(1) */
			}

/* ********** CHECK FOR ENVIRONMENTAL ASSIGNMENTS ON COMMAND LINE ** */

		if ( (strlen(cmd[0]) > 0)  &&
			(*(cmd[0] + strlen(cmd[0]) - 1) == '=') )
			{
			if ((cptr = getenv(cmd[0])) != (char *)NULL)
				{
				printf(
			"\r\n  changing \"%s%s\" ", cmd[0], cptr);
				}
			else
				{
				printf(
			"\r\n  setting \"%s\" ", cmd[0]);
				}
					/* note that the pointer in
				envp[] of main() will be changed by putenv()
				ONLY if the variable of cmd[0] is already 
				listed as one of the keywords.  furthermore 
				we MUST allocate STATIC storage for the new 
				setting with malloc() */

			tempenvp = (char *)malloc(MAX_ENVPSTR);
			if (tempenvp == (char *)NULL)
				{
				fprintf(stderr, "\nfatal error: malloc 4\n");
				fflush(stderr);
				exit(-1);
				}
			if (cmd[1] != (char *)NULL)
				sprintf(tempenvp, "%s%s", cmd[0], cmd[1]);
			else
				sprintf(tempenvp, "%s", cmd[0]);
			ret = putenv(tempenvp);
			if (ret == 0)
				{
				printf("to \"%s\" ", tempenvp);
				pid_cmdline[last_cmd] = 0;
				}
			else
				{
				printf(
			"\r\n  error: putenv(\"%s\") failed ", tempenvp);
				}
			continue;	/* with while(1) */
			}

/* ********** CHECK FOR "help" COMMAND ***************************** */

		if (strcmp(cmd[0], "help") == 0) 
			{
			pid_cmdline[last_cmd] = 0;
			help_internal();
			continue;	/* with while(1) */
			}  
		
/* ********** CHECK FOR "exit" COMMAND ***************************** */

		if (strcmp(cmd[0], "exit") == 0)
			{
			pid_cmdline[last_cmd] = 0;

					/* kill running jobs */
#if defined(_WIN95) || defined(_WINNT)

#else
			signal(SIGCLD, SIG_IGN);   /* No need checking
				for death of an offspring anymore - if
				they don't respond to a sure kill (SIGKILL)
				there is not much else we can do */

			for (i = 0 ; i < PROCESS_LINES ; i++)
				{
				if (proc[i].opid > 0)
					{
					printf(
					  "\r\n  exit: killing opid=%d ", 
						proc[i].opid);
					kill(proc[i].opid, SIGKILL);
					}
				}	
#endif
			printf("\r\n%s \r\n", ENDSHELL_MESSAGE);
			fflush(stdout);

/* #include "cleanup.c"		 We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */
			return(0);
			}  /* end of "exit" */

/* ********** START OF INTERNAL COMMANDS: dir, pwd, .. ************* */

				/* *** CHANGE DIR **** on "cd" */
		if (strcmp(cmd[0], "cd") == 0)
			{
			if (cmd[1] == NULL)
				{
				if ( GET_CWD(cwdbuf, sizeof(cwdbuf)) != NULL )
					printf("\r\n%s ", cwdbuf);
				pid_cmdline[last_cmd] = 0;
				}
			else
				{
				if (chdir(cmd[1]) != 0) printf(
				  "\r\n  error: chdir(\"%s\") failed ", cmd[1]);
				else
					pid_cmdline[last_cmd] = 0;
				}
			continue;	/* with while() */
			}  /* end of "cd" */

				/* *** ENVIRONMENT *** on "envp" */
		if (strcmp(cmd[0], "envp") == 0)
			{
				/* envp (with no arguments) will obviously 
			NOT list any NEW environmental variables we have 
			defined with KEYWORD={value} from the command line 
			since the array of pointers envp[] has a fixed size 
			as passed to main(). Under Windows NT it will NOT 
			even list new VERSIONS of existing environmental
			variables which have been redefined. */

			if (cmd[1] == NULL)
			    {
			    printf("\r\nthe environment for s_sh is: \r\n");
			    i = 0;
			    while(envp[i] != NULL)
				{
				printf("\r\n  %s", envp[i]);
				i++;
				}
			    pid_cmdline[last_cmd] = 0;
			    }
			else
			    {
			    cptr = getenv(cmd[1]);
			    if (cptr != (char *)NULL)
			    	printf("\r\ns_sh envp var %s=\"%s\" \r\n",
					cmd[1], cptr);
			    else
			    	printf("\r\ns_sh envp var %s=empty \r\n",
					cmd[1]);
			    pid_cmdline[last_cmd] = 0;
			    }
			continue;	/* with while() */
			}  /* end of "envp" */

				/* *** HISTORY *** on "history" */
		if (strcmp(cmd[0], "history") == 0)
			{
			pid_cmdline[last_cmd] = 0;
			hist_internal();
			continue;	/* with while() */
			}  /* end of "history" */

				/* *** JOBS *** on "jobs" */
		if (strcmp(cmd[0], "jobs") == 0)
			{
			pid_cmdline[last_cmd] = 0;
			proc_internal();
			continue;	/* with while() */
			}  /* end of "history" */

				/* *** DIRECTORY *** on "dir" */
		if (strcmp(cmd[0], "dir") == 0)
			{
			dir_internal();
			pid_cmdline[last_cmd] = 0;
			continue;	/* with while() */
			}  /* end of "dir" */

				/* *** CURRENT DIR *** on "pwd" */
		if (strcmp(cmd[0], "pwd") == 0)
			{
			if ( GET_CWD(cwdbuf, sizeof(cwdbuf)) != NULL )
				printf("\r\n%s ", cwdbuf);
			pid_cmdline[last_cmd] = 0;
			continue;	/* with while() */
			}  /* end of "pwd" */

				/* *** SHOW ISATTY *** on "tty" */
		if (strcmp(cmd[0], "tty") == 0)
			{
			if (no_redirect)
				printf("\r\nI/O is from console %s",
					console_tty);
			else
				printf("\r\nI/O is from pipe/socket ");
			pid_cmdline[last_cmd] = 0;
			continue;	/* with while() */
			}  /* end of "tty" */

/* ********** END OF INTERNAL COMMANDS ***************************** */

/* note that the arguments passed to spawnve() are as follows:
	spawnve(wait_type,
		command_path,command_arg_array,environmental_array) where

	wait_type = P_NOWAIT for asynchronous with wait/cwait()
	command_path = pathname to the executable command (cmd[0]).
	command_arg_array = an array of pointers to the strings which
		are the command line arguments. the last pointer must
		be NULL.
	environmental_array = same as envp[] in main unless you wish
		to change the environment that the offspring executes
		under.		*/

/* ********** DISPLAY (UNEXPANDED) COMMAND LINE: ******************* */
		i = 0;
		if (no_wildcards)
			printf("\r\n  raw_cmd[]: ");
		else
			printf(
		"\r\n  raw_cmd[](first wildcard on token %d): ", wc_index);
		while (cmd[i] != NULL)
			{
			printf("\"%s\" ", cmd[i]);
			i++;
			}

/* ********** FIND COMMAND PATH cmdpath[] TO EXECUTABLE: *********** */

		/* ..first try current path as given */
		sprintf(cmdpath, "%s", cmd[0]);
		ret = stat(cmdpath, &statbuf);
		if (ret < 0)
			{
			printf("\r\n  stat(\"%s\",&statbuf) < 0", 
				cmdpath);
			fflush(stdout);
		/* ..then try environmental PATH variable */
			cptr = getenv("PATH");
			if (cptr == (char *)NULL)
				goto searchdone;	/* failure */
			printf(
		"\r\nsearching dirs in environmental PATH variable..");
			fflush(stdout);
			k = 0;		/* limit number of PATHs */
			do  {
			    j = 0;
					/* note that we have to ALLOW
				a space in filenames to coexist with
				BUTTHEAD systems with directories
				like "Program Files" & "Happy Mac" */
			    while( (*cptr != PATHSEP) &&
				(cptr != '\0') && ((*cptr & 0x80) == 0) &&
				((*cptr & 0x7f) >= 0x20) )
				{
				cmdpath[j++] = *cptr;
				cptr++;
				}
			    cmdpath[j++] = DIRSEP;
			    cmdpath[j] = '\0';		/* terminate */
			    strcat(cmdpath, cmd[0]);
			    ret = stat(cmdpath, &statbuf);
			    if (ret >= 0)
				goto searchdone;	/* success */

#if defined(_WIN95) || defined(_WINNT)	/* check ".exe" suffix too */

			    strcat(cmdpath, EXEC_SUFFIX);
			    ret = stat(cmdpath, &statbuf);
			    if (ret >= 0)
				goto searchdone;	/* success */
#endif
			    printf("\r\n  stat(\"%s\",&statbuf) < 0", 
				cmdpath);
/* *** DIAGNOSTIC ***
			    printf(
		"\r\n  stat(\"%s\",&statbuf) failure *cptr=0x0%x ", 
				cmdpath, vaxunsign(*cptr)); */

			    fflush(stdout);
			    if (*cptr == PATHSEP) cptr++;
			    k++;
			    } while ( (*cptr != '\0') && (k < MAX_ARGS) );
			printf("\r\n");
			fflush(stdout);
			cmdpath[0] = '\0';
			} /* end if (ret < 0) */
 
				/* INSURANCE: (double) check to see that 
			stat() finds the executable AND display the
			permissions */
searchdone:
		ret = stat(cmdpath, &statbuf);
		if (ret < 0)
			{
			continue;	/* with while() */
			}
		if ( (cmdpath == NULL)  ||  (cmdpath[0] == '\0') )
			{
			printf("\r\n  cmd[0] not found. ");
			continue;	/* with while() */
			}
#if defined(_WIN95) || defined(_WINNT)

#else
		else if (S_ISDIR(statbuf.st_mode))
			{
			printf(
			  "\r\n  cmdpath[] found: \"%s\" but is a directory. ",
				cmdpath);
			continue;	/* with while() */
			}
#endif
		else
			{
			get_st_mode(&statbuf, cmdmode);
			printf(
			  "\r\n  cmdpath[] found: \"%s\" (st_mode=%s) ", 
				cmdpath, cmdmode);
			}

/* ********** IF WILDCARDS, EXPAND AND SHOW EXPANSIONS: ************ */
		if (no_wildcards == 0)
			{
			ret = expand_wcard(cmdexp, cmd, wc_index);
			if (ret == -2)
				{
				printf(
  "\r\nwildcards *** not *** implemented for systems lacking fnmatch \r\n");
				fflush(stdout);
				continue;
				}
			else if (ret == -1)
				{
				printf("\r\nopendir(\".\") failed for %s ", 
					cmd[wc_index]);
				fflush(stdout);
				continue;
				}
			else if (ret == 0)
				{
				printf(
				  "\r\nno match for %s ", 
					cmd[wc_index]);
				fflush(stdout);
				continue;
				}

			fflush(stdout);
			}  /* end of if (no_wildcards == 0) */

/* ********** SHOW (possibly modified) COMMAND LINE ARGS *********** */
		k = 0;
		printf("\r\n%s ", PREEXEC_MESSAGE);
		printf("\r\n  cmdpath[]=\"%s\" ", cmdpath);
		if (no_wildcards)
			while(cmd[k] != (char *)NULL)
				{
				printf("\r\n  cmd[%2d]=\"%s\" ",
					k, cmd[k]);
				k++;
				}
		else
			while(cmdexp[k] != (char *)NULL)
				{
				printf("\r\n  cmdexp[%2d]=\"%s\" ",
					k, cmdexp[k]);
				k++;
				}
		printf("\r\n%s ", EXEC_MESSAGE);
		fflush(stdout); 

/* ********** EXECUTE ********************************************** */

#if defined(_WIN95) || defined(_WINNT)

				/* WinNT(and OS/2) spawnve() */
		if (no_wildcards)
			opid = spawnve(P_NOWAIT, cmdpath, cmd, envp);
		else
			opid = spawnve(P_NOWAIT, cmdpath, cmdexp, envp);
#else

		opid = fork();	/* Unix fork() */
		if (opid == 0)	/* We are the OFFSPRING */
			{
#if defined(__sun)		/* Apparently a RACE condition in Solaris
			here in that parent is STILL on fork() and
			doesn't get to sigprocmask() fast enough */

			usleep(500000);
#endif
				/* Offspring overlays itself with command
				line executable */
			if (no_wildcards)
				execve(cmdpath, cmd, envp);
			else
				execve(cmdpath, cmdexp, envp);
			}
#endif
		if (opid <= 0)
			{
			perror("execve/spawnve: ");
			}
		else if (no_waitflag == 1) /* *** KEEP RUNNING *** */
			{
			pid_cmdline[last_cmd] = opid;	/* success */
			proc[no_waitslot].opid = opid;

/* *** DIAGNOSTIC *** */
			printf(" parent continuing..");
			printf("\r\n\n"); 
			fflush(stdout);
			}
		else		/* *** WAIT *** for Offspring to finish */
			{
				/* Two potential RACE conditions: 

		1. with background processes running there is the 
		   possibility that the inline process will finish, 
		   quickly, WAIT_PID will be called below, and the 
		   SIGCLD will be delayed so that by the time the handler, 
		   child_exit(), gets it, it will BLOCK (until some other 
		   offspring terminates) hanging the shell. We fix this 
		   by substituting a sigprocmask()..sigpsuspend() sequence 
		   for the overly simple waitpid() 

		2. the offspring process finishes (or faults) so quickly
		   that the sigprocmask() is NOT reached in time to
		   SIG_BLOCK the SIGCLD signal. We therefore check to
		   see if last_SIGCLD = opid (in which case a SIGCLD
		   got to us BEFORE the sigprocmask(), if so, we skip 
		   the sigpause(&zeromask). */

#if defined(_WIN95) || defined(_WINNT)

#else
					/* temporarily block SIGCLD */
			sigprocmask(SIG_BLOCK, &intrmask, &oldmask);
#endif
			pid_cmdline[last_cmd] = opid;	/* success */
			inline_opid = opid;	/* set the flag */

#if defined(_WIN95) || defined(_WINNT)

inline_wait:
			printf(" parent in waitpid(%d,..)..", opid);
			printf("\r\n\n"); fflush(stdout);

			ret = WAIT_PID(opid, &termstat, WAIT_FLAGS);
			if (ret == opid)
				{
				inline_opid = 0;	/* ok, reset
					the flag ourselves, we were
					NOT interrupted by a signal */
				}
			else if ( (ret < 0) && (inline_opid == 0) &&
					(errno == EINTR) )
				{
				;		/* ok, SIGCLD was 
					caught for this opid by 
					child_exit() which
					reset the flag to zero */
				}
			else if ( (ret < 0) && (errno == EINTR) &&
					(inline_opid != 0) )
				{
				goto inline_wait;	/* probably
					child_exit() caught
					some OTHER child process
					terminating */
				}
			else if (ret <= 0)	/* NOT expected */
				{
				perror("waitpid: ");
				inline_opid = 0;
				}
#else
			if (last_SIGCLD == opid)
				{
				printf(
	"  offspring terminated before sigprocmask() reached..");
				printf("\r\n\n"); fflush(stdout);
				}
			else 			/* SIGCLD is pending */
				{
				printf(
	"parent in while(inline_opid == %d) sigsuspend(0)..", opid);
				printf("\r\n\n"); fflush(stdout);
							/* wait until
					the signal handler gets the
					correct opid */
				while(inline_opid == opid)
					sigsuspend(&zeromask); 
				}
							/* restore the
					original signal mask */
			sigprocmask(SIG_SETMASK, &oldmask, NULL);
#endif
			}

		}  /* end of while(1) of main command loop */

	}  /* end of main */

/* ---------------------------------------------------------------------
signal handlers:
			child_exit()

   --------------------------------------------------------------------- */

#if defined(_NOPROTO)
	child_exit()
#else
void	child_exit(int dummy)
#endif
			/* actually this signal handler will NEVER
		be called under Windows NT since SIGCLD is NOT
		supported */
	{
#if defined(_WIN95) || defined(_WINNT)

#else
	int	i, opid, ostatus;

	opid = wait(&ostatus);	 
				/* Some Linux/Unix: if waitpid() has 
		been called on a child the child may die within
		the waitpid() but STILL generate SIGCLD. In this 
		case errno == ECHILD but it is a false alarm */

	if ( (opid <= 0) && (errno == ECHILD) )
		{
		printf("\r\n  child_exit: false alarm \r\n"); 
		}
	else if ( (opid <= 0) && (errno != ECHILD) )	/* NOT expected */
		{
		perror("wait: ");
		}
	else if (opid > 0)
		{
		last_SIGCLD = opid;
		printf("\r\n  child_exit: opid=%d exited \r\n", 
			last_SIGCLD);
		if (last_SIGCLD == inline_opid)
			inline_opid = 0;	/* inline child process
				has finished - inform sigsuspend() loop */

		for (i = 0 ; i < PROCESS_LINES ; i++)
			{
			if (proc[i].opid == opid)
				{
				proc[i].opid = 0;	/* free slot */
				break;
				}
			}	
		}  /* end of if (opid > 0) */
#endif

	fflush(stdout);
					
#if defined(_WIN95) || defined(_WINNT)

#else
	signal(SIGCLD, child_exit);	/* RESET handler */
#endif
	}  /* end of child_exit */

/* ---------------------------------------------------------------------
internal commands:
			help_internal()
			hist_internal()
			proc_internal()
			dir_internal()

   --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	help_internal()
#else
int	help_internal(void)
#endif
	  /* run the internal command "help" */
	{
	printf("\r\ns_sh internal commands: \r\n");
	printf("\r\n     cd   [directory] ");
	printf("\r\n     dir  [directory] ");
	printf("\r\n     envp [variable]  ");
	printf("\r\n     exit ");
	printf("\r\n     help ");
	printf("\r\n     pwd ");
	printf("\r\n     tty ");
	printf("\r\n     jobs ");
	printf("\r\n     ! [repeat] ");
	printf("\r\n     history ");
	return(0);
	}  /* end of help_internal */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	hist_internal()
#else
int	hist_internal(void)
#endif
	  /* run the internal command "history" and show the last
	HISTORY_LINES commands given with status (failed, internal,
	or offspring process id) */
	{
	int	i;

	for (i = (last_cmd + 1) ; i < HISTORY_LINES ; i++)
	    {
	    if (num_cmdline[i] > 0) 
		{
		printf("\r\n%4d ", num_cmdline[i]);
		if (pid_cmdline[i] == UNSUPPORT_ERR)
			printf("[unsupp] ");
		else if (pid_cmdline[i] == SYNTAX_ERR)
			printf("[syntax] ");
		else if (pid_cmdline[i] == EXEC_FAILED)
			printf("[failed] ");
		else if (pid_cmdline[i] == 0)
			printf("[intern] ");
		else 
			printf("[%6d] ", pid_cmdline[i]);
		printf("%s ", &raw_cmdline[i][0]);
		}
	    }
	for (i = 0 ; i < (last_cmd + 1) ; i++)
	    {
	    if (num_cmdline[i] > 0) 
		{
		printf("\r\n%4d ", num_cmdline[i]);
		if (pid_cmdline[i] == UNSUPPORT_ERR)
			printf("[unsupp] ");
		else if (pid_cmdline[i] == SYNTAX_ERR)
			printf("[syntax] ");
		else if (pid_cmdline[i] == EXEC_FAILED)
			printf("[failed] ");
		else if (pid_cmdline[i] == 0)
			printf("[intern] ");
		else 
			printf("[%6d] ", pid_cmdline[i]);
		printf("%s ", &raw_cmdline[i][0]);
		}
	    }
	return(0);
	}  /* end of hist_internal */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	proc_internal()
#else
int	proc_internal(void)
#endif
	  /* run the internal command "jobs" and show the currently
	active (backgrounded) jobs */
	{
	int	i;

	for (i = 0 ; i < PROCESS_LINES ; i++)
		{
		if (proc[i].opid > 0)
			{
			printf("\r\n%4d [%6d] %s",
				proc[i].num, proc[i].opid, proc[i].cmdline);
			}
		}	

	return(0);
	}  /* end of proc_internal */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	dir_internal()
#else
int	dir_internal(void)
#endif
	  /* run the (MSDOS) internal command "dir" (this is 
	currently NOT implemented for Unix) */
	{
#if defined(_WIN95) || defined(_WINNT)

	int	i, j, ret;
	struct	_finddata_t	fileinfo;
	long	hfile;		/* file handle for _findfirst().. */
#endif

	unsigned long	totalbytes;
	int	numfiles;

	if (no_wildcards)
		strcpy(searchpath, "*.*");
	else
		strcpy(searchpath, cmd[wc_index]);
	numfiles = 0;
	totalbytes = 0L;

#if defined(_WIN95) || defined(_WINNT)

	hfile = _findfirst(searchpath, &fileinfo);
	if (hfile < 0L) ret = -1;
		else ret = 0;	/* valid handle */
	if (ret != 0)
		printf("\r\n  total 0 ");
	else
		{
		while(ret == 0)
		  {
		  cbasename[0] = '\0';
		  for (i = 0 ; (i < 8)  &&
			(fileinfo.name[i] != '\0')  &&
			(fileinfo.name[i] != '.') ; i++)
			  cbasename[i] = fileinfo.name[i];
		  cbasename[i] = '\0';
		  if (fileinfo.name[i] == '.')
			{
			i++;
			for (j = 0 ; (j < 3)  && (fileinfo.name[i] != '\0') 
				   ; i++ , j++ )
			  extension[j] = fileinfo.name[i];
		  	extension[j] = '\0';
			}
		  else
			{
			extension[0] = '\0';
			}
		  numfiles++;
		  totalbytes = totalbytes + fileinfo.size;
		  printf("\r\n");
		  printf("%-8.8s %-3.3s  %9lu bytes  ", cbasename, extension, 
				fileinfo.size);
		  if ( fileinfo.attrib & DIRECT )
			printf("D"); else printf(".");
		  if ( fileinfo.attrib & HIDDEN )
			printf("H"); else printf(".");
		  if ( fileinfo.attrib & SYSTEM )
			printf("S"); else printf(".");
		  if ( fileinfo.attrib & READ_ONLY )
			printf("R"); else printf(".");
		  if ( fileinfo.attrib & ARCHIVE )
			printf("A"); else printf(".");

		  ret = _findnext(hfile, &fileinfo);
		  }  /* end of while */

		_findclose(hfile);
		printf("\r\n         %d files", numfiles);
		printf("       %lu bytes used ",
			totalbytes);
		}  /* end of else */
#else

	printf("  total 0 (not YET implemented) \r\n");
#endif
	return(0);
	}  /* end of dir_internal */

/* ---------------------------------------------------------------------
utilities:
			getcmd()
			parse_cmdbuf()
			expand_wcard()
			get_st_mode()
   --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	getcmd()
#else
int	getcmd(void)
#endif
	   /* this procedure, with the help of parse_cmdbuf(), functions
	as a CRUDE command line parser. Spaces and tabs are delimeters
	between tokens. A token can include a space if you quote it, e.g.
	"hello there" is one token.  The following GLOBAL variables are 
	changed after this call:

	raw_cmdline[last_cmd][]	contains a copy of the raw command line
		cmdbuf[]	contains the processed command line
				  with '\0' after each token
		cmd[]		is an array of pointers to each
				  argument in cmdbuf[] (possibly with 
				  wildcard) terminated by a NULL pointer
		no_wildcards	is set to 1 if there are NO wildcards used
		wc_index	is the index of the first token which
				  contains a wildcard (or 0).
		redirect_stdin	is set to 1 if the "<" token appeared
		infile[]	is the filename following the "<" token
				  (or the empty string)

		redirect_stdout	is set to 1 if the ">" token appeared
		outfile[]	is the filename following the ">" token
				  (or the empty string)

	Note the following LIMITATIONS:

		If the user types a line longer that MAX_LINEL the rest 
		will be thrown away.  

		If a user types a token whose length exceeds MAX_ARGLEN 
		the token will be broken into two (or more) tokens. 

		There must be AT LEAST ONE delimeter between tokens,
		even when using quotes. */
	{
	int	i, j, cnum, bytesread, byteswritten;
	char	bytebuf[16];

				/* use byte-by-byte low-level input
			which is the SAFEST thing to do and check
			that we don't overflow MAX_LINEL */
	i = 0;
	cmdbuf[i] = '\0';
rd_loop:
	bytesread = read(0, bytebuf, 1);
	while ( (bytesread == 1)  &&  (bytebuf[0] != '\r')  &&
		(bytebuf[0] != '\n') && (i < (MAX_LINEL - 1)) )
		{
/* *** DIAGNOSTIC *** 
		fprintf(stderr, 
		"\n[bytebuf[0]=0x0%x][cmdbuf[i=%d]=\"%s\"]",
			vaxunsign(bytebuf[0]), i, cmdbuf);
		fflush(stderr); */

		if (bytebuf[0] == 000)
			{
			;			/* ignore NUL */
			}
		else if ( (bytebuf[0] == 010)  ||  
			  (bytebuf[0] == 0177) )
			{
			if (i > 0) 
				{
				i--;		/* BS or DEL */
				cmdbuf[i] = '\0';
				}
			bytebuf[0] = 010;
			bytebuf[1] = ' ';
			bytebuf[2] = 010;
			byteswritten = write(1, bytebuf, 3);
			}
		else		/* accumulate in cmdbuf[] */
			{
			cmdbuf[i] = bytebuf[0];	/* ECHO if we are NOT 
				talking to a terminal console.  If we are 
				talking to a terminal console (in cooked 
				mode) the line will not be passed to us
				until the user hits [Enter] */
			i++;
			cmdbuf[i] = '\0';
			if (!no_redirect)
				byteswritten = write(1, bytebuf, 1);
			}
		bytesread = read(0, bytebuf, 1);
		}  /* end of while */

	if ( (bytesread < 0) && (errno == EINTR) )
		goto rd_loop;	/* broke out of read() on SIGCLD */
	if (bytesread == 0)
		exit(0);	/* BAD READ on SOCKET */

	cmdbuf[i] = '\0';
	bytebuf[0] = '\r';
	bytebuf[1] = '\n';
	if (!no_redirect)
		byteswritten = write(1, bytebuf, 2);
		
	strcpy(&raw_cmdline[last_cmd][0], cmdbuf);	/* make copy 
			in raw_cmdline[][] history queue since 
			parse_cmdbuf() will modify cmdbuf[] */

					/* call parse_cmdbuf() - if
			there was bad grammar j will be negative and 
			there will be an error message in syntax_error[] */
	j = parse_cmdbuf(cmdbuf, cmd, &no_wildcards, &wc_index,
		&redirect_stdin, infile, &redirect_stdout, outfile);

					/* check for "! [num]" repeat
				command from history */
	if ( (j > 1) && (strcmp(cmd[0], "!") == 0) )
		{
		cnum = atoi(cmd[1]);
		for (i = 0 ; i < HISTORY_LINES ; i++)
			{
			if (cnum == num_cmdline[i])
				break;
			}
		if ( (i >= HISTORY_LINES) || (cnum >= num_cmdline[last_cmd]) )
			{
				/* bad grammar error */
			sprintf(syntax_error, 
			  "getcmd: argument following \'!\' out of range ");
			return(-1);
			}
					/* get old raw command line and
				   overwrite "! [num]" in cmdbuf[] */
		strcpy(cmdbuf, &raw_cmdline[i][0]);
					/* make new history entry */
		strcpy(&raw_cmdline[last_cmd][0], cmdbuf);
					/* call parse_cmdbuf() again */
		j = parse_cmdbuf(cmdbuf, cmd, &no_wildcards, &wc_index,
			&redirect_stdin, infile, &redirect_stdout, outfile);
		}

	return(j);		/* return number of arguments */
	}  /* end of getcmd */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	parse_cmdbuf(cline, cargv, nowildcard_flag, first_wildcard_index,
		in_redir, in_redir_file, out_redir, out_redir_file)
	char	cline[];
	char	*cargv[];
	int	*nowildcard_flag;
	int	*first_wildcard_index;
	int	*in_redir;
	char	in_redir_file[];
	int	*out_redir;
	char	out_redir_file[];
#else
int	parse_cmdbuf(char cline[], char *cargv[], int *nowildcard_flag,
		int *first_wildcard_index, 
		int *in_redir, char in_redir_file[],
		int *out_redir, char out_redir_file[])
#endif
	   /* this procedure takes a raw command line in cline[] and
	sets the array cargv[] of pointers is to point to each of the 
	arguments starting with cargv[0], modifying cline[] in the process
	(so the caller should save a copy of the RAW command line).. 
      	The last pointer is set to NULL (this is necessary in order to use 
	exec/spawn() and is one feature which is different from the 
	invocation of main()). This parser checks for wildcards in the
	form of '*' or '?' in any token. If found, it makes a note by
	resetting *nowildcard_flag = 0 and the token index of the
	first wildcard found in *first_wildcard_index. If the input
	redirection token '<' is used it sets *in_redir = 1 and
	puts the following token into in_redir_file[]. If the output
	redirection token '>' is used it sets *out_redir = 1 and
	puts the following token into out_redir_file[].

		If the user types a line longer that MAX_LINEL the rest 
	will be thrown away. If a user types a token whose length exceeds 
	MAX_ARGLEN the token will be broken into two (or more) tokens.

	The number of command line arguments (essentially argc) is returned. 
	for example if cline[] contains:

	$$ <space>/bin/ps  <tab> -ef<cr>

	then  cargv[0] --> "/bin/ps"  
	      cargv[1] --> "-ef"     
	      cargv[2] --> NULL			*/
	{
	int	i, j, done, inquote, start_i, inredir_j, outredir_j, ret;
	
	*nowildcard_flag = 1;		/* start with this set */
	*first_wildcard_index = 0;
	inredir_j = 0;
	*in_redir = 0;
	in_redir_file[0] = '\0';
	outredir_j = 0;
	*out_redir = 0;
	out_redir_file[0] = '\0';
	inquote = 0;
	done = 0;
	i = 0;
	j = 0;
	while( (cline[i] != '\0')  &&  (i < (MAX_LINEL - MAX_ARGLEN)) )
		{
		for( ; (cline[i] == ' ') ||
		       (cline[i] == '\t') ; i++);  /* skip tab, space */

		cargv[j] = &cline[i];	/* set ptr to beginning of token
					or '\0' */
		start_i = i;
		if (cline[i] == '\0') break;    /* at eol */
		for( ; ( ((inquote == 1) && (cline[i] == 32)) 
				|| (cline[i] > 32) )  &&
		       (cline[i] < 127) &&
		       ((i - start_i) < (MAX_ARGLEN - 1)) ; i++)
			{
					/* check start of quote */
			if ( (inquote == 0) && (cline[i] == '"') )
				{
				inquote = 1;
				cargv[j]++;
				}
					/* check end of quote   */
			else if ( (inquote == 1) && (cline[i] == '"') )
				{
				inquote = 0;
				cline[i] = '\0';	/* terminate */
				}
			if ( (cline[i] == '*')  ||
			     (cline[i] == '?')  )
				{
					/* set if FIRST wildcard used */
				if (*nowildcard_flag)
					{
					*nowildcard_flag = 0;
					*first_wildcard_index = j;	
					}
				}
			}  /* end of for(.. scan over token string */

		if (cline[i] == '\0')
			done = 1;		/* at EOL */
		else
			cline[i] = '\0';/* terminate token ,i.e. replace
					   whitespace delimeter by '\0' */
		i++;
					/* check for redirection tokens */
		if ( (inredir_j == 0) && (strcmp("<", cargv[j]) == 0) )
			{
			inredir_j = j;
			*in_redir = 1;
			}
		else if ( (outredir_j == 0) && (strcmp(">", cargv[j]) == 0) )
			{
			outredir_j = j;
			*out_redir = 1;
			}
		if (j < (MAX_ARGS - 1) )
			j++;		 /* ready for next token */
		else
			done = 1;	/* cargv[] array is full */

		if (done)
			break;		/* array full or at EOL */
		}
	cargv[j] = (char *)NULL;  /* last entry in cmd[] is NULL ptr */

				/* check to see that quotes were closed */
	if (inquote == 1)
		{
		sprintf(syntax_error, "parse_cmd: quote never closed");
		return(-1);
		}
				/* check re-direction arguments */
	if (inredir_j > 0) 
		{
		if ( ((inredir_j + 1) < j)  &&
			(strcmp(cargv[inredir_j + 1], ">") != 0) )
			{
			strncpy(in_redir_file, cargv[inredir_j + 1], 
				MAX_ARGLEN - 1);
			}
		else
			{
			sprintf(syntax_error, 
				"parse_cmd: no/bad argument after \'<\' ");
			return(-1);
			}
		}
	if (outredir_j > 0) 
		{
		if ( ((outredir_j + 1) < j)  &&
			(strcmp(cargv[outredir_j + 1], "<") != 0) )
			{
			strncpy(out_redir_file, cargv[outredir_j + 1], 
				MAX_ARGLEN - 1);
			}
		else
			{
			sprintf(syntax_error, 
				"parse_cmd: no/bad argument after \'>\' ");
			return(-1);
			}
		}
				/* at this point, redirection is at least
				syntactically correct so fix cargv[] by
				deleting redirection tokens and targets */
	if ( (inredir_j > 0) && (inredir_j > outredir_j) )
		{
		if ( (ret = drop_arg(cargv, inredir_j)) == 0)
			j--;
		else
			return(-1);
		if ( (ret = drop_arg(cargv, inredir_j)) == 0)
			j--;
		else
			return(-1);
		if (outredir_j > 0)
			{
			if ( (ret = drop_arg(cargv, outredir_j)) == 0)
				j--;
			else
				return(-1);
			if ( (ret = drop_arg(cargv, outredir_j)) == 0)
				j--;
			else
				return(-1);
			}
		}
	if ( (outredir_j > 0) && (outredir_j > inredir_j) )
		{
		if ( (ret = drop_arg(cargv, outredir_j)) == 0)
			j--;
		else
			return(-1);
		if ( (ret = drop_arg(cargv, outredir_j)) == 0)
			j--;
		else
			return(-1);
		if (inredir_j > 0)
			{
			if ( (ret = drop_arg(cargv, inredir_j)) == 0)
				j--;
			else
				return(-1);
			if ( (ret = drop_arg(cargv, inredir_j)) == 0)
				j--;
			else
				return(-1);
			}
		}

	return(j);		/* return number of arguments */
	}  /* end of parse_cmdbuf */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	drop_arg(cmdargv[], drop_index)
	char	*cmdargv[];
	int	drop_index;
#else
int	drop_arg(char *cmdargv[], int drop_index)
#endif
	   /* this procedure takes a set of command arguments (as an
	NULL-terminated array of pointers to strings) and deletes
	the argument cmdarg[drop_index] from the list. 
	Example:

		drop_index = 2

		cmdargv[0] -> "/bin/ls"
		cmdargv[1] -> "-C"
		cmdargv[2] -> ">"
		cmdargv[3] -> "tmpfile"		
		cmdargv[4] -> NULL

	is converted to:

		cmdargv[0] -> "/bin/ls"
		cmdargv[1] -> "-C"
		cmdargv[2] -> "tmpfile"
		cmdargv[3] -> NULL 
		cmdargv[4] -> NULL

	It return 0 unless the index was out of range, in which case
	it returns -1 */
	{
	int	last;

	if (drop_index < 0) return(-1);	/* insurance */

	for (last = 0 ; last < drop_index ; last++)
		{
		if (cmdargv[last] == (char *)NULL)
			break;
/* *** DIAGNOSTIC ***
		printf("\r\n cmdargv[last=%d]=%s ", last, cmdargv[last]); */
		}
	if (cmdargv[last] == (char *)NULL)
		return(-1);		/* drop index too large */
	while (cmdargv[last] != (char *)NULL)
		{
		cmdargv[last] = cmdargv[last + 1];
/* *** DIAGNOSTIC ***
		if (cmdargv[last]  != (char *)NULL)
			printf("\r\n cmdargv[last=%d]=%s", last, cmdargv[last]);
		else
			printf("\r\n cmdargv[last=%d]=NULL", last); */
		last++;
		}

	return(0);
	}  /* end of drop_arg */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	expand_wcard(cmdto, cmdfrom[], wildcard_index)
	char	*cmdto[];
	char	*cmdfrom[];
	int	wildcard_index;
#else
int	expand_wcard(char *cmdto[], char *cmdfrom[], int wildcard_index)
#endif
	   /* this procedure takes a set of command arguments (as an
	NULL-terminated array of pointers to strings) such that the
	argument *cmdfrom[wildcard_index] contains a wildcard (* or ?).
	It expands the wildcard putting the result in a new 
	NULL-terminated array of pointers to strings cmdto[].
	Example:

		cmdfrom[0] -> "/bin/ls"
		cmdfrom[1] -> "-C"
		cmdfrom[2] -> "*.h"		(wildcard_index = 2)
		cmdfrom[3] -> NULL

	is converted to:

		cmdto[0] -> "/bin/ls"
		cmdto[1] -> "-C"
		cmdto[2] -> "startup.h"	
		cmdto[3] -> "cleanup.h"	
		cmdto[4] -> "specio.h"	
		cmdto[5] -> NULL 

		return(matches=3)

	Of course, the shell maximums: MAX_ARGLEN and MAX_ARGS must
	be observed. The procedure returns -2 if wildcard matching is
	not implemented for the current system, it returns -1 if 
	opendir(".") failed, it returns 0 if there were no matches, 
	otherwise it prints out the matches found and returns the 
	positive number (of matches). */
	{
	int	i, k, ret, tabstop, matches;

#if defined(_WIN95) || defined(_WINNT)

	struct	_finddata_t	fileinfo;
	long	hfile;		/* file handle for _findfirst().. */
#else

	DIR	*dfile;		/* for opendir(),readdir().. */
	struct	dirent	*sdfile;/* for opendir(),readdir().. */
#endif

	matches = 0;

		/* copy arguments up to cmdfrom[wildcard_index] */
	for (k = 0 ; k < wildcard_index ; k++)
		{
		if (cmdto[k] == (char *)NULL)
			cmdto[k] = (char *)malloc(MAX_ARGLEN);
		if (cmdto[k] == (char *)NULL)
			{
			fprintf(stderr, "\nfatal error: malloc 1\n");
			fflush(stderr);
			exit(-1);
			}
		strncpy(cmdto[k], cmdfrom[k], MAX_ARGLEN - 1);
		}
			/* set wildcard string to match */
	strcpy(searchpath, cmdfrom[wildcard_index]);

			/* searching for files in the current
		directory which match wildcards is VERY system dependent.. */

#if defined(_WIN95) || defined(_WINNT)

	hfile = _findfirst(searchpath, &fileinfo);
	if (hfile < 0L) ret = -1;
		else ret = 0;	/* valid handle */
	if (ret != 0)
		{
		return(0);	/* no matches at all */
		}
	else
		{
		printf(
		"\r\nexpand_wcard: wildcard matches for cmdfrom[%d]=\"%s\": ", 
			wildcard_index, cmdfrom[wildcard_index]);
		tabstop = 0;
		while( (ret == 0) && (k < (MAX_ARGS - 1)) )
			{
			if ((tabstop % 4) == 0)
		  		printf("\r\n");
			printf("  %15s", fileinfo.name); 
			tabstop++;

			if (cmdto[k] == (char *)NULL)
				cmdto[k] = (char *)malloc(MAX_ARGLEN);
			if (cmdto[k] == (char *)NULL)
				{
				fprintf(stderr, "\nfatal error: malloc 2\n");
				fflush(stderr); 
				exit(-1); 
				}
			strncpy(cmdto[k], fileinfo.name, MAX_ARGLEN - 1);
			matches++;
			k++;
				/* get next matching entry */
			ret = _findnext(hfile, &fileinfo);
			}  /* end of while */

		_findclose(hfile);
		}  /* end of else */

#elif defined(__alpha) || defined(__linux) || defined(__sun) || defined(__bsd)

			/* this is the POSIX method of searching a 
		directory (but you won't find it on OLDER ports of Unix) */
	dfile = opendir(".");
	if (dfile == (DIR *)NULL)
		{
		return(-1);	/* opendir() failed */
		}
	sdfile = readdir(dfile);
	if (sdfile == (struct dirent *)NULL)
		{
		return(0);	/* no matches at all */
		}
	ret = fnmatch(searchpath, sdfile->d_name, 0);
	printf("\r\nexpand_wcard: wildcard matches for cmdfrom[%d]=\"%s\": ", 
		wildcard_index, cmdfrom[wildcard_index]);
	tabstop = 0;
	while( (sdfile != (struct dirent *)NULL) && (k < (MAX_ARGS - 1)) )
		{
		if ( (ret == 0) && (k < (MAX_ARGS - 1)) )
			{
			if ((tabstop % 4) == 0) printf("\r\n");
			printf("  %15s", sdfile->d_name);
			tabstop++;

			if (cmdto[k] == (char *)NULL)
				cmdto[k] = (char *)malloc(MAX_ARGLEN);
			if (cmdto[k] == (char *)NULL)
				{
				fprintf(stderr, "\nfatal error: malloc 2\n");
				fflush(stderr);
				exit(-1);
				}
			strncpy(cmdto[k], sdfile->d_name, MAX_ARGLEN - 1);
			matches++;
			k++;
			}  /* end of if */
				/* get and test next entry */
		sdfile = readdir(dfile);
		if (sdfile != (struct dirent *)NULL)
		  	ret = fnmatch(searchpath, sdfile->d_name, 0);
		else
			ret = -1;
		}  /* end of while */
	closedir(dfile);

#else

	return(-2);		/* not implemented on this system */
#endif
		/* copy arguments after cmdfrom[wildcard_index] */
	if (cmdto[k] != (char *)NULL)
		{
		free(cmdto[k]);
		cmdto[k] = (char *)NULL;
		}
	i = wildcard_index + 1;
	while ( (cmdfrom[i] != (char *)NULL) &&
			(i < MAX_ARGS) && (k < (MAX_ARGS - 1)) )
		{
		if (cmdto[k] == (char *)NULL)
			cmdto[k] = (char *)malloc(MAX_ARGLEN);
		if (cmdto[k] == (char *)NULL)
			{
			fprintf(stderr, "\nfatal error: malloc 3\n");
			fflush(stderr);
			exit(-1);
			}

/* *** DIAGNOSTIC *** 
				fprintf(stderr,
	"\r\nstrncpy(cmdto[k=%d],cmdfrom[i=%d]=\"%s\" ", k, i, cmdfrom[i]); */

		strncpy(cmdto[k], cmdfrom[i], MAX_ARGLEN - 1);
		i++;
		}
	k++;
	if (cmdto[k] != (char *)NULL)
		{
		free(cmdto[k]);
		cmdto[k] = (char *)NULL;
		}

	return(matches);
	}  /* end of expand_wcard */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	get_st_mode(sptr, str)
	struct	stat	*sptr;
	char	str[];
#else
int	get_st_mode(struct stat *sptr, char str[])
#endif
	   /* this checks the field sptr->st_mode and forms a
	display string in the return parameter str[]. It also
	gets the owner's UID and the group's GID. It is 
	the CALLER's responsibility to see that SPACE IS AVAILABLE
	for 31 chars plus EOS. */
	{
	int	i, mode, modetest;
	char	fixed[32];

	strcpy(fixed, "rwxrwxrwx");	/* "cc" on some systems
			doesn't allow automatic initialization of
			strings */
	mode = sptr->st_mode;	
	sprintf(str,  "--------- uid=%5d gid=%5d",
		(int)(sptr->st_uid), (int)(sptr->st_gid));
	modetest = 0400;
	for (i = 0 ; i < 9 ; i++)
		{
		if (modetest & mode)
			str[i] = fixed[i];
		modetest = modetest >> 1;
		}
	if (mode & 07000)
		str[2] = 's';		/* set uid */
	return(0);
	}  /* end of get_st_mode */
