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
.global read_loader
/************************************************************************/
/*                          主函数
/*                          start
/************************************************************************/
.code16
_start:
	/* 保存启动驱动器 */
	movb %dl,(boot_drive)
	/* 初始化段寄存器及堆栈指针 */
	mov	%cs, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov $stack_top, %sp
	/* 显存段es */
	mov $0xb800, %bx
	mov %bx, %es

	/* 调用cls清屏 */
	call cls
	/* 调用disp_str显示字符串 */
	call disp_str
	/* 读取loader */
	call read_loader

	ljmp $Loader_BaseAddr,$Loader_OffsetAddr

/************************************************************************/
/*                      	清屏
/*                          cls
/************************************************************************/
cls:
	mov	$0, %ax		/* 空字符 */
	mov	$2000, %cx	/* 循环2000次 */
	xor	%di, %di	/* 对di清0 */
_cls_loop:
	mov	%ax, %es:(%di)	/* 写入 */
	add	$2, %di			/* 指向下一个空间 */
	dec	%cx				/* 循环计数器减1 */
	test %cx, %cx		/* 测试是否为0 */
	jnz	_cls_loop
_cls_end:
	ret
/************************************************************************/
/*                      在屏幕上显示字符串
/*                         disp_str
/************************************************************************/
disp_str:
	mov		$boot_msg, %si	/* 字符串地址 */
	mov		$0x0f, %ah		/* 黑底白字 */
	xor		%di, %di		/* 对di清0 */
_disp_str_loop:
	lodsb					/* (C)al=*si;si++; */
	test	%al, %al		/* 测试是否为0 */
	jz		_disp_str_end	/* 为0则结束 */
	mov		%ax, %es:(%di)	/* 写入 */
	add		$2,  %di		/* di+2*/
	jmp		_disp_str_loop	/* 进入下一次循环 */
_disp_str_end:
	ret


/************************************************************************/
/*                      读取loader
/*                     read_loader
/************************************************************************/
read_loader:
	call	read_mbr		/* 读取文件系统所在分区 */

	/* 准备参数 */
	movw	$Loader_BaseAddr,(lba_buf+2)	/* 段基址 */
	movw	$Loader_OffsetAddr,(lba_buf)	/* 偏移量 */

	movw	(loader_start),%ax
	movw	%ax,(lba_start)
	movw	(loader_start+2),%ax
	movw	%ax,(lba_start+2)

	movw	(loader_size),%ax
	movw	%ax,(lba_count)

	/* 读取loader */
	call	read_disk
	ret

/************************************************************************/
/*                      读取MBR
/*                     read_mbr
/************************************************************************/
read_mbr:

	/* 准备参数 */
	movw	$0,(lba_buf+2)
	movw 	$0x600,(lba_buf)	/* 读入MBR到0x600 */
	movb	$0,(lba_start)		/* 起始扇区 */
	movb	$1,(lba_count)		/* 扇区数 */

	/* 读取mbr */
	call	read_disk

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
	/* 写入数据包,倒字节排序 */
	mov		8(%bx),%ch
	mov		%ch,(lba_start + 0)
	mov		9(%bx),%ch
	mov		%ch,(lba_start + 1)
	mov		10(%bx),%ch
	mov		%ch,(lba_start + 2)
	mov		11(%bx),%ch
	mov		%ch,(lba_start + 3)

	/* 使用和代码段一样的附加段（兼容多系统） */
	mov		%cs,%ax
	movw	%ax,(lba_buf+2)			/* 保存段 */
	movw	$fs_head,(lba_buf)		/* 保存偏移fs_head,fs.s开头，把硬盘上的实际数据填充到符号表
									因为BIOS只读取1个扇区，而此程序要使用2个扇区，所以后一个扇区
									需要自己读取 */
_read_mbr_end:
	call	read_disk
	ret
/************************************************************************/
/*                      读取磁盘
/*                     read_disk
/************************************************************************/
read_disk:
	/* 调用中断 */
	mov		$0,%ax
	mov		%ax,%ds
	mov		$lba,%si			/* ds:si指向LBA数据包 */

	mov		$0x42, %ah			/* 中断功能号：扩展读扇区 */
	mov		(boot_drive), %dl	/* 驱动器 */
	int		$0x13				/* 调用中断 */
	jc		read_disk

	ret
/************************************************************************/
/*                      	数据区
/*                          data
/************************************************************************/
boot_msg:
	.ascii	"Booting..."
	.byte	0

lba:
	lba_size:
		.byte	16		/* 数据包大小 */
		.byte	0
	lba_count:
		.word	1		/* 扇区数 */
	lba_buf:
		.word	0
		.word	0		/* 段：偏移 */
	lba_start:
		.quad	0		/* 绝对扇区 */

free_space:
	.org	0x1BE
stack_top:
hd_partition_table:
	.byte	0x80	/* 可引导 */
	.byte	0x00	/* 起始磁头 */
	.byte	0x02	/* 起始扇区 */
	.byte	0x00	/* 起始柱面 */

	.byte	0xE0	/* Green文件系统标志 */
	.byte	0x11	/* 结束磁头号 */
	.byte	0x2D	/* 结束扇区 */
	.byte	0x01	/* 结束柱面 */

	.byte	0x01,0x00,0x00,0x00		/* 起始扇区LBA,从1开始，高位在低地址 */
	.byte	0x3F,0x0B,0x00,0x00		/* 扇区数目,高位在低地址 */


	.quad	0,0
	.quad	0,0
	.quad	0,0
boot_lable:
	.word	0xAA55
