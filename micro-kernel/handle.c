/*
 * handle.c
 *
 *  Created on: 2012-8-20
 *      Author: greenleaf
 */

#include "include/sysapi.h"
#include "include/handle.h"

/* handle data */
struct handle_bmp handle_table[HANDLE_TABLE_BLCOK_MAX];



/*
 * SysApi new
 * ��������
 * ���ܣ�����һ��������
 * ����ֵ�����id
 */
u32 new()
{
	/* ���ѭ�������ұ� */
	for(int i=0;i<HANDLE_TABLE_BLCOK_MAX;i++)		// -12
	{
		struct handle_bmp *free = &handle_table[i] ;	// -20
		if (free->bmp_size == 0)
		{
			/* Ϊ�ձ�����ڴ� */
			free->bmp_base = (u32*)alloc(HANDLE_TABLE_BLOCK_SIZE);
			bmp_set(free->bmp_base,0);	// ������һλ����������Ч���

			free->bmp_size = HANDLE_TABLE_BLOCK_ITEMS;
			free->free_item = HANDLE_TABLE_BLOCK_ITEMS;
		}
		if (free->free_item == 0) continue;

		/* ���ҿ��о�� */
		int id = bmp_search(free->bmp_base,free->bmp_size);
		if (id == -1)
		{
			return 0;
		}
		else
		{
			free->free_item--;
			bmp_set(free->bmp_base,id);
			return (u32)id;
		}
	}
	return 0;
}





/*
 * SysApi delete
 * ���������ָ��
 * ���ܣ��ͷž��
 * ����ֵ����
 */
void delete(u32 handle)
{
	bmp_clear(handle_table[handle/HANDLE_TABLE_BLOCK_ITEMS].bmp_base,
			handle%HANDLE_TABLE_BLOCK_ITEMS);
	return;
}
