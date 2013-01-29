/*
 * tss.c
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */


#include "../include/protect.h"
#include "../include/segment.h"


/*
 * ≥ı ºªØtss
 */
void tss_init()
{
	int i = gdt_add(desc_create((u32)&tss,sizeof(tss),DA_386TSS));
	gdt_load();
	tss.iobase = (u16)(u32)&tss.iomap;
	tss.end = 0xff;
	tss.ss0 = GdtGetSel(KERNEL_DATA,0);
	tss.esp0 = 0x80000;

	tss_sel = GdtGetSel(i,0);
	tss_load(tss_sel);
}
