/*
 * fs.s
 *
 *  Created on: 2012-8-24
 *      Author: greenleaf
 */


fs_head:
	loader_start:		.long			0x2
	loader_size:		.long			0x2
	kernel_start:		.long			0x4
	kernel_size:		.long			0x64
