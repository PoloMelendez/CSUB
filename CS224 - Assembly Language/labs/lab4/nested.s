.data
.text
.globl main
main:
    li $t0 6
    li $t1 0
    li $t2 0
    li $t3 1
    li $t4 6

outer:  beq $t1 $t4 exit        #for i: 0 - 5
inner:  beq $t2 $t3 continue    #for j: 0 - i
    li $a0 88
    li $v0 11
    syscall
    li $a0 32
    syscall
    addi $t2 $t2 1
    j inner
continue:
    addi $t1 $t1 1
    addi $t3 $t3 1
    li $t2 0
    li $a0 10
    li $v0 11
    syscall
    j outer
exit:
    li $a0 10
    li $v0 11
    syscall
    li $v0 10
    syscall    
