.data
usage:  .asciiz "\nUsage: spim -f gcd.s <int> <int>\n"
.text
.globl main
main:
   move $s0, $a0                #arg count
   move $s1, $a1                #pointer to args array
   li $t0, 3                    #expect 3 args
   li $t1, 1
   li $s2, 6
   li $s3, 4                    #default if no args
   beq $a0, $t1, default        #if no numbers passed, default
   bne $a0, $t0, exit_on_error  #if wrong num of args, usage
   lw $a0, 4($s1)               #load first int arg
   jal atoi
   move $s2, $v0                #now stored arg
   lw $a0, 8($s1)
   jal atoi
   move $s3, $v0                #stored second arg
default:
   slt $t0 $s2 $s3
   move $a0 $s2
   move $a1 $s3
   li $t1 1
   beq $t0 $t1 swap             #if arg1 < arg2, swap
swapped:
   move $s2 $a0
   move $s3 $a1
   jal printInt
   move $a0 $a1
   jal printInt
   li $v0 11
   li $a0 10
   syscall                      #print the swapped or not swapped ints
   
   move $a0 $s2
   jal fac
   move $s4 $v0                 #s4 holds fac(arg1)

   move $a0 $s3
   jal fac
   move $s5 $v0                 #s5 holds fac(arg2)

   move $a0 $s4
   jal printInt
   move $a0 $s5
   jal printInt
   li $v0 11
   li $a0 10
   syscall

   li $v0 10
   syscall

fac:
    move $t0 $a0
    li $t1 1                  #product variable
    li $t2 1                  #iterator
facLoop: bgt $t2 $t0 endFac
    mul $t1 $t1 $t2
    addi $t2 $t2 1
    j facLoop
endFac:
    move $v0 $t1
    jr $ra

exit_on_error:
    li $v0, 4
    la $a0, usage
    syscall
    li $v0, 10
    syscall

atoi:
    move $v0, $zero
    li $t0, 1
    #move $v0, $t0
    lbu $t1, 0($a0)

    bne $t1, 45, digit
    li $t0, -1
    addu $a0, $a0, 1

digit:
    lbu $t1, 0($a0)
    beq $t1, 45, finish
    beq $t1, 0, finish
    bltu $t1, 48, exit_on_error
    bgtu $t1, 57, exit_on_error
    subu $t1, $t1, 48
    li $t2, 10
    mult $v0, $t2
    mflo $v0
    add $v0, $v0, $t1
    addu $a0, $a0, 1
    
    b digit

finish:
    mult $v0, $t0
    mflo $v0
    #beq $v0 $zero exit_on_error
    jr $ra

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

