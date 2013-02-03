/*
 * bios.s
 *
 *  Created on: 2012-8-6
 *      Author: greenleaf
 */

.section bios,"",@nobits

.global mem_info
.global mem_mcr_number
.global mem_size
.global mem_used_map
.global mem_used_map_max
.global cmos_info

.org		0x1000
mem_info:
	.org	0x1200
mem_mcr_number:
	.long	0
mem_size:
	.long	0
mem_used_map:
	.long	0
mem_used_map_max:
	.long	0

	.org	0x1300
cmos_info:

