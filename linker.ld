ENTRY(start)
OUTPUT_FORMAT("binary")
SECTIONS
{
	. = 1M;

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
