
					/* This program calls and tests
			procedure proc_info(). This procedure prints out
			the context in which the process is currently
			operating. 


						Marc Thomas */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>		/* for getpriority(), getrlimit() */
#include <sys/resource.h>	/* for getpriority(), getrlimit() */
#include <unistd.h>
#include <sys/stat.h>		/* for stat(), fstat() */
#include <sys/ioctl.h>		/* for ioctl() */
#include <fcntl.h>		/* for fcntl() */
#include <signal.h>		/* for sigprocmask() */

#if defined(__sun)

#define	_iob		__iob	/* "typical" for Solaris */
#include <sys/mkdev.h>		/* for major(), minor() */
#include <termios.h>
#include <strings.h>		/* Sun <string.h> does NOT automatically
				   include the bzero() functions of
				   <strings.h> */
#include <sys/filio.h>		/* Sun <ioctl.h> does NOT automatically
				   include the FIONBIO-type defines due
				   to its desire to be both System V
				   AND BSD */
#include <sys/sockio.h>		/* Sun <ioctl.h> does NOT automatically
				   include the SIOCGIFCONF-type defines
				   from sockio.h */
#elif defined(__vax) || defined(__mips)

#include <ttyent.h>		/* for getttyent() */
#endif

#include <sys/socket.h>		/* for socket() */
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyname() */
#include <pwd.h>		/* for getpwuid() */
#include <grp.h>		/* for getgrgid() */
#include <time.h>		/* for time(), ctime() */

			/* conditional includes/structs due to Unix
			differences: ultrix (BSD) vs. DECunix (OSF) */
#if defined(__sun)

#define	PGRP_PARAM	
#include <utmp.h>		/* for OSF getutent() */
#elif defined(__vax) || defined(__mips)

#define	_NOPROTO	0
#define	PGRP_PARAM	0
#elif defined(__alpha) 

#define	PGRP_PARAM	
#include <utmp.h>		/* for OSF getutent() */
#elif defined(__linux)

#define	PGRP_PARAM	
#include <sys/sysmacros.h>	/* for major(), etc.  */
#include <utmp.h>		/* for OSF getutent() */
#endif
#define UINET_32	unsigned 

			/* defines for usr of proc_info() */
#define PI_NOTHING	00000
#define	PI_TIME		00001
#define	PI_PROCESS	00002
#define	PI_UGIDS	00004
#define PI_PRIORITY	00010
#define	PI_TTY		00020
#define	PI_CWD		00040
#define	PI_DESCRIPT	00100
#define PI_MEMORY	00200
#define PI_IOBLOCKS	00400	
#define	PI_SIGNALMASK	01000
#define PI_ALLINFO	01777

			/* forward references */
#if defined(_NOPROTO)
int	proc_info();
#else
int	proc_info(FILE *, int);
#endif

/* ------------------------------------------------------------------- */

#if defined(_NOPROTO)
main(argc,argv,envp)
int	argc;			/* number of command line arg's */
char	*argv[],*envp[];	/* argument and environmental pointers */
#else
int main(int argc, char *argv[], char *envp[])
#endif
	{
	int	i, ret, not_found, bit_field;
	int	pipedesc[2];	/* to test pipe desciptors */
	int	sock;		/* to test socket descriptor */
	struct	sockaddr_in	name;
	char	answer[64];
	sigset_t	intrmask, oldmask;

	i = 1;			/* parse command line */
	not_found = 1;
	answer[0] = '\0';
	while( (i < argc) && (argv[i] != NULL) && (not_found) )
		{
		if (strncmp(argv[i], "-?", 2) == 0)
			{
			not_found = 1;
			printf("\nInvocation: process bit_field");
			break;
			}
		if (strncmp(argv[i], "0", 1) == 0) 	/* octal */
			{
			sscanf(argv[i], "%o", &bit_field);
			not_found = 0;
			}
		i++;
		}
				/* if (not_found) handle below.. */

				/* open some stuff to test */
	ret = pipe(pipedesc);	/* pipedesc[0]=read, pipedesc[1]=write */
	if (ret == -1)
		{
		perror("\n pipe:");
		return(-1);
		}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		{
		perror("\n socket:");
		close(pipedesc[0]);
		close(pipedesc[1]);
		return(-1);
		}
				/* bind socket */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;  
				/* INADDR_ANY = assigned by system */
	name.sin_port = htons(0);	/* 0 = assigned by system */
	ret = bind(sock, (struct sockaddr *)&name, sizeof name);
	if (ret < 0)
		{
		perror("\n bind:");
		close(sock);
		close(pipedesc[0]);
		close(pipedesc[1]);
		return(-1);
		}
				/* set a signal mask */
	sigemptyset(&intrmask);
	sigemptyset(&oldmask);
	sigaddset(&intrmask, SIGCLD);
	sigaddset(&intrmask, SIGQUIT);
				/* block and hold SIGQUIT SIGCLD */
	sigprocmask(SIG_BLOCK, &intrmask, &oldmask);

				/* .. ask user */
	if (not_found)
		{
		printf("\n  octal constant bit_field is OR'ed from:");
		printf("\n   PI_NOTHING         00000 ");
		printf("\n   PI_TIME            00001 ");
		printf("\n   PI_PROCESS         00002 ");
		printf("\n   PI_UGIDS           00004 ");
		printf("\n   PI_PRIORITY        00010 ");
		printf("\n   PI_TTY             00020 ");
		printf("\n   PI_CWD             00040 ");
		printf("\n   PI_DESCRIPT        00100 ");
		printf("\n   PI_MEMORY          00200 ");
		printf("\n   PI_IOBLOCKS        00400 ");	
		printf("\n   PI_SIGNALMASK      01000 ");	
		printf("\n   PI_ALLINFO         01777 ");
		printf("\nEnter bit_field: ");

/* *** OLD *** gets() is dangerous
		gets(answer);	even though it converts '\n' to '\0' */

		fgets(answer, sizeof(answer) - 1, stdin);
		for (i = 0 ; i < sizeof(answer) - 1 ; i++)
			{
			if ( (answer[i] == '\n') || (answer[i] == '\0') )
				break;
			}
		if (answer[i] == '\n')
			answer[i] = '\0'; 	/* get rid of newline */

		sscanf(answer, "%o", &bit_field);
		}
	if (bit_field < 0)
		{
		printf("\nError: bad bit_field");
		goto done;
		}

				/* print information on this process */	
	ret = proc_info(stderr, bit_field);

done:				/* clean up */
	close(sock);
	close(pipedesc[0]);
	close(pipedesc[1]);

	return(0);
	}  /* end of main */

/* ------------------------------------------------------------------- 
Utility Procedures:
			proc_info(FILE *, int)
   ------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	proc_info(fp, type)
FILE	*fp;
int	type;
#else
int	proc_info(FILE *fp, int type)
#endif
			/* This procedure prints out process info onto 
		the FILE fp (which may be stdout or stderr) and varies
		the amount of information depending which bits of
		the integer type are set. Use the symbolic names
		as follows:

			PI_NOTHING	show nothing
			PI_TIME		show current day and time
			PI_PROCESS	show PID, PPID, PGRP, etc.
			PI_UGIDS	show UID, EUID, GID, EGID, etc.
			PI_PRIORITY	show process priority
			PI_TTY		show TTYGRP and /dev/tty
			PI_CWD		show current working directory
			PI_DESCRIPT	show descriptors: files, pipes, etc.
			PI_MEMORY	show VSZ and RSS sizes, etc.
			PI_IOBLOCKS	show _iob[] in detail
			PI_SIGNALMASK	show the current signal mask
			PI_ALLINFO	show all of the above

		One MAJOR problem is systems (e.g. ULTRIX) which
		have ONLY the first three entrie of _iob[] STATIC.
		Hence the use of the fdopen(), fcntl() feof(), ferror() 
		etc...  calls below.
							*/
	{
	int	i, ret;			/* return value */
	char	*retp;			/* return pointer */
	time_t	ulongtime;		/* seconds since... *e... */
	char	asciitime[64];		/* easier to read */
	int	pid, ppid, pgrp;	/* process ids */
	int	ppriority;		/* process priority */
	int	uid, euid, gid, egid;	/* user, group ids */
	char	uid_name[32],
		euid_name[32],
		gid_name[32],	
		egid_name[32];		/* symbolic names of above */
	struct	passwd	*pwdptr;	/* pointer to passwd struct */
	struct	sockaddr sname;		/* for getsockname() */
	int	slen;			/* length of socket name */
	struct	group	*grpptr;	/* pointer to group struct */
	int	ttygrp;			/* terminal ids */
	struct	utmp *utmpptr;		/* pointer to utmp struct */
	char	cwd[128];		/* current working dir */
	mode_t	oldmask, newmask;	/* file creation masks */
	struct	stat	desc_info;	/* descriptor info */
	int	under_dev;		/* if filename under /dev */
	int	devmajor, devminor;	/* major-minor device nums */
	int	max_desc;
	sigset_t	current_mask;	/* signal mask */
	FILE	*iob_ptr;		/* need this because some
				systems have _iob[] with ONLY 3 STATIC
				entries */
#if defined(__vax) || defined(__mips)
	struct	ttyent	*ttyptr;	/* pointer to ttys struct */
#endif

#if !defined(__linux)
	int	iob_ptr_flag;		/* essentially (int)(_iob[]._flag) */
#endif
	struct	rlimit	rss_datalims;	/* fields are .rlim_cur .rlim_max */
	struct	rlimit	rss_stacklims;
	struct	rusage	own_usage;

	if (type <= 0)
		goto done;		/* nothing more to say */

	if (type & PI_TIME)		/* time is ONLY used here */
	  {	
	  ulongtime = time(&ulongtime);
	  fprintf(fp, "\n");
	  strncpy(asciitime, ctime(&ulongtime), 25);
	  asciitime[24]= '\0';		/* get rid of CR */
	  fprintf(fp, "%24s", asciitime);
	  }
					/* get process IDs always */
	pid = getpid();
	ppid = getppid();
	pgrp = getpgrp(PGRP_PARAM);	/* 0 = "this process" */
	if (type & PI_PROCESS)
	  {
	  fprintf(fp, "\nProcess    IDs: PID=%5d PPID=%5d PGRP=%5d ", 
		pid, ppid, pgrp);
	  if (pid == pgrp)
		fprintf(fp, "(group leader)");
	  }
					/* get user, group IDs always */
	uid = getuid();
	euid = geteuid();
	gid = getgid();
	egid = getegid();
	if (type & PI_UGIDS)
	  {
	  fprintf(fp, 
		"\nUser/Group IDs: UID=%5d EUID=%5d  GID=%5d EGID=%5d ",
		uid, euid, gid, egid); 
	  }
					/* get symbolic names for user,
					groups, if needed */
	if (type & PI_UGIDS)
	  {
	  pwdptr = getpwuid(uid);
	  if (pwdptr != NULL)
	  	strncpy(uid_name, pwdptr->pw_name, sizeof(uid_name)-1);
	  else
		strcpy(uid_name, "--na--");
	  pwdptr = getpwuid(euid);
	  if (pwdptr != NULL)
	  	strncpy(euid_name, pwdptr->pw_name, sizeof(euid_name)-1);
	  else
		strcpy(euid_name, "--na--");
	  grpptr = getgrgid(gid);
	  if (grpptr != NULL)
		  strncpy(gid_name, grpptr->gr_name, sizeof(gid_name)-1);
	  else
		strcpy(gid_name, "--na--");
	  grpptr = getgrgid(egid);
	  if (grpptr != NULL)
	  	strncpy(egid_name, grpptr->gr_name, sizeof(egid_name)-1);
	  else
		strcpy(egid_name, "--na--");
	  fprintf(fp, 
		"\n         Names: (%8.8s) (%8.8s) (%8.8s) (%8.8s) ",
		uid_name, euid_name, gid_name, egid_name); 
	  }
					/* get process priority if
					needed and expect range in
					[-20, 20] */
	if (type & PI_PRIORITY)
	  {
	  errno = 0;			/* clear errno */
	  ppriority = getpriority(PRIO_PROCESS, pid);
	  if ( (ppriority != -1) ||
		( (ppriority == -1) && (errno == 0) ) )
		fprintf(fp, 
			"\nProc  Priority: PROCPRI=%2d (range=[-20,20])", 
			ppriority);
	  }

					/* format of an ioctl() call
			is ioctl(descriptor, REQUEST, answer). This is
			VERY MACHINE DEPENDENT and, since answer is a 
			pointer to a variety of types we MUST use a 
			cast below. ALWAYS CONSULT the file:
			/usr/include/sys/ioctl.h */

	if (type & PI_TTY)
	  {
	  ret = ioctl(0, TIOCGPGRP, (char *)&ttygrp);
	  if (ret < 0)
		{
		perror("\n  ioctl error:");
		}
	  else
		{
		fprintf(fp, "\nTerminal   IDs: TTYGRP=%5d ", ttygrp);
		if (ttygrp == pid) fprintf(fp, "(controlling) ");
		ret = ttyslot();
				/* use /etc/ttys on BSD */
#if defined(__vax) || defined(__mips)
		if ( (ret > 0) && (ret < 999) )
			{
			for (i = 0 ; i < ret ; i++)
				ttyptr = getttyent();	/* line by line */
			fprintf(fp, "name=/dev/%s ", ttyptr->ty_name);
			}
		endttyent();	/* close (so that next will rewind) */
#else

/* ***OLD *** use /etc/utmp on SysV, OSF 
		if ( (ret > 0) && (ret < 999) )
			{
			for (i = 0 ; i <= ret ; i++)	
				utmpptr = getutent();
			fprintf(fp, "name=/dev/%s ", utmpptr->ut_line);
			}
		endutent();	close (so that next will rewind) */
/* *** NEW *** */
		fprintf(fp, "name=%s", ttyname(0));
#endif
		}
	  }  /* end of if (type & PI_TTY)... */

	if (type & PI_CWD)
	  {
	  retp = getcwd(cwd, sizeof(cwd));/* get cwd[] */
	  if (retp == NULL)
		perror("\n  getcwd error:");
	  else
		fprintf(fp, "\nCurr Directory: CWD=%s", cwd);
	  }

	newmask = 022;			/* need something for umask */
	oldmask = umask(newmask);
	if (type & PI_DESCRIPT)
	  {
	  fprintf(fp, "\nFile  Creation: UMASK=0%o", (unsigned int)oldmask);
	  }
	newmask = umask(oldmask);	/* restore */

	getrlimit(RLIMIT_DATA, &rss_datalims);
	getrlimit(RLIMIT_STACK, &rss_stacklims);
	getrusage(RUSAGE_SELF, &own_usage);	
	if (type & PI_MEMORY)		/* check memory usage */
	  {
	  fprintf(fp, 
    "\nRSS Limits:     DATA(cur=%6dk  max=%6dk) STACK(cur=%6dk  max=%6dk)",
		(int)rss_datalims.rlim_cur/1000, 
		(int)rss_datalims.rlim_max/1000,
		(int)rss_stacklims.rlim_cur/1000, 
		(int)rss_stacklims.rlim_max/1000);
	  fprintf(fp, "\nRSS Usage:       current=%6d.%3dk ",
		(int)((own_usage.ru_idrss + own_usage.ru_isrss)/1000),
		(int)((own_usage.ru_idrss + own_usage.ru_isrss) % 1000));
	  fprintf(fp, 
	    "\nVirtual Memory: minpageflts=%6d majpageflts=%6d swaps=%6d ",
		(int)own_usage.ru_minflt, 
		(int)own_usage.ru_majflt, 
		(int)own_usage.ru_nswap);
	  }

	max_desc = getdtablesize();	/* look at descriptors in _iob[] */
	if (type & PI_DESCRIPT)
	  {
	  fprintf(fp, "\nIO Descriptors: ");
	  }

#if defined(__linux)

	for (i = 0 ; i < max_desc ; i++)
		{
		ret = fstat(i, &desc_info);
		if (ret != 0)
			{
			;		/* ignore since these descriptors
					are just inactive at present */
			}
		else
			{		/* NO STATIC _iob's in __linux */
	
					/* try modes: "r" "w" */
			iob_ptr = fdopen(i, "r");
			if (iob_ptr == NULL)
			  {
			  iob_ptr = fdopen(i, "w");
			  if (iob_ptr == NULL)
			    {
			    iob_ptr = fdopen(i, "a");
			    if (iob_ptr == NULL)
			      {
			      continue; /* give up!!! */
			      }
			    }
			  }
			if (type & PI_DESCRIPT)
			  {
			  fprintf(fp, "\n  _iob[%2d] inode=%8lu ", i,
				(unsigned long)desc_info.st_ino);
			  fprintf(fp, ".st_mode=%7.7o type=", 
				desc_info.st_mode);
			  }
			under_dev = 0;/* this is used by PI_DESCRIPT */
					/* S_IFMT is mask for type bits */
			switch(desc_info.st_mode & S_IFMT) {
			  case S_IFDIR:
				if (type & PI_DESCRIPT)
					fprintf(fp, "       directory"); 
				break;
			  case S_IFCHR:
				if (type & PI_DESCRIPT)
					fprintf(fp, "    char special"); 
				under_dev = 1; break;
			  case S_IFBLK:
				if (type & PI_DESCRIPT)
					fprintf(fp, "   block special"); 
				under_dev = 1; break;
			  case S_IFREG:
				if (type & PI_DESCRIPT)
					fprintf(fp, "    regular file"); 
				break; 
			  case S_IFLNK:
				if (type & PI_DESCRIPT)
					fprintf(fp, "        sym link"); 
				break;
			  case S_IFSOCK:
					/* Some systems (BSD) indicate a
					socket this way... */
				if (type & PI_DESCRIPT)
					fprintf(fp, "          socket"); 
				break;
			  case S_IFIFO:
				if (type & PI_DESCRIPT)
					fprintf(fp, "       pipe/fifo"); 
				break;
			  case       0:
				if (desc_info.st_blksize != 0) 
					{
					/* see if we have a socket */
					ret = getsockname(i, &sname, &slen);
					if (ret == 0)
						{
						if (type & PI_DESCRIPT)
							fprintf(fp, 
						"          socket");
						}
					else
						{
						if (type & PI_DESCRIPT)
							fprintf(fp, 
						"            pipe");
						}
					break;
					}

			default:
				if (type & PI_DESCRIPT)
					fprintf(fp, "         unknown");

				break;
				}  /* end of switch */

				/* you can find the relationship
				between major-minor device number and
				its st_dev by looking at the macros:
					major()
					minor()
					makedev()
				in /usr/include/sys/types.h */
			  if (desc_info.st_dev > 0)
				{
				devmajor = (int)major(desc_info.st_dev);
				devminor = (int)minor(desc_info.st_dev);
			  	if ((type & PI_DESCRIPT) == 0)
						fprintf(fp,
			"\n  _iob[%2d] major=%4d minor=%4d ", 
				i, devmajor, devminor);
				else
						fprintf(fp, 
			"\n           major=%4d minor=%4d ", 
				devmajor, devminor);
				}

			if (type & PI_IOBLOCKS)
		 	  {
			  if ((type & PI_DESCRIPT) == 0)
			  	fprintf(fp, "\n   _iob[%2d].file=   %5hd", 
					i, i);
			  else
		  	  	fprintf(fp, "\n          ._file=   %5hd", 
					i);
				  /* do snooping with fcntl() if
				  we are past the 3 static _iob[]'s */
			  ret = fcntl(i, F_GETFL, 0);
			  if (ret >= 0)
			      {
			      fprintf(fp, " ._flag= (0x%x) ", ret);
			      if (ret & O_RDWR)
				fprintf(fp, "READ WRT ");
			      else if (ret & O_WRONLY)
				fprintf(fp, "WRT ");
			      else 
				fprintf(fp, "READ ");
			      if (ret & O_APPEND)
				fprintf(fp, "APPEND ");
			      if (ret & O_NONBLOCK)
				fprintf(fp, "NBF ");
			      if (ret & O_NDELAY)
				fprintf(fp, "NDELAY ");
					/* use feof(), ferror() */
			      if (feof(iob_ptr))
				fprintf(fp, "EOF ");
			      if (ferror(iob_ptr))
				fprintf(fp, "ERR ");
			      }  /* end of if (ret >= 0) */
		  	  }  /* end of if (type & PI_IOBLOCKS) */

			}  /* end of if..else.. */
		}  /* end of for(i = 0 ; i < NFILES ; i++) */
#else
					/* non-__linux */	
	for (i = 0 ; i < max_desc ; i++)
		{
		ret = fstat(i, &desc_info);
		if (ret != 0)
			{
			;		/* ignore since these descriptors
					are just inactive at present */
			}
		else
			{		
			if ( (i >= 0) && (i < 3) )
				{
				iob_ptr = &_iob[i];	/* always STATIC */
				}
			else
				{
					/* try modes: "r" "w" */
				iob_ptr = fdopen(i, "r");
				if (iob_ptr == NULL)
				  {
				  iob_ptr = fdopen(i, "w");
				  if (iob_ptr == NULL)
				    {
				    iob_ptr = fdopen(i, "a");
				    if (iob_ptr == NULL)
				      {
				      continue; /* give up!!! */
				      }
				    }
				  }
				}
			if (type & PI_DESCRIPT)
			  {
			  fprintf(fp, "\n  _iob[%2d] inode=%8lu ", i,
				(unsigned long)desc_info.st_ino);
			  fprintf(fp, ".st_mode=%7.7o type=", 
				(unsigned int)desc_info.st_mode);
			  }
			under_dev = 0;/* this is used by PI_DESCRIPT */
					/* S_IFMT is mask for type bits */
			switch(desc_info.st_mode & S_IFMT) {
			  case S_IFDIR:
				if (type & PI_DESCRIPT)
					fprintf(fp, "       directory"); 
				break;
			  case S_IFCHR:
				if (type & PI_DESCRIPT)
					fprintf(fp, "    char special"); 
				under_dev = 1; break;
			  case S_IFBLK:
				if (type & PI_DESCRIPT)
					fprintf(fp, "   block special"); 
				under_dev = 1; break;
			  case S_IFREG:
				if (type & PI_DESCRIPT)
					fprintf(fp, "    regular file"); 
				break; 
			  case S_IFLNK:
				if (type & PI_DESCRIPT)
					fprintf(fp, "        sym link"); 
				break;
			  case S_IFSOCK:
					/* Some systems (BSD) indicate a
					socket this way... */
				if (type & PI_DESCRIPT)
					fprintf(fp, "          socket"); 
				break;
			  case S_IFIFO:
				if (type & PI_DESCRIPT)
					fprintf(fp, "       pipe/fifo"); 
				break;
			  case       0:
				if (desc_info.st_blksize != 0) 
					{
					/* see if we have a socket */
					ret = getsockname(i, &sname, &slen);
					if (ret == 0)
						{
						if (type & PI_DESCRIPT)
							fprintf(fp, 
						"          socket");
						}
					else
						{
						if (type & PI_DESCRIPT)
							fprintf(fp, 
						"            pipe");
						}
					break;
					}

			default:
				if (type & PI_DESCRIPT)
					fprintf(fp, "         unknown");

				break;
				}  /* end of switch */

				/* you can find the relationship
				between major-minor device number and
				its st_dev by looking at the macros:
					major()
					minor()
					makedev()
				in /usr/include/sys/types.h */
			  if (desc_info.st_dev > 0)
				{
				devmajor = (int)major(desc_info.st_dev);
				devminor = (int)minor(desc_info.st_dev);
			  	if ((type & PI_DESCRIPT) == 0)
						fprintf(fp,
			"\n  _iob[%2d] major=%4d minor=%4d ", 
				i, devmajor, devminor);
				else
						fprintf(fp, 
			"\n           major=%4d minor=%4d ", 
				devmajor, devminor);
				}

			if (type & PI_IOBLOCKS)
		 	  {
			  if ((type & PI_DESCRIPT) == 0)
			  	fprintf(fp, "\n   _iob[%2d].file=   %5hd", 
					i, iob_ptr->_file);
			  else
		  	  	fprintf(fp, "\n          ._file=   %5hd", 
					iob_ptr->_file);
			  if (i < 3) 
			    {
			    iob_ptr_flag = (int)(iob_ptr->_flag);
			    fprintf(fp, " ._flag= (0x%hx) ", iob_ptr_flag);
			    if (iob_ptr_flag & _IOREAD)
				fprintf(fp, "READ ");
			    if (iob_ptr_flag & _IOWRT)
				fprintf(fp, "WRT ");
			    if (iob_ptr_flag & _IONBF)
				fprintf(fp, "NBF ");
			    if (iob_ptr_flag & _IOMYBUF)
				fprintf(fp, "MYBUF ");
			    if (iob_ptr_flag & _IOEOF)
				fprintf(fp, "EOF "); 
			    if (iob_ptr_flag & _IOERR)
				fprintf(fp, "ERR ");
#if defined(__sun)

#else
			    if (iob_ptr_flag & _IOSTRG)
				fprintf(fp, "STRG ");
#endif
			    if (iob_ptr_flag & _IOLBF)
				fprintf(fp, "LBF ");
			    if (iob_ptr_flag & _IORW)
				fprintf(fp, "RW ");
#if defined(_IOAPPEND)
			    if (iob_ptr_flag & _IOAPPEND)
				fprintf(fp, "APPEND ");
#endif
			/* next new flag(s) for _THREAD_SAFE */
#if defined(_IOINUSE)
			    if (iob_ptr_flag & _IOINUSE)
				fprintf(fp, "INUSE ");
#endif
#if defined(_IONOINUSE)
			    if (iob_ptr_flag & _IONOINUSE)
				fprintf(fp, "NOINUSE ");
#endif

#if defined(_IONONSTD)
			    if (iob_ptr_flag & _IONONSTD)
				fprintf(fp, "NONSTD ");
#endif
			    }  /* end of if (i < 3) ...
				  do more snooping with fcntl() if
				  we are past the 3 static _iob[]'s */
			  else
			    {
			    ret = fcntl(i, F_GETFL, 0);
			    if (ret >= 0)
			      {
			      fprintf(fp, " ._flag= (0x%x) ", ret);
			      if (ret & O_RDWR)
				fprintf(fp, "READ WRT ");
			      else if (ret & O_WRONLY)
				fprintf(fp, "WRT ");
			      else 
				fprintf(fp, "READ ");
			      if (ret & O_APPEND)
				fprintf(fp, "APPEND ");
			      if (ret & O_NONBLOCK)
				fprintf(fp, "NBF ");
			      if (ret & O_NDELAY)
				fprintf(fp, "NDELAY ");
					/* use feof(), ferror() */
			      if (feof(iob_ptr))
				fprintf(fp, "EOF ");
			      if (ferror(iob_ptr))
				fprintf(fp, "ERR ");
			      }  /* end of if (ret >= 0) */
			    }  /* end of else */
		  	  }  /* end of if (type & PI_IOBLOCKS) */

			}  /* end of if..else.. */
		}  /* end of for(i = 0 ; i < NFILES ; i++) */
#endif
	
	if (type & PI_SIGNALMASK)
	  {
	  fprintf(fp, "\nCurrent Signal Mask: ");
			/* since the second argument is NULL the
		only thing which will be done here is to get the
		current mask in current_mask */
	  sigprocmask(SIG_BLOCK, NULL, &current_mask);
			/* sigset_t is usually implemented either as a 
		64-bit unsigned long or as an array of (shorter) unsigned
		ints but we will use the macro sigismember() to
		set what has been set. if sigprocmask() has never
		been called this is usually the zeromask. also many
		systems now have more than 32 signals. */
	  for (i = 1 ; i < 32 ; i++)
		{
		if (sigismember(&current_mask, i))
			fprintf(fp, "1");
		else
			fprintf(fp, "0");
		}
			/* print out IMPORTANT signals */
	  if (sigismember(&current_mask, SIGHUP))
			fprintf(fp, " HUP");
	  if (sigismember(&current_mask, SIGINT))
			fprintf(fp, " INT");
	  if (sigismember(&current_mask, SIGQUIT))
			fprintf(fp, " QUIT");
	  if (sigismember(&current_mask, SIGSEGV))
			fprintf(fp, " SEGV");
	  if (sigismember(&current_mask, SIGALRM))
			fprintf(fp, " ALRM");
	  if (sigismember(&current_mask, SIGPROF))
			fprintf(fp, " PROF");
	  if (sigismember(&current_mask, SIGVTALRM))
			fprintf(fp, " VTALRM");
	  if (sigismember(&current_mask, SIGTERM))
			fprintf(fp, " TERM");
	  if (sigismember(&current_mask, SIGPOLL))
			fprintf(fp, " POLL");
	  if (sigismember(&current_mask, SIGCLD))
			fprintf(fp, " CLD");
	  if (sigismember(&current_mask, SIGUSR1))
			fprintf(fp, " USR1");
	  if (sigismember(&current_mask, SIGUSR2))
			fprintf(fp, " USR2");
	  }

	fprintf(fp, "\n");		/* final CR */
done:
	fflush(fp);			/* flush stream */
	return(0);
	}  /* end of proc_info */

