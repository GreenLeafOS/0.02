/*
 * irq.h
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

#ifndef IRQ_H_
#define IRQ_H_

/* handle function */
void	handle_intr(int irq_num);
void	handler_exception(int vec_num, int err_code);
void 	idt_init();
void 	save();
void	restart();


void	divide_error();
void	single_step_exception();
void	nmi();
void	breakpoint_exception();
void	overflow();
void	bounds_check();
void	inval_opcode();
void	copr_not_available();
void	double_fault();
void	copr_seg_overrun();
void	inval_tss();
void	segment_not_present();
void	stack_exception();
void	general_protection();
void	page_fault();
void	copr_error();

void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();



/* handle data */
extern int handle_err_code;
extern int handle_ret_addr;


#endif /* HANDLE_H_ */
