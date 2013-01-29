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
	unsigned char Sector;	//������
};

struct cmos_t
{
	unsigned char Seconds;			//ʵʱ������
	unsigned char SecondAlarm;		//�뱨��
	unsigned char Minutes;			//ʵʱ�ӷ���
	unsigned char MinuteAlarm;		//�ֱ���
	unsigned char Hours;			//ʵʱ��Сʱ��
	unsigned char HourAlarm;		//ʱ����
	unsigned char DayOfWeek;		//�ܼ�
	unsigned char DayOfMonth;		//����
	unsigned char Month;			//�·�
	unsigned char Year;				//���
	unsigned char StatusRegA;		//״̬�Ĵ���A
	unsigned char StatusRegB;		//״̬�Ĵ���B
	unsigned char StatusRegC;		//״̬�Ĵ���C
	unsigned char StatusRegD;		//״̬�Ĵ���D
	unsigned char DiagStatus;		//���״̬
	unsigned char ShutDownStatus;	//�ػ�״̬
	struct
	{
		unsigned char FloppyDrive2 : 4;	//�ڶ�����������
		unsigned char FloppyDrive1 : 4;	//��һ����������
	}fd;
	unsigned char Reserved1;			//����
	struct
	{
		unsigned char FixedDrive2 : 4;	//�ڶ�Ӳ�̵�����
		unsigned char FixedDrive1 : 4;	//��һӲ�̵�����
	}hd;
	unsigned char Reserved2;		//����
	struct
	{
		unsigned char diskdrv : 1;		//����������
		unsigned char mpu : 1;			//��ѧ������
		unsigned char : 2;				//����
		unsigned char videotype : 2;		//��ǰ��ʾ����
		unsigned char numdrive : 2;		//��������������Ŀ
	}equipment;
	unsigned short RAM;				//�����ڴ�KB��
	unsigned short XMS;				//�����ڴ�KB��
	unsigned char FixedDriveType1;	//������C��չ�ֽ�
	unsigned char FixedDriveType2;	//������D��չ�ֽ�
	unsigned short Reserved3;			//����
	struct hdinfo hd1;				//Ӳ��C���������
	struct hdinfo hd2;				//Ӳ��D���������
	unsigned char Sys;				//ϵͳ�ֽ�
	unsigned short CheckSum;		//У���
	unsigned short XMS1;			//��չ�ڴ�KB��
	unsigned char DateCentury;		//���͵�BCDֵ
	unsigned char InfoFlags;		//��Ϣ��־
	unsigned char Reserved4[4];		//����
	unsigned char password[8];		//�����ֽ�
};

#pragma pack ()

extern struct cmos_t cmos_info;

#endif /* CMOS_H_ */
