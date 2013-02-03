/*
 * memory.c
 *
 *  Created on: 2013-1-31
 *      Author: greenleaf
 * 在系统开始运行的时候规划内核内存分配
 */

#include "include/config.h"
#include "include/memory.h"
#include "include/phypage.h"
#include "lib/include/string.h"

void*	phypage_base;

FreeItem free_items[256];
ListHead free_list;
ListHead used_list;

int kernel_mem_size;


/*
 * 清空系统bss段内存
 * 初始化内存分配等
 */
void memory_init()
{
	/* 内嵌汇编代码，先把bss段内存清空 */
	__asm volatile("mem_set:	\n"
			"movl	$_bss,%eax	\n"
			"movl	$_end,%ecx	\n"
			"subl	%eax,%ecx	\n"
			"_mem_set:			\n"
			"movl	$0,%es:(%eax,%ecx)	\n"
			"subl	$4,%ecx		\n"
			"jnz	_mem_set");

	/* 获得内存大小 */
	for(int i=0;i<mem_mcr_number;i++)
	{
		if(mem_info[i].Type == 1)
			if (mem_info[i].BaseAddrLow + mem_info[i].LenthLow > mem_size)
				mem_size = mem_info[i].BaseAddrLow + mem_info[i].LenthLow;
	}

	/* 修整内存大小(修正一点小小的误差) */
	mem_size = _1M * ((mem_size + _1M - 1)>>20);

	/* 初始化内核内存分配机制 */
	list_init(&free_list);
	list_init(&used_list);

	u32 size = (mem_size/PageSize)*sizeof(PhyPage);	// 物理页描述符占用的内存空间的大小

	/* 内存布局分配 */
	if (!(mem_size > _32M))
	{
		/* 内存大小不大于32M */
		kernel_mem_size = MB_TO_B(4);		// 内核空间4M，很挤阿

		/* 物理页描述符 */
		phypage_base = (void*)0x200000;		// 2M开始存放页描述符

		free_items[0].start = (u32)phypage_base + size;				// 计算剩余的空间的起始地址
		free_items[0].size = kernel_mem_size - free_items[0].start;	// 计算剩余的空间的大小
		list_addtail(&free_list,&free_items[0].node);				// 插入空闲链表
	}
	else if (!(mem_size > _512M))
	{
		/* 内存大小不大于512M */
		kernel_mem_size = MB_TO_B(16);		// 内核空间16M，宽点了
		phypage_base = (void*)0x200000;		// 2M开始存放页描述符，刚好4M结束

		free_items[0].start = MB_TO_B(4);				// 计算剩余的空间的起始地址
		free_items[0].size = MB_TO_B(12);				// 计算剩余的空间的大小
		list_addtail(&free_list,&free_items[0].node);	// 插入空闲链表
	}
	else
	{
		/* 如果大于512M */
		kernel_mem_size = MB_TO_B(32);		// 内核空间32M，十分宽敞
		phypage_base = (void*)0x1600000;	// 如果内存大于512M，则从16M开始存放页描述符

		/* 16M ~ 32M 中没有占用的部分 */
		free_items[0].start = (u32)phypage_base + size;				// 计算剩余的空间的起始地址
		free_items[0].size = kernel_mem_size - free_items[0].start;	// 计算剩余的空间的大小
		list_addtail(&free_list,&free_items[0].node);				// 插入空闲链表

		/* 2M ~ 16M */
		free_items[1].start = MB_TO_B(2);				// 计算剩余的空间的起始地址
		free_items[1].size = MB_TO_B(16);				// 计算剩余的空间的大小
		list_addtail(&free_list,&free_items[1].node);	// 插入空闲链表
	}
}



/*
 * 在内核空间分配内存
 * */
void* memory_alloc(u32 size)
{
	FreeItem* ret = (FreeItem*)&free_list;
	do
	{
		ret = (FreeItem*)ret->node.next;

		/* 如果符合条件，就分配 */
		if (ret->size >= size)
		{
			void* pRet = (void*)ret->start;

			/* 插入一个已使用内存块的项 */
			for(int i=0;i<256;i++)
			{
				if(free_items[i].start == 0)
				{
					list_addtail(&used_list,&free_items[i].node);	// 插入已使用的队列
					free_items[i].start = ret->start;				// 分配的起始地址
					free_items[i].size = size;						// 分配的大小
					break;
				}
			}

			/* 修改原来的空闲块的大小和起始地址 */
			ret->start += size;
			if((ret->size -= size) == 0)
			{
				list_unlink(&ret->node);
				memset((void*)ret,0,sizeof(FreeItem));
			}
			return pRet;
		}
	}
	while(ret != NULL && ret != (FreeItem*)&free_list);		// 如果没遍历完，就继续遍历

	return NULL;
}


/*
 * 在内核空间释放内存
 */
void memory_free(void* p)
{
	FreeItem* used = (FreeItem*)&used_list;
	do
	{
		used = (FreeItem*)used->node.next;

		/* 匹配成功 */
		if((void*)used->start == p)
		{
			list_unlink(&used->node);				// 先解除链接

			/* 然后作为一个空闲块插入 */
			FreeItem* block = (FreeItem*)free_list.next;
			FreeItem* front = (FreeItem*)&free_list;
			do
			{
				/* 一定要插入适当的位置 */
				if (used->start > block->start)
				{
					front = block;
				}
				else
				{
					break;
				}
				block = (FreeItem*)block->node.next;
			}
			while(block != NULL && block != (FreeItem*)&free_list);

			list_insert(&free_list,&used->node,&front->node);
			/* 合并空闲块 */
			memory_merger();
			break;
		}

	}
	while(used != NULL && used != (FreeItem*)&used_list);		// 如果没遍历完，就继续遍历
	return;
}


/*
 *合并空闲块
 */
void memory_merger()
{
	/* 合并空闲块 */
	FreeItem* block = (FreeItem*)free_list.next;
	do
	{
		FreeItem* next = (FreeItem*)block->node.next;

		/* 和后一个空闲块合并 */
		if (block->start + block->size == next->start)
		{
			/* 合并在block节点 */
			block->size += next->size;

			/* 销毁next节点 */
			list_unlink(&next->node);
			memset((void*)next,0,sizeof(FreeItem));
		}
		else
		{
			block = (FreeItem*)block->node.next;
		}
	}
	while(block != NULL && block != (FreeItem*)&free_list);
}

