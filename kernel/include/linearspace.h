/*
 * linearspace.h
 *
 *  Created on: 2012-8-28
 *      Author: greenleaf
 */

#ifndef LINEARSPACE_H_
#define LINEARSPACE_H_

#include "../include/phypage.h"


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


typedef struct linear_space
{
	ListHead 	head;		// 线性空间块的双向链表头指针
	u32 		free_count;	// 剩余的空间还有多少页框
	PageTable	*table;		// 页表的首地址
}LinearSpace;


typedef struct linear_block
{
	ListNode	node;
	u32 		flags;		// 线性空间块状态、类型
	u32			start;		// 映射到的线性地址空间的起始页
	LinearSpace	*room;		// 映射到的线性地址空间指针
	PhyPage		*block;		// 物理页块描述符指针
}LinearBlock;


/* linear block bit*/
#define LB_USED			0	// 1表示已经映射过了

LinearBlock		kernel_space;

#endif /* LINEARSPACE_H_ */
