/*
 * setbit.h
 *
 *  Created on: 2012-8-6
 *      Author: greenleaf
 */

#ifndef SETBIT_H_
#define SETBIT_H_

#include <lib/include/type.h>

#define CHAR_BIT  (32)
#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)



#define BmpArrayDefine(name,datatype,max,type)	\
		type datatype name[max];					\
		type u32 name##_data[max/32]

#define BmpArray(name,max)	u32 name[max/32];

static inline void bmp_set(void *bmp,u32 bit)
{
	BITSET((u32*)bmp,bit);
}

static inline void bmp_clear(void *bmp,u32 bit)
{
	BITCLEAR((u32*)bmp,bit);
}

static inline u8 bmp_test(void *bmp,u32 bit)
{
	return BITTEST((u32*)bmp,bit);
}

#define Bmp ((u32*)bmp)
static inline int bmp_search(void *bmp,int max)
{
	for(int i=0;i<(max/32);i++)
	{
		u32* bmp_int = Bmp + i;
		if (*bmp_int == 0xffffffff) continue;
		for(int j=0;j<32;j++)
		{
			if (!(BITTEST(bmp_int,j)))
			{
				return ((i*32)+j);
			}
		}
	}
	return -1;
}

#endif /* SETBIT_H_ */
