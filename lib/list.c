/*
 * list.c
 *
 *  Created on: 2012-8-11
 *      Author: greenleaf
 */

#include "include/list.h"

/*
 * ˫�����ʼ��
 * */
void list_init(ListHead *head)
{
	if (head)
	{
		head->prev = (ListNode*)head;
		head->next = (ListNode*)head;
	}
	return;
}



/*
 * ����ڿ�ͷ
 * */
Bool list_add(ListHead *head,ListNode *node)
{
	if(head && head->next && node)
	{
		/* �½ڵ��ͷ�ڵ�ĺ�ڵ㽨������ */
		node->next = head->next;
		head->next->prev = node;

		/* �½ڵ��ͷ�ڵ㽨������ */
		node->prev = (ListNode*)head;
		head->next = node;

		return True;
	}
	return False;
}

/*
 * �����ĩβ
 */
Bool list_addtail(ListHead *head,ListNode *node)
{
	if(head && head->prev && node)
	{
		/* �½ڵ��ͷ�ڵ��ǰ�ڵ㽨������ */
		node->prev = head->prev;
		head->prev->next = node;

		/* �½ڵ��ͷ�ڵ㽨������ */
		node->next = (ListNode*)head;
		head->prev = node;

		return True;
	}
	return False;
}

/*
 * ����
 */
ListNode* list_search(ListHead *head,int index)
{
	ListNode *node;
	if(head == NULL) return NULL;

	/* �������� */
	for(node=(ListNode*)head;index >= 0;index--)
	{
		node = node->next;

		if(node == NULL || node == (ListNode*)head) return NULL;
	}

	return node;
}




/*
 * ����
 * */
Bool list_insert(ListHead *head,ListNode *node,int index)
{
	ListNode *old_node = list_search(head,index);

	/* ����ָ���Ƿ���Ч */
	if (head && old_node && node && old_node->next)
	{
		/* �½ڵ�;ɽڵ�ĺ�ڵ㽨������ */
		node->next = old_node->next;
		old_node->next->prev = node;

		/* �½ڵ�;ɽڵ㽨������ */
		node->prev = old_node;
		old_node->next = node;

		return True;
	}
	return False;
}



/*
 * �������
 * */
void list_unlink(ListNode *node)
{
	if (node && node->next && node->prev)
	{
		node->prev->next = node->next;	/* ��һ��Ԫ�ص�ǰָ��ָ��ǰһ��Ԫ�� */
		node->next->prev = node->prev;	/* ǰһ��Ԫ�صĺ�ָ��ָ���һ��Ԫ�� */
	}
	return;
}


/*
 * ɾ��
 * */
Bool list_delete(ListHead *head,int index)
{
	ListNode *old_node = list_search(head,index);
	if (old_node)
	{
		list_unlink(old_node);
		return True;
	}
	return False;
}



/*
 * �ж��Ƿ�Ϊ��
 * */
Bool list_empty(ListHead *head)
{
	if (head == NULL) return True;

	Bool isnull = (head->next == NULL || head->prev == NULL);
	Bool isempty = (head->next == (ListNode*)head && head->next == (ListNode*)head);

	if (isnull || isempty)
		return True;
	else
		return False;
}
