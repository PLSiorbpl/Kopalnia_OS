OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)
SECTIONS
{
	. = 1M;
	__kernel_start = .;
	.text : {
	    __kernel_text_start = .;
	    *(.multiboot2)
	    *(.text*)
	    __kernel_text_end = .;
	}

	.rodata : {
	    __kernel_rodata_start = .;
	    *(.rodata*)
	    __kernel_rodata_end = .;
	}

	.data : {
	    __kernel_data_start = .;
	    *(.data*)
	    __kernel_data_end = .;
	}

	.bss  : {
	__kernel_bss_start = .;
	    *(.bss*)
	    __kernel_bss_end = .;
	}

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

	__kernel_end = .;
	. = ALIGN(16);
	__heap_start = .;
}