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
 * 初始化gdt
 */
void gdt_init()
{
	gdt_add(desc_create(KernelBase,	0xfffff,	DA_CR|DA_32|DA_LIMIT_4K));				// 系统代码段
	gdt_add(desc_create(KernelBase,	0xfffff,	DA_DRW|DA_32|DA_LIMIT_4K));				// 系统数据段
	gdt_add(desc_create(0,			0xfffff,	DA_CR|DA_32|DA_LIMIT_4K|DA_DPL3));		// 用户代码段
	gdt_add(desc_create(0,			0xfffff,	DA_DRW|DA_32|DA_LIMIT_4K|DA_DPL3));		// 用户数据段
	gdt_add(desc_create(KernelBase+0xb8000,	0xfffff,DA_DRW|DA_32|DA_LIMIT_4K|DA_DPL3));	// 显存段

	// 加载GDT
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



