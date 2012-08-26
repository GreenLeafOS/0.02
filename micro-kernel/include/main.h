/*
 * main.h
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "lib.h"

void kernel_main();
void kernel_main_thread();
void main_thread_create();

extern u32* kernel_stack_top;
extern int kernel_reenter;


#define		KERNEL_CODE		1
#define		KERNEL_DATA		2
#define		USER_CODE		3
#define		USER_DATA		4
#define		USER_VIDEO		5


#endif /* MAIN_H_ */
