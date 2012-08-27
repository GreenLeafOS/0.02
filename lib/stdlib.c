/*
 * stdlib.c
 *
 *  Created on: 2012-8-18
 *      Author: greenleaf
 */


#include "include/stdlib.h"
#include "include/string.h"

void itoa(int number,char* str)
{
	int num=number;
	if(num == 0)
	{
		str[0] = '0';
		str[1] = '\0';
		return;
	}
	else if(num > 0)
	{
		int i=0;
		int k=0;
		for(;num > 0;i++)
		{
			if(num<10)
			{
				str[i]='0'+num;
				str[++i]='\0';
				break;
			}
			else
			{
				k=num%10;
				str[i]='0'+k;
				num/=10;
			}
		}
		for(int i=0,j=strlen(str)-1;i<strlen(str)/2+1;i++)
		{
			char ch;
			ch=str[i];
			str[i]=str[j-i];
			str[j-i]=ch;
		}
	}
	else
	{
		num=0-num;
		int i=0;
		int k=0;
		for(;num > 0;i++)
		{
			if(num<10)
			{
				str[i]= '0' + num;
				str[++i]='\0';
				break;
			}
			else
			{
				k=num%10;
				str[i]='0'+k;
				num/=10;
			}
		}
		char temp[20];
		strcpy(temp,"-");
		strcat(temp,str);
		strcpy(str,temp);
	}
	return;
}
