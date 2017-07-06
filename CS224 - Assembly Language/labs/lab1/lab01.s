# lab01.s 
# a program to demonstrate the structure of a MIPS source program for lab01
# take this file and modify it
#
# Usage:
#         $ spim -f lab01.s

     .data                   # what follows is in data segment of executable
stuff: .asciiz "The sum of "    # the z adds a trailing \0 null char
stuff1: .asciiz " is "    # the z adds a trailing \0 null char
stuff2: .asciiz " and "    # the z adds a trailing \0 null char
dot:   .byte 46              # 46 is the ascii-decimal equivalent 
lf:    .byte 10              # 10 is an ascii linefeed 

     .text                   # code follows 
main:
     li $t0, 5
     li $t1, 10
     addu $t2, $t1, $t0

     la $a0, stuff           # stuff is an address of a string
     li $v0, 4               # 4 is syscall to print a string
     syscall                 # execute the call 

     move $a0, $t0           # stuff is an address of a string
     li $v0, 1               # 4 is syscall to print a string
     syscall                 # execute the call 
     
     la $a0, stuff2           # stuff is an address of a string
     li $v0, 4               # 4 is syscall to print a string
     syscall                 # execute the call 

     move $a0, $t1           # stuff is an address of a string
     li $v0, 1               # 4 is syscall to print a string
     syscall                 # execute the call 

     la $a0, stuff1           # stuff is an address of a string
     li $v0, 4               # 4 is syscall to print a string
     syscall                 # execute the call 

     move $a0, $t2           # stuff is an address of a string
     li $v0, 1               # 4 is syscall to print a string
     syscall                 # execute the call 

     lb $a0, dot             # load the value of the ascii dot into a0 
     li $v0, 11              # 11 is syscall to print a char 
     syscall                 # execute the call 

     lb $a0, lf              # load the value of the linefeed into a0 
     syscall                 # execute the call 

     li $v0, 10              # 10 is system call to exit
     syscall                 # execute the call 

