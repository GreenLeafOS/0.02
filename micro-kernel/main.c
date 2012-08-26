/*
 * main.c
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */

#include "include/main.h"
#include "include/sysapi.h"
#include "include/irq.h"
#include "include/module.h"

/* main data */
u32* kernel_stack_top;
int kernel_reenter;



/*
 * 内核主函数
 */
void kernel_main()
{
	char *str = "\n\n\nGreenLeafOS version 0.02.\n";
	print(str);

	kernel_stack_top = (u32*)&tss.esp0;
	kernel_reenter = 0;
	thread_init();

	main_thread_create();

	thread_schedule();
	restart();

	while(1);
}





/*
 * kernel_main_thread
 * 主线程
 */
void kernel_main_thread()
{
	while(1)
	{
		char* in = "m";
		for(int i=0;i<1000000;i++);
	//	print(in);
	}
	while(1);
}



/*
 * test
 */
int test_id;
void test_main()
{
	char *node = "test.\n";
	print(node);

	Handle handle_room,handle_block;
	struct _body_create body1 =
	{
			0,
			&handle_room,
			NULL
	};
	MsgHead msg =
	{
			MSG_REQUEST_CREATE,
			MSG_PRIORITY_KERNEL,
			test_id,
			mod_addr_room_id,
			&body1,
			sizeof(body1),
			0
	};
	post(msg);
	recv();

	char str1[3];
	char str2[3];
	char *info[2] = {"new linear address space.  handle:"
					,"\nnew linear block.          handle:"};

	itoa(handle_room,(char*)&str1);
	print(info[0]);
	print(str1);


	LinearBlock block;
	block.start = 0;

	block.block = (PhyPage*)Addr_To_Page(alloc(45));
//	block.block = page_alloc(4);
	u8 *ch = (u8*)Page_To_Addr(block.block);
	*ch = 'a';

	body1.type = 1;
	body1.ret = &handle_block;
	body1.init = &block;

	post(msg);
	recv();


	itoa(handle_block,(char*)&str2);
	print(info[1]);
	print(str2);


	msg.vector = MSG_ADDRROOM_ADD;
	struct _body_addr_room_add_del body2 =
	{
		handle_room,handle_block
	};
	msg.body_point = &body2;
	msg.body_size = sizeof(body2);
	post(msg);
	recv();


	struct _body_addr_room_switch body3 = {handle_room};
	msg.vector = MSG_ADDRROOM_SWITCH;
	msg.body_point = &body3;
	msg.body_size = sizeof(body3);

//	post(msg);
	recv();

	while(1);
}


/*
 * main_thread_create
 * 创建线程
 */
void main_thread_create()
{
	KernelThread* thread;

	/* 建立线程 */
	CreateThread(mod_public_msg,	thread,		thread_default_reg);
	CreateThread(mod_time,			thread,		thread_default_reg);
	CreateThread(mod_addr_room,		thread,		thread_default_reg);
	CreateThread(test,				thread,		thread_default_reg);
}
