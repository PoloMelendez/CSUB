CFLAGS = -I ./include
LIB    = ./lib/fmod/libfmodex64.so ./libggfonts.so
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: lab2_rainforest

lab2_rainforest: lab2_rainforest.c ppm.c fmod.c log.c
	gcc $(CFLAGS) lab2_rainforest.c ppm.c log.c fmod.c -Wall -Wextra $(LFLAGS) -o lab2_rainforest

clean:
	rm -f lab2_rainforest
	rm -f *.o

