#ifndef IO_H_
#define IO_H_

#include <lib/include/type.h>
/*===============================================================
 *                      io.h
 *                	    io����
 ===============================================================*/

/* �ֽڲ��� 8λ */
static inline void outb(u16 port,u8 v)
{
	__asm volatile("outb %0,%1" : : "a" (v), "dN" (port));
}

static inline u8 inb(u16 port)
{
	u8 v;
	__asm volatile("inb %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

/* �ֲ��� 16λ */
static inline void outw(u16 port,u16 v)
{
	__asm volatile("outw %0,%1" : : "a" (v), "dN" (port));
}

static inline u16 inw(u16 port)
{
	u16 v;
	__asm volatile("inw %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

/* ˫�ֲ��� 32λ */
static inline void outl(u16 port,u32 v)
{
	__asm volatile("outl %0,%1" : : "a" (v), "dN" (port));
}

static inline u32 inl(u32 port)
{
	u32 v;
	__asm volatile("inl %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

/* �ӳٺ��� */
static inline void io_delay(void)
{
	const u16 DELAY_PORT = 0x80;
	__asm volatile("outb %%al,%0" : : "dN" (DELAY_PORT));

}

#define TIMER0         0x40 /* I/O port for timer channel 0 */
#define TIMER_MODE     0x43 /* I/O port for timer mode control */
#define RATE_GENERATOR 0x34 /* 00-11-010-0 :
 	 	 	 	 	 	 	 * Counter0 - LSB then MSB - rate generator - binary */
#endif /* IO_H_ */
