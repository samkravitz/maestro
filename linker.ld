ENTRY(start)
OUTPUT_FORMAT("binary")
SECTIONS
{
	. = 0x10000;

    _start = .;

    .entry :
    {
        *(.entry)
    }
 
	.text :
	{
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
