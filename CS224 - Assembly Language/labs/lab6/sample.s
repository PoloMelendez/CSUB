#  sample.s
#  demonstate grabbing ascii value of a single character from cmdline
# 
#       $ spim -f sample.s  a   
#       $ aa97    << spits out ascii char as a string,a char and integer 
#
# Cmdline args are accessed via an array of addresses (pointers) to strings. 
# A MIPS address is stored in a word (4 bytes). The first address in the array 
# (offset zero) points to the filename of the executable. The next address in 
# the array (offset 4) points to the first cmdline arg and so on. Since the args
# are read in as strings, if you expect a number you need to convert it.

# Register Usage:
    .data
newline:
    .asciiz   "\n"
errmsg:
    .asciiz   "Contact your system administrator.\n"
result:
    .space   11
    
.text
.globl main
     
main:
    move $s0, $a0       # a0 holds the number of cmdline args
    move $s1, $a1       # a1 is the base pointer to the array of addresses
                        # in the cmdline structure 

    ble  $a0, 1, error  # if arg cnt <= 1 exit    
    # parse the first arg by loading its address (offset 4 from the base of the
    # array) into $a0 
    lw $a0, 4($s1)       
    
    # grab the character as a string and display it as a string 
    li $v0, 4         # 4 is print string
    lw $a0, 4($s1)    # since cmd line args are in an array of addresses - load 
    syscall           # word will load the address to character into $a0

    # grab the first byte of the string and display it as a char 
    li $v0, 11        # 11 is print char 
    lw $t0, 4($s1)    # t0 now holds the address to the 2nd cmd line arg 
    lb $a0, ($t0)     # grab the first byte at address 4($s1)
    syscall           # print the byte as an ascii character 

    # now display ascii value of the character in $a0
    li $v0, 1         # 1 is print int
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
