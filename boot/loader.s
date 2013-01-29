/*
 * loader.s
 * 输出Loading...
 * 检测磁盘，读入内核
 * 获取BIOS数据
 * 跳入保护模式
 * 初始化段寄存器
 * 输出In protected mode now!
 * 调整内核
 * 启动分页机制
 * 跳入内核
 */


 .code16
.global	_start
_start:
	jmp		real_start


/* 配置 */
.include "config.inc"
.include "protect.inc"

/* 文件系统 */
.section fs,"",@nobits
.include "fs.s"

/* 磁盘读取 */
.section .text
.include "disk.inc"
/************************************************************************/
/*						入口函数
/*						real_start
/************************************************************************/
.code16
real_start:
	/* 初始化 */
	mov		$0, %ax
	mov		%ax, %ds
	mov		%ax, %es
	mov		%ax, %ss
	mov		$0x7000,%sp

	/* 输出字符串 */
	call	disp_str

	/* 读入内核 */
	call	check_disk
	call	read_kernel

	/* 获取内存数量 */
	call	get_mem_info
	/* 获取CMOS */
	call	get_cmos

	/* 跳入保护模式 */
	jmp		jmp_pm

/************************************************************************/
/*						获取内存信息
/*					    get_mem_info
/*	读入 es:di 中
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
/*						获取CMOS信息
/*					    get_cmos
/*	读入 es:di 中
/************************************************************************/
get_cmos:
	mov		$Param_Seg,%ax
	mov		%ax,%es
	mov		$Bios_CMOS,%di		/* 偏移 */
	mov		$256,%cx			/* 循环次数 */

	xor		%ax,%ax
_get_coms_loop:
	mov		%ah,%al			/* 送CMOS内部地址 */
	out		%al,$0x70
	nop
	in		$0x71,%al		/* 读数据 */
	mov		%al,%es:(%di)	/* 保存数据 */
	inc		%ah				/* 调整CMOS内部地址 */
	inc		%di				/* 调整内存偏移 */
	loop	_get_coms_loop

	ret
/************************************************************************/
/*                      在屏幕上显示字符串
/*                         disp_str
/************************************************************************/
disp_str:
	mov		$loader_msg,%si
	mov		$160,%di
	mov		$0x0f, %ah		/* 黑底白字 */
_disp_str_loop:
	lodsb					/* (C)al=*si;si++; */
	test	%al, %al		/* 测试是否为0 */
	jz		_disp_str_end	/* 为0则结束 */
	mov		%ax, %gs:(%di)	/* 写入 */
	add		$2,  %di		/* di+2*/
	jmp		_disp_str_loop	/* 进入下一次循环 */
_disp_str_end:
	ret
loader_msg:
	.asciz	"Loading..."
/************************************************************************/
/*                      读取kernel
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
/*                      GDT段描述符表
/*                         gdt
/************************************************************************/
.code16
gdt:
/*标签				宏名			段基址			段界限		属性							*/
gdt_null:			Descriptor	0,				0,			0
gdt_desc_code:		Descriptor	0,				0xfffff,	DA_CR  | DA_32 | DA_LIMIT_4K
gdt_desc_data:		Descriptor	0,				0xfffff,	DA_DRW | DA_32 | DA_LIMIT_4K
gdt_desc_kcode:		Descriptor	KernelVAOffset,	0xfffff,	DA_CR  | DA_32 | DA_LIMIT_4K
gdt_desc_video:		Descriptor	0xb8000,		0x0ffff,	DA_DRW | DA_DPL3

/* gdtptr */
gdt_len		=	gdt_ptr - gdt
gdt_ptr:
	.word	gdt_len - 1		/* 段界限 */
	.word	gdt				/* 基地址低位 */
	.word	0				/* 高位一定为0 */

/* gdt选择子 */
sel_code		=	gdt_desc_code	- gdt
sel_data		=	gdt_desc_data	- gdt
sel_kcode		=	gdt_desc_kcode	- gdt
sel_video		=	gdt_desc_video	- gdt + SA_RPL3
/************************************************************************/
/*                     	跳入保护模式
/*                       jmp_pm
/************************************************************************/
jmp_pm:

	/* 加载gdt */
	lgdt	(gdt_ptr)
	/* 关中断 */
	cli

	/* 打开地址线A20 */
	in		$0x92,%al
	or		$0x02, %al	/* 二进制数00000010 */
	out		%al,$0x92

	/* 准备切换到保护模式,修改cr0寄存器 */
	movl	%cr0,%eax
	or		$1,%eax
	mov		%eax,%cr0

/* 真正进入保护模式 */
jmp32:
	/* AT&T汇编格式  ljmp $sel_code,$pm_start */
	/* Intel汇编格式 jmp far dword sel_code:pm_start */
	/* 机器代码 */
	ljmp $sel_code,$pm_start
	#.quad 0x000800008200ea66
/************************************************************************/
/*                 		   保护模式初始化
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
	mov 	%bx, %gs			/* 显存段gs */

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
/*                     对内存清零
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
/*                      在屏幕上显示字符串
/*                        disp_str32
/************************************************************************/
disp_str32:
	mov		$0x0f, %ah		/* 黑底白字 */
_disp_str_loop32:
	mov		(%si),%al
	inc		%si
	test	%al, %al		/* 测试是否为0 */
	jz		_disp_str_end32	/* 为0则结束 */
	mov		%ax, %gs:(%di)	/* 写入 */
	add		$2,  %di		/* di+2*/
	jmp		_disp_str_loop32	/* 进入下一次循环 */
_disp_str_end32:
	ret
loader_msg32:
	.asciz	"In protected mode now!"
loader_msg_paging:
	.asciz	"Setuping paging."
/************************************************************************/
/*                        启动分页机制
/*                        setup_paging
/************************************************************************/
/*
 * setup_paging
 * 初始化分页机制
 * 先将ebx置为0，调用_setup_pde_init，实现前4M的对等映射
 * 这种映射是暂时的，到了内核中，就会取消前4M的映射，这种暂时映射是为了保证eip能正确寻址
 * 将ebx置为PdeStart，把物理地址前4M映射到3G开始的4M
 * 然后启动分页机制
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
 * 子程序
 * setup_pde_init
 * 把内核地址空间映射到线性地址前4M或3G开始的前4M
 * pde的index由传入参数ebx确定
 */
_setup_pde_init:
	movl	$PageDirBase,%edi								/* edi:页目录基地址 */
	movl	$PdeCount,%ecx									/* ecx:pde项数 */
	movl	$PageTblBase | PG_P | PG_USU | PG_RWW,%eax		/* eax:pde项 */
_setup_pde_loop:
	movl	%eax,%ds:(%edi,%ebx,PageItemSize)				/* 写入pde */
	incl	%ebx											/* index++ */
	addl	$PageSize,%eax									/* pde项中的指针指向下一个页表 */
	loop	_setup_pde_loop									/* 循环ecx次 */

_setup_pte_init:
	movl	$PageTblBase,%edi								/* edi:页表基地址 */
	movl	$PteCount,%ecx									/* ecx:页表项数 */
	movl	$0,%ebx											/* ebx:index */
	movl	$ 0 | PG_P | PG_USU | PG_RWW,%eax				/* eax:pte项 */
_setup_pte_loop:
	movl	%eax,%ds:(%edi,%ebx,PageItemSize)				/* 写入pte */
	incl	%ebx											/* index++ */
	addl	$PageSize,%eax									/* pte项中的指针指向下一个页框 */
	loop	_setup_pte_loop									/* 循环ecx次 */
_setup_paging_end:
	ret
/************************************************************************/
/*                      初始化并进入内核(跳入保护模式后)
/*                        goto_kernel
/************************************************************************/
Elf_phnum		=	Kernel_FileAddr + 0x2c
Elf_phoff		=	Kernel_FileAddr + 0x1c

goto_kernel:
	movl	%ds:(Elf_phnum),%ecx		/* ecx:Program header table 中条目数 */
	movzx	%cx,%ecx
	movl	%ds:(Elf_phoff),%esi		/* esi:Program header table 的偏移量 */
	addl	$Kernel_FileAddr,%esi		/* 加上基址，esi指向 Program header table */
_goto_kernel_begin:
	movl	0(%esi),%eax				/* eax:p_type 段类型 */
	cmpl	$0,%eax						/* 如果eax为0 */
	jz		_goto_kernel_noaction		/* 跳入下一条目 */
	pushl	0x10(%esi)					/* 压入 p_vaddr 虚拟地址(目的指针) */
	movl	0x04(%esi),%eax				/* eax:p_offset 程序段在文件中的偏移 */
	addl	$Kernel_FileAddr,%eax		/* 加上基址，eax指向程序段 */
	pushl	%eax						/* 压入程序段地址(源指针) */
	pushl	0x08(%esi)					/* 压入 p_vaddr 虚拟地址(目的指针) */
	call	memcpy						/* 调用memcpy复制内存 */
	addl	$12,%esp					/* 清理堆栈 */
_goto_kernel_noaction:
	addl	$0x20,%esi					/* esi指向下一个条目 */
	decl	%ecx						/* 条目计数减1 */
	jnz		_goto_kernel_begin
_goto_kernel:
	ret
/************************************************************************/
/*                        内存拷贝
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
	cmpl	$0, %ecx 		/* 判断计数器 */
	jz		_memcpy_end		/* 计数器为零时跳出 */

	movb	%ds:(%esi),%al	/* 读取 */
	incl	%esi

	movb	%al,%es:(%edi)	/* 写入 */
	incl	%edi

	decl	%ecx			/* 计数器减一 */
	jmp		_memcpy_copy	/* 循环 */
_memcpy_end:
	movl	8(%ebp),%eax 	/* 返回值 */

	popl	%ecx
	popl	%edi
	popl	%esi
	movl	%ebp,%esp
	popl	%ebp

	ret						/* 函数结束，返回 */
/************************************************************************/
/*                        内存填充
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
