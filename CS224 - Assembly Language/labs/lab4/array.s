.data
    arr: .space 30
.text
.globl main
main:
    li $t0 1
    li $t1 30
    li $t2 0
    la $t3 arr
loop:
    beq $t2 $t1 endLoop
    li $v0 1
    move $a0 $t0
    syscall
    li $v0 11
    li $a0 32
    syscall
    sw $t0 0($t3)
    addi $t0 $t0 2
    addi $t3 $t3 4
    addi $t2 $t2 1
    j loop
endLoop:
    li $v0 11
    li $a0 10
    syscall

    li $t1 30
    li $t2 0
    li $t3 3
    li $t4 7
    la $t6 arr
loop2:
    beq $t2 $t1 exit
    lw $t0 0($t6)
    div $t0 $t3
    mfhi $t5
    beq $t5 $zero divisible
    div $t0 $t4
    mfhi $t5
    beq $t5 $zero divisible
endCases:
    addi $t2 $t2 1
    addi $t6 $t6 4
    j loop2
exit:
    li $v0 11
    li $a0 10
    syscall
    li $v0 10
    syscall
divisible:
    li $v0 1
    move $a0 $t0
    syscall
    li $v0 11
    li $a0 32
    syscall
    j endCases
