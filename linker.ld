ENTRY (boot)

SECTIONS
{
	. = 0x100000;
	start = .;

	.multiboot :
	{
		*(.multiboot)
    }

	. += 0xC0000000;
	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}

	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(.bss)
	}

	end = .;
}