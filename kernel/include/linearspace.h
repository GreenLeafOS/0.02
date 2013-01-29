/*
 * linearspace.h
 *
 *  Created on: 2012-8-28
 *      Author: greenleaf
 */

#ifndef LINEARSPACE_H_
#define LINEARSPACE_H_

#include "../include/phypage.h"


/* ҳ��ҳĿ¼��ṹ */
typedef struct page_entry
{
	u32 p			: 1;			/* Pλ Bit0 */
	u32 rw			: 1;			/* R/Wλ Bit1*/
	u32 us			: 1;			/* U/Sλ Bit2*/
	u32	pwt			: 1;			/* �������λ Bit3*/
	u32 pcd			: 1;			/* �������λ Bit4*/
	u32 a			: 1;			/* Aλ Bit5*/
	u32 d			: 1;			/* Dλ Bit6*/
	u32	ps			: 1;			/* ҳ��С Bit7*/
	u32 g			: 1;			/* ָʾȫ��ҳ Bit8*/
	u32 avl			: 3;			/* ϵͳʹ�ñ���λ Bit9~Bit11 */
	u32 addr		: 20;			/* �����ַ��20λ Bit12~Bit31*/
}PageEntry;


#define PDE_ALLOCATED		1		/* �Ѿ����� */

/*ҳ��ҳĿ¼�ṹ*/
typedef struct page_table
{
	PageEntry items[PAGE_SIZE/sizeof(PageEntry)];
}PageTable;


typedef struct linear_space
{
	ListHead 	head;		// ���Կռ���˫������ͷָ��
	u32 		free_count;	// ʣ��Ŀռ仹�ж���ҳ��
	PageTable	*table;		// ҳ����׵�ַ
}LinearSpace;


typedef struct linear_block
{
	ListNode	node;
	u32 		flags;		// ���Կռ��״̬������
	u32			start;		// ӳ�䵽�����Ե�ַ�ռ����ʼҳ
	LinearSpace	*room;		// ӳ�䵽�����Ե�ַ�ռ�ָ��
	PhyPage		*block;		// ����ҳ��������ָ��
}LinearBlock;


/* linear block bit*/
#define LB_USED			0	// 1��ʾ�Ѿ�ӳ�����

LinearBlock		kernel_space;

#endif /* LINEARSPACE_H_ */
