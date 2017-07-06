
/*
 * startup.h:	These should be included ONLY in the main module
 *
 *					Marc Thomas
 */

			/* Utility variables for startup.c */
FILE	*_temp_fp;
char	*_temp_cptr;
int	_temp_index;
int	_temp_chrindex;
char	_temp_line[128];
char	_temp_domain[128];
UINET_32	_temp_d1, _temp_d2, _temp_d3, _temp_d4;

			/* Next is path to network "bin" directory
		for: arp, netstat, nbtstat, hostname, nslookup, lpq,
		     ipconfig, ... (processed by startup.c) */
char	net_bin_directory[128];
			/* Next is path to network "etc" dirctory
		for: hosts and lmhosts (processed by startup.c) */
char	net_etc_directory[128];

			/* Next is full path to "networks" */
char	networks_path[128];
			/* Next is full path to "resolv.conf" */
char	resolv_conf_path[128];
#define	MAX_NS	8
int	us_nameserv_num;
					/* array of listed nameservers */
char	nameserver_ipstring[MAX_NS][16];
UINET_32 nameserver_inet32[MAX_NS];	/* in host byte order */

char	us[128];		/* our machine's hostname e.g. 
				   "winntbutthead" */
char	us_subdomain[128];	/* our machine's symbolic subdomain, e.g.
				   "cs.csubak.edu" */
char	us_domainstr[16];	/* our machine's domain as string, e.g.
				   "136.168" */
char	us_netclass[32];	/* string describing network class, e.g.
				   "class B network" */
UINET_32	us_inet32;	/* our machine's ip-address as 32-bit
				   unsigned int in host byte order */
UINET_32	us_netmask32;	/* our machines's netmask as 32-bit */
char	us_ipstring[16];	/* our machine's ip-address as dotted
				   decimal string, e.g. "136.168.201.251" */

