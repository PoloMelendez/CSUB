
/* --- Special I/O Header --------------------------------------------- */

		/* specio.h: this is used to reconcile the differences
		between the listed systems with regard to special
		terminal/socket handling 
						Marc Thomas	*/

#if defined(__alpha)

#include <sys/termios.h>
#include <sys/ioctl.h>
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */
#define READ_TERMIOS TCGETS
#define WRITE_TERMIOS TCSETS

#include <arpa/telnet.h>	/* only needed for arbitration
				   with a REAL telnetd */
#elif defined(__sun)

#include <sys/termios.h>
#include <sys/ioctl.h>
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */
#define READ_TERMIOS TCGETS
#define WRITE_TERMIOS TCSETS

#include <arpa/telnet.h>	/* only needed for arbitration
				   with a REAL telnetd */
#elif defined(__linux)		/* RedHAT 5.x Linux */

#include <sys/termios.h>
#include <sys/ioctl.h>
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */
#define READ_TERMIOS TCGETS
#define WRITE_TERMIOS TCSETS

#include <arpa/telnet.h>	/* only needed for arbitration
				   with a REAL telnetd */

#elif defined(__vax) || defined(__mips)	/* ULTRIX OS */

#include "telnet.h"		/* More up-to-date */
#include <sys/termios.h>
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */
#define READ_TERMIOS TCGETP
#define WRITE_TERMIOS TCSANOW

#elif defined(_WIN95) || defined(_WINNT)

#include <io.h>
#include <conio.h>		/* for _kbhit(), _getch() */

char	cmd_title[128];		/* current cmd.exe title which
			shows window size and current position */
COORD	save_position =
	{ 0, 0 };
COORD	last_position =
	{ 0, 0 };
int	last_margin_top = 1;
int	last_margin_bot = 25;

CONSOLE_SCREEN_BUFFER_INFO	screen_bufinfo;
SMALL_RECT			screen_win;

				/* Damn Microsoft Visual C++ has
			dropped settextposition() AND all ansi-type
			calls. We don't want to include wincon.h, or
			mapiutil.h so we (re)define the following 
			types */
typedef short	SHORT;
				/* UNDOCUMENTED externals: see the
			source directory "crt/src" ON THE CDROM. */
extern	int	_coninpfh;	/* created and valid as soon as
			_getch(), _cgets(), or kbhit() is called,
			otherwise it is -2 */
extern	int	_confh;		/* created and valid as soon as
			_putch(), or _cputs() is called,
			otherwise it is -2 */

				/* UNDOCUMENTED OS_CALLS */
void	__initconin();		
/* *** OLD ***
void	__initconout(); 	we now use my _my_initconout() with
			GENERIC_READ otherwise a number of kernel
			calls will FAIL */
				
	char	blank_line[] = "    "
"                                                                           ";
	char	blank_newline[] = "    "
"\n                                                                           ";

#define TERM_STRUCT termios
#define READ_TERMIOS TIOCGETP
#define WRITE_TERMIOS TIOCSETP

#define SIGQUIT SIGTERM		/* WinNT doesn't have SIG_QUIT */

#include "telnet.h"		/* More up-to-date */
/* *** OLD ***
  #include "arpa/telnet.h" */	/* *** OLDER *** */

#else				/* OSX/BSD 4.4 OS */

#include "telnet.h"		/* More up-to-date */
#include <sys/termios.h>
#include <sys/filio.h>		/* for FIONBIO */
#define	TERM_STRUCT termios	/* Berkeley UNIX new terminal struct */

/* *** OBSOLETE *** use tcsetattr() and tcgetattr() rather than ioctl()
   #define READ_TERMIOS TCGETS
   #define WRITE_TERMIOS TCSETS */

#endif				/* END of CONDITIONALS */

