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
   syscall                      #print the factorials

   move $a0 $s2
   move $a1 $s3
   jal Cnk
   move $a0 $v0
   move $s4 $v0                 #s4 holds nCk
   jal printInt
   li $v0 11
   li $a0 10
   syscall                      #print nCk

   move $a0 $s2
   move $a1 $s3
   jal displayRow
   li $v0 11
   li $a0 10
   syscall                      #print out row n of pascal's

   li $t0 0                     #loop counter
treeLoop: beq $t0 $s2 treeEnd
   move $a0 $t0
   jal displayRow
   li $v0 11
   li $a0 10
   syscall
   addi $t0 $t0 1
   j treeLoop
treeEnd:
       
   li $v0 11
   li $a0 10
   syscall

   li $v0 10
   syscall

displayRow:
    #push
    addi $sp $sp -24
    sw $ra 0($sp)
    sw $s0 4($sp)
    sw $s1 8($sp)
    sw $s2 12($sp)
    sw $a0 16($sp)
    sw $a1 20($sp)
    
    move $s0 $a0                # n
    li $s2 0                    #counter to n
dispLoop: bgt $s2 $s0 dispEnd
    move $a0 $s0
    move $a1 $s2
    jal Cnk
    move $a0 $v0
    li $v0 1
    syscall
    li $a0 32
    li $v0 11
    syscall
    addi $s2 $s2 1
    j dispLoop
dispEnd:    

    #pop
    lw $ra 0($sp)
    lw $s0 4($sp)
    lw $s1 8($sp)
    lw $s2 12($sp)
    lw $a0 16($sp)
    lw $a1 20($sp)
    addi $sp $sp 24
    jr $ra

Cnk:
    addi $sp $sp -32
    sw $ra 0($sp)   
    sw $t0 4($sp)   
    sw $t1 8($sp)   
    sw $t2 12($sp)  
    sw $t3 16($sp)  
    sw $t4 20($sp)  
    sw $t5 24($sp)  
    sw $t6 28($sp)   #push

    move $t0 $a0
    move $t1 $a1
    jal fac
    move $t2 $v0    #holds fac n
    move $a0 $t1
    jal fac
    move $t3 $v0    #holds fac k
    sub $t4 $t0 $t1 #holds n-k
    move $a0 $t4
    jal fac
    move $t5 $v0    #holds fac(n-k)
    mul $t6 $t5 $t3 #holds (n-k)!k!
    div $t2 $t6
    mflo $v0

    lw $ra 0($sp)
    lw $t0 4($sp)
    lw $t1 8($sp)
    lw $t2 12($sp)
    lw $t3 16($sp)
    lw $t4 20($sp)
    lw $t5 24($sp)
    lw $t6 28($sp)
    addi $sp $sp 32  #pop
    jr $ra

fac:                          #fac- factorial(a0) return in v0
    addi $sp $sp -16
    sw $ra 0($sp)
    sw $t0 4($sp)
    sw $t1 8($sp)
    sw $t2 12($sp)

    move $t0 $a0
    li $t1 1                  #product variable
    li $t2 1                  #iterator
facLoop: bgt $t2 $t0 endFac
    mul $t1 $t1 $t2
    addi $t2 $t2 1
    j facLoop
endFac:
    move $v0 $t1
    
    lw $ra 0($sp)
    lw $t0 4($sp)
    lw $t1 8($sp)
    lw $t2 12($sp)
    addi $sp $sp 16
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

