/*
 * handle.h
 *
 *  Created on: 2012-8-20
 *      Author: greenleaf
 */

#ifndef HANDLE_H_
#define HANDLE_H_

#include "lib.h"

/* handle config */
#define HANDLE_TABLE_BLOCK_SIZE		1			// 2��1�η���2��ҳ,ӳ��65536������
#define HANDLE_TABLE_BLOCK_PAGES	2			// 2��ҳ
#define HANDLE_TABLE_BLOCK_ITEMS	(2*PAGE_SIZE*32)	// һ�����λͼ����
#define HANDLE_TABLE_BLCOK_MAX		256


/* handle_bmp */
struct handle_bmp
{
	u32* bmp_base;	// λͼ��ʼ��ַ
	u32 free_item;	// ʣ�������
	u32 bmp_size;	// λͼ��С��λ��λ
};


#endif /* HANDLE_H_ */
