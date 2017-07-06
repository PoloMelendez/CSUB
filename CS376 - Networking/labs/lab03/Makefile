# The following two macros set up the compiler flags and compiler command

DFLAGS = -DDEBUG -g
CFLAGS = -Wall 
CC = gcc

# Default rule to compile files using the same CC command

% : %.c
	$(CC) $(DFLAGS) $(CFLAGS) $< -o $@

# Start of the target section

PROG = vcrec vcsend

tcp:	$(PROG)

clean:
	rm $(PROG)
