/*
 * page_init.c
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */

#include "../include/phypage.h"
#include "../include/linearspace.h"
#include "../include/config.h"

void page_init()
{
	buddy_init();
	kernel_space.start = KernelBase/PageSize;
	kernel_space.block = page_alloc(11);
}
