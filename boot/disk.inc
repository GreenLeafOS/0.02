/*
 disk.inc

 Created on: 2012-8-26
     Author: greenleaf
*/
.code16
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
	movw	$read_disk_lba,%ds:(read_disk_fun)	/* 保存调用函数地址 */
	ret

/* 初始化chs模式 */
chs_mode:
	movw	$read_disk_chs,%ds:(read_disk_fun)	/* 保存调用函数地址 */

	movb	$8, %ah
	int		$0x13				/* 获取驱动器参数 */

	movzbl	%dh, %eax
	incw	%ax
	movl	%eax,%ds:(heads)		/* 保存磁头数 */

	movzbw	%cl, %dx
	shlw	$2, %dx
	movb	%ch, %al
	movb	%dh, %ah
	incw	%ax
	movw	%ax, %ds:(cylinders)	/* 保存柱面数 */

	movzbw	%dl, %ax
	shrb	$2, %al
	movl	%eax, %ds:(sectors)		/* 保存扇面数 */

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
	divl	%ds:(sectors)	/* 绝对扇区号除以每柱面扇区数 */
	movb	%dl, %cl		/* 商为柱面号，余数为起始扇区号 */
	incb	%cl				/* chs的起始扇区参数必须大于0 */

	xorw	%dx, %dx
	divl	%ds:(heads)		/* 柱面号除以磁头数 */
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


/* 读取磁盘函数的地址 */
read_disk_fun			=			0x2008
/* lba模式和chs使用同一块缓冲区，因为不可能冲突 */
disk_address_packet		=			0x200C
sectors					=			0x2010
heads					=			0x2014
cylinders				=			0x2018
sector_start			=			0x201C
head_start				=			0x2020
cylinder_start			=			0x2024

	