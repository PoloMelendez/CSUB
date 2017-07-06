; hw09.asm
; demonstrate material covered in the homework
;
; by convention function args are passed in rdi, rsi, rdx, rcx, r8 and r9
; function return value in rax

;; put stuff in the read-only .rodata segment that never change
section .rodata
    prompt1 db  "Enter first name: ",0
    prompt2 db  "Enter ID (a positive integer): ",0
    display db  "Your first name is %s and your ID doubled is %d.",10,0  ;;10=LF
    str_fmt db  "%s",0
    num_fmt db  "%d",0

;; stuff in the .data segment has a static address but the value can change 
section .data
    buffer times 45 db 0   ;; reserve 45 bytes and zero them out

section .text
    global  main, foo
    extern  printf, scanf

    main:
        ; proloque
        ; the next 9 instructions setup the call frame for main 
        push    rbp       ; base ptr rbp points to bottom of call frame
        mov     rbp, rsp
        sub     rsp, 8
        pushfq            ; 64-bit instruction to push quad-word register flags 
        push    rbx
        push    r12
        push    r13
        push    r14
        push    r15
        
        ; prompt user to read in a string 
        mov     rdi, prompt1   ; load 32-bit address of prompt1 into rdi
        xor     rax, rax       ; zero out the return value register
        call    printf

        ;; read in the string - store it at the address in rsi
        mov     rdi, dword str_fmt
        mov     rsi, dword buffer
        xor     rax, rax       ; zero out the return value register
        call    scanf

        ; prompt user to read in ID 
        mov     rdi, prompt2
        xor     rax, rax       ; zero out the return value register
        call    printf

        ;; read in an integer
        mov     rdi, dword num_fmt
        lea     rsi, [rbp-8]   ;; rbp-8 points to empty space on call frame 
        xor     rax,rax
        call    scanf

        ; Call foo to calculate 2*id. Result returned in RDX:RAX
        mov     rdi, [rbp-8]   ; load value of n read from user off the stack
        xor     rax, rax
        call    foo
 
        ; print result as 32-bit unsigned int
        mov     rdi, dword display   ;; format string for display
        mov     rsi, buffer          ;; address of buffer that holds first name 
        mov     rdx, rax             ;; return value 2*n is in rax
        xor     rax, rax
        call    printf
 
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     rbx
        popfq
        mov     rsp, rbp
        pop     rbp
        ret

    ;; int foo(int n) {return 2*n;}
    ;; rdi holds n
    foo:
        push    rbp
        mov     rbp, rsp
        push    127  ;; just for kicks push an immediate onto the stack  
        push    rdi  ;; now push n onto the stack 

        xor     rax, rax

        ; compute 2*n 
        pop     rsi    ;; rsi now holds n
        imul    rax, rsi, 2  ;; return 2*n 

       ;; epilogue - deconstruct foo's call frame
        mov     rsp, rbp  ;; this ensures the entire call frame is popped
        pop     rbp
        ret
				
