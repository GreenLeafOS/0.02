/*
 * idt.c
 * idt相关处理函数
 */

#include "include/table.h"
#include "include/print.h"

Gate idt_table[256];
u8 idt_ptr[6];
/************************************************************************/
/*						门描述符表的存取
/*                          set get
/************************************************************************/
void idt_set(int index,Gate item)
{
	idt_table[index] = item;
	return;
}

void idt_get(int index,Gate *item)
{
	*item = idt_table[index];
	return;
}
/************************************************************************/
/*						初始化门描述符
/*                          init
/************************************************************************/

/* 调用门 */
void idt_init_call(int index,FunAddr handle,u8 dpl)
{
	idt_table[index] = gate_create((u32)handle,0x8,DA_386CGate | dpl,0);
	return;
}

/* 中断门 */
void idt_init_intr(int index,FunAddr handle,u8 dpl)
{
	idt_table[index] = gate_create((u32)handle,0x8,DA_386IGate | dpl,0);
	return;
}

/* 陷阱门 */
void idt_init_trap(int index,FunAddr handle,u8 dpl)
{
	idt_table[index] = gate_create((u32)handle,0x8,DA_386TGate | dpl,0);
	return;
}

/* 任务门 */
void idt_init_task(int index,FunAddr handle,u8 dpl)
{
	idt_table[index] = gate_create((u32)handle,0x8,DA_TaskGate | dpl,0);
	return;
}

/************************************************************************/
/*						加载门描述符
/*                          load
/************************************************************************/
void idt_load()
{
	u16* p_idt_limit = (u16*)(&idt_ptr[0]);
	u32* p_idt_base  = (u32*)(&idt_ptr[2]);
	char *pIdt = (char*)&idt_ptr[0];

	*p_idt_limit = 256 * sizeof(Gate) - 1;
	*p_idt_base  = (u32)&idt_table[0];

	__asm volatile(
			"movl %0,%%eax		\n"
			"lidt (%%eax)"
			:
			: "g"(pIdt));
	return;
}
