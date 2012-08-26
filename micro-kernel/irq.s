/*
 * irq.s
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

/*  包含了很多宏，代码中多次使用 */
.include "irq.inc"


.section	.text
/*
 * handle save
 * 参数：无
 * 功能：中断异常处理的开头，保存寄存器
 * 返回值：无
 * 备注：此函数会压入restart的地址
 */
.global save
save:
	/* 重入计数器初始值为-1,如果没有重入，则加1后为0 */
	incl	(kernel_reenter)		# if (++kernel_reenter != 0)
	jnz		save_reenter			# 		save_reenter();
									# Else
									#		save();
/*
 * save
 * 没有发生中断重入时调用
 * 保存线程上下文
 * 进入内核栈
 * 压入restart作为中断处理程序的返回地址
 */
#save:
	popl	(handle_ret_addr)	/* 弹出返回地址 */

	pushal					/* 保存所有通用寄存器 */
	push_segment			/* 保存段寄存器 */
	save_thread_stack		/* 保存栈指针 */

	into_kernel				/* 进入内核栈 */
	pushl	$restart		/* 压入restart */
	jmp		*(handle_ret_addr)	/* 返回 */


/*
 * save_reenter
 * 发生中断重入时调用
 * 保存内核上下文
 * 压入restart_reenter作为中断处理程序的返回地址
 */
save_reenter:
	popl	(handle_ret_addr)	/* 弹出返回地址 */
	pushal						/* 保存所有通用寄存器 */
	pushl	$restart_reenter	/* 压入restart_reenter */
	jmp		*(handle_ret_addr)	/* 返回 */





/*
 * handle restart
 * 参数：无
 * 功能：返回线程执行
 * 返回值：无
 */
.global restart
restart:
	decl	(kernel_reenter)	/* 重入计数器减一 */
	save_kernel_stack		/* 保存内核esp到tss */
	ret_thread				/* 恢复运行 */
	iret					/* 返回 */

restart_reenter:
	decl	(kernel_reenter)	/* 重入计数器减一 */
	popal					/* 弹出寄存器 */
	iret					/* 返回 */










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
 * irq0 - irq7 主8259A的8个中断
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
 * irq8 - irq15 从8259A的8个中断
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
