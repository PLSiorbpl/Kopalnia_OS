bits 64
extern dispatch_syscall
extern kernel_rsp
extern user_rsp

global handle_syscall

handle_syscall:
    ; rcx = user RIP (saved by syscall)
    ; r11 = user RFLAGS (saved by syscall)
    ; rax = syscall number
    ; rdi, rsi, rdx, r10 = args 1-4

    mov [user_rsp], rsp
    mov rsp, [kernel_rsp]

    push rcx
    push r11
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, rax
    call dispatch_syscall

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    pop r11
    pop rcx

    push rax
    mov ax, 0x2B
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    pop rax

    mov rsp, [user_rsp]

    o64 sysret