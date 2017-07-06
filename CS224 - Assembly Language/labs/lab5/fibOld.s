.data
msg1:       .asciiz "The fib of "
msg2:       .asciiz " is "
usage_stmt: .asciiz "\nUsage: spim -f gcd.s <int>\n"

.text
.globl main
main:

  move $s0, $a0
  move $s1, $a1

  li $t0, 2
  blt $a0, $t0, exit_on_error

  lw $a0, 4($s1)
  jal atoi
  move $s2, $v0

  move  $a0, $s2        # Put argument (5) in $a0
  jal  fib              # Call factorial function
  move $t0, $v0         # move result to t0

  la $a0, msg1          # address of msg
  li $v0, 4             # syscall 1=print string 
  syscall
  move $a0, $s2
  li $v0, 1
  syscall
  la $a0, msg2
  li $v0, 4
  syscall

  move $a0, $t0         # Move fact result to $a0 to display it
  li   $v0, 1           # syscall 1=print int
  syscall  

  li  $a0, 10           # ascii LF char
  li  $v0, 11           # syscall 1=print char 
  syscall  

  li  $v0, 10           # 10 is the code for exiting
  syscall               # Execute the exit

fib:
  addi $sp, $sp, -12    # Stack frame is 12 bytes long
  sw   $ra, 0($sp)      # Save return address
  sw   $s0, 4($sp)
  sw   $s1, 8($sp)

  add  $s0, $a0, $zero  # Load n
  
  addi $t1, $zero, 1    # Check value in v0
  beq  $s0, $zero, L2   # Branch if n == 0
  beq  $s0, $t1, L3     # Branch if n == 1
  
  addi $a0, $s0, -1     #fib(n-1)
  jal fib
  move $s1, $v0         #first fib result is in $s1

  addi $a0, $s0, -2     #fib(n-2)
  jal fib               #second fib result is in $v0

  add $v0, $v0, $s1     #add the results 
  j     L1              #Jump to code to return

L2:
  li $v0, 0
  j L1

L3:
  li $v0, 1
  j L1

L1:                 
  lw   $ra, 0($sp)      # Restore $ra
  lw   $s0, 4($sp)
  lw   $s1, 8($sp)
  addi $sp, $sp, 12     # Pop stack frame
  jr   $ra              # Return to caller

exit_on_error:
  li $v0, 4
  la $a0, usage_stmt
  syscall
  li $v0, 10
  syscall

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
