				/* this program opens a datagram
			socket in the Internet domain, binds a name
			to it and shows the portnumber of the name.
			It receives one datagram message, reading from the
			socket and terminates. It expects to be invoked via:

			$ dgrmrec &

			running in the background and waiting for
			a message from dgrmsend. */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

/* note: in.h contains the definition
struct	sockaddr_in {
	short	sin_family;
	u_short	sin_port;
	struct	in_addr	sin_addr;
	char	sin_zero[8]; }				*/

int main(argc,argv,envp)
int	argc;
char	*argv[],*envp[];
	{
	int	sock;
	struct	sockaddr_in	name;
	int	length, ret, d1, d2, d3, d4;
	char	buf[2048];
	unsigned long	ul;

/* CREATE SOCKET (i.e. create TSAP of type INTERNET DATAGRAM and of
			protocol 0 = IP protocol) */
	sock = socket(AF_INET,SOCK_DGRAM,0);
	if (sock < 0)
		{
		perror("receiver: socket() failed. ");
		exit(-1);
		}

/* BIND NAME TO SOCKET (use wildcards and get port number) */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;	/* assigned by system */
	name.sin_port = 0;			/* assigned by system */
	ret = bind(sock,(struct sockaddr *)&name,sizeof name);
	if (ret < 0)
		{
		perror("receiver: bind() failed. ");
		exit(-1);
		}

/* USE UTILITES getsockname, ntohs TO PRINT OUT NAME */
	length = sizeof(name);
	ret = getsockname(sock,(struct sockaddr *)&name,&length);
	if (ret < 0)
		{
		perror("receiver: getsockname() failed. ");
		exit(-1);
		}
	sleep(1);	/* pause for clean screen display */
	printf("\nreceiver: process id: %d \n", (int)getpid());
	  ul = name.sin_addr.s_addr;	/* unsigned long */
	  d1 = 0xff & ul;	d2 = (0xff00 & ul) >> 8;
	  d3 = (0xff0000 & ul) >> 16;	d4 = (0xff000000 & ul) >> 24;
	printf("receiver: raw name.sin_addr: %ld \n",ul);
	printf("receiver: IP address %d.%d.%d.%d \n",d4,d3,d2,d1);
	printf("receiver: raw name.sin_port: %d \n",name.sin_port);
	printf("receiver: socket portnumber: %d \n",ntohs(name.sin_port));
		/* Note that the only purpose of ntohs is to handle
		the order of byte problem for different machines */

/* READ ONE DATAGRAM FROM SOCKET AND CLOSE */
	ret = read(sock,buf,2048);
	if (ret < 0)
		{
		perror("receiver: read() failed. ");
		exit(-1);
		}
	else
		printf("        received datagram-->%s \n",buf);

	close(sock);		/* close socket */
	exit(0);

	}  /* end of main */
