
			/* g_stdio.c: this is the source code for
		our STREAM-type G_FILE I/O functions:  

			g_open()
			g_getc()
			g_gets()
			g_putc()
			g_puts()
			g_read()
			g_write() 	*** TODO ***
			g_seek()
			g_flush()	
			g_close()
			g_dump()	*** TODO ***
			g_profile()

		which work through the Unix low-level I/O interface:

			creat()
			open()
			read()
			write()
			lseek()
			unlink()
			close()

		and, of course, use memory allocation with malloc().
		Profiling is OFF by default, but you can log information
		to g_stdio.log by calling g_profile(level). The levels
		are as follows:

			level 1:	Log system calls made only (this
					  is the recommended default)
			level 2:	Log system calls and the 
					  functions:
						g_open()
						g_seek()	
						g_read()
						g_close()
			level 3:	Log as in level 2 plus the
					  string functions:
						g_gets()
						g_puts()

			level 4:	Log as in level 3 plus the
					  char functions:
						g_getc()
						g_putc()

					...

			level 9:	Log everything under the sun


						Marc Thomas */


#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h> 
				/* use the definitions of EOF
				and NULL from stdio.h */
#define	G_BUFSIZ	2048
#define G_OPEN_MAX	  20	/* max number of open G_FILEs */

typedef	struct g_iobuf {
	int	cnt;	/* for G_READ: cnt is the number of
			UNread bytes in buffer.
			   for G_WRITE: cnt is the number of
			free character positions left before
			the buffer fills. if buffer is allocated
			and (cnt == 0) then buffer is FULL */
	char	*ptr;	/* for G_READ: ptr is a pointer to the
			next character to read (if cnt > 0) 
			   for G_WRITE: ptr is a pointer to the
			next open position (if cnt > 0) */
	char	*base;	/* base address of buffer */
	int	flag;	/* mode of g_file access - if this is 0 the
			slot is currently UNUSED */
	int	fd;	/* file descriptor */
	long	baseposition;	/* this the the byte-offset into
			the G_FILE itself for the byte AT THE
			g_iob[].base position so that
			on G_READ:
				it is updated BEFORE the caller
				actually calls g_fillbuf()
			on G_WRITE:
				it is updated AFTER the write()
				in g_flushbuf() */
} G_FILE;

#define	num_read(p)	(int)((p)->ptr - (p)->base)

#define	g_stdin		(&g_iob[0])
#define	g_stdout	(&g_iob[1])
#define	g_stderr	(&g_iob[2])

				/* G_FILE mode .flag bits */
enum	g_flags {
	G_READ	= 001,	/* open G_FILE for reading */
	G_WRITE	= 002,	/* open G_FILE for writing */
	G_UNBUF	= 004,	/* G_FILE is UNbuffered */
	G_EOF	= 010,	/* G_EOF has occurred on this G_FILE */
	G_ERR	= 020,	/* error has occurred on this G_FILE */
};

#define	G_SEEK_SET	0	/* use "offset" absolutely from 0 */
#define	G_SEEK_CUR	1	/* use "offset" relative from CURrent */
#define	G_SEEK_END	2	/* use "offset" relative to EOFile */

#define	G_PERMS	0644	/* rw-r--r- initially */

			/* Our globals */
G_FILE	g_iob[ G_OPEN_MAX ] = {
	{ 0, (char *)NULL, (char *)NULL, G_READ, 0 , 0L},
	{ 0, (char *)NULL, (char *)NULL, G_WRITE, 1 , 0L},
	{ 0, (char *)NULL, (char *)NULL, G_WRITE | G_UNBUF , 2 , 0L},
	{ 0, (char *)NULL, (char *)NULL, 0, 0, 0L}};	 /* rest will be 
					like this last one */

int	g_prof_on = 0;		/* default is profiling OFF */
int	g_prof_level = 1;	/* levels 1-9 are noted above */
char	g_prof_filename[] = "g_stdio.log";
int	g_prof_fileopen = 0;
FILE	*g_pf;

#define	G_PROFILE_START		1
#define	G_PROFILE_SUSPEND	2
#define	G_PROFILE_RESUME	3
#define	G_PROFILE_STOP 		4

			/* Function Prototypes */
G_FILE	*g_open(char *name, char *mode);
int	g_getc(G_FILE *gp);
int	g_gets(char str[], int maximum, G_FILE *gp);
int	g_putc(int khar, G_FILE *gp);
int	g_puts(char str[], G_FILE *gp);
size_t	g_read(void *buf, size_t itemsize, size_t numitems, G_FILE *gp);
size_t	g_write(void *buf, size_t itemsize, size_t numitems, G_FILE *gp);
int	g_seek(G_FILE *gp, long offset, int whence);
int	g_close(G_FILE *gp);
int	g_profile(int command, int level);

#define	g_flush(f)	g_flushbuf(-1,(f))
#define	g_rewind(f)	g_seek((f),0L,SEEK_SET)

		/* Application programs do NOT normally call the
		following two routines */

int	g_fillbuf(G_FILE *gp);
int	g_flushbuf(int more, G_FILE *gp);

/* ---------------------------------------------------------------------
   Our G_FILE Stream I/O functions:

			g_open()
			g_fillbuf()
			g_getc()
			g_gets()
			g_putc()
			g_puts()
			g_read()
			g_write()
			g_seek()
			g_flush()
			g_close()
			g_dump()
			g_profile()
   --------------------------------------------------------------------- */

G_FILE	*g_open(char *name, char *mode)
		/* This function does the same thing as fopen() except
	that we DO NOT support the "b" option, i.e. mode must
	be "r", "w", or "a" only and is "binary" (as God intended).
	Note that the character buffer is NOT allocated here but will
	be allocated on the first READ or WRITE. */
	{
	int	g_desc;
	char	mesg[128];
	long	offset;
	G_FILE	*gp;

	if ( (g_prof_on) && (g_prof_fileopen) && (g_prof_level >= 2) )
		fprintf(g_pf, 
		  "\n    called: g_gopen(\"%s\",\"%s\") ", name, mode);

	if ( (*mode != 'r') && (*mode != 'w') && (*mode != 'a') )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_open: *** invalid mode=\"%s\" *** \n\n", mode);
		write(2, mesg, strlen(mesg));
#endif
		return((G_FILE *)NULL);	/* failure */
		}
			/* search for an unused slot */
	for (gp = &g_iob[0] ; gp < &g_iob[ G_OPEN_MAX ] ; gp++)
		{
		if ( (gp->flag & (G_READ | G_WRITE)) == 0)
			break;	/* found a free slot */
		}
	if (gp >= &g_iob[ G_OPEN_MAX ])
		{
#if defined(DEBUG2)
		sprintf(mesg,
	"\n\ng_open: *** max number of G_FILES already open  *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return((G_FILE *)NULL);	/* failure */
		}
			/* at this point we have an unused slot */
	if (*mode == 'w')
		{
			/* this call is actually equivalent to

		g_desc = open(name, O_WRONLY | O_CREAT | O_TRUNC, G_PERMS);

			so if the file exists it will be truncated, if
			it doesn't exist it will be created and opened
			for writing only */
		g_desc = creat(name, G_PERMS);	
		if ( (g_prof_on) && (g_prof_fileopen) )
			fprintf(g_pf, 
			  "\ng_open:        %d=creat(\"%s\",%d)",
				g_desc, name, G_PERMS );
#if defined(DEBUG2)
		if (g_desc == -1)
			{
			sprintf(mesg,
	"\n\ng_open: *** invalid desc=%d from creat(\"%s\",%o) *** \n\n", 
				g_desc, name, G_PERMS);
			write(2, mesg, strlen(mesg));
			}
#endif
		gp->baseposition = 0L;
		}
	else if (*mode == 'a')
		{
			/* try low-level open on name[] first */
		g_desc = open(name, O_WRONLY, 0);
		if ( (g_prof_on) && (g_prof_fileopen) )
			fprintf(g_pf, 
			  "\ng_open:        %d=open(\"%s\",O_WRONLY,0)",
				g_desc, name);
		if (g_desc == -1)
			{
			g_desc = creat(name, G_PERMS);
			if ( (g_prof_on) && (g_prof_fileopen) )
				fprintf(g_pf, 
				  "\ng_open:        %d=creat(\"%s\",%d)",
					g_desc, name, G_PERMS);
			}
#if defined(DEBUG2)
		if (g_desc == -1)
			{
			sprintf(mesg,
	"\n\ng_open: *** invalid desc=%d from creat(\"%s\",%o) *** \n\n", 
				g_desc, name, G_PERMS);
			write(2, mesg, strlen(mesg));
			}
#endif
		offset = lseek(g_desc, 0L, SEEK_END);
		gp->baseposition = offset;
		if ( (g_prof_on) && (g_prof_fileopen) )
			fprintf(g_pf, 
			  "\ng_open:        %8ld=lseek(%d, 0L, SEEK_END)", 
				(long)offset, g_desc);
		}
	else
		{
			/* do low-level open on name[] */
		g_desc = open(name, O_RDONLY, 0);
		if ( (g_prof_on) && (g_prof_fileopen) )
			fprintf(g_pf, 
			  "\ng_open:        %d=open(\"%s\",O_RDONLY,0)",
				g_desc, name);

#if defined(DEBUG2)
		if (g_desc == -1)
			{
			sprintf(mesg,
  "\n\ng_open: *** invalid desc=%d from open(\"%s\",O_READONLY) *** \n\n", 
				g_desc, name);
			write(2, mesg, strlen(mesg));
			}
#endif
		gp->baseposition = 0L;
		}
			/* check the validity of the descriptor.. */
	if (g_desc == -1)
		{
		return((G_FILE *)NULL);	/* failure */
		}
			/* ..then initialize record and return */
	gp->fd = g_desc;
	gp->cnt = 0;		/* no meaning since buffer is.. */
	gp->base = NULL;	/* ..not yet allocated */
	gp->ptr = NULL;		/* nothing to point to */
	gp->flag = (*mode == 'r') ? G_READ : G_WRITE;

	return(gp);
	}  /* end of g_open */

/* --------------------------------------------------------------------- */

int	g_fillbuf(G_FILE *gp)
		/* This function first checks if the buffer has been
	allocated (if not it allocates it) and then tries to fill the 
	buffer. It returns the first character it got or EOF if
	at end of file or an error occurred. The caller MUST update
	g_iob[].baseposition BEFORE calling this routine as 
	follows: 
		gp->baseposition = gp->baseposition +
					(long)num_read(gp); */
	{
	char	mesg[128];
	int	khar, bufsize;
				/* Insurance */
	if ( (gp < &g_iob[0]) || (gp >= &g_iob[ G_OPEN_MAX ]) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_fillbuf: *** G_FILE pointer out of range *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(EOF);	/* generic failure */
		}
			/* check if flags are correct */
	if ( ((gp->flag) & (G_READ | G_EOF | G_ERR)) != G_READ)
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_fillbuf: *** unnacceptable flag=%d *** \n\n",
			gp->flag);
		write(2, mesg, strlen(mesg));
#endif
		return(EOF);	/* either file is only WRITEable
				or an EOF or ERRor has occurred 
				on the file */
		}
				/* set buffer size to 1 for an
				UNbuffered file (such as stderr) */
	bufsize = ((gp->flag) & G_UNBUF) ? 1 : G_BUFSIZ;
			/* check if buffer has been allocated */
	if ((gp->base) == NULL)	/* no buffer yet */
		{
		gp->base = malloc((size_t)bufsize);
		if ((gp->base) == NULL)
			{
			if ( (g_prof_on) && (g_prof_fileopen) )
				fprintf(g_pf, 
				  "\ng_fillbuf:  NULL=malloc(%d)", bufsize);
#if defined(DEBUG2)
			sprintf(mesg,
			  "\n\ng_fillbuf: *** malloc(%d) failed *** \n\n", 
				bufsize);
			write(2, mesg, strlen(mesg));
#endif
			return(EOF);	/* malloc failed */
			}
		else
			{
			if ( (g_prof_on) && (g_prof_fileopen) )
				fprintf(g_pf, 
				  "\ng_fillbuf:       malloc(%d) ok", bufsize);
#if defined(DEBUG3)
			sprintf(mesg,
			"\n\ng_fillbuf: *** malloc(%d) successful *** \n\n", 
				bufsize);
			write(2, mesg, strlen(mesg));
#endif
			}
		}
	gp->ptr = gp->base;	/* for G_READ: we set
				pointer to nextchar to base, and
				do low-level read of up to bufsize chars */
	gp->cnt = (int)read(gp->fd, gp->ptr, bufsize);
	if ( (g_prof_on) && (g_prof_fileopen) )
		fprintf(g_pf, 
		  "\ng_fillbuf:  %4d=read(%2d,char *,%4d) baseposition=%8ld",
				gp->cnt, gp->fd, bufsize, gp->baseposition);
	if ((gp->cnt) < 0)
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_fillbuf: *** %d=read(,,%d) failed *** \n\n", 
			gp->cnt, bufsize);
		write(2, mesg, strlen(mesg));
#endif
		if ( (g_prof_on) && (g_prof_fileopen) )
			fprintf(g_pf, " ERR");
		gp->flag = (gp->flag) | G_ERR;	/* read() error */
		gp->cnt = 0;
		return(EOF);
		}
	else if ((gp->cnt) == 0)
		{
		if ( (g_prof_on) && (g_prof_fileopen) )
			fprintf(g_pf, " EOF");
		gp->flag = (gp->flag) | G_EOF;	/* at EOF */
		gp->cnt = -1;
		return(EOF);
		}
	else
		{
		khar = (*(gp->ptr)) & 0x0ff;
		(gp->cnt)--;	/* decrement count of chars left */
		(gp->ptr)++;	/* increment pointer */
		}

	return(khar);
	}  /* end of g_fillbuf */

/* --------------------------------------------------------------------- */

int	g_getc(G_FILE *gp)
		/* This function does the same thing as fgetc() */
	{
	int	khar;
	char	mesg[128];
				/* Insurance */
	if ( (gp < &g_iob[0]) || (gp >= &g_iob[ G_OPEN_MAX ]) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_getc: *** G_FILE pointer out of range *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(EOF);	/* generic failure */
		}

	if (gp == g_stdin)
		g_flush(g_stdout);  /* SYNC with standard output */

	if ( (g_prof_on) && (g_prof_fileopen) && (g_prof_level >= 4) )
		fprintf(g_pf, 
		  "\n    called: g_getc(gp->fd=%d) ", gp->fd);

	if (gp->cnt > 0)
		{
		khar = (*(gp->ptr)) & 0xff;
		(gp->cnt)--;	/* decrement count */
		(gp->ptr)++;	/* increment pointer */
		}
	else			/* khar may be NEGATIVE here */
		{
		gp->baseposition = gp->baseposition +
			(long)num_read(gp);
		khar = g_fillbuf(gp);
		}

	return(khar);
	}  /* end of g_getc */

/* --------------------------------------------------------------------- */

int	g_gets(char str[], int maximum, G_FILE *gp)
		/* There is an OBVIOUS INEFFICIENCY here but for
	now this routine is used primarily for testing g_getc().
	Note that '\n' on input is converted to '\0' in the
	string variable. */
	{
	int	i, khar;
	char	mesg[128];
				/* Insurance */
	if ( (gp < &g_iob[0]) || (gp >= &g_iob[ G_OPEN_MAX ]) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_gets: *** G_FILE pointer out of range *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(EOF);	/* generic failure */
		}

	if (gp == g_stdin)
		g_flush(g_stdout);  /* SYNC with standard output */

	if ( (g_prof_on) && (g_prof_fileopen) && (g_prof_level >= 3) )
		fprintf(g_pf, 
		  "\n    called: g_gets(str[],max=%d,gp->fd=%d) ", 
			maximum, gp->fd);

	i = 0;
	while ( ((--maximum) > 0) && ((khar = g_getc(gp)) != EOF) )
		{
		str[i] = (char)(khar & 0x0ff);
		if (str[i] == '\n')
			break;
		i++;
		}
	str[i] = '\0';		/* terminate string */
	return(i);		/* number of characters */
	}  /* end of g_gets */

/* --------------------------------------------------------------------- */

size_t	g_read(void *buf, size_t itemsize, size_t numitems, G_FILE *gp)
		/* This function does the same thing as fread() */
	{
	int	i, khar;
	char	mesg[128];
	long	totalbytes;
				/* Insurance */
	if ( (gp < &g_iob[0]) || (gp >= &g_iob[ G_OPEN_MAX ]) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_read: *** G_FILE pointer out of range *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(0);	/* generic failure */
		}

	if (gp == g_stdin)
		g_flush(g_stdout);  /* SYNC with standard output */

	if ( (g_prof_on) && (g_prof_fileopen) && (g_prof_level >= 2) )
		fprintf(g_pf, 
		  "\n    called: g_read(buf[],%d,%d,gp->fd=%d) ",
			(int)itemsize, (int)numitems, gp->fd);

	if (itemsize == 0)	/* Insurance */
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_read: *** itemsize is ZERO *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(0);	/* generic failure */
		}

	i = 0;
	totalbytes = (long)itemsize * (long)numitems;
	while (totalbytes > 0)
		{
		if ((gp->cnt) > 0)
			{
			khar = (*(gp->ptr)) & 0xff;
			((unsigned char *)buf)[i++] = (unsigned char)khar;
			(gp->cnt)--;	/* decrement count */
			(gp->ptr)++;	/* increment pointer */
			}
		else		/* khar may be NEGATIVE here */
			{
			gp->baseposition = gp->baseposition +
				(long)num_read(gp);
			khar = g_fillbuf(gp);
			if (khar < 0)
				return(0);
			((unsigned char *)buf)[i++] = (unsigned char)khar;
			}
		totalbytes--;
		}  /* end of while */

	return(i/(int)itemsize);
	}  /* end of g_read */

/* --------------------------------------------------------------------- */

int	g_flushbuf(int more, G_FILE *gp)
		/* This function first checks if the buffer has been
	allocated (if not it allocates it) and then tries to write
	the buffer out. If successful, it updates the baseposition
	for the buffer, and if more is non-negative, more is converted 
	to a character and put in the buffer and the routine returns 0, 
	otherwise it returns -1 */
	{
	int	ret, bufsize;
	int	num_written;
	char	mesg[128];
				/* Insurance */
	if ( (gp < &g_iob[0]) || (gp >= &g_iob[ G_OPEN_MAX ]) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_flushbuf: *** G_FILE pointer out of range *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(-1);	/* generic failure */
		}
			/* check if flags are correct */
	if ( ((gp->flag) & (G_WRITE | G_EOF | G_ERR)) != G_WRITE)
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_flushbuf: *** unnacceptable flag=%d *** \n\n",
			gp->flag);
		write(2, mesg, strlen(mesg));
#endif
		return(EOF);	/* either file is only READable
				or an EOF or ERROR has occurred 
				on the file */
		}
				/* set buffer size to 1 for an
				UNbuffered file (such as stderr) */
	bufsize = ((gp->flag) & G_UNBUF) ? 1 : G_BUFSIZ;
			/* check if buffer has been allocated */
	if ((gp->base) == NULL)	/* no buffer yet */
		{
		gp->base = malloc((size_t)bufsize);
		if ((gp->base) == NULL)
			{
			if ( (g_prof_on) && (g_prof_fileopen) )
				fprintf(g_pf, 
				  "\ng_flushbuf: NULL=malloc(%d)", bufsize);
#if defined(DEBUG2)
			sprintf(mesg,
			  "\n\ng_flushbuf: *** malloc(%d) failed *** \n\n", 
				bufsize);
			write(2, mesg, strlen(mesg));
#endif
			return(EOF);	/* malloc failed */
			}
		else
			{
			if ( (g_prof_on) && (g_prof_fileopen) )
				fprintf(g_pf, 
				  "\ng_flushbuf:      malloc(%d) ok", bufsize);
#if defined(DEBUG3)
			sprintf(mesg,
			"\n\ng_flushbuf: *** malloc(%d) successful *** \n\n", 
				bufsize);
			write(2, mesg, strlen(mesg));
#endif
			}
		gp->ptr = gp->base;	/* for G_WRITE: we set
				pointer to nextchar to base and
				count of empty slots to bufsize */
		gp->cnt = bufsize;
		}
	if ((gp->cnt) < bufsize)
		{
			/* do low-level write of remaining chars */
		num_written = (int)write(gp->fd, gp->base, 
			bufsize - (gp->cnt));
		if (num_written < (bufsize - (gp->cnt)))
			{
#if defined(DEBUG2)
			sprintf(mesg,
			  "\n\ng_flushbuf: *** %d=write(,,%d) failed *** \n\n", 
			num_written, bufsize - (gp->cnt));
			write(2, mesg, strlen(mesg));
#endif
			if ( (g_prof_on) && (g_prof_fileopen) )
				fprintf(g_pf, 
	  "\ng_flushbuf: %4d=write(%d,char *,%4d) baseposition=%8ld ERR",
				num_written, gp->fd, bufsize - (gp->cnt), 
				gp->baseposition);

			gp->flag = (gp->flag) | G_ERR;	/* write() error */
			gp->ptr = gp->base;
			gp->cnt = bufsize;
			gp->cnt = 0;
			return(EOF);
			}
		gp->baseposition = gp->baseposition + num_written;

			/* this MUST go between */
		if ( (g_prof_on) && (g_prof_fileopen) )
			fprintf(g_pf, 
		  "\ng_flushbuf: %4d=write(%d,char *,%4d) baseposition=%8ld",
				num_written, gp->fd, bufsize - (gp->cnt), 
				gp->baseposition);

		gp->ptr = gp->base;
		gp->cnt = bufsize;
		}

			/* put in the spare character if it
			is a REAL character */
	if (more >= 0)
		{
				/* convert */
		*(gp->ptr) = (char)(more & 0xff);
		(gp->cnt)--;	/* decrement count */
		(gp->ptr)++;	/* increment pointer */
		}
	return(0);
	}  /* end of g_flushbuf */

/* --------------------------------------------------------------------- */

int	g_putc(int khar, G_FILE *gp)
		/* This function does the same thing as fputc() */
	{
	int	ret;
	char	mesg[128];
				/* Insurance */
	if ( (gp < &g_iob[0]) || (gp >= &g_iob[ G_OPEN_MAX ]) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_putc: *** G_FILE pointer out of range *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(-1);	/* generic failure */
		}

	if ( (g_prof_on) && (g_prof_fileopen) && (g_prof_level >= 4) )
		fprintf(g_pf, 
		  "\n    called: g_putc(%d,gp->fd=%d) ", khar, gp->fd);

				/* check if there is room in buffer */
	if (gp->cnt > 0)
		{
		*(gp->ptr) = (char)(khar & 0xff);
		(gp->cnt)--;	/* decrement count */
		(gp->ptr)++;	/* increment pointer */
		}
	else		
		{
		ret = g_flushbuf(khar, gp);
		if (ret < 0)
			return(EOF);	/* error */
		}
	return(khar);		/* return the value written */
	}  /* end of g_putc */

/* --------------------------------------------------------------------- */

int	g_puts(char str[], G_FILE *gp)
		/* There is an OBVIOUS INEFFICIENCY here but for
	now this routine is used primarily for testing g_putc() 
	LIMITATIONS:
			We do NOT currently convert the '\0' of
			  the string to a '\n' */
	{
	int	i;
	char	mesg[128];
				/* Insurance */
	if ( (gp < &g_iob[0]) || (gp >= &g_iob[ G_OPEN_MAX ]) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_puts: *** G_FILE pointer out of range *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(-1);	/* generic failure */
		}

	if ( (g_prof_on) && (g_prof_fileopen) && (g_prof_level >= 3) )
		fprintf(g_pf, 
		  "\n    called: g_puts(str[%d],gp->fd=%d) ", 
			strlen(str), gp->fd);

	for (i =  0 ; i < strlen(str) ; i++)
		g_putc((int)str[i] & 0x0ff, gp);

	return(0);
	}  /* end of g_puts */

/* --------------------------------------------------------------------- */

int	g_seek(G_FILE *gp, long offset, int whence)
		/* This function does the same thing as fseek() with the
	LIMITATIONS:
			It currently ONLY supports G_SEEK_CUR and
			  G_SEEK_SET on files we are READing */
	{
	int	diff;
	off_t	loffset;
	long	current_offset, topposition, desired;
	char	mesg[128];

	if ( (whence != G_SEEK_CUR) && (whence != G_SEEK_SET) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_seek: *** whence=%d NOT YET supported *** \n\n", 
			whence);
		write(2, mesg, strlen(mesg));
#endif
		return(-1);	/* not YET supported */
		}

	if ( (g_prof_on) && (g_prof_fileopen) && (g_prof_level >= 2) )
		fprintf(g_pf, 
		  "\n    called: g_seek(gp->fd=%d,%ld,%d) ", 
				gp->fd, offset, whence);

	if ( (gp < &g_iob[0]) || (gp >= &g_iob[ G_OPEN_MAX ]) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_seek: *** G_FILE pointer out of range *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(-1);	/* generic failure */
		}
			/* check if flags are correct and no ERRs */
	if ( ((gp->flag) & (G_READ | G_ERR)) != G_READ)
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_seek: *** unnacceptable flag=%d *** \n\n",
			gp->flag);
		write(2, mesg, strlen(mesg));
#endif
		return(-1);	/* either file is only WRITEable
				or an EOF or ERROR has occurred 
				on the file */
		}
			/* check for EOFile condition.. */
	if ( ((gp->flag) & G_EOF) == G_EOF)
		{
			/* ..and rewind if necessary */
		loffset = lseek(gp->fd, (off_t)0, SEEK_SET);
		if ( (g_prof_on) && (g_prof_fileopen) )
			fprintf(g_pf, 
			  "\ng_seek: %8ld=lseek(%d,%ld,%d) (rewind) ", 
				(long)loffset, gp->fd, 0, SEEK_SET);
		if (loffset < 0)
			{
			sprintf(mesg,
		"\n\ng_seek: *** %ld=lseek(%d,%ld,%d) failed *** \n\n",
				(long)loffset, gp->fd, 0, SEEK_SET);
			write(2, mesg, strlen(mesg));

			if ( (g_prof_on) && (g_prof_fileopen) )
				fprintf(g_pf, " ERR");
			gp->flag = gp->flag | G_ERR;
			return(-1);	/* failure of lseek() */
			}
		gp->flag = (gp->flag) & ~G_EOF;	/* reset EOF flag */
		gp->baseposition = (long)0;
		gp->cnt = 0;
		gp->ptr = gp->base;
		}
	
	current_offset = (gp->baseposition) + (long)num_read(gp);
	topposition = current_offset + (long)(gp->cnt);
	if (whence == G_SEEK_SET)
		desired = offset;
	else if (whence == G_SEEK_CUR)
		desired = current_offset + offset;
	if ( (desired >= (gp->baseposition) ) && (desired < topposition) )
		{
			/* desired position is STILL in buffer */
		diff = (int)(desired - current_offset);
		gp->cnt = (gp->cnt) - diff;
		gp->ptr = (char *)(gp->ptr + diff);
		}
	else
		{
		loffset = lseek(gp->fd, (off_t)desired, SEEK_SET);
		if ( (g_prof_on) && (g_prof_fileopen) )
			fprintf(g_pf, 
			  "\ng_seek: %8ld=lseek(%d,%ld,%d) ", 
				(long)loffset, gp->fd, desired, SEEK_SET);
		if (loffset < 0)
			{
			sprintf(mesg,
		"\n\ng_seek: *** %ld=lseek(%d,%ld,%d) failed *** \n\n",
				(long)loffset, gp->fd, desired, SEEK_SET);
			write(2, mesg, strlen(mesg));

			if ( (g_prof_on) && (g_prof_fileopen) )
				fprintf(g_pf, " ERR");
			gp->flag = (gp->flag) | G_ERR;	/* seek error */
			return(-1);	/* failure of lseek() */
			}
#if defined(DEBUG2)
		if ((long)loffset != desired)
			{
			sprintf(mesg,
	"\n\ng_seek: *** loffset=%ld differs from %ld *** \n\n",
				(long)loffset, desired);
			write(2, mesg, strlen(mesg));
			}
#endif
		gp->baseposition = (long)loffset;
		gp->cnt = 0;
		gp->ptr = gp->base;
		}

	return(0);
	}  /* end of g_seek */

/* --------------------------------------------------------------------- */

int	g_close(G_FILE *gp)
		/* This function does the same thing as fclose() */
	{
	int	ret;
	char	mesg[128];
				/* Insurance */
	if ( (gp < &g_iob[0]) || (gp >= &g_iob[ G_OPEN_MAX ]) )
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_close: *** G_FILE pointer out of range *** \n\n");
		write(2, mesg, strlen(mesg));
#endif
		return(-1);	/* generic failure */
		}

	if ( (g_prof_on) && (g_prof_fileopen) && (g_prof_level >= 2) )
		fprintf(g_pf, 
		  "\n    called: g_close(gp->fd=%d) ", gp->fd);

			/* check if buffer has been flushed on
		the G_FILE we are writing */
	if ( (((gp->flag) & G_WRITE) == G_WRITE) &&
		((gp->base) != NULL) &&
		((gp->base) != (gp->ptr)) ) 
		{
				/* the ' ' is a dummy since we
			will soon set gp->flag = 0 */
		ret = g_flushbuf(' ', gp);
		}
			/* do low-level close of descriptor */
	ret = close(gp->fd);
	if ( (g_prof_on) && (g_prof_fileopen) )
		fprintf(g_pf, "\ng_close:       %d=close(%d)", ret, gp->fd);
			/* de-allocate slot and free memory */
	gp->fd = 0;
	gp->cnt = 0;
	free(gp->base);
	gp->base = NULL;
	gp->ptr = NULL;	
	gp->flag = 0;	/* slot is now marked UNUSED */
	if (ret == 0)
		{
		return(0);		/* ok */
		}
	else
		{
#if defined(DEBUG2)
		sprintf(mesg,
		  "\n\ng_close: *** %d=close(%d) failed *** \n\n",
			ret, gp->fd);
		write(2, mesg, strlen(mesg));
#endif
		return(-1);		/* error */
		}
	}  /* end of g_close */

/* --------------------------------------------------------------------- */

void	g_profflush(void)
		/* This flushes the profiling logfile "g_stdio.log"
	at exit (it is installed by the atexit() function) AND any
	other G_FILEs open for G_WRITE which have non-empty buffers */
	{
	int	i;

	for (i = 0 ; i < G_OPEN_MAX ; i++)
		{
		if ( ( (g_iob[i].flag & (G_WRITE | G_ERR)) == G_WRITE) &&
			(g_iob[i].cnt > 0) )
			{
			g_flushbuf(-1, &g_iob[i]);
			}
		}
	g_prof_on = 0;
	if (g_prof_fileopen)
		{
		fprintf(g_pf, "\n\ng_profflush: called by atexit() \n");
		fflush(g_pf);
		fclose(g_pf);
		g_prof_fileopen = 0;
		}
	}  /* end of g_profflush */

/* --------------------------------------------------------------------- */

int	g_profile(int command, int level)
		/* This function controls profiling of our G_FILE
	I/O routines to the profiling logfile "g_stdio.log" whenever
	a low-level Unix system call:
					creat()
					open()
					read()
					write()
					lseek()
					unlink()
					close()

	is made (EXCEPT by the PROFiling or DEBUGging code). 
	Commands are:
		G_PROFILE_START		open logfile and set profiling
					  to given level
		G_PROFILE_SUSPEND	temporarily stop writing log
					  (level ignored)
		G_PROFILE_RESUME	resume writing log at given
					  level unless (level == 0) in
					  which case same level is used
					  as before
		G_PROFILE_STOP 		flush to logfile and close 
					  (level ignored) */
	{
	int	ret;
	char	mesg[128];

	switch(command) {

	  case G_PROFILE_START:
		if (g_prof_fileopen)
			{
			fclose(g_pf);
			g_prof_fileopen = 0;
			}
		g_pf = fopen(g_prof_filename, "w");
		if (g_pf == (FILE *)NULL)
			{
			sprintf(mesg,
	"\n\ng_profile: *** fopen(\"%s\",\"w\") failed *** \n\n",
				g_prof_filename);
			write(2, mesg, strlen(mesg));
			return(-1);
			}
		g_prof_on = 1;		/* profiling now ON */
		g_prof_fileopen = 1;
		if ( (level >= 1) && (level <= 9) )
			g_prof_level = level;
				/* register with exit() */
		ret = atexit(g_profflush);
		if (ret)
			{
			sprintf(mesg,
	"\n\ng_profile: *** %d=atexit(g_profflush) failed *** \n\n", ret);
			write(2, mesg, strlen(mesg));
			return(-1);
			}
		fprintf(g_pf, "\ng_profile: profiling was STARTed: \n");
		break;

	  case G_PROFILE_SUSPEND:
		g_prof_on = 0;
		if (g_prof_fileopen)
			{
			fprintf(g_pf, 
				"\ng_profile: profiling was SUSPENDed: \n");
			}
		break;

	  case G_PROFILE_RESUME:
		if (g_prof_fileopen)
			{
			fprintf(g_pf, 
				"\ng_profile: profiling was RESUMed: \n");
			g_prof_on = 1;
			if ( (level >= 1) && (level <= 9) )
				g_prof_level = level;
			}
		break;

	  case G_PROFILE_STOP:
		g_prof_on = 0;
		if (g_prof_fileopen)
			{
			fprintf(g_pf, 
				"\ng_profile: profiling was STOPped: \n");
			fflush(g_pf);
			fclose(g_pf);
			g_prof_fileopen = 0;
			}
		break;

	  default:
		sprintf(mesg,
	"\n\ng_profile: *** invalid command=%d *** \n\n", command);
		write(2, mesg, strlen(mesg));
		return(-1);
	  }  /* end of switch */

	return(0);
	}  /* end of g_profile */


