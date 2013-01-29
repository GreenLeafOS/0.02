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
	u8			type;			/* ��id */
	u8			mode;			/* �ں�/�û� */
	FunAddr		constructor;	/* ���캯�� */
	FunAddr		destructor;		/* �������� */
	FunAddr*	vtable;			/* �麯����ָ�� */
}ClassDesc;



#endif /* CLASS_H_ */
