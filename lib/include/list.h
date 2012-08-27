#ifndef _LIST_H
#define _LIST_H
/*===============================================================
 *                        list.h
 *                	      双向链表
 ===============================================================*/

#include <lib/include/type.h>


/* 节点没有数据，仅仅起链接作用 */
typedef struct list_node
{
	struct list_node* prev;
	struct list_node* next;
}ListNode;


/* 链表头结构 */
typedef struct list_head
{

	struct list_node* prev;
	struct list_node* next;
}ListHead;


/* 双链表初始化 */
void list_init(ListHead *head);

/* 添加在开头 */
Bool list_add(ListHead *head,ListNode *node);
/* 添加在结尾 */
Bool list_addtail(ListHead *head,ListNode *node);
/* 搜索 */
ListNode* list_search(ListHead *head,int index);

/* 插入 */
Bool list_insert(ListHead *head,ListNode *node,int index);

/* 删除 */
Bool list_delete(ListHead *head,int index);

/* 解除链接 */
void list_unlink(ListNode *node);

/* 判断是否为空 */
Bool list_empty(ListHead *head);

#endif
