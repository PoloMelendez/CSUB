section .rodata
    num db  "%d",10,0
section .text
    global main, fib
    extern printf, atoi

main:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    pushfq
    push rbx
    push r12
    push r13
    push r14

    mov r12, rdi
    mov r13, rsi
    lea r14, [r13+8]
    mov rdi, [r14]
    xor rax, rax
    call atoi           ;number is now in rax
    
    mov rdi, rax
    xor rax, rax
    call fib
    mov rdi, num
    mov rsi, rax
    xor rax, rax
    call printf

    pop r14
    pop r13
    pop r12
    pop rbx
    popfq
    mov rsp, rbp
    pop rbp
    ret

fib:
    mov rax, 1
    cmp rdi, 2
    jb return_1     ;if n < 2, return 1

    push rbp
    mov rbp, rsp
    push rdi
    push rcx
    push rbx

    mov rcx, rdi
    sub rcx, 1      ;rcx = n - 1
    mov rdi, rcx
    call fib
    mov rbx, rax    ;rbx = fib(n-1)
    sub rcx, 1      ;rcx = n - 2
    mov rdi, rcx    
    call fib        ;rax = fib(n-2)
    add rax, rbx    ;rax = fib(n-2) + fib(n-1)

    pop rbx
    pop rcx
    pop rdi
    pop rbp
return_1:
    ret
