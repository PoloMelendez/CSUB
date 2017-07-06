;; goodbye.asm
;;
;; test linking two assembly files together
;;
;; $ nasm -f elf64 io.asm -o io.o
;; $ nasm -f elf64 goodbye.asm -o goodbye.o
;; $ ld -o goodbye goodbye.o io.o -lc          # link in libc with -lc


 
section .data
   msg     db      'Goodbye, World!', 0AH
   len     equ     $-msg
 
section .text
   global  _start
   extern print_string
 
_start: 
   ;; setup stuff to perform write() 
   mov     ebx, 1          ;; 1=stdout 
   mov     edx, len        ;; arg3=byte_cnt
   mov     ecx, msg        ;; arg2=string address
   mov     eax, 4          ;; 4=write 
   int     80h             ;; perform syscall


   ;; now print the string with print_string from a linked in assembly file
   mov     rdi, msg        ;; print_string expects address to be in rdi
   call print_string

   ;; exit() 
   mov     ebx, 0          ;; 0=status
   mov     eax, 1          ;; 1=exit
   int     80h             ;; perform the system call 

