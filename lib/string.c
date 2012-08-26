#include "include/string.h"

void *memcpy(void *dst,const void *src,size_t count)
{
	void *ret = dst;
	while (count--)
	{
		*(char*)dst++ = *(char*)src++;
	}
	return ret;
}


int memcmp(const void *buffer1,const void *buffer2,size_t count)
{
	if (!count)
		return(0);
	while(--count && *(char *)buffer1 == *(char *)buffer2)
	{
		buffer1 = (char *)buffer1 + 1;
		buffer2 = (char *)buffer2 + 1;
	}
	return( *((unsigned char *)buffer1) - *((unsigned char *)buffer2) );
}


char *strcpy(char *dst,const char *src)
{
	void *ret = dst;
	while (*src != '\0')
	{
		*dst++ = *src++;
	}
	*(dst + 1) = '\0';
	return ret;
}



char *strcat(char *dst,const char *src)
{
	void *ret = dst;
	while (*dst != '\0') dst++;
	while ((*dst++ = *src++) != '\0');
	*((char*)dst + 1) = '\0';
}
