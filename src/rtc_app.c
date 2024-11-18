#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "serial.h"
#include "flash.h"

#include "commbuf.h"
#include "prt_common.h"
#include "prtdrv.h"
#include "timer.h"
#include "serial.h"
#include "flash.h"
#include "sensor.h"
#include "ram.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "prt_interpreter.h"
#include "dsp_vfd7.h"
#include "adm.h"
#include "key.h"
#include "cal.h"
#include "programmode.h"
//#include "file.h"
#include "rtc.h"
#include "debug.h"
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"
#include "rtc_app.h"
#include "common_sunday.h"

static INT8U preMinute = 0;

INT32U rtc_getdate(char *str)
{
	INT32U pnt;
	INT8U mode,date_mode;

	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	mode    =get_global_bparam(GLOBAL_DATETIME_MODE );
	date_mode=mode>>4;
	date_mode&=0x03;
	pnt = rtc_getdate_sub(mode&0x80,mode&0x40,date_mode,str);
	return pnt;
}

INT32U rtc_gettime(char *str)
{
	INT32U pnt;
	INT8U mode;

	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	mode    =get_global_bparam(GLOBAL_DATETIME_MODE);
	mode   &=0x0f;
	pnt=rtc_gettime_sub(mode,str);
	return pnt;
}

INT32U rtc_getdate_sub(INT8U yflag,INT8U mflag,INT8U mode,char *str)
{
	INT32U pnt;
	char  sep_date;
	INT8U v8;
	char  s_mon[12],s_year[12];
	INT8U *s_month;
	INT16S   p;
	RTC_STRUCT time_str;
#ifdef USE_PERSIAN_CALENDAR
	INT16U perCentury;
#endif

//	RTC_CLK_Burst_Read();
 	Report_GetCurTime(&time_str,1); //SG060309
 
	sep_date=get_global_bparam(GLOBAL_DATE_SEPARATOR);
	if (mode==0xff) {
		mode=get_global_bparam(GLOBAL_DATETIME_MODE );
		v8  = mode>>4;
		v8 &= 0x03;
		if (v8==2) yflag=0;
		mode=v8;
	}
	if (mflag) {
		p=(INT16S)(RTCStruct.month-1);
		if (p>=12) p=11;
		if (p<0  ) p=0;
		s_month = (INT8U*)pnt;
		s_month+= GLOBAL_DATE_MONTHARRAY;
		s_month+= p*10;
		memcpy((INT8U *)s_mon,(INT8U *)s_month,10);
		s_mon[5]=0;
	} else {
		sprintf(s_mon,"%02d",time_str.month);
	}

	if (yflag) {
                sprintf(s_year,"%02d",time_str.year);		
	} else {
#ifdef USE_PERSIAN_CALENDAR
		if(RTCStruct.year < 0x5a)	perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
		else perCentury = 1300;

		sprintf(s_year,"%04d",time_str.year+perCentury);
#else
		sprintf(s_year,"%04d",time_str.year+2000);
#endif
	}

	switch (mode) {
		default:
		case 0: sprintf(str,"%s%c%s%c%02d",s_year,sep_date,s_mon,sep_date,time_str.date);
			break;
		case 1: sprintf(str,"%s%c%02d%c%s",s_mon ,sep_date,time_str.date,sep_date,s_year);
			break;
		case 2: sprintf(str,"%s%c%s",s_mon ,sep_date,s_year);
			break;
		case 3: sprintf(str,"%02d%c%s%c%s" ,time_str.date,sep_date,s_mon ,sep_date,s_year);
			break;
	}

	pnt =time_str.hour*10000;
	pnt+=time_str.min *100;
	pnt+=time_str.sec;

	return pnt;
}

INT32U rtc_gettime_sub(INT8U mode,char *str)
{
	INT32U pnt;
	char   sep_time;
	INT32U s_ampm;
	INT8U  h;
	RTC_STRUCT time_str; //SG060309
	INT8U str_ampm[5+1];

//	RTC_CLK_Burst_Read(); 
	Report_GetCurTime(&time_str,1); //SG060309
	
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	sep_time=Flash_bread(pnt+GLOBAL_TIME_SEPARATOR);
	s_ampm = pnt;
	s_ampm+=GLOBAL_DATE_AMPMARRAY;
	switch (mode) {
		default:
		case 0: //SG060309
#ifdef HEBREW_FONT	//히브리어 시,분,초 순서 반대 나오는 현상 수정
			sprintf(str,"%02d%c%02d",
			time_str.min,sep_time,time_str.hour);
#else
			sprintf(str,"%02d%c%02d",
			time_str.hour,sep_time,time_str.min);
#endif
			break;
		case 1: 							// "07:30 PM" 
			mode=time_str.hour;
			if (mode>=12) {s_ampm+=5; }
			if (mode> 12) mode=mode-12;
			Flash_sread(s_ampm, str_ampm, 5);
			str_ampm[5] = 0;
			h=mode;
#ifdef HEBREW_FONT
			sprintf(str,"%02d%c%02d %s",
				time_str.min,sep_time,h,str_ampm); 
#else
			sprintf(str,"%02d%c%02d %s",
				h,sep_time,time_str.min,str_ampm);
#endif
			break;
		case 2: 							// "19:30:27" 
#ifdef HEBREW_FONT
			sprintf(str,"%02d%c%02d%c%02d",
				time_str.sec,sep_time,time_str.min,sep_time,time_str.hour);
#else
			sprintf(str,"%02d%c%02d%c%02d",
				time_str.hour,sep_time,time_str.min,sep_time,time_str.sec);
#endif
			break;
		case 3:								// "07:30:27 PM"
			mode=time_str.hour;
			if (mode>=12) {s_ampm+=5; }
			if (mode> 12) mode=mode-12;
			Flash_sread(s_ampm, str_ampm, 5);
			str_ampm[5] = 0;
			h=mode;
#ifdef HEBREW_FONT
			sprintf(str,"%02d%c%02d%c%02d %s",
				time_str.sec,sep_time,time_str.min,sep_time,h,str_ampm);
#else
			sprintf(str,"%02d%c%02d%c%02d %s",
				h,sep_time,time_str.min,sep_time,time_str.sec,str_ampm);
#endif
			break;
	}
	pnt =time_str.hour*10000;
	pnt+=time_str.min *100;
	pnt+=time_str.sec;
	return pnt;
}

INT32U rtc_makedate_str(char *str, INT8U year, INT8U month, INT8U date)
{
	INT32U pnt;
	char  sep_date;
	INT8U v8;
	char  s_mon[12],s_year[12];
	INT8U *s_month;
	INT16S   p;
	INT8U yflag, mflag, mode,removeM=0;  //add by son remove 070126
#ifdef USE_PERSIAN_CALENDAR
	INT16U perCentury;
#endif

	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	mode    =get_global_bparam(GLOBAL_DATETIME_MODE );
	yflag = mode&0x80;
	mflag = mode&0x40;
	mode=mode>>4;
	mode&=0x03;

	sep_date=Flash_bread(pnt+GLOBAL_DATE_SEPARATOR);
	if (mode==0xff) {
		mode=get_global_bparam(GLOBAL_DATETIME_MODE );
		v8  = mode>>4;
		v8 &= 0x03;
		if (v8==2) yflag=0;
		mode=v8;
	}
	if (mflag) {
		if (month == 0)      // add by son remove 070126
		{ 
		  removeM=1;
		  month = 1;	// Added by CJK
		}
		if (month > 12) month = 1;
		p=(INT16S)(month-1);
		s_month = (INT8U*)pnt;
		s_month+= GLOBAL_DATE_MONTHARRAY;
		s_month+= p*10;
		//memcpy((INT8U *)s_mon,(INT8U *)s_month,10);
		Flash_sread((INT32U)s_month, (INT8U*)s_mon, 10);
#ifdef USE_PERSIA_DEFAULT
        s_mon[10]=0;
#else
		s_mon[3]=0; // month 캡션 3글자로 제한
#endif

	} else { 
		if (month == 0)       // add by son remove 070126
		{ 
		  removeM=1;
		  month = 1;}  
		sprintf(s_mon,"%02d",month);
	}
	if (yflag) {
		sprintf(s_year,"%02d",year);
	} else {
#ifdef USE_PERSIAN_CALENDAR
		if(RTCStruct.year < 0x5a)	perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
		else perCentury = 1300;

		sprintf(s_year,"%04d",(INT16U)year+perCentury);
#else
		sprintf(s_year,"%04d",(INT16U)year+2000);
#endif
	}
	switch (mode) {		       // add by son remove 070126
		default:
		case 0: if (removeM)
			sprintf(str,"%s%c%02d",s_year,sep_date,date);
			else
			{
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
                if(mflag) sprintf(str,"%02d%c%s%c%s" ,date,sep_date,s_mon,sep_date,s_year);
                else sprintf(str,"%s%c%s%c%02d" ,s_year,sep_date,s_mon,sep_date,date);
#else
			sprintf(str,"%s%c%s%c%02d",s_year,sep_date,s_mon,sep_date,date);
#endif
			}
			break;
		case 1: if (removeM)
			sprintf(str,"%02d%c%s",date,sep_date,s_year);
			else
			{
			sprintf(str,"%s%c%02d%c%s",s_mon ,sep_date,date,sep_date,s_year);
			}
			break;
		case 2: if (removeM)
			sprintf(str,"%s",s_year);
			else
			{
			sprintf(str,"%s%c%s",s_mon ,sep_date,s_year);
			}
			break;
		case 3: if (removeM)
			sprintf(str,"%02d%c%s",date,sep_date,s_year);
			else
			{
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
				if(mflag) sprintf(str,"%s%c%s%c%02d" ,s_year,sep_date,s_mon,sep_date,date);
				else sprintf(str,"%02d%c%s%c%s" ,date,sep_date,s_mon ,sep_date,s_year);
#else
				sprintf(str,"%02d%c%s%c%s" ,date,sep_date,s_mon ,sep_date,s_year);
#endif
			}
			break;		 // add by son end remove 070126
	}
	pnt =(INT32U)year *10000L;
	pnt+=(INT32U)month*100L;
	pnt+=(INT32U)date;
	return pnt;
}
void date_2str(INT32U date,char *str)
{
	INT32U pnt;
	INT8U year,mon,day,mode;
	if (date<=9999) {
		sprintf(str,"%d",date);
		return;
	}
	year=date/10000l;
	mon =(date%10000l)/100l;
	day =date%100;

	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	mode    =Flash_bread(pnt+GLOBAL_DATETIME_MODE);
	mode   >>= 4;
	mode   &= 0x03;
	switch (mode) {
		case 0: sprintf(str,"%02d%02d%02d",year,mon,day); break;
		case 1:
		case 2: sprintf(str,"%02d%02d%02d",mon ,day,year); break;
		case 3: sprintf(str,"%02d%02d%02d",day ,mon,year); break;
	}
}

long str_2sellbydate(char *str)
{
	char tmp[64];
	INT32U pnt;
	INT8U year,mon,day,mode;
	long  value;
	
	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	mode    =Flash_bread(pnt+GLOBAL_DATETIME_MODE);
	mode   >>= 4;
	mode    &= 0x03;
	switch (mode) {
		case 0: memcpy(tmp,&str[0],2); tmp[2]=0; year=(INT8U)atoi(tmp);
			memcpy(tmp,&str[2],2); tmp[2]=0; mon =(INT8U)atoi(tmp);
			memcpy(tmp,&str[4],2); tmp[2]=0; day =(INT8U)atoi(tmp);
			break;
		case 1:
		case 2: memcpy(tmp,&str[0],2); tmp[2]=0; mon =(INT8U)atoi(tmp);
			memcpy(tmp,&str[2],2); tmp[2]=0; day =(INT8U)atoi(tmp);
			memcpy(tmp,&str[4],2); tmp[2]=0; year=(INT8U)atoi(tmp);
			break;
		case 3: memcpy(tmp,&str[0],2); tmp[2]=0; day =(INT8U)atoi(tmp);
			memcpy(tmp,&str[2],2); tmp[2]=0; mon =(INT8U)atoi(tmp);
			memcpy(tmp,&str[4],2); tmp[2]=0; year=(INT8U)atoi(tmp);
	}
	value = atol(str);
	if (value>9999l) {
		//if (date_checkday((short)year,(short)mon,(short)day)) {
			value  = (long)year*10000l;
			value += (long)mon*100l;
			value += day;
		//} else {
		//	value = -1l;
		//}
	}
	return value;
}

INT32U rtc_maketime_str(char *str, INT8U hour, INT8U min, INT8U sec)	// bcd format
{
	INT32U pnt;
	char   sep_time;
	INT32U s_ampm;
	INT8U  h;
	INT8U mode;
	INT8U str_ampm[2+1];

	pnt =DFLASH_BASE;
	pnt+=FLASH_GLOBAL_AREA;
	mode    =Flash_bread(pnt+GLOBAL_DATETIME_MODE);
	mode   &=0x0f;
	sep_time=Flash_bread(pnt+GLOBAL_TIME_SEPARATOR);
	s_ampm = pnt;
	s_ampm += GLOBAL_DATE_AMPMARRAY;
	switch (mode) {
		default:
		case 0: 							// "19:30"
			sprintf(str,"%02d%c%02d", hour, sep_time, min);
			break;
		case 1: 							// "07:30 PM"
			if (hour>=12) {s_ampm+=5; }
			if (hour> 12) h=hour-12;
			else h=hour;
			Flash_sread(s_ampm, str_ampm, 2);
			str_ampm[2] = 0;
			sprintf(str,"%02d%c%02d %s", h,sep_time, min, str_ampm);
			break;
		case 2: 							// "19:30:27"
			sprintf(str,"%02d%c%02d%c%02d", hour, sep_time, min, sep_time, sec);
			break;
		case 3:								// "07:30:27 PM"	
			if (hour>=12) {s_ampm+=5; }
			if (hour> 12) h=hour-12;
			else h=hour;
			Flash_sread(s_ampm, str_ampm, 2);
			str_ampm[2] = 0;
			sprintf(str,"%02d%c%02d%c%02d %s", h, sep_time, min, sep_time, sec, str_ampm);
			break;
	}
	pnt =(INT32U)hour*10000L;
	pnt+=(INT32U)min *100L;
	pnt+=(INT32U)sec;
	return pnt;
}

#ifdef USE_PERSIAN_CALENDAR
ROMDATA INT8S mld[13] = {0,1,-1,0,0,1,1,2,3,3,4,4,5};
ROMDATA INT8U shm[13] = {0,1,2,3,4,5,6,6,6,6,6,6,5};
ROMDATA INT8U mm[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};	//Gregorian days
ROMDATA INT8U ss[13] = {0,31,31,31,31,31,31,30,30,30,30,30,29};	//Persian days

void  RTC_Convert_Persian_Read(void)
{
	INT8U	i;
	INT16U  greg_year,greg_month,greg_day,solar_year,solar_month,solar_day;
	INT32U	temp1,temp2,temp6;
	
	RTC_CLK_Burst_Read(); 

	greg_year = bcd2hex(RTCStruct.year);
	greg_month =bcd2hex(RTCStruct.month); 
	greg_day =bcd2hex(RTCStruct.date);
	
	greg_year = greg_year+2000;
	greg_year = greg_year -1;
	temp2 = (INT32U)greg_year;
	temp2 = temp2-1981;
	temp1 = temp2*365;
	temp1 = temp1+(temp2+1)/4;
	greg_year = (INT16U)greg_month;
	greg_year = 30*(greg_year-1);
	greg_year = greg_year+mld[greg_month-1]+greg_day;
	if(greg_month>2 && ((temp2%4)==2)) greg_year= greg_year+1;
	temp1 = temp1+(INT32U)greg_year;
	temp1 = temp1-79; //DATEOFST = 79
	temp2 = 0;
	temp6 = 0;
	for(i=0;i<100;i+=1)
	{
		temp2+=365;
		if(i%4==1 && i<13 )temp2 = temp2+1;	//4년마다 366일 (1년 = 365.2422)
		if(i%4==2 && i>13 )temp2 = temp2+1;
		if(temp2 >= temp1)
		{
			solar_year = 60+i;
			greg_year = (INT16U)(temp1-temp6);
			break;
		}
		temp6=temp2;
	}
	temp2=0;
	temp1=0;
	for(i=0;i<13;i+=1)
	{
		temp1= temp1+(INT32U)ss[i+1];
		if( ((solar_year%4)==1) && (i==11) && (solar_year<73)) temp1= temp1+1;
		if( ((solar_year%4)==2) && (i==11) && (solar_year>73)) temp1= temp1+1;
		if((INT16U)temp1 >= greg_year)
		{
			solar_month = i;
			solar_day = greg_year-(INT16U)temp2;
			break;
		}
		temp2=temp1;
	}
	RTCStruct.year = hex2bcd((solar_year+1)%100);
	RTCStruct.month = hex2bcd(solar_month+1);
	RTCStruct.date = hex2bcd(solar_day);
}

void  RTC_Convert_Persian_Write(void)
{
	INT8U	i;
	INT16U  greg_year,greg_month,greg_day,solar_year,solar_month,solar_day;
	INT32U	temp1,temp2,temp3,temp6;

	solar_year = bcd2hex(RTCStruct.year);
	solar_month = bcd2hex(RTCStruct.month); 
	solar_day = bcd2hex(RTCStruct.date);

	if(solar_year>90) { 
		solar_year = solar_year+1300;
	} else { 
		solar_year = solar_year+1400;
	}
	solar_year = solar_year-1;
	temp2 = (INT32U)solar_year;
	temp2 = temp2-1360;
	temp1 = temp2*365;
	
	if(temp2<14) temp1 = temp1+((temp2+2)/4);
	else temp1 = temp1+(3+(temp2-11)/4);
	
	solar_year = (INT16U)solar_month;
	solar_year = (30*(solar_year-1))+shm[solar_year-1]+solar_day;
	temp1 = temp1+(INT32U)solar_year;
	temp1 = temp1+79;	//DATEOFST = 79
	temp2 = 0;
	temp3 = 0;
	
	for(i=0;i<100;i+=1)
	{
		temp2= temp2+365;
		if(i%4==2) temp2= temp2+1;
		if(temp2 >= temp1)
		{
			greg_year=81+i;
			solar_year=(INT16U)(temp1- temp3);
			break;
		}
		temp3 = temp2;
	}
	temp2 = 0;
	temp1 = 0;
	
	for(i=0;i<13;i+=1)
	{
		temp1 = temp1+(INT32U)mm[i+1];
		if(greg_year%4==3 && i==1) temp1 = temp1+1;
		if((INT16U)temp1 >= solar_year)
		{
			greg_month = i;
			greg_day = solar_year-(INT16U)temp2;
			break;
		}
		temp2 = temp1;
	}
	greg_year = (greg_year+1)%100;//+1900;
	greg_month = greg_month +1;

	RTCStruct.year = hex2bcd(greg_year);
	RTCStruct.month = hex2bcd(greg_month);
	RTCStruct.date = hex2bcd(greg_day);

	RTC_CLK_Burst_Write();
  #ifdef USE_MORNITORING_CNT
	rtc_set_pre_minute();
  #endif
}
#endif

#ifdef USE_MORNITORING_CNT  
void rtc_set_pre_minute(void)
{
	preMinute = bcd2hex(RTCStruct.min);
}

INT8U rtc_get_pre_minute(void)
{
	return preMinute;
}

void rtc_cnt_scale_operated_time(void)
{
	INT8U curMin = 0;
	INT8S subMin = 0;

    RTC_CLK_Burst_Read();
    curMin = bcd2hex(RTCStruct.min);
    subMin = curMin - rtc_get_pre_minute();	
	
	if (subMin < 0)
	{
		subMin = 60 - rtc_get_pre_minute() + curMin;
	}
	
	if (subMin > 0)
	{
		OperTime.AfterCnt = OperTime.AfterCnt + subMin;
		OperTime.TotalCnt = OperTime.TotalCnt + subMin;
   
		rtc_set_pre_minute();
	}
}

void rtc_save_operated_time(void)
{
	set_nvram_lparam(NVRAM_MORNITORING_USING_TIME, OperTime.TotalCnt);
	set_nvram_lparam(NVRAM_MORNITORING_USING_TIME + 4, OperTime.AfterCnt);
    
//sprintf(MsgBuf, "after: %d , tot: %d\r\n", OperTime.AfterCnt, OperTime.TotalCnt);
//MsgOut(MsgBuf);     
    
}

void rtc_load_operated_time(void)
{
	OperTime.TotalCnt = get_nvram_lparam(NVRAM_MORNITORING_USING_TIME);
	OperTime.AfterCnt = get_nvram_lparam(NVRAM_MORNITORING_USING_TIME + 4);
	OperTime.InspectDate = get_nvram_lparam(NVRAM_MORNITORING_USING_TIME + 8);
}

void rtc_clear_operated_time(INT32U InitDate)
{
	OperTime.InspectDate = InitDate;
	OperTime.AfterCnt = 0;
	set_nvram_lparam(NVRAM_MORNITORING_USING_TIME + 4, OperTime.AfterCnt);
	set_nvram_lparam(NVRAM_MORNITORING_USING_TIME + 8, OperTime.InspectDate);	
}
#endif//#ifdef USE_MORNITORING_CNT
