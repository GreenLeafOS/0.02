/*
 * mod_time.c
 *
 *  Created on: 2012-8-19
 *      Author: greenleaf
 */


#include "include/module.h"
#include "include/sysapi.h"



/* mod_time macro */
#define SetTimeInfo(value,len)							/* 显示字符串 */		\
	dbcd_to_str(value,(char*)&date_info[date_pos]);							\
	date_pos += len
#define TIMER_FREQ     1193182L		/* PC输入频率 */
#define HZ             100			/* 输出频率10ms级 */


/* mod_time data */
char day_of_week_table[8][14] = {	/* 星期字符串常量 */
		"",
		"  Sunday     ",
		"  Monday     ",
		"  Tuesday    ",
		"  Wednesday  ",
		"  Thursday   ",
		"  Friday     ",
		"  Saturday   "};
id_t mod_time_id;					/* 主线程id */
Time mod_time_sysclock;				/* 时钟信息 */


/*
 * mod_time_show
 * 参数：无
 * 功能：显示时间
 * 返回值：无
 */
void mod_time_show()
{
	char date_info[50] = "Date: ";
	int date_pos = 6;

	SetTimeInfo(cmos_info.DateCentury,2);	// 世纪
	SetTimeInfo(cmos_info.Year,2);			// 年
	date_info[date_pos++] = '-';
	SetTimeInfo(cmos_info.Month,2);			// 月
	date_info[date_pos++] = '-';
	SetTimeInfo(cmos_info.DayOfMonth,2);	// 日
	memcpy((void*)&date_info[date_pos],(void*)&day_of_week_table[cmos_info.DayOfWeek][0],13);	// 星期
	date_pos += 13;

	SetTimeInfo(cmos_info.Hours,2);			// 时
	date_info[date_pos++] = ':';

	SetTimeInfo(cmos_info.Minutes,2);		// 分
	date_info[date_pos++] = ':';

	SetTimeInfo(cmos_info.Seconds,2);		// 秒
	date_info[date_pos++] = '\n';

	print_pos((char*)&date_info,24*160 + 86);
}





/*
 * mod_time_read_cmos
 * 参数：无
 * 功能：读取cmos的时钟信息,并更新时钟信息
 * 返回值：无
 */
void mod_time_read_cmos()
{
	for(int i=0;i<10;i++)
	{
		outb(0x70,i);
		*(((u8*)&cmos_info) + i) = inb(0x71);
	}
	mod_time_sysclock.year = dbcd_to_int(cmos_info.DateCentury) * 100 + dbcd_to_int(cmos_info.Year);
	mod_time_sysclock.month = dbcd_to_int(cmos_info.Month);
	mod_time_sysclock.day_of_month = dbcd_to_int(cmos_info.DayOfMonth);
	mod_time_sysclock.day_of_week = dbcd_to_int(cmos_info.DayOfWeek);
	mod_time_sysclock.hours = dbcd_to_int(cmos_info.Hours);
	mod_time_sysclock.minutes = dbcd_to_int(cmos_info.Minutes);
	mod_time_sysclock.seconds = dbcd_to_int(cmos_info.Seconds);
	return;
}




/*
 * mod_time_do
 * 参数：无
 * 功能：处理消息
 * 返回值：无
 */
void mod_time_do(MsgHead msg)
{
	if(msg.vector == MSG_INTR_CLOCK)
	{
		mod_time_read_cmos();
		mod_time_show();
	}
	return;
}




/*
 * mod_time_main
 * 主循环
 */
void mod_time_main()
{
	/* 注册时钟中断 */
	msg_reg.param = MSG_INTR_CLOCK;
	msg_reg.sender = mod_time_id;
	msg_reg.receiver = mod_public_msg_id;
	post(msg_reg);

	/* 初始化8253 */
	outb(TIMER_MODE,RATE_GENERATOR);
	outb(TIMER0, (u8)(TIMER_FREQ/HZ));
	outb(TIMER0, (u8)((TIMER_FREQ/HZ) >> 8));

	mod_time_show();
	while(1)
	{
		MsgHead msg = recv();		// 获取消息
		mod_time_do(msg);			// 处理消息
	}
}
