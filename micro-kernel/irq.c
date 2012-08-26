/*
 * irq.c
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

#include "include/sysapi.h"
#include "include/irq.h"
#include "include/module.h"

/* handle data */
int	handle_err_code;
int handle_ret_addr;




/*
 * handle intr
 * 参数：
 * 		irq_num	中断号
 * 功能：中断处理
 * 返回值：对应restart的地址
 * */
void intr_handle(int irq_num)
{
	MsgHead msg = {MSG_INTR + irq_num,
			msg_priority_table[MSG_INTR + irq_num],
			0,
			mod_public_msg_id,
			NULL,
			0};
	post(msg);
	thread_schedule();
	return;
}




/*
 * handle exception
 * 参数：
 * 		vec_num 异常向量号
 * 		err_code 错误码
 * 功能：异常处理
 * 返回值：无
 * */
void exception_handle(int vec_num, int err_code)
{
	MsgHead msg = {MSG_EXCP + vec_num,
				msg_priority_table[MSG_EXCP + vec_num],
				0,
				mod_public_msg_id,
				&err_code,
				sizeof(int)};
	post(msg);
	thread_schedule();
	return;
}
