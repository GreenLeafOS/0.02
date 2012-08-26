/*
 * message.h
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include "lib.h"



/* MsgHead */
typedef struct message_head
{
	u16		vector;					/* 消息向量 */
	u16		priority;				/* 消息优先级 */
	id_t	sender;					/* 发送者 */
	id_t	receiver;				/* 接收者 */
	point	body_point;				/* 消息体指针 */
	u16		body_size;				/* 消息体大小 */
	u16		param;					/* 占位 */
}MsgHead;


/* MsgHead.priority */
#define MSG_PRIORITY_REALTIME	0
#define MSG_PRIORITY_RET		1
#define MSG_PRIORITY_KERNEL		2
#define MSG_PRIORITY_DRIVER		3
#define MSG_PRIORITY_USER		4

/* MsgHead.vector */
#include "vector.h"


/* Message data */
extern MsgHead msg_ok;
extern MsgHead msg_max;
extern MsgHead msg_reg;
extern MsgHead msg_fail;

#endif /* MESSAGE_H_ */
