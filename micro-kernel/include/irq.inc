/*
 * irq.inc
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

.include	"head.inc"

.section 	.text

/* thread.c */
.extern		thread_schedule
.extern		thread_wait
.extern		thread_exit

.extern 	thread_run_stack_top
.extern		thread_run

/* save.s */
.extern		save
.extern		restart

/* irq.c */
.extern		exception_handle
.extern		intr_handle
.extern 	handle_ret_addr
.extern		handle_err_code

/* main.c */
.extern		kernel_reenter
.extern		kernel_stack_top




/*
 * �����ں�ջ
 */
.macro into_kernel
	/* װ�ضμĴ��� */
	mov		%ss,%ax
	mov		%ax,%ds
	mov		%ax,%es
	mov		%ax,%fs

	mov		$40,%ax
	mov		%ax,%gs

	/* �����ں�ջ */
	movl	(kernel_stack_top),%eax
	movl	(%eax),%esp
.endm



/*
 * �����߳�
 */
.macro ret_thread
	movl	(thread_run_stack_top),%eax
	movl	(%eax),%esp

	pop		%gs
	pop		%fs
	pop		%es
	pop		%ds
	popal
.endm


/*
 * ����μĴ���
 */
.macro push_segment
	push	%ds
	push	%es
	push	%fs
	push	%gs	
.endm



/*
 * �ָ��μĴ���
 */
.macro pop_segment
	pop		%gs
	pop		%fs
	pop		%es
	pop		%ds
.endm



/*
 * �����߳�ջָ��
 */
.macro save_thread_stack
	movl	(thread_run_stack_top),%eax
	movl	%esp,(%eax)
.endm



/*
 * �ָ��߳�ջָ��
 */
.macro release_thread_stack
	movl	(thread_run_stack_top),%eax
	movl	(%eax),%esp
.endm
 
 


/*
 * �����ں�ջָ��
 */
.macro save_kernel_stack
	movl	(kernel_stack_top),%eax
	movl	%esp,(%eax)
.endm




/*
 * ��8259A���ж�
 */
.macro hwint_master irq_num
	/* ���û�����룬�򱣴�Ĵ������̵߳��ں�̬ջ */
	/* ������룬�򱣴����ں�ջ */
	/* save������ѹ��һ�����ص�ַ */
	/* ������룬����restart_reenter */
	/* ���û���룬��ѹ��restart */
	/* ѹ��ķ��غ�����Ϊ��ret���� */
	call	save

	/* ���ε�ǰ�ж� */
	inb		$INT_M_CTLMASK,%al
	orb		$(1 << \irq_num),%al
	outb	%al,$INT_M_CTLMASK

	/* ���´���8259A */
	movb	$EOI,%al
	outb	%al,$INT_M_CTL

	/* �����жϴ������� */
	pushl	$\irq_num				/* irq */
	call	intr_handle				/* ���û����������õ��Ⱥ������ж�����ʱ��������ռ */
	popl	%ecx					/* ������������ջ */

	/* �ָ���Ӧ��ǰ�ж� */
	inb		$INT_M_CTLMASK,%al
	andb	$(~(1 << \irq_num)),%al
	outb	%al,$INT_M_CTLMASK

	/* ����ջ����ַ������ת */
	/* ����ж����룬�򷵻�ѹ���restart_reenter */
	/* ���û���ж����룬�򷵻�ѹ���restart */
	ret
.endm





/*
 * ��8259A���ж�
 */
.macro hwint_slave irq_num
	call	save

	inb		$INT_S_CTLMASK,%al
	orb		$(1 << (\irq_num-8)),%al
	outb	%al,$INT_S_CTLMASK		/* ���ε�ǰ�ж� */

	movb	$EOI,%al
	outb	%al,$INT_M_CTL			/* ���´���8259A */
	nop
	outb	%al,$INT_S_CTL			/* ���´򿪴�8259A */

	sti								/* ���ж� */
	pushl	$\irq_num				/* irq */
	call	intr_handle
	popl	%ecx					/* ������������ջ */
	cli								/* ���ж� */

	inb		$INT_M_CTLMASK,%al
	andb	$(~(1 << (\irq_num-8))),%al
	outb	%al,$INT_M_CTLMASK		/* �ָ���Ӧ��ǰ�ж� */

	ret
.endm






/*
 * �쳣����
 */
.macro exception vec_num
	popl	(handle_err_code)
	call	save
	pushl	(handle_err_code)
	pushl	$\vec_num
	call	exception_handle
	addl	$8,%esp					/* ������ջ */
	ret								/* ����restart */
.endm