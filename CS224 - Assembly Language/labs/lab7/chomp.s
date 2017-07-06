.data
prompt: .asciiz "Enter a word followed by whitespace: "
string: .space  64
.text
.globl main
main:
  la $a0 prompt
  li $v0 4
  syscall

  la $a0 string
  li $a1 64
  li $v0 8
  syscall

  jal findEnd

  li $v0 4          
  syscall

  li $a0 10
  li $v0 11
  syscall           

  li $v0 10
  syscall

findEnd:
  move $t0 $a0
  li $t1 32
  li $t3 10
  li $t4 9

loop:
  lbu $t2 0($t0)
  beq $t2 $t1 done
  beq $t2 $t3 done
  beq $t2 $t4 done
  addu $t0 $t0 1
  j loop

done:
  li $t1 46
  sb $t1 0($t0)
  addu $t0 $t0 1
  li $t1 0
  sb $t1 0($t0)
  jr $ra
