# The following two macros set up the compiler flags and compiler command

DFLAGS = -DDEBUG -g
CFLAGS = -Wall 
CC = gcc

# Default rule to compile files using the same CC command

% : %.c
	$(CC) $(DFLAGS) $(CFLAGS) $< -o $@

# Start of the target section

PROG = s_sh s_daemon

daemon:	$(PROG)

s_sh:	simple_shell.c
	$(CC) $(DFLAGS) $(CFLAGS) $< -o $@

s_daemon: simple_daemon.c
	$(CC) $(DFLAGS) $(CFLAGS) $< -o $@

clean:
	rm $(PROG)
