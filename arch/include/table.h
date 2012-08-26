#ifndef TABLE_H
#define TABLE_H

#include "desc.h"



/*===============================================================
 *                        gdt
 *                	  GDT��ش�����
 ===============================================================*/
/* ��ʼ�� */
void gdt_init();

/* gdt���� */
int gdt_add(Desc item);
void gdt_remove(int index);
void gdt_set(int index,Desc item);
void gdt_get(int index,Desc *item);

/* ����gdt */
void gdt_load();

extern Desc gdt_table[32];
extern u8 gdt_ptr[6];


/*===============================================================
 *                       ldt
 *                	  LDT��ش�����
 ===============================================================*/
/* ��ʼ�� */
void ldt_init();

/* ldt���� */
int ldt_add(Desc item);
void ldt_remove(int index);
void ldt_set(int index,Desc item);
void ldt_get(int index,Desc *item);

/* ����ldt */
void ldt_load();

extern Desc ldt_table[8];
extern u8 ldt_ptr[6];




/*===============================================================
 *                        idt
 *                	  IDT��ش�����
 ===============================================================*/
/* ��ʼ�� */
void idt_init();

/* idt���� */
void idt_set(int index,Gate item);
void idt_get(int index,Gate *item);

/* �ų�ʼ������ */
void idt_init_call(int index,FunAddr handle,u8 dpl);
void idt_init_intr(int index,FunAddr handle,u8 dpl);
void idt_init_trap(int index,FunAddr handle,u8 dpl);
void idt_init_task(int index,FunAddr handle,u8 dpl);

/* ����idt */
void idt_load();

extern Gate idt_table[256];
extern u8 idt_ptr[6];

/*===============================================================
 *                        tss
 *                	  TSS��ش�����
 ===============================================================*/
/* ��ʼ�� */
void tss_init();

/* ����tss */
void tss_load(u16 sel);

extern TSS tss;
extern u16 tss_sel;
#endif
