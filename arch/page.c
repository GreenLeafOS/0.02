/*
 * page.c
 *
 *  Created on: 2012-8-6
 *      Author: greenleaf
 */

#include "include/page.h"


/* 直接映射(从页目录表的项中获取页表地址)
 * 参数：
 * 		page_dir 页目录表指针
 * 		phy_page_addr 物理页的地址
 * 		linear_addr	页表映射到的线性地址高位的10位地址,取低10位，高位丢弃。
 * 	返回值：
 * 		0表示成功，-1表示页表不存在
 * */
int page_link(PageTable* page_dir,u32 phy_page_addr,u32 linear_addr)
{
	u16 addr_high = linear_addr >> 22;						/* 线性地址高10位 */
	u16 addr_mid =  (linear_addr >> 12) & 0x3FF;			/* 线性地址中10位 */

	if (page_dir->items[addr_high].p == 1) return -1;		/* 页表不存在 */

	u32 page_tbl_addr = page_dir->items[addr_high].addr;	/* 获取页表地址高20位 */
	PageTable* page_tbl = (PageTable*)(page_tbl_addr << 12);	/* 计算页表指针 */
	page_tbl->items[addr_mid].addr = phy_page_addr;			/* 映射 */

	return 0;
}


/* 加载页目录
 * 参数；
 * 		phy_addr 页目录的物理地址
 * */
void page_directory_load(u32 phy_addr)
{
	__asm volatile(
			"movl	%0,%%eax		\n"
			"movl	%%eax,%%cr3		\n"
			"movl	%%cr0,%%eax		\n"
			"orl	$0x80000000,%%eax	\n"
			"movl	%%eax,%%cr0		\n"
			: : "g"(phy_addr)
			);
}


