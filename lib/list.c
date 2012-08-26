/*
 * list.c
 *
 *  Created on: 2012-8-11
 *      Author: greenleaf
 */

#include "include/list.h"

/*
 * 双链表初始化
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
 * 添加在开头
 * */
Bool list_add(ListHead *head,ListNode *node)
{
	if(head && head->next && node)
	{
		/* 新节点和头节点的后节点建立链接 */
		node->next = head->next;
		head->next->prev = node;

		/* 新节点和头节点建立链接 */
		node->prev = (ListNode*)head;
		head->next = node;

		return True;
	}
	return False;
}

/*
 * 添加在末尾
 */
Bool list_addtail(ListHead *head,ListNode *node)
{
	if(head && head->prev && node)
	{
		/* 新节点和头节点的前节点建立链接 */
		node->prev = head->prev;
		head->prev->next = node;

		/* 新节点和头节点建立链接 */
		node->next = (ListNode*)head;
		head->prev = node;

		return True;
	}
	return False;
}

/*
 * 搜索
 */
ListNode* list_search(ListHead *head,int index)
{
	ListNode *node;
	if(head == NULL) return NULL;

	/* 遍历链表 */
	for(node=(ListNode*)head;index >= 0;index--)
	{
		node = node->next;

		if(node == NULL || node == (ListNode*)head) return NULL;
	}

	return node;
}




/*
 * 插入
 * */
Bool list_insert(ListHead *head,ListNode *node,int index)
{
	ListNode *old_node = list_search(head,index);

	/* 测试指针是否有效 */
	if (head && old_node && node && old_node->next)
	{
		/* 新节点和旧节点的后节点建立链接 */
		node->next = old_node->next;
		old_node->next->prev = node;

		/* 新节点和旧节点建立链接 */
		node->prev = old_node;
		old_node->next = node;

		return True;
	}
	return False;
}



/*
 * 解除链接
 * */
void list_unlink(ListNode *node)
{
	if (node && node->next && node->prev)
	{
		node->prev->next = node->next;	/* 后一个元素的前指针指向前一个元素 */
		node->next->prev = node->prev;	/* 前一个元素的后指针指向后一个元素 */
	}
	return;
}


/*
 * 删除
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
 * 判断是否为空
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
