/*===============================================================
 *                     	  boot.s
 *						������������
 ===============================================================*/
 .include "config.inc"

/* �ļ�ϵͳ */
.section fs,"",@nobits
.include "fs.s"


.section .text
.global _start
.global read_loader
/************************************************************************/
/*                          ������
/*                          start
/************************************************************************/
.code16
_start:
	/* �������������� */
	movb %dl,(boot_drive)
	/* ��ʼ���μĴ�������ջָ�� */
	mov	%cs, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov $stack_top, %sp
	/* �Դ��es */
	mov $0xb800, %bx
	mov %bx, %es

	/* ����cls���� */
	call cls
	/* ����disp_str��ʾ�ַ��� */
	call disp_str
	/* ��ȡloader */
	call read_loader

	ljmp $Loader_BaseAddr,$Loader_OffsetAddr

/************************************************************************/
/*                      	����
/*                          cls
/************************************************************************/
cls:
	mov	$0, %ax		/* ���ַ� */
	mov	$2000, %cx	/* ѭ��2000�� */
	xor	%di, %di	/* ��di��0 */
_cls_loop:
	mov	%ax, %es:(%di)	/* д�� */
	add	$2, %di			/* ָ����һ���ռ� */
	dec	%cx				/* ѭ����������1 */
	test %cx, %cx		/* �����Ƿ�Ϊ0 */
	jnz	_cls_loop
_cls_end:
	ret
/************************************************************************/
/*                      ����Ļ����ʾ�ַ���
/*                         disp_str
/************************************************************************/
disp_str:
	mov		$boot_msg, %si	/* �ַ�����ַ */
	mov		$0x0f, %ah		/* �ڵװ��� */
	xor		%di, %di		/* ��di��0 */
_disp_str_loop:
	lodsb					/* (C)al=*si;si++; */
	test	%al, %al		/* �����Ƿ�Ϊ0 */
	jz		_disp_str_end	/* Ϊ0����� */
	mov		%ax, %es:(%di)	/* д�� */
	add		$2,  %di		/* di+2*/
	jmp		_disp_str_loop	/* ������һ��ѭ�� */
_disp_str_end:
	ret


/************************************************************************/
/*                      ��ȡloader
/*                     read_loader
/************************************************************************/
read_loader:
	call	read_mbr		/* ��ȡ�ļ�ϵͳ���ڷ��� */

	/* ׼������ */
	movw	$Loader_BaseAddr,(lba_buf+2)	/* �λ�ַ */
	movw	$Loader_OffsetAddr,(lba_buf)	/* ƫ���� */

	movw	(loader_start),%ax
	movw	%ax,(lba_start)
	movw	(loader_start+2),%ax
	movw	%ax,(lba_start+2)

	movw	(loader_size),%ax
	movw	%ax,(lba_count)

	/* ��ȡloader */
	call	read_disk
	ret

/************************************************************************/
/*                      ��ȡMBR
/*                     read_mbr
/************************************************************************/
read_mbr:

	/* ׼������ */
	movw	$0,(lba_buf+2)
	movw 	$0x600,(lba_buf)	/* ����MBR��0x600 */
	movb	$0,(lba_start)		/* ��ʼ���� */
	movb	$1,(lba_count)		/* ������ */

	/* ��ȡmbr */
	call	read_disk

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
	/* д�����ݰ�,���ֽ����� */
	mov		8(%bx),%ch
	mov		%ch,(lba_start + 0)
	mov		9(%bx),%ch
	mov		%ch,(lba_start + 1)
	mov		10(%bx),%ch
	mov		%ch,(lba_start + 2)
	mov		11(%bx),%ch
	mov		%ch,(lba_start + 3)

	/* ʹ�úʹ����һ���ĸ��ӶΣ����ݶ�ϵͳ�� */
	mov		%cs,%ax
	movw	%ax,(lba_buf+2)			/* ����� */
	movw	$fs_head,(lba_buf)		/* ����ƫ��fs_head,fs.s��ͷ����Ӳ���ϵ�ʵ��������䵽���ű�
									��ΪBIOSֻ��ȡ1�����������˳���Ҫʹ��2�����������Ժ�һ������
									��Ҫ�Լ���ȡ */
_read_mbr_end:
	call	read_disk
	ret
/************************************************************************/
/*                      ��ȡ����
/*                     read_disk
/************************************************************************/
read_disk:
	/* �����ж� */
	mov		$0,%ax
	mov		%ax,%ds
	mov		$lba,%si			/* ds:siָ��LBA���ݰ� */

	mov		$0x42, %ah			/* �жϹ��ܺţ���չ������ */
	mov		(boot_drive), %dl	/* ������ */
	int		$0x13				/* �����ж� */
	jc		read_disk

	ret
/************************************************************************/
/*                      	������
/*                          data
/************************************************************************/
boot_msg:
	.ascii	"Booting..."
	.byte	0

lba:
	lba_size:
		.byte	16		/* ���ݰ���С */
		.byte	0
	lba_count:
		.word	1		/* ������ */
	lba_buf:
		.word	0
		.word	0		/* �Σ�ƫ�� */
	lba_start:
		.quad	0		/* �������� */

free_space:
	.org	0x1BE
stack_top:
hd_partition_table:
	.byte	0x80	/* ������ */
	.byte	0x00	/* ��ʼ��ͷ */
	.byte	0x02	/* ��ʼ���� */
	.byte	0x00	/* ��ʼ���� */

	.byte	0xE0	/* Green�ļ�ϵͳ��־ */
	.byte	0x11	/* ������ͷ�� */
	.byte	0x2D	/* �������� */
	.byte	0x01	/* �������� */

	.byte	0x01,0x00,0x00,0x00		/* ��ʼ����LBA,��1��ʼ����λ�ڵ͵�ַ */
	.byte	0x3F,0x0B,0x00,0x00		/* ������Ŀ,��λ�ڵ͵�ַ */


	.quad	0,0
	.quad	0,0
	.quad	0,0
boot_lable:
	.word	0xAA55
