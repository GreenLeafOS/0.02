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
 * ��������
 * ���ܣ����ϵͳ��ʼ��
 * ����ֵ����
 */
void buddy_init()
{
	/* ����ڴ��С */
	for(int i=0;i<mem_mcr_number;i++)
	{
		if(mem_info[i].Type == 1)
			if (mem_info[i].BaseAddrLow + mem_info[i].LenthLow > mem_size)
				mem_size = mem_info[i].BaseAddrLow + mem_info[i].LenthLow;
	}

	PhyPage* base = (PhyPage*)0x400000;				/* ��ʼ��ַ4M�Ժ� */

	zone.page_count = mem_size/PAGE_SIZE;			/* ҳ�� */
	zone.page_base = base;							/* ���ҳ����������ʼ��ַ */

	int biggest_count = (zone.page_count/BIGGEST_SIZE)+1;	/* ����ĸ��� */
	int page_count = (zone.page_count*sizeof(PhyPage))/PAGE_SIZE + 1;	/* ҳ������ռ�ÿռ� */

	/* ��ʼ��С�� */
	for(int i=0;i<11;i++)
	{
		list_init(&zone.free_area[i].free_list);
		zone.free_area[i].nr_free = 0;
	}

	/* ��ʼ�����,����4M��Ϊ�ں˴���ռ� */
	for (int j=1;j<biggest_count;j++)	/* jС�������� */
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
 * ������
 * 		order ���С��0 ��ʾһ����ҳ�飬1 ��ʾһ����ҳ�飬2��ʾ�ĸ�ҳ�飩
 * ���ܣ�����һ�����õĿ��С
 * ����ֵ����С���ݼ���
 */
int page_search(int order)
{
	struct free_area *area;
	unsigned int curr_order;

	for (curr_order=order;curr_order<11;curr_order++)
	{
		area = zone.free_area + curr_order;	/* ��ȡ��Ӧ��С�Ŀ��п����������� */
		if (!list_empty(&area->free_list))	/* �������Ϊ�� */
			return curr_order;				/* �򷵻� */
	}
	return -1;
}





/*
 * phypage alloc
 * ������order ��С���ݼ���
 * ���ܣ�����һ����
 * ����ֵ��������ָ��
 */
PhyPage* page_alloc(int order)
{
	/* ��ȡ�ɷ���Ŀ��С���� */
	int curr_order = page_search(order);
	if (curr_order == -1) return NULL;

	/* ��ȡ��Ӧ��С�Ŀ��п����������� */
	struct free_area *area = zone.free_area + curr_order;

	/* ��ȡ����ʼ��ҳ������ָ�� */
	PhyPage* page = (PhyPage*)area->free_list.next;

	list_unlink(&page->node);			/* �����������ʼҳ������������ */
	bmp_set(&page->flags,PG_BUSY);		/* ���ÿ�ʹ��λ */
	page->private = curr_order;			/* ���С������ */
	area->nr_free--;					/* ���п��1 */
	zone.free_pages -= 1UL << order;	/* ������ҳ����ȥ���С */


	int	size = 1 << curr_order;
	while (curr_order > order)
	{
		area--;								/* ָ��ָ����ѳɵ�С��area����������СΪ16�ֽڵĿ飬��Ӧ�ѷ��ѵĻ�����8�ֽڿ������� */
		curr_order--;						/* ���С��С���ݼ��� */
		size >>= 1;							/* ���С��С��ҳ��λ�� */
		PhyPage* buddy = page + size;		/* buddyָ����ѿ���2���еĺ�һ�飬pageָ�����Ŀ飬���Ͽ��С������ڻ��ĵ�ַ */

		/* ����buddy����Ӧ�������� */
		list_add(&area->free_list,&buddy->node);
		area->nr_free++;					/* ���п�����1 */
		buddy->private = curr_order;		/* ���ÿ��С������ */
	}
	return page;
}




/*
 * phypage free
 * ������ҳ��������ָ��,���С
 * ���ܣ��ͷ�ָ��ָ��Ŀ�
 * ����ֵ��
 */
void page_free(PhyPage* page,int order)
{
	PhyPage* base = zone.page_base;		/* ��ʼҳ����������ַ */
	u32 page_idx = page - base;			/* Ҫ�ͷ�ҳ���±� */
	PhyPage* buddy, *coalesced;
	u32 buddy_idx;

	zone.free_pages += (1 << order);		/* ���п����� */

	/* ѭ���ϲ��� */
	while (order < 10)
	{
		buddy_idx = page_idx ^ (1 << order);		/* �����±꣬Ҳ����ҳ֮ǰ��Ҳ����ҳ֮�� */
		PhyPage* buddy = base + buddy_idx;			/* ���ҳ��������ָ�� base[buddy_idx] */
		if (!page_is_buddy(buddy, order)) break;	/* �����鲻���У����˳� */

	    list_unlink(&buddy->node);					/* ���������� */
	    zone.free_area[order].nr_free--;			/* ����area�Ŀ��п��1 */
	    bmp_clear(&buddy->flags,PG_BUSY);			/* ���ʹ��λ */
	    buddy->private = 0;							/* ���С����������,��Ϊ�鱻�ϲ��������ǿ���ʼ */
	    page_idx &= buddy_idx;   					/* �ϲ���page_idx�Ǻϳɺ�Ĵ���±� */
	    order++;
	}
	coalesced = base + page_idx;			/* ���պϳɵľ޴�� */
	coalesced->private = order;				/* ���С������ */

	list_add(&zone.free_area[order].free_list,&coalesced->node);	/* �޴������Ӧ��С���� */
	zone.free_area[order].nr_free++;		/* �޴��area���п��1 */
	bmp_clear(&coalesced->flags,PG_BUSY);	/* ���ʹ��λ */

	return;
}





/*
 * phypage isbuddy
 * ������
 * 		page ҳ������ָ��
 * 		order ���С���ݼ���
 * ���ܣ�������Ƿ����
 * ����ֵ�����з���True��æ����False
 */
Bool page_is_buddy(PhyPage *page, int order)
{
	if (page->private == order && !bmp_test(&page->flags,PG_BUSY))
		return True;
	return False;
}






/*
 * SysApi alloc
 * ������size ҳ��
 * ���ܣ�����ҳ��
 * ����ֵ������ͷҳ���������ָ��
 */
void* alloc(u32 size)
{
	int order = 0;
	PagesToOrder(size,order);
	return (void*)Page_To_Addr(page_alloc(order));
}




/*
 * SysApi free
 * ������p �ͷſ����ʼҳ�����ַ
 * ���ܣ��ͷ�ҳ��
 * ����ֵ����
 */
void free(void* addr)
{
	PhyPage* page = Addr_To_Page(addr);
	page_free(page,page->private);
	return;
}
