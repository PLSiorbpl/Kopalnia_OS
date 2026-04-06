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
	_end = .;
	heap_start_ = _end;
}