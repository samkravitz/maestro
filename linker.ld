ENTRY(boot)
SECTIONS
{
	. = 1M;
 
	.text :
	{
		*(.multiboot)
		*(.text)
		etext = .;
	}
 
	.rodata :
	{
		*(.rodata)
	}
 
	.data :
	{
		*(.data)
	}
 
	.bss :
	{
		*(.bss)
		. = ALIGN(4096);
		end = .;
	}
}
