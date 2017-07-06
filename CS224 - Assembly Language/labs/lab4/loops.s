# file: loops.s
# implements this C for_loop to populate an array:
#
#    for (int i = 0; i < size; i++) 
#    {    
#         a[i] = i + 5;
#         printf(%d,a[i]);
#    }
#
# Register Usage:  
#      $t0 - loop variable i  
#      $s0 - size of the array 
#      $s1 - base address of the array (i.e., a[0])
#      $t1 - holds i + 5 
#      $t2 - holds address a[i]; calculated as s1 (base address) + 4*i

.data
my_array:     
        .space 20            # allocate 20 bytes to hold 5 4-byte integers 
newline:
        .asciiz   "\n"
onespace:
        .asciiz   " "
 
.text
.globl main
.ent main

main:
       li $s0, 5             # $s0 holds the size of the array
       la $s1, my_array      # load base address of array into $s1
       addi $t0, $zero, 0    # initialize i to 0
Loop:  slt $t1, $t0, $s0     # t1 = (i < size) ? 1 : 0;
       beq $t1, $zero, Exit  # Exit if i >= size
       addi $t1, $t0, 5      # Add 5 to i, store in t1
       sll $t2, $t0, 2       # Calculate i*4, store in t2
       add $t2, $t2, $s1     # Calculate address of a[i]
       sw $t1, 0($t2)        # Store t1 to element address
       addi $t0, $t0, 1      # Increment i

       li $v0, 1             # print the element you just stored in the array 
       lw $a0, 0($t2)        # load int value from the array into a register
       syscall 
       li $v0, 4
       la $a0,onespace 
       syscall

       j Loop                # Go to next iteration
Exit:
       li $v0, 4             # display a newline
       la $a0, newline
       syscall

       li $v0, 10
       li $a0, 0
       syscall
.end main

# If it helps, write a flow chart to depict the behavior of your algorithm. 
# Loop Flow Chart:         --------
#                          | i = 0 |
#                          |_______|
#                              |
#                             \|/
#                             / \ /__________________________
#                            /   \\                          |
#                           /     \                          |
#                          /       \                         |
#                   No    /         \   Yes                  |
#                ________/ i < size? \________              /|\
#               |        \           /        |              |
#               |         \         /        \|/             |
#               |          \       /     ______________      |
#               |           \     /     |              |     |
#               |            \   /      | a[i] = i + 5 |__\__|
#               |             \ /       | i++          |  /
#               |                       |______________|
#               |       ______
#               |_____\|      |
#                     /| Exit |
#                      |______|
#
