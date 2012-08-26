#ifndef DESC_H_
#define DESC_H_
/*===============================================================
 *                      desc.h
 *                		描述符
 ===============================================================*/
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



/************************************************************************/
/*							段描述符
/************************************************************************/
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
void desc_init(Desc * p_desc, u32 base, u32 limit, u16 attribute);
Desc desc_create(u32 base, u32 limit, u16 attribute);

/* 生成选择子 */
u16 gdt_get_sel(int index,u8 rpl);
u16 ldt_get_sel(int index,u8 rpl);
Desc	gdt_table[32];
u8 		gdt_ptr[6];

#define GdtGetSel(index,rpl) (index) << 3 + rpl + 0
#define LdtGetSel(index,rpl) (index) << 3 + rpl + 4

/************************************************************************/
/*							门描述符
/************************************************************************/
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
void gate_init(Gate* p_gate,u32 offset,u32 selector,u8 attr,u8 param_count);
Gate gate_create(u32 offset,u32 selector,u8 attr,u8 param_count);


/************************************************************************/
/*						任务状态段
/************************************************************************/
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

#endif /* DESC_H_ */
