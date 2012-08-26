/*
 * page.c
 *
 *  Created on: 2012-8-6
 *      Author: greenleaf
 */

#include "include/page.h"


/* ֱ��ӳ��(��ҳĿ¼������л�ȡҳ���ַ)
 * ������
 * 		page_dir ҳĿ¼��ָ��
 * 		phy_page_addr ����ҳ�ĵ�ַ
 * 		linear_addr	ҳ��ӳ�䵽�����Ե�ַ��λ��10λ��ַ,ȡ��10λ����λ������
 * 	����ֵ��
 * 		0��ʾ�ɹ���-1��ʾҳ������
 * */
int page_link(PageTable* page_dir,u32 phy_page_addr,u32 linear_addr)
{
	u16 addr_high = linear_addr >> 22;						/* ���Ե�ַ��10λ */
	u16 addr_mid =  (linear_addr >> 12) & 0x3FF;			/* ���Ե�ַ��10λ */

	if (page_dir->items[addr_high].p == 1) return -1;		/* ҳ������ */

	u32 page_tbl_addr = page_dir->items[addr_high].addr;	/* ��ȡҳ���ַ��20λ */
	PageTable* page_tbl = (PageTable*)(page_tbl_addr << 12);	/* ����ҳ��ָ�� */
	page_tbl->items[addr_mid].addr = phy_page_addr;			/* ӳ�� */

	return 0;
}


/* ����ҳĿ¼
 * ������
 * 		phy_addr ҳĿ¼�������ַ
 * */
void page_directory_load(u32 phy_addr)
{
	__asm volatile(
			"movl	%0,%%eax		\n"
			"movl	%%eax,%%cr3		\n"
			"movl	%%cr0,%%eax		\n"
			"orl	$0x80000000,%%eax	\n"
			"movl	%%eax,%%cr0		\n"
			: : "g"(phy_addr)
			);
}


