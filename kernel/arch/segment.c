/*
 * segment.c
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */


#include "../include/protect.h"
#include "../include/segment.h"
#include "../include/config.h"



/*
 * ��ʼ��gdt
 */
void gdt_init()
{
	gdt_add(desc_create(KernelBase,	0xfffff,	DA_CR|DA_32|DA_LIMIT_4K));				// ϵͳ�����
	gdt_add(desc_create(KernelBase,	0xfffff,	DA_DRW|DA_32|DA_LIMIT_4K));				// ϵͳ���ݶ�
	gdt_add(desc_create(0,			0xfffff,	DA_CR|DA_32|DA_LIMIT_4K|DA_DPL3));		// �û������
	gdt_add(desc_create(0,			0xfffff,	DA_DRW|DA_32|DA_LIMIT_4K|DA_DPL3));		// �û����ݶ�
	gdt_add(desc_create(KernelBase+0xb8000,	0xfffff,DA_DRW|DA_32|DA_LIMIT_4K|DA_DPL3));	// �Դ��

	// ����GDT
	gdt_load();

	__asm volatile(
			"mov	$16,%ax	\n"
			"mov	%ax,%ds	\n"
			"mov	%ax,%es	\n"
			"mov	%ax,%fs	\n"
			"mov	%ax,%ss	\n"

			"mov	$40,%ax	\n"
			"mov	%ax,%gs	\n"

			"movl	$0x80000,%esp"
			);

}



