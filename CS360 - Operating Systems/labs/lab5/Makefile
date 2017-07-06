# Makefile for week05

all:	echo  create_thread  mine  reader_writer threads1 sample1 sample2 sample3

sample1: sample1.c
	gcc -o sample1 sample1.c -lpthread

sample2: sample2.c
	gcc -o sample2 sample2.c -lpthread

sample3: sample3.c
	gcc -o sample3 sample3.c -lpthread

echo:	echo.c
	gcc -o echo echo.c -lpthread

create_thread:
	gcc -o create_thread create_thread.c -lpthread

mine:
	gcc -o mine mine.c -lpthread

reader_writer:
	gcc -o reader_writer reader_writer.c -lpthread

threads1:
	gcc -o threads1 threads1.c -lpthread

clean:
	rm -f echo create_thread mine reader_writer  threads1 sample1 sample2 sample3
