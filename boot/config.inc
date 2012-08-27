/*
 * boot和loader的配置
*/

Param_Seg				=	0x0
Param_Base				=	0x1000
Param_End				=	0x2400

boot_drive 				= 	0x2000
start_sector			= 	0x2004

Kernel_FileBaseAddr		=	0x1000
Kernel_FileOffsetAddr	=	0
Kernel_FileAddr			=	0x10000
Kernel_ImgAddr			=	Kernel_FileAddr*16

Loader_BaseAddr			=	0
Loader_OffsetAddr		=	0x8000

Bios_MCR				=	0x1000
Bios_MCRNumber			=	0x1200
Bios_CMOS				=	0x1300


PageSize			=		0x1000					/* 4k */
PageItemSize		=		0x4						/* 4字节大小的页表项 */
PageItems			=		PageSize/PageItemSize	/* 1024个表项 */

PageDirBase			=		0x300000				/* 页目录3M开始 */
PageTblBase			=		PageDirBase + PageSize	/* 页表紧接着页目录 */
KernelSize			=		0x400000				/* 内核4M大小 */
KernelVAOffset		=		0xc0000000				/* 3G开始是内核地址空间 */
KernelVAEnd			=		0xc0000000 + KernelSize	/* 映射物理地址前4M到3G */

PdeStart			=		KernelVAOffset/PageItems/PageSize	/* 计算线性地址的页表描述符的pde */

PteCount			=		KernelSize/PageSize					/* 计算需要几个pte */
PdeCount			=		PteCount/PageItems					/* 计算需要几个pde */



