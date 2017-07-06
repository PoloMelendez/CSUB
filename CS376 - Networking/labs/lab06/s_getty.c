

/* s_getty:	GENERAL:
			This program is a UNIX-OS/2 "raw socket version"
			of getty. It first opens a virtual circuit (STREAM)
			socket in the Internet Domain. If you specified
			a portnumber (which should be >= 1024) it will
			try to bind that name to it, otherwise it will
			let the system assign the portnumber. In any
			case it displays the portnumber if all went well.
			The UNIX invocation line should be as follows:

			$ s_getty [-v] [optional portnumber] &
			   or
			$ nohup s_getty [optional portnumber] &

			where the "-v" is a "verbose flag" showing more
			information as s_getty runs. s_getty then calls
			listen() once and sets up a queue for at most
			MAX_SESS pending sessions, and blocks in accept().
				The caller's host is first checked by
			the procedure check_host() and, if UNACCEPTABLE,
			the connection is terminated. Otherwise, the user
			is prompted:

			login: 

			to enter a username. If the username, as determined
			by the procedure check_user() is UNACCEPTABLE
			for MAX_TRIES times, then the connection
			is terminated. Otherwise, s_getty accepts
			the connection, opens a pseudo-terminal and
			forks an offspring process (pre_login:) at
			the slave end (ttyp[0-f]). Now the user is prompted:

			socket_code:

			to enter a password for the socket. There is
			CONDITIONAL CODE here depending upon whether
			the host uses C2-style Security (USE_C2),
			the host uses BSD4.3-style password checking
			(USE_BSD43), or does no checking at all. 
				Note that use of C2 requires either 
			that the user is in GROUP: authread (UNLIKELY) 
			or that the progam uses UNIX suid/sgid features
			which have been enabled by the system administrator
			(MORE LIKELY). If a user simply wants to open
			a socket for his/her private use, OF COURSE
			NO CLEARANCE is necessary.
				If the first or second alternative
			is used the user gets MAX_TRIES attempts
			to give a valid socket code. If successful,
			the offspring process (pre_login:) exec()'s
			a USER shell. Then the parent simply passes
			information between the message-socket (not the
			listen-socket) and the master end (ptyp[0-f]),
			and the USER shell reads from the slave end.
			s_getty expects its caller to use our "raw socket
			telnet", s_tlnt, as follows:

			$ s_tlnt [hostname] [portnumber]
				or
			s_tlnt> open [hostname] [portnumber]

			When the session (i.e. from the time of
			attachment until an exception or null read on
			select()) is over, s_getty currently kills its
			offspring and goes back to listen(). It will also
			respond to two control characters from the caller
			as follows:

			GET_REINIT	kill shell and re-initialize.
			GET_DOKILL	kill shell and terminate.

			The latter sequence is the preferred way to
			finally STOP a running s_getty. However, if s_getty
			becomes locked, use:

			$ kill -9 [s_getty's pid]

			Note that this latter action does no harm since
			all sockets will be closed automatically when 
			a process terminates. 

		OS/2 DIFFERENCES:
			The main difference here is that pre_login: runs
			concurrently as a THREAD, NOT as an OFFSPRING
			PROCESS. This is due to the problem of OS/2 not
			having an exact analogue of FORK, and makes the
			situation more efficient but also MORE DELICATE,
			since main() and pre_login() share descriptors.
			Another complication is the spawn()'d shell.
			Since the OS/2 kernel does not support socket
			descriptors (ftp software's SOCKET.DLL is doing
			some trapping) the user shell MUST BE WRITTEN
			using _fdinternalize to pick up the socket-pair
			descriptor from pre_login:.

		MAKING:
			Under UNIX, compilation and linking is straight-
			forward, just make sure you add "-lauth" to
			makefile when building under Ultrix with C2.
			OS/2 compilation and linking is more involved and
			it is easier to give a sample makefile (which
			should be run with Microsoft's NMAKE) as follows:

CC	= cl
DBFLAGS	= -Zi -Zd
OPFLAGS	= -Ot
CFLAGS	= -AL -Od -G2 -MT -D_DLL -W1 -DOS2
LFLAGS	= /NOD /NOI /NOE /CO /STACK:8192
LIBS	= pctcp bsd ftpcrt socket os2 llibcep

default:	s_tlnt.exe s_getty.exe s_sh.exe
all:	s_tlnt.exe s_getty.exe s_sh.exe

s_tlnt.exe:	s_tlnt.obj
	$(CC) -o s_tlnt.exe s_tlnt.obj $(CFLAGS) \c600\lib\setargv.obj 
-link $(LIBS) $(LFLAGS)

s_tlnt.obj:
	$(CC) -c $(CFLAGS) s_tlnt.c

s_getty.exe:	s_getty.obj
	$(CC) -o s_getty.exe s_getty.obj $(CFLAGS) \c600\lib\setargv.obj 
-link $(LIBS) $(LFLAGS)

s_getty.obj:
	$(CC) -c $(CFLAGS) s_getty.c

s_sh.exe:	s_sh.obj
	$(CC) -o s_sh.exe s_sh.obj $(CFLAGS) \c600\lib\setargv.obj 
-link $(LIBS) $(LFLAGS)

s_sh.obj:
	$(CC) -c $(CFLAGS) s_sh.c

			Note that some of the lines above are NOT
			continuation lines, they are LONGER than 80
			characters.

						Marc Thomas

Functional Diagram (under OS/2, pre_login: is a thread not a process):

           S_TLNT:
           --------------------
          | command_mode       |                S_GETTY:
          |                    |                --------------------
          |                    |               |         close(tty) |
          |                    |               |           close(0) |
          |      telnet_open() |    TCP/IP     |           close(1) | stderr
          |          connect() |---------------|                    |----------
          |                   ---->          ---->                ---->
          |                    |---------------| listen()           |----------
stdio     |                    |           sock|                    |
----------| echo_noraw()       |               |                    |
<--    --> --------------------|               |                    |
----------| fionbio_select()   |               |                    |
          |          |         |    TCP/IP     |                    |
          |          |         |---------------| accept()           |
          |       select() --<----           ---->--   check_host() |
          |                    |---------------|    |     "login: " |
          |                    |sock    msgsock|    |  check_user() |
          |                    |               |    |               |
          |                    |               |    |     ioctl(ptty,FIONBIO,)
          |                    |               |    select()        |
          | terminal_mode      |               |       |            |
           --------------------                |       |  kill(9, opid)
                                               |       |  wait(&ostatus)
                                               |       |  exit(0)   |
                                                ------ | -----------
S_TLNT CONTROL-codes:                   master: ptty | | | open_psterm()
1. CTRL-T  BREAK to command_mode                     |   |
2. CTRL-^  re-init S_GETTY                           |   |
3. CTRL-K  do-kill on S_GETTY             slave: tty |   |           
                                                ------   -----------
                            fork(): PRE-LOGIN: |     close(msgsock) |
                                               |        close(ptty) |
                                               |        dup2(tty,0) |
                                               |        dup2(tty,1) |
                                               |        dup2(tty,2) |
                                               |                    |
                                               |      raw_nocecho() |
                                               |    "socket_code: " |
                                               |      if crypt(...) |
                                               |                    |
                                           ------   -----------     |
                           exec(): SHELL: | echo_noraw()       |    |
                                          |                    |----
                                          |                    |
                                          |                    |
                                          |                    |
                                          |                    |
                                          |                    |
                                          |                    |
                                          |                    |
                                          |                    |
                                          |                    |
                                          | CTRL-D (exit)      |
                                           --------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>		/* for getpwnam() */
#include <signal.h>		/* for signal(), kill() */
#include <sys/stat.h>		/* for stat() */
#include <fcntl.h>		/* for low-level I/O */

				/* LOTS OF CONDITIONS HERE */
#if defined (__alpha)

#define	UINET_32	unsigned int	/* see <machine/machlimits.h> */
#define INT_32		int
#define L_ENDIAN
#include <sys/types.h>
#include <sys/time.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyaddr() */
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */
				/* If your system is POSIX compliant
		you can use tcgetattr() and tcsetattr() instead of
		ioctl(). DecUnix 3.2 and above support this. */
#define READ_TERMIOS TCGETS
#define WRITE_TERMIOS TCSETS
#define BSD_VHANGUP		/* none! OSF is a System V variant */
#define RE_ESTABLISH_CONTROL_TTY(x,y) ioctl((x), TIOCSCTTY, 0)
#define TTY_MESSAGE(x,y) sprintf((x), "\r\ntty=%s \r\n", y );
#define TSET_MESSAGE "if .profile fails use: [reset] [stty -tabs] \r\n"
#define	BEGIN_FORK_OR_THREAD	"_fork: ... "

				/* Some CONDITIONAL Includes */
#elif defined(__vax) || defined(__mips)	/* ULTRIX OS */

#define	UINET_32	unsigned long
#define INT_32		long
#define L_ENDIAN
#include <auth.h>			/* for C2-Security */
AUTHORIZATION *authptr;
#include <sys/types.h>
#include <sys/time.h>
#include <sys/termios.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyaddr() */
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */
#define READ_TERMIOS TCGETP
#define WRITE_TERMIOS TCSANOW
#define BSD_VHANGUP vhangup();	/* Ultrix is a BSD variant */
#define RE_ESTABLISH_CONTROL_TTY(x,y) ioctl((x), TIOCSPGRP, &(y))
#define TTY_MESSAGE(x,y) sprintf((x), "\r\ntty=%s \r\n", y );
#define TSET_MESSAGE "if .profile fails use: [tset -n -I] [stty -tabs] \r\n"
#define	BEGIN_FORK_OR_THREAD	"_fork: ... "

#elif defined(MSDOS)		/* OS/2 */

#define	UINET_32	unsigned long
#define INT_32		long
#define L_ENDIAN
#include <pctcp.h>		/* includes <sys/types.h> */
#include <bsd.h>
#include <process.h>		/* needed for threads, spawn() */
#include <stddef.h>		/* for threadid */
#include <stdlib.h>		/* for _fileinfo */
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyaddr() */
#define	STACK_SIZE 8192		/* for _beginthread() */
#define TERM_STRUCT sgttyb	/* this is old-style BSD 4.3 */
#define READ_TERMIOS TIOCGETP
#define WRITE_TERMIOS TIOCSETP
#define BSD_VHANGUP		/* none! */
#define RE_ESTABLISH_CONTROL_TTY(x,y) -1	/* none! */
#define TTY_MESSAGE(x,y) sprintf((x), "\r\ntty=PC \r\n");
#define TSET_MESSAGE "if .profile fails use: [tset -n -I] [stty -tabs] \r\n"
#define	BEGIN_FORK_OR_THREAD	"_beginthread: ... "

#else				/* SUN OS */

#define	UINET_32	unsigned long
#define INT_32		long
#define B_ENDIAN
#include <sys/types.h>
#include <sys/time.h>
#include <sys/termios.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyaddr() */
#include <sys/filio.h>		/* for FIONBIO */
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */
#define READ_TERMIOS TCGETS
#define WRITE_TERMIOS TCSETS
#define BSD_VHANGUP vhangup();
#define RE_ESTABLISH_CONTROL_TTY(x,y) ioctl((x), TIOCSPGRP, &(y))
#define TTY_MESSAGE(x,y) sprintf((x), "\r\ntty=%s \r\n", y );
#define TSET_MESSAGE "if .profile fails use: [tset -n -I] [stty -tabs] \r\n"
#define	BEGIN_FORK_OR_THREAD	"_fork: ... "

#endif				/* END of CONDITIONALS */

#define	TEL_BREAK	0x14	/* CTRL-T s_tlnt break to command mode */
#define	GET_REINIT	0x1E	/* CTRL-^ kill shell and re-spawn s_getty */
#define	GET_DOKILL	0x0B	/* CTRL-K kill s_getty */
#define MAX_SESS 5		/* maximum number of sessions */
#define MAX_TRIES 3		/* maximum number of tries to give
				   a valid username on "login: "
				   or a valid password on "socket_code: " */
#define SELECT_TIMEOUT 300	/* timeout value in seconds on select()
				   5 minutes seems fair. */
#define ULIMIT 256		/* PROBLEM: ulimit() option 4 is
				   non-existent on Ultrix and the old
				   getdtablesize() seems to be in the
				   process of being phased out on the SUN */
#define LOCAL_IP2 201
#define LOCAL_IP3 168
#define LOCAL_IP4 136
#define	SHELL_PATH "/bin/sh"	/* default if user has no SHELL */
#define	PACKET_SIZE 1024

	/* GLOBALS: */
int	verbose_flag;	/* if SET, verbose diagnostics are given */
int	ctrlc_hit=0;
int	termsave_done=0;
struct	TERM_STRUCT tsave;	/* raw_noecho(),... etc. save the
				    original settings here */

	/* GLOBALS: for main(), get_schar() */
char	gbuf[PACKET_SIZE];	/* packet buffer, used by get_schar() */
int	bufindex;	/* next character to read */
int	bufnum;		/* total characters in buffer */
char	obuf[PACKET_SIZE + 1];	/* packet buffer, used by main() before
			   fork() */
int	oindex;		/* index for obuf[] */

	/* GLOBALS: for main(), check_host(), check_user() */
int	listener[128];	/* record our hostname (as set on boot) */
int	portnumber;	/* record socket portnumber */
char	tty_name[32];	/* terminal name (either "-" or e.g. ttyxx) */
char	tty_type[32];	/* terminal type (in /etc/gettytab) */
struct	sockaddr_in caller;	/* remote(calling) host-socket */
struct	hostent hcaller;	/* remote(calling) host (hcaller.h_name[]).
		warning: this structure also contains type 
			 **char which is valid ONLY until the
			 next call to gethostbyaddr(). */
struct	passwd	spuser;		/* remote(calling) user
		warning: this structure contains pointer types
			 which are valid ONLY until the next call
			 to getpwnam(), hence the COPIES below. */
char	sp_name[64];	/* copy from struct passwd */
int	sp_uid;		/* copy from struct passwd */
char	sp_passwd[64];	/* copy from struct passwd */
char	sp_dir[128];	/* copy from struct passwd */
char	sp_shell[128];	/* if found, copy from struct passwd, otherwise
			   use SHELL_PATH as a default */
char	sp_basesh[64];	/* basename part of sp_shell */

	/* GLOBALS: for open_psterm() */
int	ps_master;	/* master side of pseudoterm: pty[p-r][0-f] */
int	ps_slave;	/* slave side of pseudoterm: tty[p-r][0-f] */
char	ps_mastername[16];
char	ps_slavename[16];
struct	stat	fsbuf;	/* for stat() call in open_psterm() */
struct	TERM_STRUCT ttysave;	/* used by: open_psterm() */

	/* GLOBALS: for open_os2psterm() */
struct	sockaddr_in	c_name;	/* master-side of socket-pair */
char	c_domainname[128];
struct	sockaddr_in	l_name; /* slave-side of socket_pair */
struct	sockaddr_in	local_caller;	/* for accept() */
int	connect_sock;
int	listen_sock;

	/* GLOBALS: for pre_login() */
int	msgsock;			/* TRANSFER socket */
int	sh_pid;	/* pid of shell: under UNIX, sh_pid = pid = fork()
				 under OS/2, sh_pid = opid = spawn() */

				/* Function Prototypes */
#if defined(_NOPROTO)
void	pre_login();		/* forward reference for _beginthread() */
int	offspring_stop();
	parent_intr();
	child_intr();
int	output_login_prompt();
int	get_schar();
int	check_host();
int	check_user();
int	open_psterm();
int	open_os2psterm();
	disp_ctrlc();
int	show_settings();
int	raw_noecho();
int	raw_cbreak();
int	fionbio_select();
int	echo_noraw();

#else
void	pre_login(void);	/* forward reference for _beginthread() */
int	offspring_stop(int spid);
void	parent_intr(int dummy);
void	child_intr(int dummy);
int	output_login_prompt(int msock);
int	get_schar(int msock);
int	check_host(void);
int	check_user(char uname[]);
int	open_psterm(void);
int	open_os2psterm(void);
void	disp_ctrlc(int dummy);
int	show_settings(FILE *fp, struct TERM_STRUCT *ptsave);
int	raw_noecho(void); 
int	raw_cbreak(void);
int	fionbio_select(void);
int	echo_noraw(void);

#endif

/* =================================================================== */

#if defined(_NOPROTO)
main(argc,argv,envp)
int	argc;	
char	*argv[],*envp[];
#else
main(int argc,char *argv[],char *envp[])
#endif
	{
	int	sock;				/* LISTENING socket */
	struct	sockaddr_in	name;		/* RECEIVING host */
	u_short	port_num;			/* RECEIVING listen port */
	int	fionbiof;	/* for ioctl() */
	char	e16pass[64];	/* for auth-style passwds */
	char	str[128];	/* for sprintf() and write() */
	char	answer[128];
	int	pid, opid, ostatus;		/* local process var's */
	int	size, length, ret, tmout, byteswritten, numtries;
	int	i, j, k, khar, d1, d2, d3, d4, first_fork;
	UINET_32 ul;
			/* next data structures are for select() */
	char	ibuf[256];	/* input buffer: socket -> ptty */
	char	pbuf[256];	/* output buffer: ptty -> socket */
	fd_set	readmask, writemask, exceptmask;
	struct	timeval	timeout;/* struct timeval is defined as:
		INT_32	tv_sec;		   seconds and
		INT_32	tv_usec;	   micro-seconds */

/* ********** CHECK COMMAND LINE for verbose flag AND tty type */
	verbose_flag = 0;			/* default = silent */
	tty_name[0] = '\0';
	strcpy(tty_type, "socket");		/* default = socket */
	port_num = 0;				/* default = system assigned */
	if (argc > 1)
	  for (i = 1 ; i < argc ; i++)
		{
		if (strcmp(argv[i], "-") == 0)
			strcpy(tty_name, argv[i]);
		else if (strncmp(argv[i], "tty", 3) == 0)
			strcpy(tty_name, argv[i]);
		else if (strncmp(argv[i], "-v", 2) == 0)
			verbose_flag = 1;
		else if ( (*argv[i] >= '0')  &&  (*argv[i] <= '9') )
			port_num = atoi(argv[i]);
		else
			strcpy(tty_type, argv[i]);
		}
	if (verbose_flag)
		{
		if (port_num != 0)
			printf(
  "\ns_getty: tty_name=%s, tty_type=%s, and port_num=%d ",
			tty_name, tty_type, port_num);
		else
			printf(
  "\ns_getty: tty_name=%s, tty_type=%s, and port_num=assigned_by_system",
			tty_name, tty_type);
		}

/* ********** SET-UP SIGNAL CONTROL ON SIGQUIT *** */
	signal(SIGQUIT, SIG_IGN);   /* DON'T allow writable CORE dumps */

/* ********** RECORD OUR OWN HOSTNAME (for prompt) */
	gethostname(listener, sizeof(listener) - 1);

/* ********** CREATE SOCKET (i.e. create TSAP of type INTERNET STREAM
			and of protocol 0 = IP protocol) */
	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock < 0)
		{
		perror("s_getty: socket() failed. ");
		exit(-1);
		}

/* ********** BIND NAME TO SOCKET (use wildcards and get Portnumber) */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY=assigned by system */
	name.sin_port = htons(port_num);	    /* 0=assigned by system */
	ret = bind(sock,(struct sockaddr *)&name,sizeof name);
	if (ret < 0)
		{
		perror("s_getty: bind() failed. ");
		exit(-1);
		}

/* ********** USE UTILITIES getsockname, ntohs TO PRINT OUT NAME */
	length = sizeof name;
	ret = getsockname(sock,(struct sockaddr *)&name,&length);
	if (ret < 0)
		{
		perror("s_getty: getsockname() failed. ");
		close(sock);
		exit(-1);
		}
	name.sin_family = AF_INET;
	if (verbose_flag)
	  {
	  printf("\ns_getty: own process id: %d ",getpid());
	  printf("\ns_getty: own net name.sin_addr: %ld ",
		name.sin_addr.s_addr);
	  }
	  ul = ntohl(name.sin_addr.s_addr);	/* VAX: reverse byte order */
	  d1 = 0xff & ul;	d2 = (0xff00 & ul) >> 8;
	  d3 = (0xff0000 & ul) >> 16;	d4 = (0xff000000 & ul) >> 24;
	if (verbose_flag)
	  {
	  printf("\ns_getty: own IP address: %d.%d.%d.%d ",d4,d3,d2,d1);
		/* ntohs() converts Network byte order to Host byte order */
	  printf("\ns_getty: own host name.sin_port: %d \n",
				ntohs(name.sin_port));
	  }

/* ********** DISPLAY PORTNUMBER, LISTEN and start ACCEPTING CONNECTIONS */
	fflush(stdout);		/* clear for "nohup" */
	portnumber = ntohs(name.sin_port);	/* record portnumber */
	first_fork = 1;		/* DON'T close 0 and 1 more than ONCE */

start_listen:
	sprintf(str, "s_getty: listening at own host portnumber: %d \n",
		ntohs(name.sin_port));
						/* This may be the
		SECOND time around (so that descriptors 0 and 1 have
		been closed) so we MUST use descriptor 2 */
	byteswritten = write(2, str, strlen(str));

/* ********** BLOCK IN listen() UNTIL CONNECT ATTEMPT */
	listen(sock, MAX_SESS);	/* MAX_SESS = max. backlog allowed */
		k = sizeof caller;
		msgsock = accept(sock,(struct sockaddr *)&caller,(int *)&k);
			/* the function accept() blocks until there is at 
			least one connection in the queue of pending 
			requests. it then extracts the first connection 
			from the queue of pending requests and creates a new
			socket (msgsock) with the same properties as sock.
			the int msgsock is used as a desc. to read and write
			data. if null parameters were used here (i.e.
			(struct sockaddr *)0 and (int *)0) the address
			of the connecting entity would be ignored.  */
		if (msgsock == -1)
		  {
		  sprintf(str, "s_getty: accept() failed. \r\n");
		  byteswritten = write(2, str, strlen(str));
		  goto start_listen;	/* msgsock never opened */
		  }  /* end of if */
		else
		  {

/* ********** CHECK REMOTE calling HOST: host will be regarded as
		ACCEPTABLE if its IP address is LOCAL, i.e. of the
		form: 136.168.*.* and its name was successfully
		looked up by gethostbyaddr(). */

		  ret = check_host();		/* check if remote host
			is ACCEPTABLE to s_getty */
		  if (ret != 0)
			{
			sprintf(str,
			  "s_getty: calling host is NOT acceptable \r\n");
			byteswritten = write(2, str, strlen(str));
			goto failedlogin;
			}

		  numtries = 0;			/* allow MAX_TRIES */
		  bzero(gbuf, sizeof gbuf);	/* set up global buf[] */
		  bufnum = 0;
		  bufindex = 0;
		  output_login_prompt(msgsock);	/* give login prompt */
		  bzero(obuf, sizeof obuf);	/* clear obuf[] */
		  oindex = 0;
				/* get reply to "login: " prompt */
		  while ((k = get_schar(msgsock)) != EOF)
			{
			if (  (k == 010)  ||  (k == 0177)  )
				{
				if (oindex > 0) 
					oindex--;	/* BS */
				str[0] = 010;
				str[1] = ' ';
				str[2] = 010;	/* BS SPACE BS */
				byteswritten = write(msgsock, str, 3);
				}
			else if ( (k == '\r')  ||  (k == '\n') )
				{
				str[0] = '\r';	/* CR (note that s_tlnt
					operates with ONLCR off) */
				str[1] = '\n';	/* LF */
				byteswritten = write(msgsock, str, 2);
					  /* convert LF to EOS */
				obuf[oindex] = '\0';	/* EOS */

/* ********** CHECK REMOTE calling USER: user will be regarded as
		ACCEPTABLE if the username can be looked up in
		/etc/passwd and had a valid UID >= 1024 */

				numtries++;
				ret = check_user(obuf);	/* check if
					remote user is ACCEPTABLE to
					s_getty */
				if (ret < 1024)
					{
					sprintf(str,
			"s_getty: %s is NOT an acceptable user \r\n", obuf);
					byteswritten = write(2,
						str, strlen(str));
					byteswritten = write(msgsock,
					"login: ", 7);
					}
				else
					{
					if (verbose_flag)
					  {
					  sprintf(str,
				"s_getty: %s:%s:%d:%d:%s:%s:%s \r\n",
			spuser.pw_name, spuser.pw_passwd,
			spuser.pw_uid, spuser.pw_gid, spuser.pw_gecos,
			spuser.pw_dir, spuser.pw_shell);
					  byteswritten = write(2,
						str, strlen(str));
					  }
					else	/* short note */
					  {
					  sprintf(str,
				"s_getty: user=%s from host=%s \r\n",
			spuser.pw_name, hcaller.h_name);
					  byteswritten = write(2,
						str, strlen(str));
					  }
						/* make copies */
					strcpy(sp_name, spuser.pw_name);
					strcpy(sp_passwd, spuser.pw_passwd);
					sp_uid = spuser.pw_uid;
					strcpy(sp_dir, spuser.pw_dir);
					if (spuser.pw_shell != NULL)
					  strcpy(sp_shell, spuser.pw_shell);
					else
					  strcpy(sp_shell, SHELL_PATH);
					j = 0;	/* scan for basename */
					for (i = 0 ; sp_shell[i] != '\0'; i++)
					  {
					  if (sp_shell[i] != '/')
					    {
					    sp_basesh[j] = sp_shell[i];
					    j++;
					    }
					  else
					    {
					    j = 0;	/* start again */
					    }
					  }
					sp_basesh[j] = '\0';
					goto openpsterm;
					}
				if (numtries == MAX_TRIES)
					goto failedlogin;
				bzero(obuf, sizeof obuf);
				oindex = 0;
				}
			else		/* still NOT at EOS */
				{
				obuf[oindex] = k;	/* ascii char */
				str[0] = k;
				byteswritten = write(msgsock, str, 1);
				oindex++;
				}
			}  /* end of while */
		  goto failedlogin;		/* EOF in input stream */
		  }  /* end of else if (msgsock == -1) */

/* ********** OPEN PSEUDO TERMINAL ******************** */
openpsterm:
				/* At this point remote HOST and
			remote USER have been accepted subject to
			socket_code required later by PRE-LOGIN. We now
			have some conditional code because OS/2, strictly
			speaking, does not have pseudo-terminals. */

#if defined(MSDOS)
	ret = open_os2psterm();
#else
	ret = open_psterm();
#endif
	if (ret == 0)
		{
		if (verbose_flag)
		  {
		  sprintf(str,
			"s_getty: %s opened as master with desc=%d \r\n",
			ps_mastername, ps_master);
		  byteswritten = write(2, str, strlen(str));
		  sprintf(str,
			"s_getty: %s opened as slave with desc=%d \r\n",
			ps_slavename, ps_slave);
		  byteswritten = write(2, str, strlen(str));
		  }
	sprintf(str, BEGIN_FORK_OR_THREAD);
	byteswritten = write(2, str, strlen(str));

/* ********** s_getty NOW FORKs (in the case of UNIX) but 
			  BEGINsTHREAD (in the case of OS/2) ** */

#if defined(MSDOS)
				/* start pre_login thread, if
			_beginthread is successful, pid will
			be the thread identification number,
			otherwise -1 will be returned. */
		pid = _beginthread(pre_login,	/* far procedure */
			     NULL,	/* assign stack bottom */
			     STACK_SIZE,/* stack size */
			     NULL);	/* argument list */
		sprintf(str, "(thread_id=%d) ... ", pid);
		byteswritten = write(2, str, strlen(str));
#else
		pid = fork();	/* *** UNIX fork *** */
#endif
		if (pid < 0)	/* fatal error (maybe process table full) */
			{
			sprintf(str, "failed \r\n");
			byteswritten = write(2, str, strlen(str));
			close(ps_slave);
			close(ps_master);
			goto failedlogin;
			}

/* ********** OFFSPRING: pre-login, set STDIN, STOUT to pseudoterm */
		if (pid == 0)       /* here (pid = fork()) == 0 */
			{
				/* CALL pre_login procedure */
			pre_login();
			
				/* a FAILURE if we got to here */
			write(2, "exec: failed \r\n", 14);
			echo_noraw();	/* insurance */
			exit(-1);
			}  /* end of offspring (pre-login ...) */

/* ********** PARENT: s_getty *************************** */
		else if (pid > 0)
			{
			sprintf(str, "successful \r\n");
			byteswritten = write(2, str, strlen(str));
#if defined(MSDOS)
				/* Shell PID (sh_pid) will be
			  set by pre_login thread at time of spawn() */
				/* DON'T close ps_slave when
			  starting pre_login as a THREAD, since
			  it NEEDS to be able to dup2(ps_slave, ) */
#else
				/* Shell PID (sh_pid) under UNIX
			  is same as offspring process. s_getty
			  can close slave side since it will ONLY
			  have to act as a GO-BETWEEN for ps_master
			  and msgsock */
			sh_pid = pid;
			close(ps_slave);
#endif
			if (first_fork)
				{
#if defined(MSDOS)
				/* DON'T close 0 and 1 when
			starting pre_login() as a THREAD, since
			it NEEDS to be able to dup2( ... ) */
#else
				close(0);
				close(1);
#endif
				first_fork = 0;
				}
/* leave STDERR open		close(2); so that we can communicate
		to the user (or to nohup.out)	*/

			fionbiof = 1;	/* next action is necessary
	on most UNIX's but it is strangely OPTIONAL on SUNOS */
/* ********** PARENT: set master side of pseudo-terminal Non-Blocking */
			ioctl(ps_master, FIONBIO, &fionbiof);
					/* next action OPTIONAL
	but RECOMMENDED before select() is called */
			ioctl(msgsock, FIONBIO, &fionbiof);

/* DURING DEVELOPMENT, trap any CTRL-C which might be mistakenly
			  sent ONLY to parent
			signal(SIGINT, parent_intr); */
/* ********** CTRL-K sent from remote host 
			  is now the PREFERRED way to kill s_getty */
			signal(SIGINT, SIG_IGN);

/* ********** PARENT: disable some SIG's so ptty is less vulnerable, this
		is OPTIONAL but RECOMMENDED */
			signal(SIGTSTP, SIG_IGN);
			signal(SIGTTIN, SIG_IGN);
			signal(SIGTTOU, SIG_IGN);

			bzero(pbuf, sizeof pbuf);
			bzero(ibuf, sizeof ibuf);
		/* START: select() loop */

selectloop:
		/* SETUP: select() loop: we will watch:
			1. reads on ps_master (exceptions would 
			   be RARE here)
			2. reads and exceptions on msgsock */
				FD_ZERO(&readmask);
				FD_ZERO(&writemask);
				FD_ZERO(&exceptmask);
				FD_SET(ps_master, &readmask);
				FD_SET(msgsock, &exceptmask);
				FD_SET(msgsock, &readmask);
				/* set timeout values */
			timeout.tv_sec = SELECT_TIMEOUT;
			timeout.tv_usec = 0;
/* ********** BLOCK ON: select() NOTE that under OS/2 select() will
			work properly for: sockets, files and the console
			but NOT for pipes. ******* */

			tmout = select(ULIMIT, &readmask,
				&writemask, &exceptmask, &timeout);

		/* CHECK: TIMEOUT (select() normally returns the number
			  of file descriptors selected. if zero
			  is returned, a timeout occurred. if
			  -1 is returned, an error occurred). */
			if (tmout == 0)
			  {
			  write(2, "s_getty: timeout \r\n", 19);
			  offspring_stop(sh_pid);
			/* RETURN to listen() to allow a re-login */
			  goto start_listen;
			  }
		/* CHECK: EXCEPTION on select() */
			if (tmout < 0)
			  {
			  write(2, "s_getty: select error: \r\n", 25);
			  offspring_stop(sh_pid);
			  close(sock);
			  write(2, "s_getty: exiting \r\n", 19);
			  exit(0);
			  }
		/* CHECK: EXCEPTION on socket */
			if (FD_ISSET(msgsock, &exceptmask))
			  {
			  write(2, "s_getty: exception on socket: \r\n",32);
			  offspring_stop(sh_pid);
			  close(sock);
			  write(2, "s_getty: exiting \r\n", 19);
			  exit(0);
			  }
		/* CHECK: ptty(read) --> socket(write) */
			if (FD_ISSET(ps_master, &readmask))
			  {
			  ret = read(ps_master, pbuf, 80);
			  if (ret <= 0)
				{
				/* This typically happens
		when you finally terminate the /usr/bin/ksh that pre-login
		has exec'd with CTRL-D 
				write(2, "s_getty: null ptty read \r\n", 26);*/
				write(2, sp_shell, strlen(sp_shell));
				write(2, " terminated \r\n", 14);
			  	offspring_stop(sh_pid);
			/* RETURN to listen() to allow a re-login */
			  	goto start_listen;
				}
			  else
				{
				byteswritten = write(msgsock, pbuf, ret);
		/* a PRODUCTION PROGRAM should check here to see 
		   if (byteswritten < ret) due to high load and take
		   proper action (whatever that might be) */
				}
			  bzero(pbuf, sizeof pbuf);
			  }
		/* CHECK: socket(read) --> ptty(write) */
			if (FD_ISSET(msgsock, &readmask))
			  {
			  ret = read(msgsock, ibuf, 80);
			  if (ret <= 0)
				{
				/* This typically happens
		when you PREMATURELY terminate the calling TELNET
				write(2,"s_getty: null socket read \r\n",28);*/
				write(2, "telnet killed \r\n", 16);
				offspring_stop(sh_pid);
			/* RETURN to listen() to allow a re-login */
			  	goto start_listen;
				}
			  else		/* READ was valid */
				{
			/* check for the CONTROL chars:	
						1. GET_REINIT
						2. GET_DOKILL */
				for (j = 0 ; j < ret ; j++)
				  {
					/* check for REINIT getty */
				  if (ibuf[j] == GET_REINIT)
				    {
				    write(2, "s_getty: re-init \r\n", 19);
				    offspring_stop(sh_pid);
			/* RETURN to listen() to allow a re-login */
				    goto start_listen;
				    }
					/* check for DOKILL getty,
		this is the PREFERRED METHOD for stopping s_getty */
				  else if (ibuf[j] == GET_DOKILL)
				    {
				    write(2, "s_getty: do-kill \r\n", 19);
				    offspring_stop(sh_pid);
				    close(sock);
				    write(2, "s_getty: exiting \r\n", 19);
/* ********** THIS IS THE FINAL EXIT POINT *** */
				    return(0);
				    }
				  }	/* no SPECIAL CONTROL
				    characters were in buffer */
				byteswritten = write(ps_master, ibuf, ret);
		/* a PRODUCTION PROGRAM should check here to see 
		   if (byteswritten < ret) due to high load and take
		   proper action (whatever that might be) */
				}
			  bzero(ibuf, sizeof ibuf);
			  }
			goto selectloop;  /* END: select() loop */
			}  /* end of parent (s_getty) */

		}
	else	   /* if (ret != 0) on open_psterm() */
		{
		byteswritten = write(msgsock, "open_psterm: failed \r\n", 22);
		}  /* open_psterm() failed */

/* ********** WE JUMP HERE PREMATURELY (before the fork() in s_getty) IF:
		1. THE CALLER'S HOST WAS NOT ACCEPTABLE 
		2. THE CALLER'S USERNAME WAS NOT ACCEPTABLE 
		3. THE CALLER FAILED MAX_TRIES ATTEMPTS TO LOGIN 
		4. THERE WAS AN EOF IN THE INPUT STREAM
		5. open_psterm() FAILED
	      OR IF
		6. THE fork() ITSELF FAILED. */

failedlogin:	;

/* ********** CLOSE msgsock AND RETURN TO LISTEN */
	close(msgsock);		/* close socket descriptor */
	sprintf(str, "s_getty: ending login without fork() \r\n");
	byteswritten = write(2, str, strlen(str));
	goto start_listen;

	}  /* end of main */

/* ---------------------------------------------------------------------
offspring procedures(UNIX)/threads(OS/2):
			void	pre_login(void)
   --------------------------------------------------------------------- */

#if defined(_NOPROTO)
void	pre_login()	
#else
void	pre_login(void)
#endif
			/* This procedure/thread is called by s_getty's 
		OFFSPRING when run under UNIX. When run under OS/2
		it executes CONCURRENTLY as a THREAD */
	{
	int	i, j, k, ret, bytesread, byteswritten;
	int	ostatus, opid, opgrp, khar, numtries;
	char	answer[128], str[128], tmpstr[32];
	char	e16pass[64];		/* for auth-style passwds */

			opid = getpid();/* get OWN pid (but don't use) */
#if defined(MSDOS)
				/* DON'T close sockets when
			running pre_login() as a THREAD */
#else
			close(msgsock);
			close(ps_master);
#endif
			/* replace stdin by ps_slave */
			if (dup2(ps_slave, 0) < 0)
				{
				perror("pre_login: dup2() ");
				exit(-1);
				}
			/* replace stdout by ps_slave */
			if (dup2(ps_slave, 1) < 0)
				{
				perror("pre_login: dup2() ");
				exit(-1);
				}
/* leave STDERR open A WHILE LONGER	dup2(ps_slave, 2);      */

/* ********** OFFSPRING: set COOKED mode and save pseudoterm settings */
			echo_noraw();
			ioctl(0, READ_TERMIOS, &ttysave);

/* DURING DEVELOPMENT trap any CTRL-C which parent (s_getty) might SEND
			signal(SIGINT, child_intr); */

			/* offspring will NOW be killed
			   automatically by parent s_getty
			   when remote telnet sends CTRL-K */
			signal(SIGINT, SIG_IGN);
			signal(SIGQUIT, SIG_IGN);/* insurance */
			sleep(1);	/* let parent BLOCK
				in select() first */

/* DIAGNOSTIC:	  printf( "pre_login: %s settings: \n", ps_slavename);
			show_settings(&ttysave);	*/

/* ********** OFFSPRING: start socket_code entry: */

			numtries = 0;	/* allow MAX_TRIES */
socketcode_loop:
			printf("\rsocket_code: ");
					/* either use fflush() after
				STREAM calls or use write() */
			fflush(stdout);
/* ********** OFFSPRING: temporarily set RAW mode for socket_code entry.
		   	 since this program by default DOES NOT use
			 any priviledged socket, if "USE_C2" and 
			 "USE_BSD43" are NOT defined during compilation, 
			 the user CAN SIMPLY HIT <RETURN> at the prompt. */
			raw_noecho();

			for (j = 0 ; sizeof(answer) ; j++)
			  {
			  read(0, &answer[j], 1);
			  if ( (answer[j] == 010)  ||
				(answer[j] == 0177) )
				{
				if (j > 0) j--;	/* BS or DEL */
				}
			  else if ( (answer[j] == '\r')  ||
			  	     (answer[j] == '\n') )
				{
				answer[j] = '\0';
				str[0] = '\r';
				str[1] = '\n';
				write(1, str, 2);
				break;
				}
			  }
/* ********** OFFSPRING: restore COOKED mode */
			echo_noraw();

/* ********** OFFSPRING: check socket_code, NOTE that Ultrix4.x 
			 supports the C2-Security Package so that
			 IF YOU WANT TO DEFINE "USE_C2" then
			 the effective userid (EUID) of this program
			 must be a member of group "authread" or have 
			 equivalent privileges. */

#if defined(USE_C2)			/* if C2 Security (note
				the "crypt16" below) */
			authptr = getauthuid(sp_uid);
			if (authptr != NULL)
			  {
			  strcpy(e16pass, (*authptr).a_password);
			  if (strcmp(e16pass,
				crypt16(answer, e16pass)) != 0)
				{
				numtries++;
				if (numtries == MAX_TRIES)
				  {
				  		/* FAILURE -
					KILL offspring pre-login */
				  sprintf(str,
			"pre-login: bad socket_code, exiting ... \r\n");
				  write(2, str, strlen(str));
				  sleep(1);
				  exit(-1);
				  }
						/* TRY AGAIN */
				sprintf(str,
			"pre-login: failure %d on socket code \r\n",
						numtries);
				write(2, str, strlen(str));
				goto socketcode_loop;
				}
			  }
			else
			  {
						/* FAILURE -
					egid not authread or
					insufficient euid */
			  sprintf(str,
			"pre-login: auth failed, exiting ... \r\n");
			  write(2, str, strlen(str));
			  sleep(1);
			  exit(-1);
			  }
#elif defined(USE_BSD43)		/* else if BSD4.3 */
				/* this is STRAIGHT BSD4.3 password
				   type checking and works for
				   OS/2 except that passwd file
				   is usually in \TCPOS2\ETC */
			if (strcmp(sp_passwd, 
			    crypt(answer, sp_passwd)) != 0)
				{
				numtries++;
				if (numtries == MAX_TRIES)
				  {
				  		/* FAILURE -
					KILL offspring pre-login */
				  sprintf(str,
			"pre-login: bad socket_code, exiting ... \r\n");
				  write(2, str, strlen(str));
				  sleep(1);
				  exit(-1);
				  }
						/* TRY AGAIN */
				sprintf(str,
			"pre-login: failure %d on socket code \r\n",
						numtries);
				write(2, str, strlen(str));
				goto socketcode_loop;
				}
#else				/* Private use of a non-priviledged
				SOCKET by ordinary user (who can only 
				hurt himself) */
			sleep(1);
#endif

/* ********** OFFSPRING: socket_code WAS OK ********************* */
#if defined(MSDOS)
			sleep(1);
				/* DON'T close STDERR when running
				as THREAD */
#else
				/* inform user that we are ready to
				exec a SHELL */
			write(2, "pre-login: exec ", 16);
			write(2, sp_shell, strlen(sp_shell));
			write(2, "\r\n", 2);
			sleep(1);
				/* close STDERR NOW since the Bourne
				shell uses this for the prompt and
				we would otherwise write to the
				WRONG terminal */
			dup2(ps_slave, 2);
#endif
				/* detach ourselves from parent 
			process group and form our OWN PROCESS GROUP. 
			This is IMPORTANT since we DO NOT want to be
			listed as having the SAME terminal as our parent */
				/* WARNING:
			setpgrp(self, 0) is the SAME as setsid() on the 
			SUNOS but NOT on ULTRIX, POSIX-complant systems, 
			or on OSF/1, hence we use the newer call */
#if defined(MSDOS)
						/* NO setsid() */
#else
			if (verbose_flag)
			  {
					/* before: DISPLAY for user */
			  sprintf(str, 
			  "\r\nbefore setsid: PPID=%d PID=%d PGRP=%d ",
				getppid(), getpid(), getpgrp());
			  write(1, str, strlen(str));
			  }

			opgrp = setsid();
			if (opgrp < 0)
				perror("setsid ");
#endif

/* ********** OFFSPRING: final pseudo-terminal SETUP: */
					/* We essentially now need to
			(RE)ESTABLISH a controlling terminal.  Here 
			BSD-type systems like ULTRIX and SUNOS 4.1.3 
			(do: man 4 tty) DIFFER from OSF/1 
			(do: man 7 tty) with regard to vhangup()
			and the parameters in the ioctl() call:

			BSD-type systems in addition USUALLY:
				1. need to ensure that this offspring
				   has a CLEAN psuedo terminal to use.
				2. unless this CLOSE-REOPEN is done there
				   will be NO CONTROLLING TERMINAL listed
				   in the kernel process tables (there is
				   always a controlling terminal, just
				   use "tty" but programs like "more"
				   will DEADLOCK if it is not listed in
				   the process tables). */
#if defined(MSDOS)
						/* NO vhangup() */
#else
			signal(SIGHUP, SIG_IGN);
			BSD_VHANGUP		/* system dependent */
			signal(SIGHUP, SIG_DFL);

						/* now RE_OPEN tty */
			ps_slave = open(ps_slavename, O_RDWR);
			if (ps_slave < 0)
				perror("open ");

				/* now SWITCH the (pseudo)
				terminal to OUR PROCESS GROUP */
			ret = RE_ESTABLISH_CONTROL_TTY(ps_slave, opgrp) ;
			if (ret != 0)
				perror("ioctl ");

			if (verbose_flag)
			  {
					/* after: DISPLAY for user */
			  sprintf(str, 
			  "\r\nafter setsid:  PPID=%d PID=%d PGRP=%d ",
				getppid(), getpid(), getpgrp());
			  write(1, str, strlen(str));
			  }
#endif
					/* DISPLAY for user */
			TTY_MESSAGE(str, ttyname(0));
			write(1, str, strlen(str));
					/* You may have to give a
			"tset -n -I" or "reset" to set erase(BS) to DEL 
			and "stty -tabs" to set tabs correctly */
			sprintf(str, TSET_MESSAGE);
			write(1, str, strlen(str));

/* ********** OFFSPRING: chdir to $HOME and exec SHELL */

#if defined(MSDOS)
			ret = chdir(sp_dir);
			if (ret < 0)
				perror("pre_login: chdir() ");
			else
				{
				sprintf(str,
				"pre_login: chdir() to %s \r\n", sp_dir);
				write(2, str, strlen(str));
				}
#else
			chdir(sp_dir);
#endif

#if defined(MSDOS)
/*			_fileinfo = 1;	   This DOESN'T really help
				since SOCKETS CANNOT be passed to
				offspring like open FILES can! This
				seems to be a limitation of OS/2 and
				we must use _fdexternalize/_fdinternalize
				to get around this, i.e. our shell MUST
				expect an invocation like:

			s_sh.exe -S itoa(_fdexternalize(ps_slave),tmpstr,10)

					 */
					/* IMPORTANT: close stdin, stdout
				BEFORE _fdexternalize since we have used
				dup2() with 0 and 1. */
			close(0);
			close(1);
			itoa(_fdexternalize(ps_slave),tmpstr,10);
			sprintf(str,
		"spawnlp(P_NOWAIT,%s,\"s_sh.exe\",\"-S\",%s,NULL); \r\n",
				sp_shell, tmpstr);
			write(2, str, strlen(str));

					/* SPAWN with spawnlp() since we
				are a THREAD under OS/2  */
			ret = spawnlp(P_NOWAIT,
					sp_shell,	/* s_sh.exe */
					"s_sh.exe",	/* basename */
					"-S",		/* flag */
					tmpstr,		/* _fdext... */
					NULL);		/* end arg's */
			if (ret < 0)
				perror("pre_login: spawnlp() ");
			else
				{
				sh_pid = ret;	/* record shell pid */
				wait(&ostatus);	/* wait for shell to exit */
				}

/* ********** RE-OPEN stdio, stdout SO THAT DESCRIPTORS 0 AND 1 ARE NOT
	      AVAILABLE WHEN open_os2psterm() IS CALLED (otherwise socket-
	      pair will be CLOBBERED later by dup2(ps_slave, )! *** */
			close(ps_slave);
			close(listen_sock);
					/* main() will close the
			descriptors: ps_master & msgsock */
			dup(2);		/* re-open 0 */
			dup(2);		/* re-open 1 */
			if (verbose_flag)
				{
				sprintf(str,
					"pre_login: ... _endthread \r\n");
				write(2, str, strlen(str));
				}
			_endthread();	/* TERMINATE pre_login thread */
#else
					/* OVERLAY offspring which is
				running pre_login() with execlp().
				IMPORTANT: note that UNIX shells do NOT
				USE sp_basesh as arg[0]. */
			execlp(sp_shell, "-", NULL);
#endif

	}  /* end of pre_login */

/* ---------------------------------------------------------------------
s_getty utilities:
			int offspring_stop(int spid)
			void parent_intr(int dummy)
			void disp_ctrlc(int dummy)
			int output_login_prompt(int msock);
			int get_schar(int msock)
			int check_host(void)
			int check_user(char uname[])
			int open_psterm(void)	
			int open_os2psterm(void)
   --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	offspring_stop(spid)	
int	spid;
#else
int	offspring_stop(int spid)
#endif
				/* This function stops the offspring
		shell process, but is dependent on whether we are
		running under UNIX (use SIGKILL) or OS/2 (use SIGBREAK -
		i.e. CTRL-C, SIGKILL doesn't work under OS/2). It
		also CLOSES: msgsock & ps_master */
	{
	int	opid, ostatus;

	sleep(1);
#if defined(MSDOS)
	kill(spid, SIGBREAK);
#else
	kill(spid, SIGKILL);
#endif
	opid = wait(&ostatus);
				/* RECYCLE descriptors */
	close(msgsock);
	close(ps_master);
	return(0);
	}  /* end of offspring_kill */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
parent_intr()
#else
void	parent_intr(int dummy)
#endif
				/* this trap was set DURING DEVELOPMENT
		so that CTRL-C from s_getty's terminal could stop both
		parent and offspring. Now we normally run s_getty in 
		the background with only stderr open (and if "nohup" is
		used in invocation, then this will be written to
		the file nohup.out */
	{
	int	opid, ostatus;
				/* notify console or nohup.out */
	write(2, "\r\nSIGINT received by parent(s_getty) \r\n", 37);
	kill(sh_pid, SIGKILL);	/* SURE kill */
	opid = wait(&ostatus);
	write(2,
	  "s_getty: exiting \r\n", 19);
	exit(0);
	}  /* end of parent_intr */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
child_intr()
#else
void	child_intr(int dummy)
#endif
				/* this trap was set DURING DEVELOPMENT,
		see message above on parent_intr() */
	{
	echo_noraw();
	exit(0);
	}  /* end of child_intr */

/* --------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	output_login_prompt(msock)	
int	msock;
#else
int	output_login_prompt(int msock)
#endif
			/* This function outputs a login prompt on
		the designated communication socket */
	{
	int	bytswr;
	char	tmpstr[128];

	sprintf(tmpstr, "\r\n\nUNIX (%s) \n", listener);
	bytswr = write(msock, tmpstr, strlen(tmpstr));
	bytswr = write(msock, "\r\nlogin: ", 9);
	return(0);
	}  /* end of output_login_prompt */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	get_schar(msock)	
int	msock;
#else
int	get_schar(int msock)
#endif
				/* This function maintains the GLOBAL
		buffer gbuf[], filling it when it is empty with read()
		from msock (so it may block). It returns a character 
		to the caller (as an int). If read() returns 0 
		(indicating that the socket has been closed at the
		other end) then it returns EOF to the caller. 
		It is currently used only to get username at the "login: " 
		prompt (BEFORE open_psterm() and fork()) */
	{
	char	str[80];
	int	ret, val, bw;

	if (bufindex < bufnum)			/* char's left to read */
		{
		val = gbuf[bufindex];
		bufindex++;
		}
	else					/* buffer has been read */
		{
		bzero(gbuf,sizeof gbuf);	/* zero buffer */
		bufnum = 0;
		bufindex = 0;
		ret = read(msock, gbuf, PACKET_SIZE);
		if (ret < 0)
			{
			perror("get_schar: read() failed. ");
			return(EOF);		/* fatal ERR */
			}
		else if (ret == 0)
			{
	sprintf(str, "\r\nget_schar: sender has ended connection \r\n");
			bw = write(2, str, strlen(str));
			return(EOF);		/* virtual EOF */
			}
		else				/* ret was > 0 */
			{
			bufnum = ret;
			val = gbuf[0];
			bufindex++;
			}
	  }  /* end of else */

	return(val);
	}  /* end of get_schar */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	check_host()
#else
int	check_host(void)
#endif

			/* This function checks the information
		in struct sockaddr_in caller to see if the calling host
		has a name which can be looked up by gethostbyaddr()
		(and in the process records the information in the global
		struct hostent hcaller) and which is LOCAL in the sense
		that its IP address is of the form:
			LOCAL_IP4 . LOCAL_IP3 . * . *
		If this is the case then it returns 0, otherwise it
		returns -1. */
	{
	char	str[80];
	UINET_32 ul, ulnet;
	int	ret, d1, d2, d3, d4, bw;
	struct hostent *hp;

	if (verbose_flag)
	  {
	  sprintf(str, "valid connection from: \r\n");
	  bw = write(2, str, strlen(str));
	  }
				/* VAX: reverse byte order */
	ulnet = caller.sin_addr.s_addr;		/* use network byte order */
	ul = ntohl(caller.sin_addr.s_addr);	/* use host byte order */
	d1 = 0xff & ul;	d2 = (0xff00 & ul) >> 8;
	d3 = (0xff0000 & ul) >> 16;	d4 = (0xff000000 & ul) >> 24;
	if (verbose_flag)
	  {
	  sprintf(str, "  caller IP address: %d.%d.%d.%d \r\n",d4,d3,d2,d1);
	  bw = write(2, str, strlen(str));
	  }
				/* WARNING: gethostbyaddr() will FAIL
			if there are no other read privileges on /etc/hosts */
	hp = gethostbyaddr(&ulnet, sizeof(ulnet), AF_INET);
	if (hp == NULL)
		{
		sprintf(str, "  gethostbyaddr: failed \r\n");
		bw = write(2, str, strlen(str));
		ret = -1;
		}
	else
		{
		bcopy((char *)hp, (char *)&hcaller, sizeof(struct hostent));
		if (verbose_flag)
		  {
		  sprintf(str, "  get(calling)hostbyaddr: %s \r\n",
			hcaller.h_name);
		  bw = write(2, str, strlen(str));
		  }
			/* allow "localhost" 127.0.0.1 */
		if (  ( (d4 != 127)  &&  (d4 != LOCAL_IP4) )  ||
		      ( (d3 != 0)    &&  (d3 != LOCAL_IP3) )  )
			ret = -1;
		else
			ret = 0;
		}
	if (verbose_flag)
	  {
	  sprintf(str, "  host caller.sin_port: %d \r\n", 
		ntohs(caller.sin_port));
	  bw = write(2, str, strlen(str));
	  }
	return(ret);
	}  /* end of check_host */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	check_user(uname)	
char	uname[];
#else
int	check_user(char uname[])
#endif
				/* This function checks to see that
		there is a USER with the given uname[] on the system. if
		this is the case, the user info is put into the global
		struct passwd spuser and the user's UID is returned,
		otherwise, -1 is returned. OBVIOUSLY a production program 
		might require more security! */
	{
	struct passwd *sppasswdptr;

	sppasswdptr = getpwnam(uname);
	if (sppasswdptr == NULL)
		return(-1);
	else
		{
		bcopy((char *)sppasswdptr, (char *)&spuser,
			 sizeof(struct passwd));
		return(spuser.pw_uid);
		}
	}  /* end of check_user */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	open_psterm()	
#else
int	open_psterm(void)	
#endif

			/* This function attempts to open a pseudo-
		terminal. If successful, it records the master
		descriptor (from pty[p-r][0-f] in the /dev directory) 
		in the global variable ps_master, the slave descriptor 
		(from tty[p-r][0-f] in the /dev directory) in the 
		global variable ps_slave, the respective names
		in ps_mastername[] and ps_slavename[], and returns 0; 
		otherwise it returns -1. */
	{
	int	i, j, k, bw, gotpty;
	char	c, line[16], str[80];

	gotpty = 0;
					/* search for free psterm */
	for (c = 'p' ; !gotpty && c <= 'r' ; c++)
		{
		strcpy(line, "/dev/ptyXX");
		line[sizeof("/dev/pty") - 1] = c;
		line[sizeof("/dev/ptyp") - 1] = '0';
		if (stat(line, &fsbuf) < 0)
			break;		/* suffix [p-r] not found */
		for (i = 0 ; i < 16 ; i++)
			{
			line[sizeof("/dev/ptyp") - 1]
			  = "0123456789abcdef"[i];
			if (verbose_flag)
			  {
			  sprintf(str, "open_psterm: trying %s ... ", line);
			  bw = write(2, str, strlen(str));
			  }
			ps_master = open(line, O_RDWR);
			if (ps_master >= 0)	/* check [0-f] */
				{
				gotpty = 1;
				if (verbose_flag)
					{
					sprintf(str, "opened \r\n");
					bw = write(2, str, strlen(str));
					}
				break;
				}
			else
				{
				if (verbose_flag)
					{
					str[0] = '\r';
					str[1] = '\n';
					bw = write(2, str, 2);
					}
				}
			}  /* end of for (i = .. */
		}  /* end of for (c = .. */
			/* check if psterm has been found */
	if (!gotpty)
		{
		sprintf(str, "open_psterm: ALL network ports in use \r\n");
		bw = write(2, str, strlen(str));
		return(-1);
		}
			/* at this point we have master */
	strcpy(ps_mastername, line);
			/* open slave side */
	line[sizeof("/dev/") - 1] = 't';	/* pty -> tty */
	strcpy(ps_slavename, line);
	ps_slave = open(line, O_RDWR);
	if (ps_slave < 0)
		{
		sprintf(str, "open_psterm: slave open on %s failed \r\n", line);
		bw = write(2, str, strlen(str));
		close(ps_master);
		return(-1);
		}
	return(0);
	}  /* end of open_psterm */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	open_os2psterm()
#else
int	open_os2psterm(void)
#endif
			/* this function simulates the opening of
		a pseudo-terminal under OS/2 (using a socket-pair).
		If successful, it records the master descriptor in the
		global variable ps_master, the slave descriptor in the
		global variable ps_slave, the respective names in
		ps_mastername[] and ps_slavename[], and returns 0;
		otherwise it returns -1. */
	{
	char	str[80];
	int	k, ret, len, d1, d2, d3, d4;
	UINET_32 ul;
	struct	hostent	*hp;

/* ********** CREATE listen SOCKET (i.e. create TSAP of type INTERNET STREAM
			and of protocol 0 = IP protocol) */
	listen_sock = socket(AF_INET,SOCK_STREAM,0);
	if (listen_sock < 0)
		{
		perror("open_os2psterm: socket() failed. ");
		return(-1);
		}

/* ********** BIND NAME TO listen SOCKET (use wildcards and get Portnumber) */
	l_name.sin_family = AF_INET;
	l_name.sin_addr.s_addr = INADDR_ANY; /*   = assigned by system */
	l_name.sin_port = htons(0);	     /* 0 = assigned by system */
	ret = bind(listen_sock,(struct sockaddr *)&l_name,sizeof l_name);
	if (ret < 0)
		{
		perror("open_os2psterm: bind() failed. ");
		close(listen_sock);
		return(-1);
		}

/* ********** USE UTILITIES getsockname, ntohs TO PRINT OUT NAME */
	len = sizeof l_name;
	ret = getsockname(listen_sock,(struct sockaddr *)&l_name,&len);
	if (ret < 0)
		{
		perror("open_os2psterm: getsockname() failed. ");
		close(listen_sock);
		return(-1);
		}
	l_name.sin_family = AF_INET;
	if (verbose_flag)
	  {
	  sprintf(str, "\r\nopen_os2psterm: own net l_name.sin_addr: %ld ",
		l_name.sin_addr.s_addr);
	  write(2, str, strlen(str));
	  }
	  ul = ntohl(l_name.sin_addr.s_addr);
	  d1 = 0xff & ul;	d2 = (0xff00 & ul) >> 8;
	  d3 = (0xff0000 & ul) >> 16;	d4 = (0xff000000 & ul) >> 24;
	if (verbose_flag)
	  {
	  sprintf(str,
		"\r\nopen_os2psterm: own IP address: %d.%d.%d.%d ",d4,d3,d2,d1);
	  write(2, str, strlen(str));
	  sprintf(str,
		"\r\nopen_os2psterm: own net l_name.sin_port: %d ",
		l_name.sin_port);
	  write(2, str, strlen(str));
	  sprintf(str,
		"\r\nopen_os2psterm: own host l_name.sin_port: %d \r\n",
				ntohs(l_name.sin_port));
	  write(2, str, strlen(str));
	  }

/* ********** GET FULL HOSTNAME (localhost) *** */
	hp = gethostbyname("localhost");
	if (hp == NULL)
		{
		perror("open_os2psterm: gethostbyname() failed. ");
		close(listen_sock);
		return(-1);
		}
	strcpy(c_domainname, hp->h_name);	/* copy domainname */
	bcopy((char *)hp->h_addr,(char *)&c_name.sin_addr,hp->h_length);
	c_name.sin_family = AF_INET;
			/* target portnumber = listening portnumber */
	c_name.sin_port = l_name.sin_port;

/* ********** CREATE connect SOCKET (i.e. create TSAP of type INTERNET STREAM
			and of protocol 0 = IP protocol) */
	connect_sock = socket(AF_INET,SOCK_STREAM,0);
	if (connect_sock < 0)
		{
		perror("open_os2psterm: socket() failed. ");
		close(listen_sock);
		return(-1);
		}

/* ********** LISTEN with a QUEUE LENGTH of 2 (DON'T BLOCK) *** */
	listen(listen_sock, 2);

/* ********** CONNECT connect_sock with LISTENer at listen_sock *** */
	ret = connect(connect_sock,(struct sockaddr *)&c_name, sizeof c_name);
	if (ret < 0)
		{
		perror("open_os2psterm: connect() failed. ");
		close(listen_sock);
		close(connect_sock);
		return(-1);
		}
	ps_master = connect_sock;
	strcpy(ps_mastername, "ptty");

/* ********** ACCEPT requested CONNECTion and get MESSAGE_SOCKET *** */
	k = sizeof local_caller;
	ps_slave = accept(listen_sock,(struct sockaddr *)&local_caller,
		(int *)&k);
	if (ps_slave < 0)
		{
		perror("open_os2psterm: accept() failed. ");
		close(listen_sock);
		close(connect_sock);
		return(-1);
		}
	strcpy(ps_slavename, "tty");
	return(0);
	}  /* end of open_os2psterm */

/* --------------------------------------------------------------------
Terminal Utility Routines:
			void disp_ctrlc(int)
			int show_settings(FILE *, struct TERM_STRUCT *)
			int raw_noecho(void)   
			int raw_cbreak(void)
			int fionbio_select(void)
			int echo_noraw(void)			
   -------------------------------------------------------------------- */

#if defined(_NOPROTO)
disp_ctrlc()	
#else
void disp_ctrlc(int dummy)
#endif			
			/* Rather than use signal(SIGINT, SIG_IGN), which
		will completely disable action taken after CTRL-C, this
		interrupt routine is useful in CBREAK mode (raw_cbreak())
		to simply display the string "^C " on stdout */
	{
	ctrlc_hit = 1;			/* set global */
	write(1, "^C ", 3);
	signal(SIGINT, disp_ctrlc);
	}  /* end of disp_ctrlc */

/* --------------------------------------------------------------------
Organization of Terminal Flags in:   struct termio/termios(SUN)
Usually only those marked with (*) are actually changed:

.c_iflag (input)
	IGNBRK		If set, BREAK is ignored
	BRKINT		If set, BREAK generates interrupt
       *IGNPAR		If set, ignore parity and framing errors
       *ISTRIP		If set(usually), 8th bit is stripped
	IGNCR		If set(rare), CR is ignored on input
       *ICRNL		If set(usually), received CR maps to CR-LF
	IXON		If set(usually), CTRL-S/CRTL-Q enabled
	IXOFF		If set(rare), no use of CRTL-S/CRTL-Q
.c_oflag (output)
       *ONLCR		If set(usually), LF maps to CR-LF on output
	OCRNL		If set(rare), CR maps to LF on output
.c_lflag (line discipline)
       *ISIG		If set(usually), INTR, QUIT, (and, on SUN, SUSP)
			   are active
       *ECHO		If set(usually), echo is enabled
       *ICANON		If set(usually), cooked mode with erase (BS on
			   System V, DEL on Berkeley) and kill (CTRL-U)
.c_cc (control chars)
       *.c_cc[VTIME]	If 0, no timeout (on SUN, VTIME=VEOL)
       *.c_cc[VMIN]	If 1, return from read after 1 char (on SUN, VMIN=VEOF)
	.c_cc[VINTR]	SIGINT character, usually DEL or CTRL-C on System V
				and CRTL-C on Berkeley
	.c_cc[VQUIT]	SIGQUIT character, usually CTRL-\ or FS
	.c_cc[VERASE]	Erase with BS or DEL on System V and
				DEL on Berkeley
	.c_cc[VKILL]	Delete entire line with CTRL-U
	.c_cc[VEOF]	End of file/transmission, usually CTRL-D
	.c_cc[VEOL]	Additional (to LF) end of line, usually NUL

SUN only "FEATURES" in struct "termios"
       *.c_cc[VSUSP]	Default CTRL-Z for task switch in csh
NOTE THAT
	signal(SIGTSTP, SIG_IGN)	will disable CRTL-Z.
   -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int show_settings(fp, ptsave)
FILE	*fp;
struct	TERM_STRUCT *ptsave;
#else
int show_settings(FILE *fp, struct TERM_STRUCT *ptsave)
#endif
			/* This function displays the most important
		terminal settings and control characters */
	{

#if defined(MSDOS)
	fprintf(fp,
"ERASE  KILL    [TANDEM]  [CBREAK]  [LCASE]   [ECHO]    [CRMOD]    [RAW] ");
	fprintf(fp, "\r\n");
	fprintf(fp, " %2.2Xh   ", ptsave->sg_erase);
	fprintf(fp, " %2.2Xh   ", ptsave->sg_kill);
	if (ptsave->sg_flags & TANDEM)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & CBREAK)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & LCASE)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & ECHO)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & CRMOD)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & RAW)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	fprintf(fp, "\r\n");
#else
	fprintf(fp,
"IGNBRK IGNPAR ISTRIP IGNCR  ICRNL  IXON   IXOFF  ISIG [VSUSP] [VTIME] [VEOF]");
	fprintf(fp, "\r\n");
	if (ptsave->c_iflag & IGNBRK)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & IGNPAR)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & ISTRIP)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & IGNCR)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & ICRNL)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & IXON)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & IXOFF)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_lflag & ISIG)
		fprintf(fp, " set  ");
	else
		fprintf(fp, "      ");
	fprintf(fp, "[%2.2Xh] ",ptsave->c_cc[VSUSP]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VTIME]);
	fprintf(fp, " [%2.2Xh] ",ptsave->c_cc[VEOF]);
	fprintf(fp, "\r\n");
	fprintf(fp,
"BRKINT ONLCR  OCRNL  ECHO   ICANON   [VINTR] [VQUIT] [VKILL] [VERASE] [VMIN]");
	fprintf(fp, "\r\n");
	if (ptsave->c_iflag & BRKINT)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_oflag & ONLCR)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_oflag & OCRNL)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_lflag & ECHO)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_lflag & ICANON)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	fprintf(fp, "   [%2.2Xh] ",ptsave->c_cc[VINTR]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VQUIT]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VKILL]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VERASE]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VMIN]);
	fprintf(fp, "\r\n");
#endif

	return(0);
	}  /* end of show_settings */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int raw_noecho()   
#else
int raw_noecho(void)   
#endif
			/* This function sets full RAW mode for single
		character I/O, that is, getchar() waits for a
		single ascii character and returns when a key is
		pressed. Since ISIG is DISABLED, CTRL-C (SIGINT)
		will return 0x03 and CTRL-\ (SIGQUIT) will return
		0x1B=FS. Do NOT use this mode with select(). */
	{
	int	ret, zero;
	struct	TERM_STRUCT tinfo;

	if (!termsave_done) 
		{
		ret = ioctl(0,READ_TERMIOS,&tsave);	
		termsave_done = 1;
		}
	ret = ioctl(0,READ_TERMIOS,&tinfo);
	zero = 0;

#if defined(MSDOS)
	tinfo.sg_erase = 0x00;
	tinfo.sg_kill = 0x00;
	tinfo.sg_flags = (tsave.sg_flags & ~(CBREAK | ECHO | CRMOD)) | RAW;
#else
				 	/* modify settings in tinfo */
	tinfo.c_iflag = (tsave.c_iflag & ~ICRNL) | IGNPAR |
		ISTRIP;  /* turn off   cr cr-lf  conversion, ignore
		parity errors, strip off 8th bit */
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR | OCRNL);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON | ISIG);  /* turn
		off echo and erase and kill functions */
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 1;    /* return from read after 1 character */
	tinfo.c_cc[VSUSP] = 0;	 /* disable CTRL-Z (SUSPEND) */
#endif

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = ioctl(0,WRITE_TERMIOS,&tinfo);	/* set RAW mode */
	return(0);
	}  /* end of raw_noecho */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int raw_cbreak()   
#else
int raw_cbreak(void)   
#endif
			/* This function sets UNIX CBREAK mode for single
		character I/O, that is, getchar() waits for a
		single ascii character and returns when a key is
		pushed. Since ISIG has NOT been changed, CTRL-C
		(SIGINT) and CTRL-\ (SIGQUIT) are still ACTIVE.
		Do NOT use this mode with select(). */
	{
	int	ret, zero;
	struct	TERM_STRUCT tinfo;

	if (!termsave_done) 
		{
		ret = ioctl(0,READ_TERMIOS,&tsave);
		termsave_done = 1;
		}
	ret = ioctl(0,READ_TERMIOS,&tinfo);
	zero = 0;

#if defined(MSDOS)
	tinfo.sg_erase = 0x00;
	tinfo.sg_kill = 0x00;
	tinfo.sg_flags = (tsave.sg_flags & ~(RAW | ECHO | CRMOD)) | CBREAK;
#else
				 	/* modify settings in tinfo */
	tinfo.c_iflag = (tsave.c_iflag & ~ICRNL) | IGNPAR |
		ISTRIP;  /* turn off   cr cr-lf  conversion, ignore
		parity errors, strip off 8th bit */
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR | OCRNL);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON);  /* turn off
		echo and erase functions */
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 1;    /* return from read after 1 character */
	tinfo.c_cc[VSUSP] = 0;	 /* disable CTRL-Z (SUSPEND) */
#endif

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = ioctl(0,WRITE_TERMIOS,&tinfo);	/* set CBREAK mode */
	return(0);
	}  /* end of raw_cbreak */

/* ------------------------------------------------------------------- */

#if defined(_NOPROTO)
int fionbio_select()
#else
int fionbio_select(void)
#endif
			/* This function is adapted for use with 
		non-blocking I/O (FIONBIOF) and the select() function
		(so VMIN=VTIME=0) and the rest is like cooked mode
		EXCEPT ECHO and ICANON are OFF. */
	{
	int	ret, one;
	struct	TERM_STRUCT tinfo;

	if (!termsave_done) 
		{
		ret = ioctl(0,READ_TERMIOS,&tsave);
		termsave_done = 1;
		}
	ret = ioctl(0,READ_TERMIOS,&tinfo);
	one = 1;

#if defined(MSDOS)
	tinfo.sg_erase = 0x00;
	tinfo.sg_kill = 0x00;
/*	tinfo.sg_flags = (tsave.sg_flags & ~(CBREAK | ECHO | CRMOD)) | RAW;
		struct sgttyb differs from termios HERE */
	tinfo.sg_flags = (tsave.sg_flags & ~(RAW | ECHO | CRMOD)) | CBREAK;
#else
				 	/* modify settings in tinfo */
	tinfo.c_iflag = tsave.c_iflag | ICRNL | IGNPAR |
		ISTRIP;  /* turn on   cr cr-lf  conversion, still ignore
		parity errors, still strip off 8th bit */
	tinfo.c_oflag = tsave.c_oflag & ~ONLCR;  /* turn off lf cr-lf
		conversion (otherwise vi will fail) */
	tinfo.c_lflag = (tsave.c_lflag & ~(ECHO | ICANON)) | ISIG; /* turn
		off echo and erase but save kill function */
	tinfo.c_cc[VTIME] = 0;   /* none */
	tinfo.c_cc[VMIN] = 0;    /* none */
	tinfo.c_cc[VSUSP] = 0; /* disable CTRL-Z (SUSPEND) */
#endif
	
	ioctl(0, FIONBIO, &one);	/* set non-blocking I/O */
	ret = ioctl(0,WRITE_TERMIOS,&tinfo);	/* set COOKED mode */
	return(0);
	}  /* end of fionbio_select */

/* ------------------------------------------------------------------- */

#if defined(_NOPROTO)
int echo_noraw()  
#else
int echo_noraw(void)  
#endif 
			/* This function sets the normal, default, 
		UNIX COOKED mode (also called line-edited mode) */
	{
	int	ret, zero;
	struct	TERM_STRUCT tinfo;

	if (!termsave_done) 
		{
		ret = ioctl(0,READ_TERMIOS,&tsave);
		termsave_done = 1;
		}
	ret = ioctl(0,READ_TERMIOS,&tinfo);
	zero = 0;

#if defined(MSDOS)
	tinfo.sg_erase = 0x08;	/* CTRL-H active */
	tinfo.sg_kill = 0x15;	/* CTRL-U active */
	tinfo.sg_flags = (tsave.sg_flags & ~(CBREAK | RAW)) | ECHO | CRMOD;
#else
				 	/* modify settings in tinfo */
	tinfo.c_iflag = tsave.c_iflag | ICRNL | IGNPAR |
		ISTRIP;  /* turn on   cr cr-lf  conversion, still ignore
		parity errors, still strip off 8th bit */
	tinfo.c_oflag = tsave.c_oflag | ONLCR;  /* turn on lf cr-lf
		conversion */
	tinfo.c_lflag = tsave.c_lflag | ECHO | ICANON | ISIG;  /* turn
		on echo and erase and kill functions */
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 4;    /* return from read after 4 characters */
	tinfo.c_cc[VSUSP] = 0x1A; /* enable CTRL-Z (SUSPEND) */
#endif

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = ioctl(0,WRITE_TERMIOS,&tinfo);	/* set COOKED mode */
	return(0);
	}  /* end of echo_noraw */

