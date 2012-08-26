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
 * ��������
 * ���ܣ���ʼ���̹߳�������
 * ����ֵ����
 */
void thread_init()
{
	memset(&thread_table[0],0,sizeof(thread_table));
	list_init(&thread_queue_sleep);
	list_init(&thread_queue_ready);

	/* �����߳� */
	KernelThread* thread = create();
	thread_default_reg.eip = (u32)kernel_main_thread;					// eip
	thread->thread_info.stack_top -= sizeof(thread_default_reg);
	*(StackFrame*)thread->thread_info.stack_top = thread_default_reg;

	thread->thread_info.ticks = 2;						// ʱ��ƬΪ0
	thread->thread_info.priority = 4;					// û��ʱ��Ƭ
	list_addtail(&thread_queue_ready,&thread->thread_info.node);
	SetRunThread(thread_table[0]);						// ����Ϊ�����߳�

	thread_realtime = NULL;
	thread_resched_lock.value = True;
}




/*
 * thread set
 * �������߳�ָ��
 * ���ܣ����߳�ָ��д���̱߳�
 * ����ֵ���ɹ�����id,ʧ�ܷ���-1
 */
id_t thread_set(KernelThread* thread)
{
	for(int i=0;i<NR_THREAD;i++)
	{
		/* ��Чָ����ΪNULL */
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
 * ��������
 * ���ܣ�wait�ĺ�������
 * ����ֵ����
 */
void thread_wait()
{
	/* �Ӿ��������Ƴ� */
	list_unlink(&thread_run->thread_info.node);

	/* ����˯�߶��� */
	list_add(&thread_queue_sleep,&thread_run->thread_info.node);

	/* ���õ��ȳ��� */
	thread_schedule();
}




/*
 * thread exit
 * ��������
 * ���ܣ�exit�ĺ�������
 * ����ֵ����
 */
void thread_exit()
{
	id_t id = thread_run->thread_info.id;

	/* ���̴߳��̵߳�ַ�����Ƴ� */
	thread_table[id] = NULL;

	/* ���̴߳��κ�һ���������Ƴ� */
	list_unlink(&thread_run->thread_info.node);

	/* ����״̬ */
	thread_run->thread_info.state = THREAD_STATE_EXIT;

	/* ���͹㲥��Ϣ��֪ͨ��������������Դ*/
	MsgHead msg = {MSG_THREAD_KILL,	// vector
			MSG_PRIORITY_REALTIME,	// priority
			id,						// sender
			0,						// receiver
			NULL,					// body_point
			0};						// body_size
	post(msg);

	/* ���õ��ȳ��� */
	thread_schedule();
}


/*
 * thread schedule
 * ��������
 * ���ܣ������߳�
 * ����ֵ����
 */
void thread_schedule()
{
	/* ����Ƿ�������ռ */
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

	/* ��������߳��Ƿ���Ҫ�л� */
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


	/* ������ִ���߳� */
	KernelThread *thread = (KernelThread*)&thread_queue_ready;
	while(thread != NULL)
	{
		thread = (KernelThread*)thread->thread_info.node.next;
		if (thread == (KernelThread*)&thread_queue_ready) break;

		if (thread->thread_info.ticks)
		{
			if (thread->thread_info.id != 0 && thread->thread_info.state == THREAD_STATE_READY)
			{
				/* �����ռ�˿���Ȩ���߳�ִ�����,������������ռ */
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
	/* û����������ִ�п���ϵͳ�߳� */
	SetRunThread(thread_table[0]);
}




/*
 * SysApi create
 * ��������
 * ���ܣ������߳�
 * ����ֵ���߳�ָ��
 */
KernelThread* create()
{
	KernelLock();
		/* ���ڴ����������2��ҳ�Ŀռ� */
		KernelThread *new_space = (KernelThread*)alloc(2);
		if (new_space == NULL) return NULL;

		/* �����߳�ָ��д���̱߳���ȡһ������id */
		id_t id = thread_set(new_space);
		if (id == -1) return NULL;

		/* ��ʼ���߳������� */
		new_space->thread_info.id = id;
		new_space->thread_info.state = THREAD_STATE_START;		// ����̬
		new_space->thread_info.priority = 2;
		new_space->thread_info.stack_top = (u32)&new_space->stack[8192];	// ջ��
		new_space->thread_info.ticks = (5-new_space->thread_info.priority)* 4;
	KernelUnlock();

	return new_space;
}



/*
 * SysApi ready
 * �������߳�ָ��
 * ���ܣ��̼߳������
 * ����ֵ����
 */
void ready(KernelThread* thread)
{
	KernelLock();
		if (thread == NULL) return;

		/* ׼��̬ */
		thread->thread_info.state = THREAD_STATE_READY;

		/* ����������� */
		list_addtail(&thread_queue_ready,&thread->thread_info.node);
	KernelUnlock();
}




/*
 * SysApi sleep
 * �������߳�ָ��
 * ���ܣ�˯���߳�
 * ����ֵ����
 */
void sleep(KernelThread* thread)
{
	KernelLock();
		if (thread == NULL) return;

		/* ˯��̬ */
		thread->thread_info.state = THREAD_STATE_SLEEPING;

		/* �Ӿ��������Ƴ� */
		list_unlink(&thread->thread_info.node);

		/* ����˯�߶��� */
		list_add(&thread_queue_sleep,&thread->thread_info.node);
	KernelUnlock();
}




/*
 * SysApi wake
 * �������߳�ָ��
 * ���ܣ������߳�
 * ����ֵ����
 */
void wake(KernelThread* thread)
{
	KernelLock();
		if (thread == NULL) return;

		/* ����̬ */
		thread->thread_info.state = THREAD_STATE_READY;

		/* ��˯�߶������Ƴ� */
		list_unlink(&thread->thread_info.node);

		/* ����������� */
		list_addtail(&thread_queue_ready,&thread->thread_info.node);
	KernelUnlock();
}




/*
 * SysApi kill
 * �������߳�ָ��
 * ���ܣ�ɱ���߳�
 * ����ֵ����
 */
void kill(KernelThread* thread)
{
	KernelLock();
		id_t id = thread->thread_info.id;

		/* ���̴߳��̵߳�ַ�����Ƴ� */
		thread_table[id] = NULL;

		/* ���̴߳��κ�һ���������Ƴ� */
		list_unlink(&thread->thread_info.node);

		/* ���͹㲥��Ϣ��֪ͨ��������������Դ*/
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
 * ��������
 * ���ܣ���ֹ�ں���ռ
 * ����ֵ����
 */
void lock()
{
	atomic_write(&thread_resched_lock,False);
}




/*
 * SysApi unlock
 * ��������
 * ���ܣ������ں���ռ
 * ����ֵ����
 */
void unlock()
{
	atomic_write(&thread_resched_lock,True);
}
