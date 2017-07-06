
				/* This program tests the terminal
		utility routines which support raw, cbreak, and cooked 
		modes:
				
		  int raw_noecho(void) 
		  int raw_cbreak(void)
		  int fionbio_select(void)
		  int echo_noraw(void)
		  int restore_orig(void)	

		  int show_settings(FILE *fp, struct TERM_STRUCT *ptsave);

		also tested is the use of a select loop. You should try
		entering various characters, CTRL-C, CTRL-Q, CTRL-Z (which 
		if not ignored will require "fg %1" to resume job), etc. 
		This program originally used ioctl()'s for terminal control 
		but now uses the newer POSIX standards: 
		
		  tcgetattr() 
		  tcsetattr()

		for example, older usage:

		  ioctl(0, READ_TERMIOS, &tstuff); 

		is replaced by POSIX:

		  tcgetattr(STDIN_FILENO, &tstuff);

						 Marc */
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#define SELECT_TIMEOUT 30

/* ---------------------------------------------------------------- */

#include "header.h"	/* OUR header which handles MOST of the
			WinSock/32-bit Unix/64-bit Unix Differences */
#include "startup.h"	/* globals for main module set by the
			code startup.c */
#include "specio.h"	/* OUR header file for routines and defines
			for special terminal/socket I/O */
#include "diagnost.h"	/* OUR dump_xxxx() routines and other
			diagnostics */

/* ----------------------------------------------------------------- */

				/* global variables */
				/* note that STDIN_FILENO is defined
			in unistd.h as 0 (not unexpectedly) */
int ctrlc_hit = 0;		/* set by ctrlc_hander() */

struct	TERM_STRUCT tsave;	/* raw_noecho(),... etc. save settings */
struct	TERM_STRUCT toriginal;	/* raw_noecho(),... etc. original settings */
int	termsave_done = 0;	/* set when ORIGINAL settings SAVED */
int	current_mode =  0;	/* ECHO_NORAW_MODE */
#define	ECHO_NORAW_MODE	  0
#define	CBREAK_MODE	  1
#define	RAW_NOECHO_MODE	  2
#define	SELECT_MODE	  3
#define NOECHO_NORAW_MODE 4
				/* forward references */

void ctrlc_handler(int dummy);
int show_settings(FILE *fp, struct TERM_STRUCT *ptsave);
int raw_noecho(void); 
int raw_cbreak(void);
int fionbio_select(void);
int echo_noraw(void);
int restore_orig(void);

/* ------------------------------------------------------------------------ */

int	main(int argc, char *argv[], char *envp[])
	{
	int	i, bytesread, byteswritten, tmout, khar;
	struct TERM_STRUCT tstuff;
	char	buf[256], chrline[128];
	fd_set	readmask, writemask, exceptmask;
	struct	timeval	timeout;	/* struct timeval is def'd as
		long	tv_sec;		   seconds, and
		long	tv_usec;	   micro-seconds */

#include "startup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix startup. Main modules
			ALSO require the inclusion of 
				header.h
				startup.h */

	printf("current settings: \n");	
	tcgetattr(STDIN_FILENO, &tstuff);
	show_settings(stdout, &tstuff);	/* show current settings */
	signal(SIGINT, ctrlc_handler);	/* set CTRL-C handler */

	raw_cbreak();		/* enter and test CBREAK mode
				   note that CTRL-C still sends SIGINT
				   but other control chars are disabled */
	ctrlc_hit = 0;	
	printf("\nraw_cbreak: getchar(): ");
	khar = getchar();	/* getchar() in CBREAK mode */
	if (ctrlc_hit)
		{
		printf("\r\n");	/* SIGNAL interrupts I/O */
		fflush(stdout);
		ctrlc_hit = 0;		/* reset */
		}
	else 
		{
		printf("character code=%2.2Xh \r\n", khar);
		fflush(stdout);
		}
	tcgetattr(STDIN_FILENO, &tstuff);
	show_settings(stdout, &tstuff);

	raw_noecho();		/* enter and test RAW mode with NO ECHO
				   note that CTRL-C, CTRL-Q, CTRL-Z ...
				   have NO special action. */
	printf("\nraw_noecho: getchar(): ");
	khar = getchar();
	printf("character code=%2.2Xh \r\n", khar);
	tcgetattr(STDIN_FILENO, &tstuff);
	show_settings(stdout, &tstuff);

	fionbio_select();	 /* set non-blocking I/O on stdin */
	printf("\nfionbio_select: read(): [end with cr]: ");
	fflush(stdout);
				 /* do I/O test with a SELECT loop */
selectloop:
	bzero(buf, sizeof(buf));
	FD_ZERO(&readmask);
	FD_ZERO(&writemask);
	FD_ZERO(&exceptmask);
	FD_SET(0, &readmask);		/* activiate stdin read */
	FD_SET(0, &exceptmask);		/* activiate stdin exception */
	timeout.tv_sec = SELECT_TIMEOUT;
	timeout.tv_usec = 0;
	tmout = select(ULIMIT, &readmask, &writemask, &exceptmask,
		&timeout);
	if (tmout == 0)
		{
		printf(" timeout! \r\n");
		fflush(stdout);
		}
	else if (tmout < 0)
		{
		if (errno == EINTR)
			{
			printf(" signal interrupted select \r\n continue: ");
			fflush(stdout);
			goto selectloop;
			}
		else
			{
			printf(" exception on select! \r\n");
			fflush(stdout);
			}
		}
	else if(FD_ISSET(0, &exceptmask))
		{
		printf(" exception on stdin! \r\n");
		fflush(stdout);
		}
	else if(FD_ISSET(0, &readmask))
		{
		bytesread = read(0, buf, sizeof(buf));
		byteswritten = write(1, buf, bytesread);
		i = 0;
		while (buf[i] != '\0')
			{
			if ( (buf[i] == '\r')  ||  (buf[i] == '\n') )
				goto endselect;
			i++;
			}
		goto selectloop;
		}
endselect:
	printf("\r\n");

	tcgetattr(STDIN_FILENO, &tstuff);
	show_settings(stdout, &tstuff);

	echo_noraw();		/* return to COOKED (LINE-EDITED) mode */
	printf("\necho_noraw: gets() [end with cr]: ");

/* *** OLD *** gets() is dangerous
	gets(chrline);		even though it converts '\n' to '\0' */

	fgets(chrline, sizeof(chrline) - 1, stdin);
	for (i = 0 ; i < sizeof(chrline) - 1 ; i++)
		{
		if ( (chrline[i] == '\n') || (chrline[i] == '\0') )
			break;
		}
	if (chrline[i] == '\n')
		chrline[i] = '\0'; 	/* get rid of newline */

	if (ctrlc_hit)
		{
		printf("\r\n");	/* SIGNAL interrupts I/O */
		ctrlc_hit = 0;		/* reset */
		}
	tcgetattr(STDIN_FILENO, &tstuff);
	show_settings(stdout, &tstuff);

	restore_orig();		/* we should be back in normal echo
			noraw mode but just in case.. */

#include "cleanup.c"		/* We have written this code to make
			it EASIER to reconcile Winsock 1.1/2.0 to
			(the MUCH easier) Unix cleanup. */

	return(0);
	}  /* end of main */

/* --------------------------------------------------------------------
Terminal Utility Routines:
			void ctrlc_handler(int)
			int show_settings(FILE *, struct TERM_STRUCT *)
			int raw_noecho(void)   
			int raw_cbreak(void)
			int fionbio_select(void)
			int echo_noraw(void)			
   -------------------------------------------------------------------- */

void ctrlc_handler(int dummy)
			/* Rather than use signal(SIGINT, SIG_IGN), which
		will completely disable action taken after CTRL-C, this
		interrupt routine is useful in CBREAK mode (raw_cbreak())
		to simply display the string "^C " on stdout */
	{
	ctrlc_hit = 1;			/* set global */
	write(1, " ctrlc_handler: ^C ", 18);
	signal(SIGINT, ctrlc_handler);	/* should be unnecessary if
				POSIX compliant */
	}  /* end of ctrlc_handler */

/* --------------------------------------------------------------------
Organization of Terminal Flags in:   struct termio/termios(SUN)
Usually only those marked with (*) are actually changed:

.c_iflag (input)
	IGNBRK		If set, BREAK is ignored
	BRKINT		If set, BREAK generates interrupt
       *IGNPAR		If set, ignore parity and framing errors
       *ISTRIP		If set(usually), 8th bit is stripped
	IGNCR		If set(rare), CR is ignored on input
       *ICRNL		If set(usually), received CR maps to CR-LF
	IXON		If set(usually), CTRL-S/CRTL-Q enabled
	IXOFF		If set(rare), no use of CRTL-S/CRTL-Q
.c_oflag (output)
       *ONLCR		If set(usually), LF maps to CR-LF on output
	OCRNL		If set(rare), CR maps to LF on output
.c_lflag (line discipline)
       *ISIG		If set(usually), INTR, QUIT, (and, on SUN, SUSP)
			   are active
       *ECHO		If set(usually), echo is enabled
       *ICANON		If set(usually), cooked mode with erase (BS on
			   System V, DEL on Berkeley) and kill (CTRL-U)
.c_cc (control chars)
       *.c_cc[VTIME]	If 0, no timeout (on SUN, VTIME=VEOL)
       *.c_cc[VMIN]	If 1, return from read after 1 char (on SUN, VMIN=VEOF)
	.c_cc[VINTR]	SIGINT character, usually DEL or CTRL-C on System V
				and CRTL-C on Berkeley
	.c_cc[VQUIT]	SIGQUIT character, usually CTRL-\ or FS
	.c_cc[VERASE]	Erase with BS or DEL on System V and
				DEL on Berkeley
	.c_cc[VKILL]	Delete entire line with CTRL-U
	.c_cc[VEOF]	End of file/transmission, usually CTRL-D
	.c_cc[VEOL]	Additional (to LF) end of line, usually NUL

SUN only "FEATURES" in struct "termios"
       *.c_cc[VSUSP]	Default CTRL-Z for task switch in csh
NOTE THAT
	signal(SIGTSTP, SIG_IGN)	will disable CRTL-Z.
   -------------------------------------------------------------------- */

int show_settings(FILE *fp, struct TERM_STRUCT *ptsave)
			/* This function displays the most important
		terminal settings and control characters */
	{

#if defined(MSDOS)
	fprintf(fp,
"ERASE  KILL    [TANDEM]  [CBREAK]  [LCASE]   [ECHO]    [CRMOD]    [RAW] ");
	fprintf(fp, "\r\n");
	fprintf(fp, " %2.2Xh   ", ptsave->sg_erase);
	fprintf(fp, " %2.2Xh   ", ptsave->sg_kill);
	if (ptsave->sg_flags & TANDEM)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & CBREAK)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & LCASE)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & ECHO)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & CRMOD)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	if (ptsave->sg_flags & RAW)
		fprintf(fp, "   set    ");
	else
		fprintf(fp, "          ");
	fprintf(fp, "\r\n");
#else
	fprintf(fp,
"IGNBRK IGNPAR ISTRIP IGNCR  ICRNL  IXON   IXOFF  ISIG [VSUSP] [VTIME] [VEOF]");
	fprintf(fp, "\r\n");
	if (ptsave->c_iflag & IGNBRK)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & IGNPAR)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & ISTRIP)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & IGNCR)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & ICRNL)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & IXON)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_iflag & IXOFF)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_lflag & ISIG)
		fprintf(fp, " set  ");
	else
		fprintf(fp, "      ");
	fprintf(fp, "[%2.2Xh] ",ptsave->c_cc[VSUSP]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VTIME]);
	fprintf(fp, " [%2.2Xh] ",ptsave->c_cc[VEOF]);
	fprintf(fp, "\r\n");
#if defined(__bsd)
	fprintf(fp,
"BRKINT ONLCR  ECHO   ICANON   [VINTR] [VQUIT] [VKILL] [VERASE] [VMIN]");
#else
	fprintf(fp,
"BRKINT ONLCR  OCRNL  ECHO   ICANON   [VINTR] [VQUIT] [VKILL] [VERASE] [VMIN]");
#endif
	fprintf(fp, "\r\n");
	if (ptsave->c_iflag & BRKINT)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_oflag & ONLCR)
		fprintf(fp, "  set  ");
	else
		fprintf(fp, "       ");
#if defined(__bsd)

#else
	if (ptsave->c_oflag & OCRNL)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
#endif
	if (ptsave->c_lflag & ECHO)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	if (ptsave->c_lflag & ICANON)
		fprintf(fp, " set   ");
	else
		fprintf(fp, "       ");
	fprintf(fp, "   [%2.2Xh] ",ptsave->c_cc[VINTR]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VQUIT]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VKILL]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VERASE]);
	fprintf(fp, "  [%2.2Xh] ",ptsave->c_cc[VMIN]);
	fprintf(fp, "\r\n");
#endif

	return(0);
	}  /* end of show_settings */

/* -------------------------------------------------------------------- */

int	raw_noecho(void)   
			/* This function sets full RAW mode for single
		character I/O, that is, getchar() waits for a
		single ascii character and returns when a key is
		pressed. Since ISIG is DISABLED, CTRL-C (SIGINT)
		will return 0x03 and CTRL-\ (SIGQUIT) will return
		0x1B=FS. Do NOT use this mode with select(). */
	{
#if defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = RAW_NOECHO_MODE;	/* record current setting */

#else			/* Unix */
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	zero = 0;
				 	/* modify settings in tinfo */
	tinfo.c_iflag = (tsave.c_iflag & ~ICRNL) | IGNPAR |
		ISTRIP;  /* turn off   cr cr-lf  conversion, ignore
		parity errors, strip off 8th bit */
#if defined(__bsd)
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON | ISIG);  /* turn
		off echo and erase and kill functions */
#else
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR | OCRNL);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON | ISIG);  /* turn
		off echo and erase and kill functions */
#endif
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 1;    /* return from read after 1 character */
	tinfo.c_cc[VSUSP] = 0;	 /* disable CTRL-Z (SUSPEND) */

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
#if defined(__bsd)
	ret = tcsetattr(0, TCSAFLUSH, &tinfo);
#else
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
#endif
	current_mode = RAW_NOECHO_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of raw_noecho */

/* -------------------------------------------------------------------- */

int	raw_cbreak(void)   
			/* This function sets UNIX CBREAK mode for single
		character I/O, that is, getchar() waits for a
		single ascii character and returns when a key is
		pushed. Since ISIG has NOT been changed, CTRL-C
		(SIGINT) and CTRL-\ (SIGQUIT) are still ACTIVE.
		Do NOT use this mode with select(). */
	{
#if defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = CBREAK_MODE;	/* record current setting */

#else			/* Unix */
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	zero = 0;
				 	/* modify settings in tinfo */
	tinfo.c_iflag = (tsave.c_iflag & ~ICRNL) | IGNPAR |
		ISTRIP;  /* turn off   cr cr-lf  conversion, ignore
		parity errors, strip off 8th bit */
#if defined(__bsd)
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON);  /* turn
		off echo and erase functions */
#else
	tinfo.c_oflag = tsave.c_oflag & ~(ONLCR | OCRNL);  /* turn off
		lf cr-lf conversion */
	tinfo.c_lflag = tsave.c_lflag & ~(ECHO | ICANON);  /* turn off
		echo and erase functions */
#endif
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 1;    /* return from read after 1 character */
	tinfo.c_cc[VSUSP] = 0;	 /* disable CTRL-Z (SUSPEND) */

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
#if defined(__bsd)
	ret = tcsetattr(0, TCSAFLUSH, &tinfo);
#else
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
#endif
	current_mode = CBREAK_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of raw_cbreak */

/* ------------------------------------------------------------------- */

int	fionbio_select(void)	
			/* This function is adapted for use with 
		non-blocking I/O (FIONBIOF) and select()
		(so VMIN=VTIME=0) and the rest is like cooked mode
		EXCEPT ECHO and ICANON are OFF. */
	{
#if defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = SELECT_MODE;	/* record current setting */

#else			/* Unix */
	int	ret, one;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	one = 1;
				 	/* modify settings in tinfo */
	tinfo.c_iflag = tsave.c_iflag | ICRNL | IGNPAR |
		ISTRIP;  /* turn on   cr cr-lf  conversion, still ignore
		parity errors, still strip off 8th bit */
	tinfo.c_oflag = tsave.c_oflag & ~ONLCR;  /* turn off lf cr-lf
		conversion (otherwise vi will fail) */
	tinfo.c_lflag = (tsave.c_lflag & ~(ECHO | ICANON)) | ISIG; /* turn
		off echo and erase but save kill function */
	tinfo.c_cc[VTIME] = 0;   /* none */
	tinfo.c_cc[VMIN] = 0;    /* none */
	tinfo.c_cc[VSUSP] = 0; /* disable CTRL-Z (SUSPEND) */
	
	ioctl(0, FIONBIO, &one);	/* set non-blocking I/O */
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
	current_mode = SELECT_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of fionbio_select */

/* ------------------------------------------------------------------- */

int	echo_noraw(void)   
			/* This function sets the normal, default, UNIX
		COOKED mode (also called line-edited mode) */
	{
#if defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = ECHO_NORAW_MODE;	/* record current setting */

#else			/* Unix */
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	zero = 0;
				 	/* modify settings in tinfo */
	tinfo.c_iflag = tsave.c_iflag | ICRNL | IGNPAR |
		ISTRIP;  /* turn on   cr cr-lf  conversion, still ignore
		parity errors, still strip off 8th bit */
	tinfo.c_oflag = tsave.c_oflag | ONLCR;  /* turn on lf cr-lf
		conversion */
	tinfo.c_lflag = tsave.c_lflag | ECHO | ICANON | ISIG;  /* turn
		on echo and erase and kill functions */
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 4;    /* return from read after 4 characters */
	tinfo.c_cc[VSUSP] = 0x1A; /* enable CTRL-Z (SUSPEND) */

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
	current_mode = ECHO_NORAW_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of echo_noraw */

/* ------------------------------------------------------------------- */

int	noecho_noraw(void)   
			/* This function is EXACTLY like echo_noraw()
		except that ECHO is OFF */
	{
#if defined(_WIN95) || defined(_WINNT)

#else			/* Unix */
	int	ret, zero;
	struct	TERM_STRUCT tsave, tinfo;

	if (!termsave_done) 
		{
		ret = tcgetattr(STDIN_FILENO, &toriginal);
		termsave_done = 1;
		}
	ret = tcgetattr(STDIN_FILENO, &tsave);
	ret = tcgetattr(STDIN_FILENO, &tinfo);
	zero = 0;
				 	/* modify settings in tinfo */
	tinfo.c_iflag = tsave.c_iflag | ICRNL | IGNPAR |
		ISTRIP;  /* turn on   cr cr-lf  conversion, still ignore
		parity errors, still strip off 8th bit */
	tinfo.c_oflag = tsave.c_oflag | ONLCR;  /* turn on lf cr-lf
		conversion */
	tinfo.c_lflag = (tsave.c_lflag | ICANON | ISIG) & (~ECHO);  
		/* turn off echo, turn on erase and kill functions */
	tinfo.c_cc[VTIME] = 0;   /* no timeout */
	tinfo.c_cc[VMIN] = 4;    /* return from read after 4 characters */
	tinfo.c_cc[VSUSP] = 0x1A; /* enable CTRL-Z (SUSPEND) */

	ioctl(0, FIONBIO, &zero);	/* set blocking I/O */
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &tinfo);
	current_mode = ECHO_NORAW_MODE;	/* record current setting */
#endif
	return(0);
	}  /* end of noecho_noraw */

/* ------------------------------------------------------------------- */

int	restore_orig(void)	
			/* This function RESTORES the ORIGINALLY
		saved settings (if, indeed, they were saved) and
		closes the "kb_emul.log" file. */
	{
#if defined(_WIN95) || defined(_WINNT)

			/* ... and we *** FAKE *** it again */

	current_mode = ECHO_NORAW_MODE;	/* actually an assumption */

#else			/* Unix */
	int	ret;

	if (termsave_done) 
		{
		ret = tcsetattr(STDIN_FILENO, TCSANOW, &toriginal);
		}
	current_mode = ECHO_NORAW_MODE;	/* actually an assumption */
#endif
	return(0);
	}  /* end of restore_orig */

/* ------------------------------------------------------------------- */
