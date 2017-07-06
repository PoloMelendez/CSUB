
/* --- Preamble Winsock/Unix Socket Header ---------------------------- */

	/* OUR HEADER define block tries to reconcile Windows NT with 
	winsock2.h with the Unix/Linux networking environment of <sys/ .h> 
	<net/ .h> and <netinet/ .h>. 

	For each machine type (__alpha, __sun, ..)  we define globals:

	UINET_32		a 32-bit unsigned integer
	L_ENDIAN/B_ENDIAN	set according to machine type, little
				  or big endian
	ULIMIT			bit width for select()
	CLOSE_SOCKET		the function call to close a socket (on
				  Unix: close(), on NT closesocket() 
	DEF_NET_BIN_DIRECTORY	directory which contains the network
				  binaries (for ping, netstat, etc.)
	DEF_NET_ETC_DIRECTORY	directory which contains the network
				  config files (e.g. resolv.conf, etc.)
	DEF_ROUTEFILE_PATH	default routes are usually in one of
				  following places: /etc/routes, 
				  /etc/defaultrouter, /proc/net/route,
				  or nonexistent
	PING_TIMES		ping command option to ping once

	We include additional header files for:

				struct timeval
				strings.h (if not included by string.h)
				filio.h (if not included by ioctl.h)
				sockio.h (if not included by ioctl.h)

	We make additional keyword defines IF they are missing from the 
	standard network and special I/O header files:

	typedef int	SOCKET; 	(to make Unix compatible with NT)

	#define	ATF_USE802	0x0020
	#define	ATF_STALE	0x0100
	#define	ATF_DEAD	0x0200

	struct termios {
		unsigned c_iflag; 	(input)
		unsigned c_oflag;	(output)
		unsigned c_lflag;	(line discipline)
		char	 c_cc[32];	(control chars)
	};

	We define _NOPROTO if we have an OLD C, non-ANSI compiler which
	doesn't like function prototypes (this should be retired):

	#define _NOPROTO	0	

	We finally make a number of additional defines for Windows NT 
	(which has very different run time libraries) and handle 
	some Unix/Linux differences.

						Marc Thomas	*/


				/* 64-bit Alpha processor: True64 Unix */

#if defined (__alpha) && !defined(__linux)	/* 64-bit processor */
#define	UINET_32	unsigned int	/* see <machine/machlimits.h> */
#define L_ENDIAN
#define	ULIMIT	255		/* for select() */
				/* Unix closes socket with "close" */
#define	CLOSE_SOCKET	close
typedef int	SOCKET; 

#include <sys/time.h>		/* for timeval */
#define	DEF_NET_BIN_DIRECTORY	"/usr/sbin"
#define	DEF_NET_ETC_DIRECTORY	"/etc"
#define DEF_ROUTEFILE_PATH	"/etc/routes"

#define	PING_TIMES	"-c1"


				/* Sun/Fujitsu Sparc Ultra: Solaris 7/8 */
#elif defined (__sun) 		
#define	UINET_32	unsigned int	/* see <machine/machlimits.h> */
#define B_ENDIAN
#define	ULIMIT	255		/* for select() */
				/* Unix closes socket with "close" */
#define	CLOSE_SOCKET	close
typedef int	SOCKET; 

#include <sys/time.h>		/* for timeval */
#include <strings.h>		/* Sun <string.h> does NOT automatically
				   include the bzero() functions of
				   <strings.h> */
#include <sys/filio.h>		/* Sun <ioctl.h> does NOT automatically
				   include the FIONBIO-type defines due
				   to its desire to be both System V
				   AND BSD */
#include <sys/sockio.h>		/* Sun <ioctl.h> does NOT automatically
				   include the SIOCGIFCONF-type defines
				   from sockio.h */
#define	DEF_NET_BIN_DIRECTORY	"/usr/sbin"
#define	DEF_NET_ETC_DIRECTORY	"/etc"
#define DEF_ROUTEFILE_PATH	"/etc/defaultrouter"
#define	PING_TIMES	"-c1"

#define	ATF_USE802	0x0020
#define	ATF_STALE	0x0100
#define	ATF_DEAD	0x0200


				/* Older DEC Vax/Mips: Ultrix & BSD4.3 */
#elif defined(__vax) || defined(__mips)	
#define	UINET_32	unsigned long
#define L_ENDIAN
#define	ULIMIT	255		/* for select() */
				/* Unix closes socket with "close" */
#define	CLOSE_SOCKET	close
typedef int	SOCKET; 

#include <sys/time.h>		/* for timeval */
#define	DEF_NET_BIN_DIRECTORY	"/usr/etc"
#define	DEF_NET_ETC_DIRECTORY	"/etc"
#define DEF_ROUTEFILE_PATH	""	/* none */
#define	PING_TIMES	"   "

#define _NOPROTO	0	

#define	ATF_USE802	0x0020
#define	ATF_STALE	0x0100
#define	ATF_DEAD	0x0200

				/* FreeBSD 4.4/MAC OSX/Darwin */
#elif defined(__bsd)
#define	UINET_32	unsigned int
#define B_ENDIAN
#define	ULIMIT	255		/* for select() */
				/* Unix closes socket with "close" */
#define	CLOSE_SOCKET	close
typedef int	SOCKET; 

#include <sys/time.h>		/* for timeval */
#define	DEF_NET_BIN_DIRECTORY	"/usr/sbin"
#define	DEF_NET_ETC_DIRECTORY	"/etc"
#define DEF_ROUTEFILE_PATH	""	/* none */
#define	PING_TIMES	"   "

#define	ATF_USE802	0x0020
#define	ATF_STALE	0x0100
#define	ATF_DEAD	0x0200

int	getdomainname(char *, int);	/* should be in unistd.h but is not */
#define SIGCLD	SIGCHLD

				/* Linux: Red Hat 5.x/6.x -- other Linuxes
				   will need minor modifications */
#elif defined(__linux)		
#define	UINET_32	unsigned
#define L_ENDIAN
#define	ULIMIT	255		/* for select() */
				/* Unix closes socket with "close" */
#define	CLOSE_SOCKET	close
typedef int	SOCKET; 

#include <sys/time.h>		/* for timeval */
#define	DEF_NET_BIN_DIRECTORY	"/bin"	/* for "ping" */
#define	DEF_NET_ETC_DIRECTORY	"/etc"
#define DEF_ROUTEFILE_PATH	"/proc/net/route"
#define	PING_TIMES	"-c1"

#define	ATF_INUSE	0x0001
#define	ATF_USE802	0x0020
#define	ATF_STALE	0x0100
#define	ATF_DEAD	0x0200

#define	AF_LINK		3
#define	AF_DLI		18


				/* Pentium: Windows NT 4.0 */
#elif defined(_WINNT) || defined(_WIN95)
#define	UINET_32	unsigned long
#define L_ENDIAN
			/* Next is path to network "bin" directory
		for: arp, netstat, nbtstat, hostname, nslookup, lpq,
		     ipconfig, ... (processed by startup.c) */
#define	DEF_NET_BIN_DIRECTORY	"d:\\winnt\\system32"

			/* Next is path to network "etc" dirctory
		for: hosts and lmhosts (processed by startup.c) */
#define	DEF_NET_ETC_DIRECTORY	"d:\\winnt\\system32\\drivers\\etc"
#define DEF_ROUTEFILE_PATH	""	/* none */

#define	ULIMIT	255		/* for select() */
				/* Winsock uses "closesocket" not "close" */
#define	CLOSE_SOCKET	closesocket
				/* NOTE: SOCKET descriptor is UNsigned in
				Winsock so that INVALID_SOCKET is ~0 and is
				NOT negative UNLESS you cast it as "int" 
typedef int	SOCKET;  	 */

				/* we DON'T actually use termios */
struct termios {
	unsigned c_iflag; 	/* (input) */
	unsigned c_oflag;	/* (output) */
	unsigned c_lflag;	/* (line discipline) */
	char	 c_cc[32];	/* (control chars) */
};


				/* Generic Unix: e.g. SunOS 4.1.3 */
#else 				
#define	UINET_32	unsigned long
#define B_ENDIAN
#define	ULIMIT	255		/* for select() */
#include <sys/time.h>		/* for timeval */
#define	DEF_NET_BIN_DIRECTORY	"/usr/etc"
#include <netinet/ip_var.h>
#include <nlist.h>

#define	ATF_USE802	0x0020
#define	ATF_STALE	0x0100
#define	ATF_DEAD	0x0200

#define _NOPROTO	0	/* in case we have the old Sun compiler */

#endif


			/* We now handle operating system differences between 
			the macrokernel Unix (which contains the network code)
			and the less powerful microkernel NT (which requires
			a host of dll's to to networking, and which lacks
			global descriptors for sockets) */
#if defined(_WINNT) || defined(_WIN95) /* Windows NT */

#pragma pack(4)
#define WIN32_LEAN_AND_MEAN

#include <process.h>
#include <winsock2.h>

			/* Windows uses "memxxx" rather than "bxxx" */
#define bcopy(s,d,n)	memcpy((d),(s),(n))
#define	bcmp(s1,s2,n)	memcmp((s1),(s2),(n))
#define	bzero(s,n)	memset((s),0,(n))

			/* No sleep() so we create a macro */
#define	sleep(x) 	Sleep((x) * 1000);

			/* No kill() so we create a macro. Note
			that threads can ONLY send signals to their
			OWN processes */
#define	kill(x,y)	raise(y);

#include "netinet/in_systm.h"

/*
 * Next is from Unix <netinet/in.h> but is LEFT OUT OF WINSOCK
 *
 * Structure of an internet header (LITTLE ENDIAN), naked of options.
 * We declare ip_len and ip_off to be short, rather than u_short
 * pragmatically since otherwise unsigned comparisons can result
 * against negative integers quite easily, and fail in subtle ways.
 *
 */

#define	IPVERSION	4
struct ip {
	u_char	ip_hl:4,		/* header length */
		ip_v:4;			/* version */
	u_char	ip_tos;			/* type of service */
	short	ip_len;			/* total length */
	u_short	ip_id;			/* identification */
	short	ip_off;			/* fragment offset field */
#define	IP_DF 0x4000			/* dont fragment flag */
#define	IP_MF 0x2000			/* more fragments flag */
	u_char	ip_ttl;			/* time to live */
	u_char	ip_p;			/* protocol */
	u_short	ip_sum;			/* checksum */
	struct	in_addr ip_src,ip_dst;	/* source and dest address */
};

#define	IP_MAXPACKET	65535		/* maximum packet size */
#include "netinet/ip_icmp.h"

#else				/* Unix */

#include <unistd.h>		/* for gethostname() */

#include <sys/param.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/if_ether.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <arpa/inet.h>		/* for inet_addr() */

#endif

				/* Lastly we have some minor  diff's between 
				Unix & linux: */
#if defined(__alpha) && defined(__linux)


#elif defined(__alpha) || defined(__mips) || defined(__vax) || defined(__sun)

#include <netinet/ip_var.h>
#include <nlist.h>
#elif defined(_WINNT) || defined(_WIN95) || defined(__linux)


#else				/* generic Unix */

#include <netinet/ip_var.h>
#include <nlist.h>
#endif


				/* Common Global variables/defines */

				/* eternal thanks to damn Vax SIGNED chars */
#define	vaxunsign(x)	((unsigned)x & 0xff)

  /* ALL *** FOLLOWING *** variables are SET by code: startup.c */

extern	char	networks_path[128];	/* full path to "networks" */
extern	char	resolv_conf_path[128];	/* full path to "resolv.conf" */

#define	MAX_NS	8
extern	int	us_nameserv_num;
					/* array of listed nameservers */
extern	char	nameserver_ipstring[MAX_NS][16];
extern	UINET_32 nameserver_inet32[MAX_NS];

extern	char	us[128];		/* our machine's hostname */
extern	char	us_subdomain[128];	/* our machine's subdomain */
extern	char	us_domainstr[16];	/* our machine's domain as string */
extern	char	us_netclass[32];	/* network class */
extern	UINET_32	us_inet32;	/* our machine's ip-address */
extern	char	us_ipstring[16];	/* our machine's string ip-address */

/* --- End Preamble Header -------------------------------------------- */

