					

					/* this program opens a virtual
		circuit (stream) socket in the Internet domain, binds a
		name to it, and shows the portnumber of the name. It 
		expects to be invoked via:

		$ vcrec [optional packet size] &

		and thus runs in the background, waiting for a message 
		from vcsend. It calls listen once and sets up a queue 
		for at most 5 sessions, although currently it closes the 
		socket and exits after the end of the first session (i.e. 
		from the time of attachment until a return of 0 on read).
		if no packet size is set it allows messages of of up to 
		2048 bytes, but try setting a packet size of, say, 5 bytes 
		for comparison.	if you need to kill it, type:

		$ ps

		to make sure that you have the correct pid, and then type:

		$ kill -9 <vcrec's pid>

		note that this does no harm since all sockets will
		be closed automatically when a process terminates. Makefiles
		are provided as follows:

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


						Marc Thomas           */

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
int main(argc, argv, envp)
int	argc;	
char	*argv[],*envp[];
#else
int main(int argc, char *argv[], char *envp[])
#endif
	{
	SOCKET	sock;
	struct	sockaddr_in	name, caller;
	SOCKET	msgsock;
	char	buf[2048];
	int	size, length, ret, k;

#include "startup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix startup. Main modules
			ALSO require the inclusion of 
				header.h
				startup.h */

/* CHECK COMMAND LINE for optional buffer size ******************** */
	if (argc > 1)
		{
		size = atoi(argv[1]);
		if (size < 1  ||  size > sizeof(buf) - 1) 
			size = sizeof(buf) - 1;
		}
	else
		{
		size = sizeof(buf) - 1;
		}


/* CREATE TCP (ie. STREAM) SOCKET ******************************** */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		{
		perror("receiver: socket() failed. ");
		return(-1);
		}

/* BIND NAME TO SOCKET using portnumber 0 = "host assigned" ******* */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;  
				/* INADDR_ANY = assigned by system */
	name.sin_port = htons(0);	/* 0 = assigned by system */
	ret = bind(sock,(struct sockaddr *)&name,sizeof name);
	if (ret < 0)
		{
		perror("receiver: bind() failed. ");
		return(-1);
		}

/* USE GETSOCKNAME  to find out which port was assigned (note that since
		we are listening, not connecting, the socket the host 
		field name.sin_addr will be 0) ******************** */
	length = sizeof name;
	ret = getsockname(sock, (struct sockaddr *)&name, &length);
	if (ret < 0)
		{
		perror("receiver: getsockname() failed. ");
		return(-1);
		}
#if defined(_WIN95) || defined(_WINNT)

#else
	sleep(1);	/* pause for clean screen display */
#endif
	printf("\nreceiver: own process id: %d ", (int)getpid());
	printf("\nreceiver: we are using the socket: ");
	dump_sockaddr(stdout, &name);
	printf("\n");
	fflush(stdout);

/* LISTEN and start ACCEPTING CONNECTIONS ************************* */

	listen(sock,5);		/* 5 = max. backlog allowed */
		k = sizeof caller;
		msgsock = accept(sock,(struct sockaddr *)&caller,(int *)&k);
			/* the function accept blocks until there is at least
			one connection in the queue of pending requests. it
			then extracts the first connection from the
			queue of pending requests and creates a new
			socket (msgsock) with the same properties as sock.
			the int msgsock is used as a desc. to read and write
			data. if null parameters were used here (i.e.
			(struct sockaddr *)0 and (int *)0) the address
			of the connecting entity would be ignored.  */
		if (msgsock == -1)
			{
			perror("receiver: accept() failed. ");
			}
		else
		  {
		  printf("        received-->valid connection from: ");
		  dump_sockaddr(stdout, &caller);
		  printf("\n");
		  fflush(stdout);

			/* let vcsend know we are ready by writing
			a single character (crude handshake) */
		  buf[0]= '0';		
		  send(msgsock, buf, 1, 0);
		  
		  do	{
			bzero(buf,sizeof buf);	/* zero buffer */

			ret = recv(msgsock, buf, size, 0);
			if (ret < 0)
				{
				perror("receiver: recv() failed. ");
				}
			if (ret == 0)
				{
		printf("        received-->sender has ended connection \n");
				}
			else
				{
				printf("        received-->%s \n",buf);
				}
			} while (ret != 0);
		 }

/* CLEAN UP and EXIT ********************************************** */

		CLOSE_SOCKET(msgsock);		/* close socket descriptor */
		printf("receiver: ending session also and exiting. \n");
		CLOSE_SOCKET(sock);


#include "cleanup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */

	return(0);
	}  /* end of main */
