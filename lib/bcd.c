/*
 * bcd.c
 *
 *  Created on: 2012-8-19
 *      Author: greenleaf
 */


#include "include/bcd.h"

char* dbcd_to_str(unsigned char bcd,char* dst)
{
	*(dst+1) = (bcd & 0xf) + '0';
	*dst = (bcd >> 4) + '0';
	return dst;
}

int	dbcd_to_int(unsigned char bcd)
{
	/*         个位            十位       */
	return (bcd & 0xf) + ((bcd >> 4) * 10);
}
