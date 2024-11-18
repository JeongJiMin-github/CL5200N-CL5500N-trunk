/*****************************************************************************
|*			
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	rtc.h
|*  Version			:	0.1
|*  Programmer(s)	:	Hong Yi-Phyo (HYP)
|*  Created			:	2003/07/31
|*  Description		:	CLP Printer RTC control routine header    
|*				
****************************************************************************/

#ifndef _RTC_H
#define _RTC_H

#define	RTC_SEC_WRITE	0x80
#define RTC_SEC_READ	0x81
#define	RTC_MIN_WRITE	0x82
#define RTC_MIN_READ	0x83
#define	RTC_HR_WRITE	0x84
#define RTC_HR_READ		0x85
#define	RTC_DATE_WRITE	0x86
#define RTC_DATE_READ	0x87
#define	RTC_MONTH_WRITE	0x88
#define RTC_MONTH_READ	0x89
#define	RTC_DAY_WRITE	0x8A
#define RTC_DAY_READ	0x8B
#define	RTC_YEAR_WRITE	0x8C
#define RTC_YEAR_READ	0x8D
#define	RTC_CTRL_WRITE	0x8E
#define RTC_CTRL_READ	0x8F
#define	RTC_TRCHA_WRITE	0x90
#define RTC_TRCHA_READ	0x91
#define	RTC_CLK_BUR_WRITE	0xBE
#define RTC_CLK_BUR_READ	0xBF
#define	RTC_RAM_BUR_WRITE	0xFE
#define RTC_RAM_BUR_READ	0xFF

#define SEC			0
#define MIN			1
#define HOUR		2
#define DATE		3
#define MONTH		4
#define DAY			5
#define YEAR		6
#pragma pack(4)
typedef struct {
	INT8U sec,min,hour,date,month,day,year;		// day = 1..7 12 => 0x12 : BCD type
} RTC_STRUCT;									// date= 1..31 , hour = 0..23, year=00..99, month=1..12

//SG060309
typedef struct {
	INT8U sec,min,hour,date,month,year,week;
	short deltadays;
	INT16U sum;  
} PACKDATE_STRUCT;							  
#pragma pack()

// CL5500 rtc struct »ç¿ë
typedef struct
{
  INT8U century;
  INT8U year;
  INT8U month;
  INT8U day;
  INT8U weekday;
}RTC_DATE;

typedef struct
{
  INT8U hours;
  INT8U minutes;
  INT8U seconds;
  INT8U milliseconds;
}RTC_TIME;

#define BINARY 0
#define BCD 1


//extern HUGEDATA RTC_STRUCT RTCStruct;
extern NEARDATA RTC_STRUCT RTCStruct;//HYP 20060412 _huge --> _near for small code size

extern void  RTC_CLK_Write(INT8U clk_addr, INT8U data);
extern INT8U RTC_CLK_Read(INT8U clk_addr);
extern void  RTC_RAM_Write(INT8U ram_addr, INT8U data);
extern INT8U RTC_RAM_Read(INT8U ram_addr);
extern void  RTC_RAM_Burst_Read(HUGEDATA INT8U *p_data);
extern void  RTC_RAM_Burst_Write(HUGEDATA INT8U *p_data);
extern void  RTC_CLK_Burst_Read(void);
extern void  RTC_CLK_Burst_Write(void);
extern void  RTCInit(void);

extern void RTC_SetDate(RTC_DATE date);
extern void RTC_SetTime(RTC_TIME time);

#endif /* _RTC_H */
