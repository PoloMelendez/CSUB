#  rev.s
#  week07 example code
#  display a string in reverse  
# 
#       $ spim -f rev.s happy 
#       yppah
#       $ spim -f rev.s 
#       anahanahana 
# 

# If no string is passed then use a hardcoded string - we do this to be able
# to debug the code inside spim 

# Cmdline args are accessed via an array of addresses (pointers) to strings. 
# Base ptr to the structure is in $a1; num args is in $a0

# A MIPS address is stored in a word (4 bytes). The first address in the array 
# (offset zero) points to the filename of the executable. The next address in 
# the array (offset 4) points to the first cmdline arg and so on. The args
# are read in as strings terminated by NUL \0 (ASCII value 0)

#  Register Usage:
#             $s0     - holds base address of string - this never changes 

#             $t0     - will be set to point to end of string
#             $t1     - holds current character in string
#             $t2,$t3 - temporary storage when you swap
#             $t4     - points to start of string and moves right
    .data

astring:
    .asciiz   "anahanahana"    # reverse this string if nothing is passed
                               # this string is a palindrome country name  

newline:
    .asciiz   "\n"
    
    .text
.globl main
.globl swap             # make this global so you can set a breakpoint
     
main:
    # test if cmd line arg was passed
    li $t6, 1 
    bgt $a0, $t6, L1
    la $t0, astring 
    b L2
L1:    
    lw $t0, 4($a1)
L2:
    move $t4, $t0   
    move $s0, $t0   

find_end:
    lbu $t1, 0($t0)
    beqz $t1, done 

    addu $t0, $t0, 1

    # print the current character for debugging
    # li $v0, 11 
    # move $a0, $t1 
    # syscall

    b find_end 

done:

    subu $t0, $t0, 1    # back up one byte to skip the null character

swap:
    # done when pointers t4 (start) and t0 (end) meet
    bgeu  $t4, $t0, exit   

    # exchange characters
    lbu $t2, 0($t4)
    lbu $t3, 0($t0)
    sb $t2, 0($t0)
    sb $t3, 0($t4)

    # print the characters you are exchanging for debugging
    # li $v0, 11 
    # move $a0, $t3 
    # syscall
    # move $a0, $t2 
    # syscall

    # step to the next pair
    addu $t4, $t4, 1
    subu $t0, $t0, 1
    b swap

exit:
    # add null character to the end of the string
    # sb $zero, 1($t4)

    # display string
    li $v0, 4
    move $a0, $s0
    syscall

    la $a0, newline
    syscall

    li $v0, 10
    syscall
