
				/* this program send a message to the
		program "vcrec" which is listening on some socket. It
		expects to be invoked via:

		$ vcsend <hostname, i.e. "ultrix6"> <portnumber>

		where the portnumber has been PREVIOUSLY printed out by 
		the process vcrec. Messages are sent as a stream of
		bytes (with NO preservation of boundaries) as lines 
		are typed by the user until a line is given which BEGINS 
		with ".". The socket is then closed, terminating the 
		connection. 

		Makefiles are provided as follows:

		makefile.unx		DecUnix (alpha) or Ultrix (VAX)
		makefile.vc5		Windows NT with Winsock2 DLL
		makefile.sun	 	SUNOS 4.1.x on SparcStation


	A very basic TCP connection proceeds as follows:

	client				server
	_____________________________   ________________________________

	socket()         calling sock	socket()             listen sock
	init sockaddr_in struct		init sockaddr_in struct
	  with REMOTE socket name	  with LOCAL socket name
					bind()
					getsockname() if we asked system
					  to assign tcp port number

					listen()          on listen sock
	connect() ------------------->
					accept()               data sock

	  (data exchange phase can be response-driven but if more
	   than one descriptor must be managed use of select() is
	   strongly recommended)

	send()  --------------------->	recv()
	recv() <--------------------- 	send()


	close()/closesocket()		close()/closesocket()   data sock
					close()/closesocket() listen sock


						Marc Thomas          */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

/* ------------------------------------------------------------------ */

#include "header.h"	/* OUR header which handles MOST of the
			WinSock/32-bit Unix/64-bit Unix Differences */
#include "startup.h"	/* globals for main module set by the
			code startup.c */
#include "diagnost.h"	/* OUR dump_xxxx() routines and other
			diagnostics */

/* ------------------------------------------------------------------ */


/* ------------------------------------------------------------------ */

#if defined(_NOPROTO)
int main(argc,argv,envp)
int	argc;	
char	*argv[],*envp[];	
#else
int main(int argc,char *argv[], char *envp[])
#endif
	{
	SOCKET	sock;
	struct	sockaddr_in	name;
	struct	hostent	*hp;
	char	chrline[80];
	int	i, ret, length;

#include "startup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix startup. Main modules
			ALSO require the inclusion of 
				header.h
				startup.h */

/* CHECK COMMAND LINE for hostname AND tcp portnumber *********** */
	if (argc < 3)
		{
		printf("error: invoke this program via: \n");
		printf("       vcsend <hostname> <tcp_portnumber> \n");
		return(-1);
		}

/* USE GETHOSTBYNAME to find our target's IP-address ************ */
	hp = gethostbyname(argv[1]);
	if (hp == NULL)
		{
		perror("sender: gethostbyname() failed. ");
		printf("        are you sure the hostname is correct? \n");
		return(-1);
		}
	printf("\nsender: our target host structure is:");
	dump_hostent(stdout, hp);

		/* build struct sockaddr_in for target socket */
	bcopy((char *)hp->h_addr,(char *)&name.sin_addr,hp->h_length);
	name.sin_family = AF_INET;
			/* convert string to
			integer to internal representation of integer
			with correct byte order. */
	name.sin_port = htons((short)atoi(argv[2]));	

	printf("\nsender: our target socket is:");
	dump_sockaddr(stdout, &name);
	printf("\n");
	fflush(stdout);

/* CREATE TCP (ie. STREAM) SOCKET ********************************* */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		{
		perror("sender: socket() failed. ");
		goto finish;
		}

/* CONNECT TO TARGET SOCKET using HOSTNAME and given TCP PORT ***** */

		/* Note the type cast used to cast 
			(struct sockaddr_in *)
		as
			(struct sockaddr *)			*/
	ret = connect(sock, (struct sockaddr *)&name, sizeof name);
	if (ret != 0)
		{
		perror("sender: connect() failed. ");
		fprintf(stderr, 
		"\n        are you sure that the portnumber is correct? \n");
		fflush(stderr);
		goto finish;
		}

/* WRITE (SEND) LOOP ********************************************** */
#if defined(_WIN95) || defined(_WINNT)

#else
	sleep(1);	/* pause for clean screen display */
#endif
	printf("\nwaiting for vcrec...");
	fflush(stdout);
			/* block on recv until we get a single
		character from vcrec (this is a crude handshake) */
	recv(sock, chrline, sizeof(chrline) - 1, 0);
	printf("ok");

	printf("\nStart entering stream of characters at prompt;");
	printf(" type .<cr> to stop program. \n");
loop:	
	printf("enter line> ");

/* *** OLD *** gets() is dangerous 
	gets(chrline);		even though it converts '\n' to '\0' */

	fgets(chrline, sizeof(chrline) - 1, stdin);
	for (i = 0 ; i < (sizeof(chrline) - 1) ; i++)
		{
		if ( (chrline[i] == '\n') || (chrline[i] == '\0') )
			break;
		}
	if (chrline[i] == '\n')
		chrline[i] = '\0'; 	/* get rid of newline */

	length = strlen(chrline);
	if (chrline[0] == '.')		/* end of stream */
		{
		printf("sender: termination requested \n");
		goto finish;
		}

	ret = send(sock, chrline, length + 1, 0);

	if (ret < 0)
		perror("sender: write() failed. ");
	else
		goto loop;

finish:

/* CLEAN UP and EXIT ********************************************** */

	CLOSE_SOCKET(sock);		/* close socket descriptor */

#include "cleanup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */

	return(0);
	}  /* end of main */

