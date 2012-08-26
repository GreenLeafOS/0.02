#ifndef REG_H
#define REG_H
/*===============================================================
 *                      reg.h
 *                保护、恢复寄存器以及跳转
 ===============================================================*/
#include <lib/include/type.h>

/*===========线程上下文寄存器结构=============*/
typedef struct stack_frame {
	/* PushSeg压入 */
	u32	gs;
	u32	fs;
	u32	es;
	u32	ds;
	/* PushReg压入*/
	u32	edi;
	u32	esi;
	u32	ebp;
	u32	kernel_esp;
	u32	ebx;
	u32	edx;
	u32	ecx;
	u32	eax;
	/* 系统压入 */
	u32	eip;
	u32	cs;
	u32	eflags;
	u32	esp;
	u32	ss;
}StackFrame;

/************************************************************************/
/*                     	通用寄存器
/*                        Reg
/************************************************************************/
#define PUSH_REG_OFFSET(reg_stack_frame) &reg_stack_frame.reg.eax
#define POP_REG_OFFSET(reg_stack_frame) &reg_stack_frame.reg.edi

#define PushReg()	__asm volatile("pushal")
#define PopReg()	__asm volatile("popal")
/************************************************************************/
/*                     	段寄存器
/*                        Seg
/************************************************************************/
#define PUSH_SEG_OFFSET(reg_stack_frame) &reg_stack_frame.reg.ds
#define POP_SEG_OFFSET(reg_stack_frame) &reg_stack_frame.reg.gs

#define PushSeg()							\
	__asm volatile(							\
				"push %ds		\n"			\
				"push %es		\n"			\
				"push %fs		\n"			\
				"push %gs		\n"			\
				)

#define PopSeg()							\
	asm volatile(							\
				"pop %gs		\n"			\
				"pop %fs		\n"			\
				"pop %es		\n"			\
				"pop %ds		\n"			\
				)

/************************************************************************/
/*                     	状态寄存器
/*                       Flags
/************************************************************************/
#define PUSH_FLAGS_OFFSET(reg_stack_frame) ((u32*)&reg_stack_frame.reg.eflags + 1)
#define POP_FLAGS_OFFSET(reg_stack_frame) &reg_stack_frame.reg.eflags

#define PushFlags()		__asm volatile("pushf")
#define PopFlags()		__asm volatile("popf")


#define StoreFlags(eflags)		\
	__asm volatile( 			\
		"pushf\n\t"				\
		"pop %0\n\t"			\
		: "+m"(eflags)			\
		:						\
		: "memory"				\
        )


#define RecoveryFlags(eflags) 	\
	__asm volatile( 			\
		"pushl %0\n\t"			\
		"popf\n\t"				\
		: 						\
		: "m"(eflags)			\
		: "memory"				\
        )



/* 关中断 */
#define CloseInt() 	__asm volatile("cli")
/* 开中断 */
#define OpenInt()	__asm volatile("sti")
/************************************************************************/
/*                     	  跳转
/*                        Jmp
/************************************************************************/

/* 段内转移 */
#define JmpNear(eip)		__asm volatile("jmp *%0" : "=m"((u32)(eip))

/* 段间无特权级转移 */
#define JmpFar(cs,eip)		__asm volatile("ljmp *%0,*%1" : "=m"((u16)(cs)),"=m"((u32)(eip))


/* 段间转移。高特权级转移向低特权级 */
#define JmpDown(cs,eip,ss,esp)				\
	__asm volatile(							\
				"pushl	%0		\n"			\
				"pushl	%1		\n"			\
				"pushl	%2		\n"			\
				"pushl	%3		\n"			\
				"lret			\n"			\
				: "=m"((u16)(ss)),"=m"((u32)(esp)),"=m"((u16)(cs)),"=m"((u32)(eip))			\
				)
#endif
