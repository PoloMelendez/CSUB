

	/* s_tlnt: 	This program is a UNIX-WinNT "raw socket version" 
		of telnet. The UNIX invocation line should be as follows:

		$ s_tlnt [-v] [-snoopy] [hostname] [portnumber]

		where the "-v" is a "verbose flag" showing more information 
		as s_tlnt runs, "-snoopy" puts the terminal in transparent 
		mode (showing all control characters, characters with the 8th
		bit set, and any arbitration), hostname is the symbolic name 
		of the host (e.g. "localhost", "ultrix6", etc.) and the 
		portnumber is where the desired connection is to be made 
		(typically it is the portnumber where s_getty/s_daemon is 
		listening).  This program opens a socket and initiates
		a connection with the socket whose portnumber was given in 
		the command line. If successful, TERMINAL mode is entered 
		and messages are sent as a stream to the receiver and any 
		reply is printed out on the screen. It expects that the 
		listening process uses the same terminal mode conventions as
		s_getty, getty, or in.telnetd. You can BREAK to COMMAND 
		line mode:

		s_tlnt>

		with TEL_BREAK where the "help" command will show all command 
		line options and terminal options.  When using s_tlnt with 
		s_getty, you can also use the CONTROL characters:

		GET_REINIT	to kill (remote) shell and 
					re-initialize s_getty.
		GET_DOKILL	to kill (remote) shell and terminate
					s_getty.

		FUNCTION AS A GENERAL TELNET WITH ARBITRATION:
		If you use this program to connect to a REAL telnetd on tcp 
		port 23, it will support the usual telnet arbitration (see 
		rfc854, 855,856...) of DO,DONT,WILL,WONT. That is

		s_tlnt [hostname] 23

		will function as a MINIMAL but real telnet. On Windows NT
		there will be LIMITED ANSI screen handling because 
		Microsoft made the decision NOT to support ANSI.SYS-type
		escape sequences in consoles (cmd.exe) and we have only 
		PARTIAL support for this in process_esc(). See also the 
		function telnet_arb().
	
							Marc Thomas

Functional Diagram (under WinNT, pre_login: is a thread not a process):

           S_TLNT:
           --------------------
          | command_mode       |               
          |                    |              
          |                    |             
          |                    |            
          |      telnet_open() |    TCP/IP 
          |          connect() |---------------
          |                   ---->          ----> 
          |                    |---------------
stdio     |                    |           sock
----------| echo_noraw()       |              
<--    --> --------------------|             
----------| fionbio_select()   |            
          |          |         |    TCP/IP    
          |          |         |---------------
          |       select() --<----           ---->
          |                    |---------------
          |                    |sock    msgsock
          |                    |               
          |                    |              
          |                    |             
          | terminal_mode      |            
           --------------------            
                                        
S_TLNT CONTROL-codes:                  
1. CTRL-T  BREAK to command_mode      
2. CTRL-^  re-init S_GETTY           
3. CTRL-K  do-kill on S_GETTY       
                                   
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>		/* for signal(), kill() */
#include <fcntl.h>		/* for low-level I/O */
#ifndef TELCMDS
#define TELCMDS			/* we want to use these in telnet.h */
#endif
#ifndef TELOPTS
#define TELOPTS			/* we want to use these in telnet.h */
#endif

/* 
 * ---------------------------------------------------------------- */

#include "header.h"	/* OUR header which handles MOST of the
			WinSock/32-bit Unix/64-bit Unix Differences */
#include "startup.h"	/* globals for main module set by the
			code startup.c */
#include "specio.h"	/* OUR header file for routines and defines
			for special terminal/socket I/O */
#include "diagnost.h"	/* OUR dump_xxxx() routines and other
			diagnostics */

/* -----------------------------------------------------------------
 */

#define	TEL_BREAK	0x14	/* CTRL-T: s_tlnet break to command mode */
#define	GET_REINIT	0x1E	/* CTRL-^: kill shell and re-spawn s_getty */
#define	GET_DOKILL	0x0B	/* CTRL-K: kill s_getty */
#define	SIGLETTERS	3	/* Number of significant letters for
				   s_telnet commands */
#define SELECT_TIMEOUT 600	/* timeout value in seconds on select(),
				   10 minutes seems fair with NO
				   negotiation possible. */
#define NEGOT_TIMEOUT	60	/* ONLY 60 seconds before we send
				   IAC AYT (are you there?) */
#define	LOCAL_IP2 201
#define LOCAL_IP3 168
#define	LOCAL_IP4 136
				/* GLOBALS: for main(),  */
int	verbose_flag;		/* set when "-v" option given */

struct	TERM_STRUCT tsave;	/* raw_noecho(),... etc. save settings */
struct	TERM_STRUCT toriginal;	/* raw_noecho(),... etc. original settings */
int	termsave_done = 0;	/* set when ORIGINAL settings SAVED */
int	current_mode =  0;	/* ECHO_NORAW_MODE */
#define	ECHO_NORAW_MODE	  0
#define	CBREAK_MODE	  1
#define	RAW_NOECHO_MODE	  2
#define	SELECT_MODE	  3
#define NOECHO_NORAW_MODE 4

int	in_terminalmode;	/* SET when talking to remote connection */
int	in_snoopymode;		/* SET when in terminalmode and showing
				       ALL control characters */
int	real_telnet;		/* SET when port IS the telnet (23) port */
int	in_commandmode;		/* SET when at "s_tlnt> " prompt */
char	connect_host[128];	/*   connection: hostname */
int	connect_port;		/*   connection: port */
int	connection_opened;	/* set after successful connect() */

				/* GLOBAL ADDRESS VARIABLES: telnet_open() */
char	connect_domainname[128];/* set by telnet_open(): full domainname */
struct	sockaddr_in	name;	/* formal socket name */

				/* GLOBALS: for telnet_arb() */
int	arb_state = 0;		/* state of telnet arbitration (if used) */
				/* Options FOLLOWING are 
					+1 for WILL/DO (ack) 
					 0 for other end didn't ask
					-1 for WONT/DONT (nack) */
int	neg_active = 0;		/*   set if we start getting IAC's */
int	negotiated_ttype = 0;	/*   terminal type */
char	  ttytype[64];		/*     for name of terminal */
int	negotiated_norcrd = 0;	/*   CR disposition */
int	negotiated_sga = 0;	/*   suppress go ahead */
int	negotiated_binary = 0;	/*   8-bit data path */
int	negotiated_echo = 0;	/*   echo */
int	negotiated_lflow = 0;	/*   remote flow control */

				/* GLOBALS: for s_tlnt> getcmd() */
char	raw_cmdline[128];/* raw "s_tlnt>" command line */
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

			/* ESCape Sequence Traps WinNT. Note that OS/2 
			ANSI.SYS uses " ESC [ ? num h " to set
			Video_Mode = num ! */
int	esc_last;		/* ESC previously */
int	escbrack_last;		/* ESC [ previously */
int	escinfo_last;		/* ESC [ info previously
			info is numeric [0-9] punctuation ; etc. */
int	esctermchar_last;	/* ESC [ info TERM_CHAR */
int	escbrackquest_last;	/* ESC [ ? previously */
#define	MAX_ESC_SEQ	64
char	esc_message[ MAX_ESC_SEQ + 1 ];
int	esc_index;

SOCKET	sock;		/* descriptor for IP STREAM socket (this must
			be global for process_esc()'s sake) */

				/* Function Prototypes */
#if defined(_NOPROTO)

int	telnet_arb();
int	telnet_open();
int	getcmd();
int	update_title();
int	process_esc();
	disp_ctrlc();
int	raw_noecho();
int	raw_cbreak();
int	fionbio_select();
int	echo_noraw();

#else

int	telnet_arb(char buf[], int *iptr, int max, SOCKET wsock);
int	telnet_open(char host_name[], int host_port);
int	getcmd(void);
int	update_title(void);
int	process_esc(char msg[]);
void	disp_ctrlc(int dummy);
int	raw_noecho(void); 
int	raw_cbreak(void);
int	fionbio_select(void);
int	echo_noraw(void);

#endif

/* =================================================================== */

#if defined(_NOPROTO)
	main(argc, argv, envp)
	int	argc;	
	char	*argv[],*envp[];
#else
int	main(int argc, char *argv[], char *envp[])
#endif
	{
	char	str[80];	/* miscellaneous use */
	char	answer[80];	/* for command line mode */
	int	ret, i, j, k, d1, d2, d3, d4;
	int	arguments, byteswritten, tmout;
	UINET_32 ul;
				/* next data structures are for select() */
	char	ibuf[256];	/* input buffer, s_tlnt <-- port */
	char	pbuf[256];	/* output buffer, s_tlnt --> port */
	fd_set	readmask, writemask, exceptmask;
	struct	timeval timeout;/* struct timeval is defined as:
		INT_32	tv_sec;		   seconds and
		INT_32	tv_usec;	   micro-seconds */


#include "startup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix startup. Main modules
			ALSO require the inclusion of 
				header.h
				startup.h */

/* ********** INITIALIZE GLOBAL VARIABLES ******************************* */
	in_terminalmode = 0;
	in_snoopymode = 0;
	real_telnet = 0;
	in_commandmode = 0;
	connection_opened = 0;
	ctrlc_received = 0;
	reinit_sent = 0;
	dokill_sent = 0;

/* ********** SET-UP SIGNAL CONTROL ************************************** */
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
		else if (strcmp(argv[i], "-verbose") == 0)
			verbose_flag = 1;
		else if (strcmp(argv[i], "-snoopy") == 0)
			in_snoopymode = 1;
		else if ( (*argv[i] >= '0') && (*argv[i] <= '9') )
			connect_port = atoi(argv[i]);
		else
			strcpy(connect_host, argv[i]);
		}
	if ( (connect_host[0] == '\0')  ||  (connect_port < 0) )
		{
		printf("\ns_tlnt: invoke this program via: \n");
		printf(
	"          s_tlnt [-v] [-snoopy] [hostname] [portnumber] \n");
		printf("or enter, at the s_tlnt> prompt,: \n");
		printf("          open [hostname] [portnumber] \n");
		fflush(stdout);
		goto commandmode;
		}
	if (connect_port == 23)
		real_telnet = 1;	/* reserved service port */

/* ********** ATTEMPT TO CONNECT TO TARGET: ****************************** */
	sock = telnet_open(connect_host, connect_port);

/* ********** CHECK return CODE ****************************************** */
	if (sock == -1)
		{
		perror("s_tlnt: gethostbyname() failed. ");
		printf("        are you sure hostname is correct? \n");
		fflush(stdout);
		return(-1);
		}
	if (sock == -2)
		{
		perror("s_tlnt: socket() failed. ");
		return(-1);
		}
	if (sock == -3)
		{
		perror("s_tlnt: connect() failed. ");
		printf("        are you sure that portnumber is correct? \n");
		fflush(stdout);
		return(-1);
		}
	  ul = ntohl(name.sin_addr.s_addr);	/* VAX: reverse byte order */
	  d1 = 0xff & ul;	d2 = (0xff00 & ul) >> 8;
	  d3 = (0xff0000 & ul) >> 16;	d4 = (0xff000000 & ul) >> 24;
	if (verbose_flag)
	  {
	  printf("\ns_tlnt: target IP address: %d.%d.%d.%d ",d4,d3,d2,d1);
	  printf("\ns_tlnt: target host name.sin_port: %d \n",
			ntohs(name.sin_port));
	  fflush(stdout);
	  }

newterminalmode:	/* begin newly "opened" connection */

			/* we show window size, position in WinNT */
	update_title();

	connection_opened = 1;		/* STATUS: connection is open */
					/* RESET: negotiation flags */
	neg_active = 0;		
	negotiated_ttype = 0;
	  ttytype[0] = '\0';
	negotiated_norcrd = 0;
	negotiated_sga = 0;
	negotiated_binary = 0;
	negotiated_echo = 0;
	negotiated_lflow = 0;


/* ********** IN_TERMINALMODE: READ-WRITE LOOP (fionbio_select) ********** */
terminalmode:
	fionbio_select();	/* set SELECT_MODE:
			VMIN=VTIME=0, ECHO, ICANON off, and (IMPORTANT!) 
			ONLCR off since we are going through s_getty and
			ONLCR will give us a SECOND CR */

	signal(SIGINT, disp_ctrlc);	/* trap CTRL-C and display ^C */
	in_terminalmode = 1;		/* STATUS: terminal emulation */
	in_commandmode = 0;
	reinit_sent = 0;
	dokill_sent = 0;
	printf("\r\nConnected to %s ", connect_domainname);
	printf("\r\nEscape character is '^T' \r\n");
	fflush(stdout);
	bzero(pbuf, sizeof pbuf);
	bzero(ibuf, sizeof ibuf);

selectloop:
		/* SETUP: select() loop: we will watch
			1. reads on stdin (exceptions would 
			   be RARE here)
			2. reads and exceptions on sock  */

		/* BGBullshit complication: some genius at Microsoft
		decided that select() ONLY handles socket descriptors;
		you CANNOT mix (which was the main reason the call was
		added to BSD 4.2 in 1984) stdin, stdout, pipes, etc. 
		with sockets in select() - *** AMAZING *** ... */

#if defined(_WIN95) || defined(_WINNT)

		/* ... so we *** FAKE *** it */

	FD_ZERO(&readmask);
	FD_ZERO(&writemask);
	FD_ZERO(&exceptmask);
			 		/* set timeout values */
	timeout.tv_sec = 0;		/* set a VERY SMALL micro-sec
				timeout (almost non-BLOCKING) so that
				we don't PEG THE CPU AT 100% */
	timeout.tv_usec = 500;

	if (_kbhit() != 0)
		{
		readmask.fd_array[0] = 0x0001;
		tmout = 1;
		}

/* ********** NON-BLOCK (almost) ON SELECT: ****************************** */

	else
		{
		FD_SET(sock, &exceptmask);
		FD_SET(sock, &readmask);
		tmout = select(ULIMIT, &readmask, &writemask,
			&exceptmask, &timeout);
		}
	if (tmout == 0)
		goto selectloop;	/* we are NON-blocking 
				so timeout has NO meaning */
#else
	FD_ZERO(&readmask);
	FD_ZERO(&writemask);
	FD_ZERO(&exceptmask);
	FD_SET(0, &readmask);
	FD_SET(sock, &exceptmask);
	FD_SET(sock, &readmask);

	if (neg_active) 		/* set timeout values */
		timeout.tv_sec = NEGOT_TIMEOUT;
	else	/* NO ACTIVE negotiation, set LONGER timeout */
		timeout.tv_sec = SELECT_TIMEOUT;
	timeout.tv_usec = 0;

/* ********** BLOCK ON select(): ***************************************** */

	tmout = select(ULIMIT, &readmask, &writemask,
		&exceptmask, &timeout);
#endif

	/* CHECK: TIMEOUT (select() normally returns the
		   number of file descriptors selected. if zero
		   is returned, a timeout occurred. if -1 is
		   returned, an error occurred) */
	if (tmout == 0)
		{
		if (neg_active)
			{
			str[0] = (char)IAC;
			str[1] = (char)AYT;	/* are you there? */
			send(sock, str, 2, 0);
			sprintf(str, " [us:IAC,AYT] ");
			write(1, str, strlen(str)); 
			goto selectloop;	/* try again */
			}
		printf("s_tlnt: select timeout \r\n");
		fflush(stdout);
		connection_opened = 0;
		CLOSE_SOCKET(sock);
		goto commandmode;
		}
	/* CHECK: EXCEPTION on select: this is MOST OFTEN
		   caused by CTRL-C (SIGINT) interrupting I/O. */
	if (tmout < 0)
		{
		if (ctrlc_received)
			{
			ctrlc_received = 0;
			str[0] = 0x03;
			send(sock, str, 1, 0);	/* send CTRL-C */
			goto selectloop;
			}
		printf("s_tlnt: select error \r\n");
		fflush(stdout);
		connection_opened = 0;
		CLOSE_SOCKET(sock);
		tmout = 0;			/* reset */
		goto commandmode;
		}
	/* CHECK: EXCEPTION on socket */
	if (FD_ISSET(sock, &exceptmask))
		{
		printf("s_tlnt: exception on socket \r\n");
		fflush(stdout);
		connection_opened = 0;
		CLOSE_SOCKET(sock);
		tmout = 0;			/* reset */
		goto commandmode;
		}

	/* CHECK: stdin(read) --> socket(write) */
#if defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	if (readmask.fd_array[0] & 0x0001)
#else
	if (FD_ISSET(0, &readmask))
#endif
		{

#if defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

		if (current_mode != ECHO_NORAW_MODE)
		    {
		    ret = 0;
		    do
			{
			pbuf[ret++] = (char)(_getch() & 0xff);
			} while ( (_kbhit() != 0) && (ret < 80) );
		    }
		else
		    {
		    ret = read(0, pbuf, 80);
		    }
#else
		ret = read(0, pbuf, 80);
#endif
		if (ret <= 0)
			{
			printf("s_tlnt: null stdin recv \r\n");
			fflush(stdout);
			connection_opened = 0;
			CLOSE_SOCKET(sock);
			tmout = 0;		/* reset */
			goto commandmode;
			}
		else
			{
			for (j = 0 ; j < ret ; j++)
				{
					/* Check for:
		1. (UNIX and OS/2) CONTROL characters in output STREAM
		2. (OS/2 only) mapping: CTRL-H (BS) -> DEL */

#if defined(_WIN95) || defined(_WINNT)

				if (pbuf[j] == 010)		/* BS */
					{
					pbuf[j] = 0177;		/* DEL */
					}
#endif
				if (pbuf[j] == TEL_BREAK)
					{
					printf("\r\n");
					fflush(stdout);
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
			byteswritten = send(sock, pbuf, ret, 0);
		/* a PRODUCTION PROGRAM should check here to see
		   if (byteswritten < ret) due to high load and take
		   proper action (whatever that might be) */
			if (byteswritten <= 0)
				{
				printf("s_tlnt: socket has been closed \r\n");
				fflush(stdout);
				connection_opened = 0;
				CLOSE_SOCKET(sock);
				tmout = 0;	/* reset */
				goto commandmode;
				}
			}
		if (tmout > 0)
			tmout--;		/* reset */
		bzero(pbuf, sizeof pbuf);
		}
	/* CHECK: socket(recv) --> stout(write) */
	if (FD_ISSET(sock, &readmask))
		{
		ret = recv(sock, ibuf, 80, 0);
		if (ret <= 0)
			{
				/* This next is most often caused by
			  a NORMAL EXIT (CTRL-D) by the (remote) shell
			printf("s_tlnt: null recv on socket \r\n");  */
			printf(
			  "\r\nConnection closed by foreign host \r\n");
			fflush(stdout);
			connection_opened = 0;
			if (reinit_sent)
				{
				reinit_sent = 0;
				goto commandmode;  /* ALLOW RE-LOGIN */
				}
			else if (dokill_sent)
				{
				dokill_sent = 0;
				CLOSE_SOCKET(sock);
				echo_noraw();
/* ************** THIS IS A NORMAL EXIT POINT with s_getty KILL ********** */

#include "cleanup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */

				return(0);
				}
			else
				{
				CLOSE_SOCKET(sock);
				echo_noraw();
/* ************** THIS IS A NORMAL EXIT POINT WITHOUT s_getty KILL ******* */

#include "cleanup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */

				return(0);
				}
			} 
		else if ( (ret > 0) && (in_snoopymode) ) 
			{ 
			for (j = 0 ; j < ret ; j++) 
				{ 
					/* check if 8th bit set: this
		  may be CRAZY connection or a REAL telnet arbitration.. */
				if (
					(arb_state != 0) ||
					((ibuf[j] & 0x80) != 0x00) )
				  {
	/* CHECK: REAL TELNET arbitration (starting with IAC)... */
				  if (telnet_arb(ibuf, &j, ret, sock) < 0)
					{
						/* process it ourselves */
					sprintf(str, " %2.2X",
						(ibuf[j] & 0x000000ff));
					write(1, str, 3); /* DON'T echo */
				 	}
				  }
				else if (ibuf[j] == 033)  /* ESCape */
					{
					str[0] = '^';	  /* display CTRL-[ */
					str[1] = '[';
					write(1, str, 2); /* DON'T echo */
					}
				else if (ibuf[j] == 0x00)
					{
					sprintf(str, "[NL]");
					write(1, str, 4);
					}
				else if (ibuf[j] == '\n')
					{
					sprintf(str, "[LF]\n");
					write(1, str, 5);
					}
				else if (ibuf[j] == '\r')
					{
					sprintf(str, "[CR]\r\n");
					write(1, str, 6);
					} 
				else if (ibuf[j] == '\t')
					{
					sprintf(str, "[TB]\t");
					write(1, str, 5);
					}
				else if (ibuf[j] < 32) 	  /* OTHER */
					{ 
					str[0] = '^'; 	  /* display CTRL-x */
					str[1] = ibuf[j] + 64; 
					str[2] = ibuf[j]; /* and ECHO */
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
				}  /* end of for */
			}
		else 				/* non-snoopy */
			{
			for (j = 0 ; j < ret ; j++) 
				{ 
					/* check if 8th bit set: this
		  may be CRAZY connection or a REAL telnet arbitration.. */
				if (
					(arb_state != 0) ||
					((ibuf[j] & 0x80) != 0x00) )
				  {
	/* CHECK: REAL TELNET arbitration (starting with IAC)... */
				  if (telnet_arb(ibuf, &j, ret, sock) < 0)
					{
						/* process it ourselves */
					sprintf(str, " %2.2X",
						(ibuf[j] & 0x000000ff));
					write(1, str, 3); /* DON'T echo */
				 	}
				  }

			/* Another *** COMPLICATION *** since cmd.exe 
		sessions in WinNT are non-ANSI we currently monitor 
		ESCape sequences but pass them on to process_esc(). 
		Most of the sequences start with 
			ESCape [ (or, less commonly, ESCape O ) 
		but there are also the SPECIAL single-char sequences:
			ESCape c
			ESCape 7
			ESCape 8 ...
		and see process_esc() for details */

				else if ( (real_telnet) &&
						(ibuf[j] == '\r') )
					{
							/* CR is
				problematic on WinNT in that the
				console likes to ignore it - this
				is not so good in vi */

					esc_message[0] = '\r';
					esc_message[1] = '\0';
					k = process_esc(esc_message);
					}
				else if ( (real_telnet) &&
						(ibuf[j] == '\n') )
					{
							/* LF is
				problematic on WinNT in that the
				console likes to supply a CR - this
				is not so good in vi */

					esc_message[0] = '\n';
					esc_message[1] = '\0';
					k = process_esc(esc_message);
					}
				else if (ibuf[j] == 033)
					{
					esc_last = 1;	/* ESC */
					esc_index = 0;
					esc_message[esc_index] = '\0';
					}
				else if ( (esc_index < MAX_ESC_SEQ) &&
					  (esc_last) &&
				( (ibuf[j] == 'c') || 
				  (ibuf[j] == 'A') || (ibuf[j] == 'B') ||
				  (ibuf[j] == 'C') || (ibuf[j] == 'D') ||
				  (ibuf[j] == 'H') || (ibuf[j] == 'I') ||
				  (ibuf[j] == 'J') || (ibuf[j] == 'K') ||
				  (ibuf[j] == 'M') ||
				  (ibuf[j] == '7') ||
				  (ibuf[j] == '8') ) )
					{
						/* RESET the TRAP */
					esc_last = 0;
					esc_message[esc_index++] = ibuf[j];
					esc_message[esc_index] = '\0';
					esc_index = 0;
					k = process_esc(esc_message);
						/* check verbose flag */
					if ( (k != 0) || (verbose_flag) )
					  {
					  printf(
				"\r\n [them:Esc%s] ", 
						esc_message);
					  fflush(stdout);
					  }
					}
				else if ( (esc_index < MAX_ESC_SEQ) &&
					  (esc_last) &&
				( (ibuf[j] == '[') || (ibuf[j] == 'O') ) )
					{
					esc_last = 0;
					escbrack_last = 1;
					esc_message[esc_index++] = ibuf[j];
					esc_message[esc_index] = '\0';
					}
				else if ( (esc_index < MAX_ESC_SEQ) &&
					  (escbrack_last)  &&
				(
				  ( (ibuf[j] >= '0') && (ibuf[j] <= '9') ) ||
				    (ibuf[j] == ';') ||
				    (ibuf[j] == '?')
				) )
					{
						/* start of info parameters 
					separated by semicolons or prefaced
					by question marks */
					escbrack_last = 0;
					escinfo_last = 1;
					esc_message[esc_index++] = ibuf[j];
					esc_message[esc_index] = '\0';
					}
				else if ( (esc_index < MAX_ESC_SEQ) &&
					  (escbrack_last) )
					{
						/* a short sequence:
					ESCape [ single_letter 
						so RESET the TRAP */
					escbrack_last = 0;
					esc_message[esc_index++] = ibuf[j];
					esc_message[esc_index] = '\0';
					esc_index = 0;
					k = process_esc(esc_message);
						/* check verbose flag */
					if ( (k != 0) || (verbose_flag) )
					  {
					  printf(
				"\r\n [them:Esc%s] ", 
						esc_message);
					  fflush(stdout);
					  }
					}
				else if ( (esc_index < MAX_ESC_SEQ) &&
					  (escinfo_last)  &&
				(
				  ( (ibuf[j] >= '0') && (ibuf[j] <= '9') ) ||
				    (ibuf[j] == ';') ||
				    (ibuf[j] == '?')
				) )
					{
						/* STAY in info as long
						as parameters continue */
					esc_message[esc_index++] = ibuf[j];
					esc_message[esc_index] = '\0';
					}
				else if ( (esc_index < MAX_ESC_SEQ) &&
					  (escinfo_last) )
					{
						/* RESET the TRAP */
					escinfo_last = 0;
					esc_message[esc_index++] = ibuf[j];
					esc_message[esc_index] = '\0';
					esc_index = 0;
					k = process_esc(esc_message);
						/* check verbose flag */
					if ( (k != 0) || (verbose_flag) )
					  {
					  printf(
				"\r\n [them:Esc%s] ", 
						esc_message);
					  fflush(stdout);
					  }
					}
				else if (esc_index >= MAX_ESC_SEQ) 
					{
						/* RESET the TRAP */
					escinfo_last = 0;
					esc_index = 0;
					printf(
				"\r\n [them:Esc%s..] too long \r\n", 
						esc_message);
					fflush(stdout);
					}
				else
					{
					if (esc_last)
						esc_last = 0;	/* probably
						was only ESCape in vi */
					write(1, &ibuf[j], 1);
					}
				}  /* end of for */

/* *** OLD *** WE ALLOW FOR NEGOTIATION SO ALL BYTES MUST BE CHECKED 
			byteswritten = write(1, ibuf, ret); */

			update_title();
			}
		if (tmout > 0)
			tmout--;		/* reset */
		bzero(ibuf, sizeof ibuf);
		}
	if (tmout > 0)
		{
		printf("\nerror: tmout=%d descriptors UNread", 
			tmout);
		fflush(stdout);
		}
	goto selectloop;  /* END: select() loop */ 


/* ********** IN_COMMANDMODE: READ-WRITE LOOP (echo_noraw) *************** */
commandmode:
	echo_noraw();			/* restore line-edited I/O */
	signal(SIGINT, SIG_DFL);	/* restore CTRL-C */
	in_terminalmode = 0;
	in_commandmode = 1;		/* STATUS: at "s_tlnt> " prompt */
	printf("s_tlnt> ");
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
			CLOSE_SOCKET(sock);
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
			if (connect_port == 23)
				real_telnet = 1;
			else
				real_telnet = 0;
			sock = telnet_open(connect_host, connect_port);
			if (sock < 0)
				{			/* failure */
				printf("telnet_open: failed \n");
				}
			else				/* success */
				{
				goto newterminalmode;
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
				/* RESET: negotiation flags */
			neg_active = 0;		
			negotiated_ttype = 0;
			  ttytype[0] = '\0';
			negotiated_norcrd = 0;
			negotiated_sga = 0;
			negotiated_binary = 0;
			negotiated_echo = 0;
			negotiated_lflow = 0;

			CLOSE_SOCKET(sock);
			}
		else
			{
			printf("s_tlnt: no connection is open \n");
			}
		}
	else if (strncmp(answer, "ctrlc", SIGLETTERS) == 0)
		{
		if (connection_opened)
			{
			str[0] = 0x03;	/* CTRL-C */
			send(sock, str, 1, 0);
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
	else if (strncmp(answer, "verbose", SIGLETTERS) == 0)
		{
		verbose_flag = 1;
		}
	else if (strncmp(answer, "noverbose", SIGLETTERS) == 0)
		{
		verbose_flag = 0;
		}
	else if (strncmp(answer, "status", SIGLETTERS) == 0)
		{
		if (connection_opened)
			{
			if (real_telnet)
			    printf(
		"connection:  OPEN to %s port %d (telnet reserved port) \n",
				connect_host, connect_port);
			else
			    printf(
		"connection:  OPEN to %s port %d \n",
				connect_host, connect_port);
			}
		else
			printf("connection:  <none> \n");
		if (verbose_flag)
			printf("terminal:    VERBOSE flag is SET\n");
		else
			printf("terminal:    VERBOSE flag is NOT set\n");
		if (in_snoopymode)
			printf("terminal:    TRANSPARENT (snoopy) mode\n");
		else
			printf("terminal:    APPLICATION (non-snoopy) mode\n");

#if defined(_WIN95) || defined(_WINNT)
			printf("local OS:    Windows NT\n");
#else
			printf("local OS:    Unix\n");
#endif
		if (neg_active)
			{
			printf("negotiation: ACTIVE \n");
			if (negotiated_ttype == 1)
				printf("             WILL/DO %s %s \n", 
					TELOPT(TELOPT_TTYPE), ttytype);
			else if (negotiated_ttype == -1)
				printf("             WONT/DONT %s \n", 
					TELOPT(TELOPT_TTYPE));
			if (negotiated_sga == 1)
				printf("             WILL/DO %s \n", 
					TELOPT(TELOPT_SGA));
			else if (negotiated_sga == -1)
				printf("             WONT/DONT %s \n", 
					TELOPT(TELOPT_SGA));
			if (negotiated_echo == 1)
				printf("             WILL/DO %s \n", 
					TELOPT(TELOPT_ECHO));
			else if (negotiated_echo == -1)
				printf("             WONT/DONT %s \n", 
					TELOPT(TELOPT_ECHO));
			if (negotiated_binary == 1)
				printf("             WILL/DO %s \n", 
					TELOPT(TELOPT_BINARY));
			else if (negotiated_binary == -1)
				printf("             WONT/DONT %s \n", 
					TELOPT(TELOPT_BINARY));
			if (negotiated_lflow == 1)
				printf("             WILL/DO %s \n", 
					TELOPT(TELOPT_LFLOW));
			else if (negotiated_lflow == -1)
				printf("             WONT/DONT %s \n", 
					TELOPT(TELOPT_LFLOW));
			}
		}
	else		/* help */
		{
	printf("s_tlnt> commands: \n");
	printf("  close       _ close existing connection \n");
	printf("  ctrlc       _ send CTRL-C over connection \n");
	printf("  exit        _ close any connection and exit \n");
	printf("  help        - print this command summary \n");
	printf("  open [hostname] [portnumber] - open connection \n");
	printf("  quit        - close any connection and exit \n");
	printf("  return      - return to terminal mode \n");
	printf("  snoopy      - set transparent terminal mode \n");
	printf("  nosnoopy    - reset to line edited terminal mode \n");
	printf("  status      - show current telnet status \n");
	printf("  verbose     - set verbose flag \n");
	printf("  noverbose   - reset verbose flag \n");
	printf("terminal mode control sequences: \n");
	printf("  CTRL-T      - escape to command mode \n");
	printf("  CTRL-^      - kill (remote) shell, re-init socket getty \n");
	printf("  CTRL-K      - kill (remote) socket getty \n");
		fflush(stdout);
		}
	goto commandmode;

	}  /* end of main */

/* ----------------------------------------------------------------------
Utilities:
		int telnet_arb(char buf[], int *iptr, int max, SOCKET wsock)
		int telnet_open(char host_name[], int host_port)
		int getcmd(void)
		int update_title(void)
		int process_esc(char msg[])
   ---------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	telnet_arb(buf, iptr, max, wsock)
	char	buf[];
	int	*iptr, max;
	SOCKET	wsock;
#else
int	telnet_arb(char buf[], int *iptr, int max, SOCKET wsock)
#endif
				/* This function maintains the state
		of  typical telnet arbitration sequences such as
			IAC,DO,TERMINAL TYPE
			IAC,DO, -
		and answers (on wsock) with
			IAC,WILL,TERMINAL TYPE
			IAC,SB,TERMINAL TYPE,IAC,SE 
			IAC,WONT, -
		etc... As it reads characters, it increments the index
		*iptr but NEVER beyond (max-1).	It normally
		returns 0 unless syntax is incorrect, in which case
		it returns a number < 0. This function DISPLAYS
		what it is doing if (in_snoopymode || verbose_flag) */
	{
	int	telnet_cmd, telnet_opt, len, show;
	char	str[80], reply[80], *cptr;

	if ( (in_snoopymode) || (verbose_flag) ) show = 1;
		else show = 0;

/* *** DIAGNOSTIC ***
	sprintf(str, "\r\nStarting telnet arbitration..\r\n");
	if (show) write(1, str, strlen(str)); */

again:
	while ((*iptr) < max)
	  {
	  switch(arb_state) {
	  case 0:		/* expecting: IAC */
			if ((buf[*iptr] & 0x00ff) == IAC)
				{
				/* note telnet negotiation ACTIVE */
				neg_active = 1;
				arb_state = 1;
				(*iptr)++;
				goto again;
				}
			else
				{
				return(-1); /* let caller hanndle */
				}
	  case 1:	/* have IAC, expecting: 	DO,DONT,
							WILL,WONT,
							SB, ... */
		if (!TELCMD_OK(buf[*iptr] & 0x00ff))
			{
			arb_state = 0;	/* reset arb_state */
			return(-2);	/* let caller handle */
			}
		else 
			{
					/* record TELNET command */
			telnet_cmd = buf[*iptr] & 0x00ff;
			if ( (telnet_cmd == DO) ||
			     (telnet_cmd == DONT) )
				{
				arb_state = 2;
				(*iptr)++;
				goto again;
				}
			else if ( (telnet_cmd == WILL) ||
				  (telnet_cmd == WONT) )
				{
				arb_state = 3;
				(*iptr)++;
				goto again;
				}
			else if (telnet_cmd == SB)
				{
				arb_state = 4;
				(*iptr)++;
				goto again;
				}
			else
				{
			/* *** FOR NOW IGNORE *** */
				sprintf(str, " [them:IAC, ");
				if (show) write(1, str, strlen(str));
				arb_state = 0;
				return(-1);
				}
			}
	  case 2:		/* have IAC,DO/DONT, expecting: TELOPT */
				/* record TELNET option */
			telnet_opt = buf[*iptr] & 0x00ff;

		  	sprintf(str, " [them:IAC,%s,%s] \r\n", 
				TELCMD(telnet_cmd), 
				TELOPT(telnet_opt));
		  	if (show) write(1, str, strlen(str));
			if (telnet_cmd == DO)
			  switch(telnet_opt)  {
			  case	TELOPT_TTYPE:
				reply[0] = (char)IAC;
				reply[1] = (char)WILL;
				reply[2] = (char)TELOPT_TTYPE;
				reply[3] = (char)IAC;
				reply[4] = (char)SB;
				reply[5] = (char)TELOPT_TTYPE;
				if ((cptr = getenv("TERM")) != NULL)
					{
					negotiated_ttype = 1;
					strcpy(ttytype, cptr);
					}
				else
					{
					ttytype[0] = '\0';
					}
				len = strlen(ttytype);
				strncpy(&reply[6], ttytype, len);
				reply[6 + len] = (char)IAC;
				reply[7 + len] = (char)SE;
				reply[8 + len] = '\0';
				send(wsock, reply, 8 + len, 0);
				sprintf(str, 
			" [us:IAC,WILL,%s] [IAC,SB,%s] %s [IAC,SE] \r\n",
					TELOPT(telnet_opt), 
					TELOPT(telnet_opt), ttytype);
				if (show) write(1, str, strlen(str));
				break;

			  case	TELOPT_SGA:
				reply[0] = (char)IAC; 
				reply[1] = (char)WILL;	/* ack */
				reply[2] = (char)TELOPT_SGA;
				negotiated_sga = 1;
				send(wsock, reply, 3, 0);
				sprintf(str, " [us:IAC,WILL,%s] \r\n",
					TELOPT(telnet_opt));
				if (show) write(1, str, strlen(str));
				break;

			  default:	/* say "WONT" */
				reply[0] = (char)IAC;
				reply[1] = (char)WONT;	/* nack */
				reply[2] = telnet_opt;
				if (telnet_opt == TELOPT_ECHO)
					negotiated_echo = -1;
				else if (telnet_opt == TELOPT_BINARY)
					negotiated_binary = -1;
				else if (telnet_opt == TELOPT_LFLOW)
					negotiated_lflow = -1;
				send(wsock, reply, 3, 0);
				sprintf(str, " [us:IAC,WONT,%s] \r\n",
					TELOPT(telnet_opt));
				if (show) write(1, str, strlen(str));
				break;
			  }  /* end of switch(telnet_opt) - on DO */
			else if (telnet_cmd == DONT)
			  switch(telnet_opt) {

			  default:	/* say "WONT" if NOT already
				   set to -1 (avoid INFINITE LOOPS) */
				if (telnet_opt == TELOPT_ECHO) 
				  {
				  if (negotiated_echo == -1)  
					break;
				  else
					negotiated_echo = -1;
				  }
				if (telnet_opt == TELOPT_BINARY)
				  {
				  if (negotiated_binary == -1) 
					break;
				  else
					negotiated_binary = -1;
				  }
				if (telnet_opt == TELOPT_LFLOW) 
				  {
				  if (negotiated_lflow == -1) 
					break;
				  else
					negotiated_lflow = -1;
				  }
				reply[0] = (char)IAC;
				reply[1] = (char)WONT;	/* nack */
				reply[2] = telnet_opt;
				if (telnet_opt == TELOPT_ECHO)
					negotiated_echo = -1;
				else if (telnet_opt == TELOPT_BINARY)
					negotiated_binary = -1;
				else if (telnet_opt == TELOPT_LFLOW)
					negotiated_lflow = -1;
				send(wsock, reply, 3, 0);
				sprintf(str, " [us:IAC,WONT,%s] \r\n",
					TELOPT(telnet_opt));
				if (show) write(1, str, strlen(str));
				break;
			  }  /* end of switch(telnet_opt) - on DONT */
			
			arb_state = 0;
			return(0);	/* OK, done */

	  case 3:		/* have IAC,WILL/WONT, expecting: TELOPT */
				/* record TELNET option */
			telnet_opt = buf[*iptr] & 0x00ff;

		  	sprintf(str, " [them:IAC,%s,%s] \r\n", 
				TELCMD(telnet_cmd), 
				TELOPT(telnet_opt));
		  	if (show) write(1, str, strlen(str));
			if (telnet_cmd == WILL)
			  switch(telnet_opt)  {
			  case	TELOPT_SGA:
				reply[0] = (char)IAC; 
				reply[1] = (char)DO;		/* ack */
				reply[2] = (char)TELOPT_SGA;
				negotiated_sga = 1;
				send(wsock, reply, 3, 0);
				sprintf(str, " [us:IAC,DO,%s] \r\n",
					TELOPT(telnet_opt));
				if (show) write(1, str, strlen(str));
				break;

			  default:	;	 /* nothing for NOW */
			  }  /* endo of switch(telnet_opt) - on WILL */

			arb_state = 0;
			return(0);	/* OK, done */

	  case 4:		/* have IAC,SB, expecting: TELOPT ... */
				/* record TELNET option */
			telnet_opt = buf[*iptr] & 0x00ff;
		  	sprintf(str, " [them:IAC,%s,%s] ", 
				TELCMD(telnet_cmd), 
				TELOPT(telnet_opt));
		  	if (show) write(1, str, strlen(str));
			(*iptr)++;
			arb_state = 5;	/* string ending with ... IAC SE */
			goto again;

	  case 5:		/* expecting: ... IAC (then SE) */
		if ( (buf[*iptr] & 0x00ff) != IAC)
			{
			sprintf(str, "%2.2X ", (buf[*iptr] & 0x00ff)); 
			if (show) write(1, str, strlen(str));
			(*iptr)++;
			goto again;
			}  /* end of while */
		sprintf(str, " [IAC,");
		if (show) write(1, str, strlen(str));
		arb_state = 6;	/* ending SE */
		(*iptr)++;
		goto again;

	  case 6:		/* have IAC ... expecting: SE */
		if ( (buf[*iptr] & 0x00ff) == SE)
			{
			sprintf(str, "SE] \r\n");
			if (show) write(1, str, strlen(str));
			arb_state = 0;	/* done */
			return(0);	/* OK, done */
			}
		else		/* ERROR */
			{
			arb_state = 0;	/* reset arb_state */
			return(-6);	/* let caller handle */
			}

	    default:	arb_state = 0;
			return(-9);	/* let caller handle */
	    }  /* end of switch(arb_state) */
	  }  /* end of while((*iptr) < max) */

	fflush(stdout);
	return(0);		/* buf[] is empty, so far, so good */
	}  /* end of telnet_arb */

/* ---------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	telnet_open(host_name, host_port)		
	char	host_name[];
	int	host_port;
#else
int	telnet_open(char host_name[], int host_port)
#endif
						/* This function
		accepts a host_name (e.g. "sparc1") and a host_port
		(e.g. 1492) and attempts to make a connection to the
		desired socket (there must, of course, be a processs
		listening at the target). If it is successful, it
		fills in the global struct sockaddr_in name, and
		returns the descriptor of a socket which can be used
		for recv()'s and send()'s. If not, it prints out an
		error message and returns -1 (if gethostbyname() failed)
		and -2 (if socket creation failed) or -3 (if connect()
		failed). */
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
	name.sin_port = htons((short)host_port);/* convert atoi(string)
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
		CLOSE_SOCKET(sock);
		return(-3);
		}
	return(sock);			/* SUCCESS */
	}  /* end of telnet_open */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	getcmd()   
#else
int	getcmd(void)
#endif

		/* this is a crude parser, spaces and tabs are delimeters
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

/* *** OLD *** gets() is dangerous
	gets(cmdbuf);  even though it replaces '\n' by '\0' */

	fgets(cmdbuf, sizeof(cmdbuf) - 1, stdin);
	for (i = 0 ; i < sizeof(cmdbuf) - 1 ; i++)
		{
		if ( (cmdbuf[i] == '\n') || (cmdbuf[i] == '\0') )
			break;
		}
	if (cmdbuf[i] == '\n')
		cmdbuf[i] = '\0'; 	/* get rid of newline */

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

/* -------------------------------------------------------------------- */

#if defined(_WIN95) || defined(_WINNT)

void __cdecl __my_initconout(void)

			/* This is my MODIFIED initialization for
		console output. I have changed "GENERIC_WRITE" to
		"GENERIC_WRITE | GENERIC_READ" so that the kernel
		call GetConsoleScreenBufferInfo() will NOT fail.
		As to what I think of a console interface such as 
		this one I will not reply but simply quote Winston
		Churchill.

		Interviewer: "Do you have contempt for Nazism?"

		Churchill: "Oh no, Nazism is beneath contempt."   */
	{
        _confh = (int)CreateFile( "CONOUT$",
                                  GENERIC_WRITE | GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL,
                                  OPEN_EXISTING,
                                  0,
                                  NULL );
	}
#endif

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int update_title()
#else
int update_title(void)
#endif
			/* This function currently does nothing in
		Unix. In Windows NT it

		1. checks if _confh (CONOUT$) and _coninpfh (CONIN$)
		   need to be initialized,
		2. updates the title, showing window size and current
		   cursor position. 

		It returns -1 if there were any problems. */
	{
#if defined(_WIN95) || defined(_WINNT)

	int	ret, num_written;
	char	str[128];


	if (_confh == -2) 
		{
		__my_initconout(); 	/* my modification of _initconout */
		if (_confh == -1) 
			{ 
			printf( 
			"\r\n _confh    still *** invalid *** \r\n"); 
			fflush(stdout); 
			return(-1);
			} 
		}
	if (_coninpfh == -2)
		{
		__initconin(); 		/* this is UNmodified */
		if (_coninpfh == -1) 
			{ 
			printf( 
			"\r\n _coninpfh still *** invalid *** \r\n"); 
			fflush(stdout); 
			return(-1);
			} 
		}
	ret = GetConsoleScreenBufferInfo((HANDLE)_confh,
		(PCONSOLE_SCREEN_BUFFER_INFO) &screen_bufinfo);
	if (ret == 0)
		{
		sprintf(str,
			"\r\n kernel call *** failed *** %ld \r\n",
			GetLastError());
		WriteConsole((HANDLE)_confh,
			(LPVOID)str,
			strlen(str),
			&num_written, NULL);
		return(-1);
		}
		/* update globals for process_esc() */
	last_position.X = screen_bufinfo.dwCursorPosition.X;
	last_position.Y = screen_bufinfo.dwCursorPosition.Y;
	screen_win.Left = screen_bufinfo.srWindow.Left;
	screen_win.Top = screen_bufinfo.srWindow.Top;
	screen_win.Right = screen_bufinfo.srWindow.Right;
	screen_win.Bottom = screen_bufinfo.srWindow.Bottom;
		/* change title */
	if (in_snoopymode)
	    {
	    sprintf(cmd_title, 
	  "Program s_tlnt: (%d,%d)x(%d,%d) (in SNOOPY mode)",
		(int)screen_bufinfo.srWindow.Left,
		(int)screen_bufinfo.srWindow.Top,
		(int)screen_bufinfo.srWindow.Right,
		(int)screen_bufinfo.srWindow.Bottom);
	    }
	else
	    {
	    sprintf(cmd_title, 
	  "Program s_tlnt: (%d,%d)x(%d,%d) (row=%2d,col=%2d)",
		(int)screen_bufinfo.srWindow.Left,
		(int)screen_bufinfo.srWindow.Top,
		(int)screen_bufinfo.srWindow.Right,
		(int)screen_bufinfo.srWindow.Bottom,
		(int)screen_bufinfo.dwCursorPosition.Y,
		(int)screen_bufinfo.dwCursorPosition.X);
	    }

	ret = SetConsoleTitle((LPCTSTR) cmd_title);
#else

#endif
	return(0);
	}  /* end of update_title() */

/* -------------------------------------------------------------------- */

#define	MAX_VT100_PARAMS	8

#if defined(_WIN95) || defined(_WINNT)

			/* next should have been done by update_title() */
#define	CHECK__CONFH \
	   	if (_confh == -2) __my_initconout(); \
		if (_confh == -1) \
			{ printf( \
		"\r\n _confh still *** invalid *** \r\n"); \
			fflush(stdout); \
			goto finish; } 

SMALL_RECT	temp_win;
COORD		one_line = { 0, 1 } ;
CHAR_INFO	blank_space = { ' ', 0x07 };
#define SCROLL_UP(x) \
		one_line.Y = (x); \
		temp_win.Left = screen_win.Left;    \
		temp_win.Top = screen_win.Top;      \
		temp_win.Right = screen_win.Right;  \
		temp_win.Bottom = screen_win.Bottom - (x); \
		if (ScrollConsoleScreenBuffer((HANDLE)_confh, \
			&temp_win, NULL, \
			one_line, &blank_space) == 0) \
			{ printf( \
		"\r\n scroll up %d lines *** failed *** \r\n", (x)); \
			fflush(stdout);} 

#define SCROLL_DOWN(x) \
		one_line.Y = 0; \
		temp_win.Left = screen_win.Left;    \
		temp_win.Top = screen_win.Top + (x);      \
		temp_win.Right = screen_win.Right;  \
		temp_win.Bottom = screen_win.Bottom; \
		if (ScrollConsoleScreenBuffer((HANDLE)_confh, \
			&temp_win, NULL, \
			one_line, &blank_space) == 0) \
			{ printf( \
		"\r\n scroll down %d lines *** failed *** \r\n", (x)); \
			fflush(stdout);} 
#endif


#if defined(_NOPROTO)
int	process_esc(msg)   
	char	msg[];
#else
int	process_esc(char msg[])
#endif
			/* This function now handles the 
		*** COMPLICATIONS *** caused by WinNT's 32-bit 
		Dos Command Processor (cmd.exe, which is called a
		"Console" in kernel32 language) NOT handling
		ANSI/VT100 ESCape sequences. In the case of Unix
		the ESCape sequence is passed to stdout. In the
		case of Windows NT we try to handle it with the
		conio.h/graph.h calls: if successful we return 0, 
		otherwise we return 1.

	CR			GOTO BOL
	LF			LINEFEED NO CR

	ESC c			FULL RESET
	ESC 7			SAVE CURSOR
	ESC 8			RESTORE CURSOR
	ESC A			CURSOR UP		(also VT52)
	ESC B			CURSOR DOWN		(also VT52)
	ESC C			CURSOR RIGHT		(also VT52)
	ESC D			CURSOR LEFT		(also VT52)
	ESC H			CURSOR HOME		(also VT52)
	ESC I			REVERSE LINE FEED	(also VT52)
	ESC J			CLEAR TO EOS		(also VT52)
	ESC K			CLEAR TO EOL		(also VT52)
	ESC M			REVERSE INDEX
	ESC [ lines  A		UP
	ESC [ lines  B		DOWN
	ESC [ lines  C 		RIGHT
	ESC [ lines  D 		LEFT
	ESC [ H			HOME
	ESC [ coords H		HOME CURSOR
	ESC [ J			CLEAR TO EOS
	ESC [ number J		CLEAR TO EOS
	ESC [ K			CLEAR TO EOL
	ESC [ number K		CLEAR TO EOL
	ESC [ attrib m		COLOR ATTRIBUTES
	ESC [ margin r		MARGINS 

	ESC [ 6n		POSITION ENQUIRY (ans: ESC [ row ; col R )
			 */
	{
#if defined(_WIN95) || defined(_WINNT)
	int	i, j, k, ret, dindex, digit[ MAX_VT100_PARAMS ];
	char	tmpdigit[64];

	char	report[64];
	COORD	position;
	ULONG	oldstate, newstate, num_written;

				/* check LF first */
	if (msg[0] == '\n')
		{
		msg[0] = 'B';	/* replace by cursor DOWN and 
				service below */	
		}
	else if (msg[0] == '\r')
		{
	 	fflush(stdout);	/* go to Beginning of Line */
		CHECK__CONFH
		last_position.X = 0;
		SetConsoleCursorPosition((HANDLE)_confh, last_position);
		return(0);
		}

	if ( (msg[0] != '[') &&
	     (msg[0] != 'O') &&
	     (msg[0] != '7') &&
	     (msg[0] != '8') &&
	     (msg[0] != 'A') && (msg[0] != 'B') &&
	     (msg[0] != 'C') && (msg[0] != 'D') &&
	     (msg[0] != 'H') && (msg[0] != 'I') &&
	     (msg[0] != 'J') && (msg[0] != 'K') &&
	     (msg[0] != 'M') &&
	     (msg[0] != 'c') )	/* insurance */
		return(1);

	if ( (msg[0] == 'c') ||
	     (msg[0] == 'C') || 
	     (msg[0] == 'H') || (msg[0] == 'I') ||
	     (msg[0] == 'J') || (msg[0] == 'K') )
		{
		return(1);	/* all *** TODO *** */
		}
	if (msg[0] == '7') 	/* save cursor position */
		{
	 	fflush(stdout);
		CHECK__CONFH
		save_position.X = last_position.X;
		save_position.Y = last_position.Y;
		return(0);
		}
	if (msg[0] == '8') 	/* restore cursor position */
		{
	 	fflush(stdout);
		CHECK__CONFH
		last_position.X = save_position.X;
		last_position.Y = save_position.Y;
		SetConsoleCursorPosition((HANDLE)_confh, last_position);
		return(0);
		}
	if (msg[0] == 'M')	/* scroll UP */
		{
	 	fflush(stdout);
		CHECK__CONFH
		SCROLL_UP(1)
		return(0);
		}
	if (msg[0] == 'A')	/* cursor UP */
		{
	 	fflush(stdout);
		CHECK__CONFH
		if (last_position.Y > screen_win.Top) 
			{
			position.Y = last_position.Y - 1;
			}
		else 
			{
			SCROLL_UP(1)
			position.Y = screen_win.Top;
			}
		position.X = last_position.X;
		SetConsoleCursorPosition((HANDLE)_confh, position);
				/* update */
		last_position.X = position.X;
		last_position.Y = position.Y;
		return(0);
		}
	if ( (msg[0] == 'B') ||
		(msg[0] == 'D')	)	/* cursor DOWN */
		{
	 	fflush(stdout);
		CHECK__CONFH
		if (last_position.Y < screen_win.Bottom) 
			{
			position.Y = last_position.Y + 1;
			}
		else 
			{
			SCROLL_DOWN(1)
			position.Y = screen_win.Bottom;
			}
		position.X = last_position.X;
		SetConsoleCursorPosition((HANDLE)_confh, position);
				/* update */
		last_position.X = position.X;
		last_position.Y = position.Y;
		return(0);
		}
	i = 1;			/* initialize */
	j = 0;
	ret = 1;		/* assume NOT successful */
				/* default for implicit vt100
			numeric parameter is zero */
	for (dindex = 0 ; dindex < MAX_VT100_PARAMS ; dindex++)
		digit[dindex] = 0;
	dindex = 0;
numericparam:
	switch(msg[i])  {
	  case	'0':	;
	  case	'1':	;
	  case	'2':	;
	  case	'3':	;
	  case	'4':	;
	  case	'5':	;
	  case	'6':	;
	  case	'7':	;
	  case	'8':	;
	  case	'9':	;
			tmpdigit[j] = msg[i];
			i++;
			j++;
			tmpdigit[j] = '\0';
			goto numericparam;

	  case  ';':	digit[ dindex ] = atoi(tmpdigit);
			i++;
			j = 0;
			if (dindex < (MAX_VT100_PARAMS - 1))
				{
				dindex++;
				goto numericparam;
				}
			goto finish;	/* TOO many parameters */

	  case	'?':	i++;
			goto numericparam;	/* skip and expect
					following digit */

		/* the following characters are *** TERMINAL *** */

	  case  'l':	fflush(stdout);	/* these are MODE calls */
			if (j > 0)
				digit[0] = atoi(tmpdigit);
			else
				digit[0] = 0;
			if (digit[0] == 1)
				ret = 0;	/* set cursor key mode
					but we can really do nothing */
			goto finish;

	  case	'r':	if (dindex > 0)		/* margins: record
					but we can really do nothing if
					margins are set LESS than
					fullscreen */
				{
	  			digit[ dindex ] = atoi(tmpdigit);
				dindex++;
				}
			last_margin_top = (SHORT)digit[0];
			last_margin_bot = (SHORT)digit[1];
			if ( (digit[0] != 0) && 
			  (last_margin_top != (int)(screen_win.Top + 1)) )
				{
				printf(
				  "\r\n top margin *** invalid *** \r\n");
				fflush(stdout);
				}
			else if ( (digit[1] != 0) && 
			  (last_margin_bot != (int)(screen_win.Bottom + 1)) )
				{
				printf(
				  "\r\n bottom margin *** invalid *** \r\n");
				fflush(stdout);
				}
			else
				{
				ret = 0;
				}
			goto finish;

	  case	'n': 	if (j > 0)
				digit[0] = atoi(tmpdigit);
			else
				digit[0] = 0;
			if (digit[0] != 6)	/* enquiry: position */
				goto finish;
			report[0] = 033;	/* ESCape */
			sprintf(&report[1],
				"[%d;%dR\r\n", 
				(int)last_position.Y + 1,
				(int)last_position.X + 1);
			send(sock, report, strlen(report), 0);
			ret = 0;
			goto finish;

	  case	'A':	fflush(stdout);
			CHECK__CONFH
			if (j > 0)
				digit[0] = atoi(tmpdigit);
			else
				digit[0] = 1;
			if (last_position.Y >= digit[0] + screen_win.Top) 
				{
				position.Y = last_position.Y - digit[0];
				}
			else 
				{
				SCROLL_UP(digit[0])
				position.Y = screen_win.Top;
				}
			position.X = last_position.X;
			SetConsoleCursorPosition((HANDLE)_confh, position);
					/* update */
			last_position.X = position.X;
			last_position.Y = position.Y;
			ret = 0;
			goto finish;

	  case  'H':    if (dindex > 0)
				{
	  			digit[ dindex ] = atoi(tmpdigit);
				dindex++;
				}
				/* vt100 has REVERSED coords which
				START at 1 not zero */
			position.X = (SHORT)digit[1] - 1;
			position.Y = (SHORT)digit[0] - 1;
				/* stay inside screen window */
			if (position.X > screen_win.Right)
				position.X = screen_win.Right;
			else if (position.X < screen_win.Left)
				position.X = screen_win.Left;
			if (position.Y > screen_win.Bottom)
				position.Y = screen_win.Bottom;
			else if (position.Y < screen_win.Top)
				position.Y = screen_win.Top;
			CHECK__CONFH
			SetConsoleCursorPosition((HANDLE)_confh, 
				position);
				/* update */
			last_position.X = position.X;
			last_position.Y = position.Y;
			ret = 0;
			goto finish;

	   case	'J':	fflush(stdout);
			CHECK__CONFH
			for (k = 0 ; k < 50 ; k++)
				WriteConsole((HANDLE)_confh,
					(LPVOID)blank_newline,
					strlen(blank_newline),
					&num_written, NULL);
			position.X = 0;
			position.Y = 0;
			SetConsoleCursorPosition((HANDLE)_confh, 
				position);
				/* update */
			last_position.X = position.X;
			last_position.Y = position.Y;
			ret = 0;

			goto finish;

	   case  'K':	fflush(stdout);
			CHECK__CONFH
			GetConsoleMode((HANDLE)_confh, &oldstate);
				/* turn OFF line wrap */
			SetConsoleMode((HANDLE)_confh, 
				ENABLE_PROCESSED_OUTPUT);
			WriteConsole((HANDLE)_confh,
				(LPVOID)blank_line,
				strlen(blank_line),
				&num_written, NULL);
				/* restore wrap */
			newstate = oldstate | 
				ENABLE_PROCESSED_OUTPUT |
				ENABLE_WRAP_AT_EOL_OUTPUT;
			SetConsoleMode((HANDLE)_confh, newstate);
				/* and restore position */
			SetConsoleCursorPosition((HANDLE)_confh, 
				last_position);
			ret = 0;
			goto finish;

	  default:	goto finish;

	  }  /* end of first switch */

finish:
	if (ret == 0)
		fflush(stdout);
	return(ret);		/* NOT processed */
#else

	if ( (msg[0] == '\n') || (msg[0] == '\r') )
		{
		putchar(msg[0]);
		fflush(stdout);
		return(0);
		}
	putchar(033);		/* ESCape */
	printf("%s", msg);
	fflush(stdout);
	return(0);
#endif
	}  /* end of process_esc */

/* --------------------------------------------------------------------
Low-Level Terminal Routines:
			disp_ctrlc()
			raw_noecho()
			raw_cbreak()
			fionbio_select()
			echo_noraw()
			noecho_noraw()
			restore_orig()
   -------------------------------------------------------------------- */

#if defined(_NOPROTO)
	disp_ctrlc()		
#else
void	disp_ctrlc(int dummy)		
#endif
			/* Rather than use signal(SIGINT, SIG_IGN), which
		will completely disable action taken after CTRL-C, this
		interrupt routine is useful in CBREAK mode (raw_cbreak())
		to simply display the string "^C " on stdout */
	{
	ctrlc_received = 1;		/* alert main() */
	write(1, "trapped: ^C ", 12);
	signal(SIGINT, disp_ctrlc);
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

#if defined(_NOPROTO)
int	raw_noecho()   
#else
int	raw_noecho(void)   
#endif
			/* This function sets full RAW mode for single
		character I/O, that is, getchar() waits for a
		single ascii character and returns when a key is
		pressed. Since ISIG is DISABLED, CTRL-C (SIGINT)
		will return 0x03 and CTRL-\ (SIGQUIT) will return
		0x1B=FS. Do NOT use this mode with select(). */
	{
#if defined(__bcc) || defined(_DJGCC)	/* *** Borland *** C++ under OS/2 */

#elif defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = RAW_NOECHO_MODE;	/* record current setting */

#else			/* *** Unix *** */
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	zero = 0;
				 	/* modify settings in tinfo */
	tinfo.c_iflag = (tsave.c_iflag & ~ICRNL) | IGNPAR |
		ISTRIP;  /* turn off   cr cr-lf  conversion, ignore
		parity errors, strip off 8th bit */
#if defined(__bsd)
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON | ISIG);  /* turn
		off echo and erase and kill functions */
#else
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR | OCRNL);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON | ISIG);  /* turn
		off echo and erase and kill functions */
#endif
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 1;    /* return from read after 1 character */
	tinfo.c_cc[VSUSP] = 0;	 /* disable CTRL-Z (SUSPEND) */

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
#if defined(__bsd)
	ret = tcsetattr(0, TCSAFLUSH, &tinfo);
#else
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
#endif
	current_mode = RAW_NOECHO_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of raw_noecho */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	raw_cbreak()   
#else
int	raw_cbreak(void)   
#endif
			/* This function sets UNIX CBREAK mode for single
		character I/O, that is, getchar() waits for a
		single ascii character and returns when a key is
		pushed. Since ISIG has NOT been changed, CTRL-C
		(SIGINT) and CTRL-\ (SIGQUIT) are still ACTIVE.
		Do NOT use this mode with select(). */
	{
#if defined(__bcc) || defined(_DJGCC)	/* *** Borland *** C++ under OS/2 */

#elif defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = CBREAK_MODE;	/* record current setting */

#else			/* *** Unix *** */
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	zero = 0;
				 	/* modify settings in tinfo */
	tinfo.c_iflag = (tsave.c_iflag & ~ICRNL) | IGNPAR |
		ISTRIP;  /* turn off   cr cr-lf  conversion, ignore
		parity errors, strip off 8th bit */
#if defined(__bsd)
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON);  /* turn
		off echo and erase functions */
#else
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR | OCRNL);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON);  /* turn off
		echo and erase functions */
#endif
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 1;    /* return from read after 1 character */
	tinfo.c_cc[VSUSP] = 0;	 /* disable CTRL-Z (SUSPEND) */

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
#if defined(__bsd)
	ret = tcsetattr(0, TCSAFLUSH, &tinfo);
#else
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
#endif
	current_mode = CBREAK_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of raw_cbreak */

/* ------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	fionbio_select()	
#else
int	fionbio_select(void)	
#endif
			/* This function is adapted for use with 
		non-blocking I/O (FIONBIOF) and select()
		(so VMIN=VTIME=0) and the rest is like cooked mode
		EXCEPT ECHO and ICANON are OFF. */
	{
#if defined(__bcc) || defined(_DJGCC)	/* *** Borland *** C++ under OS/2 */

#elif defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = SELECT_MODE;	/* record current setting */

#else			/* *** Unix *** */
	int	ret, one;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	one = 1;
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
	
	ioctl(0, FIONBIO, &one);	/* set non-blocking I/O */
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
	current_mode = SELECT_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of fionbio_select */

/* ------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	echo_noraw()   
#else
int	echo_noraw(void)   
#endif
			/* This function sets the normal, default, UNIX
		COOKED mode (also called line-edited mode) */
	{
#if defined(__bcc) || defined(_DJGCC)	/* *** Borland *** C++ under OS/2 */

#elif defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = ECHO_NORAW_MODE;	/* record current setting */

#else			/* *** Unix *** */
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	zero = 0;
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

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
	current_mode = ECHO_NORAW_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of echo_noraw */

/* ------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	noecho_noraw()   
#else
int	noecho_noraw(void)   
#endif
			/* This function is EXACTLY like echo_noraw()
		except that ECHO is OFF */
	{
#if defined(__bcc) || defined(_DJGCC)	/* *** Borland *** C++ under OS/2 */

#elif defined(_WIN95) || defined(_WINNT)

#else			/* *** Unix *** */
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	zero = 0;
				 	/* modify settings in tinfo */
	tinfo.c_iflag = tsave.c_iflag | ICRNL | IGNPAR |
		ISTRIP;  /* turn on   cr cr-lf  conversion, still ignore
		parity errors, still strip off 8th bit */
	tinfo.c_oflag = tsave.c_oflag | ONLCR;  /* turn on lf cr-lf
		conversion */
	tinfo.c_lflag = (tsave.c_lflag | ICANON | ISIG) & (~ECHO);  
		/* turn off echo, turn on erase and kill functions */
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 4;    /* return from read after 4 characters */
	tinfo.c_cc[VSUSP] = 0x1A; /* enable CTRL-Z (SUSPEND) */

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
	current_mode = ECHO_NORAW_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of noecho_noraw */

/* ------------------------------------------------------------------- */

#if defined(_NOPROTO)
int	restore_orig()	
#else
int	restore_orig(void)	
#endif
			/* This function RESTORES the ORIGINALLY
		saved settings (if, indeed, they were saved) and
		closes the "kb_emul.log" file. */
	{
#if defined(__bcc) || defined(_DJGCC)	/* *** Borland *** C++ under OS/2 */

#elif defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = ECHO_NORAW_MODE;	/* actually an assumption */

#else			/* *** Unix *** */
	int	ret;

	if (termsave_done) 
		{
		ret = tcsetattr(STDIN_FILENO, TCSANOW, &toriginal);
		}
	current_mode = ECHO_NORAW_MODE;	/* actually an assumption */
#endif
	return(0);
	}  /* end of restore_orig */

/* ------------------------------------------------------------------- */

