/*
 * phypage.c
 *
 *  Created on: 2012-8-17
 *      Author: greenleaf
 */


#include "include/phypage.h"
#include "include/sysapi.h"

#include <arch/include/memory.h>

/* phypage data */
struct zone_t zone;



/*
 * Phypage init
 * 参数：无
 * 功能：伙伴系统初始化
 * 返回值：无
 */
void buddy_init()
{
	/* 获得内存大小 */
	for(int i=0;i<mem_mcr_number;i++)
	{
		if(mem_info[i].Type == 1)
			if (mem_info[i].BaseAddrLow + mem_info[i].LenthLow > mem_size)
				mem_size = mem_info[i].BaseAddrLow + mem_info[i].LenthLow;
	}

	PhyPage* base = (PhyPage*)0x400000;				/* 起始地址4M以后 */

	zone.page_count = mem_size/PAGE_SIZE;			/* 页数 */
	zone.page_base = base;							/* 存放页描述符的起始地址 */

	int biggest_count = (zone.page_count/BIGGEST_SIZE)+1;	/* 最大块的个数 */
	int page_count = (zone.page_count*sizeof(PhyPage))/PAGE_SIZE + 1;	/* 页描述符占用空间 */

	/* 初始化小块 */
	for(int i=0;i<11;i++)
	{
		list_init(&zone.free_area[i].free_list);
		zone.free_area[i].nr_free = 0;
	}

	/* 初始化大块,保留4M作为内核代码空间 */
	for (int j=1;j<biggest_count;j++)	/* j小于最大块数 */
	{
		PhyPage* block = base + (j * BIGGEST_SIZE);
		block->private = BIGGEST_SIZE;
		zone.free_pages += BIGGEST_SIZE;
		list_addtail(&zone.free_area[10].free_list,&block->node);
	}
	int order = 0;
	PagesToOrder(page_count,order);

	page_alloc(order);

	return;
}





/*
 * phypage search
 * 参数：
 * 		order 块大小（0 表示一个单页块，1 表示一个两页块，2表示四个页块）
 * 功能：搜索一个可用的块大小
 * 返回值：大小（幂级）
 */
int page_search(int order)
{
	struct free_area *area;
	unsigned int curr_order;

	for (curr_order=order;curr_order<11;curr_order++)
	{
		area = zone.free_area + curr_order;	/* 获取相应大小的空闲块链表描述符 */
		if (!list_empty(&area->free_list))	/* 如果链表不为空 */
			return curr_order;				/* 则返回 */
	}
	return -1;
}





/*
 * phypage alloc
 * 参数：order 大小（幂级）
 * 功能：分配一个块
 * 返回值：描述符指针
 */
PhyPage* page_alloc(int order)
{
	/* 获取可分配的块大小对数 */
	int curr_order = page_search(order);
	if (curr_order == -1) return NULL;

	/* 获取相应大小的空闲块链表描述符 */
	struct free_area *area = zone.free_area + curr_order;

	/* 获取块起始的页描述符指针 */
	PhyPage* page = (PhyPage*)area->free_list.next;

	list_unlink(&page->node);			/* 解除分配块的起始页描述符的链接 */
	bmp_set(&page->flags,PG_BUSY);		/* 设置块使用位 */
	page->private = curr_order;			/* 块大小计数器 */
	area->nr_free--;					/* 空闲块减1 */
	zone.free_pages -= 1UL << order;	/* 空闲总页数减去块大小 */


	int	size = 1 << curr_order;
	while (curr_order > order)
	{
		area--;								/* 指针指向分裂成的小块area，如果分配大小为16字节的块，则应把分裂的伙伴插入8字节空闲链表 */
		curr_order--;						/* 块大小减小（幂级） */
		size >>= 1;							/* 块大小减小（页单位） */
		PhyPage* buddy = page + size;		/* buddy指向分裂开的2块中的后一块，page指向分配的块，加上块大小，则等于伙伴的地址 */

		/* 插入buddy到对应空闲链表 */
		list_add(&area->free_list,&buddy->node);
		area->nr_free++;					/* 空闲块数加1 */
		buddy->private = curr_order;		/* 设置块大小计数器 */
	}
	return page;
}




/*
 * phypage free
 * 参数：页框描述符指针,块大小
 * 功能：释放指针指向的块
 * 返回值：
 */
void page_free(PhyPage* page,int order)
{
	PhyPage* base = zone.page_base;		/* 起始页框描述符地址 */
	u32 page_idx = page - base;			/* 要释放页的下标 */
	PhyPage* buddy, *coalesced;
	u32 buddy_idx;

	zone.free_pages += (1 << order);		/* 空闲块增加 */

	/* 循环合并块 */
	while (order < 10)
	{
		buddy_idx = page_idx ^ (1 << order);		/* 伙伴的下标，也许在页之前，也许在页之后 */
		PhyPage* buddy = base + buddy_idx;			/* 伙伴页描述符的指针 base[buddy_idx] */
		if (!page_is_buddy(buddy, order)) break;	/* 如果伙伴不空闲，则退出 */

	    list_unlink(&buddy->node);					/* 伙伴块解除链接 */
	    zone.free_area[order].nr_free--;			/* 伙伴块area的空闲块减1 */
	    bmp_clear(&buddy->flags,PG_BUSY);			/* 清除使用位 */
	    buddy->private = 0;							/* 块大小计数器清零,因为块被合并，则不再是块起始 */
	    page_idx &= buddy_idx;   					/* 合并，page_idx是合成后的大块下标 */
	    order++;
	}
	coalesced = base + page_idx;			/* 最终合成的巨大块 */
	coalesced->private = order;				/* 块大小计数器 */

	list_add(&zone.free_area[order].free_list,&coalesced->node);	/* 巨大块插入对应大小链表 */
	zone.free_area[order].nr_free++;		/* 巨大块area空闲块加1 */
	bmp_clear(&coalesced->flags,PG_BUSY);	/* 清除使用位 */

	return;
}





/*
 * phypage isbuddy
 * 参数：
 * 		page 页描述符指针
 * 		order 块大小（幂级）
 * 功能：检测伙伴是否空闲
 * 返回值：空闲返回True，忙返回False
 */
Bool page_is_buddy(PhyPage *page, int order)
{
	if (page->private == order && !bmp_test(&page->flags,PG_BUSY))
		return True;
	return False;
}






/*
 * SysApi alloc
 * 参数：size 页数
 * 功能：分配页框
 * 返回值：返回头页框的描述符指针
 */
void* alloc(u32 size)
{
	int order = 0;
	PagesToOrder(size,order);
	return (void*)Page_To_Addr(page_alloc(order));
}




/*
 * SysApi free
 * 参数：p 释放块的起始页物理地址
 * 功能：释放页框
 * 返回值：无
 */
void free(void* addr)
{
	PhyPage* page = Addr_To_Page(addr);
	page_free(page,page->private);
	return;
}
