/*
 * protect.h
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */

#ifndef PROTECT_H_
#define PROTECT_H_

#include <lib/include/type.h>

/* 描述符类型值说明 */
#define	DA_32			0x4000	/* 32 位段				*/
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节	*/
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/
/* 存储段描述符类型值说明 */
#define	DA_DR			0x90	/* 存在的只读数据段类型值		*/
#define	DA_DRW			0x92	/* 存在的可读写数据段属性值		*/
#define	DA_DRWA			0x93	/* 存在的已访问可读写数据段类型值	*/
#define	DA_C			0x98	/* 存在的只执行代码段属性值		*/
#define	DA_CR			0x9A	/* 存在的可执行可读代码段属性值		*/
#define	DA_CCO			0x9C	/* 存在的只执行一致代码段属性值		*/
#define	DA_CCOR			0x9E	/* 存在的可执行可读一致代码段属性值	*/
/* 系统段描述符类型值说明 */
#define	DA_LDT			0x82	/* 局部描述符表段类型值			*/
#define	DA_TaskGate		0x85	/* 任务门类型值				*/
#define	DA_386TSS		0x89	/* 可用 386 任务状态段类型值	*/
#define	DA_386CGate		0x8C	/* 386 调用门类型值			*/
#define	DA_386IGate		0x8E	/* 386 中断门类型值			*/
#define	DA_386TGate		0x8F	/* 386 陷阱门类型值			*/




/* struct s_desc_bit */
typedef struct s_desc_bit	/* 共 8 个字节 */
{
    u16 limit_low		: 16;			/* 段长低16位 Bit0~Bit15: 2个字节宽 */
    u16 base_low		: 16;			/* 段基址低16位 Bit16~Bit31： 16位宽 */
    u16 base_mid		: 8;			/* 段基址31~39位 Bit31~Bit39： 8位宽 */
    u16 type			: 4;			/* 段类型 Bit40~Bit43: 4位表示段的类型 */
    u16 s				: 1;			/* 系统段 Bit44: 0 表示系统描述符 1 表示段描述符 */
    u16 dpl				: 2;			/* 描述符权限 Bit45~Bit46: 访问段所需的权限 */
    u16 p				: 1;			/* Preset 位 Bit47: 表示是否在不在主存中 */
    u16 limit_high 		: 4;			/* 段长高4位 Bit48~Bit51: 段长共20位 */
    u16 avl				: 1;			/* AVL位 Bit52: 这位被软件使用，处理器不使用 */
    u16					: 1;			/* Bit53: 此位保留不被使用 */
    u16 d				: 1;			/* D位 Bit54：缺省操作数位指示缺省的操作数大小 1 表示缺省为32位 0 表示缺省为16位 */
    u16 g				: 1;			/* G位 Bit55：粒度位,0 表示粒度为字节 1 表示粒度为 4K 字节*/
    u16 base_high		: 8;			/* 段基址高8位 Bit56~Bit63: 共32位段基址 */
}Desc;

/* 段描述符的操作 */
Desc desc_create(u32 base, u32 limit, u16 attribute);

/* 生成选择子 */
#define GdtGetSel(index,rpl) (index) << 3 + rpl + 0
#define LdtGetSel(index,rpl) (index) << 3 + rpl + 4



/* struct s_gate */
typedef struct s_gate
{
	u16	offset_low;			/* 偏移低16位 Bit0~Bit15: 2个字节宽 */
	u16	selector;			/* 选择子 Bit16~Bit31 2个字节宽 */
	u8	param_count	: 5;	/* 调用门中参数个数 Bit32~Bit39 1个字节宽 */
	u8				: 3;
	u8	type		: 4;	/* 段类型 Bit40~Bit43: 4位表示段的类型 */
	u8	S			: 1;	/* 系统段 Bit44: 0 表示系统描述符 1 表示段描述符 */
	u8	DPL			: 2;	/* 描述符权限 Bit45~Bit46: 访问段所需的权限 */
	u8	P			: 1;	/* Preset 位 Bit47: 表示是否在不在主存中 */
	u16	offset_high;		/* 偏移高16位 Bit0~Bit15: 2个字节宽 */
}Gate;

/* 门描述符的操作 */
Gate gate_create(u32 offset,u32 selector,u8 attr,u8 param_count);


/* struct s_tss */
typedef struct s_tss {
	u32	backlink;
	u32	esp0;		/* stack pointer to use during interrupt */
	u32	ss0;		/*   "   segment  "  "    "        "     */
	u32	esp1;
	u32	ss1;
	u32	esp2;
	u32	ss2;
	u32	cr3;
	u32	eip;
	u32	flags;
	u32	eax;
	u32	ecx;
	u32	edx;
	u32	ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u32	es;
	u32	cs;
	u32	ss;
	u32	ds;
	u32	fs;
	u32	gs;
	u32	ldt;
	u16	trap;
	u16	iobase;		/* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
	u8	iomap[0];
	u16	end;
}TSS;

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

Desc gdt_table[32];
u8 gdt_ptr[6];

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

Gate idt_table[256];
u8 idt_ptr[6];

/*===============================================================
 *                        tss
 *                	  TSS相关处理函数
 ===============================================================*/
/* 初始化 */
void tss_init();

/* 加载tss */
void tss_load(u16 sel);

TSS tss;
u16 tss_sel;

/*
 ; 分页机制常量
*/
#define PG_P 	1	/* 页存在属性位 */
#define PG_RWR	0	/* R/W 属性位值, 读/执行 */
#define PG_RWW	2	/* R/W 属性位值, 读/写/执行 */
#define PG_USS	0	/* U/S 属性位值, 系统级 */
#define PG_USU	4	/* U/S 属性位值, 用户级 */

#endif /* PROTECT_H_ */
