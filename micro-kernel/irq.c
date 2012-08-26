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
 * ������
 * 		irq_num	�жϺ�
 * ���ܣ��жϴ���
 * ����ֵ����Ӧrestart�ĵ�ַ
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
 * ������
 * 		vec_num �쳣������
 * 		err_code ������
 * ���ܣ��쳣����
 * ����ֵ����
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
