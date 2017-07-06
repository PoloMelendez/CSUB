# chop.s  
# chops off the end of a string after character 10
#
# Register Usage:
#                   $s0 holds address of string

   .data
mystring: .asciiz "supercalifragilisticexbialadotous"
newline: .asciiz "\n"

   .text
main:

   # load string address
   la $s0, mystring

   # display the original string
   move $a0, $s0
   li $v0, 4
   syscall

   la $a0, newline 
   syscall

   # insert null character at location 10
   sb $zero, 10($s0)

   # print the chopped off string - supercalif
   move $a0, $s0
   syscall

   la $a0, newline 
   syscall

   li $v0, 10
   syscall


