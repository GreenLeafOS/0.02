#################################################
#			Makefile for kernel					#
#				 Î¢ÄÚºË´úÂë±àÒë					#
#################################################

final_obj	=	../bin/kernel.o

cpp_objs	=	
cc_objs		=	memory.c.o arch/arch_init.c.o arch/protect.c.o arch/segment.c.o arch/tss.c.o arch/irq.c.o arch/i8259.c.o \
				page/page_init.c.o page/phypage.c.o page/linearspace.c.o	\
				handle/handle.c.o
asm_objs	=	start.s.o arch/bios.s.o

sinclude ../make.inc