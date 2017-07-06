# filename: loops2.s
# CS224 Week 04 Example 
# initialize an array with integers 1^2, 2^2, .., 10^2; display values in
# array in a second loop
   
# Register Usage:
#     $t0  - pointer to array 
   
   .data
   array: .word 0:10   # allocate 10 consecutive words aligned on word boundary
                       # load 32-bit integer value 0 in each word
   
   space: .asciiz " "
   newline: .asciiz "\n"
   
   .text
   .globl main
   
main:
   la $t0, array
   li $t1, 1          # counter
   li $t5, 10         # sentinel
   
fill_loop:            # for (count = 1; count < 10; count++)
   move $t2, $t1      # temp = counter
   mult $t2, $t2      # temp2 = temp * temp
   mflo $t2           # move product in lo to temp2
   sw $t2, ($t0)      # store value in t2 to array 
   addi $t0, 4        # add 4 bytes to the array address to point to next slot 
   addi $t1, 1        # count++
   ble $t1, $t5, fill_loop    # if t1 <= t5 continue looping
   
   # prepare to print the array
   la $t0, array      # load addr of array
   li $t1, 1          # counter
   li $t5, 10         # sentinel
   
prnt_loop:
   li $v0, 1          # load print_integer syscall
   lw $a0, ($t0)      # load value at array index 
   syscall
   
   li $v0, 4
   la $a0, space
   syscall
   
   addi $t0, 4
   addi $t1, 1
   ble $t1, $t5, prnt_loop
   
   li $v0, 4
   la $a0, newline
   syscall
   
exit:
   li $v0, 10
   li $a0, 0
   syscall
