			/* This program sets up a virtual circuit with
			a remote station (which runs SESSSEND.EXE). On
			program invocation it enters a shell-like mode
			accepting keyboard input and executing external
			commands (*.BAT, *.COM, and *.EXE). By typing
			"listen" it makes a synchronous LISTEN to the
			NETBIOS until some PC CALLs. At this point is
			begins making asynchronous RECEIVEs which are
			displayed in the top window of the screen while
			still accepting shell commands. This program no
			longer needs to be linked with DOS960.OBJ due to
			the use of the (included) int86x(), int86() calls.
			An executable can be made with a batch file as
			follows:

			cl /AC /Fe%1.exe /Fm%1.map %1.c graphics.lib

			in the case of a Microsoft C 5.0 or later
			version Compiler (the COMPACT model of segmentation
			is used since our code is much less than 64K
			but we may need multiple data segments). The
			program is invoked by typing:

			sessrecv CLIENTxx

			where the operand is the name of the local station
			(there is no second operand since the program
			accepts calls from any PC source). note: this name
			is CASE SENSITIVE but this program automatically
			converts the name to UPPER CASE.

						Marc Thomas	*/

#include <stdio.h>
#include <graph.h>		/* for screen-handling functions */
#include <bios.h>		/* for _bios_keybrd()		*/
#include <stdlib.h>		/* for system()			*/
#include <process.h>		/* for system()			*/
#include <dos.h>		/* for int86x() and int86()	*/

/* extern	send_ncb();	   OLD procedure in DOS960.OBJ	*/

#define byte unsigned char	/* for ASM compatibility */
#define word unsigned int
#define dword unsigned long

struct ncb_type	{	/* typical NETBIOS Network Control Block  */
byte	command;	/* b0 -- b6 = opcode, b7 = no_wait option */
byte	retcode;	/* 00H = successful completion of command */
byte	lsn;		/* local session number returned by CALL or LISTEN */
byte	num;		/* name table entry returned by ADDNAME */
byte	*buffer;	/* pointer to data buffer (up to 64K)   */
word	length;		/* input: length avail., output: length actual */
byte	callname[16];	/* name of user (process) being called  */
byte	name[16];	/* name of local user (process) 	*/
byte	rto;		/* receive time out (500 m.s. increments) */
byte	sto;		/* send time out (500 m.s. increments)  */
word	off_post;	/* pointer to posting routine used with */
word	seg_post;	/* 	NO_WAIT option			*/
byte	lana_num;	/* adapter number: 0 = primary		*/
byte	cmd_cplt;	/* command/status for NO_WAIT option	*/
byte	reserve[14];	/* reserved by NETBIOS			*/
} ;

#define BUFSIZE	4096
#define HISTLINES 7		/* seven line history buffer */

struct	ncb_type ncb, cncb;	/* receive NCB and cancel NCB  */
byte	rbuffer[BUFSIZE];	/* data buffer for receive NCB */
byte	s_number;		/* local session number     */
byte	n_index;		/* index into local name table */

#define	SUCCESS	0		/* some NETBIOS return codes */
#define FAILURE	-1
#define PENDING 0xFF

#define CALL 0x10		/* some common NETBIOS opcodes */
#define LISTEN 0x11
#define HANGUP 0x12
#define SEND 0x14
#define RECV 0x15
#define ADDNAME 0x30
#define DELETENAME 0x31
#define RESET 0x32		/* side effect of RESET is LOSS of
				INITIALIZATION of MS-NET (presentation
				and application layers) */
#define ADAPTSTATUS 0x33
#define SESSSTATUS 0x34
#define CANCEL 0x35
#define ASYNCSEND 0x94		/* NO_WAIT type call */
#define ASYNCRECV 0x95		/* NO_WAIT type call */

				/* IMPORTANT global variables */
char	signon[] = "\nNetbios Session Level Receive Test. \n\n";
char	hist[HISTLINES][80];	/* history buffer, consists of lines
				   of the form:
				   aedit filename<CR><LF><null>  */
int	nbios_ok;		/* 1 = NETBIOS is alive and well */
int	conn_flag;		/* 0 = no current connection */
struct	WORDREGS wreg,outreg;	/* for int86x() calls	     */
struct	SREGS		sreg;	/* for int86x() calls	     */
struct	rccoord save_top, save_bot;	/* screen window status vars. */

/* ---------------------------------------------------------------------- */

main(argc, argv)		/* main program for NETBIOS Receive Test */
int 	argc;
char	**argv;
	{
	int	i, j, k, ret, sret, len, lindex;
	unsigned	scan;			/* for _bios_keybrd() */
	char	linebuf[80];		/* <command><CR><LF><null>    */
	char	cmdbuf[80];		/* <command><null>	      */
	char	khar;
	char	home[64], chrline[80], prompt[80], cbuf[2], message[132];
	char	*blank = "                ";	/* for initializing   */

	printf("%s",signon);		/* hello... */

		/* CHECK THE COMMAND LINE ARGUMENTS */
	if (argc < 2)
	  {
	  printf("enter local clientname: ");
	  gets(home);
	  }
	else
	  strcpy(home,argv[1]);	/* copy from command line */
	for (i = 0 ; i < 64 ; i++)	/* force UPPER CASE since name
				table is CASE SENSITIVE */
		{
		if (home[i] == '\0') break;
		if ((home[i] >= 'a')  &&  (home[i] <= 'z'))
			home[i] = home[i] - 32;
		} 

		/* INITIALIZE THE NCB and CNCB */
/*	if (check_nbios() != 0) 	   CURRENTLY check_nbios() is
		nbios_ok = 0;			NOT WORKING ???
	else
		nbios_ok = 1;		   NETBIOS alive and well */
	nbios_ok = 1;			/* KLUGE */

	conn_flag = 0;			/* no connection yet */
	ncb.buffer = rbuffer;		/* set pointer to rbuffer[] */
	ncb.retcode = SUCCESS;
	cncb.retcode = SUCCESS;
	strncpy(ncb.callname,blank,16);	/* clear callname field	*/
	strncpy(cncb.callname,blank,16);/*   (ncb_listen will complete) */
	strncpy(ncb.name,blank,16);	/* clear name field */
	strncpy(cncb.name,blank,16);	/*   (ncb_listen will complete) */
	ncb.sto = 60;			/* 60 * 500 mSec = 30 seconds   */
	ncb.rto = 0x00;			/* 0 = no receive timeout */
	ncb.off_post = 0;		/* used only with NO_WAIT option */
	ncb.seg_post = 0;		/* dword = 0 pointer */
	ncb.cmd_cplt = PENDING;		/* used only with NO_WAIT option */
	ncb.lana_num = 0;		/* primary adapter */
	cncb.lana_num = 0;		/* primary adapter */

		/* SET UP SCREEN WINDOWS AND PROMPT */
	getcwd(prompt,64);
	strcat(prompt," $ ");
	clear_hist();			/* clear history buffer */
	_clearscreen(_GCLEARSCREEN);
	banners();
		/* line 1: net shell banner
		   lines 2 -- 8 (7 lines): net window
		   line 9: net shell banner 
		   lines 10 -- 24 (15 lines): shell window
		   line 25:					*/
	_settextwindow(2,1,8,80);
	_settextposition(7,1);		/* start on last line */
	save_top = _gettextposition();
	_settextwindow(10,1,24,80);
	_settextposition(15,1);		/* start prompt on last line */
	save_bot = _gettextposition();
	lindex = 0;
	_outtext(prompt);

		/* START ASYNCHRONOUS RECEIVE IF CONNECTED */
srloop:
	if (conn_flag  &&  nbios_ok)
	  {
	  ncb.command = ASYNCRECV;	/* NO_WAIT type call */
	  ncb.retcode = SUCCESS;
	  ncb.length = BUFSIZE;		/* space available */
	  ncb.off_post = 0x0000;	/* no posting routine */
	  ncb.seg_post = 0x0000;
	  ncb.cmd_cplt = PENDING;	/* poll this flag */
	  ret = asyn_issue_ncb(&ncb);	/* issue NCB and return to
				main program, waiting on ncb.cmd_cplt */
	  }


		/* SHELL LOOP -- POLL KEYBOARD AND NCB.CMD_CPLT */
arloop:	
		/* check KEYBOARD first */
	if ((scan = _bios_keybrd(_KEYBRD_READY)) != 0)
		{
		scan = _bios_keybrd(_KEYBRD_READ);
		khar = scan & 0x7f;		/* low byte is ascii */
		if (khar == 0x03) goto over;	/* CTRL-C exit */
		if (khar == 0x08)		/* BACKSPACE */
			{
			if (lindex > 0)
				{
				putchar(khar);	/* BS - putchar, printf do not
					affect _outtext() position */
				putchar(' ');	/* SPACE */
				putchar(khar);	/* BS */
				save_bot = _gettextposition();
				--save_bot.col;
				_settextposition(save_bot.row,save_bot.col);
				--lindex;
				}
			goto arloop;
			}
		if ((khar == 0x00)  &&  (scan == 0x3D00))	/* F3 */
			{
			_outtext("\n");		/* force newline */
			scroll_down(linebuf);	/* discard current command
					and recall last command */
			i = 0;
			while ( (k = linebuf[i]) != 10  &&
				(k != 13)  )
				{
				cmdbuf[i] = k;
				++i;
				}
			lindex = i;
			linebuf[i] = '\0';
			cmdbuf[i] = '\0';
			_outtext(prompt);
			_outtext(linebuf);
			save_bot = _gettextposition();
			post_hist();	/* new history buffer */
			goto arloop;
			}
		linebuf[lindex] = khar;
		cmdbuf[lindex] = khar;
		cbuf[0] = khar; cbuf[1] = '\0';
		_outtext(cbuf);
		++lindex;
				/* in DOS the first item below will be
				   the one that actually occurs */
		if ((khar != 13)  &&  (khar != '\r')  &&  (khar != '\n'))
			goto arloop;	/* look for more key presses */
		else				/* EXECute command */
			{
			linebuf[lindex] = 10;	/* add linefeed to CR */
			cmdbuf[lindex - 1] = '\0';  /* cmdbuf[] doesn't need
				CR LF in the string like linebuf[] does. */
			++lindex;
			linebuf[lindex] = '\0';	/* end string in linebuf[] */
			_outtext("\n");		/* force newline at EOL  */
/* WINDOW STATUS:
   BOTTOM WINDOW ACTIVE ON NEWLINE>>> */
		/* EARLY CHECK FOR HISTORY COMMAND */
			if (strncmp(cmdbuf,"history",7) == 0)
				{
				scroll_down(linebuf);	/* get last command */
				i = 0;
				while ( (k = linebuf[i]) != 10  &&
					(k != 13)  )
					{
					cmdbuf[i] = k;
					++i;
					}
				lindex = i;
				linebuf[i] = '\0';
				cmdbuf[i] = '\0';
				_outtext(prompt);
				_outtext(linebuf);
				save_bot = _gettextposition();
				post_hist();	/* new history buffer */
				goto arloop;
				}
		/* NON-HISTORY COMMAND */
			post_top_window(linebuf);	/* "POST" commands
				always save and restore state of bottom
				window */
			scroll_up(linebuf);	/* LOG in history buffer */
/* WINDOW STATUS:
   BOTTOM WINDOW STILL ACTIVE ON NEWLINE>>> */
		/* "FILTER" SOME INTERNAL COMMANDS */
			if (strncmp(cmdbuf,"help",4) == 0)	/* HELP */
				{
				help();		/* show available commands */
				lindex = 0;	/* clear linebuf[] */
				_outtext(prompt);
				goto arloop;	/* internal command */
				}
			if (strncmp(cmdbuf,"listen",6) == 0)  /* LISTEN */
				{
				if (!nbios_ok)
					goto nlisten;	/* clear linebuf[] */
				if (conn_flag)
					goto nlisten;	/* already connected */
			    				/* try TO
		    SET UP A VIRTUAL CIRCUIT with sender */
				ret = ncb_listen(home);
				if (ret != 0)
					{
					printf("\n");
					conn_flag = 0;	/* insurance */
					}
				else
					{
					sret = sess_status();
					conn_flag = 1;		/* have a
							 valid connection */
					}
nlisten:					/* update buffer information */
				banners();
				_settextwindow(10,1,24,80);	/* grestore */
				_settextposition(save_bot.row,save_bot.col);

				lindex = 0;	/* clear linebuf[] */
				_outtext(prompt);
				goto srloop;	/* start an ASYNCRECV if
							conn_flag == 1 */
				}  /* end of if "listen" ... */

			if (strncmp(cmdbuf,"exit",4) == 0) 	/* EXIT */
				goto over;	/* end program */
					/* no action yet on following: */
			if ( (strncmp(cmdbuf,"cd",2) == 0)   ||
			     (strncmp(cmdbuf,"rmdir",5) == 0)  ||
			     (strncmp(cmdbuf,"mkdir",5) == 0)  ||
			     (strncmp(cmdbuf,"mode",4) == 0)   )
				goto aexec;		/* i.e. don't EXEC */

		/* ALLOW SOME EXTERNAL COMMANDS */
			else if (cmdbuf[0] != '\0') 
				{
					/* setup for command EXEC */
/* WINDOW STATUS:
   BOTTOM WINDOW CLEAR AND ACTIVE ON LINE 1>>> */
				_settextwindow(10,1,24,80);
				_clearscreen(_GWINDOW);

				system(cmdbuf);	/* FORK...EXEC...WAIT */

				printf("\n");	/* scroll one free line */
				}	/* fall through to aexec */

/* WINDOW STATUS:
   SCREEN IN UNKNOWN STATE AFTER EXEC>>> */
aexec:
		/* CLEAN UP AFTER COMMAND EXECUTION */
			banners();
			_settextwindow(10,1,24,80);	/* grestore */
			_settextposition(save_bot.row,save_bot.col);

			lindex = 0;		/* start new command line */
			_outtext(prompt);	/* output prompt */
			post_hist();		/* restore history, recall
				that "POST" commands always save and restore
				state of bottom window */
			}  /* end of else (EXECute command) */
		goto arloop;
		}  /* end of if ((scan = _bios_keybrd( ... */

		/* if connected, check ASYNCRECV polling flag NCB.CMD_CPLT */
/* WINDOW STATUS:
   BOTTOM WINDOW ACTIVE IN MIDDLE OF SOME COMMAND>>> */
	if ((conn_flag == 1)  &&  (ncb.cmd_cplt != PENDING))
		{
		if ((ncb.retcode != SUCCESS)  || 
			(ncb.cmd_cplt != SUCCESS))
			{
			if (ncb.cmd_cplt != 0x0a) 	/* session has been
					legitimately closed by the sender */
				{
			  	sprintf(chrline,
				"async_recv_error>> cmd_cplt = %2.2xh \n",
				ncb.cmd_cplt);
				post_top_window(chrline);
/* 				scroll_up(chrline);  DO NOT put in HISTory */
				}
			ncb.retcode = SUCCESS;
			ncb.cmd_cplt = PENDING;	/* some FATAL error so */
			conn_flag = 0;		/*     connection lost */
						/*     stop RECEIVING  */

			banners();		/* new banner */
			_settextwindow(10,1,24,80);	/* grestore */
			_settextposition(save_bot.row,save_bot.col);

			goto arloop;
			}
						/* otherwise, display */
		strcpy(message,"sender>> ");		/* source   */
		strncat(message,rbuffer,ncb.length);	/* add data */
		post_top_window(message);	/* LOG message */
/*		scroll_up(message);	/* DO NOT put in HISTory */
		goto srloop;		/* start another ASYNCRECV command */
		}  /* end of if (ncb.cmd_cplt != PENDING ... */

	goto arloop;		/* no activity found, try again ... */



		/* TERMINATE THE VIRTUAL CIRCUIT */
over:
	_settextwindow(10,1,24,80);
	_settextposition(save_bot.row,save_bot.col);
	if (!nbios_ok)		/* case of no NETBIOS */
		{
		printf("\nNormal program exit. \n");
		exit(0);
		}
	if (conn_flag)		/* if an ASYNCRECV is running use CANCEL */
		{
		_outtext("    trying to cancel ASYNCRECV...\n");
		cncb.command = CANCEL;
		cncb.retcode = SUCCESS;		/* initialize */
		cncb.buffer = (byte *)&ncb;	/* address of NCB to CANCEL */
		cncb.lana_num = 0;		/* primary adapter */
		syn_issue_ncb(&cncb);		/* issue CANCEL command */
		while (ncb.cmd_cplt == PENDING);	/* wait */
		}	
	sret = sess_status();	/* check status after session closure */
	if (sret == 0x03)	/* first see if HANGUP needed */
		{
		ncb.command = HANGUP;
		ncb.retcode = SUCCESS;
		ret = syn_issue_ncb(&ncb);	/* issue HANGUP */
		if (ret != 0) printf("\n");
		sret = sess_status();		/* check status again */
		}
	if (sret == 0x03)
		{
		printf("\n    problem in hangup, resetting... \n");
		ncb.command = RESET;
		ncb.retcode = SUCCESS;
		ncb.lsn = 0x00;		/* set default number of
				sessions (i.e. 6) */
		ncb.num = 0x00;		/* set default number of
				outstanding ncbs (i.e. 12) */
		ncb.lana_num = 0x00;	/* primary adapter */
		send_ncb(&ncb);	/* call DOS960 - no error checking */
		printf("    and ending program. \n");
		exit(1);
		}
	ncb.command = DELETENAME;	/* normal mode of exit     */
	ncb.retcode = SUCCESS;
	send_ncb(&ncb);		/* call DOS960 - no error checking */
	printf("\nNormal program exit. \n");

	exit(0);
	}  /* end of main */

/* -------------------------------------------------------------------
80x86int Functions:		send_ncb
				check_nbios
   ----------------------------------------------------------------- */

send_ncb(pncb)		/* this replaces the ASM call in DOS960.OBJ */
struct ncb_type *pncb;
	{
	unsigned long	point;

	point = (unsigned long)pncb;	/* see Intel iNA960 doc. */
	wreg.bx = (unsigned int)(point & 0x0000ffff);
	sreg.es = (unsigned int)((point & 0xffff0000) >> 16);
	int86x(0x5C,(union REGS *)&wreg,(union REGS *)&outreg,&sreg);

	return(outreg.cflag);
	}  /* end of send_ncb */

/* ------------------------------------------------------------------ */

check_nbios()		/* This function verifies the NETBIOS
			installation and returns 0 if the NETBIOS is
			alive and well, otherwise it returns -1. */
	{
	wreg.ax = 0xB852;		/* see Intel iNA960 doc. */
	wreg.bx = 0x0000;
	int86(0x2F,(union REGS *)&wreg,(union REGS *)&outreg);
	if (outreg.bx != 0x0000)
		return(0);		/* NETBIOS is OK */
	else
		return(-1);		/* failure	 */
	}  /* end of check_nbios */

/* -------------------------------------------------------------------
Utility Functions:		help
				banners
				post_top_window
				syn_issue_ncb
				asyn_issue_ncb
				ncb_call
				ncb_reconn
				ncb_listen
				sess_status
				err_type
  ------------------------------------------------------------------- */

help()			/* this function displays available commands */
	{
	_outtext("  exit    -- cancel any net commands and terminate \n");
	_outtext("  F3 or history -- recall last command into linebuf[] \n");
	_outtext("  listen  -- do a passive open, blocking until CALLed \n");

	return(0);
	}  /* end of help */

/* ------------------------------------------------------------------ */

banners()		/* This function draws screen banners. It may
			be advisable to reactive window and restore
			window position after this call. */
	{
				/* top banner */
	_settextwindow(1,1,1,80);
	_setbkcolor(1L);
	_settextcolor(15L);
	if (conn_flag)
	  {
	  _outtext("  Net Shell: Connected                  ");
	  _outtext("                    Type exit To End  ");
	  }
	else if (nbios_ok)
	  {
	  _outtext("  Net Shell: UnConnected                ");
	  _outtext("                    Type exit To End  ");
	  }
	else
	  {
	  _outtext("  Net Shell: NetBios Not Found          ");
	  _outtext("                    Type exit To End  ");
	  }
				/* middle banner */
	_settextwindow(9,1,9,80);
	  _outtext("                                        ");
	  _outtext("                  Type help For Help  ");
				/* restore colors */
	_settextcolor(7L);
	_setbkcolor(0L);

	return(0);
	}  /* end of banners */

/* ------------------------------------------------------------------ */

post_top_window(s)		/* this function
				1. saves the status of the bottom window.
				2  posts a string (which may contain a
					newline but must end with '\0')
					in the top window.
				3. saves the new status of the top window.
				4. restores the saved status of the
					bottom window.			*/
char	s[];
	{
	save_bot = _gettextposition();	/* gsave bottom */
	_settextwindow(2,1,8,80);	/* grestore top */
	_settextposition(save_top.row,save_top.col);
	_outtext(s);			/* post message in top window */
	save_top = _gettextposition();	/* gsave new top */
	_settextwindow(10,1,24,80);	/* grestore old bottom */
	_settextposition(save_bot.row,save_bot.col);

	return(0);
	}  /* end of post_top_window */

/* ------------------------------------------------------------------ */

syn_issue_ncb(pncb)		/* this function primarily is used to
				issue an NCB to the NETBIOS when the
				call is a WAIT-type call. Unlike
				the lower level send_ncb() in DOS960.OBJ
				which it uses, it DOES REPORT ERROR
				messages to the user. it also returns
				the error code.	WAIT-type synchronous calls
				should ALWAYS INITIALIZE ncb.retcode to
				0x00 (SUCCESS) before this call. */
struct ncb_type *pncb;
	{
	int	ret;

				/* check the NCB */
    	if ((pncb->retcode != SUCCESS)  ||  ((pncb->command & 0x80) != 0))
      		{
		printf("\nWarning: NCB is not initialized. \n");
		pncb->retcode = SUCCESS;	/* reinitialize */
        	}

    	ret = send_ncb(pncb);			/* call DOS960.ASM */

	if (pncb->retcode != SUCCESS)		/* possible error  */
		{
           printf("\nError: NCB Command is %2.2Xh and retcode is %2.2Xh: "
                     , pncb->command, pncb->retcode);
		ret = pncb->retcode;
		err_type(ret);
		pncb->retcode = SUCCESS;	/* reinitialize */
           	return(ret);
        	}
	return(0);
	}  /* end of syn_issue_ncb */

/* -------------------------------------------------------------------- */

asyn_issue_ncb(pncb)		/* this function primarily is used to
				issue an NCB to the NETBIOS when the
				call is a NO_WAIT-type call BUT DOES NOT
				HAVE A POSTING ROUTINE (i.e. the caller
				is going to poll pncb->cmd_cplt). Unlike
				the lower level send_ncb() in DOS960.OBJ
				which it uses, it DOES REPORT ERROR
				messages to the user. it also returns
				the error code.	NO_WAIT-type asynchronous
				calls should always initialize ncb.retcode to
				0x00 (SUCCESS), ncb.cmd_cplt to PENDING, and
				ncb.off_post:ncb.seg_post to 0x00000000
				before this call. */
struct ncb_type *pncb;
	{
	int	ret;

				/* check the NCB */
    	if ((pncb->retcode != SUCCESS)  ||  (pncb->cmd_cplt != PENDING)  ||
		((pncb->command & 0x80) == 0)  ||  
		(pncb->off_post != 0x0000)  ||
		(pncb->seg_post != 0x0000))
      		{
		printf("\nWarning: NCB is not initialized. \n");
		pncb->retcode = SUCCESS;	/* reinitialize */
		pncb->cmd_cplt = PENDING;
		pncb->command = pncb->command | 0x80;	/* NO_WAIT */
		pncb->off_post = 0x0000;	/* no posting routine */
		pncb->seg_post = 0x0000;
        	}

    	ret = send_ncb(pncb);			/* call DOS960.ASM */

						/* USER NOW MUST POLL
			FLAG ncb.cmd_cplt (which, immediately after
			send_ncb(), should still be PENDING) */

	return(ret);				/* in this case, ret
			is from the AL register after the send_ncb()
			call and is non-zero only in GROSS ERROR
			situations. */
	}  /* end of asyn_issue_ncb */

/* -------------------------------------------------------------------- */

ncb_call(lname, rname)		/* this function does two things: it
				adds the source local "lname" to its name
				table (if not there already) identifying
				the name entity to the network, and
				it sets up a session between local name
				"lname" and remote name "rname" */
char *lname;
char *rname;
	{  
	int ret;

			/* first use ADDNAME command */
	strncpy(ncb.name, lname,strlen(lname));
	strncpy(ncb.callname, rname, strlen(rname));
	ncb.command = ADDNAME;	/* add remote name to name table */
	ncb.retcode = SUCCESS;
	ret = syn_issue_ncb(&ncb);  /* issue NCB and check for errors */
	n_index = ncb.num;	/* record name table index */
	if (ret == 0x0D)
	  printf("\n    (the name was already present, no problem.) \n");

			/* second use CALL command */
	ncb.command = CALL;	/* call remote name (which is hopefully
				   listening) */
	ncb.retcode = SUCCESS;
	ret = syn_issue_ncb(&ncb);  /* issue NCB and check for errors */
	s_number = ncb.lsn;	/* record local session number */

	return(ret);
	}  /* end of ncb_call */

/* ------------------------------------------------------------------- */

ncb_reconn()			/* this function tries to re-connect the
				local and remote clients */
	{
	int	ret;

			/* use CALL command */
	ncb.command = CALL;	/* call remote name (which is hopefully
				   listening) */
	ncb.retcode = SUCCESS;
	ret = syn_issue_ncb(&ncb);  /* issue NCB and check for errors */
	s_number = ncb.lsn;	/* record local session number */

	return(ret);
	}  /* end of ncb_reconn */

/* -------------------------------------------------------------------- */

ncb_listen(lname)		/* this function does two things: it
				adds the source local "lname" to its name
				table (if not there already) identifying
				the name entity to the network, and
				it sets up a session between local name
				"lname" and remote name "*", meaning
				ANY REMOTE NODE requesting CALL. */
char *lname;
	{  
	int ret;
	char *wildcard = "*";

			/* first use ADDNAME command */
	strncpy(ncb.name, lname, strlen(lname));
	strncpy(cncb.name, lname, strlen(lname));
	strncpy(ncb.callname, wildcard, strlen(wildcard));
	strncpy(cncb.callname, wildcard, strlen(wildcard));
	ncb.command = ADDNAME;	/* add remote name to name table */
	ncb.retcode = SUCCESS;
	ret = syn_issue_ncb(&ncb);  /* issue NCB and check for errors */
	n_index = ncb.num;	/* record name table index */
	if (ret == 0x0D)
	  printf("\n    (the name was already present, no problem.) \n");

			/* second use LISTEN command */
	ncb.command = LISTEN;	/* listen for any remote CALLS */
	ncb.retcode = SUCCESS;
			/* procedure will BLOCK here until
			some remote node requests CALL */
	printf("\n    passive open: blocked and listening... \n\n");
	ret = syn_issue_ncb(&ncb);  /* issue NCB and check for errors */
	s_number = ncb.lsn;	/* record local session number */

	return(ret);
	}  /* end of ncb_listen */

/* ------------------------------------------------------------------- */

sess_status()			/* this function queries the NETBIOS
			for information concerning the current session
			and displays some of the most pertinent fields.
			it returns the session status code (see below)
			or 0 in the case of NO ACTIVE SESSIONS. Note
			that the documention on the block structure
			of the returned status is NOT ALWAYS ACCURATE. */
	{
	int ret,sret,bstart;

			/* use SESSION_STATUS command */
	ncb.command = SESSSTATUS;	/* set command opcode */
	ncb.retcode = SUCCESS;		/* initialize */
	ncb.lsn = s_number;		/* set local session number -
				seems to be necessary but undocumented */
	ncb.length = 1024;	/* plenty of room available */
	ret = syn_issue_ncb(&ncb);  /* issue NCB and check for errors */
	bstart = 4;		/* first try start block[0] = active */
	if (ret != 0x00)
		printf("\n    session status failed. \n");
	else
		{		/* display */
		printf("Session Status (nti = %2.2xh and lsn = %2.2xh): \n",
				n_index,s_number);
		printf("  name table index: %2.2xh \n",rbuffer[0]);
		printf("  total sessions:   %2.2xh \n",rbuffer[1]);
		if (rbuffer[1] == 0x00)
			{
		  printf("  no active sessions: other fields undefined. \n");
			return(0);	/* case of zero sessions */
			}
		if (s_number == rbuffer[4])	/* hunt for `active' block,
					blocks are of length 36 bytes. */
			bstart = 4;
		else if (s_number == rbuffer[40])
			bstart = 40;
		else if (s_number == rbuffer[76])
			bstart = 76;
		else if (s_number == rbuffer[112])
			bstart = 112;
		else if (s_number == rbuffer[148])
			bstart = 148;
		else if (s_number == rbuffer[184])
			bstart = 184;
		else if (s_number == rbuffer[220])
			bstart = 220;
		else if (s_number == rbuffer[256])
			bstart = 256;
		else
			{
	printf("\nProblem: can't match block, using bstart = 4: \n");
			bstart = 4;
			}
		printf("  session number:   %2.2xh \n",rbuffer[bstart]);
		printf("  session status:   ");
		sret = rbuffer[bstart + 1];	/* record */
		switch(sret)  {
		  case 0x01 : printf("01h = pending LISTEN \n");
			      break;
		  case 0x02 : printf("02h = pending CALL \n");
			      break;
	  	  case 0x03 : printf("03h = connection established \n");
			      break;
		  case 0x04 : printf("04h = pending HANG-UP \n");
			      break;
		  case 0x05 : printf("05h = completed HANG-UP \n");
			      break;
		  case 0x06 : printf("06h = aborted session \n");
			      break;

		  default :   printf("      invalid code \n");
		  }  /* end of switch(rbuffer[bstart + 1]) */
		printf("  local name:       %16.16s \n",&rbuffer[bstart + 2]);
		printf("  remote name:      %16.16s \n",&rbuffer[bstart + 18]);
		printf("  outstanding receive commands: %3.3d \n",
			rbuffer[bstart + 34]);
		printf("  outstanding send commands:    %3.3d \n",
			rbuffer[bstart + 35]);
		}

	return(sret);
	}  /* end of sess_status */

/* ------------------------------------------------------------------- */

err_type(code)			/* this decodes the type of commmon
			errors one often gets from the code returned
			in ncb.retcode */
int	code;
	{
	switch(code)  {
	case 0x00 :	printf("success");
			break;
	case 0x01 :	printf("illegal buffer length");
			break;
	case 0x03 :	printf("command not valid");
			break;
	case 0x05 :	printf("command timed out");
			break;
	case 0x06 :	printf("incomplete message");
			break;
	case 0x08 :	printf("illegal session number");
			break;
	case 0x09 :	printf("no resources available");
			break;
	case 0x0a :	printf("session is closed");
			break;
	case 0x0b :	printf("command cancelled");
			break;
	case 0x0d :	printf("duplicate name");
			break;
	case 0x0e :	printf("local name table full");
			break;
	case 0x0f :	printf("name has active sessions");
			break;
	case 0x11 :	printf("local session table full");
			break;
	case 0x12 :	printf("session open rejected");
			break;
	case 0x13 : 	printf("illegal name number");
			break;
	case 0x14 :	printf("callname not found");
			break;
	case 0x15 :	printf("cannot specify wildcard");
			break;
	case 0x16 :	printf("name being used on remote adapter");
			break;
	case 0x17 :	printf("name deleted");
			break;
	case 0x18 :	printf("session abnormally terminated");
			break;
	case 0xff :	printf("pending");
			break;

	default:	printf("      ");
	}

	return(0);
	}  /* end of err_type */

/* ---------------------------------------------------------------------
history buffer routines		clear_line
				move_line
				clear_hist
				scroll_up
				scroll_down
				post_hist
  -------------------------------------------------------------------- */

clear_line(num)			/* this function clears the designated
			line in the history buffer */
int	num;
	{
	int	i;

	if (num < 0  ||  num >= HISTLINES) return(-1);	/* error */
	for (i = 0 ; i < 79 ; i++)
		hist[num][i] = ' ';	/* space */
	hist[num][1] = 13;		/* Carriage Return */
	hist[num][2] = 10;		/* Line Feed */
	hist[num][3] = '\0';		/* EOString  */
	hist[num][79] = '\0';		/* insurance */
	return(0);
	}  /* end of clear_line */

/* ------------------------------------------------------------------ */

move_line(dest,source)		/* this function moves line number source
			to line number dest (overwriting its former
			contents) */
int	dest,source;
	{
	int	i,k;

	if (dest < 0  ||  dest >= HISTLINES) return(-1);	/* error */
	if (source < 0  ||  source >= HISTLINES) return(-1);	/* error */
	i = 0;
	while ((k = hist[source][i]) != '\0')
		{
		hist[dest][i] = k;
		++i;
		if (i == 79) break;		/* insurance */
		}
	hist[dest][i] = '\0';
	return(0);
	}  /* end of move_line */

/* ------------------------------------------------------------------ */

clear_hist()			/* this function clears the history
			buffer */
	{
	int	i;

	for (i = 0 ; i < HISTLINES ; i++)
		clear_line(i);
	return(0);
	}  /* end of clear_hist */

/* ----------------------------------------------------------------- */

scroll_up(lbuf)			/* this function scrolls the history
			buffer up one line, clearing the last line and
			inserting lbuf in its place. */
char	lbuf[];
	{
	int 	i,k;

	for (i = 0 ; i < HISTLINES - 1 ; i++)	/* move lines */
		move_line(i, i + 1);
	clear_line(HISTLINES - 1);
	i = 0;
	while ((k = lbuf[i]) != '\0')
		{
		hist[HISTLINES - 1][i] = k;
		++i;
		if (i == 79) break;		/* insurance */
		}
	hist[HISTLINES - 1][i] = '\0';
	return(0);
	}  /* end of scroll_up */

/* ----------------------------------------------------------------- */

scroll_down(lbuf)		/* this function scrolls the history
			buffer down one line, but leaves line 0 as
			is (i.e. it is replicated) and returns
			line HISTLINE - 1 in lbuf[80] */
char	lbuf[];
	{
	int	i,k;
	i = 0;
	while ((k = hist[HISTLINES - 1][i]) != '\0')	/* save */
		{
		lbuf[i] = k;
		++i;
		if (i == 79) break;		/* insurance */
		}
	lbuf[i] = '\0';
	for (i = HISTLINES - 2 ; i >= 0 ; i--)	/* now move lines */
		move_line(i + 1, i);
	return(0);
	}  /* end of scroll_down */

/* ---------------------------------------------------------------- */
post_hist()			/* this function
				1. saves the status of the bottom window.
				2. clears the top window.
				3. copies the lines in the history buffer
					to the top window.
				3. saves the new status of the top window.
				4. restores the saved status of the
					bottom window.			*/
	{
	int	i;

	save_bot = _gettextposition();	/* gsave bottom */
	_settextwindow(2,1,8,80);	/* grestore top */
	_clearscreen(_GWINDOW);		/* clear top window */
	_settextposition(HISTLINES,1);
	for (i = 0 ; i < HISTLINES ; i++)
		{
		_outtext(&hist[i][0]);
		}
	save_top = _gettextposition();	/* gsave new top */
	_settextwindow(10,1,24,80);	/* grestore old bottom */
	_settextposition(save_bot.row,save_bot.col);

	return(0);
	}  /* end of post_hist */
