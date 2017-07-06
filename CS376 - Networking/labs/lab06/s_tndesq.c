/* s_tlnt:	GENERAL:
			This program is a UNIX-OS/2 "raw socket version"
			of telnet. The UNIX invocation line should be
			as follows:

			$ s_tlnt [-v] [hostname] [portnumber]

			where the "-v" is a "verbose flag" showing more
			information as s_tlnt runs, hostname is the
			symbolic name of the host (e.g. "localhost",
			"sparc1", etc.) and the portnumber is where the
			desired connection is to be made (typically it
			is the portnumber where s_getty is listening).
			This program opens a socket and initiates
			a connection with the socket whose portnumber was
			given in the command line. If successful, TERMINAL
			mode is entered and messages are sent as a stream
			to the receiver and any reply is printed out on the
			screen. It expects that the listening process
			uses the same terminal mode conventions as
			s_getty, getty, or in.telnetd. You can BREAK to
			COMMAND line mode:

			telnet>

			with TEL_BREAK where the "help" command will show
			all command line options and terminal options.
			When using s_tlnt with s_getty, you can also use
			the CONTROL characters:

			GET_REINIT	to kill (remote) shell and 
					re-initialize s_getty.
			GET_DOKILL	to kill (remote) shell and terminate
					s_getty.

		OS/2 DIFFERENCES:
			There is no difference in invocation or operation.
			Internally, s_tlnt MUST trap some ANSI ESCape
			sequences (e.g. set keypadmode application) which
			OS/2 ANSI.SYS will incorrectly interpret (typically
			by doing somthing STRANGE like entering 40 column
			mode).

		MAKING:
			Under UNIX, compilation and linking is straight-
			forward. OS/2 compilation and linking is more
			involved and it is easier to give a sample
			makefile (which should be run with Microsoft's
			NMAKE) as follows:

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
1. CTRL-]  BREAK to command_mode                     |   |
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

#include <signal.h>		/* for signal(), kill() */
#include <fcntl.h>		/* for low-level I/O */
#include <stdio.h>

#define	TEL_BREAK	0x1D	/* CTRL-]: s_tlnet break to command mode */
#define	GET_REINIT	0x1E	/* CTRL-^: kill shell and re-spawn s_getty */
#define	GET_DOKILL	0x0B	/* CTRL-K: kill s_getty */
#define	SIGLETTERS	3	/* Number of significant letters for
				   s_telnet commands */

#define SELECT_TIMEOUT 600	/* timeout value in seconds on select(),
				   10 minutes seems fair. */
#define ULIMIT 256		/* PROBLEM: ulimit() option 4 is
				   non-existent on Ultrix and the old
				   getdtablesize() seems to be in the
				   process of being phased out on the SUN */

				/* Some Conditional includes/defines */
#if defined(__vax) || defined(__mips)	/* ULTRIX OS */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/termios.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyname() */
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */
#define READ_TERMIOS TCGETP
#define WRITE_TERMIOS TCSANOW

#elif defined(DESQ)		/* DJGCC/DESQview/X package */

#include <pc.h>			/* for kbhit(), UNFORTUNATELY termio
				   is NOT supported */
#include <sys/ioctl.h>
#include <sys/types.h>

struct timeval {		/* kluge for select() in socket.h */
  long tv_sec;
  long tv_usec;
};

#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyname() */
#include <curses.h>

struct sgttyb {			/* some stuff MISSING in ioctl.h */
	char sg_ispeed;
	char sg_ospeed;
	char sg_erase;
	char sg_kill;
	short sg_flags;
};

#define TANDEM 0x0001
#define CBREAK 0x0002
#define LCASE 0x0004
#define ECHO 0x0008
#define CRMOD 0x0010
#define RAW 0x0020
#define ODDP 0x0040
#define EVENP 0x0080
#define ANYP (ODDP|EVENP)
#define XTABS 0x0100

#define TERM_STRUCT sgttyb	/* this is old-style BSD 4.3 */
#define READ_TERMIOS TIOCGETP
#define WRITE_TERMIOS TIOCSETP

#elif defined(MSDOS)		/* OS/2 */

#include <pctcp.h>		/* includes <sys/types.h> */
#include <bsd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyname() */
#define TERM_STRUCT sgttyb	/* this is old-style BSD 4.3 */
#define READ_TERMIOS TIOCGETP
#define WRITE_TERMIOS TIOCSETP

#else				/* SUN OS */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/termios.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyname() */
#include <sys/filio.h>		/* for FIONBIO */
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */
#define READ_TERMIOS TCGETS
#define WRITE_TERMIOS TCSETS

#endif

#define	LOCAL_IP2 201
#define LOCAL_IP3 168
#define	LOCAL_IP4 136
				/* GLOBAL STATE VARIABLES: main() */
int	verbose_flag;		/* set when "-v" option given */
int	in_terminalmode;	/* SET when talking to remote connection */
int	in_snoopymode;		/* SET when in terminalmode and showing
				       ALL control characters */
int	in_commandmode;		/* SET when at "telnet> " prompt */
char	connect_host[128];	/*   connection: hostname */
int	connect_port;		/*   connection: port */
int	connection_opened;	/* set after successful connect() */

				/* GLOBAL ADDRESS VARIABLES: telnet_open() */
char	connect_domainname[128];/* set by telnet_open(): full domainname */
struct	sockaddr_in	name;	/* formal socket name */
/* in <netdb.h>
struct	hostent {
	char	*h_name;
	char	**h_aliases;
	int	h_addrtype;
	int	h_length;
	char	**h_addr_list;
#define	h_addr	h_addr_list[0]
}
   in <in.h>
struct	sockaddr_in  {
	short	sin_family;
	u_short	sin_port;
	struct	in_addr	sin_addr;
	char	sin_zero[8];  
}	*/
				/* GLOBALS: for telnet> getcmd() */
char	raw_cmdline[128];/* raw "telnet>" command line */
char	cmdbuf[1024];	/* buffer for command line that user will type.
			this is filled by getcmd() and MODIFIED in
			the process of parsing. */
char	*cmd[32];	/* array for pointers to strings which are
			command line arguments (note that a pointer
			to a string is the same as a pointer to char).
			these pointers will actually point to
			sub-strings in cmdbuf[]. */
int	ctrlc_received;	/* set by disp_ctrlc(), used to recover when
			I/O has been interrupted */
int	reinit_sent;	/* SET if we sent CTRL-^ to s_getty */
int	dokill_sent;	/* SET if we sent CTRL-K to s_getty */

			/* ESCape Sequence Traps (OS/2 ONLY). Note
			that OS/2 ANSI.SYS uses " ESC [ ? num h " to set
			Video_Mode = num ! */
int	esc_last;		/* ESC previously */
int	escbrack_last;		/* ESC [ previously */
int	escbrackquest_last;	/* ESC [ ? previously */

/* =================================================================== */

main(argc,argv,envp)
int	argc;			/* number of commmand line arg's */
char	*argv[],*envp[];	/* argument and environmental pointers */
	{
	char	str[80];	/* miscellaneous use */
	char	chrline[80], answer[80];	/* for command line mode */
	int	sock;		/* descriptor for IP STREAM socket */
	int	disp_ctrlc();	/* forward reference */
	int	length, ret, i, j, k, d1, d2, d3, d4;
	int	arguments, byteswritten, tmout;
#if defined(DESQ)
	int	desqi;
	long	nonBlocking = 1L;
#endif
	unsigned long	ul;
				/* next data structures are for select() */
	char	ibuf[256];	/* input buffer, s_tlnt <-- port */
	char	pbuf[256];	/* output buffer, s_tlnt --> port */
	fd_set	readmask, writemask, exceptmask;
	struct	timeval timeout;/* struct timeval is defined as:
		long	tv_sec;		   seconds and
		long	tv_usec;	   micro-seconds */

/* ********** INITIALIZE GLOBAL VARIABLES */
	in_terminalmode = 0;
	in_snoopymode = 0;
	in_commandmode = 0;
	connection_opened = 0;
	ctrlc_received = 0;
	reinit_sent = 0;
	dokill_sent = 0;

/* ********** SET-UP SIGNAL CONTROL *** */
	signal(SIGQUIT, SIG_IGN);	/* DON'T allow writable CORE dumps */
	
/* ********** CHECK COMMAND LINE for verbose_flag, hostname and portnumber */
	verbose_flag = 0;			/* default = silent */
	connect_host[0] = '\0';
	connect_port = -1;
	if (argc > 1)
	  for (i = 1 ; i < argc ; i++)
		{
		if (strcmp(argv[i], "-v") == 0)
			verbose_flag = 1;
		else if ( (*argv[i] >= '0') && (*argv[i] <= '9') )
			connect_port = atoi(argv[i]);
		else
			strcpy(connect_host, argv[i]);
		}
	if ( (connect_host[0] == '\0')  ||  (connect_port < 0) )
		{
		printf("\ns_tlnt: invoke this program via: \n");
		printf("  s_tlnt [-v] [hostname] [portnumber] \n");
		printf("or use, at the telnet> prompt,: \n");
		printf("         open [hostname] [portnumber] \n");
		goto commandmode;
		}

#if defined(DESQ)
	printf("DESQ has been defined\n");
	fflush(stdout);
#endif

/* ********** ATTEMPT TO CONNECT TO TARGET: */
	sock = telnet_open(connect_host, connect_port);

/* ********** CHECK return CODE *** */
	if (sock == -1)
		{
		perror("s_tlnt: gethostbyname() failed. ");
		printf("        are you sure hostname is correct? \n");
		exit(-1);
		}
	if (sock == -2)
		{
		perror("s_tlnt: socket() failed. ");
		exit(-1);
		}
	if (sock == -3)
		{
		perror("s_tlnt: connect() failed. ");
		printf("        are you sure that portnumber is correct? \n");
		exit(-1);
		}
	if (verbose_flag)
	  {
	  printf("\ns_tlnt: target net name.sin_addr: %ld",
		name.sin_addr.s_addr);
	  }
	  ul = ntohl(name.sin_addr.s_addr);	/* VAX: reverse byte order */
	  d1 = 0xff & ul;	d2 = (0xff00 & ul) >> 8;
	  d3 = (0xff0000 & ul) >> 16;	d4 = (0xff000000 & ul) >> 24;
	if (verbose_flag)
	  {
	  printf("\ns_tlnt: target IP address: %d.%d.%d.%d ",d4,d3,d2,d1);
	  printf("\ns_tlnt: target net name.sin_port: %d ",name.sin_port);
						/* VAX: reverse byte order */
	  printf("\ns_tlnt: target host name.sin_port: %d \n",
			ntohs(name.sin_port));
	  }
	connection_opened = 1;		/* STATUS: connection is open */


/* ********** IN_TERMINALMODE: READ-WRITE LOOP (fionbio_select) */
terminalmode:
	fionbio_select();	/* set VMIN=VTIME=0, ECHO, ICANON off,
				   and (IMPORTANT!) ONLCR off since
				   we are going through s_getty and
				   ONLCR will give us a SECOND CR */
#if defined(DESQ)
	/* The USUAL problem with signal(): Is the 2nd parameter an int
	   or a function returning pointer to void ? */

	signal(SIGINT, (SignalHandler)disp_ctrlc);
#else
	signal(SIGINT, disp_ctrlc);	/* trap CTRL-C and display ^C */
#endif
	in_terminalmode = 1;		/* STATUS: terminal emulation */
	in_commandmode = 0;
	reinit_sent = 0;
	dokill_sent = 0;
	printf("\r\nConnected to %s ", connect_domainname);
	printf("\r\nEscape character is '^]' \r\n");
	bzero(pbuf, sizeof pbuf);
	bzero(ibuf, sizeof ibuf);

#if defined(DESQ)
	/* Need to mark socket as non-Blocking. */

	ioctl(sock, FIONBIO, (char *)&nonBlocking);
#endif

selectloop:
		/* SETUP FOR: select() */

#if defined(DESQ)
	/* Apparently DJGCC/DESQview/X dislikes anything other
	   than a socket descriptor in select(), otherwise
	   select() returns an error. Hence the separate check
	   after a short timeout for kbhit(). */

	FD_ZERO(&readmask);
	FD_ZERO(&writemask);
	FD_ZERO(&exceptmask);
	FD_SET(sock, &exceptmask);
	FD_SET(sock, &readmask);
		/* set timeout values */
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000L; 	/* 1/10 second */
#else
	FD_ZERO(&readmask);
	FD_ZERO(&writemask);
	FD_ZERO(&exceptmask);
	FD_SET(0, &readmask);
	FD_SET(sock, &exceptmask);
	FD_SET(sock, &readmask);
		/* set timeout values */
	timeout.tv_sec = SELECT_TIMEOUT;
	timeout.tv_usec = 0;
#endif
/* ********** BLOCK ON select(): ***************** */

#if defined(DESQ)
	/* Apparently DJGCC/DESQview/X cannot handle very many
	   descriptors at once, so we use (sock + 1) instead
	   of ULIMIT. We also have to treat stdio (descriptor 0)
	   differently since it is NOT a socket. */
	tmout = select(sock + 1, &readmask, &writemask,
		&exceptmask, &timeout);
#else
	tmout = select(ULIMIT, &readmask, &writemask,
		&exceptmask, &timeout);
#endif
		/* CHECK: TIMEOUT (select() normally returns the
		   number of file descriptors selected. if zero
		   is returned, a timeout occurred. if -1 is
		   returned, an error occurred) */
	if (tmout == 0)
		{
#if defined(DESQ)
	/* After timeout on select() we have to manually check the
	   PC keyboard with kbhit() */
		if (kbhit())
			{
			FD_SET(0, &readmask);
			goto readstdin;
			}
		else
			{
			goto selectloop;
			}
#else
		printf("s_tlnt: select timeout \r\n");
		connection_opened = 0;
		close(sock);
		goto commandmode;
#endif
		}
		/* CHECK: EXCEPTION on select: this is MOST OFTEN
		   caused by CTRL-C (SIGINT) interrupting I/O. */
	if (tmout < 0)
		{
		if (ctrlc_received)
			{
			ctrlc_received = 0;
			str[0] = 0x03;

#if defined(DESQ)
	/* write() seems to fail with DVX sockets, so we use send() with
	   flags = 0 instead */

			send(sock, str, 1, 0);
#else
			write(sock, str, 1);	/* send CTRL-C */
#endif
			goto selectloop;
			}

#if defined(DESQ)
	/* close() seems to fail on sockets in DVX (i.e they are NOT
	   closed and you eventually run out of ports on ETHDRV, so
	   we use their so_close() */

		perror("s_tlnt: DESQview/X: ");
		connection_opened = 0;
		so_close(sock);
#else
		printf("s_tlnt: select error \r\n");
		connection_opened = 0;
		close(sock);
#endif
		goto commandmode;
		}
		/* CHECK: EXCEPTION on socket */
	if (FD_ISSET(sock, &exceptmask))
		{
		printf("s_tlnt: exception on socket \r\n");
		connection_opened = 0;
#if defined(DESQ)
	/* close() seems to fail on sockets in DVX (i.e they are NOT
	   closed and you eventually run out of ports on ETHDRV, so
	   we use their so_close() */

		so_close(sock);
#else
		close(sock);
#endif
		goto commandmode;
		}
		/* CHECK: stdin(read) --> socket(write) */
readstdin:
	if (FD_ISSET(0, &readmask))
		{

#if defined(DESQ)
	/* Since termios.h is not present we have to do transparent,
	   raw_noecho I/O in this PC system dependent way. */
		pbuf[0] = (unsigned char)(getkey() & 0x007f);
		desqi = 1;
		while((kbhit())  &&  (desqi < 80))
			{
			pbuf[desqi++] = (unsigned char)(getkey() & 0x007f);
			}  /* end of while */
		ret = desqi;
#else
		ret = read(0, pbuf, 80);
#endif
		if (ret <= 0)
			{
			printf("s_tlnt: null stdin read \r\n");
			connection_opened = 0;
#if defined(DESQ)
	/* close() seems to fail on sockets in DVX (i.e they are NOT
	   closed and you eventually run out of ports on ETHDRV, so
	   we use their so_close() */

			so_close(sock);
#else
			close(sock);
#endif
			goto commandmode;
			}
		else
			{
			for (j = 0 ; j < ret ; j++)
				{
					/* Check for:
		1. (UNIX and OS/2) CONTROL characters in output STREAM
		2. (OS/2 only) mapping: CTRL-H (BS) -> DEL */

#if defined(MSDOS)
				if (pbuf[j] == 010)		/* BS */
					{
					pbuf[j] = 0177;		/* DEL */
					}
#endif
				if (pbuf[j] == TEL_BREAK)
					{
					printf("\r\n");
					goto commandmode;
					}
				else if (pbuf[j] == GET_REINIT)
					{
					reinit_sent = 1;
					}
				else if (pbuf[j] == GET_DOKILL)
					{
					dokill_sent = 1;
					}

				}  /* end of for */

#if defined(DESQ)
	/* write() seems to fail with DVX sockets, so we use send() with
	   flags = 0 instead */

			byteswritten = send(sock, pbuf, ret, 0);
#else
			byteswritten = write(sock, pbuf, ret);
#endif
			if (byteswritten <= 0)
				{
				printf("s_tlnt: socket has been closed \r\n");
				connection_opened = 0;
#if defined(DESQ)
	/* close() seems to fail on sockets in DVX (i.e they are NOT
	   closed and you eventually run out of ports on ETHDRV, so
	   we use their so_close() */

				so_close(sock);
#else
				close(sock);
#endif
				goto commandmode;
				}
			}
		bzero(pbuf, sizeof pbuf);
		}
		/* CHECK: socket(read) --> stout(write) */
	if (FD_ISSET(sock, &readmask))
		{

#if defined(DESQ)
	/* read() seems to fail with DVX sockets, so we use recv() with
	   flags = 0 instead */

		ret = recv(sock, ibuf, 80, 0);
#else
		ret = read(sock, ibuf, 80);
#endif
		if (ret <= 0)
			{
				/* This next is most often caused by
			  a NORMAL EXIT (CTRL-D) by the (remote) shell
			printf("s_tlnt: null read on socket \r\n");  */
			printf(
			  "\r\nConnection closed by foreign host \r\n");
			connection_opened = 0;
			if (reinit_sent)
				{
				reinit_sent = 0;
				goto commandmode;  /* ALLOW RE-LOGIN */
				}
			else if (dokill_sent)
				{
				dokill_sent = 0;
#if defined(DESQ)
	/* close() seems to fail on sockets in DVX (i.e they are NOT
	   closed and you eventually run out of ports on ETHDRV, so
	   we use their so_close() */

				so_close(sock);
#else
				close(sock);
#endif
				echo_noraw();
				exit(0);  /* NORMAL EXIT with GETTY KILL */
				}
			else
				{
#if defined(DESQ)
	/* close() seems to fail on sockets in DVX (i.e they are NOT
	   closed and you eventually run out of ports on ETHDRV, so
	   we use their so_close() */

				so_close(sock);
#else
				close(sock);
#endif
				echo_noraw();
				exit(0);  /* NORMAL EXIT without GETTY KILL */
				}
			} 
				/* Check:
		1. (OS/2 only) ANSI ESCape sequences (like ESC [ ? 1 h
			which ANSI.SYS will STUPIDLY interpret
			as "the user desires 40 column mode")). We will
			fix this below, but note that vi will still not
			reverse scroll properly since ESC 7 (save cursor)
			and ESC 8 (restore cursor) aren't recognized by
			ANSI.SYS (it uses ESC [ s   and   ESC [ u
			instead). In summary, FREQUENT use of CTRL-L
			to re-sync screen in vi is ADVISABLE */
#if defined(MSDOS)
		else if ( (ret > 0)  &&  (in_snoopymode == 0) )
			{
			for (j = 0 ; j < ret ; j++)
				{
				if (ibuf[j] == 033)
					{
					esc_last = 1;	/* ESC */
					}
				else if ( (esc_last) &&
						(ibuf[j] == '[') )
					{
					esc_last = 0;
					escbrack_last = 1;
					}
				else if ( (escbrack_last) &&
						(ibuf[j] == '?') )
					{
					escbrack_last = 0;
					escbrackquest_last = 1;
					}
				else if ( (escbrackquest_last) &&
						(ibuf[j] == '1') )
					{
					escbrackquest_last = 0;
					ibuf[j] = '3';	/* CO80 */
					}
				else			/* RESET traps */
					{
					esc_last = 0;
					escbrack_last = 0;
					escbrackquest_last = 0;
					}
				}  /* end of for */
			byteswritten = write(1, ibuf, ret);
			}
#endif
		else if ( (ret > 0) && (in_snoopymode) ) 
			{ 
			for (j = 0 ; j < ret ; j++) 
				{ 
				if (ibuf[j] == 033)	/* ESCape */
					{
					str[0] = '^';	  /* display CTRL-[ */
					str[1] = '[';
					write(1, str, 2); /* DON'T show */
					}
				else if (ibuf[j] < 32) 
					{ 
					str[0] = '^'; 	  /* display CTRL-x */
					str[1] = ibuf[j] + 64; 
					str[2] = ibuf[j]; /* and SHOW */
					write(1, str, 3); 
					} 
				else if (ibuf[j] > 127)
					{
					write(1, &ibuf[j], 1);
					}
				else
					{
					write(1, &ibuf[j], 1);
					}
				}
			}
		else
			{
			byteswritten = write(1, ibuf, ret);
			}
		bzero(ibuf, sizeof ibuf);
		}
	goto selectloop;


/* ********** IN_COMMANDMODE: READ-WRITE LOOP (echo_noraw) */
commandmode:
	echo_noraw();			/* restore line-edited I/O */
	signal(SIGINT, SIG_DFL);	/* restore CTRL-C */
	in_terminalmode = 0;
	in_commandmode = 1;		/* STATUS: at "telnet> " prompt */
	printf("telnet> ");
	arguments = getcmd();		/* get and parse command line */
	if (arguments >= 1)
		strcpy(answer, cmd[0]);
	else
		answer[0] = '\0';
	if ( (strncmp(answer, "quit", SIGLETTERS) == 0) || 
	     (strncmp(answer, "exit", SIGLETTERS) == 0) )
		{
		if (connection_opened)
			{
			printf("s_tlnt: closing connection \n");
#if defined(DESQ)
	/* close() seems to fail on sockets in DVX (i.e they are NOT
	   closed and you eventually run out of ports on ETHDRV, so
	   we use their so_close() */

			so_close(sock);
#else
			close(sock);
#endif
			}
		exit(0);
		}
	else if (strncmp(answer, "return", SIGLETTERS) == 0)
		{
		if (!connection_opened)
			printf(
		  "s_tlnt: cannot return, no connection is open \n");
		else
			goto terminalmode;
		}
	else if (strncmp(answer, "open", SIGLETTERS) == 0)
		{
		if (connection_opened)		/* check open connection */
		  {
		  printf(
		  "s_tlnt: connection is already open, use close first \n");
		  }
		else
		  {				/* check parameters */
		  if ( (cmd[1] == NULL) || (cmd[2] == NULL) )
			{
			printf(
			  "s_tlnt syntax: open [hostname] [portnumber] \n");
			}
		  else				/* try to open connection */
			{
			strcpy(connect_host, cmd[1]);
			connect_port = atoi(cmd[2]);
			sock = telnet_open(connect_host, connect_port);
			if (sock < 0)
				{			/* failure */
				printf("telnet_open: failed \n");
				}
			else				/* success */
				{
				connection_opened = 1;
				goto terminalmode;
				}
			}
		  }
		}
	else if (strncmp(answer, "close", SIGLETTERS) == 0)
		{
		if (connection_opened)
			{
			printf("s_tlnt: closing connection to %s port %d \n",
				connect_host, connect_port);
			connection_opened = 0;
#if defined(DESQ)
	/* close() seems to fail on sockets in DVX (i.e they are NOT
	   closed and you eventually run out of ports on ETHDRV, so
	   we use their so_close() */

			so_close(sock);
#else
			close(sock);
#endif
			}
		else
			{
			printf("s_tlnt: no connection is open \n");
			}
		}
	else if (strncmp(answer, "snoopy", SIGLETTERS) == 0)
		{
		in_snoopymode = 1;	/* set for transparent mode */
		}
	else if (strncmp(answer, "nosnoopy", SIGLETTERS) == 0)
		{
		in_snoopymode = 0;	/* set for line-edited mode */
		}
	else if (strncmp(answer, "status", SIGLETTERS) == 0)
		{
		if (connection_opened)
			printf("connection: open to %s port %d",
				connect_host, connect_port);
		else
			printf("connection: <none>");
		if (in_snoopymode)
			printf("    terminal: transparent mode \n");
		else
			printf("    terminal: line-edited mode \n");
		}
	else		/* help */
		{
	printf("telnet> commands: \n");
	printf("  close       _ close existing connection \n");
	printf("  exit        _ close any connection and exit \n");
	printf("  help        - print this command summary \n");
	printf("  nosnoopy    - reset to line edited terminal mode \n");
	printf("  open [hostname] [portnumber] - open connection \n");
	printf("  quit        - close any connection and exit \n");
	printf("  return      - return to terminal mode \n");
	printf("  snoopy      - set transparent terminal mode \n");
	printf("  status      - show current telnet status \n");
	printf("terminal mode control sequences: \n");
	printf("  CTRL-]      - escape to command mode \n");
	printf("  CTRL-^      - kill (remote) shell, re-init socket getty \n");
	printf("  CTRL-K      - kill (remote) socket getty \n");
		;
		}
	goto commandmode;

	}  /* end of main */

/* ----------------------------------------------------------------------
Utilities:
		telnet_open()
		getcmd()
   ---------------------------------------------------------------------- */

int	telnet_open(host_name, host_port)		/* This function
		accepts a host_name (e.g. "sparc1") and a host_port
		(e.g. 1492) and attempts to make a connection to the
		desired socket (there must, of course, be a processs
		listening at the target). If it is successful, it
		fills in the global struct sockaddr_in name, and
		returns the descriptor of a socket which can be used
		for read()'s and write()'s. If not, it prints out an
		error message and returns -1 (if gethostbyname() failed)
		and -2 (if socket creation failed) or -3 (if connect()
		failed). */
char	host_name[];
int	host_port;
	{
	int	sock;
	int	ret;
	struct	hostent	*hp;		/* for gethostbyname() */

/* ********** GET FULL HOSTNAME *** */
	hp = gethostbyname(host_name);
	if (hp == NULL)
		return(-1);	/* gethostbyname() failed */
			/* make copy of formal host-domainname */
	strcpy(connect_domainname, hp->h_name);
			/* copy into GLOBAL struct sockaddr_in name */
	bcopy((char *)hp->h_addr,(char *)&name.sin_addr,hp->h_length);
	name.sin_family = AF_INET;
	name.sin_port = htons(host_port);	/* convert atoi(string)
			host integer to network representation of integer
			with correct byte order. */

/* ********** CREATE SOCKET (i.e. create TSAP of type INTERNET STREAM and of
			protocol 0 = IP protocol) */
	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock < 0)
		return(-2);

/* ********** CONNECT TO RECEIVING SOCKET using HOSTNAME and given PORT */
	ret = connect(sock,(struct sockaddr *)&name,sizeof name);
	if (ret < 0)
		{
#if defined(DESQ)
	/* close() seems to fail on sockets in DVX (i.e they are NOT
	   closed and you eventually run out of ports on ETHDRV, so
	   we use their so_close() */

		so_close(sock);
#else
		close(sock);
#endif
		return(-3);
		}
	return(sock);			/* SUCCESS */
	}  /* end of telnet_open */

/* -------------------------------------------------------------------- */

int getcmd()   /* this is a crude parser, spaces and tabs are delimeters
	      between tokens. the raw command line is put into both
	      raw_cmdline[] and cmdbuf[] and the array cmd[] of
	      pointers is set to point to each of the arguments
	      starting with cmd[0]. Note that cmdbuf[] is MODIFIED in
	      the process. Also note that the last pointer is set
	      to NULL (this is necessary in order to use exec() and
	      is one feature where exec() DIFFERS from main()).
	      Thee number of command line arguments (essentially argc)
	      is returned. for example if the user types 

	      $$ <space>/bin/ps  <tab> -ef<cr>

	      then  cmd[0] --> "/bin/ps"  and  cmd[1] --> "-ef"     */
	{
	int	i,j;

	gets(cmdbuf);  /* get raw command line, note that
			   gets() replaces '\n' by '\0' */
	strcpy(raw_cmdline, cmdbuf);	/* make copy */
	i = 0;
	j = 0;
	while(cmdbuf[i] != '\0')
		{
		for( ; (cmdbuf[i] == ' ') ||
		       (cmdbuf[i] == '\t') ; i++);  /* skip tab, space */
		cmd[j] = &cmdbuf[i];  /* set ptr to string or '\0' */
		if (cmdbuf[i] == '\0') break;    /* at eol */
		for( ; (cmdbuf[i] > 32) &&
		       (cmdbuf[i] < 127) ; i++); /* pass over string */
		j++;				  /* ready for next */
		if (cmdbuf[i] == '\0') break;    /* at eol */
		cmdbuf[i] = '\0';   /* terminate string ,i.e. replace
					delimeter by '\0'   */
		i++;
		}
	cmd[j] = (char *)NULL;  /* last entry in cmd[] is NULL ptr */

	return(j);		/* return number of arguments */
	}  /* end of getcmd */

/* --------------------------------------------------------------------
Terminal Routines:
			disp_ctrlc()
			show_settings()
			raw_noecho()
			raw_cbreak()
			fionbio_select()
			echo_noraw()			
   -------------------------------------------------------------------- */

disp_ctrlc()		/* Rather than use signal(SIGINT, SIG_IGN), which
		will completely disable action taken after CTRL-C, this
		interrupt routine is useful in CBREAK mode (raw_cbreak())
		to simply display the string "^C " BS BS on stdout
		and setting the semaphore ctrlc_received so that the
		select() error created will be recoverable. */
	{
	char	str[16];

	str[0] = '^';
	str[1] = 'C';
	str[2] = 0x08;		/* BS */
	str[3] = 0x08;		/* BS */
	write(1, str, 4);	/* insurance, so we are informed if
		(remote) shell doesn't respond and echo "^C" */
	ctrlc_received = 1;

#if defined(DESQ)
	/* The USUAL problem with signal(): Is the 2nd parameter an int
	   or a function returning pointer to void ? */

	signal(SIGINT, (SignalHandler)disp_ctrlc);
#else
	signal(SIGINT, disp_ctrlc);
#endif
	}  /* end of display_ctrlc */

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
	.c_cc[VINTR]	SIGINT character, usually DEL on System V
				and CRTL-C on Berkeley
	.c_cc[VQUIT]	SIGQUIT character, usually CTRL-\ or FS
	.c_cc[VERASE]	Erase with BS on System V and
				DEL on Berkeley
	.c_cc[VKILL]	Delete entire line with CTRL-U
	.c_cc[VEOF]	End of file/transmission, usually CTRL-D
	.c_cc[VEOL]	Additional (to LF) end of line, usually NUL

SUN only "FEATURES" in struct "termios"
       *.c_cc[VSUSP]	Default CTRL-Z for task switch in csh
NOTE THAT
	signal(SIGTSTP, SIG_IGN)	will disable CRTL-Z.
   -------------------------------------------------------------------- */

int show_settings(ptsave)
struct	TERM_STRUCT *ptsave;
	{

#if defined(MSDOS)
	printf(
"ERASE  KILL    [TANDEM]  [CBREAK]  [LCASE]   [ECHO]    [CRMOD]    [RAW] ");
	printf("\r\n");
	printf(" %2.2Xh   ", ptsave->sg_erase);
	printf(" %2.2Xh   ", ptsave->sg_kill);
	if (ptsave->sg_flags & TANDEM)
		printf("   set    ");
	else
		printf("          ");
	if (ptsave->sg_flags & CBREAK)
		printf("   set    ");
	else
		printf("          ");
	if (ptsave->sg_flags & LCASE)
		printf("   set    ");
	else
		printf("          ");
	if (ptsave->sg_flags & ECHO)
		printf("   set    ");
	else
		printf("          ");
	if (ptsave->sg_flags & CRMOD)
		printf("   set    ");
	else
		printf("          ");
	if (ptsave->sg_flags & RAW)
		printf("   set    ");
	else
		printf("          ");
	printf("\r\n");
#else
	printf(
"IGNBRK IGNPAR ISTRIP IGNCR  ICRNL  IXON   IXOFF  ISIG [VSUSP] [VTIME] [VEOF]");
	printf("\r\n");
	if (ptsave->c_iflag & IGNBRK)
		printf("  set  ");
	else
		printf("       ");
	if (ptsave->c_iflag & IGNPAR)
		printf("  set  ");
	else
		printf("       ");
	if (ptsave->c_iflag & ISTRIP)
		printf("  set  ");
	else
		printf("       ");
	if (ptsave->c_iflag & IGNCR)
		printf(" set   ");
	else
		printf("       ");
	if (ptsave->c_iflag & ICRNL)
		printf(" set   ");
	else
		printf("       ");
	if (ptsave->c_iflag & IXON)
		printf(" set   ");
	else
		printf("       ");
	if (ptsave->c_iflag & IXOFF)
		printf("  set  ");
	else
		printf("       ");
	if (ptsave->c_lflag & ISIG)
		printf(" set  ");
	else
		printf("      ");
	printf("[%2.2Xh] ",ptsave->c_cc[VSUSP]);
	printf("  [%2.2Xh] ",ptsave->c_cc[VTIME]);
	printf(" [%2.2Xh] ",ptsave->c_cc[VEOF]);
	printf("\r\n");
	printf(
"BRKINT ONLCR  OCRNL  ECHO   ICANON   [VINTR] [VQUIT] [VKILL] [VERASE] [VMIN]");
	printf("\r\n");
	if (ptsave->c_iflag & BRKINT)
		printf("  set  ");
	else
		printf("       ");
	if (ptsave->c_oflag & ONLCR)
		printf("  set  ");
	else
		printf("       ");
	if (ptsave->c_oflag & OCRNL)
		printf(" set   ");
	else
		printf("       ");
	if (ptsave->c_lflag & ECHO)
		printf(" set   ");
	else
		printf("       ");
	if (ptsave->c_lflag & ICANON)
		printf(" set   ");
	else
		printf("       ");
	printf("   [%2.2Xh] ",ptsave->c_cc[VINTR]);
	printf("  [%2.2Xh] ",ptsave->c_cc[VQUIT]);
	printf("  [%2.2Xh] ",ptsave->c_cc[VKILL]);
	printf("  [%2.2Xh] ",ptsave->c_cc[VERASE]);
	printf("  [%2.2Xh] ",ptsave->c_cc[VMIN]);
	printf("\r\n");
#endif

	return(0);
	}  /* end of show_settings */

/* -------------------------------------------------------------------- */

int raw_noecho()   /* This function sets full RAW mode for single
		character I/O, that is, getchar() waits for a
		single ascii character and returns when a key is
		pressed. Since ISIG is DISABLED, CTRL-C (SIGINT)
		will return 0x03 and CTRL-\ (SIGQUIT) will return
		0x1B=FS. Do NOT use this mode with select(). */
	{
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

#if defined(DESQ)

#else
	ret = ioctl(0,READ_TERMIOS,&tsave);	/* get current settings */
	ret = ioctl(0,READ_TERMIOS,&tinfo);
#endif
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
/*	signal(SIGTSTP, SIG_IGN);   Needed for classic SUN sloppiness: if
				    you use struct termio (instead of
				    termios) you may deadlock processes
				    with CTRL-Z */
#endif

#if defined(DESQ)

#else
	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = ioctl(0,WRITE_TERMIOS,&tinfo);	/* set RAW mode */
#endif
	return(0);
	}  /* end of raw_noecho */

/* -------------------------------------------------------------------- */

int raw_cbreak()   /* This function sets UNIX CBREAK mode for single
		character I/O, that is, getchar() waits for a
		single ascii character and returns when a key is
		pushed. Since ISIG has NOT been changed, CTRL-C
		(SIGINT) and CTRL-\ (SIGQUIT) are still ACTIVE.
		Do NOT use this mode with select(). */
	{
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

#if defined(DESQ)

#else
	ret = ioctl(0,READ_TERMIOS,&tsave);	/* get current settings */
	ret = ioctl(0,READ_TERMIOS,&tinfo);
#endif
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
/*	signal(SIGTSTP, SIG_IGN);   Needed for classic SUN sloppiness: if
				    you use struct termio (instead of
				    termios) you may deadlock processes
				    with CTRL-Z */
#endif

#if defined(DESQ)

#else
	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = ioctl(0,WRITE_TERMIOS,&tinfo);	/* set CBREAK mode */
#endif
	return(0);
	}  /* end of raw_cbreak */

/* ------------------------------------------------------------------- */

int fionbio_select()	/* This function is adapted for use with 
		non-blocking I/O (FIONBIOF) and select()
		(so VMIN=VTIME=0) and the rest is like cooked mode
		EXCEPT ECHO and ICANON are OFF. */
	{
	int	ret, one;
	struct	TERM_STRUCT tsave, tinfo;

#if defined(DESQ)

#else
	ret = ioctl(0,READ_TERMIOS,&tsave);	/* get current settings */
	ret = ioctl(0,READ_TERMIOS,&tinfo);
#endif
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
/*	signal(SIGTSTP, SIG_IGN);   Needed for classic SUN sloppiness: if
				    you use struct termio (instead of
				    termios) you may deadlock processes
				    with CTRL-Z */
#endif

#if defined(DESQ)

#else	
	ioctl(0, FIONBIO, &one);	/* set non-blocking I/O */
	ret = ioctl(0,WRITE_TERMIOS,&tinfo);	/* set COOKED mode */
#endif
	return(0);
	}  /* end of fionbio_select */

/* ------------------------------------------------------------------- */

int echo_noraw()   /* This function sets the normal, default, UNIX
		COOKED mode (also called line-edited mode) */
	{
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

#if defined(DESQ)

#else
	ret = ioctl(0,READ_TERMIOS,&tsave);	/* get current settings */
	ret = ioctl(0,READ_TERMIOS,&tinfo);
#endif
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
/*	signal(SIGTSTP, SIG_IGN);   Needed for classic SUN sloppiness: if
				    you use struct termio (instead of
				    termios) you may deadlock processes
				    with CTRL-Z */
#endif

#if defined(DESQ)

#else
	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = ioctl(0,WRITE_TERMIOS,&tinfo);	/* set COOKED mode */
#endif
	return(0);
	}  /* end of echo_noraw */

