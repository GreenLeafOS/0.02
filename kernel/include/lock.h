/*
 * lock.h
 *
 *  Created on: 2012-8-8
 *      Author: greenleaf
 */

#ifndef LOCK_H_
#define LOCK_H_

#include "reg.h"


#define KernelLock() \
		unsigned int eflags;	\
		StoreFlags(eflags);		\
		CloseInt()


#define KernelUnlock()	\
		RecoveryFlags(eflags)


#endif /* LOCK_H_ */
