/*
 * module.h
 *
 *  Created on: 2012-8-19
 *      Author: greenleaf
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "lib.h"
#include "sysapi.h"

/* mod_pubmsg.c */
void mod_public_msg_main();
id_t mod_public_msg_id;



/* mod_time.c */
typedef struct time
{
	u16	year;
	u8	month;
	u8	day_of_month;
	u8	day_of_week;
	u8	hours;
	u8	minutes;
	u8	seconds;
}Time;

void mod_time_main();
id_t mod_time_id;
Time mod_time_sysclock;



/* mod_addr_room.c */
typedef struct linear_room
{
	ListHead 	head;		// 线性空间块的双向链表头指针
	u32 		free_count;	// 剩余的空间还有多少页框
	PageTable	*table;		// 页表的首地址
}LinearRoom;


typedef struct linear_block
{
	ListNode	node;
	u32 		flags;		// 线性空间块状态、类型
	u32			start;		// 映射到的线性地址空间的起始页
	LinearRoom	*room;		// 映射到的线性地址空间指针
	PhyPage		*block;		// 物理页块描述符指针
}LinearBlock;


/* linear block bit*/
#define LB_USED			0	// 1表示已经映射过了


void mod_addr_room_main();
id_t mod_addr_room_id;




/* mod_hd.c */
void mod_hd_main();
id_t mod_hd_id;





#endif /* MODULE_H_ */
