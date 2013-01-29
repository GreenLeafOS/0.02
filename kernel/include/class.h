/*
 * class.h
 *
 *  Created on: 2012-8-29
 *      Author: greenleaf
 */

#ifndef CLASS_H_
#define CLASS_H_

#include <lib/include/type.h>


/* struct class_t */
typedef struct class_desc
{
	u8			type;			/* 类id */
	u8			mode;			/* 内核/用户 */
	FunAddr		constructor;	/* 构造函数 */
	FunAddr		destructor;		/* 析构函数 */
	FunAddr*	vtable;			/* 虚函数表指针 */
}ClassDesc;



#endif /* CLASS_H_ */
