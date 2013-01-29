/*
 * protect.c
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */


#include "../include/protect.h"

Desc	gdt_table[32];
u8 		gdt_ptr[6];
Gate 	idt_table[256];
u8 		idt_ptr[6];
TSS 	tss;
u16 	tss_sel;



/*
 * desc_create
 * ���ܣ�����һ����������
 * ������
 * 		base		�λ�ַ
 * 		limit		���ƫ��
 * 		attribute	����
 * ����ֵ����������
 */
Desc desc_create(u32 base,u32 limit, u16 attribute)
{
	Desc new_desc;
	new_desc.base_low = base & 0xFFFF;
	new_desc.base_mid = (base >> 16) & 0xFF;
	new_desc.base_high = (base >> 24) & 0xFF;
	new_desc.limit_low = limit & 0xFFFF;
	new_desc.limit_high = (limit >> 16);
	*(((u8*)&new_desc)+5) = (u8)attribute;
	*(((u8*)&new_desc)+6) |= (u8)(attribute >> 8);
	return new_desc;
}



/*
 * gate_create
 * ���ܣ�����һ����������
 * ������
 * 		offset		����ƫ��
 * 		selector	������
 * 		attr		����
 * 		param_count	��������
 * ����ֵ����������
 */
Gate gate_create(u32 offset,u32 selector,u8 attr,u8 param_count)
{
	Gate new_gate;
	new_gate.selector = selector;
	new_gate.offset_low = (u16)offset;
	new_gate.offset_high = (u16)(offset >> 16);
	new_gate.param_count = param_count;
	*(((u8*)&new_gate)+5) = (u8)attr;
	return new_gate;
}





/*
 * gdt_add
 * ���ܣ����gdt������
 * ������
 * 		item		��������
 * ����ֵ���ɹ�����������ʧ�ܷ���-1
 */
int gdt_add(Desc item)
{
	for(int i=1;i<32;i++)
	{
		Desc* buf = &gdt_table[i];
		if (!(buf->avl))		/* avl��־λΪ1��Ϊռ�ã�Ϊ0����� */
		{
			gdt_set(i,item);
			return i;
		}
	}
	return -1;
}




/*
 * gdt_remove
 * ���ܣ�ɾ��gdt������
 * ������
 * 		index		����
 * ����ֵ����
 */
void gdt_remove(int index)
{
	gdt_table[index].avl = 0;	/* ���avlλ���� */
}





/*
 * gdt_set
 * ���ܣ��޸�ĳ��
 * ������
 * 		index		����
 * 		item		������
 * ����ֵ����
 */
void gdt_set(int index,Desc item)
{
	item.avl = 1;
	gdt_table[index] = item;
}




/*
 * gdt_get
 * ���ܣ���ȡ��
 * ������
 * 		index		����
 * 		item		����ָ��
 * ����ֵ����
 */
void gdt_get(int index,Desc *item)
{
	*item = gdt_table[index];
}




/*
 * gdt_load
 * ���ܣ�����gdt
 * ��������
 * ����ֵ����
 */
void gdt_load()
{
	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
	u32* p_gdt_base  = (u32*)(&gdt_ptr[2]);
	char *pGdt = (char*)&gdt_ptr[0];

	*p_gdt_limit = 32 * sizeof(Desc) - 1;
	*p_gdt_base  = (u32)&gdt_table[0];

	__asm volatile(
			"movl %0,%%eax		\n"
			"lgdt (%%eax)"
			:
			: "g"(pGdt));
}





/*
 * idt_set
 * ���ܣ��޸�ĳ��
 * ������
 * 		index		����
 * 		item		������
 * ����ֵ����
 */
void idt_set(int index,Gate item)
{
	idt_table[index] = item;
}






/*
 * idt_get
 * ���ܣ���ȡ��
 * ������
 * 		index		����
 * 		item		����ָ��
 * ����ֵ����
 */
void idt_get(int index,Gate *item)
{
	*item = idt_table[index];
}





/*
 * idt_init
 * ���ܣ���ʼ����������
 * ������
 * 		index		����
 * 		handle		����ָ��
 * 		dpl			��Ȩ��
 * ����ֵ����
 */

/* ������ */
void idt_init_call(int index,FunAddr handle,u8 dpl)
{
	idt_table[index] = gate_create((u32)handle,0x8,DA_386CGate | dpl,0);
	return;
}

/* �ж��� */
void idt_init_intr(int index,FunAddr handle,u8 dpl)
{
	idt_table[index] = gate_create((u32)handle,0x8,DA_386IGate | dpl,0);
	return;
}

/* ������ */
void idt_init_trap(int index,FunAddr handle,u8 dpl)
{
	idt_table[index] = gate_create((u32)handle,0x8,DA_386TGate | dpl,0);
	return;
}

/* ������ */
void idt_init_task(int index,FunAddr handle,u8 dpl)
{
	idt_table[index] = gate_create((u32)handle,0x8,DA_TaskGate | dpl,0);
	return;
}






/*
 * idt_load
 * ���ܣ�����gdt
 * ��������
 * ����ֵ����
 */
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




/*
 * idt_tss
 * ���ܣ�����tss
 * ��������
 * ����ֵ����
 */
void tss_load(u16 sel)
{
	__asm volatile(
			"mov %0,%%ax	\n"
			"ltr %%ax"
			: : "g"(sel)
			);
}

