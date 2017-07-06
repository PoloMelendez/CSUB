
			/* This program shows the simple use of
		gethostbyname() and gethostbyaddr() which are available 
		to programmers as part of the DNS.

		It basically does one of two things:
			  1. if given a symbolic host/domainname, it
			     looks up the IP address 
			  2. if given an IP address, it looks up the
			     canonical host/domainname 

							marc thomas */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

/* ---------------------------------------------------------------- */

#include "header.h"	/* OUR header which handles MOST of the
			WinSock/32-bit Unix/64-bit Unix Differences */
#include "startup.h"	/* globals for main module set by the
			code startup.c */
#include "diagnost.h"	/* OUR dump_xxxx() routines and other
			diagnostics */

/* ---------------------------------------------------------------- */

				/* GLOBAL VARIABLES */
struct	hostent	*hp, *ihp;	/* for gethostbyname(), gethostbyaddr() */
struct	sockaddr_in	name;	/* socket name (although we don't
				   actually open a socket here) */

UINET_32	ulnet;		/* for a network byte order (ie. Big
				   Endian) IP address (32 bits) */

/* ------------------------------------------------------------- */

int main(int argc,char *argv[], char *envp[])
	{
	int	i, do_by_name;
	char	host_name[128], khar, *cptr;
	UINET_32	ul, d1, d2, d3, d4;

#include "startup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix startup. Main modules
			ALSO require the inclusion of 
				header.h
				startup.h */

	do_by_name = 1;		/* default */

/* ********** CHECK COMMAND LINE FIRST *************** */
	if (argc > 1)
		{
		if (strcmp("-?", argv[1]) == 0)
			{
			printf("\nexample invocations: ");
			printf("\ngethost ultrix5          [get by name]");
			printf("\ngethost 136.168.201.7    [get by addr]");
			printf("\n");
			exit(1);
			}
		else
			{
			strcpy(host_name, argv[1]);
			}
		}  /* end of if (argc > 1) */
	else	   /* ask user for hostname */
		{
		printf("\nEnter Hostname: ");

/* *** OLD *** gets() is dangerous
		gets(host_name);	even though it converts '\n' to '\0' */

		fgets(host_name, sizeof(host_name) - 1, stdin);
		for (i = 0 ; i < sizeof(host_name) - 1 ; i++)
			{
			if ( (host_name[i] == '\n') || (host_name[i] == '\0') )
				break;
			}
		if (host_name[i] == '\n')
			host_name[i] = '\0'; 	/* get rid of newline */
		}

/* ********** CHECK WHETHER USER GAVE IP ADDRESS ***** */

	if ( ((khar = host_name[0]) >= '0') && (khar <= '9') )
		{
		do_by_name = 0;
		d4 = 0;
		cptr = &host_name[0];
		while ((*cptr != '.') && (*cptr != '\0'))
			{
			d4 = d4 * 10;
			d4 = d4 + ( (int)(*cptr) - (int)'0' );
			cptr++;
			}
		if (*cptr == '\0')
			{
			printf("\nBad IP address format \n");
			exit(-1);
			}
		cptr++;		/* skip '.' */
		d3 = 0;
		while ((*cptr != '.') && (*cptr != '\0'))
			{
			d3 = d3 * 10;
			d3 = d3 + ( (int)(*cptr) - (int)'0' );
			cptr++;
			}
		if (*cptr == '\0')
			{
			printf("\nBad IP address format \n");
			exit(-1);
			}
		cptr++;		/* skip '.' */
		d2 = 0;
		while ((*cptr != '.') && (*cptr != '\0'))
			{
			d2 = d2 * 10;
			d2 = d2 + ( (int)(*cptr) - (int)'0' );
			cptr++;
			}
		if (*cptr == '\0')
			{
			printf("\nBad IP address format \n");
			exit(-1);
			}
		cptr++;		/* skip '.' */
		d1 = 0L;
		while ((*cptr != '.') && (*cptr != '\0'))
			{
			d1 = d1 * 10;
			d1 = d1 + ( (int)(*cptr) - (int)'0' );
			cptr++;
			}	
		ul = (d4 << 24) | (d3 << 16) | (d2 << 8) | d1;
		printf( "\nconvert:   IP address(=%s) to host_order(=%u)",
			host_name, ul);
		goto number_to_name;	   /* do gethostbyaddr */
		}  /* end of if (((khar ... */

/* ********** GET IP ADDRESS FROM HOST(DOMAIN)NAME *** */
/*
name_to_number: 
 */
	hp = gethostbyname(host_name);
	if (hp == NULL)
		{
		printf("\n");
		fflush(stdout);
		perror("gethostbyname: ");
		exit(-1);	/* gethostbyname() failed */
		}
	printf("\ncalling:   hp = gethostbyname(\"%s\")", host_name);

			/* REPORT struct hp TOP-LEVEL contents */
	printf("\n*hp= ");
	dump_hostent(stdout, hp);	/* dump structure */
	printf("\n");

#include "cleanup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */

	return(0);


/* ********** GET HOST(DOMAIN)NAME FROM (CONVERTED) IP ADDRESS *** */

number_to_name:
			/* now CONVERT VAX unsigned long to IP address */
	ulnet = htonl(ul);
	printf("\nconvert:   host_order(=%u) to network_order(=%u)",
		ul, ulnet);

			/* now ask for symbolic name from IP address */
	ihp = gethostbyaddr((char *)&ulnet, sizeof(ulnet), AF_INET);
	if (ihp == NULL)
		{
		printf("\n");
		fflush(stdout);
		perror("gethostbyaddr: ");	/* gethostbyaddr() failed */
		exit(-2);
		}
	printf(
	  "\ncalling:   ihp = gethostbyaddr(&network_order, %d, AF_INET)",
		(int)(sizeof(ulnet)));
	printf("\n*ihp= ");
	dump_hostent(stdout, ihp);	/* dump structure */
	printf("\n");

#include "cleanup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */

	return(0);
	}  /* end of main */

