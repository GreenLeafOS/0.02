/*
 * thread.s
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

/*  包含了很多宏，代码中多次使用 */
.include "irq.inc"


.section	.text
/*
 * SysApi wait
 * 参数：无
 * 功能：线程睡眠自己，等待某个事件发生。
 * 返回值：无
 */
.global	wait
wait:
	incl	(kernel_reenter)	/* 重入计数器加1 */
	popl	(handle_ret_addr)	/* 弹出返回地址 */

	push	%ss					/* 保存栈 */
	pushl	%esp
	pushf						/* 保存eflags */
	cli							/* 关中断 */
	push	%cs					/* 保存程序指针 */
	pushl	(handle_ret_addr)
	pushal						/* 保存寄存器 */
	push_segment				/* 保存段寄存器 */
	save_thread_stack			/* 保存栈指针 */

	into_kernel					/* 进入内核 */
	pushl	$restart			/* 压入restart作为睡眠处理程序的返回地址 */
	jmp		thread_wait			/* 进入睡眠处理程序 */




/*
 * SysApi exit
 * 参数：无
 * 功能：线程退出
 * 返回值：无
 */
 .global exit
exit:
	incl	(kernel_reenter)	/* 重入计数器加1 */
	popl	(handle_ret_addr)	/* 弹出返回地址 */
	into_kernel					/* 进入内核 */
	pushl	$restart			/* 压入restart作为退出处理程序的返回地址 */
	jmp		thread_exit			/* 进入退出处理程序 */
