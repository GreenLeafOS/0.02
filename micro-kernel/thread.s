/*
 * thread.s
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

/*  �����˺ܶ�꣬�����ж��ʹ�� */
.include "irq.inc"


.section	.text
/*
 * SysApi wait
 * ��������
 * ���ܣ��߳�˯���Լ����ȴ�ĳ���¼�������
 * ����ֵ����
 */
.global	wait
wait:
	incl	(kernel_reenter)	/* �����������1 */
	popl	(handle_ret_addr)	/* �������ص�ַ */

	push	%ss					/* ����ջ */
	pushl	%esp
	pushf						/* ����eflags */
	cli							/* ���ж� */
	push	%cs					/* �������ָ�� */
	pushl	(handle_ret_addr)
	pushal						/* ����Ĵ��� */
	push_segment				/* ����μĴ��� */
	save_thread_stack			/* ����ջָ�� */

	into_kernel					/* �����ں� */
	pushl	$restart			/* ѹ��restart��Ϊ˯�ߴ������ķ��ص�ַ */
	jmp		thread_wait			/* ����˯�ߴ������ */




/*
 * SysApi exit
 * ��������
 * ���ܣ��߳��˳�
 * ����ֵ����
 */
 .global exit
exit:
	incl	(kernel_reenter)	/* �����������1 */
	popl	(handle_ret_addr)	/* �������ص�ַ */
	into_kernel					/* �����ں� */
	pushl	$restart			/* ѹ��restart��Ϊ�˳��������ķ��ص�ַ */
	jmp		thread_exit			/* �����˳�������� */
