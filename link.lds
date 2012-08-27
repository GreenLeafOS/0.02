ENTRY(_start)
SECTIONS
{
	.text 0x100000 :
	{ 
		_text = ABSOLUTE(.); 
		*(.text)
	}
	.data :
	{
		_data = ABSOLUTE(.);
		*(.data .rodata)
	}
	.bss : 
	{
		_bss = ABSOLUTE(.);
		*(.bss)
		*(COMMON)
		_end = ABSOLUTE(.);
	}
	.bios 0 :
	{
		*(bios)
	}
	 /DISCARD/ : 
	 {
	 	*(.symtab) *(.comment) *(.eh_frame) *(.iplt) *(.shstrtab)  *(.strtab) *(.shstrtab) *(.rel.dyn) *(.shstrtab)
	 }
}