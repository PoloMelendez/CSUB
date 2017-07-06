

#if defined(_NOPROTO)

int	dump_hostent();
int	dump_sockaddr();
void	dump_icmp();
int	dump_hexbytes();

#else

int	dump_hostent(FILE *tfp, struct hostent *hp);
int	dump_sockaddr(FILE *tfp, struct sockaddr_in *sinptr);
void	dump_icmp(FILE *tfp, char *packetbuf, int st_icmp, 
		int maxbytes, int *plhead);
int	dump_hexbytes(FILE *tfp, char buf[], int bufnum, int saveplace);
#endif

	/* Next are UNIX only procedures since there is no API in
	Winsock1.1 or Winsock2.0 DLL's to the ARP-cache */

#if defined(_NOPROTO) && !defined(_WIN95) && !defined(_WINNT)

int	dump_arpstruct();
#elif !defined(_WIN95) && !defined(_WINNT)

int	dump_arpstruct(FILE *tfp, struct arpreq *arpptr);
#endif
