				/* this program expects to be invoked
			via:

			$ dgramsend <hostname, i.e. "sparc1"> <portnumber>

			where the portnumber has been previously printed
			out by the background program dgrmrec. This program
			opens a socket which will be used to send to the
			socket whose portnumber was given in the command
			line. It then prompts the user to enter some
			text for a single datagram, which it then sends.
			The socket is then closed and the program exits. */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>		/* Internet domain */
#include <netdb.h>		/* for gethostbyname() */
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
	int	sock, ret;
	struct	sockaddr_in	name;
	struct	hostent	*hp, *gethostbyname();	/* forward ref */
	char	chrline[132];
	int	length, d1, d2, d3, d4;
	unsigned long	ul;

/* CHECK COMMAND LINE */
	if (argc < 3)
		{
		printf("error: invoke this program via: \n");
		printf("        dgrmsend <hostname> <portnumber> \n");
		exit(-1);
		}

/* GET FULL HOSTNAME */
	hp = gethostbyname(argv[1]);
	if (hp == 0)
		{
		perror("sender: gethostbyname() failed. ");
		printf("        are you sure hostname is correct? \n");
		exit(-1);
		}
	bcopy((char *)hp->h_addr,(char *)&name.sin_addr,hp->h_length);
	  ul = name.sin_addr.s_addr;	/* unsigned long */
	  d1 = 0xff & ul;	d2 = (0xff00 & ul) >> 8;
	  d3 = (0xff0000 & ul) >> 16;	d4 = (0xff000000 & ul) >> 24;
	printf("sender: raw name.sin_addr: %ld \n",ul);
	printf("sender: IP address: %d.%d.%d.%d \n",d4,d3,d2,d1);
	name.sin_family = AF_INET;
	name.sin_port = htons(atoi(argv[2]));	/* convert string to
			integer to internal representation of integer
			with correct byte order. */
	printf("sender: raw name.sin_port: %d \n",name.sin_port);

/* CREATE SOCKET (i.e. create TSAP of type INTERNET DATAGRAM and
			of protocol 0 = IP protocol) */
	sock = socket(AF_INET,SOCK_DGRAM,0);
	if (sock < 0)
		{
		perror("sender: socket() failed. \n");
		exit(-1);
		}

/* SEND SHORT DATAGRAM */
	sleep(1);			/* pause */
	printf("enter datagram> ");
	gets(chrline);
	length = strlen(chrline) + 1;
	ret = sendto(sock,chrline,length,0,(struct sockaddr *)&name,
					sizeof name);
		/* actually no failure to deliver a datagram is
		implicit in sendto(), even if the portnumber is wrong. */
	if (ret < 0)
		{
		perror("sender: sendto() failed. \n");
		exit(-1);
		} 

/* CLOSE SOCKET and end program */
	close(sock);
	return(0);
	}  /* end of main */
