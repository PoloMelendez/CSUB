# Factorial example from Appendix A, pp A-27 to A-29 (on CD)
#
# This implements the following program:
#
# int main() {
#   printf("The factorial of 5 is %d\n", fact(5));
# }
# int fact(int n) {
#   if(n < 1) return 1;
#   else return n * fact(n-1);
# }
#
  .text
  .globl main    # Start execution at main
main:
  # The book makes use of frame pointers in this example.
  # As described on page A-27, by convention a frame is 24 bytes
  # to store a0 - a3 and ra. This includes 4 bytes of padding.
  # To save fp, another 8 bytes are added (4 for fp and 4 for 
  # padding), making the frame 32 bytes long. Even though a0-a3
  # are not saved to the stack, space is still reserved on the
  # stack for them.

  addi  $sp, $sp, -32  # Stack frame is 32 bytes long
  sw  $ra, 20($sp)     # Save return address
  sw  $fp, 16($sp)     # Save old frame pointer
  addi  $fp, $sp, 28   # point to first word in bottom of frame 

  # With the return address and frame pointer saved, main()
  # can now make all its needed function calls

  li  $a0, 5    # Put argument (5) in $a0
  jal  fact     # Call factorial function
  move $t0, $v0 # move result to t0

  # display stuff now
  la $a0, msg   # address of msg
  li $v0, 4     # syscall 1=print string 
  syscall  

  move $a0, $t0  # Move fact result to $a0 to display it
  li   $v0, 1    # syscall 1=print int
  syscall  

  li  $a0, 10    # ascii LF char
  li  $v0, 11    # syscall 1=print char 
  syscall  

  # Since the function calls are done, restore the return address
  # and frame pointer.

  lw  $ra, 20($sp)  # Restore return address
  lw  $fp, 16($sp)  # Restore frame pointer
  addi  $sp, $sp, 32  # Pop stack frame

  li  $v0, 10    # 10 is the code for exiting
  syscall      # Execute the exit

  .end main

  .rdata
msg:
  .asciiz "The factorial of 5 is "

  .text      # Another segment of instructions

  .ent fact    # Begin the definition of the fact function
fact:
  # As above, create a 32 byte frame to store a0-a3, ra and fp.
  # For the fact procedure, a0 will be saved in offset 28 from the
  # sp (offset 0 from the fp).

  addi $sp, $sp, -32  # Stack frame is 32 bytes long
  sw   $ra, 20($sp)   # Save return address
  sw   $fp, 16($sp)   # Save frame pointer
  addi $fp, $sp, 28   # Set up frame pointer
  sw   $a0, 0($fp)    # Save argument (n)

  lw    $v0, 0($fp)  # Load n
  bgtz  $v0, L2      # Branch if n > 0
  li    $v0, 1       # Return 1
  j     L1           # Jump to code to return

L2:
  lw    $v1, 0($fp)   # Load n
  addi  $v0, $v1, -1  # Compute n - 1
  move  $a0, $v0      # Move value to $a0
  jal   fact          # recursive call to factorial function

  lw  $v1, 0($fp)     # Load n
  mul  $v0, $v0, $v1  # Compute fact(n-1) * n, put result in $v0

L1:                 
  lw   $ra, 20($sp)  # Restore $ra
  lw   $fp, 16($sp)  # Restore frame pointer
  addi $sp, $sp, 32  # Pop stack frame
  jr   $ra           # Return to caller

  .end fact
