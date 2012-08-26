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

/*
 * start
 */
.code16
_start:
	ljmp	$0,$real_start


/*
 * ��ʼ����
 * ��start������
 */
real_start:
	/* ��ʼ���μĴ��� */
	mov		%cs,%ax
	mov		%ax,%ds
	mov		%ax,%ss
	mov		$0x7c00,%sp

	mov 	$0xb800,%bx
	mov 	%bx, %gs

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
/*                      ������ģʽ
/*                     	check_disk
/************************************************************************/
check_disk:
	/* ��֤�Ƿ�֧��LBA */
	movb	$0x41, %ah
	movw	$0x55aa, %bx
	int		$0x13

	/* �����֧�֣���ʹ��chs */
	jc		chs_mode
	cmpw	$0xaa55, %bx
	jne		chs_mode

	andw	$1, %cx
	jz		chs_mode
#	jmp		chs_mode

/* ��ʼ��lbaģʽ */
lba_mode:
	movw	$read_disk_lba,(read_disk_fun)	/* ������ú�����ַ */
	ret

/* ��ʼ��chsģʽ */
chs_mode:
	movw	$read_disk_chs,(read_disk_fun)	/* ������ú�����ַ */

	movb	$8, %ah
	int		$0x13				/* ��ȡ���������� */

	movzbl	%dh, %eax
	incw	%ax
	movl	%eax,(heads)		/* �����ͷ�� */

	movzbw	%cl, %dx
	shlw	$2, %dx
	movb	%ch, %al
	movb	%dh, %ah
	incw	%ax
	movw	%ax, (cylinders)	/* ���������� */

	movzbw	%dl, %ax
	shrb	$2, %al
	movl	%eax, (sectors)		/* ���������� */

	ret
/************************************************************************/
/*                      ��ȡ����
/*                     read_disk
/* ��ڲ�����
/*		lba		lba����������ַ	eax
/*		count	������			cx
/*		seg		��				bx
/*		offset	Ŀ���ַ			di
/************************************************************************/
.macro read_disk lba,count,seg,offset
	movl	\lba,%eax
	movw	\count,%cx
	movw	\seg,%bx
	movw	\offset,%di
	call	*(read_disk_fun)
.endm

/* lbaģʽ */
read_disk_lba:
	pushl	%eax
	pushl	%edx

	movw	$disk_address_packet,%si
	movb	$0x10,	0(%si)		/* ����С��Ĭ��0x10 */
	movw	%cx,	2(%si)		/* ��ȡ���� */
	movw	%di,	4(%si)		/* ƫ�� */
	movw	%bx,	6(%si)		/* �� */
	movl	%eax,	8(%si)		/* lba����������ַ */

	movb	$0x42, %ah			/* ���ܺ� */
	mov		%ds:(boot_drive), %dl			/* ������ */
	int		$0x13

	/* ��������ͳ���chsģʽ  */
	popl	%edx
	popl	%eax
	jc		read_disk_chs

	ret


/* chsģʽ */
read_disk_chs:
	pushl	%ebp
	movl	%esp,%ebp
	subl	$24,%esp

	movl	%eax,-16(%ebp)	/* int lba */

	movb	%cl,%al			/* ������ */
	movb	$0x02, %ah		/* ���ܺ� */
	movl	%eax,-20(%ebp)	/* ��������eax */

	movw	%bx, %es		/* �������� */
	movw	%di, %bx		/* ������ƫ�� */

	movl	-16(%ebp),%eax	/* ��ȡlba���������� */
	xorl	%edx, %edx
	divl	(sectors)		/* ���������ų���ÿ���������� */
	movb	%dl, %cl		/* ��Ϊ����ţ�����Ϊ��ʼ������ */
	incb	%cl				/* chs����ʼ���������������0 */

	xorw	%dx, %dx
	divl	(heads)			/* ����ų��Դ�ͷ�� */
	movb	%al, %ch		/* ���������,����Ϊ��ͷ�� */

	xorb	%al, %al		/* ����ŵĸ��ֽ� */
	shrw	$2, %ax
	orb		%al, %cl

	movb	%dl, %dh		/* ��ͷ�� */

	movl	-20(%ebp),%eax	/* �ָ�����eax */

	movb	%ds:(boot_drive),%dl
	int		$0x13

	movl	%ebp,%esp
	popl	%ebp
	ret
/************************************************************************/
/*                      ��ȡloader
/*                     read_loader
/************************************************************************/
read_loader:
	/* ��ȡmbr 										*/
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

	/* ��ȡloader 														*/
	/*			lba		count			seg					offset 		*/
	read_disk	%eax,	(loader_size),	$Loader_BaseAddr,	$Loader_OffsetAddr

	ret
/************************************************************************/
/*                      	������
/*                          data
/************************************************************************/
boot_msg:		.asciz	"Booting..."

/* ��ȡ���̺����ĵ�ַ */
read_disk_fun:
	.long		0

/* lbaģʽ��chsʹ��ͬһ�黺��������Ϊ�����ܳ�ͻ */
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
