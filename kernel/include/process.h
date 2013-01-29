/*
 * process.h
 *
 *  Created on: 2012-8-28
 *      Author: greenleaf
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <lib/include/list.h>
#include <lib/include/type.h>
#include "handle.h"
#include "message.h"

typedef struct process
{
	ListNode node;					/* �������½��̵Ľڵ� */
	Handle handle;					/* ���̾�� */
	u16  state;						/* ״̬��Ϣ */
	u16  priority;					/* ���ȼ� */
	u16  ticks;						/* ʱ��Ƭ */
	u32  stack_top;					/* �ں�ջջ�� */
	u32 msg_queue_bmp[THREAD_NR_MSGQUEUE/32];		/* ��Ϣ����λͼ */
	MsgHead msg_queue[THREAD_NR_MSGQUEUE];			/* ��Ϣ���� */
}Process;


#endif /* PROCESS_H_ */
