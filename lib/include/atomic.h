#ifndef _ATOMIC_H
#define _ATOMIC_H
/*===============================================================
 *                      Atomic.h
 *                     原子数据类 定义
 *                     								绿叶-柳暗花明
 ===============================================================*/
#include <lib/include/type.h>

typedef struct atomic_t
{
	volatile int value;
}Atomic;

/* 读 */
static inline int atomic_read(Atomic *data)
{
	return data->value;
}

/* 写 */
static inline void atomic_write(Atomic *data,int value)
{
	data->value = value;
	return;
}

/* 加 */
static inline void atomic_add(Atomic *data,int value)
{
	__asm volatile("addl %1,%0"
		             : "+m" (data->value)
		             : "r" (value));
}

/* 减 */
static inline void atomic_sub(Atomic *data,int value)
{
	__asm volatile("subl %1,%0"
			             : "+m" (data->value)
			             : "r" (value));
}


/* 自加或自减运算 */
static inline void atomic_inc(Atomic *data)
{
	__asm volatile("incl %0"
					: "+m" (data->value));
}
static inline void atomic_dec(Atomic *data)
{
	__asm volatile("decl %0"
					: "+m" (data->value));
}


/* 加或减后，返回新值 */
static inline int atomic_add_ret(Atomic *data,int value)
{
	atomic_add(data,value);
	return data->value;
}

static inline int atomic_sub_ret(Atomic *data,int value)
{
	atomic_sub(data,value);
	return data->value;
}

static inline int atomic_inc_ret(Atomic *data)
{
	atomic_inc(data);
	return data->value;
}
static inline int atomic_dec_ret(Atomic *data)
{
	atomic_dec(data);
	return data->value;
}


/* 位图操作 */
static inline void atomic_set_bit(Atomic *data,u8 bit)
{
	int mask = 1 << bit;
	__asm volatile("orl %0,%1"
				:
				: "r" (mask), "m" (data->value)
				: "memory");
}

static inline void atomic_clear_bit(Atomic *data,u8 bit)
{
	int mask = ~(1 << bit);
	__asm volatile("andl %0,%1"
				:
				: "r" (mask), "m" (data->value)
				: "memory");
}

static inline Bool atomic_test_bit(Atomic *data,u8 bit)
{
	int mask = 1 << bit;
	__asm volatile("andl %0,%1"
				:
				: "r" (data->value),"m" (mask)
				: "memory");
	return data->value;
}

#endif
