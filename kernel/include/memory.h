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

/* ���õ��ڴ��С */
#define _1M				1*1024*1024
#define _32M			32*1024*1024
#define _512M			512*1024*1024
#define _4G				4096*1024*1024

/* ���浥λ���� */
#define MB_TO_PAGE(i)	i*256
#define B_TO_PAGE(i)	i/PAGE_SIZE
#define MB_TO_B(i)		i*1024*1024
/* ������Ҫ��ҳ���� */
#define PAGE_TO_NEED_TABLE(i)		i/MAX_PAGE_ENTRY
#define B_TO_NEED_TABLE(i)			PAGE_TO_NEED_TABLE(B_TO_PAGE(i))
#define MB_TO_NEED_TABLE(i)			PAGE_TO_NEED_TABLE(MB_TO_PAGE(i))


/* ���ڴ���ص����� */
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


/* �ں��ڴ���� */
typedef struct free_list_item
{
	ListNode node;
	u32 start;
	u32 size;
}FreeItem;

/* �����б� */
FreeItem free_items[256];
ListHead free_list;
ListHead used_list;

/* �ں˿ռ��С */
int kernel_mem_size;

/* �ں��ڴ���亯�� */
void* memory_alloc(u32 size);
void memory_free(void* p);
void memory_merger();

#endif /* MEMORY_H_ */
