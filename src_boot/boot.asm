BITS 32
global _start
extern kernel_main
extern Kernel_64
extern gdt
extern gdt_descriptor
extern stack_top
extern tss

extern Print
extern Paging
extern Elevate

section .multiboot2 ;multiboot_header
align 8
header_start:

align 8
dd 0xE85250D6       ; magic multiboot1 = 0x1BADB002 multiboot2 = 0xE85250D6
dd 0x0              ; flags
dd header_end - header_start
dd -(0xE85250D6 + 0 + (header_end - header_start))

; Framebuffer tag
;dw 5
;dw 0
;dd 24
;dd 800
;dd 600
;dd 32

align 8

; end tag
dw 0
dw 0
dd 8

header_end:

align 8

init_gdt_32:
    mov eax, gdt
    mov [gdt_descriptor + 2], eax
    mov eax, tss
    mov word [gdt + 56], 103  ; limit[15:0]
    mov word [gdt + 58], ax   ; base[15:0]
    shr eax, 16
    mov byte [gdt + 60], al   ; base[23:16]
    mov byte [gdt + 61], 0x89 ; access
    mov byte [gdt + 62], 0x00 ; flags
    mov byte [gdt + 63], ah   ; base[31:24]
    mov dword [gdt + 64], 0   ; base[63:32] high slot
    mov dword [gdt + 68], 0
    ret

section .text
_start:
    cli
    mov esp, stack_top
    push eax ; Magic
    push ebx ; Mb2 Info

    call init_gdt_32
    call Elevate
.hang:
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits