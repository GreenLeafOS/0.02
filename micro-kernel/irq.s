/*
 * irq.s
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

/*  �����˺ܶ�꣬�����ж��ʹ�� */
.include "irq.inc"


.section	.text
/*
 * handle save
 * ��������
 * ���ܣ��ж��쳣����Ŀ�ͷ������Ĵ���
 * ����ֵ����
 * ��ע���˺�����ѹ��restart�ĵ�ַ
 */
.global save
save:
	/* �����������ʼֵΪ-1,���û�����룬���1��Ϊ0 */
	incl	(kernel_reenter)		# if (++kernel_reenter != 0)
	jnz		save_reenter			# 		save_reenter();
									# Else
									#		save();
/*
 * save
 * û�з����ж�����ʱ����
 * �����߳�������
 * �����ں�ջ
 * ѹ��restart��Ϊ�жϴ������ķ��ص�ַ
 */
#save:
	popl	(handle_ret_addr)	/* �������ص�ַ */

	pushal					/* ��������ͨ�üĴ��� */
	push_segment			/* ����μĴ��� */
	save_thread_stack		/* ����ջָ�� */

	into_kernel				/* �����ں�ջ */
	pushl	$restart		/* ѹ��restart */
	jmp		*(handle_ret_addr)	/* ���� */


/*
 * save_reenter
 * �����ж�����ʱ����
 * �����ں�������
 * ѹ��restart_reenter��Ϊ�жϴ������ķ��ص�ַ
 */
save_reenter:
	popl	(handle_ret_addr)	/* �������ص�ַ */
	pushal						/* ��������ͨ�üĴ��� */
	pushl	$restart_reenter	/* ѹ��restart_reenter */
	jmp		*(handle_ret_addr)	/* ���� */





/*
 * handle restart
 * ��������
 * ���ܣ������߳�ִ��
 * ����ֵ����
 */
.global restart
restart:
	decl	(kernel_reenter)	/* �����������һ */
	save_kernel_stack		/* �����ں�esp��tss */
	ret_thread				/* �ָ����� */
	iret					/* ���� */

restart_reenter:
	decl	(kernel_reenter)	/* �����������һ */
	popal					/* �����Ĵ��� */
	iret					/* ���� */










.global		hwint00
.global		hwint01
.global		hwint02
.global		hwint03
.global		hwint04
.global		hwint05
.global		hwint06
.global		hwint07
.global		hwint08
.global		hwint09
.global		hwint10
.global		hwint11
.global		hwint12
.global		hwint13
.global		hwint14
.global		hwint15


/*
 * irq0 - irq7 ��8259A��8���ж�
 */
.align	16
hwint00:		/* Interrupt routine for irq 0 (the clock). */
	hwint_master	0

.align	16
hwint01:		/* Interrupt routine for irq 1 (keyboard) */
	hwint_master	1

.align	16
hwint02:		/* Interrupt routine for irq 2 (cascade!) */
	hwint_master	2

.align	16
hwint03:		/* Interrupt routine for irq 3 (second serial) */
	hwint_master	3

.align	16
hwint04:		/* Interrupt routine for irq 4 (first serial) */
	hwint_master	4

.align	16
hwint05:		/* Interrupt routine for irq 5 (XT winchester) */
	hwint_master	5

.align	16
hwint06:		/* Interrupt routine for irq 6 (floppy) */
	hwint_master	6

.align	16
hwint07:		/* Interrupt routine for irq 7 (printer) */
	hwint_master	7



/*
 * irq8 - irq15 ��8259A��8���ж�
 */
.align	16
hwint08:		/* Interrupt routine for irq 8 (realtime clock) */
	hwint_slave		8

.align	16
hwint09:		/* Interrupt routine for irq 9 (irq 2 redirected) */
	hwint_slave		9

.align	16
hwint10:		/* Interrupt routine for irq 10 */
	hwint_slave		10

.align	16
hwint11:		/* Interrupt routine for irq 11 */
	hwint_slave		11

.align	16
hwint12:		/* Interrupt routine for irq 12 */
	hwint_slave		12

.align	16
hwint13:		/* Interrupt routine for irq 13 (FPU exception) */
	hwint_slave		13

.align	16
hwint14:		/* Interrupt routine for irq 14 (AT winchester) */
	hwint_slave		14

.align	16
hwint15:		/* Interrupt routine for irq 15 */
	hwint_slave		15




.global	divide_error
.global	single_step_exception
.global	nmi
.global	breakpoint_exception
.global	overflow
.global	bounds_check
.global	inval_opcode
.global	copr_not_available
.global	double_fault
.global	copr_seg_overrun
.global	inval_tss
.global	segment_not_present
.global	stack_exception
.global	general_protection
.global	page_fault
.global	copr_error


divide_error:
	pushl	$0xFFFFFFFF
	exception 0
single_step_exception:
	pushl	$0xFFFFFFFF
	exception 1
nmi:
	pushl	$0xFFFFFFFF
	exception 2
breakpoint_exception:
	pushl	$0xFFFFFFFF
	exception 3
overflow:
	pushl	$0xFFFFFFFF
	exception 4
bounds_check:
	pushl	$0xFFFFFFFF
	exception 5
inval_opcode:
	pushl	$0xFFFFFFFF
	exception 6
copr_not_available:
	pushl	$0xFFFFFFFF
	exception 7
double_fault:
	exception 8
copr_seg_overrun:
	pushl	$0xFFFFFFFF
	exception 9
inval_tss:
	exception 10
segment_not_present:
	exception 11
stack_exception:
	exception 12
general_protection:
	exception 13
page_fault:
	exception 14
copr_error:
	pushl	$0xFFFFFFFF
	exception 16
