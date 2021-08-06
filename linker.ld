ENTRY(boot)
OUTPUT_FORMAT("binary")
SECTIONS {
	. = 0x7c00;
	.text :
	{
		*(.boot)
		stext = .;
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
