/*
 * mbrboot.s
 *
 *  Created on: 2012-8-26
 *      Author: greenleaf
 */

Loader_Start		=	2
Loader_End			=	4
Loader_Size			=	Loader_End - Loader_Start + 1
Loader_BaseAddr		=	0xb800
Loader_OffsetAddr	=	0x0

boot_drive			=	0x2000


.global _start
.code16
_start:
	/* 保存启动驱动器 */
	mov		%cs, %ax
	mov 	%ax, %ds
	mov 	%ax, %ss
	mov 	$0x7c00, %sp

	pushw	$0xb800
	popw	%gs

	movb 	%dl,%ds:(boot_drive)
	movb	%dl,%al
	call	DispAL

	call	read_loader

	loop:	jmp loop
	ljmp 	$Loader_BaseAddr,$Loader_OffsetAddr

DispAL:
	push	%ecx
	push	%edx
	push	%edi

	mov		(pos),%edi

	mov		$0x0F,%ah
	mov		%al,%dl

	shr		$4,%al
	mov		$2,%ecx
begin:
	and		$0xF,%al
	cmp		$9,%al
	ja		L1
	add		$'0',%al
	jmp		L2
L1:
	sub		$0x0A,%al
	add		$'A',%al
L2:
	mov		%ax,%gs:(%edi)
	add		$2,%edi

	mov		%dl,%al
	loop	begin

	add		$2,%edi
	mov		%edi,(pos)

	pop		%edi
	pop		%edx
	pop		%ecx

	ret

DispInt:
	mov		4(%esp),%eax
	shr		$24,%eax
	call	DispAL

	mov		4(%esp),%eax
	shr		$16,%eax
	call	DispAL

	mov		4(%esp),%eax
	shr		$8,%eax
	call	DispAL

	mov		4(%esp),%eax
	call	DispAL

	ret

read_loader:
	mov 	$Loader_BaseAddr,%ax	/* 段基址 */
	mov		%ax,%es
	mov		$Loader_OffsetAddr,%bx	/* 偏移量 */

	mov 	$2, %ah				/* 中断功能号 */
	mov		$0x80, %dl			/* 驱动器 */
	mov		$0,	%dh				/* 磁头号 */
	mov		$0, %ch				/* 柱面 */
	mov		$Loader_Start,%cl	/* 起始扇区 */
	mov		$Loader_Size,%al	/* 扇区数 */
	int		$0x13				/* 调用中断*/

	mov		%ah,%al
	call	DispAL
	movb	%ds:(boot_drive),%al
	call	DispAL
#	jc		read_loader			/* 检测错误 */

	ret
/************************************************************************/
/*                      	数据区
/*                          data
/************************************************************************/
pos:
	.word	1600


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

	.byte	0x04,0x00,0x00,0x00		/* 起始扇区LBA,从4开始，高位在低地址 */
	.byte	0xE0,0x01,0x00,0x00		/* 扇区数目,高位在低地址 */


	.quad	0,0
	.quad	0,0
	.quad	0,0
boot_lable:
	.word	0xAA55
