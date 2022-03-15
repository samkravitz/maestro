ENTRY(start)
OUTPUT_FORMAT("binary")
SECTIONS
{
	. = 0xc0000000;

    _start = .;
	_start_phys = 1M;

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
	}

	. = ALIGN(4096);
	end = .;
	end_phys = _start_phys + (end - _start);
}
