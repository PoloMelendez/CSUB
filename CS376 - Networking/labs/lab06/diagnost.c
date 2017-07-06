

#include <stdio.h>
#include <stdlib.h>

#include "header.h"	/* OUR header which handles MOST of the
			WinSock/32-bit Unix/64-bit Unix Differences */

static	char *address_family[] = {
		"AF_UNSPEC",
		"AF_UNIX",
		"AF_INET",
		"AF_IMPLINK",
		"AF_PUP",
		"AF_CHAOS",
		"AF_IPX/XNS",
		"AF_ISO",
		"AF_ECMA",
		"AF_DATAKIT",
		"AF_CCITT",
		"AF_SNA",
		"AF_DECnet",
		"AF_DLI",
		"AF_LAT",
		"AF_HYLINK",
		"AF_APPLETALK",
		"AF_ROUTE",
		"AF_LINK",
		"AF_XTP",
		"AF_NETMAN",
		"AF_X25",
		"AF_CTF",
		"AF_WAN",
		"AF_USER",
		"AF_UNKNOWN",
		"_EOLIST" };
#define	MAX_AF_NAME	25

/*
 * -----------------------------------------------------------------
 * dump_hostent: diagnostic dump of struct hostent (ref: netdb.h)
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
int	dump_hostent(tfp, hp)
	FILE *tfp;
	struct hostent *hp;
#else
int	dump_hostent(FILE *tfp, struct hostent *hp)
#endif

	{
	int	i, j, k;
	unsigned char	*cptr;
	UINET_32	uip, ul;

	/* in <netdb.h>
	struct	hostent {
		char	*h_name;
		char	**h_aliases;
		int	h_addrtype;
		int	h_length;
		char	**h_addr_list;
	#define	h_addr	h_addr_list[0] 
	}	*/

	if ( (hp == (struct hostent *)NULL) ||
	     (tfp == (FILE *)NULL) )
		return(-1);			/* insurance */

	fprintf(tfp,
	"\n  struct hostent { ");		/* print hostname */
	fprintf(tfp,
	"\n       char  *h_name(=\"%s\");", hp->h_name);

	if (hp->h_aliases != NULL)		/* check any aliases */
		{
		i = 0;
		while (hp->h_aliases[i] != NULL)
			{
			fprintf(tfp,
	"\n       int   h_aliases[%d](=\"%s\");", i, hp->h_aliases[i]);

			++i;
			}  /* end of while */
		}

	if ((hp->h_addrtype) > MAX_AF_NAME)	/* print address type */
		k = MAX_AF_NAME;
	else
		k = hp->h_addrtype;
	fprintf(tfp,
	"\n       int   h_addrtype(=%s);", address_family[k]);

						/* print address length */
	fprintf(tfp,
	"\n       int   h_length(bytes=%d);", hp->h_length);

	if (hp->h_addr_list != NULL)		/* check all addresses */
		{
		i = 0;
		while (hp->h_addr_list[i] != NULL)
			{
	  		uip = *(UINET_32 *)(hp->h_addr_list[0]);
	  		ul = ntohl(uip);	/* convert HOST byte order */

			fprintf(tfp,
	"\n       int   h_addr_list[%d](=%u=", i, ul);

			cptr = (unsigned char *)hp->h_addr_list[i];
			for (j = 0 ; j < hp->h_length ; j++)
				{
				fprintf(tfp, "%d.", (int)*cptr & 0x00ff);
				++cptr;
				}
			fprintf(tfp, ");");
			++i;
			}  /* end of while */
		}
	fprintf(tfp,
	"\n  } ");

	fflush(tfp);
	return(0);
	}  /* end of dump_hostent */

/*
 * -----------------------------------------------------------------
 * dump_sockaddr: diagnostic dump of struct sockaddr_in (ref: in.h)
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
int	dump_sockaddr(tfp, sinptr)
	FILE *tfp;
	struct sockaddr_in *sinptr;
#else
int	dump_sockaddr(FILE *tfp, struct sockaddr_in *sinptr)
#endif

	{
	int	k;

	/*  in <in.h>
	struct	sockaddr_in  {
		short	sin_family;
		u_short	sin_port;
		struct	in_addr	sin_addr;
		char	sin_zero[8];  
	}	*/

	if ( (sinptr == (struct sockaddr_in *)NULL) ||
	     (tfp == (FILE *)NULL) )
		return(-1);			/* insurance */

	fprintf(tfp,
	"\n  struct sockaddr_in { ");
	if ((sinptr->sin_family) > MAX_AF_NAME)	/* print address family */
		k = MAX_AF_NAME;
	else
		k = sinptr->sin_family;
	fprintf(tfp,
	"\n       short   sin_family(=%s);", address_family[k]);

						/* print port number (or
				0 = "to be assigned by system" */
	fprintf(tfp,
	"\n       u_short sin_port(=%hu);", ntohs(sinptr->sin_port));

						/* print ip address */
	fprintf(tfp,
	"\n       struct  in_addr sin_addr.s_addr(=%u=%d.%d.%d.%d);",
			ntohl(sinptr->sin_addr.s_addr),
			(ntohl(sinptr->sin_addr.s_addr) & 0xff000000) >> 24,
			(ntohl(sinptr->sin_addr.s_addr) & 0x00ff0000) >> 16,
			(ntohl(sinptr->sin_addr.s_addr) & 0x0000ff00) >>  8,
			(ntohl(sinptr->sin_addr.s_addr) & 0x000000ff));
	fprintf(tfp,
	"\n       char    sin_zero[8](=%x %x %x %x %x %x %x %x);",
			vaxunsign(sinptr->sin_zero[0]),
			vaxunsign(sinptr->sin_zero[1]),
			vaxunsign(sinptr->sin_zero[2]),
			vaxunsign(sinptr->sin_zero[3]),
			vaxunsign(sinptr->sin_zero[4]),
			vaxunsign(sinptr->sin_zero[5]),
			vaxunsign(sinptr->sin_zero[6]),
			vaxunsign(sinptr->sin_zero[7]));
	fprintf(tfp,
	"\n  } ");

	fflush(tfp);
	return(0);
	}  /* end of dump_sockaddr */

/*
 * -----------------------------------------------------------------
 * dump_arpstruct: diagnostic dump of struct arpreq. This is for
 *		Unix *** ONLY ***. The Winsock1.1 and Winsock2.0
 *		DLL's have *** NO *** API interface to the ARP-cache.
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO) && !defined(_WIN95) && !defined(_WINNT)
int	dump_arpstruct(tfp, arpptr)
	FILE *tfp;
	struct arpreq *arpptr;
#elif !defined(_WIN95) && !defined(_WINNT)
int	dump_arpstruct(FILE *tfp, struct arpreq *arpptr)
#endif

#if defined(_WIN95) || defined(_WINNT)

#else
	{
	int	k;
	struct	sockaddr_in *sinptr;	/* for ip address */
	struct	sockaddr *sockptr;	/* for hardware address */

	/*  in <net/if_arp.h>
	struct	arpreq {
		struct	sockaddr arp_pa;	(protocol)
		struct	sockaddr arp_ha;	(hardware)
		int	arp_flags;
	}
	and in <sys/socket.h>
	struct	sockaddr  {
		u_short	sa_family;
		char	sa_data[14];  		(room for all socket
					types: in, dl, ipx ...)
	}	*/

	if ( (arpptr == (struct arpreq *)NULL) ||
	     (tfp == (FILE *)NULL) )
		return(-1);			/* insurance */

	fprintf(tfp,
	"\n  struct arpreq { ");
					/* set protocol pointer */
	sinptr = (struct sockaddr_in *)&(arpptr->arp_pa);

	fprintf(tfp,
	"\n    struct sockaddr arp_pa { ");
	if ((sinptr->sin_family) > MAX_AF_NAME)	/* print address family */
		k = MAX_AF_NAME;
	else
		k = sinptr->sin_family;
	fprintf(tfp,
	"\n         short   sin_family(=%s);", address_family[k]);

						/* print port number (or
				0 = "to be assigned by system" */
	fprintf(tfp,
	"\n         u_short sin_port(=%hu);", ntohs(sinptr->sin_port));

						/* print ip address */
	fprintf(tfp,
	"\n         struct  in_addr sin_addr.s_addr(=%u=%d.%d.%d.%d);",
			ntohl(sinptr->sin_addr.s_addr),
			(ntohl(sinptr->sin_addr.s_addr) & 0xff000000) >> 24,
			(ntohl(sinptr->sin_addr.s_addr) & 0x00ff0000) >> 16,
			(ntohl(sinptr->sin_addr.s_addr) & 0x0000ff00) >>  8,
			(ntohl(sinptr->sin_addr.s_addr) & 0x000000ff));
	fprintf(tfp,
	"\n         char    sin_zero[8](=%x %x %x %x %x %x %x %x);",
			vaxunsign(sinptr->sin_zero[0]),
			vaxunsign(sinptr->sin_zero[1]),
			vaxunsign(sinptr->sin_zero[2]),
			vaxunsign(sinptr->sin_zero[3]),
			vaxunsign(sinptr->sin_zero[4]),
			vaxunsign(sinptr->sin_zero[5]),
			vaxunsign(sinptr->sin_zero[6]),
			vaxunsign(sinptr->sin_zero[7]));
	fprintf(tfp,
	"\n    } ");
					/* set hardware pointer */
	sockptr = (struct sockaddr *)&(arpptr->arp_ha);
	fprintf(tfp,
	"\n    struct sockaddr arp_ha { ");
	if ((sockptr->sa_family) > MAX_AF_NAME)/* print address family */
		k = MAX_AF_NAME;
	else
		k = sockptr->sa_family;
	fprintf(tfp,
	"\n         short   sa_family(=%s);", address_family[k]);
	fprintf(tfp,
	"\n         char    sa_data[6](=%2.2x-%2.2x-%2.2x-%2.2x-%2.2x-%2.2x);",
			vaxunsign(sockptr->sa_data[0]),
			vaxunsign(sockptr->sa_data[1]),
			vaxunsign(sockptr->sa_data[2]),
			vaxunsign(sockptr->sa_data[3]),
			vaxunsign(sockptr->sa_data[4]),
			vaxunsign(sockptr->sa_data[5]));
	fprintf(tfp,
	"\n    } ");
				/* show arp flags.. */
	fprintf(tfp,
	"\n         int     arp_flags=(0x%4x);", arpptr->arp_flags);
				/* ..and decode arp flags */
	if ((arpptr->arp_flags) & ATF_COM)
		fprintf(tfp, " COMPLETE");
	if ((arpptr->arp_flags) & ATF_PERM)
		fprintf(tfp, " PERM");
#if defined(__alpha) && defined(__linux)

#else
	if ((arpptr->arp_flags) & ATF_INUSE)
		fprintf(tfp, " INUSE");
	if ((arpptr->arp_flags) & ATF_STALE)
		fprintf(tfp, " STALE");
	if ((arpptr->arp_flags) & ATF_USE802)
		fprintf(tfp, " USE802");
	if ((arpptr->arp_flags) & ATF_DEAD)
		fprintf(tfp, " DEAD");
#endif
	if ((arpptr->arp_flags) & ATF_USETRAILERS)
		fprintf(tfp, " USETRAIL");

	fprintf(tfp, ";");
	fprintf(tfp,
	"\n  } ");
	fflush(tfp);
	return(0);
	}  /* end of dump_arpstruct */
#endif

/*
 * -----------------------------------------------------------------
 * dump_icmp: diagnostic dump of an ICMP packet
 * -----------------------------------------------------------------
 */

#if defined(_NOPROTO)
	dump_icmp(tfp, packetbuf, st_icmp, maxbytes, plhead)
	FILE *tfp;
	char *packetbuf;
	int  st_icmp, maxbytes, *plhead;
#else
void	dump_icmp(FILE *tfp, char *packetbuf, int st_icmp, 
		int maxbytes, int *plhead)
#endif

		/* this function dumps to the FILE *tfp, the
	decoded ICMP_Header of an ICMP_frame, given the
	following input:
		packetbuf = a character buffer
		st_icmp   = the starting offset of the ICMP_frame 
			    in packetbuf[]
		maxbytes  = absolute maximum number in ICMP_frame this
			    should be AT LEAST 8 (if UNKNOWN this should 
			    allow for the RFC maximum of 65536 - 60 - 8)  
	It returns (by reference):
		*plhead   = MIN length of the ICMP_header. */
	{
	int	i;
	unsigned char	*cptr;

	cptr = (unsigned char *)&packetbuf[st_icmp];

	*plhead = 4;		/* MIN ICMP_Header is always 4 bytes */

	fprintf(tfp,
	  "\n  %2.2d         type      code        chksum \n", *plhead);
	fprintf(tfp,"  ICMP_H ");
	fprintf(tfp,
	  "      %2.2x          %2.2x        %2.2x%2.2x",
		cptr[0], cptr[1], cptr[2], cptr[3]);

				/* EXT ICMP_Header is 4 more bytes */
	fprintf(tfp,
	  "\n  %2.2d        ident      sequence \n", 4);
	fprintf(tfp,"  ICMP_EH");
	fprintf(tfp,
	  "    %2.2x%2.2x        %2.2x%2.2x",
		cptr[4], cptr[5], cptr[6], cptr[7]);

	fprintf(tfp,
	  "\n  %2.2d             \n", maxbytes - 8);
	fprintf(tfp,"  ICMP_DATA ");
	for (i = 8 ; i < maxbytes ; i++)
		{
		if ((i % 8) == 0)
			fprintf(tfp, "\n  ");
		fprintf(tfp, "%2.2x ", cptr[i]);
		}

	fflush(tfp);
	}  /* end of dump_icmp */

/*
 * -----------------------------------------------------------------
 * dump_hexbytes: diagnostic hex-dump
 * -----------------------------------------------------------------
 */

static	char	line[16];	/* these will be unsigned but that
			keyword causes problems on the VAX */
static	int	dhindex = 0;
static	int	linenum = 0;

#if defined(_NOPROTO)
int dump_hexbytes(tfp, buf, bufnum, saveplace)
	FILE	*tfp;
	char	buf[];
	int	bufnum, saveplace;
#else
int dump_hexbytes(FILE *tfp, char buf[], int bufnum, int saveplace)
#endif
		/* This function is a typical hex-dump of buffer buf[]
	which also shows any ascii characters in the range [32, 126]
	at the end of the line. If saveplace == 1 the routine starts
	where it left off, otherwise it starts at column zero. */
	{
	int	i, j;

	if (bufnum <= 0)
		return(-1);	/* bad parameter */

	for (i = 0 ; i < bufnum ; i++)
		{
		if (dhindex == 0) 
			{
			fprintf(tfp, "\n%4.4X  ",
				(linenum << 4));
			linenum++;
			}
		line[dhindex] = buf[i];
		fprintf(tfp, "%2.2x ", vaxunsign(buf[i]));
		dhindex++;
		if (dhindex >= 16)
			{
			fprintf(tfp, "  ");
			for (j = 0 ; j < 16 ; j++)
				if ( (vaxunsign(line[j]) >= 32) &&
				     (vaxunsign(line[j]) <= 126) )
					fprintf(tfp, "%c", line[j]);
				else
					fprintf(tfp, ".");
			dhindex = 0;
			}
		}  /* end of for */

	fflush(tfp);
	if (saveplace == 0)
		{
		for (j = dhindex ; j < 16 ; j++)
			fprintf(tfp, "   ");	/* skip to ascii */
		fprintf(tfp, "  ");
		for (j = 0 ; j < dhindex ; j++)
			if ( (vaxunsign(line[j]) >= 32) &&
			     (vaxunsign(line[j]) <= 126) )
				fprintf(tfp, "%c", line[j]);
			else
				fprintf(tfp, ".");

		dhindex = 0;	/* reset position */
		linenum = 0;
		fflush(tfp);
		}
	return(0);
	}  /* end of dump_hexbytes */

