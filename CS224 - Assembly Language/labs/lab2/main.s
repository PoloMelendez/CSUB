# filename: main.s
# purpose:  test output facilities in print.s and input facilities in read.s

#  spim> re "main.s"
#  spim> re "printf.s"
#  spim> re "read.s"
#  spim> run
#  spim> exit 

.text
.globl  main
.ent  main

main:
  la $a0, format4
  jal printf
  la $a0, fname_buf
  li $v0, 8
  syscall

  la $a0, format5
  jal printf
  la $a0, lname_buf
  li $v0, 8
  syscall

  la $a0, format6
  jal printf
  li $v0, 5
  syscall
  move $t0, $v0

  la $a0,nl
  jal printf

  la  $a0,fname_buf
  jal printf

  la  $a0,lname_buf
  jal printf       
                   
  move $a0,$t0
  li $v0, 1
  syscall
  
  la $a0,nl
  jal printf


  li  $v0,10
  syscall    

.end  main



.data
format1: 
  .asciiz "Spade\n"        # asciiz adds trailing null byte to string
format2:
  .asciiz "Sam\n"
#  .asciiz "Register $a1 holds: %d\n"
format3:
  .asciiz "999\n" 
#  .asciiz "%d plus %d is %d\n"
format4:
  .asciiz "Enter your first name:\n"
format5:
  .asciiz "Enter your last name:\n"
format6:
  .asciiz "Enter a 3 digit ID:\n"
nl:
  .asciiz "\n"
fname_buf: .space 32
lname_buf: .space 32
