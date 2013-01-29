/*
 * loader.s
 * ���Loading...
 * �����̣������ں�
 * ��ȡBIOS����
 * ���뱣��ģʽ
 * ��ʼ���μĴ���
 * ���In protected mode now!
 * �����ں�
 * ������ҳ����
 * �����ں�
 */


 .code16
.global	_start
_start:
	jmp		real_start


/* ���� */
.include "config.inc"
.include "protect.inc"

/* �ļ�ϵͳ */
.section fs,"",@nobits
.include "fs.s"

/* ���̶�ȡ */
.section .text
.include "disk.inc"
/************************************************************************/
/*						��ں���
/*						real_start
/************************************************************************/
.code16
real_start:
	/* ��ʼ�� */
	mov		$0, %ax
	mov		%ax, %ds
	mov		%ax, %es
	mov		%ax, %ss
	mov		$0x7000,%sp

	/* ����ַ��� */
	call	disp_str

	/* �����ں� */
	call	check_disk
	call	read_kernel

	/* ��ȡ�ڴ����� */
	call	get_mem_info
	/* ��ȡCMOS */
	call	get_cmos

	/* ���뱣��ģʽ */
	jmp		jmp_pm

/************************************************************************/
/*						��ȡ�ڴ���Ϣ
/*					    get_mem_info
/*	���� es:di ��
/************************************************************************/
get_mem_info:
	mov		$Param_Seg,%ax
	mov		%ax,%es
	mov		$Bios_MCR,%di

	movl	$0,%ebx
_get_mem_info_loop:
	mov		$0xE820,%eax
	mov		$20,%ecx
	mov		$0x534D4150,%edx
	int		$0x15
	jc		_get_mem_info_fail
	add		$20,%di
	incl	%es:(Bios_MCRNumber)
	cmp		$0,%ebx
	jne		_get_mem_info_loop

	ret
_get_mem_info_fail:
	movl	$0,%es:(Bios_MCRNumber)
	ret

/************************************************************************/
/*						��ȡCMOS��Ϣ
/*					    get_cmos
/*	���� es:di ��
/************************************************************************/
get_cmos:
	mov		$Param_Seg,%ax
	mov		%ax,%es
	mov		$Bios_CMOS,%di		/* ƫ�� */
	mov		$256,%cx			/* ѭ������ */

	xor		%ax,%ax
_get_coms_loop:
	mov		%ah,%al			/* ��CMOS�ڲ���ַ */
	out		%al,$0x70
	nop
	in		$0x71,%al		/* ������ */
	mov		%al,%es:(%di)	/* �������� */
	inc		%ah				/* ����CMOS�ڲ���ַ */
	inc		%di				/* �����ڴ�ƫ�� */
	loop	_get_coms_loop

	ret
/************************************************************************/
/*                      ����Ļ����ʾ�ַ���
/*                         disp_str
/************************************************************************/
disp_str:
	mov		$loader_msg,%si
	mov		$160,%di
	mov		$0x0f, %ah		/* �ڵװ��� */
_disp_str_loop:
	lodsb					/* (C)al=*si;si++; */
	test	%al, %al		/* �����Ƿ�Ϊ0 */
	jz		_disp_str_end	/* Ϊ0����� */
	mov		%ax, %gs:(%di)	/* д�� */
	add		$2,  %di		/* di+2*/
	jmp		_disp_str_loop	/* ������һ��ѭ�� */
_disp_str_end:
	ret
loader_msg:
	.asciz	"Loading..."
/************************************************************************/
/*                      ��ȡkernel
/*                     read_kernel
/************************************************************************/
read_kernel:
	movl	(start_sector),%eax
	movl	(kernel_start),%ebx
	addl	%ebx,%eax

	/*			lba		count			seg						offset 				*/
	read_disk	%eax,	(kernel_size),	$Kernel_FileBaseAddr,	$Kernel_FileOffsetAddr
	ret
/************************************************************************/
/*                      GDT����������
/*                         gdt
/************************************************************************/
.code16
gdt:
/*��ǩ				����			�λ�ַ			�ν���		����							*/
gdt_null:			Descriptor	0,				0,			0
gdt_desc_code:		Descriptor	0,				0xfffff,	DA_CR  | DA_32 | DA_LIMIT_4K
gdt_desc_data:		Descriptor	0,				0xfffff,	DA_DRW | DA_32 | DA_LIMIT_4K
gdt_desc_kcode:		Descriptor	KernelVAOffset,	0xfffff,	DA_CR  | DA_32 | DA_LIMIT_4K
gdt_desc_video:		Descriptor	0xb8000,		0x0ffff,	DA_DRW | DA_DPL3

/* gdtptr */
gdt_len		=	gdt_ptr - gdt
gdt_ptr:
	.word	gdt_len - 1		/* �ν��� */
	.word	gdt				/* ����ַ��λ */
	.word	0				/* ��λһ��Ϊ0 */

/* gdtѡ���� */
sel_code		=	gdt_desc_code	- gdt
sel_data		=	gdt_desc_data	- gdt
sel_kcode		=	gdt_desc_kcode	- gdt
sel_video		=	gdt_desc_video	- gdt + SA_RPL3
/************************************************************************/
/*                     	���뱣��ģʽ
/*                       jmp_pm
/************************************************************************/
jmp_pm:

	/* ����gdt */
	lgdt	(gdt_ptr)
	/* ���ж� */
	cli

	/* �򿪵�ַ��A20 */
	in		$0x92,%al
	or		$0x02, %al	/* ��������00000010 */
	out		%al,$0x92

	/* ׼���л�������ģʽ,�޸�cr0�Ĵ��� */
	movl	%cr0,%eax
	or		$1,%eax
	mov		%eax,%cr0

/* �������뱣��ģʽ */
jmp32:
	/* AT&T����ʽ  ljmp $sel_code,$pm_start */
	/* Intel����ʽ jmp far dword sel_code:pm_start */
	/* �������� */
	ljmp $sel_code,$pm_start
	#.quad 0x000800008200ea66
/************************************************************************/
/*                 		   ����ģʽ��ʼ��
/*                         	pm_start
/************************************************************************/
.code32
.org 512
pm_start:
	mov		$sel_data,%bx
	mov		%bx,%ds
	mov		%bx,%es
	mov		%bx,%ss
	mov		%bx,%fs
	mov		$0x8000,%esp

	mov 	$sel_video, %bx
	mov 	%bx, %gs			/* �Դ��gs */

	mov		$loader_msg32,%si
	mov		$320,%di
	call	disp_str32

	call	setup_paging

	mov		$loader_msg_paging,%si
	mov		$480,%di
	call	disp_str32

	call	goto_kernel
	ljmp	$sel_kcode,$Kernel_ImgAddr

/************************************************************************/
/*                     ���ڴ�����
/*                 		mem_set
/************************************************************************/
mem_set:
	movl	$PageDirBase,%eax
	movl	$PageDirBase + PtbCount*PageSize,%ecx
	subl	%eax,%ecx
_mem_set:
	movl	$0,%ds:(%eax,%ecx)
	subl	$4,%ecx
	jnz		_mem_set

	ret
/************************************************************************/
/*                      ����Ļ����ʾ�ַ���
/*                        disp_str32
/************************************************************************/
disp_str32:
	mov		$0x0f, %ah		/* �ڵװ��� */
_disp_str_loop32:
	mov		(%si),%al
	inc		%si
	test	%al, %al		/* �����Ƿ�Ϊ0 */
	jz		_disp_str_end32	/* Ϊ0����� */
	mov		%ax, %gs:(%di)	/* д�� */
	add		$2,  %di		/* di+2*/
	jmp		_disp_str_loop32	/* ������һ��ѭ�� */
_disp_str_end32:
	ret
loader_msg32:
	.asciz	"In protected mode now!"
loader_msg_paging:
	.asciz	"Setuping paging."
/************************************************************************/
/*                        ������ҳ����
/*                        setup_paging
/************************************************************************/
/*
 * setup_paging
 * ��ʼ����ҳ����
 * �Ƚ�ebx��Ϊ0������_setup_pde_init��ʵ��ǰ4M�ĶԵ�ӳ��
 * ����ӳ������ʱ�ģ������ں��У��ͻ�ȡ��ǰ4M��ӳ�䣬������ʱӳ����Ϊ�˱�֤eip����ȷѰַ
 * ��ebx��ΪPdeStart���������ַǰ4Mӳ�䵽3G��ʼ��4M
 * Ȼ��������ҳ����
 */
setup_paging:
	call	mem_set

	movl	$0,%ebx
	call	_setup_pde_init
	movl	$PdeStart,%ebx
	call	_setup_pde_init

	movl	$PageDirBase,%eax
	movl	%eax,%cr3
	movl	%cr0,%eax
	orl		$0x80000000,%eax
	movl	%eax,%cr0

	ret

/*
 * �ӳ���
 * setup_pde_init
 * ���ں˵�ַ�ռ�ӳ�䵽���Ե�ַǰ4M��3G��ʼ��ǰ4M
 * pde��index�ɴ������ebxȷ��
 */
_setup_pde_init:
	movl	$PageDirBase,%edi								/* edi:ҳĿ¼����ַ */
	movl	$PdeCount,%ecx									/* ecx:pde���� */
	movl	$PageTblBase | PG_P | PG_USU | PG_RWW,%eax		/* eax:pde�� */
_setup_pde_loop:
	movl	%eax,%ds:(%edi,%ebx,PageItemSize)				/* д��pde */
	incl	%ebx											/* index++ */
	addl	$PageSize,%eax									/* pde���е�ָ��ָ����һ��ҳ�� */
	loop	_setup_pde_loop									/* ѭ��ecx�� */

_setup_pte_init:
	movl	$PageTblBase,%edi								/* edi:ҳ�����ַ */
	movl	$PteCount,%ecx									/* ecx:ҳ������ */
	movl	$0,%ebx											/* ebx:index */
	movl	$ 0 | PG_P | PG_USU | PG_RWW,%eax				/* eax:pte�� */
_setup_pte_loop:
	movl	%eax,%ds:(%edi,%ebx,PageItemSize)				/* д��pte */
	incl	%ebx											/* index++ */
	addl	$PageSize,%eax									/* pte���е�ָ��ָ����һ��ҳ�� */
	loop	_setup_pte_loop									/* ѭ��ecx�� */
_setup_paging_end:
	ret
/************************************************************************/
/*                      ��ʼ���������ں�(���뱣��ģʽ��)
/*                        goto_kernel
/************************************************************************/
Elf_phnum		=	Kernel_FileAddr + 0x2c
Elf_phoff		=	Kernel_FileAddr + 0x1c

goto_kernel:
	movl	%ds:(Elf_phnum),%ecx		/* ecx:Program header table ����Ŀ�� */
	movzx	%cx,%ecx
	movl	%ds:(Elf_phoff),%esi		/* esi:Program header table ��ƫ���� */
	addl	$Kernel_FileAddr,%esi		/* ���ϻ�ַ��esiָ�� Program header table */
_goto_kernel_begin:
	movl	0(%esi),%eax				/* eax:p_type ������ */
	cmpl	$0,%eax						/* ���eaxΪ0 */
	jz		_goto_kernel_noaction		/* ������һ��Ŀ */
	pushl	0x10(%esi)					/* ѹ�� p_vaddr �����ַ(Ŀ��ָ��) */
	movl	0x04(%esi),%eax				/* eax:p_offset ��������ļ��е�ƫ�� */
	addl	$Kernel_FileAddr,%eax		/* ���ϻ�ַ��eaxָ������ */
	pushl	%eax						/* ѹ�����ε�ַ(Դָ��) */
	pushl	0x08(%esi)					/* ѹ�� p_vaddr �����ַ(Ŀ��ָ��) */
	call	memcpy						/* ����memcpy�����ڴ� */
	addl	$12,%esp					/* �����ջ */
_goto_kernel_noaction:
	addl	$0x20,%esi					/* esiָ����һ����Ŀ */
	decl	%ecx						/* ��Ŀ������1 */
	jnz		_goto_kernel_begin
_goto_kernel:
	ret
/************************************************************************/
/*                        �ڴ濽��
/*                        memcpy
/* void* memcpy(void* es:pDest, void* ds:pSrc, int iSize);
/************************************************************************/
memcpy:
	pushl	%ebp
	movl	%esp,%ebp

	pushl	%esi
	pushl	%edi
	pushl	%ecx

	movl	8(%ebp),%edi	/* Destination */
	movl	12(%ebp),%esi	/* Source */
	movl	16(%ebp),%ecx	/* Counter */
_memcpy_copy:
	cmpl	$0, %ecx 		/* �жϼ����� */
	jz		_memcpy_end		/* ������Ϊ��ʱ���� */

	movb	%ds:(%esi),%al	/* ��ȡ */
	incl	%esi

	movb	%al,%es:(%edi)	/* д�� */
	incl	%edi

	decl	%ecx			/* ��������һ */
	jmp		_memcpy_copy	/* ѭ�� */
_memcpy_end:
	movl	8(%ebp),%eax 	/* ����ֵ */

	popl	%ecx
	popl	%edi
	popl	%esi
	movl	%ebp,%esp
	popl	%ebp

	ret						/* �������������� */
/************************************************************************/
/*                        �ڴ����
/*                        memcpy
/* void    *memset(void *, int, size_t);
/************************************************************************/
memset:
	push %ebp
	mov %esp,%ebp
	mov 8(%ebp),%edi
    mov 12(%ebp),%eax
    mov 16(%ebp),%ecx
    rep
    stosb
    mov 8(%ebp),%eax
    mov %ebp,%esp
    pop %ebp
    ret
