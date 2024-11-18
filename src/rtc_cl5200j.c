/*****************************************************************************
|*			
|*  Copyright		:	(c) 2014 CAS
|*  Filename		:	rtc_cl5200j.c
|*  Version			:	0.1
|*  Programmer(s)	:	CJK
|*  Created			:	2014/12/01
|*  Modified		:
|*  Description	:	
|*				
****************************************************************************/

#include <stdio.h>
#include "globals.h"

#ifdef CPU_PIC32MZ

#include "rtc.h"
#include "i2c_cl5200j.h"
#include "common.h"

RTC_TIME time;
RTC_DATE date;

NEARDATA RTC_STRUCT RTCStruct;//HYP 20060412 _huge --> _near for small code size

#ifdef USE_MORNITORING_CNT
extern void rtc_set_pre_minute(void);
#endif

#define RTC_RX8010SJ_CMD_SEC   0x10
#define RTC_RX8010SJ_CMD_MIN   0x11
#define RTC_RX8010SJ_CMD_HOUR  0x12
#define RTC_RX8010SJ_CMD_WEEK  0x13
#define RTC_RX8010SJ_CMD_DAY   0x14
#define RTC_RX8010SJ_CMD_MONTH 0x15
#define RTC_RX8010SJ_CMD_YEAR  0x16


void RTC_AppInit(void)
{
    int i;
    char initData[16] = {0x50, 0x59, 0x23, 0x20, 0x07, 0x11, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    char buf[10];

	External_Interrupt_Disable();
    I2C_ReadData(RTC_RX8010SJ_CMD_SEC, buf, 7); // dummy read
	External_Interrupt_Enable();

    // set default time
    //I2C_WriteData(0x10, initData, 16);
}

void RTC_GetDate(INT8U type, RTC_DATE *date)
{
	INT8U buf[4];
	INT8U week;
	
	External_Interrupt_Disable();
	I2C_ReadData(RTC_RX8010SJ_CMD_WEEK, (char*)buf, 4);
	External_Interrupt_Enable();

	switch (buf[0]) // weekday
	{
		default:
		case 0x01: week = 0; break; // Sunday
		case 0x02: week = 1; break; // Monday
		case 0x04: week = 2; break; // Tuesday
		case 0x08: week = 3; break; // Wednesday
		case 0x10: week = 4; break; // Thursday
		case 0x20: week = 5; break; // Friday
		case 0x40: week = 6; break; // Saturday
	}

	if (type == BCD)
	{
		date->weekday = week;
		date->day = buf[1];
		date->month = buf[2];
		date->year = buf[3];
	}
	else
	{
		date->weekday = week;
		date->day = bcd2hex(buf[1]);
		date->month = bcd2hex(buf[2]);
		date->year = bcd2hex(buf[3]);
	}
}

void RTC_GetTime(INT8U type, RTC_TIME *time)
{
	INT8U buf[4];

	External_Interrupt_Disable();
	I2C_ReadData(RTC_RX8010SJ_CMD_SEC, (char*)buf, 3);
	External_Interrupt_Enable();

	if (type == BCD)
	{
		time->seconds = buf[0];
		time->minutes = buf[1];
		time->hours = buf[2];
	}
	else
	{
		time->seconds = bcd2hex(buf[0]);
		time->minutes = bcd2hex(buf[1]);
		time->hours = bcd2hex(buf[2]);
	}
}

INT8U weekdayTable_rx8010sj[7] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40}; // SUN, MON, TUE, ..., SAT
void RTC_SetDate(RTC_DATE date) // by hex : CL5500과 호환을 위해 hex로 입력받아 bcd로 저장함
{
	INT8U buf[4];
	INT8U week;

	if (date.weekday > 6) date.weekday = 0;// SUN
	week = weekdayTable_rx8010sj[date.weekday];

	buf[0] = week;
	buf[1] = hex2bcd(date.day);
	buf[2] = hex2bcd(date.month);
	buf[3] = hex2bcd(date.year);

	External_Interrupt_Disable();
	I2C_WriteData(RTC_RX8010SJ_CMD_WEEK, (char*)buf, 4);
	External_Interrupt_Enable();
}

void RTC_SetTime(RTC_TIME time) // by hex : CL5500과 호환을 위해 hex로 입력받아 bcd로 저장함
{
	INT8U buf[3];

	buf[0] = hex2bcd(time.seconds);
	buf[1] = hex2bcd(time.minutes);
	buf[2] = hex2bcd(time.hours);

	External_Interrupt_Disable();
	I2C_WriteData(RTC_RX8010SJ_CMD_SEC, (char*)buf, 3);
	External_Interrupt_Enable();
}

void RTC_CLK_Write(INT8U clk_addr, INT8U data)
{
	INT8U cmd;

	switch(clk_addr)
	{
		case 3: // date
			cmd = RTC_RX8010SJ_CMD_DAY;
			break;
		case 4: // month
			cmd = RTC_RX8010SJ_CMD_MONTH;
			break;
		case 5: // week day
			cmd = RTC_RX8010SJ_CMD_WEEK;
			if (data > 6) data = 0;// SUN
			data = weekdayTable_rx8010sj[data];
			break;
		case 6: // year
			cmd = RTC_RX8010SJ_CMD_YEAR;
			break;
		default:
			return;
	}
	External_Interrupt_Disable();
	I2C_WriteData(cmd, (char*)&data, 1);
	External_Interrupt_Enable();
}

void RTC_CLK_Burst_Write(void)
{
	date.day = bcd2hex(RTCStruct.date); 						 /* set day */
	date.weekday = bcd2hex(RTCStruct.day);						/* set weekday */
	date.month = bcd2hex(RTCStruct.month); 						/* set month */
	date.year = bcd2hex(RTCStruct.year);							/* set year */
	time.hours = bcd2hex(RTCStruct.hour);						/* set hours */
	time.minutes = bcd2hex(RTCStruct.min);						 /* set minutes */
	time.seconds = bcd2hex(RTCStruct.sec);						/* set seconds */

	/*set time & date*/
	RTC_SetDate(date);						/* set date in RTC */
	RTC_SetTime(time);						/* set time in RTC registers */
}

void RTC_CLK_Burst_Read(void)
{
    RTC_GetDate(BCD,&date);
    RTC_GetTime(BCD,&time); 				/*time structure must be equal to alarm time*/	   
	RTCStruct.sec = time.seconds;
	RTCStruct.min = time.minutes;
	RTCStruct.hour = time.hours;	
	RTCStruct.date = date.day;
	RTCStruct.month = date.month;
	RTCStruct.day = date.weekday;
	// CJK080703
	// RTC week : 1~7, calculation : 0~6
	// 1:MON, 2:TUE ...7:SUN => 0:SUN, 1:MON, 2:TUE ...6:SAT
	// RTC에서 읽을 경우  7(SUN)을 0으로 바꿈
	if (RTCStruct.day == 7) RTCStruct.day = 0;
	///////
	RTCStruct.year  = date.year;			   
}

void RTCInit(void)
{
	RTC_AppInit();
}


#endif //#ifdef CPU_PIC32MZ

