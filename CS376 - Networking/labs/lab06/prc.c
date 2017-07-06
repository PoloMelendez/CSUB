#include <stdio.h>
#include <sys/types.h>
#include "getsockaddr.h"
#define TELCMDS
#define TELOPTS
#include <arpa/telnet.h>

#define	TRANSBINARY    0
#define TOGGFLOWCONT  33
#define	FC_OFF 0
#define XON	(unsigned char)17
#define XOFF	(unsigned char)19
					/* see RFC's 854, 855, 1372 */
unsigned char dotbin[] = { IAC , DO , TRANSBINARY };
unsigned char dotfc[] = { IAC , DO , TOGGFLOWCONT };
unsigned char nofc[] = { IAC , SB , TOGGFLOWCONT , FC_OFF , IAC , SE };

int	s;		/* global socket */

char *host = "termserv1";
char *port = "tsport1";

main(argc, argv)
  int argc;
  char *argv[];
  {
  struct sockaddr_in sin;	
  if (argc > 1)
    port = argv[1];
  if (argc > 2)
    host = argv[2];

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0)
    {
    perror("socket:");
    exit(1);
    }
  getsockaddr(&sin, host, port, "tcp");
  if (connect(s, &sin, sizeof(sin)) < 0)
    {
    perror("connect:");
    exit(1);
    }

/* transmit BINARY */
    negopt(DO, TRANSBINARY);
    negopt(WILL, TRANSBINARY);

/* negotatiate NO flow control */
    negopt(DO, TOGGFLOWCONT);
    negopt(WILL, TOGGFLOWCONT);

/* TRY AGAIN -- 90TL is an ASSHOLE */
    write(s, nofc, sizeof(nofc));	/* DISABLE flow control */

/* negotiate GO AHEAD suppression */
    negopt(DO, TOGGFLOWCONT);
    negopt(WILL, TOGGFLOWCONT);

  for(;;)
    {
    fd_set readable;
    unsigned char c[1];

    FD_ZERO(&readable);
    FD_SET(s, &readable);
    FD_SET(0, &readable);
    select(255, &readable, NULL, NULL, NULL, NULL);
    if (FD_ISSET(0, &readable))
      {
      if (read(0, c, 1) != 1)
	goto done;
      if (*c == IAC)
	{
	write(s, c, 1);
	write(s, c, 1);
	}
      else if (*c == XON)
	{
	write(s, c, 1);
	fprintf(stderr, "sent D1=XON\n");
	}
      else if (*c == XOFF)
	{
	write(s, c, 1);
	fprintf(stderr, "sent D3=XOFF\n");
	}
      else
	write(s, c, 1);
      }
    if (FD_ISSET(s, &readable))
      {
      get_telnet();
      }
    }
done:
  close(s);
  }

/* ------------------------------------------------------------ */

get_telnet()
  {
  int ngot;
  unsigned char c;
  static int state = 0;

  ngot = read(s, &c, 1);
  if (ngot != 1)
    {
    close(s);
    fprintf(stderr, "Server closed connection\n");
    exit(1);
    }
  switch(state)
    {
    case 0:
      if (c == IAC)
	{
	fprintf(stderr, "IAC ");
	state = 1;
	goto readagain;
	}
      else
	return(c);
    case 1:
      if (!TELCMD_OK(c))
	{
	fprintf(stderr, "?%X\n", c);
	return(c);
	}
      if (c == DONT || c == DO || c == WILL || c == WONT)
	{
	fprintf(stderr, "%s ", TELCMD(c));
	state = 2;
	goto readagain;
	}
      if (c == SE)
	{
	fprintf(stderr, "%s ", TELCMD(c));
	state = 3;
	goto readagain;
	}
      fprintf(stderr, "%s\n", TELCMD(c));
      state = 0;
      goto readagain;
    case 2:
      if (!TELOPT_OK(c))
	fprintf(stderr, "UNKNOWN %x\n", c);
      else
	fprintf(stderr, "%s\n", TELOPT(c));
      state = 0;
      goto readagain;
    case 3:
      if (c == SE)
	{
	fprintf(stderr, "SE\n");
	state = 0;
	goto readagain;
	}
      fprintf(stderr, "%X ", c);
      goto readagain;
    }
  readagain:
  return(0);
  }

negopt(type, opt)
  char type, opt;
  {
  char buf[3];
  buf[0] = IAC;
  buf[1] = type;
  buf[2] = opt;

  write(s, buf, 3);
  }


