#ifndef DESC_H_
#define DESC_H_
/*===============================================================
 *                      desc.h
 *                		������
 ===============================================================*/
#include <lib/include/type.h>

/* ����������ֵ˵�� */
#define	DA_32			0x4000	/* 32 λ��				*/
#define	DA_LIMIT_4K		0x8000	/* �ν�������Ϊ 4K �ֽ�	*/
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/
/* �洢������������ֵ˵�� */
#define	DA_DR			0x90	/* ���ڵ�ֻ�����ݶ�����ֵ		*/
#define	DA_DRW			0x92	/* ���ڵĿɶ�д���ݶ�����ֵ		*/
#define	DA_DRWA			0x93	/* ���ڵ��ѷ��ʿɶ�д���ݶ�����ֵ	*/
#define	DA_C			0x98	/* ���ڵ�ִֻ�д��������ֵ		*/
#define	DA_CR			0x9A	/* ���ڵĿ�ִ�пɶ����������ֵ		*/
#define	DA_CCO			0x9C	/* ���ڵ�ִֻ��һ�´��������ֵ		*/
#define	DA_CCOR			0x9E	/* ���ڵĿ�ִ�пɶ�һ�´��������ֵ	*/
/* ϵͳ������������ֵ˵�� */
#define	DA_LDT			0x82	/* �ֲ��������������ֵ			*/
#define	DA_TaskGate		0x85	/* ����������ֵ				*/
#define	DA_386TSS		0x89	/* ���� 386 ����״̬������ֵ	*/
#define	DA_386CGate		0x8C	/* 386 ����������ֵ			*/
#define	DA_386IGate		0x8E	/* 386 �ж�������ֵ			*/
#define	DA_386TGate		0x8F	/* 386 ����������ֵ			*/



/************************************************************************/
/*							��������
/************************************************************************/
typedef struct s_desc_bit	/* �� 8 ���ֽ� */
{
    u16 limit_low		: 16;			/* �γ���16λ Bit0~Bit15: 2���ֽڿ� */
    u16 base_low		: 16;			/* �λ�ַ��16λ Bit16~Bit31�� 16λ�� */
    u16 base_mid		: 8;			/* �λ�ַ31~39λ Bit31~Bit39�� 8λ�� */
    u16 type			: 4;			/* ������ Bit40~Bit43: 4λ��ʾ�ε����� */
    u16 s				: 1;			/* ϵͳ�� Bit44: 0 ��ʾϵͳ������ 1 ��ʾ�������� */
    u16 dpl				: 2;			/* ������Ȩ�� Bit45~Bit46: ���ʶ������Ȩ�� */
    u16 p				: 1;			/* Preset λ Bit47: ��ʾ�Ƿ��ڲ��������� */
    u16 limit_high 		: 4;			/* �γ���4λ Bit48~Bit51: �γ���20λ */
    u16 avl				: 1;			/* AVLλ Bit52: ��λ�����ʹ�ã���������ʹ�� */
    u16					: 1;			/* Bit53: ��λ��������ʹ�� */
    u16 d				: 1;			/* Dλ Bit54��ȱʡ������λָʾȱʡ�Ĳ�������С 1 ��ʾȱʡΪ32λ 0 ��ʾȱʡΪ16λ */
    u16 g				: 1;			/* Gλ Bit55������λ,0 ��ʾ����Ϊ�ֽ� 1 ��ʾ����Ϊ 4K �ֽ�*/
    u16 base_high		: 8;			/* �λ�ַ��8λ Bit56~Bit63: ��32λ�λ�ַ */
}Desc;

/* ���������Ĳ��� */
void desc_init(Desc * p_desc, u32 base, u32 limit, u16 attribute);
Desc desc_create(u32 base, u32 limit, u16 attribute);

/* ����ѡ���� */
u16 gdt_get_sel(int index,u8 rpl);
u16 ldt_get_sel(int index,u8 rpl);
Desc	gdt_table[32];
u8 		gdt_ptr[6];

#define GdtGetSel(index,rpl) (index) << 3 + rpl + 0
#define LdtGetSel(index,rpl) (index) << 3 + rpl + 4

/************************************************************************/
/*							��������
/************************************************************************/
typedef struct s_gate
{
	u16	offset_low;			/* ƫ�Ƶ�16λ Bit0~Bit15: 2���ֽڿ� */
	u16	selector;			/* ѡ���� Bit16~Bit31 2���ֽڿ� */
	u8	param_count	: 5;	/* �������в������� Bit32~Bit39 1���ֽڿ� */
	u8				: 3;
	u8	type		: 4;	/* ������ Bit40~Bit43: 4λ��ʾ�ε����� */
	u8	S			: 1;	/* ϵͳ�� Bit44: 0 ��ʾϵͳ������ 1 ��ʾ�������� */
	u8	DPL			: 2;	/* ������Ȩ�� Bit45~Bit46: ���ʶ������Ȩ�� */
	u8	P			: 1;	/* Preset λ Bit47: ��ʾ�Ƿ��ڲ��������� */
	u16	offset_high;		/* ƫ�Ƹ�16λ Bit0~Bit15: 2���ֽڿ� */
}Gate;

/* ���������Ĳ��� */
void gate_init(Gate* p_gate,u32 offset,u32 selector,u8 attr,u8 param_count);
Gate gate_create(u32 offset,u32 selector,u8 attr,u8 param_count);


/************************************************************************/
/*						����״̬��
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
	u16	iobase;		/* I/Oλͼ��ַ���ڻ����TSS�ν��ޣ��ͱ�ʾû��I/O���λͼ */
	u8	iomap[0];
	u16	end;
}TSS;

#endif /* DESC_H_ */
