/*
 * 8259A.h
 *
 *  Created on: 2012-8-6
 *      Author: greenleaf
 */

#ifndef _8259A_H_
#define _8259A_H_

#include "io.h"

/* 8259A interrupt controller ports. */
#define	INT_M_CTL		0x20	/* I/O port for interrupt controller         <Master> */
#define	INT_M_CTLMASK	0x21	/* setting bits in this port disables ints   <Master> */
#define	INT_S_CTL		0xA0	/* I/O port for second interrupt controller  <Slave>  */
#define	INT_S_CTLMASK	0xA1	/* setting bits in this port disables ints   <Slave>  */

/* interrupt vector */
#define INT_VECTOR_IRQ0		0x20
#define INT_VECTOR_IRQ8		0x28


void 	init_8259A();
void	enable_irq(int irq);
void	disable_irq(int irq);

#endif /* 8259A_H_ */
