/* driver.c 
 * test calling an x86 assembly function 
 *
 * $ gcc -c driver.c -o driver.o
 * $ nasm -f elf64 io.asm
 * $ gcc -o driver io.o driver.o
 */

#include <stdlib.h>
#include <stdio.h>

char mystr[15] = "hello\n";

int main() {

  /* this is a function written in x86 assembly */
  print_string(mystr);

  return 0;
} 
