CFLAGS = -I ./include
LIB    = ./libggfonts.so
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

#all: lab2_asteroids
all: lab2test

#lab2_asteroids: lab2_asteroids.c ppm.c log.c
lab2test: lab2test.c ppm.c log.c
	#gcc $(CFLAGS) lab2_asteroids.c log.c -Wall -Wextra $(LFLAGS) -o lab2_asteroids
	gcc $(CFLAGS) lab2test.c log.c -Wall -Wextra $(LFLAGS) -o lab2test

clean:
	#rm -f lab2_asteroids
	rm -f lab2test
	rm -f *.o

