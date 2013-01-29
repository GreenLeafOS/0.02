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
KernelThread*	create();						/* �����߳� */
void			ready(KernelThread* thread);	/* �̳߳�ʼ����ϣ�������� */
void			sleep(KernelThread* thread);	/* ˯���߳� */
void			wake(KernelThread* thread);		/* �����߳� */
void			kill(KernelThread* thread);		/* ɱ���߳� */

void			lock();							/* ��ֹ�ں���ռ */
void			unlock();						/* �����ں���ռ */
/* thread.s */
void			wait();							/* �߳�����˯�ߣ��Եȴ�ĳ���¼� */
void			exit();							/* �߳��˳� */
/* message.c */
void			post(MsgHead msg);				/* ������Ϣ������ȴ��� */
MsgHead			recv();							/* ������Ϣ */
/* phypage.c */
void*			alloc(u32 size);				/* ��������ҳ�� */
void			free(void* addr);				/* �ͷ�����ҳ�� */
/* handle.c */
u32				new();							/* ��������� */
void			delete(u32 handle);				/* �ͷŶ����� */


#endif /* SYSAPI_H_ */
