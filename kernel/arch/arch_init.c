/*
 * arch_init.c
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */

#include "../include/segment.h"
#include "../include/protect.h"
#include "../include/i8259.h"

void arch_init()
{
	gdt_init();
	idt_init();
	tss_init();
	init_8259A();
}
