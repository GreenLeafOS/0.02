/*
 * thread.h
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

#ifndef THREAD_H_
#define THREAD_H_
#include "lib.h"
#include "message.h"


/* ThreadDesc.msg_queue[] */
#define THREAD_NR_MSGQUEUE		512

/* ThreadDesc */
typedef struct kernel_thread_desc
{
	ListNode node;					/* ���������̵߳Ľڵ� */
	id_t id;						/* �߳�ID */
	u16  state;						/* ״̬��Ϣ */
	u16  priority;					/* ���ȼ� */
	u16  ticks;						/* ʱ��Ƭ */
	u32  stack_top;					/* ջ�� */
	u32 msg_queue_bmp[THREAD_NR_MSGQUEUE/32];		/* ��Ϣ����λͼ */
	MsgHead msg_queue[THREAD_NR_MSGQUEUE];			/* ��Ϣ���� */
}ThreadDesc;

/* ThreadDesc.state */
#define THREAD_STATE_RUNNING	1
#define THREAD_STATE_READY		2
#define THREAD_STATE_SLEEPING	4
#define THREAD_STATE_SENDING	8
#define THREAD_STATE_RECVING	16
#define THREAD_STATE_START		32
#define THREAD_STATE_EXIT		64


/* KernelThread */
typedef union thread_union
{
	ThreadDesc thread_info;
	u8 stack[8192];		/* 8k��ջ */
}KernelThread;


/* KernelThread[] */
#define NR_THREAD 32



/* thread function */
void thread_init();
id_t thread_set(KernelThread* thread);
void thread_wait();
void thread_exit();
void thread_schedule();

/* thread data */
extern KernelThread*	thread_table[NR_THREAD];
extern KernelThread*	thread_run;
extern u32*				thread_run_stack_top;
extern ListHead			thread_queue_ready;
extern ListHead			thread_queue_sleep;
extern StackFrame		thread_default_reg;
extern Atomic			thread_resched_lock;
extern KernelThread*	thread_realtime;

/* thread macro */
#define CreateThread(name,thread,regs)	\
		regs.eip = (u32)name##_main;								\
		thread = create();											\
		thread->thread_info.stack_top -= sizeof(regs);				\
		*(StackFrame*)thread->thread_info.stack_top = regs;			\
		name##_id = thread->thread_info.id;							\
		ready(thread)

#define SetRunThread(thread)		\
		thread_run = (KernelThread*)thread;								\
		thread_run->thread_info.state = THREAD_STATE_RUNNING;			\
		thread_run_stack_top =(u32*)&thread_run->thread_info.stack_top

#endif /* THREAD_H_ */
