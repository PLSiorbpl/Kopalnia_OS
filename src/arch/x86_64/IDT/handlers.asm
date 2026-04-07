bits 64

extern isr_common

section .text

%macro pushall 0
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro
%macro popall 0
    pop rax
    pop rcx
    pop rdx
    pop rbx
    pop rbp
    pop rsi
    pop rdi
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
%endmacro

%macro ISR_NOERR 1
%define _isr_name_1 isr %+ %1
global _isr_name_1
_isr_name_1:
    push 0
    push %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERR 1
%define _isr_name_2 isr %+ %1
global _isr_name_2
_isr_name_2:
    push %1
    jmp isr_common_stub
%endmacro

isr_common_stub:
    sub rsp, 8
    pushall
    mov rdi, rsp
    call isr_common
    popall
    add rsp, 16
    iretq

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15 ; we cant use that btw
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR 21
ISR_NOERR 22 ; General for all 0x16 - 0xff
%assign i 23
%rep 233
    ISR_NOERR i
%assign i i+1
%endrep

section .data
    global isr_table

    isr_table:
    %assign i 0
    %rep 256
        dq isr %+ i
    %assign i i+1
    %endrep

section .note.GNU-stack noalloc noexec nowrite progbits