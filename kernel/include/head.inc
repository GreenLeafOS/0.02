/*
 * head.s
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */
INT_M_CTL		=	0x20	/* I/O port for interrupt controller         <Master> */
INT_M_CTLMASK	=	0x21	/* setting bits in this port disables ints   <Master> */
INT_S_CTL		=	0xA0	/* I/O port for second interrupt controller  <Slave> */
INT_S_CTLMASK	=	0xA1	/* setting bits in this port disables ints   <Slave> */

EOI				=	0x20


sel_data		=			16
