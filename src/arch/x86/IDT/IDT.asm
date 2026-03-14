BITS 32

global isr_dummy
global irq0_handler
global irq1_handler

extern irq0_handler_c
extern irq1_handler_c

section .text

isr_dummy:
    iret

irq0_handler:
    pusha
    call irq0_handler_c
    popa
    mov al, 0x20
    out 0x20, al
    iret

irq1_handler:
    pusha
    call irq1_handler_c
    popa
    mov al, 0x20
    out 0x20, al
    iret

section .note.GNU-stack noalloc noexec nowrite progbits