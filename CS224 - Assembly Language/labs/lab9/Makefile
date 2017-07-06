# Makefile for nasm/yasm assembly code in examples for week09

all:	myecho  fac  foo  fib readloop hw09 lab09

# demonstrate ATT x86-64 syntax and use of compiler, linker and loader
foo:		foo.c
	gcc -S foo.c          # compiler front-end produces ATT x86-64 assembly foo.s
	as -o foo.o foo.s     # as assembler produces machine code foo.o 
	gcc -o foo foo.s -v 2>out # calls linker to produce ELF64 executable; verbose

# this code tests some stuff covered in hw09
hw09:		hw09.asm
	nasm -f elf64 -g -F dwarf hw09.asm
	gcc -o hw09 hw09.o -lc

lab09:		lab09.asm
	nasm -f elf64 -g -F dwarf lab09.asm
	gcc -o lab09 lab09.o -lc

# implement fib function with ATT x86-64 syntax to demonstrate compiler limits
fib:		fib.c
	gcc -S fib.c
	gcc -o fib fib.s

myecho:		myecho.asm
	nasm -f elf64 -g -F dwarf myecho.asm
	gcc -o myecho myecho.o -lc

fac:		fac.asm
	nasm -f elf64 -g -F dwarf fac.asm
	gcc -o fac fac.o -lc

lab08:		lab08.asm
	nasm -f elf64 -g -F dwarf lab08.asm
	gcc -o lab08 lab08.o -lc

readloop:		readloop.asm
	nasm -f elf64 -g -F dwarf readloop.asm
	gcc -o readloop readloop.o -lc

clean:  
	rm -f *.o

veryclean:  
	rm -f *.o
	rm -f myecho fac a.out foo foo.s fib.s fib readloop lab08
