
/*
 * Copyright (c) 1984 Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Sun Microsystems, Inc.
 *
 */

/* 
 * -----------------------------------------------------------------
 * I HAVE *** HEAVILY MODIFIED *** THIS PROGRAM SO THAT
 *
 *	1. the "-a" option which required the (SUN-specific) kvm-calls 
 *	   has been removed and the rest of the code will now compile 
 *	   on a variety of Unix platforms.
 *
 *	2. I have added a pinger() routine to do a ping in case the
 *	   arp entry is NOT found in the cache.
 *
 *	3. the "-m" option tabulates the result as ONE LINE only which
 *	   is in the (fully-qualified) format
 *	   
 *	   host.subdomain.edu A 136.168.123.456 00:05:00:12:34:56
 *
 *	4. I have added two command-line modifiers:
 *
 *		-v	verbose - show progress and packets on stderr
 *		-w	when pinging, wait for return frame(s) or
 *			timout before reading ARP cache
 *
 *	5. The program now compiles under BOTH Unix and Windows NT version
 *	   4.0 (with the Microsoft Visual C++ version 5.0 Compiler and
 *	   Winsock support). See makefile.unx/makefile.vc5  for build.
 *	   There is MUCH difference between Unix and Windows NT because
 *	   neither Winsock1.1 or Winsock2.0 provides an API to ARP
 *	   functions.
 *
 *	6. If you are running this program on Windows NT you will need
 *	   to have Administrator privileges. If you are running this
 *	   program under Unix you will have to ask root to:
 *
 *		6a. put digarp in /usr/local/bin (or equiv.)
 *		6b. chown root digarp		 (ownership root)
 *		6c. chmod u+s digarp             (setuid)
 *
 *	   otherwise you will get an error message when the program
 *	   tries to open a RAW SOCKET.
 *
 *						Marc Thomas
 * -----------------------------------------------------------------
 */


/* 
 * -----------------------------------------------------------------
 * digarp - get, set, and delete arp table entries
 * -----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DO_TIMING	1		/* time ICMP round-trip */

/* 
 * ---------------------------------------------------------------- */

#include "header.h"	/* OUR header which handles MOST of the
			WinSock/32-bit Unix/64-bit Unix Differences */
#include "startup.h"	/* globals for main module set by the
			code startup.c */
#include "diagnost.h"	/* OUR dump_xxxx() routines and other
			diagnostics */

/* -----------------------------------------------------------------
 */

#include <errno.h>

#if defined(_WINNT) || defined(_WIN95)

#define MAXTRIES 3
#else			/* Unix */

#define MAXTRIES 4
#endif

			/* Function Prototypes */
#if defined(_NOPROTO)

	usage();
	get();
int	set();
	delete();
int	file();
int	ether_aton();
	ether_print();
	pinger();
int	in_cksum();
int	parse_system_output();
int	strip_subdomain();

#else

void	usage(void);
void	get(char *host, char *addsub);
int	set(int argc, char **argv);
void	delete(char *host);
int	file(char *name);
int	ether_aton(char *a, u_char *n);
void	ether_print(u_char *cp);
void	pinger(int sock, char *hostname, struct sockaddr *pwhereto);
int	in_cksum(u_short *addr, int len);
int	parse_system_output(FILE *fp, char *ipaddress, char *ethaddress);
int	strip_subdomain(char *hostnm);

#endif

int	option_v = 0;
int	option_w = 0;
int	option_g = 0;
int	option_m = 0;	/* Our flag for new option "-m" */
char	  hostnm[128];
int	option_s = 0;
int	option_d = 0;
int	option_f = 0;

/* 
 * -----------------------------------------------------------------
 * Main program
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
	main(argc, argv, envp)
	int	argc;
	char	*argv[], *envp[];
#else
int	main(int argc, char *argv[], char *envp[])
#endif
	{
	int ret;
	int i;


#include "startup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix startup. Main modules
			ALSO require the inclusion of 
				header.h
				startup.h */

#if defined(DEBUG2)
	fprintf(stderr, "\nus=\"%s\" us_subdomain=\"%s\" ", us, us_subdomain);
	fprintf(stderr, "\nus_domainstr=\"%s\" (%s) ", 
			us_domainstr, us_netclass);
	fprintf(stderr, "\nus_inet32=%lx us_ipstring=\"%s\" ", 
			(long)us_inet32 & 0x0ffffffff, us_ipstring);
	fflush(stderr);
#endif

	i = 1;
	for (i = 1 ; i < argc ; i++)
		{
		if (strcmp(argv[i], "-?") == 0)
			{
			usage();
			}
		else if (strcmp(argv[i], "-v") == 0)
			{
			option_v = 1;		/* verbose flag */
			}
		else if (strcmp(argv[i], "-w") == 0)
			{
			option_w = 1;		/* wait flag */
			}
		else if (strcmp(argv[i], "-d") == 0)
			{
			option_d = 1;		/* delete arp entry */
			++i;
			if (i != (argc - 1))
				{
				fprintf(stderr,
			"\nerror: expected only hostname after \"-d\" \n");
				fflush(stderr);
				usage();
				}
			else
				{
				strcpy(hostnm, argv[i]); 
				delete(hostnm);
				goto finish;
				}
			}
		else if (strcmp(argv[i], "-g") == 0)
			{
			option_g = 1;		/* get arp entry */
			++i;
			if (i != (argc - 1))
				{
				fprintf(stderr,
			"\nerror: expected only hostname after \"-g\" \n");
				fflush(stderr);
				usage();
				}
			else
				{
				strcpy(hostnm, argv[i]); 
				strip_subdomain(hostnm);
				get(hostnm, (char *)NULL);
				goto finish;
				}
			}
		else if (strcmp(argv[i], "-m") == 0)
			{
			option_m = 1;		/* like "-g" but use
				TERSE output (for scripts) so TURN OFF
				option_v */
			option_v = 0;
			++i;
			if (i != (argc - 1))
				{
				fprintf(stderr,
			"\nerror: expected only hostname after \"-m\" \n");
				fflush(stderr);
				usage();
				}
			else
				{
				strcpy(hostnm, argv[i]); 
				ret = strip_subdomain(hostnm);
				if (ret)
				/* display FULLY-qualified name */
					get(hostnm, us_subdomain);
				else
					get(hostnm, (char *)NULL);
				goto finish;
				}
			}
		else if (strcmp(argv[i], "-s") == 0)
			{
			option_s = 1;		/* get arp entry */
			++i;
			if (i > (argc - 2))
				{
				fprintf(stderr,
  "\nerror: expected hostname ether_addr [temp] [pub] [trail] after \"-s\"\n");
				fflush(stderr);
				usage();
				}
			else
				{
				ret = (set(argc-i, &argv[i]) ? 1 : 0);
				goto finish;
				}
			}
		else if (strcmp(argv[i], "-f") == 0)
			{
			option_f = 1;		/* from filename */
			++i;
			if (i != (argc - 1))
				{
				fprintf(stderr,
			"\nerror: expected only filename after \"-f\" \n");
				fflush(stderr);
				usage();
				}
			else
				{
				ret = (file(argv[i]) ? 1 : 0);
				goto finish;
				}
			}
		}  /* end of for (i = 1 ; i < argc ; i++) */

	usage();   /* unknown option or command */

finish:

#include "cleanup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */

	return(0);
	}  /* end of main */

/* 
 * -----------------------------------------------------------------
 * file: Process a file to set standard arp entries
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
int	file(name)
	char *name;
#else
int	file(char *name)
#endif
	{
	FILE *fp;
	int i, retval, lineno;
	char line[100], arg[5][50], *args[5];

	if ((fp = fopen(name, "r")) == NULL) 
		{
		fprintf(stderr, 
			"file error: cannot open file \"%s\" \n", name);
		fflush(stderr);
		exit(1);
		}
	args[0] = &arg[0][0];
	args[1] = &arg[1][0];
	args[2] = &arg[2][0];
	args[3] = &arg[3][0];
	args[4] = &arg[4][0];
	retval = 0;
	lineno = 0;
	while(fgets(line, 100, fp) != NULL) 
		{
		lineno++;
		if (strlen(line) < 2)
			{
			fprintf(stderr, "arp: empty line %d: \n", lineno);
			fflush(stderr);
			retval = 1;
			continue;
			}
		i = sscanf(line, "%s %s %s %s %s", arg[0], arg[1], arg[2],
		    arg[3], arg[4]);
		if (i < 2) 
			{
			fprintf(stderr, "arp: bad line %d: \"%s\"\n", 
				lineno, line);
			fflush(stderr);
			retval = 1;
			continue;
			}
		if (set(i, args))
			retval = 1;
		}  /* end of while */
	fclose(fp);
	return (retval);
	}

/*
 * -----------------------------------------------------------------
 * set: Try to set an individual arp entry (in the arp cache). 
 *	Return 0 on success and 1 on failure. This is equivalent
 *	to the WinNT command line:
 *
 *	arp -s [inet_address] [ethernet_address]
 *
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
int	set(argc, argv)
	int argc;
	char **argv;
#else
int	set(int argc, char **argv)
#endif
	{

#if defined(_WINNT) || defined(_WIN95)
	int	i;
	char	cmdline[128];
	char	meaddr[64];
	struct sockaddr_in dest;
	struct hostent *hp;
	unsigned int	addr;
	char	ipaddress[64];	/* e.g. 127.0.0.1 */

#else				/* Unix .. */
	struct arpreq ar;	/* .. has a standard ARP interface */
	struct hostent *hp;
	struct sockaddr_in *sin;
	u_char *ea;
	int s;
#endif
	char *host = argv[0], *eaddr = argv[1];

	argc -= 2;
	argv += 2;

#if defined(_WINNT) || defined(_WIN95)

	memset(&dest, 0, sizeof(dest));
	hp = gethostbyname(host);	/* query DNS services */
	if (!hp)
		{
		if ( (!option_m) && (option_v) )
			{
			fprintf(stderr,
			  "\ngethostbyname() failed, trying inet_addr...");
			fflush(stderr);
			}
		addr = inet_addr(host);	/* assume IP address (string) */
		if (addr == INADDR_NONE)
			{
			fprintf(stderr,
			  "\nerror: unable to resolve name \"%s\" \n",host);
			fflush(stderr);
			exit(1);
			}
			/* copy valid (NETWORK BYTE ORDER) IP address */
		dest.sin_addr.s_addr = addr;
		sprintf(ipaddress, "%d.%d.%d.%d",
			(ntohl(addr) & 0xff000000) >> 24,
			(ntohl(addr) & 0x00ff0000) >> 16,
			(ntohl(addr) & 0x0000ff00) >>  8,
			(ntohl(addr) & 0x000000ff));
		if ( (!option_m) && (option_v) )
			{
			fprintf(stderr, "\naddr=%s", ipaddress);
			fflush(stderr);
			}
		dest.sin_family = AF_INET;
		}
	else
		{
		if ( (!option_m) && (option_v) )
			{
			fprintf(stderr, "\ngethostbyname successful: ");
			fflush(stderr);
			dump_hostent(stderr, hp);
			}

			/* copy valid (NETWORK BYTE ORDER) IP address */
		memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
		dest.sin_family = hp->h_addrtype;
		sprintf(ipaddress, "%d.%d.%d.%d",
			(ntohl(dest.sin_addr.s_addr) & 0xff000000) >> 24,
			(ntohl(dest.sin_addr.s_addr) & 0x00ff0000) >> 16,
			(ntohl(dest.sin_addr.s_addr) & 0x0000ff00) >>  8,
			(ntohl(dest.sin_addr.s_addr) & 0x000000ff));
		}
	if ( (!option_m) && (option_v) )
		{
		fprintf(stderr, "\nsocket address ready: ");
		fflush(stderr);
		dump_sockaddr(stderr, &dest);
		}

		/* change "xx:xx:xx:xx:xx:xx" to "xx-xx-xx-xx-xx-xx" */
	for (i = 0 ; i < (int)strlen(eaddr) ; i++)
		{
		if (eaddr[i] == ':') meaddr[i] = '-';
			else meaddr[i] = eaddr[i];
		}
	meaddr[strlen(eaddr)] = '\0';
	sprintf(cmdline, "%s\\arp.exe -s %s %s",
		net_bin_directory, ipaddress, meaddr);
	if (option_v)
		{
		fprintf(stderr, "\nSet ARP Cache: system(%s)\n", cmdline);
		fflush(stderr);
		}
	system(cmdline);

#else					/* Unix */

	bzero((caddr_t)&ar, sizeof ar);
	sin = (struct sockaddr_in *)&ar.arp_pa;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = inet_addr(host);
	if (sin->sin_addr.s_addr == -1) {
		if (!(hp = gethostbyname(host))) {
			fprintf(stderr, "arp: %s: ", host);
/* *** OLD ***
			herror((char *)NULL); */
			return (1);
		}
		bcopy((char *)hp->h_addr, (char *)&sin->sin_addr,
		    sizeof sin->sin_addr);
	}
	ea = (u_char *)ar.arp_ha.sa_data;
	if (ether_aton(eaddr, ea))
		return (1);
	ar.arp_flags = ATF_PERM;
	while (argc-- > 0) {
		if (strncmp(argv[0], "temp", 4) == 0)
			ar.arp_flags &= ~ATF_PERM;
		else if (strncmp(argv[0], "pub", 3) == 0)
			ar.arp_flags |= ATF_PUBL;
		else if (strncmp(argv[0], "trail", 5) == 0)
			ar.arp_flags |= ATF_USETRAILERS;
		argv++;
		}  /* end of while */
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) 
		{
		perror("arp: socket");
		exit(1);
		}
	if (ioctl(s, SIOCSARP, (caddr_t)&ar) < 0) 
		{
		perror(host);
		exit(1);
		}
	close(s);
#endif
	return (0);
	}

/*
 * -----------------------------------------------------------------
 * Display an individual arp entry. There are three possibilities:
 *
 *	1. hardware (Ethernet) entry is IN the ARP cache.
 *
 *	2. an entry is IN the ARP cache but "(incomplete)" meaning
 *	   that the target host is probably down.
 *
 *	3. NO entry is in the arp cache, in which case we will PING
 *	   the target and try again (MAXTRIES - 1) time(s). Unless
 *	   net is very heavily loaded we expect to resolve down
 *	   to case 1. or 2. above.
 *
 *	4. Note that the output line will be host ONLY unless the
 *	   char pointer addsub is non-NULL.
 *
 *	5. This is equivalent to the WinNT command line:
 *
 *	arp -g [inet_address] 
 *
 *					Marc Thomas
 * -----------------------------------------------------------------
 */

#define MAX_RECV_SPACE	65536
u_char	recvbuf[MAX_RECV_SPACE];


#if defined(_NOPROTO)
	get(host, addsub)
	char	*host, *addsub;
#else
void	get(char *host, char *addsub)
#endif
	{
	int	tries;		/* tries */
	int	ret, len;
	struct hostent *hp;
	struct protoent *proto;
	struct sockaddr_in from;
	int	fromlen = sizeof(from);
	time_t	ltime, stime;
	struct	timeval	stimeout;
	fd_set	readmask;
	char	scmdline[128];

#if defined(_WINNT) || defined(_WIN95)

	struct sockaddr_in dest;
	char	cmdline[128];
	unsigned int	addr;
	char	ipaddress[64];	/* e.g. 127.0.0.1 */
	SOCKET	sockRaw;	/* non-Unix SOCKET is unsigned */
	char	ethaddress[64];	/* e.g. 01-02-03-04-05-06 */
	FILE	*sfp;

#else				/* Unix .. */
	struct arpreq ar;	/* .. has a standard ARP interface 
				(defined in /usr/include/net/if_arp.h) */
	char *inet_ntoa();
	struct sockaddr_in *sin;
	time_t	sstime;
	u_char *ea;
	int s, sr;	/* dgram socket and raw socket descriptors */
	struct	timeval	curtime;
#endif

	tries = 0;

#if defined(_WINNT) || defined(_WIN95)

	memset(&dest, 0, sizeof(dest));
	hp = gethostbyname(host);	/* query DNS services */
	if (!hp)
		{
		if (option_v)
			{
			fprintf(stderr,
			  "\ngethostbyname() failed, trying inet_addr...");
			fflush(stderr);
			}
		addr = inet_addr(host);	/* assume IP address (string) */
		if (addr == INADDR_NONE)
			{
			fprintf(stderr,
			  "\nerror: unable to resolve name \"%s\" \n",host);
			fflush(stderr);
			exit(1);
			}
			/* copy valid (NETWORK BYTE ORDER) IP address */
		dest.sin_addr.s_addr = addr;
		sprintf(ipaddress, "%d.%d.%d.%d",
			(ntohl(addr) & 0xff000000) >> 24,
			(ntohl(addr) & 0x00ff0000) >> 16,
			(ntohl(addr) & 0x0000ff00) >>  8,
			(ntohl(addr) & 0x000000ff));
		if (option_v)
			{
			fprintf(stderr, "\naddr=%s", ipaddress);
			fflush(stderr);
			}
		dest.sin_family = AF_INET;
		}
	else
		{
		if (option_v)
			{
			fprintf(stderr, "\ngethostbyname successful: ");
			fflush(stderr);
			dump_hostent(stderr, hp);
			}

			/* copy valid (NETWORK BYTE ORDER) IP address */
		memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
		dest.sin_family = hp->h_addrtype;
		sprintf(ipaddress, "%d.%d.%d.%d",
			(ntohl(dest.sin_addr.s_addr) & 0xff000000) >> 24,
			(ntohl(dest.sin_addr.s_addr) & 0x00ff0000) >> 16,
			(ntohl(dest.sin_addr.s_addr) & 0x0000ff00) >>  8,
			(ntohl(dest.sin_addr.s_addr) & 0x000000ff));
		}
	if (option_v)
		{
		fprintf(stderr, "\nsocket address ready: ");
		fflush(stderr);
		dump_sockaddr(stderr, &dest);
		}

	proto = getprotobyname("icmp");
	if (proto == NULL)
		{
		perror("getprotobyname error:");
		exit(2);
		}
			/* We use the Winsock 2.0 "WSASocket" rather
		than "socket" here because we want to be SURE we
		can get a RAW socket (NOT guaranteed in Winsock 1.1) */

	sockRaw = WSASocket(AF_INET, SOCK_RAW, proto->p_proto, NULL, 0, 0);
	if (sockRaw < 0)
		{
		perror("WSASocket(AF_INET,SOCK_RAW,...) error:");
		exit(3);
		}


	/* *** BSD-CODE *** : the SO_XXXTIMEO setsockopt() is NOT 
		SUPPORTED in the Windows NT Winsock 2.0 although they
		ship Visual C++ version 5.0 with a ping.c example
		which uses it! Therefore, we use a select() loop
		below.

	timeout = 1000;
	ret = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
		sizeof(timeout));
	if (ret == SOCKET_ERROR)
		{
		fprintf(stderr,
		"\nerror: failed to set recv timeout %d \n",
			WSAGetLastError());
		fflush(stderr);
		exit(4);
		}
	timeout = 1000;
	ret = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
		sizeof(timeout));
	if (ret == SOCKET_ERROR)
		{
		fprintf(stderr,
		"\nerror: failed to set send timeout %d \n",
			WSAGetLastError());
		fflush(stderr);
		exit(5);
		} */

queryarp:
		/* ping the target to force an ARP */
	pinger(sockRaw, host, (struct sockaddr *)&dest);

	if (option_w)	/* wait for response to PING */
		{
			/* use a select() loop for receive */	
		stimeout.tv_sec = 0;
		stimeout.tv_usec = 10000;
		FD_ZERO(&readmask);
		FD_SET(sockRaw, &readmask);

		if (!option_m)
			{
			fprintf(stderr,
	"\nSelect: select(ULIMIT(=%d),..,timeout(usecs=%d))..",
				ULIMIT, stimeout.tv_usec);
			fflush(stderr);
			}
	
		ret = select(ULIMIT, (fd_set *)&readmask, 
			(fd_set *)NULL, (fd_set *)NULL, &stimeout);
		if (ret < 0)
			{
			if (!option_m)
				{
				fprintf(stderr, "select error");
				fflush(stderr);
				}
			goto closedown;
			}
		else if (ret == 0)
			{
			if (!option_m)
				{
				fprintf(stderr, "timed out.");
				fflush(stderr);
				}
			goto closedown;
			}
		else
			{
			if (!option_m)
				{
				fprintf(stderr, "socket read ready");
				fflush(stderr);
				}

			ret = recvfrom(sockRaw, recvbuf, MAX_RECV_SPACE,
				0, (struct sockaddr *)&from, &fromlen);
	
			if (!option_m)
				{
				fprintf(stderr,
	"\nRecv ICMP: recvfrom(sock(=%d), (char *)buf, max(=%d),..)..",
				sockRaw, MAX_RECV_SPACE);
				fflush(stderr);
				}

			if (ret == SOCKET_ERROR)
				{
				if ( (!option_m) &&
				     (WSAGetLastError() == WSAETIMEDOUT) )
					{
					fprintf(stderr, "timed out.");
					fflush(stderr);
					}
				else
					{
					fprintf(stderr,
			"\nrecvfrom error: %d \n", WSAGetLastError());
					fflush(stderr);
					exit(6);
					}
				}
			else
				{
				if (!option_m)
					{
					fprintf(stderr, "%d bytes ", ret);
					fflush(stderr);
					}

				if ( (!option_m) && (DO_TIMING) )
				  {
				  /* format of ICMP response is
					ICMP_HEADER	 8
					IP_HEADER WHICH
					 ENCAPSULATED
					 ORG ICMP 	20
					ORG_SENT_DATA   56 */
				  ltime = (time_t)GetTickCount();

				  /* compare to SENT time starting 
				     at offset 28 */
				  stime = (time_t)0;
				  stime = (time_t)recvbuf[28];
				  stime = (stime << 8)
						+ (time_t)recvbuf[29];
				  stime = (stime << 8)
						+ (time_t)recvbuf[30];
				  stime = (stime << 8)
						+ (time_t)recvbuf[31];

				  /* RAW sockets catch lots of stuff -
				  make SURE that time is reasonable */
				  if  ( ((int)(ltime - stime) >= 0) &&
					((int)(ltime - stime) < 10000) )
					{
				  	fprintf(stderr, "%d ms", 
						(int)(ltime - stime));
					}
				  else
					{
				  	fprintf(stderr, "BAD ms"); 
					}
				 fflush(stderr);
				  }

				if (!option_m) 
				  dump_icmp(stderr, (char *)recvbuf, 0, 
					ret, &len);
				}
			}  /* select NON-timeout, NON-error */

		}  /* end of if (option_w) */

closedown:
		   /* run Windows NT ARP program .. */

	sprintf(cmdline, "%s\\arp.exe -g %s > digarp.tmp",
			net_bin_directory, ipaddress);
	sprintf(scmdline, "%s\\arp.exe -g %s",
			net_bin_directory, ipaddress);

	if ( (!option_m) && ( (option_w) || (option_v) ) )
		{
		fprintf(stderr, "\nCheck ARP Cache: system(%s)\n", 
			scmdline);
		fflush(stderr);
		}
	system(cmdline);

		   /* .. and parse output of Windows NT ARP */
	sfp = fopen("digarp.tmp", "r");
	if (sfp == (FILE *)NULL)
		{
		fprintf(stderr, "\nfopen(\"digarp.tmp\",\"r\" failed.");
		fflush(stderr);
		exit(1);
		}

	ret = parse_system_output(sfp, ipaddress, ethaddress);
	if (ret == 0)
		{
		if (addsub != (char *)NULL)
			printf("%s.%s A %s %s", host, addsub,
				ipaddress, ethaddress);
		else
			printf("%s A %s %s", host, 
				ipaddress, ethaddress);
		printf("\n");
		fflush(stdout);
		fclose(sfp);
		}
	else if (!option_m)
		{
		if (addsub != (char *)NULL)
			printf("%s.%s A %s %s", host, addsub,
				ipaddress, "(incomplete)");
		else
			printf("%s A %s %s", host, 
				ipaddress, "(incomplete)");
		printf("\n");
		fflush(stdout);
		fclose(sfp);
		if (++tries < MAXTRIES)
			goto queryarp; 	/* look again */
		}
	CLOSE_SOCKET(sockRaw);

#else					/* Unix */

	bzero((caddr_t)&ar, sizeof ar);
	ar.arp_pa.sa_family = AF_INET;
	sin = (struct sockaddr_in *)&ar.arp_pa;
	sin->sin_family = AF_INET;
					/* if host is given symbolically,
			inet_addr() will return -1, otherwise it will
			convert host from dotted-decimal to UINET_32 */
	sin->sin_addr.s_addr = inet_addr(host);
	if (sin->sin_addr.s_addr == -1) 
		{
		if (!(hp = gethostbyname(host))) 
			{
			if ( (!option_m) && (option_v) )
				{
				fprintf(stderr,
			  	  "\ngethostbyname(%s) failed...", host); 
				fflush(stderr);
				}
			exit(1);
			}
		bcopy((char *)hp->h_addr, (char *)&sin->sin_addr,
		    sizeof sin->sin_addr);
		}
queryarp:
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) 
		{
		perror("arp: socket");
		exit(1);
		}
		/* ioctl will return < 0 if NOT in arp cache */

/* SIOCGARP NOW FAILS with 2.2.x kernels -- it seems someone fucked
	with the code so now you HAVE TO ADD THE FOLLOWING: */

#if defined(__linux)		/* have to do this first because
			we might be on __alphalx where both
			__alpha and __linux are defined */

	ar.arp_dev[0] = 'e';
	ar.arp_dev[1] = 't';
	ar.arp_dev[2] = 'h';
	ar.arp_dev[3] = '0';
	ar.arp_dev[4] = '\0';

#elif defined(__alpha) || defined(__sun) || defined(__bsd)

#else
	ar.arp_dev[0] = 'e';
	ar.arp_dev[1] = 't';
	ar.arp_dev[2] = 'h';
	ar.arp_dev[3] = '0';
	ar.arp_dev[4] = '\0';
#endif

	if (ioctl(s, SIOCGARP, (caddr_t)&ar) < 0) 
		{
		if ( (!option_m) && (option_v) )
			{
			fprintf(stderr, "\nNo entry in arp cache: ");
			dump_arpstruct(stderr, &ar);
			fprintf(stderr, "\n");
			fflush(stderr);
			}
		if (errno == ENXIO)
			{
			if (option_m == 0)
				printf("%s (%s) -- no entry\n",
				    host, inet_ntoa(sin->sin_addr));
			proto = getprotobyname("icmp");
			if (proto == NULL)
				{
				perror("getprotobyname");
				exit(2);
				}
			sr = socket(AF_INET, SOCK_RAW, proto->p_proto);
			if (sr < 0)
				{
				if (!option_m)
				  perror("socket(AF_INET,SOCK_RAW,...)");

				/* installer didn't suid root on 
				the executable, so we use system() to
				go the job */

				sprintf(scmdline,
				  "%s/ping %s %s >/dev/null", 
					DEF_NET_BIN_DIRECTORY,
					PING_TIMES, host);

				if (!option_m)
				  {
				  fprintf(stderr, 
				     "\nrunning system(\"%s\")... \n", 
					scmdline);
				  fflush(stderr);
				  }
				system(scmdline);
				}
			else
				{
				/* ping (through the RAW_SOCKET) the 
				target to force an arp */
				pinger(sr, host, &ar.arp_pa);

				if (option_w)	/* wait for response */
				  {
				/* use a select() loop for receive */	
				  stimeout.tv_sec = 0;
				  stimeout.tv_usec = 10000;
				  FD_ZERO(&readmask);
				  FD_SET(sr, &readmask);
	
				  if (!option_m)
				  	{
					fprintf(stderr,
		"\nSelect: select(ULIMIT(=%d),..,timeout(usecs=%d))..",
					ULIMIT, (int)stimeout.tv_usec);
					fflush(stderr);
					}
	
				  ret = select(ULIMIT, (fd_set *)&readmask, 
					(fd_set *)NULL, 
					(fd_set *)NULL, 
					&stimeout);
				  if (ret < 0)
					{
					if (!option_m)
						{
						fprintf(stderr, 
						  "select error\n");
						fflush(stderr);
						}
					goto closedown;
					}
				  else if (ret == 0)
					{
					if (!option_m)
						{
						fprintf(stderr, 
						  "timed out.\n");
						fflush(stderr);
						}
					goto closedown;
					}
				  else
					{
					if (!option_m)
						{
						fprintf(stderr, 
						  "socket read ready");
						fflush(stderr);
						}

					ret = recvfrom(sr, 
					  recvbuf, MAX_RECV_SPACE,
					  0, (struct sockaddr *)&from, 
					  &fromlen);

					if (!option_m)
						{
						fprintf(stderr,
	"\nRecv ICMP: recvfrom(sock(=%d), (char *)buf, max(=%d),..)..",
						sr, MAX_RECV_SPACE);
						fflush(stderr);
						}

					}
					/* ret NOW is number of bytes
					read by recvfrom() */
				  if (ret > 0)
					{
					if (!option_m)
						{
						fprintf(stderr, 
						  "%d bytes ", ret);
						fflush(stderr);
						}

					if ( (!option_m) && (DO_TIMING) )
				  		{
				  /* format of ICMP response is
					ICMP_HEADER	 8
					IP_HEADER WHICH
					 ENCAPSULATED
					 ORG ICMP 	20
					ORG_SENT_DATA   56 */

			(void)gettimeofday((struct timeval *)&curtime,
			    (struct timezone *)NULL);

					ltime = (time_t)curtime.tv_usec;

				  /* compare to SENT time starting 
				     at offset 28/32 (usecs) Note that
				     we manually put this in B_ENDIAN) */
				sstime = (time_t)0;
				sstime = (time_t)recvbuf[28];
				sstime = (sstime << 8) + (time_t)recvbuf[29];
				sstime = (sstime << 8) + (time_t)recvbuf[30];
				sstime = (sstime << 8) + (time_t)recvbuf[31];

				stime = (time_t)0;
				stime = (time_t)recvbuf[32];
				stime = (stime << 8) + (time_t)recvbuf[33];
				stime = (stime << 8) + (time_t)recvbuf[34];
				stime = (stime << 8) + (time_t)recvbuf[35];
/* *** OLD ***
				sstime = (time_t)0;
				sstime = (time_t)recvbuf[31];
				sstime = (sstime << 8) + (time_t)recvbuf[30];
				sstime = (sstime << 8) + (time_t)recvbuf[29];
				sstime = (sstime << 8) + (time_t)recvbuf[28];

				stime = (time_t)0;
				stime = (time_t)recvbuf[35];
				stime = (stime << 8) + (time_t)recvbuf[34];
				stime = (stime << 8) + (time_t)recvbuf[33];
				stime = (stime << 8) + (time_t)recvbuf[32]; */

				  /* RAW sockets catch lots of stuff -
				  make SURE that time is reasonable */
				  if ((curtime.tv_sec - sstime) <= 3) 
							{
				  		fprintf(stderr, "%d ms", 
						 (int)(ltime - stime));
							}
						  else
							{

/* *** DIAGNOSTIC ***
				  			fprintf(stderr, 
							  "BAD ms");  */

						  	fprintf(stderr, 
			  "\n BAD ms? (ltime=0x%4x - stime=0x%4x = %d msecs)",
					(int)ltime, (int)stime,
					(int)(ltime - stime));   
							}
						fflush(stderr);
				  		}  /* end of TIMING */

					if (!option_m)
					  {
					  dump_icmp(stderr, (char *)recvbuf, 
						0, ret, &len);
					  fprintf(stderr, "\n");
					  fflush(stderr);
					  }
				    	}  /* got some valid reply */
				  }  /* end of if (option_w) */
closedown:
				close(sr);
				}
				/* look again */
			if (++tries < MAXTRIES)
				{
				goto queryarp;	
				}
			}
		else
			{
			perror("SIOCGARP");
			}
		exit(1);
		}
	if ( (!option_m) && (option_v) )
		{
		fprintf(stderr, "\nEntry in arp cache: ");
		dump_arpstruct(stderr, &ar);
		fprintf(stderr, "\n");
		fflush(stderr);
		}
	close(s);
	ea = (u_char *)ar.arp_ha.sa_data;
	if (option_m == 0)
		printf("%s (%s) at ", host, inet_ntoa(sin->sin_addr));
	if (ar.arp_flags & ATF_COM)	/* completed entry */
		{
		if (option_m == 0) ether_print(ea);
		}
	else
		{
		if (option_m == 0) printf("(incomplete)");
		if (++tries < MAXTRIES) 
			{
			if (option_m == 0) printf("...\n");
			sleep(1);
			goto queryarp;	
			}
		}
	if (ar.arp_flags & ATF_PERM)
		if (option_m == 0) printf(" permanent");
	if (ar.arp_flags & ATF_PUBL)
		if (option_m == 0) printf(" published");
	if (ar.arp_flags & ATF_USETRAILERS)
		if (option_m == 0) printf(" trailers");
	if (option_m == 0) printf("\n");
	if (option_m)
		{
		printf("%s A %s ", host, inet_ntoa(sin->sin_addr));
		if (ar.arp_flags & ATF_COM)	/* completed entry */
			ether_print(ea);
		else
			printf("00:00:00:00:00:00");
		printf("\n");
		fflush(stdout);
		}
#endif
	} /* end of get */

/*
 * -----------------------------------------------------------------
 * delete: Delete an arp entry. This is equivalent to the WinNT 
 * 	command line:
 *
 *	arp -d [inet_address] 
 *
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
	delete(host)
	char *host;
#else
void	delete(char *host)
#endif
	{

#if defined(_WINNT) || defined(_WIN95)
	char	cmdline[128];
	struct sockaddr_in dest;
	struct hostent *hp;
	unsigned int	addr;
	char	ipaddress[64];	/* e.g. 127.0.0.1 */

#else				/* Unix .. */
	struct arpreq ar;	/* .. has a standard ARP interface */
	struct hostent *hp;
	struct sockaddr_in *sin;
	int s;
#endif

#if defined(_WINNT) || defined(_WIN95)

	memset(&dest, 0, sizeof(dest));
	hp = gethostbyname(host);	/* query DNS services */
	if (!hp)
		{
		if ( (!option_m) && (option_v) )
			{
			fprintf(stderr,
			  "\ngethostbyname() failed, trying inet_addr...");
			fflush(stderr);
			}
		addr = inet_addr(host);	/* assume IP address (string) */
		if (addr == INADDR_NONE)
			{
			fprintf(stderr,
			  "\nerror: unable to resolve name \"%s\" \n",host);
			fflush(stderr);
			exit(1);
			}
			/* copy valid (NETWORK BYTE ORDER) IP address */
		dest.sin_addr.s_addr = addr;
		sprintf(ipaddress, "%d.%d.%d.%d",
			(ntohl(addr) & 0xff000000) >> 24,
			(ntohl(addr) & 0x00ff0000) >> 16,
			(ntohl(addr) & 0x0000ff00) >>  8,
			(ntohl(addr) & 0x000000ff));
		if ( (!option_m) && (option_v) )
			{
			fprintf(stderr, "\naddr=%s", ipaddress);
			fflush(stderr);
			}
		dest.sin_family = AF_INET;
		}
	else
		{
		if ( (!option_m) && (option_v) )
			{
			fprintf(stderr, "\ngethostbyname successful: ");
			fflush(stderr);
			dump_hostent(stderr, hp);
			}

			/* copy valid (NETWORK BYTE ORDER) IP address */
		memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
		dest.sin_family = hp->h_addrtype;
		sprintf(ipaddress, "%d.%d.%d.%d",
			(ntohl(dest.sin_addr.s_addr) & 0xff000000) >> 24,
			(ntohl(dest.sin_addr.s_addr) & 0x00ff0000) >> 16,
			(ntohl(dest.sin_addr.s_addr) & 0x0000ff00) >>  8,
			(ntohl(dest.sin_addr.s_addr) & 0x000000ff));
		}
	if ( (!option_m) && (option_v) )
		{
		fprintf(stderr, "\nsocket address ready: ");
		fflush(stderr);
		dump_sockaddr(stderr, &dest);
		}

	sprintf(cmdline, "%s\\arp.exe -d %s",
		net_bin_directory, ipaddress);
	if (option_v)
		{
		fprintf(stderr, "\nDelete ARP Cache: system(%s)\n", cmdline);
		fflush(stderr);
		}
	system(cmdline);

#else					/* Unix */

	bzero((caddr_t)&ar, sizeof ar);
	ar.arp_pa.sa_family = AF_INET;
	sin = (struct sockaddr_in *)&ar.arp_pa;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = inet_addr(host);
	if (sin->sin_addr.s_addr == -1) {
		if (!(hp = gethostbyname(host))) {
			fprintf(stderr, "arp: %s: ", host);
/* *** OLD ***
			herror((char *)NULL); */
			exit(1);
		}
		bcopy((char *)hp->h_addr, (char *)&sin->sin_addr,
		    sizeof sin->sin_addr);
	}
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("arp: socket");
		exit(1);
	}
	if (ioctl(s, SIOCDARP, (caddr_t)&ar) < 0) 
		{
		if (errno == ENXIO)
			printf("%s (%s) -- no entry\n",
			    host, inet_ntoa(sin->sin_addr));
		else
			perror("SIOCDARP");
		exit(1);
		}
	close(s);
	printf("%s (%s) deleted\n", host, inet_ntoa(sin->sin_addr));
#endif
	}  /* end of delete */


#if defined(_WINNT) || defined(_WIN95)

#elif defined(__linux)

struct nlist {
    char *n_name;
    unsigned long n_value;
    short n_type;		/* 0 if not there, 1 if found */
    short reserved;
} nl[] = {
#define	X_ARPTAB	0
	{ "_arptab" },
#define	X_ARPTAB_SIZE	1
	{ "_arptab_size" },
	{ "" },
};

#else
struct nlist nl[] = {
#define	X_ARPTAB	0
	{ "_arptab" },
#define	X_ARPTAB_SIZE	1
	{ "_arptab_size" },
	{ "" },
};

#endif


/*
 * -----------------------------------------------------------------
 * ether_print: take an unsigned char array[6] and print out the
 *	encapsulated ethernet address (assuming NETWORK BYTE ORDER)
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
	ether_print(cp)
	u_char *cp;
#else
void	ether_print(u_char *cp)
#endif
	{
	printf("%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", 
		cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]);
	}

/*
 * -----------------------------------------------------------------
 * ether_aton: take an ethernet (string) address of the form
 *	"%x:%x:%x:%x:%x:%x" and, if valid, convert to an array
 *	of six unsigned chars n[6] in NETWORK BYTE ORDER, and
 *	return 0, otherwise return 1.
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
int	ether_aton(a, n)
	char *a;
	u_char *n;
#else
int	ether_aton(char *a, u_char *n)
#endif
	{
	int i, o[6];

	i = sscanf(a, "%x:%x:%x:%x:%x:%x", &o[0], &o[1], &o[2],
					   &o[3], &o[4], &o[5]);
	if (i != 6) 
		{
		fprintf(stderr, "arp: invalid Ethernet address '%s'\n", a);
		fflush(stderr);
		return (1);
		}
	for (i=0; i<6; i++)
		n[i] = o[i];
	return (0);
	}  /* end of ether_aton */

/* 
 * -----------------------------------------------------------------
 * usage: show command line options
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
	usage()
#else
void	usage(void)
#endif
  {
  printf("\nusage: arp [-v] [-w] [-mgdsf] hostname [additional params] ");
  printf("\n");
  printf("\nmodifier:  -v     verbose - show progress and packets on stderr");
  printf("\n           -w     when pinging, wait for return frame(s) or ");
  printf("\n                  timeout before checking arp-cache ");
  printf("\noption:    -g     check for hostname\'s ether_addr in ARP-cache,");
  printf("\n                  if not present, ping hostname up to %d times ",
	MAXTRIES);
  printf("\n                  and look again ");
  printf("\n           -m     same as \"-g\" but TERSE-mode ");
  printf("\n           -d     delete hostname entry from ARP-cache ");
  printf("\n           -s     set hostname ether_addr in ARP-cache, e.g. ");
  printf("\n       arp -s hostname ether_addr [temp] [pub] [trail] ");
  printf("\n           -f     set input from /etc/ethers-type FILE, e.g. ");
  printf("\n       arp -f filename \n");
  exit(1);
  }  /* end of usage */

/*
 * -----------------------------------------------------------------
 * pinger: Compose and transmit an ICMP ECHO REQUEST packet through a
 *   RAW socket (need suid root).  The ID field is our UNIX process ID,
 *   and the sequence number is an ascending integer.  Under Unix, the
 *   first 8 bytes of the data portion are used to hold a UNIX timeval 
 *   struct in VAX byte-order, to compute the round-trip time. Under Win
 *   NT only 4 bytes are used (with GetTickCount() used to find time
 *   in milliseconds since BOOT). This code has been taken and modified 
 *   from BSD ping.c
 *						Marc Thomas 
 * -----------------------------------------------------------------
 */

#define	DEFDATALEN	(64 - 8)	/* default data length */
#define	MAXIPLEN	60
#define	MAXICMPLEN	76
#define	MAXPACKET	(65536 - 60 - 8)/* max packet size */
#define	MAXWAIT_DEFAULT	10		/* max seconds to wait for response */
#define	NROUTES		9		/* number of record route slots */

#define	A(bit)		rcvd_tbl[(bit)>>3]	/* identify byte in array */
#define	B(bit)		(1 << ((bit) & 0x07))	/* identify bit in byte */
#define	SET(bit)	(A(bit) |= B(bit))
#define	CLR(bit)	(A(bit) &= (~B(bit)))
#define	TST(bit)	(A(bit) & B(bit))

#define MAX_DUP_CHK	(8 * 128)
int	mx_dup_ck = MAX_DUP_CHK;
char	rcvd_tbl[MAX_DUP_CHK / 8];
u_char	outpack[MAXPACKET];
long	ntransmitted = 0L;
int	datalen = DEFDATALEN;

#if defined(_NOPROTO)
     pinger(sock, hostname, pwhereto)
	int	sock;
	char	*hostname;
	struct sockaddr *pwhereto;
#else
void pinger(int sock, char *hostname, struct sockaddr *pwhereto)
#endif
	{
	register struct icmp *icp;
	register int cc;
	int i, len;
#if defined(_WINNT) || defined(_WIN95)
	time_t	ltime;
#else
	struct	timeval	curtime;
#endif

	icp = (struct icmp *)outpack;
	icp->icmp_type = ICMP_ECHO;
	icp->icmp_code = 0;
	icp->icmp_cksum = 0;
	icp->icmp_seq = htons((unsigned short)ntransmitted++);
	icp->icmp_id = getpid() & 0xffff;		/* ID */

	CLR(icp->icmp_seq % mx_dup_ck);

#if defined(_WINNT) || defined(_WIN95)

	if (DO_TIMING)
		{
		ltime = (time_t)GetTickCount();

		/* time(&ltime); */
		outpack[ 8] = (ltime & 0xff000000) >> 24;
		outpack[ 9] = (ltime & 0x00ff0000) >> 16;
		outpack[10] = (ltime & 0x0000ff00) >>  8;
		outpack[11] = (ltime & 0x000000ff);
		}
#else
				/* we will put secs, usecs in
			B_ENDIAN order into bytes [8-15] so that 
			any answer to the arp will replicate these 
			in bytes [28-35] */
	if (DO_TIMING)
		{
		(void)gettimeofday((struct timeval *)&curtime,
		    (struct timezone *)NULL);
		outpack[ 8] = (curtime.tv_sec & 0xff000000) >> 24;
		outpack[ 9] = (curtime.tv_sec & 0x00ff0000) >> 16;
		outpack[10] = (curtime.tv_sec & 0x0000ff00) >>  8;
		outpack[11] = (curtime.tv_sec & 0x000000ff);

		outpack[12] = (curtime.tv_usec & 0xff000000) >> 24;
		outpack[13] = (curtime.tv_usec & 0x00ff0000) >> 16;
		outpack[14] = (curtime.tv_usec & 0x0000ff00) >>  8;
		outpack[15] = (curtime.tv_usec & 0x000000ff);
		}
#endif

	cc = datalen + 8;			/* skips ICMP portion */

	/* compute ICMP checksum here */
	icp->icmp_cksum = in_cksum((u_short *)icp, cc);

	i = sendto(sock, (char *)outpack, cc, 0, pwhereto,
	    sizeof(struct sockaddr));

	if ( (!option_m) && ( (option_v) || (option_w) ) )
		{
		fprintf(stderr,
  "\nSend ICMP: i(=%d)=sendto(sock(=%d), (char *)buf, num(=%d),..)",
			i, sock, cc);
		fflush(stderr);
		}

	if ( (!option_m) && ( (option_v) || (option_w) ) )
		dump_icmp(stderr, (char *)outpack, 0, cc, &len);

	if (i < 0 || i != cc)  
		{
		if (i < 0)
			perror("pinger: sendto");
		(void)printf("ping: wrote %s %d chars, ret=%d\n",
		    hostname, cc, i);
		}
	} /* end of pinger */

/*
 * -----------------------------------------------------------------
 * in_cksum: Checksum routine for Internet Protocol family headers 
 *	(C Version)
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
int	in_cksum(addr, len)
	u_short *addr;
	int	len;
#else
int	in_cksum(u_short *addr, int len)
#endif
	{
	register int nleft = len;
	register u_short *w = addr;
	register int sum = 0;
	u_short answer = 0;

	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1) {
		*(u_char *)(&answer) = *(u_char *)w ;
		sum += answer;
		}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return(answer);
	}

/* -----------------------------------------------------------------
 * strip_subdomain: take fully qualified name (such as 
 *	"thumper.cs.csubak.edu") and if its subdomain MATCHES ours
 *	in the global string variable us_subdomain[] then truncate 
 *	to JUST the hostname (such as "thumper") and return 1, 
 *	otherwise return 0.
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
int	strip_subdomain(hostnm)
	char	*hostnm;
#else
int	strip_subdomain(char *hostnm)
#endif
	{
	int	i;

	if (us_subdomain[0] == '\0')	/* our subdomain unknown */
		return(0);
	i = 0;
	while ( (hostnm[i] != '\0') && (hostnm[i] != '.') ) i++;

	if (hostnm[i] == '\0')
		return(0);		/* no subdomain */
	if (strcmp(us_subdomain, &hostnm[i+1]) == 0)
		{
		hostnm[i] = '\0';	/* truncate */
		return(1);
		}
	return(0);
	}  /* end of strip_subdomain */

/*
 * -----------------------------------------------------------------
 * parse_system_output: Look through the (opened) FILE (which contains
 *	Windows NT arp.exe command output) for ipaddress followed by a 
 * 	VALID ethernet address. If found, copy to ethaddress[] (changing
 *	any "-"'s to ":"'s and return 0, otherwise return 1.
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
int	parse_system_output(fp, ipaddress, ethaddress)
	FILE *fp;
	char *ipaddress, *ethaddress;
#else
int	parse_system_output(FILE *fp, char *ipaddress, char *ethaddress)
#endif

	{
	char	line[100];
	char	*cptr;
	int	ret, i, numhyphens;

	ret = 1;
	rewind(fp);			/* insurance */
	strcpy(ethaddress, "invalid");	/* insurance */

				/* search for ipaddress */
	while(fgets(line, 100, fp) != NULL) 
		{
		if ((cptr = strstr(line, ipaddress)) != (char *)NULL)
			{
			while( (*cptr != '\0') && 
				(*(cptr + 1) != '\0') &&
				(*(cptr + 2) != '\0') &&
				(*(cptr + 2) != '-') ) cptr++;
			if ( (*cptr != '\0') &&
				(*(cptr + 1) != '\0') &&
				(*(cptr + 2) != '\0') &&
				(*(cptr + 2) == '-') ) 
				{
				i = 0;
				while ( (*cptr != ' ') && (*cptr != '\0') )
					{
					ethaddress[i] = *cptr;
					i++;
					cptr++;
					}
				ethaddress[i] = '\0';

		/* check validity of ethernet address */
				numhyphens = 0;
				for (i = 0 ; i < (int)strlen(ethaddress) ;
					i++)
					{
					if (ethaddress[i] == '-')
						{
						ethaddress[i] = ':';
						++numhyphens;
						}
					}
				if (numhyphens == 5) ret = 0;
					else ret = 1;
				return(ret);
				}
			}
		}  /* end of while */

	return(ret);
	}  /* end of parse_system_output */

