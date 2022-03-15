ENTRY(entry)
OUTPUT_FORMAT("binary")
SECTIONS
{
	. = 0xc0000000;

    start = .;
	start_phys = 1M;

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
	end_phys = start_phys + (end - start);

	size = end - start;
}
