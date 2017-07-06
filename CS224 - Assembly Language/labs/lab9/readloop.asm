; readloop.asm
;
; does this:
;       1) prompt for input
;       2) do {
;               read console
;               display buffer
;          } while (buffer isn't empty)
;       3) quit on Ctrl-D
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%define STDIN 1
%define STDOUT 2
%define STDERR 3

        section .data
buflen  equ 35 
buffer  times buflen db 0   ; reserve buflen bytes and zero them out
prompt  db "Enter text. Press 'Ctrl-D' when done: "
prlen   equ $ - prompt

        section .text
;-------------------------------------
; Use kernel writeservice to display
; text to console.
; Expects:
;       text's ADDRESS in ECX
;       text's LENGTH in EDX
; Returns:
;       nothing

wrtproc:       
        mov eax, 4      ; kernel's write service
        mov ebx, STDOUT
        int 0x80
        ret

;-------------------------------------
; Use kernel writeservice to acquire
; text from console.
; Expects:
;       buffer's ADDRESS in ECX
;       buffer's maximum length in EDX
; Returns:
;       # of bytes acquired, in EAX
rdproc:
        mov eax, 3             ; linux kernel's write service
        mov ebx, STDIN
        int 0x80               ; number of bytes read will be in eax
        ret

        global main 
main:
        mov ecx, prompt
        mov edx, prlen
        call wrtproc
readloop:
        mov ecx, buffer
        mov edx, buflen
        call rdproc             ; number actually read will be in eax
        test eax, eax
        jz done                 ; jump when last result is zero 
       
        mov edx, eax            ; grab the used-buffer-size first
        mov ecx, buffer
        call wrtproc
        jmp readloop            ; repeat

done:
        mov ebx, 0
        mov eax, 1
        int 0x80
