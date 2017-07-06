			/* This program sets up a virtual circuit with
			a remote station (which runs SESSRECV.EXE) by
			making synchronous procedure calls to the NETBIOS.
			It then sends line oriented ascii data obtained
			by getline() to the remote receiver. The program
			is ended by typing a line starting with "~".
			This program must be linked with DOS960.OBJ, say
			by using a batch file such as:

			cl /AC /Fe%1.exe /Fm%1.map %1.c dos960.obj

			in the case of a Microsoft C 5.0 or later
			version Compiler (the COMPACT model of segmentation
			is used since our code is much less than 64K
			but we may need multiple data segments). The
			program is invoked by typing:

			sesssend CLIENTxx CLIENTxx

			where the first operand is the name of the local
			station and the second operand is the name of
			the remote station. note: these names are
			CASE SENSITIVE but this program automatically
			converts names to UPPER CASE.

						Marc Thomas	*/

#include <stdio.h>

extern	send_ncb();		/* procedure in DOS960.OBJ	*/

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

struct	ncb_type	ncb;	/* send NCB */
byte	sbuffer[BUFSIZE];	/* data buffer for send NCB */
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

char	signon[] = "\nNetbios Session Level Send Test. \n\n";

/* ---------------------------------------------------------------------- */

main(argc, argv)		/* main program for NETBIOS Send Test */
int 	argc;
char	**argv;
	{
	int	i,j,k,ret,sret,len;
	char	source[64],dest[64];
	char	chrline[132];
	char	*blank = "                ";

	printf("%s",signon);		/* hello... */

		/* CHECK THE COMMAND LINE ARGUMENTS */
	if (argc < 2)
	  {
	  printf("enter local clientname: ");
	  gets(source);
	  }
	else
	  strcpy(source,argv[1]);	/* copy from command line */
	if (argc < 3)
	  {
	  printf("enter remote clientname: ");
	  gets(dest);
	  }
	else
	  strcpy(dest,argv[2]);		/* copy from command line */
	for (i = 0 ; i < 64 ; i++)	/* force UPPER CASE since name
				table is CASE SENSITIVE */
		{
		if (source[i] == '\0') break;
		if ((source[i] >= 'a')  &&  (source[i] <= 'z'))
			source[i] = source[i] - 32;
		} 
	for (i = 0 ; i < 64 ; i++)	/* force UPPER CASE since name
				table is CASE SENSITIVE */
		{
		if (dest[i] == '\0') break;
		if ((dest[i] >= 'a')  &&  (dest[i] <= 'z'))
			dest[i] = dest[i] - 32;
		} 

		/* INITIALIZE THE NCB */
	ncb.buffer = sbuffer;		/* set pointer to sbuffer[] */
	ncb.retcode = SUCCESS;
	strncpy(ncb.callname,blank,16);	/* clear callname field	*/
	strncpy(ncb.name,blank,16);	/* clear name field */
	ncb.sto = 60;			/* 60 * 500 mSec */
	ncb.off_post = 0;		/* used only with NO_WAIT option */
	ncb.seg_post = 0;		/* dword = 0 pointer */
	ncb.cmd_cplt = PENDING;	/* used only with NO_WAIT option */
	ncb.lana_num = 0;		/* primary adapter */

		/* SET UP A VIRTUAL CIRCUIT */
	ret = ncb_call(source,dest); /* set up virtual circuit between local
				   name "source" and remote name "dest" */
	if (ret != 0)
		{
		printf("\n");
		sret = sess_status();
		printf("\n    connection failed, resetting... \n");
		ncb.command = RESET;	/* note that a side effect of
				RESET is the DE-INITIALIZATION of the
				higher presentation and application
				layers (i.e. MS-NET) */
		ncb.lsn = 0x00;		/* set default number of
				sessions (i.e. 6) */
		ncb.num = 0x00;		/* set default number of
				outstanding ncbs (i.e. 12) */
		ncb.lana_num = 0x00;	/* primary adapter */
		send_ncb(&ncb);	/* call DOS960 - no error checking */
		printf("    and ending program. \n");
		exit(1);
		}
	else
		sret = sess_status();
	printf("\nEnter lines of text, type ~ to end. \n\n");

		/* START ENTER TEXT LOOP */
loop:	printf("enter line> ");
	len = getline(chrline);
	if ((len == 0) || (chrline[0] =='~'))
		goto over;
					/* put line of data in buffer   */
	strncpy(sbuffer,chrline,len + 1);
	ncb.command = SEND;
	ncb.length = len;		/* actual characters number len */
	ret = syn_issue_ncb(&ncb);	/* issue SEND NCB and check
				for errors */
	if (ret == 0x05)		/* when timeout occurs for a SEND
				command the connection is abnormally
				terminated (the remote node will show a
				HANGUP) and the connection must be 
				re-established. */
		{
		printf("\n   timeout occured, trying to re-connect... \n");
		ret = ncb_reconn();
		}
	if (ret != 0x00) 
		{
		printf("\n");
		sret = sess_status();
		printf("\n    unrecoverable error, resetting... \n");
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
	goto loop;

		/* TERMINATE THE VIRTUAL CIRCUIT */
over:
	ncb.command = HANGUP;
	ncb.retcode = SUCCESS;
	ret = syn_issue_ncb(&ncb);	/* issue HANGUP NCB and check
				for errors */
	if (ret != 0) printf("\n");
	sret = sess_status();	/* check status after HANGUP	   */

	if (sret == 0x03)	/* there is an undocumented problem with
				HANGUP if a SEND has timed-out previously
				then SESSION_STATUS gives incorrect info.
				unless a RESET is done. */
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
Utility Functions:		getline
				syn_issue_ncb
				asyn_issue_ncb
				ncb_call
				ncb_reconn
				ncb_listen
				sess_status
				err_type
  ------------------------------------------------------------------- */

getline(s)			/* this function gets a line (with echo)
				typed from the console. It differs from
				gets() in that the newline is part of
				the string. */
char	s[];
	{
	int 	c,i;

	for (i = 0 ; 
	     (c = getchar()) != EOF  &&  c != '\n'  && c != '\r' ;
	     i++ ) s[i] = c;
	if ((c == '\n')  ||  (c == '\r'))
	     {
	     s[i] = '\n';
	     ++i;
	     }
	s[i] = '\0';

	return(i);
	}  /* end of getline */

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
			send_ncb(), should be PENDING) */

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
	strncpy(ncb.name, lname,strlen(lname));
	strncpy(ncb.callname, wildcard, strlen(wildcard));
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
		printf("  name table index: %2.2xh \n",sbuffer[0]);
		printf("  total sessions:   %2.2xh \n",sbuffer[1]);
		if (sbuffer[1] == 0x00)
			{
		  printf("  no active sessions: other fields undefined. \n");
			return(0);	/* case of zero sessions */
			}
		if (s_number == sbuffer[4])	/* hunt for `active' block,
					blocks are of length 36 bytes. */
			bstart = 4;
		else if (s_number == sbuffer[40])
			bstart = 40;
		else if (s_number == sbuffer[76])
			bstart = 76;
		else if (s_number == sbuffer[112])
			bstart = 112;
		else if (s_number == sbuffer[148])
			bstart = 148;
		else if (s_number == sbuffer[184])
			bstart = 184;
		else if (s_number == sbuffer[220])
			bstart = 220;
		else if (s_number == sbuffer[256])
			bstart = 256;
		else
			{
	printf("\nProblem: can't match block, using bstart = 4: \n");
			bstart = 4;
			}
		printf("  session number:   %2.2xh \n",sbuffer[bstart]);
		printf("  session status:   ");
		sret = sbuffer[bstart + 1];	/* record */
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
		  }  /* end of switch(sbuffer[bstart + 1]) */
		printf("  local name:       %16.16s \n",&sbuffer[bstart + 2]);
		printf("  remote name:      %16.16s \n",&sbuffer[bstart + 18]);
		printf("  outstanding receive commands: %3.3d \n",
			sbuffer[bstart + 34]);
		printf("  outstanding send commands:    %3.3d \n",
			sbuffer[bstart + 35]);
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

