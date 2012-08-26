/*===============================================================
 *                       ldt.c
 *                	  LDT��ش�������
 ===============================================================*/
#include "include/table.h"

/************************************************************************/
/*							������
/************************************************************************/
Desc	ldt_table[8];
u8 		ldt_ptr[6];

/************************************************************************/
/*							��ʼ��
/*                           init
/************************************************************************/
void ldt_init()
{
	return;
}

/************************************************************************/
/*					   ldt�����������ʱ��������
/*                            ldt
/************************************************************************/


/****************************
 * ����
 * avl��־λΪ1��Ϊռ�ã�Ϊ0�����
 ****************************/
int ldt_add(Desc item)
{
	for(int i=1;i<8;i++)
	{
		Desc* buf = &ldt_table[i];
		if (!(buf->avl))
		{
			ldt_set(i,item);
			return i;
		}
	}
	return -1;
}



/****************************
 * ɾ��
 * ���avlλ����
 ****************************/
void ldt_remove(int index)
{
	Desc* buf = &ldt_table[index];
	buf->avl = 0;
}



/****************************
 * �޸���
 ****************************/
void ldt_set(int index,Desc item)
{
	Desc* buf = &ldt_table[index];
	*buf = item;
	buf->avl = 1;
}



/****************************
 * ��ȡ��
 ****************************/
void ldt_get(int index,Desc *item)
{
	Desc* buf = &ldt_table[index];
	*item = *buf;
}



/************************************************************************/
/*					 		���� ldt
/*                            ldt
/************************************************************************/
void ldt_load()
{
	u16* p_ldt_limit = (u16*)(&ldt_ptr[0]);
	u32* p_ldt_base  = (u32*)(&ldt_ptr[2]);
	char *pLdt = (char*)&ldt_ptr[0];

	*p_ldt_limit = 8 * sizeof(Desc) - 1;
	*p_ldt_base  = (u32)&ldt_table[0];

	__asm volatile(
			"movl %0,%%eax		\n"
			"lldt (%%eax)"
			:
			: "g"(pLdt));
}
