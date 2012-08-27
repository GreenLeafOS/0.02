#################################################
#				Makefile for OS					#
#					系统编译						#
#################################################
entrypoint			= 0x100000
biosinfo			= 0x30000
#------------连接-----------
ld					=	ld
ld_flags			=	-T link.lds -Map bin/kernel.map -s
# -T link.lds -Map bin/kernel.map -s
# -s -Ttext $(entrypoint) -section-start memory=$(memoryinfo) -Map bin/kernel.map
#---------最终目标----------
image		= 	bin/kernel.img
#------------目标-----------
boot		=	bin/boot.bin bin/loader.bin
sub_dir		=	kernel
# arch micro-kernel lib
objs 		:= $(foreach path,$(sub_dir),bin/$(path).o)

.PHONY : clean all finalclean image

# 编译
all : finalclean $(image) $(objs) $(boot) 

	@echo "linking kernel..."
	@$(ld) $(ld_flags) $(objs) -o bin/kernel.bin
	
	@echo "Writing image..."
#	@dd conv=notrunc of=bin/kernel.img bs=512 count=1 if=bin/mbrboot.bin 
	@dd conv=notrunc of=bin/kernel.img bs=512 count=1 if=bin/boot.bin
	@dd conv=notrunc of=bin/kernel.img bs=512 count=1 if=bin/fs.bin seek=1
	@dd conv=notrunc of=bin/kernel.img bs=512 count=2 if=bin/loader.bin seek=2
	@dd conv=notrunc of=bin/kernel.img bs=512 count=69 if=bin/kernel.bin seek=4
	
image: all
	@dd if=bin/kernel.img bs=4096 count=500 of=/dev/sdb
	
# 循环调用子目录中的makefile	
$(objs):bin/%.o : %
	make -s -C $<
	
# boot
$(boot):
	@echo "making boot..."
	@make -s -C boot
	
# image
$(image): 
	@echo "Creating image..."
	@-mkdir bin/
	@dd if=/dev/zero of=$(image) bs=512 count=2880
	
# 清理	
finalclean:
	@rm -f bin/*.o
	@rm -f bin/*.bin
	
# 循环清理各个子目录
clean :
	@make clean -s -C boot
	@for dir in $(sub_dir); \
		do make clean -s -C $$dir; \
		echo "cleaning "$$dir"...";\
	done


