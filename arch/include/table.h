#ifndef TABLE_H
#define TABLE_H

#include "desc.h"



/*===============================================================
 *                        gdt
 *                	  GDT相关处理函数
 ===============================================================*/
/* 初始化 */
void gdt_init();

/* gdt操作 */
int gdt_add(Desc item);
void gdt_remove(int index);
void gdt_set(int index,Desc item);
void gdt_get(int index,Desc *item);

/* 加载gdt */
void gdt_load();

extern Desc gdt_table[32];
extern u8 gdt_ptr[6];


/*===============================================================
 *                       ldt
 *                	  LDT相关处理函数
 ===============================================================*/
/* 初始化 */
void ldt_init();

/* ldt操作 */
int ldt_add(Desc item);
void ldt_remove(int index);
void ldt_set(int index,Desc item);
void ldt_get(int index,Desc *item);

/* 加载ldt */
void ldt_load();

extern Desc ldt_table[8];
extern u8 ldt_ptr[6];




/*===============================================================
 *                        idt
 *                	  IDT相关处理函数
 ===============================================================*/
/* 初始化 */
void idt_init();

/* idt操作 */
void idt_set(int index,Gate item);
void idt_get(int index,Gate *item);

/* 门初始化操作 */
void idt_init_call(int index,FunAddr handle,u8 dpl);
void idt_init_intr(int index,FunAddr handle,u8 dpl);
void idt_init_trap(int index,FunAddr handle,u8 dpl);
void idt_init_task(int index,FunAddr handle,u8 dpl);

/* 加载idt */
void idt_load();

extern Gate idt_table[256];
extern u8 idt_ptr[6];

/*===============================================================
 *                        tss
 *                	  TSS相关处理函数
 ===============================================================*/
/* 初始化 */
void tss_init();

/* 加载tss */
void tss_load(u16 sel);

extern TSS tss;
extern u16 tss_sel;
#endif
