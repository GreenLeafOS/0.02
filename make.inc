asm					=	as
asm_flags			=	-I ../arch/include/ -I ../micro-kernel/include/

cc					=	gcc
cc_flags			=	-I ../ -c -std=c99 -fno-stack-protector -m32
						
cpp					=	g++
cpp_flags			=	-c -fno-rtti \
						-Wwrite-strings

ld					=	ld
ld_flags			=	-r


objs		+=	$(asm_objs) $(cpp_objs) $(cc_objs) 

.PHONY : clean


$(final_obj) : $(objs)
	$(ld) $(ld_flags) $(objs) -o $@	

$(cc_objs): %.c.o : %.c
	$(cc) $(cc_flags) $< -o $@

$(cpp_objs): %.cc.o : %.cc
	$(cpp) $(cpp_flags) $< -o $@

$(asm_objs): %.s.o : %.s
	$(asm) $(asm_flags) $< -o $@
	
	
clean :
	-rm -f $(objs)
	-rm -f $(final_obj)
