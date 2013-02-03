/*
 * page_init.c
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */

#include "../include/phypage.h"
#include "../include/linearspace.h"
#include "../include/config.h"
#include "../include/protect.h"
#include "lib/include/string.h"

void page_init()
{

	/* 建立内核永久映射 */
	u32* PgDir = (u32*)0x30000;
	u32* PgTbl = (u32*)0x31000;

	/* 初始化页目录 */
	memset((void *)0x30000,0,0x1000);
	int base = KernelBase/PageSize/(PageSize/4);
	for(int j=0;j<((kernel_mem_size/PageSize+0x3ff)>>10);j++)
	{
		PgDir[base + j] = (0x31000 + PageSize*j) | PG_P | PG_USU | PG_RWW;
	}

	/* 初始化页表 */
	for(int k=0;k<kernel_mem_size/PageSize;k++)
	{
		PgTbl[k] = (k*PageSize) | PG_P | PG_USU | PG_RWW;
	}


	/* 加载页目录 */
	__asm volatile(
			"movl	$0x30000,%eax	\n"
			"movl	%eax,%cr3		\n");

	buddy_init();
	kernel_space.start = KernelBase/PageSize;
	kernel_space.block = page_alloc(11);
	return;
}
