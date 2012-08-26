/*
 * 8259A.c
 *
 *  Created on: 2012-8-6
 *      Author: greenleaf
 */


#include "include/8259A.h"
#include "include/lock.h"

/*======================================================================*
                            init_8259A
 *======================================================================*/
void init_8259A()
{
	outb(INT_M_CTL,	0x11);					// Master 8259, ICW1. 00010001
	outb(INT_S_CTL,	0x11);					// Slave  8259, ICW1. 00010001

	outb(INT_M_CTLMASK,	INT_VECTOR_IRQ0);	// Master 8259, ICW2. 0x20 主8259的中断向量
	outb(INT_S_CTLMASK,	INT_VECTOR_IRQ8);	// Slave  8259, ICW2. 0x28 从8259的中断向量

	outb(INT_M_CTLMASK,	0x4);				// Master 8259, ICW3. 00000100	IR2级联从片
	outb(INT_S_CTLMASK,	0x2);				// Slave  8259, ICW3. 0x2       从片连的主片的IR号

	outb(INT_M_CTLMASK,	0x1);				// Master 8259, ICW4. 00000001	8086模式,正常EOI
	outb(INT_S_CTLMASK,	0x1);				// Slave  8259, ICW4. 00000001

	outb(INT_M_CTLMASK,	0xFA);				// Master 8259, OCW1. 11111010
	outb(INT_S_CTLMASK,	0xFF);				// Slave  8259, OCW1. 11111111
}




void disable_irq(int irq)
{
	KernelLock();
		if(irq < 8)
			outb(INT_M_CTLMASK, inb(INT_M_CTLMASK) | (1 << irq));
		else
			outb(INT_S_CTLMASK,inb(INT_S_CTLMASK) | (1 << (irq%8)));
	KernelUnlock();
}





void enable_irq(int irq)
{
	KernelLock();
		if(irq < 8)
			outb(INT_M_CTLMASK, inb(INT_M_CTLMASK) & (~(1 << irq)));
		else
			outb(INT_S_CTLMASK,inb(INT_S_CTLMASK) & (~(1 << (irq%8))));
	KernelUnlock();
}
