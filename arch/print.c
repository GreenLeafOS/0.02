/*
 * print.c
 *
 *  Created on: 2012-8-19
 *      Author: greenleaf
 */

#include "include/print.h"

int print_pos(char *str,int pos)
{
	int old_pos = disp_pos;
	disp_pos = pos;
	print(str);
	return (pos = disp_pos,disp_pos = old_pos);
}
