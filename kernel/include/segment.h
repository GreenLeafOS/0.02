/*
 * segment.h
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_


#define		KERNEL_CODE		1
#define		KERNEL_DATA		2
#define		USER_CODE		3
#define		USER_DATA		4
#define		USER_VIDEO		5

void		gdt_init();

#endif /* SEGMENT_H_ */
