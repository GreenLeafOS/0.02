/*
 disk.inc

 Created on: 2012-8-26
     Author: greenleaf
*/
.code16
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
	movw	$read_disk_lba,%ds:(read_disk_fun)	/* ������ú�����ַ */
	ret

/* ��ʼ��chsģʽ */
chs_mode:
	movw	$read_disk_chs,%ds:(read_disk_fun)	/* ������ú�����ַ */

	movb	$8, %ah
	int		$0x13				/* ��ȡ���������� */

	movzbl	%dh, %eax
	incw	%ax
	movl	%eax,%ds:(heads)		/* �����ͷ�� */

	movzbw	%cl, %dx
	shlw	$2, %dx
	movb	%ch, %al
	movb	%dh, %ah
	incw	%ax
	movw	%ax, %ds:(cylinders)	/* ���������� */

	movzbw	%dl, %ax
	shrb	$2, %al
	movl	%eax, %ds:(sectors)		/* ���������� */

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

	/* ����������ͳ���chsģʽ  */
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
	divl	%ds:(sectors)	/* ���������ų���ÿ���������� */
	movb	%dl, %cl		/* ��Ϊ����ţ�����Ϊ��ʼ������ */
	incb	%cl				/* chs����ʼ���������������0 */

	xorw	%dx, %dx
	divl	%ds:(heads)		/* ����ų��Դ�ͷ�� */
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


/* ��ȡ���̺����ĵ�ַ */
read_disk_fun			=			0x2008
/* lbaģʽ��chsʹ��ͬһ�黺��������Ϊ�����ܳ�ͻ */
disk_address_packet		=			0x200C
sectors					=			0x2010
heads					=			0x2014
cylinders				=			0x2018
sector_start			=			0x201C
head_start				=			0x2020
cylinder_start			=			0x2024

	