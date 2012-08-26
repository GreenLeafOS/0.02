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
	ListHead 	head;		// ���Կռ���˫������ͷָ��
	u32 		free_count;	// ʣ��Ŀռ仹�ж���ҳ��
	PageTable	*table;		// ҳ����׵�ַ
}LinearRoom;


typedef struct linear_block
{
	ListNode	node;
	u32 		flags;		// ���Կռ��״̬������
	u32			start;		// ӳ�䵽�����Ե�ַ�ռ����ʼҳ
	LinearRoom	*room;		// ӳ�䵽�����Ե�ַ�ռ�ָ��
	PhyPage		*block;		// ����ҳ��������ָ��
}LinearBlock;


/* linear block bit*/
#define LB_USED			0	// 1��ʾ�Ѿ�ӳ�����


void mod_addr_room_main();
id_t mod_addr_room_id;




/* mod_hd.c */
void mod_hd_main();
id_t mod_hd_id;





#endif /* MODULE_H_ */
