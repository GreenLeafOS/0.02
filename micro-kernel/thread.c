/*
 * thread.c
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

#include "include/sysapi.h"
#include "include/main.h"

/* thread data */
KernelThread*	thread_table[NR_THREAD];
KernelThread*	thread_run;
u32* 			thread_run_stack_top;
ListHead		thread_queue_ready;
ListHead		thread_queue_sleep;
Atomic			thread_resched_lock;
KernelThread*	thread_realtime;

StackFrame		thread_default_reg =
{
		GdtGetSel(USER_VIDEO,0),		// gs
		GdtGetSel(KERNEL_DATA,0),		// fs
		GdtGetSel(KERNEL_DATA,0),		// es
		GdtGetSel(KERNEL_DATA,0),		// ds
		0,0,0,0,0,0,0,0,
		0,								// eip
		GdtGetSel(KERNEL_CODE,0),		// cs
		0x1202,							// eflags
		0,								// esp
		GdtGetSel(KERNEL_DATA,0)		// ss
};

/*
 * thread init
 * 参数：无
 * 功能：初始化线程管理数据
 * 返回值：无
 */
void thread_init()
{
	memset(&thread_table[0],0,sizeof(thread_table));
	list_init(&thread_queue_sleep);
	list_init(&thread_queue_ready);

	/* 建立线程 */
	KernelThread* thread = create();
	thread_default_reg.eip = (u32)kernel_main_thread;					// eip
	thread->thread_info.stack_top -= sizeof(thread_default_reg);
	*(StackFrame*)thread->thread_info.stack_top = thread_default_reg;

	thread->thread_info.ticks = 2;						// 时间片为0
	thread->thread_info.priority = 4;					// 没有时间片
	list_addtail(&thread_queue_ready,&thread->thread_info.node);
	SetRunThread(thread_table[0]);						// 设置为运行线程

	thread_realtime = NULL;
	thread_resched_lock.value = True;
}




/*
 * thread set
 * 参数：线程指针
 * 功能：把线程指针写入线程表
 * 返回值：成功返回id,失败返回-1
 */
id_t thread_set(KernelThread* thread)
{
	for(int i=0;i<NR_THREAD;i++)
	{
		/* 无效指针置为NULL */
		if (thread_table[i] == NULL)
		{
			thread_table[i] = thread;
			return i;
		}
	}
	return -1;
}




/*
 * thread wait
 * 参数：无
 * 功能：wait的后续处理
 * 返回值：无
 */
void thread_wait()
{
	/* 从就绪队列移除 */
	list_unlink(&thread_run->thread_info.node);

	/* 加入睡眠队列 */
	list_add(&thread_queue_sleep,&thread_run->thread_info.node);

	/* 调用调度程序 */
	thread_schedule();
}




/*
 * thread exit
 * 参数：无
 * 功能：exit的后续处理
 * 返回值：无
 */
void thread_exit()
{
	id_t id = thread_run->thread_info.id;

	/* 把线程从线程地址表中移除 */
	thread_table[id] = NULL;

	/* 把线程从任何一个队列中移除 */
	list_unlink(&thread_run->thread_info.node);

	/* 设置状态 */
	thread_run->thread_info.state = THREAD_STATE_EXIT;

	/* 发送广播消息，通知各个部件撤销资源*/
	MsgHead msg = {MSG_THREAD_KILL,	// vector
			MSG_PRIORITY_REALTIME,	// priority
			id,						// sender
			0,						// receiver
			NULL,					// body_point
			0};						// body_size
	post(msg);

	/* 调用调度程序 */
	thread_schedule();
}


/*
 * thread schedule
 * 参数：无
 * 功能：调度线程
 * 返回值：无
 */
void thread_schedule()
{
	/* 检查是否允许抢占 */
	if (thread_realtime && thread_resched_lock.value)
	{
		thread_run->thread_info.state = THREAD_STATE_READY;
		SetRunThread(thread_realtime);
		thread_realtime = NULL;
		lock();
		if (thread_run->thread_info.ticks == 0)
			thread_run->thread_info.ticks = (5-thread_run->thread_info.priority)* 4;
		return;
	}

	/* 检查运行线程是否需要切换 */
	if (thread_run->thread_info.state == THREAD_STATE_RUNNING)
	{
		if(thread_run->thread_info.ticks--)
		{
			return;
		}
		else
		{
			thread_run->thread_info.state = THREAD_STATE_READY;
		}
	}


	/* 搜索可执行线程 */
	KernelThread *thread = (KernelThread*)&thread_queue_ready;
	while(thread != NULL)
	{
		thread = (KernelThread*)thread->thread_info.node.next;
		if (thread == (KernelThread*)&thread_queue_ready) break;

		if (thread->thread_info.ticks)
		{
			if (thread->thread_info.id != 0 && thread->thread_info.state == THREAD_STATE_READY)
			{
				/* 如果抢占了控制权的线程执行完毕,则重新允许抢占 */
				if (!(thread_resched_lock.value)) unlock();
				SetRunThread(thread);
				return;
			}
		}
		else
		{
			thread->thread_info.ticks = (5-thread->thread_info.priority)* 2;
		}
	}
	/* 没有搜索到，执行空闲系统线程 */
	SetRunThread(thread_table[0]);
}




/*
 * SysApi create
 * 参数：无
 * 功能：创建线程
 * 返回值：线程指针
 */
KernelThread* create()
{
	KernelLock();
		/* 向内存分配器申请2个页的空间 */
		KernelThread *new_space = (KernelThread*)alloc(2);
		if (new_space == NULL) return NULL;

		/* 把新线程指针写入线程表并获取一个可用id */
		id_t id = thread_set(new_space);
		if (id == -1) return NULL;

		/* 初始化线程描述符 */
		new_space->thread_info.id = id;
		new_space->thread_info.state = THREAD_STATE_START;		// 创建态
		new_space->thread_info.priority = 2;
		new_space->thread_info.stack_top = (u32)&new_space->stack[8192];	// 栈顶
		new_space->thread_info.ticks = (5-new_space->thread_info.priority)* 4;
	KernelUnlock();

	return new_space;
}



/*
 * SysApi ready
 * 参数：线程指针
 * 功能：线程加入调度
 * 返回值：无
 */
void ready(KernelThread* thread)
{
	KernelLock();
		if (thread == NULL) return;

		/* 准备态 */
		thread->thread_info.state = THREAD_STATE_READY;

		/* 加入就绪队列 */
		list_addtail(&thread_queue_ready,&thread->thread_info.node);
	KernelUnlock();
}




/*
 * SysApi sleep
 * 参数：线程指针
 * 功能：睡眠线程
 * 返回值：无
 */
void sleep(KernelThread* thread)
{
	KernelLock();
		if (thread == NULL) return;

		/* 睡眠态 */
		thread->thread_info.state = THREAD_STATE_SLEEPING;

		/* 从就绪队列移除 */
		list_unlink(&thread->thread_info.node);

		/* 加入睡眠队列 */
		list_add(&thread_queue_sleep,&thread->thread_info.node);
	KernelUnlock();
}




/*
 * SysApi wake
 * 参数：线程指针
 * 功能：唤醒线程
 * 返回值：无
 */
void wake(KernelThread* thread)
{
	KernelLock();
		if (thread == NULL) return;

		/* 就绪态 */
		thread->thread_info.state = THREAD_STATE_READY;

		/* 从睡眠队列中移除 */
		list_unlink(&thread->thread_info.node);

		/* 加入就绪队列 */
		list_addtail(&thread_queue_ready,&thread->thread_info.node);
	KernelUnlock();
}




/*
 * SysApi kill
 * 参数：线程指针
 * 功能：杀死线程
 * 返回值：无
 */
void kill(KernelThread* thread)
{
	KernelLock();
		id_t id = thread->thread_info.id;

		/* 把线程从线程地址表中移除 */
		thread_table[id] = NULL;

		/* 把线程从任何一个队列中移除 */
		list_unlink(&thread->thread_info.node);

		/* 发送广播消息，通知各个部件撤销资源*/
		MsgHead msg = {MSG_THREAD_KILL,	// vector
				MSG_PRIORITY_REALTIME,	// priority
				id,						// sender
				0,						// receiver
				NULL,					// body_point
				0};						// body_size
		post(msg);
	KernelUnlock();
}





/*
 * SysApi lock
 * 参数：无
 * 功能：禁止内核抢占
 * 返回值：无
 */
void lock()
{
	atomic_write(&thread_resched_lock,False);
}




/*
 * SysApi unlock
 * 参数：无
 * 功能：允许内核抢占
 * 返回值：无
 */
void unlock()
{
	atomic_write(&thread_resched_lock,True);
}
