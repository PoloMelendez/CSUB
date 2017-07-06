# hello.s
# a sample MIPS program to demonstrate MIPS basics 
# Usage:
#    $ spim -f hello.s

     .data                   # data segment begins here

stuff:  .asciiz "Hello World!\n"

     .text                   # code segment begins here 

main:
     la $a0, stuff
     li $v0, 4               # 4 is syscall to print a string
     syscall                 # execute the call 

     li $a0, 10              # 10 is ascii value for linefeed
     li $v0, 11              # 11 is syscall to print char
     syscall

     li $v0, 10              # 10 is system call to exit
     syscall                 # execute the call 


