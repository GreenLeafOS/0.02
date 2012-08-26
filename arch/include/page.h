/*
 * page.h
 *
 *  Created on: 2012-8-6
 *      Author: greenleaf
 */

#ifndef PAGE_H_
#define PAGE_H_

#include "memory.h"

/* ҳ��ҳĿ¼��ṹ */
typedef struct page_entry
{
	u32 p			: 1;			/* Pλ Bit0 */
	u32 rw			: 1;			/* R/Wλ Bit1*/
	u32 us			: 1;			/* U/Sλ Bit2*/
	u32	pwt			: 1;			/* �������λ Bit3*/
	u32 pcd			: 1;			/* �������λ Bit4*/
	u32 a			: 1;			/* Aλ Bit5*/
	u32 d			: 1;			/* Dλ Bit6*/
	u32	ps			: 1;			/* ҳ��С Bit7*/
	u32 g			: 1;			/* ָʾȫ��ҳ Bit8*/
	u32 avl			: 3;			/* ϵͳʹ�ñ���λ Bit9~Bit11 */
	u32 addr		: 20;			/* �����ַ��20λ Bit12~Bit31*/
}PageEntry;


#define PDE_ALLOCATED		1		/* �Ѿ����� */

/*ҳ��ҳĿ¼�ṹ*/
typedef struct page_table
{
	PageEntry items[PAGE_SIZE/sizeof(PageEntry)];
}PageTable;


/* ��ʼ�� */
void page_init();


/* ҳĿ¼����ҳ��ҳ��ӳ�䵽ҳĿ¼�� */
void page_link_table(PageTable* page_dir,PageTable* page_tbl,u16 linear_addr);
/* ҳ�����ӵ�ַ�����Ե�ַӳ�䵽�����ַ�� */
void page_link_addr(PageTable* page_tbl,u32 phy_page_addr,u16 linear_addr);
/* ֱ��ӳ��(��ҳĿ¼������л�ȡҳ���ַ) */
int page_link(PageTable* page_dir,u32 phy_page_addr,u32 linear_addr);


/* ����ҳĿ¼ */
void page_directory_load(u32 phy_addr);



/*
 * ��д����֮ǰ����������һ���ˡ�
 * ����: 4 b
 * ҳĿ¼:4 kb = 4096 b
 * ҳ��: 4 kb = 4096 b
 * ���Ե�ַ�ĸ�10λ��ҳĿ¼���ƫ��
 * ��Ϊ����4kb����������PDE�ڱ��е�ƫ�Ʋ��ᳬ��4kb,��4096b,����ֻ��Ҫ12λ���ɡ�
 * ��Ϊ������4b������PDE�ڱ��е�ƫ���ܻ���4�ı�������ƫ�Ƶĵ�2λ����0�����ڿ�����10λ�㹻�ˡ�
 * ��ô������16λƫ��Ӧ���� 10λƫ������2λ����λ��0�õ���
 * ��������10λ����ƫ������� 16λƫ�����ƣ���λ�ضϵõ���
 * ��ʱ��10λƫ��ǡ����������±ꡣ���ú����룬�Ͷ��ˣ�
 * ��ҳ��Ԫ��Ѱַʱ�����Կ�����ִ������ôһ������(C����α����)
 * bits������������ȡ��Ӧλ��bit10ָ10λ�����Դ����ơ�
 * u32 linear_to_phy(u32 linear)
 * {
 *		bit10 high = bits(linear,22,31);
 *		bit10 mid = bits(linear,12,21);
 *		bit12 low = bits(linear,0,11);
 *
 *		pde = page_dir->item[high];
 *		page_tbl = (pde->addr) << 12;
 *		pte = page_tbl->item[mid];
 *		phy_page = (pte->addr) << 12;
 *		phy_addr = phy_page + low;
 *		return phy_addr;
 * }
 *
 * �˽��������д�����ǲ��������������أ�
 * �������
 * 		x /256bx 0x16000
 * 		b 0x10bd4
 * 		b 0x10c07
 *
 * */



/*
 * ebp - 20 : *page_dir
 * ebp - 28 : i
 * ebp - 16 : *page_tbl
 * ebp - 24 : j
 * ����
 * esp + 8  : i
 * esp + 4  : *page_tbl
 * esp + 0  : *page_dir
 * ����
 * esp + 8  : j
 * esp + 4  : phy_addr
 * esp + 0  : *page_tbl
 */


/*
 * ҳĿ¼����ҳ��ҳ��ӳ�䵽ҳĿ¼��
 * ������
 * 		page_dir ҳĿ¼��ָ��
 * 		page_tbl ҳ��ָ�롣ȡ��20λ
 * 		linear_index	ҳ��ӳ�䵽�ĸ�λ��10λ��ַ,ȡ��10λ����λ������
 * */
#define page_link_table(page_dir,page_tbl,linear_index)	\
	((PageTable*)page_dir)->items[(u16)linear_index & 0x3FF].addr = (u32)((page_tbl)>>12);


/* ҳ�����ӵ�ַ�����Ե�ַӳ�䵽�����ַ��
 * ������
 * 		page_tbl ҳ��ָ��
 * 		phy_page_addr ����ҳ�ĵ�ַ.ȡ��20λ
 * 		linear_index	ҳ��ӳ�䵽�ĸ�λ��10λ��ַ,ȡ��10λ����λ������
 * */
#define page_link_addr(page_tbl,phy_page_addr,linear_index) \
	((PageTable*)page_tbl)->items[(u16)linear_index & 0x3FF].addr = (u32)((phy_page_addr)>>12);



#endif /* PAGE_H_ */
