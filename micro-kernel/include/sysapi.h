/*
 * sysapi.h
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

#ifndef SYSAPI_H_
#define SYSAPI_H_

#include "thread.h"
#include "message.h"
#include "phypage.h"
#include "handle.h"

/* thread.c */
KernelThread*	create();						/* 创建线程 */
void			ready(KernelThread* thread);	/* 线程初始化完毕，加入调度 */
void			sleep(KernelThread* thread);	/* 睡眠线程 */
void			wake(KernelThread* thread);		/* 唤醒线程 */
void			kill(KernelThread* thread);		/* 杀死线程 */

void			lock();							/* 禁止内核抢占 */
void			unlock();						/* 允许内核抢占 */
/* thread.s */
void			wait();							/* 线程自我睡眠，以等待某个事件 */
void			exit();							/* 线程退出 */
/* message.c */
void			post(MsgHead msg);				/* 发送消息（无需等待） */
MsgHead			recv();							/* 接收消息 */
/* phypage.c */
void*			alloc(u32 size);				/* 分配物理页框 */
void			free(void* addr);				/* 释放物理页框 */
/* handle.c */
u32				new();							/* 分配对象句柄 */
void			delete(u32 handle);				/* 释放对象句柄 */


#endif /* SYSAPI_H_ */
