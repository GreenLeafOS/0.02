/*
 * page.h
 *
 *  Created on: 2012-8-6
 *      Author: greenleaf
 */

#ifndef PAGE_H_
#define PAGE_H_

#include "memory.h"

/* 页表页目录项结构 */
typedef struct page_entry
{
	u32 p			: 1;			/* P位 Bit0 */
	u32 rw			: 1;			/* R/W位 Bit1*/
	u32 us			: 1;			/* U/S位 Bit2*/
	u32	pwt			: 1;			/* 缓冲策略位 Bit3*/
	u32 pcd			: 1;			/* 缓冲许可位 Bit4*/
	u32 a			: 1;			/* A位 Bit5*/
	u32 d			: 1;			/* D位 Bit6*/
	u32	ps			: 1;			/* 页大小 Bit7*/
	u32 g			: 1;			/* 指示全局页 Bit8*/
	u32 avl			: 3;			/* 系统使用保留位 Bit9~Bit11 */
	u32 addr		: 20;			/* 物理地址高20位 Bit12~Bit31*/
}PageEntry;


#define PDE_ALLOCATED		1		/* 已经分配 */

/*页表页目录结构*/
typedef struct page_table
{
	PageEntry items[PAGE_SIZE/sizeof(PageEntry)];
}PageTable;


/* 初始化 */
void page_init();


/* 页目录链接页表（页表映射到页目录） */
void page_link_table(PageTable* page_dir,PageTable* page_tbl,u16 linear_addr);
/* 页表链接地址（线性地址映射到物理地址） */
void page_link_addr(PageTable* page_tbl,u32 phy_page_addr,u16 linear_addr);
/* 直接映射(从页目录表的项中获取页表地址) */
int page_link(PageTable* page_dir,u32 phy_page_addr,u32 linear_addr);


/* 加载页目录 */
void page_directory_load(u32 phy_addr);



/*
 * 在写代码之前，我们先算一笔账。
 * 表项: 4 b
 * 页目录:4 kb = 4096 b
 * 页表: 4 kb = 4096 b
 * 线性地址的高10位是页目录表的偏移
 * 因为表是4kb大，所以所以PDE在表中的偏移不会超过4kb,即4096b,所以只需要12位即可。
 * 因为表项是4b大，所以PDE在表中的偏移总会是4的倍数，即偏移的低2位总是0。现在看来，10位足够了。
 * 那么真正的16位偏移应该是 10位偏移左移2位，高位补0得到。
 * 反过来，10位线性偏移则等于 16位偏移右移，高位截断得到。
 * 这时的10位偏移恰好是数组的下标。（好好想想，就懂了）
 * 分页单元在寻址时，可以看作是执行了这么一个函数(C语言伪代码)
 * bits函数的作用是取相应位。bit10指10位数，以此类推。
 * u32 linear_to_phy(u32 linear)
 * {
 *		bit10 high = bits(linear,22,31);
 *		bit10 mid = bits(linear,12,21);
 *		bit12 low = bits(linear,0,11);
 *
 *		pde = page_dir->item[high];
 *		page_tbl = (pde->addr) << 12;
 *		pte = page_tbl->item[mid];
 *		phy_page = (pte->addr) << 12;
 *		phy_addr = phy_page + low;
 *		return phy_addr;
 * }
 *
 * 了解了这个，写起来是不是游刃有余了呢？
 * 调试命令：
 * 		x /256bx 0x16000
 * 		b 0x10bd4
 * 		b 0x10c07
 *
 * */



/*
 * ebp - 20 : *page_dir
 * ebp - 28 : i
 * ebp - 16 : *page_tbl
 * ebp - 24 : j
 * 参数
 * esp + 8  : i
 * esp + 4  : *page_tbl
 * esp + 0  : *page_dir
 * 参数
 * esp + 8  : j
 * esp + 4  : phy_addr
 * esp + 0  : *page_tbl
 */


/*
 * 页目录链接页表（页表映射到页目录）
 * 参数：
 * 		page_dir 页目录表指针
 * 		page_tbl 页表指针。取高20位
 * 		linear_index	页表映射到的高位的10位地址,取低10位，高位丢弃。
 * */
#define page_link_table(page_dir,page_tbl,linear_index)	\
	((PageTable*)page_dir)->items[(u16)linear_index & 0x3FF].addr = (u32)((page_tbl)>>12);


/* 页表链接地址（线性地址映射到物理地址）
 * 参数：
 * 		page_tbl 页表指针
 * 		phy_page_addr 物理页的地址.取高20位
 * 		linear_index	页表映射到的高位的10位地址,取低10位，高位丢弃。
 * */
#define page_link_addr(page_tbl,phy_page_addr,linear_index) \
	((PageTable*)page_tbl)->items[(u16)linear_index & 0x3FF].addr = (u32)((phy_page_addr)>>12);



#endif /* PAGE_H_ */
