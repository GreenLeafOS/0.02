#################################################
#				Makefile for arch				#
#				 Ó²¼þ²ã´úÂë±àÒë					#
#################################################

final_obj	=	../bin/arch.o

cpp_objs	=
cc_objs		=	desc.c.o			\
				table_gdt.c.o table_ldt.c.o table_idt.c.o \
				page.c.o \
				tss.c.o 8259A.c.o print.c.o
asm_objs	=	start.s.o print.s.o bios.s.o

sinclude ../make.inc