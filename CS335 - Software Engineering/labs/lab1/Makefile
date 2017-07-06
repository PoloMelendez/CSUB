CFLAGS = -I ./include
LIB    = ./libggfonts.so
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: lab1

lab1: lab1.c timing.c xwin.c log.c
	gcc $(CFLAGS) lab1.c timing.c xwin.c log.c -Wall -Wextra $(LFLAGS) -o lab1

clean:
	rm -f lab1
	rm -f *.o

