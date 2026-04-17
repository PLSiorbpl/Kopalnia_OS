OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)
SECTIONS
{
	. = ALIGN(1M);
	start_ = .;
	.text : {
	    *(.multiboot2)
	    *(.text*)
	}
	.rodata : {*(.rodata*)}
	.data : {*(.data*)}
	.bss  : {*(.bss*)}
    .stack (NOLOAD) : {
        . = ALIGN(16);
        stack_bottom = .;
        . += 64K; /* so stack overflows dont delete stuf */
        stack_guard = .;
        . += 16K;
        stack_top = .;
    }
    .user_stack (NOLOAD) : {
        . = ALIGN(16);
        user_stack_bottom = .;
        . += 64K; /* so stack overflows dont delete stuf */
        user_stack_guard = .;
        . += 16K;
        user_stack_top = .;
    }
	_end = .;
	heap_start_ = _end;
}