
			/* g_stdio.h: this is the header file for
		our stream-type G_FILE I/O functions:  

			g_open()
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

						Marc Thomas */

#if !defined(_G_STDIO_H)
#define	_G_STDIO_H	1

#include <fcntl.h>
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

extern	G_FILE	g_iob[];

#define	g_stdin		(&g_iob[0])
#define	g_stdout	(&g_iob[1])
#define	g_stderr	(&g_iob[2])

				/* G_FILE .flag bits */
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

#define	G_PROFILE_START		1
#define	G_PROFILE_SUSPEND	2
#define	G_PROFILE_RESUME	3
#define	G_PROFILE_STOP 		4

			/* Function Prototypes */
extern	G_FILE	*g_open(char *name, char *mode);
extern	int	g_getc(G_FILE *gp);
extern	int	g_gets(char str[], int maximum, G_FILE *gp);
extern	int	g_putc(int khar, G_FILE *gp);
extern	int	g_puts(char str[], G_FILE *gp);
extern	size_t	g_read(void *buf, size_t itemsize, size_t numitems, 
			G_FILE *gp);
extern	size_t	g_write(void *buf, size_t itemsize, size_t numitems, 
			G_FILE *gp);
extern	int	g_seek(G_FILE *gp, long offset, int whence);
extern	int	g_close(G_FILE *gp);
extern	int	g_profile(int command, int level);

#define	g_flush(f)	g_flushbuf(-1,f)
#define	g_rewind(f)	g_seek(f,0L,SEEK_SET)

		/* Application programs do NOT normally call the
		following two routines */

extern	int	g_fillbuf(G_FILE *gp);
extern	int	g_flushbuf(int more, G_FILE *gp);

#endif

