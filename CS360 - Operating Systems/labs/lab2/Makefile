# CMPS 360 Week 02
# Makefile for code in /examples/week02

all:	sigmask sig-fork sig-hand1 sig-hand2 perror readwrite startstop

# this source demonstrates sending signals
sigmask:	sigmask.c
	gcc -o sigmask sigmask.c

# this source demonstrates sending signals from child to parent
sig-fork:	sig-fork.c 
	gcc -o sig-fork sig-fork.c

# this source demonstrates simple signal handling 
sig-hand1:	sig-hand1.c 
	gcc -o sig-hand1 sig-hand1.c

# demonstrates signal handling within a signal handler
sig-hand2:	sig-hand2.c 
	gcc -o sig-hand2 sig-hand2.c

# demonstrates use of perror
perror:	perror.c 
	gcc -o perror perror.c

# demonstrates use of low-level read/write utilities
readwrite:	readwrite.c 
	gcc -o readwrite readwrite.c

# this source demonstrates competition between two signal handlers
startstop:	startstop.c
	gcc -g -o startstop startstop.c

clean:  # start from scratch 
	rm -f *.o
	rm -f sigmask sig-fork sig-hand1 sig-hand2 perror readwrite startstop
