/*
 * linearspace.c
 *
 *  Created on: 2012-8-28
 *      Author: greenleaf
 */


#include "../include/linearspace.h"
#include "../include/config.h"
//#include "../include/sysapi.h"



/* addr_room data */
hash_info 		linear_space_hash;
hash_info 		page_block_hash;

LinearSpace		linear_space_base[256];
LinearBlock		page_block_base[1024];

LinearBlock		kernel_space;



/*
 * mod_addr_room_create
 * 创建线性地址空间
 */
Handle linear_space_create()
{
	Handle ret = new();
	LinearSpace room,*item;
	hash_set(&linear_space_hash,ret,&room);
	item = hash_getaddr(&linear_space_hash,ret);

	list_init(&item->head);
	list_add(&item->head,&kernel_space.node);
	item->free_count = UserLinearSpacePages;
	item->table = (PageTable*)alloc(0);

	return ret;
}




/*
 * mod_addr_room_alloc
 * 分配线性块
 */
Handle mod_addr_room_alloc(LinearBlock *block)
{
	Handle ret = new();
	LinearBlock new_block;
	if(block != NULL) new_block = *block;
	hash_set(&page_block_hash,ret,&new_block);

	return ret;
}




/*
 * mod_addr_room_add
 * 添加内存块对象到地址空间
 */
Bool mod_addr_room_add(Handle addr_room,Handle linear_block)
{
	int i=0,pages=1;

	LinearBlock *other;

	/* 获取linear_block的指针 */
	LinearBlock *block = (LinearBlock*)hash_getaddr(&page_block_hash,linear_block);

	/* 获取addr_room的指针 */
	LinearSpace *room = (LinearSpace*)hash_getaddr(&linear_space_hash,addr_room);

	/* 新块的起始和结束 */
	int start = block->start;
	int end = block->start + block->block->private;


	/* 只要获得的指针不为NULL，搜索新块位置是否无效 */
	while(other = (LinearBlock*)list_search(&room->head,i))
	{
		/* 已有块的起始和结束 */
		int other_start = other->start;
		int other_end = other->start + other->block->private;

		if ((other_start >= start && other_start <= end) ||	/* 已有块的起始介于新块区域之间 */
			(other_end >= start   && other_end <= end))		/* 已有块的结束介于新块区域之间 */
		{
			return False;
		}
		i++;
	}

	/* 没有返回，则直接插入双向链表 */
	list_add(&room->head,&block->node);

	/* 空闲数减小 */
	OrderToPages(block->block->private,pages);
	room->free_count -= pages;

	return True;
}




/*
 * mod_addr_room_delete
 * 从地址空间中删除对象
 */
void mod_addr_room_del(Handle addr_room,Handle linear_block)
{
	/* 获取linear_block的指针 */
	LinearBlock *block = (LinearBlock*)hash_getaddr(&page_block_hash,linear_block);

	/* 获取addr_room的指针 */
	LinearSpace *room = (LinearSpace*)hash_getaddr(&linear_space_hash,addr_room);

	list_unlink(&block->node);
}





/*
 * mod_addr_room_switch
 * 切换地址空间
 */
void mod_addr_room_switch(Handle addr_room)
{
	LinearBlock *block;		// -32
	/* 获取addr_room的指针 */
	LinearSpace *room = (LinearSpace*)hash_getaddr(&linear_space_hash,addr_room);	// -28


	/* 只要获得的指针不为NULL，循环搜索块 */
	int i=0;	// -12
	while(block = (LinearBlock*)list_search(&room->head,i++))
	{
		/* 如果块没有映射过 */
		if (!(bmp_test(&block->flags,LB_USED)))
		{
			int max = 1;		// -16
			OrderToPages(block->block->private,max);
			for(int j=0;j<max;j++)								// -20
			{
				u32 page_index = (block->start + j) & 0x3FF;	/* 取得页号，高位屏蔽 */					// -36
				/* 页目录项下标等于起始页号除以一个项可以映射的页数 */
				PageEntry *pde = (PageEntry*)&room->table->items[page_index/1024];	// -40
				PageTable *tbl = (PageTable*)Index_To_Addr(pde->addr);				// -24
				PageEntry *pte;	// -36

				/* 如果页表没有分配 */
				if (pde->avl != PDE_ALLOCATED)
				{
					/* 就马上分配一个页 */
					tbl = alloc(0);
					pde->avl = PDE_ALLOCATED;
					pde->addr = Addr_To_Index(tbl);
					pde->p = 1;
				}

				/* 页表项的下标等于起始页号除以一个项可以映射的页数的余数 */
				pte = (PageEntry*)&tbl->items[page_index%1024];

				/* 映射 */
				u32 phy = Page_To_Index(block->block + j);	// -44
				pte->addr = phy;
				pte->p = 1;
				pte->avl = PDE_ALLOCATED;
			}
		}
		bmp_clear(&block->flags,LB_USED);
	}
	/* 加载页表 */
	__asm volatile(
			"movl	%0,%%eax		\n"
			"movl	%%eax,%%cr3		\n"
			"movl	%%cr0,%%eax		\n"
			: : "g"(room->table)
			);
}
