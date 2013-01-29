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
	ListNode node;					/* 链接上下进程的节点 */
	Handle handle;					/* 进程句柄 */
	u16  state;						/* 状态信息 */
	u16  priority;					/* 优先级 */
	u16  ticks;						/* 时间片 */
	u32  stack_top;					/* 内核栈栈顶 */
	u32 msg_queue_bmp[THREAD_NR_MSGQUEUE/32];		/* 消息队列位图 */
	MsgHead msg_queue[THREAD_NR_MSGQUEUE];			/* 消息队列 */
}Process;


#endif /* PROCESS_H_ */
