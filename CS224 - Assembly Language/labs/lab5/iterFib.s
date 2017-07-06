.data
msg1:       .asciiz "The fib of "
msg2:       .asciiz " is "
usage_stmt: .asciiz "\nUsage: spim -f iterFib.s <int>\n"

.text
main:
  move $s0, $a0
  move $s1, $a1

  li $t0, 2
  blt $a0, $t0, exit_on_error

  lw $a0, 4($s1)
  jal atoi
  move $s2, $v0

  move $a0, $s2

  move $t1 $a0
  li $t2 0
  li $t3 1
  beq $t1 $zero zeroCase #if n==0, return 0
  beq $t1 $t3 oneCase #if n==1, return 1
loop:   bgt $t0 $t1 end
  add $v0 $t2 $t3
  move $t2 $t3
  move $t3 $v0
  addi $t0 1
  j loop
end:
  move $t0, $v0
  la $a0, msg1
  li $v0, 4
  syscall
  move $a0, $s2
  li $v0, 1
  syscall
  la $a0, msg2
  li $v0, 4
  syscall
  move $a0, $t0
  li $v0, 1
  syscall
  li $a0, 10
  li $v0, 11
  syscall
  li $v0, 10
  syscall

zeroCase:
  li $v0 0
  j end
oneCase:
  li $v0 1
  j end

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

