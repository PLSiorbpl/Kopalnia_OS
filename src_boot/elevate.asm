bits 32

global Elevate
extern gdt_descriptor
extern PML4_Table
extern Kernel_64

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .data
testword: DW 0x55AA
value_37F: DW 0x37f
value_37E: DW 0x37e
value_37A: DW 0x37a

section .text
Elevate:
    pusha
    cli
    lgdt [gdt_descriptor]

    mov eax, cr4
    or eax, (1 << 5) | (1 << 9) | (1 << 10)
    mov cr4, eax

    mov eax, PML4_Table
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x100 | 0x800
    wrmsr

    mov eax, cr0
    and eax, ~((1 << 2) | (1 << 3)) ; Set bit 2 off bc itll cause #UD if using FPU and we wanna actually use the FPU and set bit 3 to off bc u need to idk
    or eax, (1 << 31) | (1 << 0) | (1 << 1) | (1 << 5); Paging bit, Protected Mode bit, Monitor co-processor bit, Numeric Error bit?
    mov cr0, eax

    fninit                                 ; load defaults to FPU
    fnstsw [testword]
    cmp word [testword], 0
    jne nofpu                              ; jump if the FPU hasn't written anything (i.e. it's not there)

    fldcw [value_37F]   ; writes 0x37f into the control word: the value written by F(N)INIT
    fldcw [value_37E]   ; writes 0x37e, the default with invalid operand exceptions enabled
    fldcw [value_37A]   ; writes 0x37a, both division by zero and invalid operands cause exceptions.

    popa

    jmp 0x18:long_mode_entry
    ret ; In case 64bit no work or smt

nofpu:
    ; Reset FPU to off
    mov eax, cr0
    or eax, (1 << 31) | (1 << 0) | (1 << 1) | (1 << 2)
    mov cr0, eax

    popa

    jmp 0x18:long_mode_entry

.hang:
    cli
    hlt
    jmp .hang

bits 64
long_mode_entry:
    cli
    mov ax, 0x10 ; Set the A-register to the data descriptor.
    mov ds, ax   ; Set the data segment to the A-register.
    mov es, ax   ; Set the extra segment to the A-register.
    mov fs, ax   ; Set the F-segment to the A-register.
    mov gs, ax   ; Set the G-segment to the A-register.
    mov ss, ax   ; Set the stack segment to the A-register.

    mov rsp, stack_top
    and rsp, ~0xF
    sub rsp, 8
    xor rbp, rbp

    jmp Kernel_64

section .note.GNU-stack noalloc noexec nowrite progbits