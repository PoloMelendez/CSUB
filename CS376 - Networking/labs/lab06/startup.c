
/*
 * startup.c:	this code tries to reconcile network setup differences
 *		between Windows NT with Winsock 1.1/2.0 and (the MUCH
 *		more straightforward) Unix . 
 * 
 * Main modules will usually require
 *
 *			#include <stdio.h>
 *			#include <stdlib.h>
 *			#include <string.h>
 *			#include <ctype.h>
 *			#include <sys/types.h>
 *			#include <time.h>
 *
 * Main modules will ALWAYS require
 *
 *			#include "header.h"
 *			#include "startup.h"
 *
 *			...
 *
 *			main() (
 *			... declarations ...
 *			#include "startup.c"
 *
 *			...
 *
 *			#include "cleanup.c"
 *			return(0);
 *			} ... end of main
 *
 * Non-main modules will ALWAYS require
 *
 *			#include "header.h"
 *
 *						Marc Thomas
 *
 */

	char	*us_cptr;
	struct	hostent	*husp;

#if defined(_WINNT) || defined(_WIN95)

/* --- Winsock Startup ------------------------------------------------ */

	WSADATA	wsaData;

		/* Next line is a *** MUST *** with WINSOCK on WinNT */

	if (WSAStartup(MAKEWORD(2,1), &wsaData) != 0)
		{
		fprintf(stderr, "\nWSAStartup() failed. \n");
		fflush(stderr);
		exit(-1);
		}

/* --- Winsock Startup Done ------------------------------------------- */

		/* check ENVIRONMENT for "windir" which SHOULD always
		be present on Windows NT as a "system variable" */
	if (getenv("windir") == (char *)NULL)
		{
		/* set up path to BIN directory using DEF's */
		sprintf(net_bin_directory, "%s", DEF_NET_BIN_DIRECTORY);

		/* set up path to ETC directory using DEF's */
		sprintf(net_etc_directory, "%s", DEF_NET_ETC_DIRECTORY);
		}
	else
		{
		/* set up path to BIN directory */
		sprintf(net_bin_directory, "%s\\system32", 
			getenv("windir"));

		/* set up path to ETC directory */
		sprintf(net_etc_directory, "%s\\system32\\drivers\\etc",
			getenv("windir"));
		}
	sprintf(networks_path, "%s\\networks", net_etc_directory);
	sprintf(resolv_conf_path, "%s\\resolv.con", net_etc_directory);

#else		/* Unix */

	sprintf(net_bin_directory, "%s", DEF_NET_BIN_DIRECTORY);
	sprintf(net_etc_directory, "%s", DEF_NET_ETC_DIRECTORY);
	sprintf(networks_path, "%s/networks", net_etc_directory);
	sprintf(resolv_conf_path, "%s/resolv.conf", net_etc_directory);

#endif
		/* Winsock and New Solaris do NOT support getdomainname() 
	which is really an old Yellow Pages call (although one finds it 
	on most Unix operating systems) - so we use 
	method1: call gethostname() and parse.. but one problem with
		 some Linuxes is that us=hostname.domain but 
		 us_cptr=hostname only */

	gethostname(us, sizeof(us));	/* put who WE are in us[] */
	husp = gethostbyname(us);	/* query DNS services */
	if ( (husp != (struct hostent *)NULL) &&
			(husp->h_name != NULL) )
		{
		us_cptr = husp->h_name;
		while ( (*us_cptr != '\0') && (*us_cptr != '.') )
			us_cptr++;

		if (*us_cptr != '.')	/* gethostname() split off the
				domain, try us[] instead */
			{
			us_cptr = &us[0];
			while ( (*us_cptr != '\0') && (*us_cptr != '.') )
				us_cptr++;
			}
			
		if (*us_cptr == '.')	/* if this fails we have an
				empty us_subdomain[] */
			{
			us_cptr++;	/* put the subdomain WE are
					in in us_subdomain[] */
			strcpy(us_subdomain, us_cptr);
		us_inet32 = ntohl(*(UINET_32 *)(husp->h_addr_list[0]));
			sprintf(us_ipstring, 
			  "%d.%d.%d.%d", 
			  (int)(((us_inet32) & 0xff000000) >> 24),
			  (int)(((us_inet32) & 0x00ff0000) >> 16),
			  (int)(((us_inet32) & 0x0000ff00) >>  8),
			  (int)(((us_inet32) & 0x000000ff)));
		if (((us_inet32) & 0xe0000000) == 0x00000000)
				{
				strcpy(us_netclass, "class A network");
				sprintf(us_domainstr, 
				  "%d", 
			  (int)(((us_inet32) & 0xff000000) >> 24));
				us_netmask32 = 0x0ff000000;
				}
		else if (((us_inet32) & 0xe0000000) == 0x80000000)
				{
				strcpy(us_netclass, "class B network");
				sprintf(us_domainstr, 
				  "%d.%d", 
			  (int)(((us_inet32) & 0xff000000) >> 24),
			  (int)(((us_inet32) & 0x00ff0000) >> 16));
				us_netmask32 = 0x0ffff0000;
				}
		else if (((us_inet32) & 0xe0000000) == 0xb0000000)
				{
				strcpy(us_netclass, "class C network");
				sprintf(us_domainstr, 
				  "%d.%d.%d", 
			  (int)(((us_inet32) & 0xff000000) >> 24),
			  (int)(((us_inet32) & 0x00ff0000) >> 16),
			  (int)(((us_inet32) & 0x0000ff00) >>  8));
				us_netmask32 = 0x0ffffff00;
				}
			else
				{
				strcpy(us_netclass, "class ? network");
				us_domainstr[0] = '\0';
				us_netmask32 = 0x0ffffffff;
				}
			}  /* end of if (*us_cptr == '.') */
		else
			/* hostname did NOT have domainname in it AND
			gethostbyname only gave the hostname, (e.g. "sumatra" 
			instead of "sumatra.cs.csubak.ed" */
			{
#if defined(_WINNT) || defined(_WIN95) || defined(__vax) \
	|| defined(__mips) || defined(__alpha)

			us_subdomain[0] = '\0';	/* failure */

#elif defined(__sunos)
				/* No getdomainname() on New Solaris 
				Strange, they invented the call. */
			us_subdomain[0] = '\0';	/* failure */

#else			/* Linux */
				/* method2: call getdomainname() */
			getdomainname(us_subdomain, sizeof(us_subdomain));

				/* use previous husp .. */
		us_inet32 = ntohl(*(UINET_32 *)(husp->h_addr_list[0]));
			sprintf(us_ipstring, 
			  "%d.%d.%d.%d", 
			  (int)(((us_inet32) & 0xff000000) >> 24),
			  (int)(((us_inet32) & 0x00ff0000) >> 16),
			  (int)(((us_inet32) & 0x0000ff00) >>  8),
			  (int)(((us_inet32) & 0x000000ff)));
		if (((us_inet32) & 0xe0000000) == 0x00000000)
				{
				strcpy(us_netclass, "class A network");
				sprintf(us_domainstr, 
				  "%d", 
			  (int)(((us_inet32) & 0xff000000) >> 24));
				us_netmask32 = 0x0ff000000;
				}
		else if (((us_inet32) & 0xe0000000) == 0x80000000)
				{
				strcpy(us_netclass, "class B network");
				sprintf(us_domainstr, 
				  "%d.%d", 
			  (int)(((us_inet32) & 0xff000000) >> 24),
			  (int)(((us_inet32) & 0x00ff0000) >> 16));
				us_netmask32 = 0x0ffff0000;
				}
		else if (((us_inet32) & 0xe0000000) == 0xb0000000)
				{
				strcpy(us_netclass, "class C network");
				sprintf(us_domainstr, 
				  "%d.%d.%d", 
			  (int)(((us_inet32) & 0xff000000) >> 24),
			  (int)(((us_inet32) & 0x00ff0000) >> 16),
			  (int)(((us_inet32) & 0x0000ff00) >>  8));
				us_netmask32 = 0x0ffffff00;
				}
			else
			    	{
				fprintf(stderr,
		"\nstartup.c: warning: getdomainname() failed (with none)");
	  			fflush(stderr);
			   	strcpy(us_netclass, "class ? network");
			    	us_subdomain[0] = '\0';  /* (NO hope) failure */
				us_netmask32 = 0x0ffffffff;
			    	} /* end of if (_temp_ret != 0) */
#endif
			}  /* end of if (*us_cptr != '.') */
		}  /* end of if (husp != (struct hostent *)NULL) */
	else
		{
		us_subdomain[0] = '\0';		/* failure of method1 */
		}  /* end of if (husp == (struct hostent *)NULL) */

	_temp_fp = fopen(resolv_conf_path, "r");
	if (_temp_fp != (FILE *)NULL)
	  {
	  _temp_cptr = fgets(_temp_line, sizeof(_temp_line), _temp_fp);
	  _temp_index = 1;
	  us_nameserv_num = 0;

			/* parse /etc/resolv.conf for "nameserver" lines,
		and to (double)check us_subdomain: */
	  while ( (us_nameserv_num < MAX_NS) &&
			(_temp_cptr != (char *)NULL) )
		{
				/* break at "nameserver" note that
			Caldera uses '#' instead of ';' for commentline */
		if ( (_temp_line[0] == ';') || (_temp_line[0] == '#') ||
		   ( (strstr(_temp_line, "nameserver") == (char *)NULL) ) )
			{

			/* as long as we are parsing /etc/resolv.conf
			we may as well check for a "domain" line to
			see if there is agreement */

		    if ( (_temp_line[0] != ';') && (_temp_line[0] != '#') &&
		       ( (strstr(_temp_line, "domain") != (char *)NULL) ) )

				/* get what follows "domain" and
				compare with previous us_subdomain */
				{
				if (_temp_cptr != (char *)NULL)
					{
					/* skip "domain" */
					_temp_cptr = _temp_cptr + 
						sizeof("domain");
					while ( (*_temp_cptr != '\0') &&
						(*_temp_cptr != '\n') &&
						(*_temp_cptr != '.') &&
					  ( (*_temp_cptr < 'a') || 
						(*_temp_cptr > 'z') ) )
						{
						_temp_cptr++;
						}
					_temp_chrindex = 0;
					_temp_domain[_temp_chrindex] =
						*_temp_cptr;
					_temp_chrindex++;
					_temp_cptr++;
					while ( (*_temp_cptr != '\0') &&
						(*_temp_cptr != '\n') &&
			(_temp_chrindex < (sizeof(_temp_domain) - 1)) &&
					  ( (*_temp_cptr == '.') ||
						(*_temp_cptr >= 'a') || 
						(*_temp_cptr <= 'z') ) )
						{
						_temp_domain[_temp_chrindex] =
							*_temp_cptr;
						_temp_chrindex++;
						_temp_cptr++;
						}
					_temp_domain[_temp_chrindex] = '\0';

						/* check consistency but if
					    us_subdomain is empty just use
					    entry from /etc/resolv.conf */
					if ( (strlen(us_subdomain) == 0) &&
					     (strlen(_temp_domain) != 0) )
						{
					strcpy(us_subdomain, _temp_domain);
						}
					else if 
				(strcmp(us_subdomain, _temp_domain) != 0)
						{
						fprintf(stderr,
  "\nerror: us_subdomain=\"%s\" does not match", us_subdomain);
						fprintf(stderr,
  "\n       /etc/resolv.conf entry=\"domain %s\"\n", _temp_domain);
						}
						/* scrap rest of line */
					_temp_cptr = fgets(_temp_line, 
					     sizeof(_temp_line), _temp_fp);
					_temp_index++;
					}
				continue;
				}
			else
				{
					/* scrap the line */
				_temp_cptr = fgets(_temp_line, 
					sizeof(_temp_line), _temp_fp);
				_temp_index++;
				continue;
				}
			}		/* at end or found "nameserver" */

		if (_temp_cptr != (char *)NULL)
			{
					/* skip "nameserver" */
			_temp_cptr = _temp_cptr + sizeof("nameserver");
			while ( (*_temp_cptr != '\0') &&
				(*_temp_cptr != '\n') &&
				(*_temp_cptr != '.') &&
			  ( (*_temp_cptr < '0') || (*_temp_cptr > '9') ) )
				_temp_cptr++;
			sprintf(&nameserver_ipstring[us_nameserv_num][0], 
				"%-15.15s", _temp_cptr);
			nameserver_ipstring[us_nameserv_num][15] = '\0';
			_temp_cptr = (char *)
				&nameserver_ipstring[us_nameserv_num][0];
			while ( (*_temp_cptr != '\0') &&
				(*_temp_cptr != '\n') &&
				( (*_temp_cptr == '.') ||
			  ( (*_temp_cptr >= '0') && (*_temp_cptr <= '9') ) ) )
				_temp_cptr++;
			*_temp_cptr = '\0';
			}
		else
			{
			break;
			}
	
		_temp_d4 = 0;
		_temp_cptr = &nameserver_ipstring[us_nameserv_num][0];
		while ((*_temp_cptr != '.') && (*_temp_cptr != '\0') &&
			(*_temp_cptr >= '0') && (*_temp_cptr <= '9') )
			{
			_temp_d4 = _temp_d4 * 10;
			_temp_d4 = _temp_d4 + ( (int)(*_temp_cptr) - (int)'0' );
			_temp_cptr++;
			}
		if (*_temp_cptr != '.')
			{
			fprintf(stderr, 
			"\nnameserver_ipstring=\"%s\" is bad (line=%d). \n",
				&nameserver_ipstring[us_nameserv_num][0],
				_temp_index);
			fflush(stderr);
			_temp_index++;
			continue;
			}
		_temp_cptr++;		/* skip '.' */
		_temp_d3 = 0;
		while ((*_temp_cptr != '.') && (*_temp_cptr != '\0') &&
			(*_temp_cptr >= '0') && (*_temp_cptr <= '9') )
			{
			_temp_d3 = _temp_d3 * 10;
			_temp_d3 = _temp_d3 + ( (int)(*_temp_cptr) - (int)'0' );
			_temp_cptr++;
			}
		if (*_temp_cptr != '.')
			{
			fprintf(stderr, 
			"\nnameserver_ipstring=\"%s\" is bad (line=%d). \n",
				&nameserver_ipstring[us_nameserv_num][0],
				_temp_index);
			fflush(stderr);
			_temp_index++;
			continue;
			}
		_temp_cptr++;		/* skip '.' */
		_temp_d2 = 0;
		while ((*_temp_cptr != '.') && (*_temp_cptr != '\0') &&
			(*_temp_cptr >= '0') && (*_temp_cptr <= '9') )
			{
			_temp_d2 = _temp_d2 * 10;
			_temp_d2 = _temp_d2 + ( (int)(*_temp_cptr) - (int)'0' );
			_temp_cptr++;
			}
		if (*_temp_cptr != '.')
			{
			fprintf(stderr, 
			"\nnameserver_ipstring=\"%s\" is bad (line=%d). \n",
				&nameserver_ipstring[us_nameserv_num][0],
				_temp_index);
			fflush(stderr);
			_temp_index++;
			continue;
			}
		_temp_cptr++;		/* skip '.' */
		_temp_d1 = 0;
		while ((*_temp_cptr != '.') && (*_temp_cptr != '\0') &&
			(*_temp_cptr >= '0') && (*_temp_cptr <= '9') )
			{
			_temp_d1 = _temp_d1 * 10;
			_temp_d1 = _temp_d1 + ( (int)(*_temp_cptr) - (int)'0' );
			_temp_cptr++;
			}	
		if (*_temp_cptr != '\0')
			{
			fprintf(stderr, 
			"\nnameserver_ipstring=\"%s\" is bad (line=%d). \n",
				&nameserver_ipstring[us_nameserv_num][0],
				_temp_index);
			fflush(stderr);
			_temp_index++;
			continue;
			}
		nameserver_inet32[us_nameserv_num] = 
			(_temp_d4 << 24) | (_temp_d3 << 16) | 
				(_temp_d2 << 8) | _temp_d1;

			/* OK - nameserver is valid */
		us_nameserv_num++;
		_temp_index++;
	  	_temp_cptr = fgets(_temp_line, sizeof(_temp_line), _temp_fp);
		}  /* end of if (_temp_fp != (FILE *)NULL) */
	  if (_temp_fp != (FILE *)NULL)
		fclose(_temp_fp);

	  }  /* end of if (_temp_fp != (FILE *)NULL) */
	else
	  {
	  fprintf(stderr,
		"\nstartup.c: warning: %s not found\n", resolv_conf_path);
	  fflush(stderr);
	  }

