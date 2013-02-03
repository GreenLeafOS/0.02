/*
 * memory.c
 *
 *  Created on: 2013-1-31
 *      Author: greenleaf
 * ��ϵͳ��ʼ���е�ʱ��滮�ں��ڴ����
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
 * ���ϵͳbss���ڴ�
 * ��ʼ���ڴ�����
 */
void memory_init()
{
	/* ��Ƕ�����룬�Ȱ�bss���ڴ���� */
	__asm volatile("mem_set:	\n"
			"movl	$_bss,%eax	\n"
			"movl	$_end,%ecx	\n"
			"subl	%eax,%ecx	\n"
			"_mem_set:			\n"
			"movl	$0,%es:(%eax,%ecx)	\n"
			"subl	$4,%ecx		\n"
			"jnz	_mem_set");

	/* ����ڴ��С */
	for(int i=0;i<mem_mcr_number;i++)
	{
		if(mem_info[i].Type == 1)
			if (mem_info[i].BaseAddrLow + mem_info[i].LenthLow > mem_size)
				mem_size = mem_info[i].BaseAddrLow + mem_info[i].LenthLow;
	}

	/* �����ڴ��С(����һ��СС�����) */
	mem_size = _1M * ((mem_size + _1M - 1)>>20);

	/* ��ʼ���ں��ڴ������� */
	list_init(&free_list);
	list_init(&used_list);

	u32 size = (mem_size/PageSize)*sizeof(PhyPage);	// ����ҳ������ռ�õ��ڴ�ռ�Ĵ�С

	/* �ڴ沼�ַ��� */
	if (!(mem_size > _32M))
	{
		/* �ڴ��С������32M */
		kernel_mem_size = MB_TO_B(4);		// �ں˿ռ�4M���ܼ���

		/* ����ҳ������ */
		phypage_base = (void*)0x200000;		// 2M��ʼ���ҳ������

		free_items[0].start = (u32)phypage_base + size;				// ����ʣ��Ŀռ����ʼ��ַ
		free_items[0].size = kernel_mem_size - free_items[0].start;	// ����ʣ��Ŀռ�Ĵ�С
		list_addtail(&free_list,&free_items[0].node);				// �����������
	}
	else if (!(mem_size > _512M))
	{
		/* �ڴ��С������512M */
		kernel_mem_size = MB_TO_B(16);		// �ں˿ռ�16M�������
		phypage_base = (void*)0x200000;		// 2M��ʼ���ҳ���������պ�4M����

		free_items[0].start = MB_TO_B(4);				// ����ʣ��Ŀռ����ʼ��ַ
		free_items[0].size = MB_TO_B(12);				// ����ʣ��Ŀռ�Ĵ�С
		list_addtail(&free_list,&free_items[0].node);	// �����������
	}
	else
	{
		/* �������512M */
		kernel_mem_size = MB_TO_B(32);		// �ں˿ռ�32M��ʮ�ֿ�
		phypage_base = (void*)0x1600000;	// ����ڴ����512M�����16M��ʼ���ҳ������

		/* 16M ~ 32M ��û��ռ�õĲ��� */
		free_items[0].start = (u32)phypage_base + size;				// ����ʣ��Ŀռ����ʼ��ַ
		free_items[0].size = kernel_mem_size - free_items[0].start;	// ����ʣ��Ŀռ�Ĵ�С
		list_addtail(&free_list,&free_items[0].node);				// �����������

		/* 2M ~ 16M */
		free_items[1].start = MB_TO_B(2);				// ����ʣ��Ŀռ����ʼ��ַ
		free_items[1].size = MB_TO_B(16);				// ����ʣ��Ŀռ�Ĵ�С
		list_addtail(&free_list,&free_items[1].node);	// �����������
	}
}



/*
 * ���ں˿ռ�����ڴ�
 * */
void* memory_alloc(u32 size)
{
	FreeItem* ret = (FreeItem*)&free_list;
	do
	{
		ret = (FreeItem*)ret->node.next;

		/* ��������������ͷ��� */
		if (ret->size >= size)
		{
			void* pRet = (void*)ret->start;

			/* ����һ����ʹ���ڴ����� */
			for(int i=0;i<256;i++)
			{
				if(free_items[i].start == 0)
				{
					list_addtail(&used_list,&free_items[i].node);	// ������ʹ�õĶ���
					free_items[i].start = ret->start;				// �������ʼ��ַ
					free_items[i].size = size;						// ����Ĵ�С
					break;
				}
			}

			/* �޸�ԭ���Ŀ��п�Ĵ�С����ʼ��ַ */
			ret->start += size;
			if((ret->size -= size) == 0)
			{
				list_unlink(&ret->node);
				memset((void*)ret,0,sizeof(FreeItem));
			}
			return pRet;
		}
	}
	while(ret != NULL && ret != (FreeItem*)&free_list);		// ���û�����꣬�ͼ�������

	return NULL;
}


/*
 * ���ں˿ռ��ͷ��ڴ�
 */
void memory_free(void* p)
{
	FreeItem* used = (FreeItem*)&used_list;
	do
	{
		used = (FreeItem*)used->node.next;

		/* ƥ��ɹ� */
		if((void*)used->start == p)
		{
			list_unlink(&used->node);				// �Ƚ������

			/* Ȼ����Ϊһ�����п���� */
			FreeItem* block = (FreeItem*)free_list.next;
			FreeItem* front = (FreeItem*)&free_list;
			do
			{
				/* һ��Ҫ�����ʵ���λ�� */
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
			/* �ϲ����п� */
			memory_merger();
			break;
		}

	}
	while(used != NULL && used != (FreeItem*)&used_list);		// ���û�����꣬�ͼ�������
	return;
}


/*
 *�ϲ����п�
 */
void memory_merger()
{
	/* �ϲ����п� */
	FreeItem* block = (FreeItem*)free_list.next;
	do
	{
		FreeItem* next = (FreeItem*)block->node.next;

		/* �ͺ�һ�����п�ϲ� */
		if (block->start + block->size == next->start)
		{
			/* �ϲ���block�ڵ� */
			block->size += next->size;

			/* ����next�ڵ� */
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

