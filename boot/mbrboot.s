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
	/* �������������� */
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
	mov 	$Loader_BaseAddr,%ax	/* �λ�ַ */
	mov		%ax,%es
	mov		$Loader_OffsetAddr,%bx	/* ƫ���� */

	mov 	$2, %ah				/* �жϹ��ܺ� */
	mov		$0x80, %dl			/* ������ */
	mov		$0,	%dh				/* ��ͷ�� */
	mov		$0, %ch				/* ���� */
	mov		$Loader_Start,%cl	/* ��ʼ���� */
	mov		$Loader_Size,%al	/* ������ */
	int		$0x13				/* �����ж�*/

	mov		%ah,%al
	call	DispAL
	movb	%ds:(boot_drive),%al
	call	DispAL
#	jc		read_loader			/* ������ */

	ret
/************************************************************************/
/*                      	������
/*                          data
/************************************************************************/
pos:
	.word	1600


	.org	0x1BE
hd_partition_table:
	.byte	0x80	/* ������ */
	.byte	0x00	/* ��ʼ��ͷ */
	.byte	0x02	/* ��ʼ���� */
	.byte	0x00	/* ��ʼ���� */

	.byte	0xE0	/* Green�ļ�ϵͳ��־ */
	.byte	0x0A	/* ������ͷ�� */
	.byte	0x1F	/* �������� */
	.byte	0x00	/* �������� */

	.byte	0x04,0x00,0x00,0x00		/* ��ʼ����LBA,��4��ʼ����λ�ڵ͵�ַ */
	.byte	0xE0,0x01,0x00,0x00		/* ������Ŀ,��λ�ڵ͵�ַ */


	.quad	0,0
	.quad	0,0
	.quad	0,0
boot_lable:
	.word	0xAA55
