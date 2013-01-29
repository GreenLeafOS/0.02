/*
 * irq.c
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */

#include "../include/irq.h"
#include "../include/protect.h"
#include "../include/message.h"

/* handle data */
int	handle_err_code;
int handle_ret_addr;


/*
 * ��ʼ���ж��쳣
 */
void idt_init()
{
	// ȫ����ʼ�����ж���
/*	idt_init_intr(MSG_EXCP_DE,		divide_error,			0);
	idt_init_intr(MSG_EXCP_DB,		single_step_exception,	0);
	idt_init_intr(MSG_EXCP_NMI,		nmi,					0);
	idt_init_intr(MSG_EXCP_BP,		breakpoint_exception,	0);
	idt_init_intr(MSG_EXCP_OF,		overflow,				0);
	idt_init_intr(MSG_EXCP_BR,		bounds_check,			0);
	idt_init_intr(MSG_EXCP_UD,		inval_opcode,			0);
	idt_init_intr(MSG_EXCP_NM,		copr_not_available,		0);
	idt_init_intr(MSG_EXCP_DF,		double_fault,			0);
	idt_init_intr(MSG_EXCP_9,		copr_seg_overrun,		0);
	idt_init_intr(MSG_EXCP_TS,		inval_tss,				0);
	idt_init_intr(MSG_EXCP_SS,		segment_not_present,	0);
	idt_init_intr(MSG_EXCP_GP,		general_protection,		0);
	idt_init_intr(MSG_EXCP_PF,		page_fault,				0);
	idt_init_intr(MSG_EXCP_MF,		copr_error,				0);

	idt_init_intr(MSG_INTR + 0,		hwint00,				0);
	idt_init_intr(MSG_INTR + 1,		hwint01,				0);
	idt_init_intr(MSG_INTR + 2,		hwint02,				0);
	idt_init_intr(MSG_INTR + 3,		hwint03,				0);
	idt_init_intr(MSG_INTR + 4,		hwint04,				0);
	idt_init_intr(MSG_INTR + 5,		hwint05,				0);
	idt_init_intr(MSG_INTR + 6,		hwint06,				0);
	idt_init_intr(MSG_INTR + 7,		hwint07,				0);
	idt_init_intr(MSG_INTR + 8,		hwint08,				0);
	idt_init_intr(MSG_INTR + 9,		hwint09,				0);
	idt_init_intr(MSG_INTR + 10,	hwint10,				0);
	idt_init_intr(MSG_INTR + 11,	hwint11,				0);
	idt_init_intr(MSG_INTR + 12,	hwint12,				0);
	idt_init_intr(MSG_INTR + 13,	hwint13,				0);
	idt_init_intr(MSG_INTR + 14,	hwint14,				0);
	idt_init_intr(MSG_INTR + 15,	hwint15,				0);

	idt_load();*/
	return;
}



/*
 * handle intr
 * ������
 * 		irq_num	�жϺ�
 * ���ܣ��жϴ���
 * ����ֵ����Ӧrestart�ĵ�ַ
 * */
void intr_handle(int irq_num)
{
/*	MsgHead msg = {MSG_INTR + irq_num,
			msg_priority_table[MSG_INTR + irq_num],
			0,
			mod_public_msg_id,
			NULL,
			0};
	post(msg);
	thread_schedule(); */
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
/*	MsgHead msg = {MSG_EXCP + vec_num,
				msg_priority_table[MSG_EXCP + vec_num],
				0,
				mod_public_msg_id,
				&err_code,
				sizeof(int)};
	post(msg);
	thread_schedule(); */
	return;
}

