bits 32

global PML4_Table

section .text

align 4096
PML4_Table:
    dq PDPT_Table + 0x03
    times 511 dq 0

align 4096
PDPT_Table:
    dq PD_Table + 0x03
    times 511 dq 0

align 4096
PD_Table:
    dq (0x0000000000000087)
    dq (0x0000000020000087)
    dq (0x0000000040000087)
    dq (0x0000000060000087)
    dq (0x0000000080000087)
    times 507 dq 0

section .note.GNU-stack noalloc noexec nowrite progbits