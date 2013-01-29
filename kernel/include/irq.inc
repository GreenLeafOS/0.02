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
 * 进入内核栈
 */
.macro into_kernel
	/* 装载段寄存器 */
	mov		%ss,%ax
	mov		%ax,%ds
	mov		%ax,%es
	mov		%ax,%fs

	mov		$40,%ax
	mov		%ax,%gs

	/* 进入内核栈 */
	movl	(kernel_stack_top),%eax
	movl	(%eax),%esp
.endm



/*
 * 返回线程
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
 * 保存段寄存器
 */
.macro push_segment
	push	%ds
	push	%es
	push	%fs
	push	%gs	
.endm



/*
 * 恢复段寄存器
 */
.macro pop_segment
	pop		%gs
	pop		%fs
	pop		%es
	pop		%ds
.endm



/*
 * 保存线程栈指针
 */
.macro save_thread_stack
	movl	(thread_run_stack_top),%eax
	movl	%esp,(%eax)
.endm



/*
 * 恢复线程栈指针
 */
.macro release_thread_stack
	movl	(thread_run_stack_top),%eax
	movl	(%eax),%esp
.endm
 
 


/*
 * 保存内核栈指针
 */
.macro save_kernel_stack
	movl	(kernel_stack_top),%eax
	movl	%esp,(%eax)
.endm




/*
 * 主8259A的中断
 */
.macro hwint_master irq_num
	/* 如果没有重入，则保存寄存器在线程的内核态栈 */
	/* 如果重入，则保存在内核栈 */
	/* save函数会压入一个返回地址 */
	/* 如果重入，则是restart_reenter */
	/* 如果没重入，则压入restart */
	/* 压入的返回函数作为由ret返回 */
	call	save

	/* 屏蔽当前中断 */
	inb		$INT_M_CTLMASK,%al
	orb		$(1 << \irq_num),%al
	outb	%al,$INT_M_CTLMASK

	/* 重新打开主8259A */
	movb	$EOI,%al
	outb	%al,$INT_M_CTL

	/* 调用中断处理函数 */
	pushl	$\irq_num				/* irq */
	call	intr_handle				/* 如果没有重入则调用调度函数。中断重入时不允许抢占 */
	popl	%ecx					/* 调用者清理堆栈 */

	/* 恢复响应当前中断 */
	inb		$INT_M_CTLMASK,%al
	andb	$(~(1 << \irq_num)),%al
	outb	%al,$INT_M_CTLMASK

	/* 根据栈顶地址进行跳转 */
	/* 如果中断重入，则返回压入的restart_reenter */
	/* 如果没有中断重入，则返回压入的restart */
	ret
.endm





/*
 * 从8259A的中断
 */
.macro hwint_slave irq_num
	call	save

	inb		$INT_S_CTLMASK,%al
	orb		$(1 << (\irq_num-8)),%al
	outb	%al,$INT_S_CTLMASK		/* 屏蔽当前中断 */

	movb	$EOI,%al
	outb	%al,$INT_M_CTL			/* 重新打开主8259A */
	nop
	outb	%al,$INT_S_CTL			/* 重新打开从8259A */

	sti								/* 开中断 */
	pushl	$\irq_num				/* irq */
	call	intr_handle
	popl	%ecx					/* 调用者清理堆栈 */
	cli								/* 关中断 */

	inb		$INT_M_CTLMASK,%al
	andb	$(~(1 << (\irq_num-8))),%al
	outb	%al,$INT_M_CTLMASK		/* 恢复相应当前中断 */

	ret
.endm






/*
 * 异常处理
 */
.macro exception vec_num
	popl	(handle_err_code)
	call	save
	pushl	(handle_err_code)
	pushl	$\vec_num
	call	exception_handle
	addl	$8,%esp					/* 清理堆栈 */
	ret								/* 返回restart */
.endm
