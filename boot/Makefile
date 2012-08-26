#################################################
#				Makefile for Boot				#
#					∆Ù∂Ø±‡“Î						#
#################################################
.PHONY: all clean finalclean

all: finalclean
	as boot.s -o boot.o
	as loader.s -o loader.o
	as fs.s -o fs.o
	ld boot.o 	-Ttext 0x7c00 -section-start fs=0x800 --oformat=binary -o ../bin/boot.bin
	ld loader.o -Ttext 0x8000 -section-start fs=0x800 --oformat=binary -o ../bin/loader.bin
	ld fs.o --oformat=binary -o ../bin/fs.bin
clean: finalclean
	-rm -f boot.o
	-rm -f loader.o
	-rm -f fs.o
	
finalclean:
	-rm -f ../bin/boot.bin
	-rm -f ../bin/loader.bin
	-rm -f ../bin/fs.bin