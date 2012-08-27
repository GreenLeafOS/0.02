/*===============================================================
 *                     	  boot.s
 *						������������
 ===============================================================*/
.global	_start
_start:
	ljmp	$0,$real_start


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
/*					   real_start
/************************************************************************/
.code16
real_start:
	/* ��ʼ���μĴ��� */
	mov		$0,%ax
	mov		%ax,%ds
	mov		%ax,%ss
	mov		$0x7c00,%sp

	mov 	$0xb800,%bx
	mov 	%bx, %gs

	/* ���ڴ����� */
	call	mem_set

	movb 	%dl,%ds:(boot_drive)

	/* ����cls���� */
	call 	cls

	/* ����disp_str��ʾ�ַ��� */
	mov		$boot_msg,%si
	mov		$0,%di
	call	disp_str

	/* ��ȡloader */
	call	check_disk
	call 	read_loader

	ljmp 	$Loader_BaseAddr,$Loader_OffsetAddr
/************************************************************************/
/*						  ����ڴ�
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
/*                      	����
/*                          cls
/************************************************************************/
cls:
	mov		$1000, %ecx	/* ѭ��2000�� */
_cls_loop:
	movl	$0, %gs:(%di)	/* д�� */
	add		$4, %di			/* ָ����һ���ռ� */
	loop	_cls_loop

	ret
/************************************************************************/
/*                      ����Ļ����ʾ�ַ���
/*                         disp_str
/************************************************************************/
disp_str:
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
/************************************************************************/
/*                      ��ȡloader
/*                     read_loader
/************************************************************************/
read_loader:
	/*===================��ȡmbr=====================*/
	/*			lba		count	seg		offset 		*/
	read_disk	$0,		$1,		$0,		$0x600


/*
 * ����Green����
 */
_read_mbr_get_partition:
	mov 	$0x7BE,%bx			/* bxָ�������0x600 + 0x1be */
	mov		$4,%cl				/* cl������ */
	jmp		_read_mbr_loop

_read_mbr_next:
	add		$16,%bx				/* ָ����һ���� */
	dec		%cl					/* cl��������1 */
	jz		_read_mbr_end		/* ���Ϊ0���򷵻� */

_read_mbr_loop:

	/* ״̬ */
	movb 	0(%bx),%al
	cmp		$0x80,%al			/* �����Ƿ������ */
	jnz		_read_mbr_next		/* ������һ������ */

	/* ��������id */
	movb 	4(%bx),%al
	cmp		$0xe0,%al			/* 0xe0��ʾ��Green�ļ�ϵͳ */
	jnz		_read_mbr_next		/* ������һ������ */


_read_mbr_fs:
	movl	8(%bx),%eax			/* д�����ݰ�,���ֽ����� */
#	bswap	%eax
	movl	%eax,(start_sector)
_read_mbr_end:

	/*			lba		count	seg		offset 		*/
	read_disk 	%eax,	$1,		%cs,	$fs_head

_read_loader_read:
	movl	(loader_start),%eax			/* ��ȡloaderƫ�� */
	movl	(start_sector),%ebx			/* ��ȡ��ʼ���� */
	addl	%ebx,%eax					/* loader�ڷ�����ƫ�Ƽӷ���������¼����ַ */

	/*========================��ȡloader==================================*/
	/*			lba		count			seg					offset 		*/
	read_disk	%eax,	(loader_size),	$Loader_BaseAddr,	$Loader_OffsetAddr

	ret
/************************************************************************/
/*                      	������
/*                          data
/************************************************************************/
boot_msg:		.asciz	"Booting..."
free_space:
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

	.byte	0x01,0x00,0x00,0x00		/* ��ʼ����LBA,��1��ʼ����λ�ڵ͵�ַ */
	.byte	0xE0,0x01,0x00,0x00		/* ������Ŀ,��λ�ڵ͵�ַ */


	.quad	0,0
	.quad	0,0
	.quad	0,0
boot_lable:
	.word	0xAA55
