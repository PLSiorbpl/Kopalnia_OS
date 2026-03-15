OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
SECTIONS
{
	. = 1M;
	.text : {
	    *(.multiboot_header)
	    *(.text*)
	}
	.rodata : {*(.rodata*)}
	.data : {*(.data*)}
	.bss  : {*(.bss*)}
	_end = .;
	heap_start_ = _end;
}