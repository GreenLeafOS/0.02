/*===============================================================
 *                     	  boot.s
 *						启动扇区代码
 ===============================================================*/
.global	_start
_start:
	ljmp	$0,$real_start


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
/*					   real_start
/************************************************************************/
.code16
real_start:
	/* 初始化段寄存器 */
	mov		$0,%ax
	mov		%ax,%ds
	mov		%ax,%ss
	mov		$0x7c00,%sp

	mov 	$0xb800,%bx
	mov 	%bx, %gs

	/* 对内存清零 */
	call	mem_set

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
/*						  清除内存
/*					     mem_set
/************************************************************************/
mem_set:
	mov		$Param_Seg,%ax
	mov		%ax,%es
	movl	$Param_Base,%eax
	movl	$Param_End,%ecx
	subl	%eax,%ecx
_mem_set:
	movl	$0,%es:(%eax,%ecx)
	subl	$4,%ecx
	jnz		_mem_set

	ret
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
/*                      读取loader
/*                     read_loader
/************************************************************************/
read_loader:
	/*===================读取mbr=====================*/
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

	/*========================读取loader==================================*/
	/*			lba		count			seg					offset 		*/
	read_disk	%eax,	(loader_size),	$Loader_BaseAddr,	$Loader_OffsetAddr

	ret
/************************************************************************/
/*                      	数据区
/*                          data
/************************************************************************/
boot_msg:		.asciz	"Booting..."
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
