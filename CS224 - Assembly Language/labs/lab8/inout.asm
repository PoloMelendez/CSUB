; inout.asm
; demonstrate Linux system call 3 and 4 to perform input and output
; for a listing of linux syscalls see
; http://asm.sourceforge.net/syscall.html#p32 
;
;  $ nasm -f elf64 -o inout.o inout.asm 
;  $ ld -o inout inout.o 
;  $ ./inout
;
; system call service 0x80:
;
; unsigned service0x80(unsigned svcnum, unsigned dest, char *buf, int buflen);
;
;
%define readservice 3        ; these assembler directives behave like C defines
%define writeservice 4       ; use defines to improve code readability
%define STDIN 1
%define STDOUT 2
%define STDERR 3


; start of data section - you do not need a colon after labels in data section

        section .data                      
buflen  equ 64
buffer  times buflen db 0  ; reserve 64 bytes and zero them out
prompt  db "Enter some words: "   ; a string constant
prlen   equ $ - prompt   ; $ evaluates to to assembly position at the beginning
                         ; of the line containing the expression
                         ; this effectively assigns length of prompt to prlen

; start of text section
        section .text
        global _start, main
_start:
main:
        nop
        mov eax, writeservice
        mov ebx, STDOUT
        mov ecx, prompt
        mov edx, prlen
        int 0x80                ; make a system call
       
        mov eax, readservice
        mov ebx, STDIN
        mov ecx, buffer
        mov edx, buflen
        int 0x80                ; number of bytes actually read will be in eax
       
        mov edx, eax            ; grab the used-buffer-size first
        mov eax, writeservice
        mov ebx, STDOUT
        mov ecx, buffer
        int 0x80

done:
        mov ebx, 0
        mov eax, 1
        int 0x80                ; execute system call
