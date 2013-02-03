/*
 * memory.h
 *
 *  Created on: 2012-8-6
 *      Author: greenleaf
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <lib/include/type.h>
#include <lib/include/list.h>
#include <lib/include/setbit.h>

#define MAX_PAGE_ENTRY	1024
#define PAGE_SIZE		4096
#define PageSize		4096

/* 常用的内存大小 */
#define _1M				1*1024*1024
#define _32M			32*1024*1024
#define _512M			512*1024*1024
#define _4G				4096*1024*1024

/* 储存单位换算 */
#define MB_TO_PAGE(i)	i*256
#define B_TO_PAGE(i)	i/PAGE_SIZE
#define MB_TO_B(i)		i*1024*1024
/* 计算需要的页表数 */
#define PAGE_TO_NEED_TABLE(i)		i/MAX_PAGE_ENTRY
#define B_TO_NEED_TABLE(i)			PAGE_TO_NEED_TABLE(B_TO_PAGE(i))
#define MB_TO_NEED_TABLE(i)			PAGE_TO_NEED_TABLE(MB_TO_PAGE(i))


/* 与内存相关的数据 */
struct ARDS
{
	u32 BaseAddrLow;
	u32 BaseAddrHigh;
	u32 LenthLow;
	u32 LenthHigh;
	u32 Type;
};

extern struct ARDS mem_info[12];
extern u32 mem_size;
extern int mem_mcr_number;


/* 内核内存管理 */
typedef struct free_list_item
{
	ListNode node;
	u32 start;
	u32 size;
}FreeItem;

/* 空闲列表 */
FreeItem free_items[256];
ListHead free_list;
ListHead used_list;

/* 内核空间大小 */
int kernel_mem_size;

/* 内核内存分配函数 */
void* memory_alloc(u32 size);
void memory_free(void* p);
void memory_merger();

#endif /* MEMORY_H_ */
