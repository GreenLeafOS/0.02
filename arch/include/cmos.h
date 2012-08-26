/*
 * cmos.h
 *
 *  Created on: 2012-8-18
 *      Author: greenleaf
 */

#ifndef CMOS_H_
#define CMOS_H_

#include <lib/include/type.h>

#pragma pack(1)

struct hdinfo
{
	unsigned short Cylinder;
	unsigned char head;
	unsigned short WP;
	unsigned short LZ;
	unsigned char Sector;	//扇区数
};

struct cmos_t
{
	unsigned char Seconds;			//实时钟秒数
	unsigned char SecondAlarm;		//秒报警
	unsigned char Minutes;			//实时钟分数
	unsigned char MinuteAlarm;		//分报警
	unsigned char Hours;			//实时钟小时数
	unsigned char HourAlarm;		//时报警
	unsigned char DayOfWeek;		//周几
	unsigned char DayOfMonth;		//号数
	unsigned char Month;			//月份
	unsigned char Year;				//年份
	unsigned char StatusRegA;		//状态寄存器A
	unsigned char StatusRegB;		//状态寄存器B
	unsigned char StatusRegC;		//状态寄存器C
	unsigned char StatusRegD;		//状态寄存器D
	unsigned char DiagStatus;		//诊断状态
	unsigned char ShutDownStatus;	//关机状态
	struct
	{
		unsigned char FloppyDrive2 : 4;	//第二软驱的类型
		unsigned char FloppyDrive1 : 4;	//第一软驱的类型
	}fd;
	unsigned char Reserved1;			//保留
	struct
	{
		unsigned char FixedDrive2 : 4;	//第二硬盘的类型
		unsigned char FixedDrive1 : 4;	//第一硬盘的类型
	}hd;
	unsigned char Reserved2;		//保留
	struct
	{
		unsigned char diskdrv : 1;		//磁盘驱动器
		unsigned char mpu : 1;			//数学处理器
		unsigned char : 2;				//保留
		unsigned char videotype : 2;		//当前显示类型
		unsigned char numdrive : 2;		//磁盘驱动器的数目
	}equipment;
	unsigned short RAM;				//基本内存KB数
	unsigned short XMS;				//扩充内存KB数
	unsigned char FixedDriveType1;	//驱动器C扩展字节
	unsigned char FixedDriveType2;	//驱动器D扩展字节
	unsigned short Reserved3;			//保留
	struct hdinfo hd1;				//硬盘C的物理参数
	struct hdinfo hd2;				//硬盘D的物理参数
	unsigned char Sys;				//系统字节
	unsigned short CheckSum;		//校验和
	unsigned short XMS1;			//扩展内存KB数
	unsigned char DateCentury;		//世纪的BCD值
	unsigned char InfoFlags;		//信息标志
	unsigned char Reserved4[4];		//保留
	unsigned char password[8];		//口令字节
};

#pragma pack ()

extern struct cmos_t cmos_info;

#endif /* CMOS_H_ */
