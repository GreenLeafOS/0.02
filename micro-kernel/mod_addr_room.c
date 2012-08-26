/*
 * mod_addr_room.c
 *
 *  Created on: 2012-8-20
 *      Author: greenleaf
 */


#include "include/sysapi.h"
#include "include/module.h"

/* addr_room  macro */
#define UserLinearSpacePages		524288		// 2G



/* addr_room data */
id_t 			mod_addr_room_id;

hash_info 		mod_addr_room_hash;
hash_info 		mod_addr_block_hash;

LinearRoom		mod_addr_room_base[256];
LinearBlock		mod_addr_block_base[1024];


/*
 * mod_addr_room_create
 * �������Ե�ַ�ռ�
 */
Handle mod_addr_room_create()
{
	Handle ret = new();
	LinearRoom room,*item;
	hash_set(&mod_addr_room_hash,ret,&room);
	item = hash_getaddr(&mod_addr_room_hash,ret);

	list_init(&item->head);
	item->free_count = UserLinearSpacePages;
	item->table = (PageTable*)alloc(0);

	return ret;
}




/*
 * mod_addr_room_alloc
 * �������Կ�
 */
Handle mod_addr_room_alloc(LinearBlock *block)
{
	Handle ret = new();
	LinearBlock new_block;
	if(block != NULL) new_block = *block;
	hash_set(&mod_addr_block_hash,ret,&new_block);

	return ret;
}




/*
 * mod_addr_room_add
 * ����ڴ����󵽵�ַ�ռ�
 */
Bool mod_addr_room_add(Handle addr_room,Handle linear_block)
{
	int i=0,pages=1;

	LinearBlock *other;

	/* ��ȡlinear_block��ָ�� */
	LinearBlock *block = (LinearBlock*)hash_getaddr(&mod_addr_block_hash,linear_block);

	/* ��ȡaddr_room��ָ�� */
	LinearRoom *room = (LinearRoom*)hash_getaddr(&mod_addr_room_hash,addr_room);

	/* �¿����ʼ�ͽ��� */
	int start = block->start;
	int end = block->start + block->block->private;


	/* ֻҪ��õ�ָ�벻ΪNULL�������¿�λ���Ƿ���Ч */
	while(other = (LinearBlock*)list_search(&room->head,i))
	{
		/* ���п����ʼ�ͽ��� */
		int other_start = other->start;
		int other_end = other->start + other->block->private;

		if ((other_start >= start && other_start <= end) ||	/* ���п����ʼ�����¿�����֮�� */
			(other_end >= start   && other_end <= end))		/* ���п�Ľ��������¿�����֮�� */
		{
			return False;
		}
		i++;
	}

	/* û�з��أ���ֱ�Ӳ���˫������ */
	list_add(&room->head,&block->node);

	/* ��������С */
	OrderToPages(block->block->private,pages);
	room->free_count -= pages;

	return True;
}




/*
 * mod_addr_room_delete
 * �ӵ�ַ�ռ���ɾ������
 */
void mod_addr_room_del(Handle addr_room,Handle linear_block)
{
	/* ��ȡlinear_block��ָ�� */
	LinearBlock *block = (LinearBlock*)hash_getaddr(&mod_addr_block_hash,linear_block);

	/* ��ȡaddr_room��ָ�� */
	LinearRoom *room = (LinearRoom*)hash_getaddr(&mod_addr_room_hash,addr_room);

	list_unlink(&block->node);
}





/*
 * mod_addr_room_switch
 * �л���ַ�ռ�
 */
void mod_addr_room_switch(Handle addr_room)
{
	LinearBlock *block;		// -32
	/* ��ȡaddr_room��ָ�� */
	LinearRoom *room = (LinearRoom*)hash_getaddr(&mod_addr_room_hash,addr_room);	// -28


	/* ֻҪ��õ�ָ�벻ΪNULL��ѭ�������� */
	int i=0;	// -12
	while(block = (LinearBlock*)list_search(&room->head,i++))
	{
		/* �����û��ӳ��� */
		if (!(bmp_test(&block->flags,LB_USED)))
		{
			int max = 1;		// -16
			OrderToPages(block->block->private,max);
			for(int j=0;j<max;j++)								// -20
			{
				u32 page_index = (block->start + j) & 0x3FF;	/* ȡ��ҳ�ţ���λ���� */					// -36
				/* ҳĿ¼���±������ʼҳ�ų���һ�������ӳ���ҳ�� */
				PageEntry *pde = (PageEntry*)&room->table->items[page_index/1024];	// -40
				PageTable *tbl = (PageTable*)Index_To_Addr(pde->addr);				// -24
				PageEntry *pte;	// -36

				/* ���ҳ��û�з��� */
				if (pde->avl != PDE_ALLOCATED)
				{
					/* �����Ϸ���һ��ҳ */
					tbl = alloc(0);
					pde->avl = PDE_ALLOCATED;
					pde->addr = Addr_To_Index(tbl);
					pde->p = 1;
				}

				/* ҳ������±������ʼҳ�ų���һ�������ӳ���ҳ�������� */
				pte = (PageEntry*)&tbl->items[page_index%1024];

				/* ӳ�� */
				u32 phy = Page_To_Index(block->block + j);	// -44
				pte->addr = phy;
				pte->p = 1;
				pte->avl = PDE_ALLOCATED;
			}
		}
		bmp_clear(&block->flags,LB_USED);
	}
	/* ����ҳ�� */
	__asm volatile(
			"movl	%0,%%eax		\n"
			"movl	%%eax,%%cr3		\n"
			"movl	%%cr0,%%eax		\n"
			: : "g"(room->table)
			);
}




/*
 * mod_addr_room_do
 * ������Ϣ
 */
void mod_addr_room_do(MsgHead msg)
{
	switch(msg.vector)
	{
		/* ������Դ */
		case MSG_REQUEST_CREATE:
		{
			struct _body_create* body = (struct _body_create*)msg.body_point;
			/* ���Ե�ַ�ռ� */
			if (body->type ==  MSG_ADDRROOM_CREATE_ROOM)
			{
				*body->ret = mod_addr_room_create();
				goto Success;
			}
			/* ���Կ� */
			else
			{
				*body->ret = mod_addr_room_alloc((LinearBlock*)body->init);
				goto Success;
			}
		}
		break;


		/* ��� */
		case MSG_ADDRROOM_ADD:
		{
			struct _body_addr_room_add_del* body = (struct _body_addr_room_add_del*)msg.body_point;
			if (mod_addr_room_add(body->room,body->block))
				goto Success;
			else
				goto Fail;

		}
		break;

		/* ɾ�� */
		case MSG_ADDRROOM_DEL:
		{
			struct _body_addr_room_add_del* body = (struct _body_addr_room_add_del*)msg.body_point;
			mod_addr_room_del(body->room,body->block);
			goto Success;
		}
		break;


		/* ɾ�� */
		case MSG_ADDRROOM_SWITCH:
		{
			struct _body_addr_room_switch* body = (struct _body_addr_room_switch*)msg.body_point;
			mod_addr_room_switch(body->room);
			goto Success;
		}
		break;
	}
	return;

Success:
	msg_ok.sender = mod_addr_room_id;
	msg_ok.receiver = msg.sender;
	post(msg_ok);
	return;

Fail:
	msg_fail.sender = mod_addr_room_id;
	msg_fail.receiver = msg.sender;
	post(msg_fail);
	return;

}



/*
 * mod_addr_room_main
 * ��ѭ��
 */
void mod_addr_room_main()
{
	char* str = "linear address space.\n";
	print(str);
	hash_create(&mod_addr_room_hash,(char*)&mod_addr_room_base,256*sizeof(LinearRoom),sizeof(LinearRoom),NULL);
	hash_create(&mod_addr_block_hash,(char*)&mod_addr_block_base,256*sizeof(LinearBlock),sizeof(LinearBlock),NULL);
	while(1)
	{
		MsgHead msg = recv();		/* ��ȡ��Ϣ */
		mod_addr_room_do(msg);		/* ������Ϣ */
	}
}



