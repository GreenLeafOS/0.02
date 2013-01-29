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
 * �������Ե�ַ�ռ�
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
 * �������Կ�
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
 * ����ڴ����󵽵�ַ�ռ�
 */
Bool mod_addr_room_add(Handle addr_room,Handle linear_block)
{
	int i=0,pages=1;

	LinearBlock *other;

	/* ��ȡlinear_block��ָ�� */
	LinearBlock *block = (LinearBlock*)hash_getaddr(&page_block_hash,linear_block);

	/* ��ȡaddr_room��ָ�� */
	LinearSpace *room = (LinearSpace*)hash_getaddr(&linear_space_hash,addr_room);

	/* �¿����ʼ�ͽ��� */
	int start = block->start;
	int end = block->start + block->block->private;


	/* ֻҪ��õ�ָ�벻ΪNULL�������¿�λ���Ƿ���Ч */
	while(other = (LinearBlock*)list_search(&room->head,i))
	{
		/* ���п����ʼ�ͽ��� */
		int other_start = other->start;
		int other_end = other->start + other->block->private;

		if ((other_start >= start && other_start <= end) ||	/* ���п����ʼ�����¿�����֮�� */
			(other_end >= start   && other_end <= end))		/* ���п�Ľ��������¿�����֮�� */
		{
			return False;
		}
		i++;
	}

	/* û�з��أ���ֱ�Ӳ���˫������ */
	list_add(&room->head,&block->node);

	/* ��������С */
	OrderToPages(block->block->private,pages);
	room->free_count -= pages;

	return True;
}




/*
 * mod_addr_room_delete
 * �ӵ�ַ�ռ���ɾ������
 */
void mod_addr_room_del(Handle addr_room,Handle linear_block)
{
	/* ��ȡlinear_block��ָ�� */
	LinearBlock *block = (LinearBlock*)hash_getaddr(&page_block_hash,linear_block);

	/* ��ȡaddr_room��ָ�� */
	LinearSpace *room = (LinearSpace*)hash_getaddr(&linear_space_hash,addr_room);

	list_unlink(&block->node);
}





/*
 * mod_addr_room_switch
 * �л���ַ�ռ�
 */
void mod_addr_room_switch(Handle addr_room)
{
	LinearBlock *block;		// -32
	/* ��ȡaddr_room��ָ�� */
	LinearSpace *room = (LinearSpace*)hash_getaddr(&linear_space_hash,addr_room);	// -28


	/* ֻҪ��õ�ָ�벻ΪNULL��ѭ�������� */
	int i=0;	// -12
	while(block = (LinearBlock*)list_search(&room->head,i++))
	{
		/* �����û��ӳ��� */
		if (!(bmp_test(&block->flags,LB_USED)))
		{
			int max = 1;		// -16
			OrderToPages(block->block->private,max);
			for(int j=0;j<max;j++)								// -20
			{
				u32 page_index = (block->start + j) & 0x3FF;	/* ȡ��ҳ�ţ���λ���� */					// -36
				/* ҳĿ¼���±������ʼҳ�ų���һ�������ӳ���ҳ�� */
				PageEntry *pde = (PageEntry*)&room->table->items[page_index/1024];	// -40
				PageTable *tbl = (PageTable*)Index_To_Addr(pde->addr);				// -24
				PageEntry *pte;	// -36

				/* ���ҳ��û�з��� */
				if (pde->avl != PDE_ALLOCATED)
				{
					/* �����Ϸ���һ��ҳ */
					tbl = alloc(0);
					pde->avl = PDE_ALLOCATED;
					pde->addr = Addr_To_Index(tbl);
					pde->p = 1;
				}

				/* ҳ������±������ʼҳ�ų���һ�������ӳ���ҳ�������� */
				pte = (PageEntry*)&tbl->items[page_index%1024];

				/* ӳ�� */
				u32 phy = Page_To_Index(block->block + j);	// -44
				pte->addr = phy;
				pte->p = 1;
				pte->avl = PDE_ALLOCATED;
			}
		}
		bmp_clear(&block->flags,LB_USED);
	}
	/* ����ҳ�� */
	__asm volatile(
			"movl	%0,%%eax		\n"
			"movl	%%eax,%%cr3		\n"
			"movl	%%cr0,%%eax		\n"
			: : "g"(room->table)
			);
}
