/*
 * boot∫Õloaderµƒ≈‰÷√
*/

boot_drive 				= 	0x2000
start_sector			= 	0x2004

Kernel_FileBaseAddr		=	0x1000
Kernel_FileOffsetAddr	=	0
Kernel_FileAddr			=	0x10000
Kernel_ImgAddr			=	Kernel_FileAddr*16

Loader_BaseAddr			=	0
Loader_OffsetAddr		=	0x8000

Bios_InfoBase			=	0x3000
Bios_MCR				=	0x0
Bios_MCRNumber			=	0x200
Bios_CMOS				=	0x300



