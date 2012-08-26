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
#define HANDLE_TABLE_BLOCK_SIZE		1			// 2的1次方，2个页,映射65536个表项
#define HANDLE_TABLE_BLOCK_PAGES	2			// 2个页
#define HANDLE_TABLE_BLOCK_ITEMS	(2*PAGE_SIZE*32)	// 一个块的位图项数
#define HANDLE_TABLE_BLCOK_MAX		256


/* handle_bmp */
struct handle_bmp
{
	u32* bmp_base;	// 位图起始地址
	u32 free_item;	// 剩余表项数
	u32 bmp_size;	// 位图大小，位单位
};


#endif /* HANDLE_H_ */
