
/*
 * cleanup.c:	this code tries to reconcile network setup differences
 *		between Windows NT with Winsock 1.1/2.0 and (the MUCH
 *		more straightforward) Unix . Main modules will require
 *
 *			#include "header.h"
 *			#include "startup.h"
 *			...
 *			main() (
 *			... declarations ...
 *			#include "startup.c"
 *			...
 *			#include "cleanup.c"
 *			return(0);
 *			} ... end of main
 *		Non-main modules will only require "header.h"
 *
 *						Marc Thomas
 *
 */

#if defined(_WINNT) || defined(_WIN95)

		/* Release the DLL */

	if (WSACleanup() == SOCKET_ERROR)
		{

		fprintf(stderr,
		"\nWSACleanup() error: %d \n", WSAGetLastError());
		fflush(stderr);
		}
#endif

