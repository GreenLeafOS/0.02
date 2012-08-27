#ifndef _LIST_H
#define _LIST_H
/*===============================================================
 *                        list.h
 *                	      ˫������
 ===============================================================*/

#include <lib/include/type.h>


/* �ڵ�û�����ݣ��������������� */
typedef struct list_node
{
	struct list_node* prev;
	struct list_node* next;
}ListNode;


/* ����ͷ�ṹ */
typedef struct list_head
{

	struct list_node* prev;
	struct list_node* next;
}ListHead;


/* ˫�����ʼ�� */
void list_init(ListHead *head);

/* ����ڿ�ͷ */
Bool list_add(ListHead *head,ListNode *node);
/* ����ڽ�β */
Bool list_addtail(ListHead *head,ListNode *node);
/* ���� */
ListNode* list_search(ListHead *head,int index);

/* ���� */
Bool list_insert(ListHead *head,ListNode *node,int index);

/* ɾ�� */
Bool list_delete(ListHead *head,int index);

/* ������� */
void list_unlink(ListNode *node);

/* �ж��Ƿ�Ϊ�� */
Bool list_empty(ListHead *head);

#endif
