; bare.asm
; demonstrate bare-bones x86 assembly
; executes and returns a value to the kernel 
; 
; $ nasm -f elf64 -o bare.o bare.asm
; $ ld -o bare bare.o
; $ ./bare
; $ echo $?
;
; To view the header in the executable:
;
;  vi bare 
;  ESC:%!xxd        ; switch into hex mode
;  ESC:%!xxd -r     ; exit from xxd hex mode and return to vi 


section .data     ; start of data segment
exitcode equ 119  ; assigns the value 119 (Ox77) to the identifier exitcode  -
                  ; labels in data segment do not require colon 

section .text                ; mark start of text segment 
   global _start             ; _start must be global
   _start:                   ; program entry point
      mov   ax,233           ; load ax register with decimal value 233
      mov   eax, 1           ; load eax register with 1 (syscall 1 is exit)
     
      mov   ebx, exitcode    ; load return value register ebx with exit code 
      int   80h              ; perform syscall (interrupt 80h is a syscall)

