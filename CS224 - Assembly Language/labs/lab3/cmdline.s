#  filename: cmdline.s
#  purpose:  demonstrate use of command line arguments
#  
#       $ spim -f cmdline.s  3  4  
#       $ 12 
#
# Cmdline args are accessed via an array of addresses (pointers) to strings. 
# A MIPS address is stored in a word (4 bytes). The first address in the array 
# (offset zero) points to the filename of the executable. The next address in 
# the array (offset 4) points to the first cmdline arg and so on. Since the args
# are read in as strings, if you expect a number you need to convert it.

    .data

errmsg:
    .asciiz   "\nUsage: spim -f cmdline.s <int> <int>\n\n"
newline:
    .asciiz   "\n"
result:
    .space   11
    
.text
.globl main
     
main:

    # immediately move the cmd line info into other registers since $a0 and
    # $a1 are used for all kinds of things

    move $s0, $a0       # a0 holds the number of cmdline args
    move $s1, $a1       # a1 is the base pointer to the array of addresses 
    move $s2, $zero     # zero out my registers just to be sure nothing
    move $s3, $zero     # funky happens later on
    move $s4, $zero
     
    # check if two arguments are given - the total number of args should be
    # three since the filename is the first arg
    li $t0, 2
    ble $a0, $t0, error    # if arg count is <= 2 exit with err msg 
     
    # parse and display the executable filename (offset 0 from the base of $s1)
    lw $a0, 0($s1)      # $s1 holds base address to cmdline structure 
    li $v0, 4           # syscall to display string
    syscall

    # parse the first arg by loading its address (offset 4 from the base of the
    # array) into $a0 and then calling atoi to convert the string into an int
    lw $a0, 4($s1)       
    jal atoi
    move $s2, $v0
     
    # parse the second number in similar fashion
    lw $a0, 8($s1)
    jal atoi
    move $s3, $v0
     
    # multiply the numbers
    mult $s2, $s3
    mflo $s4
     
    # format the result
    la $a0, result
    move $a1, $s4
    jal itoa
     
    # print the result
    li $v0, 4
    la $a0, result
    syscall
    li $v0, 4
    la $a0, newline
    syscall

    # unconditional branch to exit 
    b exit

error:
    # print error message
    li $v0, 4
    la $a0, errmsg
    syscall

exit:
    li $v0, 10
    li $a0, 0
    syscall
     
atoi:
    move $v0, $zero
     
    # detect sign
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
     
itoa:
    subu $sp, $sp, 40
    sd $s0, 0($sp)
    sd $s2, 8($sp)
    sd $s4, 16($sp)
    sd $s6, 24($sp)
    sw $ra, 32($sp)

    move $s0, $a0
    move $s1, $a1

    # print minus if the number is negative
    bgez $s1, char
    li $t0, 45
    sb $t0, 0($s0)
    addu $s0, $s0, 1
    addu $a0, $a0, 1

    # make the number positive
    li $t0, -1
    mult $s1, $t0
    mflo $s1

char:
    # divide number by ten
    li $t0, 10
    divu $s1, $t0
    mflo $s1
    mfhi $t0

    # translate digit to character
    addu $t0, $t0, 48

    # write character
    sb $t0, 0($a0)

    # finish if quotient is zero
    beqz $s1, return

    # move to the next position
    addu $a0, $a0, 1
    b char

return:
    # put null character to the end of the string
    move $t0, $zero
    sb $t0, 1($a0)

    # reverse the string with the digits
    move $a0, $s0
    jal reverse

    ld $s0, 0($sp)
    ld $s2, 8($sp)
    ld $s4, 16($sp)
    ld $s6, 24($sp)
    lw $ra, 32($sp)
    addu $sp, $sp, 40
    jr $ra

reverse:
    # find the last character in the string
    move $t0, $a0

next:
    lbu $t1, 0($t0)
    beqz $t1, last
    addu $t0, $t0, 1
    b next

last:
    # move to the last character
    subu $t0, $t0, 1

swap:
    # done when pointers meet
    bgeu $a0, $t0, done

    # exchange characters
    lbu $t2, 0($a0)
    lbu $t3, 0($t0)
    sb $t2, 0($t0)
    sb $t3, 0($a0)

    # step to the next pair
    addu $a0, $a0, 1
    subu $t0, $t0, 1
    b swap

done:
    jr $ra
