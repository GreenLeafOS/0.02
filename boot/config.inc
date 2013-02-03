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

PageDirBase			=		0x20000					/* 页目录在1M以下的零碎空间中找 */
PageTblBase			=		0x24000					/* 页表开始 */
KernelSize			=		0x800000				/* 内核物理空间暂定8M大小，进入了内核便会根据内存大小进行调整 */
KernelVAOffset		=		0xc0000000				/* 3G开始是内核地址空间 */
KernelVAEnd			=		0xc0000000 + KernelSize	/* 映射物理地址前8M到3G */

PdeStart			=		KernelVAOffset/PageItems/PageSize	/* 计算线性地址的页表描述符的pde */

PgCount				=		KernelSize/PageSize					/* 计算有几个需要映射的页框 */
PtbCount			=		(PgCount+0x3ff)>>10					/* 计算需要几个页表,先加上1023，再除以1024，就可以实现向上进位 */



