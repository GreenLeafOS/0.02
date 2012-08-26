/*
 * vector.h
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

#ifndef MSG_H_
#define MSG_H_

/* 起始向量 */
#define		MSG_EXCP		0
#define		MSG_INTR		32
#define		MSG_THREAD		MSG_INTR + 16
/* 异常 */
#define		MSG_EXCP_DE					MSG_EXCP + 0
#define		MSG_EXCP_DB					MSG_EXCP + 1
#define		MSG_EXCP_NMI				MSG_EXCP + 2
#define		MSG_EXCP_BP					MSG_EXCP + 3
#define		MSG_EXCP_OF					MSG_EXCP + 4
#define		MSG_EXCP_BR					MSG_EXCP + 5
#define		MSG_EXCP_UD					MSG_EXCP + 6
#define		MSG_EXCP_NM					MSG_EXCP + 7
#define		MSG_EXCP_DF					MSG_EXCP + 8
#define		MSG_EXCP_9					MSG_EXCP + 9
#define		MSG_EXCP_TS					MSG_EXCP + 10
#define		MSG_EXCP_NP					MSG_EXCP + 11
#define		MSG_EXCP_SS					MSG_EXCP + 12
#define		MSG_EXCP_GP					MSG_EXCP + 13
#define		MSG_EXCP_PF					MSG_EXCP + 14
#define		MSG_EXCP_15					MSG_EXCP + 15
#define		MSG_EXCP_MF					MSG_EXCP + 16
#define		MSG_EXCP_AC					MSG_EXCP + 17
#define		MSG_EXCP_MC					MSG_EXCP + 18
#define		MSG_EXCP_XF					MSG_EXCP + 19

/* 中断 */
#define 	MSG_INTR_CLOCK				MSG_INTR + 0
#define		MSG_INTR_KEYBOARD			MSG_INTR + 1
#define		MSG_INTR_CASCADE			MSG_INTR + 2
#define		MSG_INTR_SERIAL2			MSG_INTR + 3
#define		MSG_INTR_SERIAL1			MSG_INTR + 4
#define		MSG_INTR_LPT2				MSG_INTR + 5
#define		MSG_INTR_FLOPPY				MSG_INTR + 6
#define		MSG_INTR_LPT1				MSG_INTR + 7
#define		MSG_INTR_REALTIME_CLOCK		MSG_INTR + 8
#define		MSG_INTR_IRQ2_REDIRECTED	MSG_INTR + 9
#define		MSG_INTR_IRQ10				MSG_INTR + 10
#define		MSG_INTR_IRQ11				MSG_INTR + 11
#define		MSG_INTR_MOUSE				MSG_INTR + 12
#define		MSG_INTR_FPU				MSG_INTR + 13
#define		MSG_INTR_AT_WINCHESTER		MSG_INTR + 14
#define		MSG_INTR_IRQ15				MSG_INTR + 15

/* 线程消息 */
#define		MSG_THREAD_KILL				MSG_THREAD + 0					// 线程被杀死
#define		MSG_THREAD_REGPUB			MSG_THREAD + 1					// 线程注册广播消息

#define		MSG_RET_MAX					MSG_THREAD + 2					// 表项已满
#define		MSG_RET_OK					MSG_THREAD + 3					// 执行成功
#define		MSG_RET_FAIL				MSG_THREAD + 4					// 失败

#define		MSG_REQUEST_CREATE			MSG_THREAD + 5					// 创建资源

#define		MSG_ADDRROOM_ADD			MSG_THREAD + 6					// 添加线性区对象
#define		MSG_ADDRROOM_DEL			MSG_THREAD + 7					// 删除线性区对象
#define		MSG_ADDRROOM_SWITCH			MSG_THREAD + 8					// 切换线性区

#define		MSG_ADDRROOM_CREATE_ROOM	0
#define		MSG_ADDRROOM_CREATE_BLOCK	1

struct		_body_create
{
	u32		type;
	Handle*	ret;
	void*	init;
};

struct		_body_addr_room_add_del
{
	Handle	room,block;
};

struct		_body_addr_room_switch
{
	Handle	room;
};

/* 优先级 */
#define		MSG_PRIORITY(msg,value) value,

extern const int msg_priority_table[48];

extern MsgHead msg_ok;
extern MsgHead msg_max;
extern MsgHead msg_reg;

#endif /* MSG_H_ */

