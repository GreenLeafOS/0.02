/*===============================================================
 *                     	  boot.s
 *						启动扇区代码
 ===============================================================*/
 .include "config.inc"

/* 文件系统 */
.section fs,"",@nobits
.include "fs.s"


.section .text
.global _start

/*
 * start
 */
.code16
_start:
	ljmp	$0,$real_start


/*
 * 起始函数
 * 由start长跳入
 */
real_start:
	/* 初始化段寄存器 */
	mov		%cs,%ax
	mov		%ax,%ds
	mov		%ax,%ss
	mov		$0x7c00,%sp

	mov 	$0xb800,%bx
	mov 	%bx, %gs

	movb 	%dl,%ds:(boot_drive)

	/* 调用cls清屏 */
	call 	cls

	/* 调用disp_str显示字符串 */
	mov		$boot_msg,%si
	mov		$0,%di
	call	disp_str

	/* 读取loader */
	call	check_disk
	call 	read_loader

	ljmp 	$Loader_BaseAddr,$Loader_OffsetAddr

/************************************************************************/
/*                      	清屏
/*                          cls
/************************************************************************/
cls:
	mov		$1000, %ecx	/* 循环2000次 */
_cls_loop:
	movl	$0, %gs:(%di)	/* 写入 */
	add		$4, %di			/* 指向下一个空间 */
	loop	_cls_loop

	ret
/************************************************************************/
/*                      在屏幕上显示字符串
/*                         disp_str
/************************************************************************/
disp_str:
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
/************************************************************************/
/*                      检测磁盘模式
/*                     	check_disk
/************************************************************************/
check_disk:
	/* 验证是否支持LBA */
	movb	$0x41, %ah
	movw	$0x55aa, %bx
	int		$0x13

	/* 如果不支持，就使用chs */
	jc		chs_mode
	cmpw	$0xaa55, %bx
	jne		chs_mode

	andw	$1, %cx
	jz		chs_mode
#	jmp		chs_mode

/* 初始化lba模式 */
lba_mode:
	movw	$read_disk_lba,(read_disk_fun)	/* 保存调用函数地址 */
	ret

/* 初始化chs模式 */
chs_mode:
	movw	$read_disk_chs,(read_disk_fun)	/* 保存调用函数地址 */

	movb	$8, %ah
	int		$0x13				/* 获取驱动器参数 */

	movzbl	%dh, %eax
	incw	%ax
	movl	%eax,(heads)		/* 保存磁头数 */

	movzbw	%cl, %dx
	shlw	$2, %dx
	movb	%ch, %al
	movb	%dh, %ah
	incw	%ax
	movw	%ax, (cylinders)	/* 保存柱面数 */

	movzbw	%dl, %ax
	shrb	$2, %al
	movl	%eax, (sectors)		/* 保存扇面数 */

	ret
/************************************************************************/
/*                      读取磁盘
/*                     read_disk
/* 入口参数：
/*		lba		lba绝对扇区地址	eax
/*		count	扇区数			cx
/*		seg		段				bx
/*		offset	目标地址			di
/************************************************************************/
.macro read_disk lba,count,seg,offset
	movl	\lba,%eax
	movw	\count,%cx
	movw	\seg,%bx
	movw	\offset,%di
	call	*(read_disk_fun)
.endm

/* lba模式 */
read_disk_lba:
	pushl	%eax
	pushl	%edx

	movw	$disk_address_packet,%si
	movb	$0x10,	0(%si)		/* 包大小，默认0x10 */
	movw	%cx,	2(%si)		/* 读取块数 */
	movw	%di,	4(%si)		/* 偏移 */
	movw	%bx,	6(%si)		/* 段 */
	movl	%eax,	8(%si)		/* lba绝对扇区地址 */

	movb	$0x42, %ah			/* 功能号 */
	mov		%ds:(boot_drive), %dl			/* 驱动器 */
	int		$0x13

	/* 如果出错，就尝试chs模式  */
	popl	%edx
	popl	%eax
	jc		read_disk_chs

	ret


/* chs模式 */
read_disk_chs:
	pushl	%ebp
	movl	%esp,%ebp
	subl	$24,%esp

	movl	%eax,-16(%ebp)	/* int lba */

	movb	%cl,%al			/* 扇区数 */
	movb	$0x02, %ah		/* 功能号 */
	movl	%eax,-20(%ebp)	/* 保存最终eax */

	movw	%bx, %es		/* 缓冲区段 */
	movw	%di, %bx		/* 缓冲区偏移 */

	movl	-16(%ebp),%eax	/* 读取lba绝对扇区号 */
	xorl	%edx, %edx
	divl	(sectors)		/* 绝对扇区号除以每柱面扇区数 */
	movb	%dl, %cl		/* 商为柱面号，余数为起始扇区号 */
	incb	%cl				/* chs的起始扇区参数必须大于0 */

	xorw	%dx, %dx
	divl	(heads)			/* 柱面号除以磁头数 */
	movb	%al, %ch		/* 商是柱面号,余数为磁头号 */

	xorb	%al, %al		/* 柱面号的高字节 */
	shrw	$2, %ax
	orb		%al, %cl

	movb	%dl, %dh		/* 磁头号 */

	movl	-20(%ebp),%eax	/* 恢复最终eax */

	movb	%ds:(boot_drive),%dl
	int		$0x13

	movl	%ebp,%esp
	popl	%ebp
	ret
/************************************************************************/
/*                      读取loader
/*                     read_loader
/************************************************************************/
read_loader:
	/* 读取mbr 										*/
	/*			lba		count	seg		offset 		*/
	read_disk	$0,		$1,		$0,		$0x600


/*
 * 查找Green分区
 */
_read_mbr_get_partition:
	mov 	$0x7BE,%bx			/* bx指向分区表，0x600 + 0x1be */
	mov		$4,%cl				/* cl计数器 */
	jmp		_read_mbr_loop

_read_mbr_next:
	add		$16,%bx				/* 指向下一个项 */
	dec		%cl					/* cl计数器减1 */
	jz		_read_mbr_end		/* 如果为0，则返回 */

_read_mbr_loop:

	/* 状态 */
	movb 	0(%bx),%al
	cmp		$0x80,%al			/* 测试是否可引导 */
	jnz		_read_mbr_next		/* 搜索下一个分区 */

	/* 分区类型id */
	movb 	4(%bx),%al
	cmp		$0xe0,%al			/* 0xe0表示是Green文件系统 */
	jnz		_read_mbr_next		/* 搜索下一个分区 */


_read_mbr_fs:
	movl	8(%bx),%eax			/* 写入数据包,倒字节排序 */
#	bswap	%eax
	movl	%eax,(start_sector)
_read_mbr_end:

	/*			lba		count	seg		offset 		*/
	read_disk 	%eax,	$1,		%cs,	$fs_head

_read_loader_read:
	movl	(loader_start),%eax			/* 读取loader偏移 */
	movl	(start_sector),%ebx			/* 读取起始扇区 */
	addl	%ebx,%eax					/* loader在分区的偏移加分区引导记录基地址 */

	/* 读取loader 														*/
	/*			lba		count			seg					offset 		*/
	read_disk	%eax,	(loader_size),	$Loader_BaseAddr,	$Loader_OffsetAddr

	ret
/************************************************************************/
/*                      	数据区
/*                          data
/************************************************************************/
boot_msg:		.asciz	"Booting..."

/* 读取磁盘函数的地址 */
read_disk_fun:
	.long		0

/* lba模式和chs使用同一块缓冲区，因为不可能冲突 */
disk_address_packet:
sectors:
	.long	0
heads:
	.long	0
cylinders:
	.word	0
sector_start:
	.byte	0
head_start:
	.byte	0
cylinder_start:
	.word	0
	.word	0

free_space:
	.org	0x1BE
hd_partition_table:
	.byte	0x80	/* 可引导 */
	.byte	0x00	/* 起始磁头 */
	.byte	0x02	/* 起始扇区 */
	.byte	0x00	/* 起始柱面 */

	.byte	0xE0	/* Green文件系统标志 */
	.byte	0x0A	/* 结束磁头号 */
	.byte	0x1F	/* 结束扇区 */
	.byte	0x00	/* 结束柱面 */

	.byte	0x01,0x00,0x00,0x00		/* 起始扇区LBA,从1开始，高位在低地址 */
	.byte	0xE0,0x01,0x00,0x00		/* 扇区数目,高位在低地址 */


	.quad	0,0
	.quad	0,0
	.quad	0,0
boot_lable:
	.word	0xAA55
