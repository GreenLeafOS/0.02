/*===============================================================
 *                     	 loader.s
 *						引导程序代码
 ===============================================================*/
 .code16
.global	_start
.global	_memcpy_end
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

	/* 清除内存 */
	call	mem_set

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
/*						  清除内存
/*					     mem_set
/************************************************************************/
mem_set:
	movl	$Bios_InfoSeg,%eax
	mov		%ax,%es
	movl	$Bios_InfoBase,%eax
	movl	$Bios_InfoEnd,%ecx
	subl	%eax,%ecx
_mem_set:
	movl	$0,%es:(%eax,%ecx)
	subl	$4,%ecx
	jnz		_mem_set

	ret
/************************************************************************/
/*						获取内存信息
/*					    get_mem_info
/*	读入 es:di 中
/************************************************************************/
get_mem_info:
	mov		$Bios_InfoSeg,%ax
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
	mov		$Bios_InfoSeg,%ax
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
	.asciz	"Load the kernel."
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
/*标签				宏名			段基址		段界限		属性							*/
gdt_null:			Descriptor	0,			0,			0
gdt_desc_code:		Descriptor	0,			0xfffff,	DA_CR  | DA_32 | DA_LIMIT_4K
gdt_desc_data:		Descriptor	0,			0xfffff,	DA_DRW | DA_32 | DA_LIMIT_4K
gdt_desc_video:		Descriptor	0xb8000,	0x0ffff,	DA_DRW | DA_DPL3

/* gdtptr */
gdt_len		=	gdt_ptr - gdt
gdt_ptr:
	.word	gdt_len - 1		/* 段界限 */
	.word	gdt				/* 基地址低位 */
	.word	0				/* 高位一定为0 */

/* gdt选择子 */
sel_code		=	gdt_desc_code	- gdt
sel_data		=	gdt_desc_data	- gdt
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

	call	disp_str32

	jmp		goto_kernel
/************************************************************************/
/*                      在屏幕上显示字符串
/*                         disp_str
/************************************************************************/
disp_str32:
	mov		$loader_msg32,%si
	mov		$320,%di
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
	ljmp	$sel_code,$Kernel_ImgAddr
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

