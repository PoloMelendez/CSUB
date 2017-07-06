; io.asm
; nasm -f elf64 -o io.o io.asm
; you can link io.o into other assembly programs

%macro prologue 0
      push    rbp
      mov     rbp,rsp
      push    rbx
      push    r12
      push    r13
      push    r14
      push    r15
%endmacro
%macro epilogue 0
      pop     r15
      pop     r14
      pop     r13
      pop     r12
      pop     rbx
      leave
      ret
%endmacro

section .rodata
  int_format db  "%i",0
  str_format db "%s",0

section .text
  global  print_string, print_nl, print_int, read_int
  extern printf, scanf, putchar

  print_string:
      prologue
      mov     rsi,rdi              ; string address is passed in rsi
      mov     rdi,dword str_format ; format string address is passed in rdi 
      xor     rax,rax              ; rax is return value register - zero it out
      call    printf
      epilogue

  print_nl:
      prologue
      mov     rdi,0xA
      xor     rax,rax
      call    putchar
      epilogue

  print_int:
      prologue
      ;integer arg is in rdi
      mov     rsi, rdi
      mov     rdi, dword int_format
      xor     rax,rax
      call    printf
      epilogue

  read_int:
      prologue
      ;rdi is assumed to have the address of the int to be read in
      mov     rsi, rdi
      mov     rdi, dword int_format
      xor     rax,rax
      call    scanf
      epilogue

