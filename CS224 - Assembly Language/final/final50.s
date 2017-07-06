.data
usage:  .asciiz "\nUsage: spim -f gcd.s <int> <int>\n"
.text
.globl main
main:
   move $s0, $a0                #arg count
   move $s1, $a1                #pointer to args array
   li $t0, 3                    #expect 3 args
   blt $a0, $t0, exit_on_error  #if too few args, usage statement
   lw $a0, 4($s1)               #load first int arg
   jal atoi
   move $s2, $v0                #now stored arg
   lw $a0, 8($s1)
   jal atoi
   move $s3, $v0                #stored second arg
   slt $t0 $s2 $s3
   move $a0 $s2
   move $a1 $s3
   li $t1 1
   beq $t0 $t1 swap
swapped:
   jal printInt
   move $a0 $a1
   jal printInt
   li $v0 11
   li $a0 10
   syscall

   li $v0 10
   syscall


exit_on_error:
    li $v0, 4
    la $a0, usage
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
    lbu $t1, 0($a0)
    bltu $t1, 48, finish
    bgtu $t1, 57, finish
    subu $t1, $t1, 48
    li $t2, 10
    mult $v0, $t2
    mflo $v0
    add $v0, $v0, $t1
    addu $a0, $a0, 1
    b digit

swap: #Uses t0, a0, a1 - swaps a0 and a1 values
    move $t0 $a0
    move $a0 $a1
    move $a1 $t0
    j swapped

printInt:
    li $v0 1
    syscall
    li $v0 11
    li $a0 32
    syscall
    jr $ra

finish:
    mult $v0, $t0
    mflo $v0
    jr $ra
