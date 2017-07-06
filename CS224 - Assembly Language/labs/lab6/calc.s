# filename: calc.s
# description: cs 224 lab 06 

# prompt the user to enter two integers, sum integers and display the result

.text
.globl read 
.globl main 
.ent  main

main:

  jal read
  li  $v0,10       # 10 is exit system call
  syscall    

.end  main


read:

  addi $sp $sp -4
  sw $ra 0($sp)

  # prompt for first integer
  move $s0, $a0
  move $s1, $a1
  li $t0, 4
  blt $s0, $t0, exit_on_error
  lw $a0, 4($s1)
  jal atoi
  move $s2, $v0

  lw $t0 8($s1)
  lbu $s3, 0($t0)

  lw $a0, 12($s1)
  jal atoi
  move $s4, $v0
  
  li $t3 45         #sub
  li $t4 43         #add
  li $t5 42         #mult
  beq $s3 $t3 subtraction
  beq $s3 $t4 addition
  beq $s3 $t5 multiplication
calculated:

  move $t0, $v0
  move $a0, $t0    # move the result into register $a0
  li $v0, 1        # setup syscall 1 (print_int)
  syscall          # make the call to display the integer 
   
  li $v0, 4        # print newline
  la $a0, newline  
  syscall

  li $t1, 2
  div $t0, $t1
  mfhi $t2
  beq $t2 $zero evenNum
  j oddNum
evenOdd:

  lw $ra 0($sp)
  addi $sp $sp 4 
  jr $ra

evenNum:
  li $v0 4
  la $a0 even
  syscall
  j evenOdd

oddNum:
  li $v0 4
  la $a0 odd
  syscall
  j evenOdd

subtraction:
  sub $v0 $s2 $s4
  j calculated

addition:
  add $v0 $s2 $s4
  j calculated

multiplication:
  mult $s2 $s4
  mflo $v0
  j calculated

atoi:
  move $v0, $zero
  li $t0, 1
  lbu $t1, 0($a0)
  bne $t1, 45, digit
  li $t0, -1
  addu $a0, $a0, 1

digit:
    # read character
    lbu $t1, 0($a0)

    # finish when non-digit encountered
    bltu $t1, 48, finish
    bgtu $t1, 57, finish

    # translate character into digit
    subu $t1, $t1, 48

    # multiply the accumulator by ten
    li $t2, 10
    mult $v0, $t2
    mflo $v0

    # add digit to the accumulator
    add $v0, $v0, $t1

    # next character
    addu $a0, $a0, 1
    b digit

finish:
  mult $v0, $t0
  mflo $v0
  jr $ra

exit_on_error:
  li $v0, 4
  la $a0, usage
  syscall
  li $v0, 10
  syscall

.data

iprompt: .asciiz "Enter an integer [return]:\n"
newline: .asciiz "\n"
usage: .asciiz "Usage: spim -f calc.s <int> <operator> <int>\n"
even: .asciiz "EVEN\n"
odd: .asciiz "ODD\n"
