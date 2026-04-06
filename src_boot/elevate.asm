bits 32

global Elevate
extern gdt_descriptor
extern PML4_Table
extern Kernel_64

section .text
Elevate:
    pusha
    cli
    lgdt [gdt_descriptor]

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov eax, PML4_Table
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x100
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, (1 << 31) | (1 << 0)
    mov cr0, eax

    popa

    jmp 0x18:long_mode_entry
    ret ; In case 64bit no work or smt

bits 64
long_mode_entry:
    cli
    mov ax, 0x10 ; Set the A-register to the data descriptor.
    mov ds, ax   ; Set the data segment to the A-register.
    mov es, ax   ; Set the extra segment to the A-register.
    mov fs, ax   ; Set the F-segment to the A-register.
    mov gs, ax   ; Set the G-segment to the A-register.
    mov ss, ax   ; Set the stack segment to the A-register.

    jmp Kernel_64 ; currently no such thing

section .note.GNU-stack noalloc noexec nowrite progbits