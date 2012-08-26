/*
 * bcd.h
 *
 *  Created on: 2012-8-19
 *      Author: greenleaf
 */

#ifndef BCD_H_
#define BCD_H_


char* dbcd_to_str(unsigned char bcd,char* dst);
char* abcd_to_str(unsigned char bcd,char* dst);

int	dbcd_to_int(unsigned char bcd);

#endif /* BCD_H_ */
