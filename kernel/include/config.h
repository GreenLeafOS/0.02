/*
 * config.h
 *
 *  Created on: 2012-8-27
 *      Author: greenleaf
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#define KernelBase	0xc0000000
#define UserLinearSpacePages		786432		// 3G

void*	phypage_base;		//	物理页描述符起始地址


#endif /* CONFIG_H_ */
