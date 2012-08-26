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

/* 储存单位换算 */
#define MB_TO_PAGE(i)	i*256
#define B_TO_PAGE(i)	i/PAGE_SIZE

/* 计算需要的页表数 */
#define PAGE_TO_NEED_TABLE(i)		i/MAX_PAGE_ENTRY
#define B_TO_NEED_TABLE(i)			PAGE_TO_NEED_TABLE(B_TO_PAGE(i))
#define MB_TO_NEED_TABLE(i)			PAGE_TO_NEED_TABLE(MB_TO_PAGE(i))

/* 计算需要的位图数 */
#define PAGE_TO_NEED_ITEM(i)		i/32
#define B_TO_NEED_ITEM(i)			PAGE_TO_NEED_ITEM(B_TO_PAGE(i))
#define MB_TO_NEED_ITEM(i)			PAGE_TO_NEED_ITEM(MB_TO_PAGE(i))

#define KERNEL_USED_MEM_MB			4
#define KERNEL_USED_MEM_ITEM		MB_TO_NEED_ITEM(KERNEL_USED_MEM_MB)

struct ARDS
{
	u32 BaseAddrLow;
	u32 BaseAddrHigh;
	u32 LenthLow;
	u32 LenthHigh;
	u32 Type;
};

extern struct ARDS mem_info[12];
extern int mem_size;
extern u32* mem_used_map;
extern int mem_used_map_max;
extern int mem_mcr_number;

#endif /* MEMORY_H_ */
