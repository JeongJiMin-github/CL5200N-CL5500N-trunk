/*****************************************************************************
|*			
|*  Copyright		:	(c) 2002 CAS
|*  Filename		:	sale_report.c
|*  Version		:	0.1
|*  Modified		:	2004/08/20
|*				
****************************************************************************/
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
#include "adm_app.h"
#include "key.h"
#include "key_typedef.h"
#include "key_function.h"
#include "key_app.h"

#include "cal.h"
#include "programmode.h"
// #include "file.h"
#include "rtc.h"
#include "debug.h"
#include "ethernet.h"
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"

#include "mode_sale.h"
#include "mode_main.h"
#include "mode_commun.h"
#include "mode_error.h"
#include "mode_sale_display.h"
#include "plu_manager.h"
#include "caption_app.h"
#include "dsp_app.h"
#include "rtc_app.h"
#include "plu_auxcall.h"
#include "discount.h"
#include "menu.h"
#include "cashdrawer.h"
#include "department.h"
#include "clerk_app.h"
#include "plu_device.h"
#include "shopname.h"
#include "prt_app.h"
#include "prt_cmd.h"

#include "pgm_app.h"
#include "network_command.h"

#include "login.h"
#include "tax.h"
#include "label_caption.h"
#include "common_sunday.h"

#include "prt_cmdtest.h"
#include "sale_report.h"
#include "customer_app.h"
#include "traceability.h"
#include "prt_list.h"
#include "plu_edit.h"
#include "usbmem_app.h"
#include "pcs_unit.h"

#ifdef USE_CHN_CART_SCALE
#include "traceability_chn.h"
#endif
#ifdef USE_PERSIA_CAPTION
#include "caption_IR_1256.h"
#endif

char ReportTypeChar[2] = {'X', 'Z'};
extern STRUCT_PRT_TOTAL PrtTotalStr;
#ifdef USE_INDIA_TAX
INT32U No_taxable_total = 0;
#endif

extern PLU_SALES_SUMMARY_TYPE* FindPluSummary(INT16S mode,INT32U id,INT16U dept,INT8U pluType,INT16U *pindex);
INT8U ConfirmPrintOnLabel;
INT8U EnablePrintOnLabel = OFF;
STRUCT_TOTAL_PRICE TotalData;
extern SALE_G_OPER_TYPE	GlbOper;
extern DISC_SALE_TYPE DiscSale;
extern PRICE_TYPE Price;
extern CURRENCY_PRICE_TYPE CurrencyPrice;
//extern long	SummaryRoundPrice;
extern TRANSACTION_PAY transPay;
extern SALE_G_PRINT_TYPE	GlbPrint;
extern PRINT_OPER_TYPE		PrtOper;	// Added by CJK 20051226
extern INT8U GlbFlagPCnMasterConnection;
extern void customer_read(INT16U code, CUSTOMER_STRUCT *custom);


#ifdef TEST_REPORT
void TestPrintSaleReportAll(INT16U j, INT16U k)
{
	PLU_SALES_SUMMARY_TYPE *psr;
	PLU_SALES_SUMMARY_TYPE pst;
	SALES_SUMMARY_TYPE *sr;
	INT16U i;
	INT16U head, tail;
	INT32U addr;
	INT8U reportDate1[6];
	INT8U reportDate2[6];

	psr = &pst;

	sprintf(MsgBuf, "<TEST>\r\n");
	MsgOut(MsgBuf);

	if (k == 0) addr = NVRAM_PLU_SUMMARY;
	else if (k == 1) addr = NVEXT2_REPORT1_PLU_SUMMARY;
	else if (k == 2) addr = NVEXT2_REPORT2_PLU_SUMMARY;

	if (k)
	{
		for (i=0; i<6; i++)
		{
			reportDate1[i] = get_nvram_bparam(NVEXT2_REPORT1_PLU_UPDATE_DATE+i);
			reportDate2[i] = get_nvram_bparam(NVEXT2_REPORT2_PLU_UPDATE_DATE+i);
		}
		sprintf(MsgBuf, "ReportDate1 %02d,%02d,%02d \r\n", reportDate1[0],reportDate1[1],reportDate1[2]);
		MsgOut(MsgBuf);
		sprintf(MsgBuf, "ReportTime1 %02d,%02d,%02d \r\n", reportDate1[3],reportDate1[4],reportDate1[5]);
		MsgOut(MsgBuf);
		sprintf(MsgBuf, "ReportDate2 %02d,%02d,%02d \r\n", reportDate2[0],reportDate2[1],reportDate2[2]);
		MsgOut(MsgBuf);
		sprintf(MsgBuf, "ReportTime2 %02d,%02d,%02d \r\n", reportDate2[3],reportDate2[4],reportDate2[5]);
		MsgOut(MsgBuf);
		sprintf(MsgBuf, "NVEXT2_REPORT_PLU_BACKUP_INDEX %d \r\n",get_nvram_bparam(NVEXT2_REPORT_PLU_BACKUP_INDEX));
		MsgOut(MsgBuf);
	}	 

	head = get_nvram_wparam(NVRAM_PLUSUM_HEADINDEX);
	tail = get_nvram_wparam(NVRAM_PLUSUM_TAILINDEX);
	sprintf(MsgBuf, "HEAD = %d, TAIL = %d\r\n", head, tail);
	MsgOut(MsgBuf);
	if(j > 1 || j < 0) j = 0;
	for(i = 0; i < 10; i++) {
		//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(addr, i);
		get_nvram_plu_summary(addr, i, psr);
		sprintf(MsgBuf, "%2d [N:%7ld, D:%2d, T:%2d, V:%9ld, PV:%2d, NX:%2d]\r\n", i, psr->pstPLU, psr->pstDept, psr->pstTransactions[j], psr->pstVolume[j], psr->pstPrev, psr->pstNext);
		MsgOut(MsgBuf);
	}
}
#endif

//SG060309
void Report_GetCurTime(RTC_STRUCT *time_str, INT8U mode)
{
	short yy, mm, dd;
	short tempdelta;
#ifdef USE_PERSIAN_CALENDAR
	INT16U perCentury;
#endif
	
#ifdef USE_PERSIAN_CALENDAR
	RTC_Convert_Persian_Read();
#else
	RTC_CLK_Burst_Read();
#endif

//	if( mPackDate.deltadays && Operation.wPrepack && mode==1){
	if( mPackDate.deltadays && mode==1){
		tempdelta = mPackDate.deltadays;
	} else {
		tempdelta = 0;
	}
#ifdef USE_PERSIAN_CALENDAR
	if(RTCStruct.year < 0x5a) 	perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
	else perCentury = 1300;
	
	yy = (short)bcd2hex(RTCStruct.year)+perCentury;
#else
	yy = (short)bcd2hex(RTCStruct.year)+2000;
#endif
	mm = (short)bcd2hex(RTCStruct.month);
	dd = (short)bcd2hex(RTCStruct.date);

	if(tempdelta > 0){
		date_getnextday(&yy,&mm,&dd,tempdelta);
	} else if(tempdelta<0){
		date_getbeforeday(&yy,&mm,&dd,(-tempdelta));
	}
	
	time_str->day = (INT8U)date_getweek(yy,mm,dd);
#ifdef USE_PERSIAN_CALENDAR
	time_str->year  = (INT8U)(yy-perCentury);
#else
	time_str->year  = (INT8U)(yy-2000); 
#endif
	time_str->month = (INT8U)mm;
	time_str->date  = (INT8U)dd;
	time_str->hour  = bcd2hex(RTCStruct.hour);
	time_str->min   = bcd2hex(RTCStruct.min);
	time_str->sec   = bcd2hex(RTCStruct.sec);

}

void Report_SetResetTime(INT32U rel_addr, RTC_STRUCT *time_str, INT8U period)
{
	rel_addr += (6*period);
	set_nvram_bparam(rel_addr++, time_str->year);
	set_nvram_bparam(rel_addr++, time_str->month);
	set_nvram_bparam(rel_addr++, time_str->date);
	set_nvram_bparam(rel_addr++, time_str->hour);
	set_nvram_bparam(rel_addr++, time_str->min);
	set_nvram_bparam(rel_addr,   time_str->sec);
}

void Report_GetResetTime(INT32U rel_addr, RTC_STRUCT *time_str, INT8U period)
{
	rel_addr += (6*period);
	time_str->year  = get_nvram_bparam(rel_addr++);
	time_str->month = get_nvram_bparam(rel_addr++);
	time_str->date  = get_nvram_bparam(rel_addr++);
	time_str->hour  = get_nvram_bparam(rel_addr++);
	time_str->min   = get_nvram_bparam(rel_addr++);
	time_str->sec   = get_nvram_bparam(rel_addr);
	time_str->day   = date_getweek(time_str->year, time_str->month, time_str->date);
}

void Report_PeriodResetTime(INT8U period,RTC_STRUCT *time_str)
{
	Report_SetResetTime(NVRAM_RESET_TIME_TOTAL   , time_str, period);
	Report_SetResetTime(NVRAM_RESET_TIME_DEPT    , time_str, period);
	Report_SetResetTime(NVRAM_RESET_TIME_SCALE   , time_str, period);
	Report_SetResetTime(NVRAM_RESET_TIME_GROUP   , time_str, period);
	Report_SetResetTime(NVRAM_RESET_TIME_CLERK   , time_str, period);
	Report_SetResetTime(NVRAM_RESET_TIME_HOURLY  , time_str, period);
	Report_SetResetTime(NVRAM_RESET_TIME_TOTAL   , time_str, period);
}

void Report_InitResetTime(void)
{
	RTC_STRUCT time_str;

	Report_GetCurTime(&time_str,0);
	Report_PeriodResetTime(0,&time_str);
#ifndef DISABLE_X2Z2_REPORT
	Report_PeriodResetTime(1,&time_str);
#endif
}

void Report_ReadResetTime(INT32U rel_addr, INT16S period, RTC_STRUCT *time_str)
{
//	INT8U flag_r;
//	flag_r = get_global_bparam(GLOBAL_REPORT_FLAG);	// Report Individual Clear

	//if(flag_r&0x01) {
	if(PrtOper.IndividualClear) {
		Report_GetResetTime(rel_addr, time_str, period);
	} else {
		Report_GetResetTime(NVRAM_RESET_TIME_TOTAL, time_str, period);
	}
}

void Report_WriteCurResetTime(INT32U rel_addr, INT16S period)
{
	RTC_STRUCT time_str;

	Report_GetCurTime(&time_str,0);
	Report_SetResetTime(rel_addr, &time_str, period);
}

// mode = 0 	: long type	ex) 2006.12.01(금)  08:51
//	  1	: middle type	ex) 2006.12.01  08:51
//        2	: short type	ex) 2006.12.01
void Print_getTimeStr(RTC_STRUCT *time_str,char *string_buf, INT8U mode)
{
	INT16S  leng;
	char weekday[11];

	weekday[0] = 0;
	if (mode == 0)
	{
		get_global_sparam(GLOBAL_DATE_WEEKARRAY+(time_str->day*10), (INT8U *)string_buf, 10);
		string_buf[10] = 0;
		sprintf(weekday, "(%s)", string_buf);
	}

	rtc_makedate_str(string_buf, time_str->year, time_str->month, time_str->date); // 11 byte(month 최대 3글자로 제한)
	strcat(string_buf, weekday);
	
	if (mode == 0 || mode == 1)
	{
		leng = strlen(string_buf);
		string_buf[leng++] = ' ';
		string_buf[leng++] = ' ';
		rtc_maketime_str(string_buf+leng, time_str->hour, time_str->min, time_str->sec); // 11 byte (AM/PM 2byte로 제한)
	}
}

void PrintFixedTime(RTC_STRUCT *time_str)
{
	char string_buf[50];
//	char weekday[11];
//	INT8U len;
//	get_global_sparam(GLOBAL_DATE_WEEKARRAY+(time_str->day*10), (INT8U *)string_buf, 10);
//	string_buf[10] = 0;
//	sprintf(weekday, "(%s)", string_buf);
//	rtc_makedate_str(string_buf, time_str->year, time_str->month, time_str->date);
//	strcat(string_buf, weekday);
//	len = strlen(string_buf);
//	string_buf[len++] = ' ';
//	string_buf[len++] = ' ';
//	rtc_maketime_str(string_buf+len, time_str->hour, time_str->min, time_str->sec);	
//
	Print_getTimeStr(time_str,string_buf, 0);

#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);	
		PutDataTxBuf(&CommBuf, (char *)string_buf, strlen(string_buf));
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
#else
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
#endif//USE_DOUBLE_PRINT
}


void PrintCurrentTime(void)
{
	RTC_STRUCT time_str;

	Report_GetCurTime(&time_str,0);
	PrintFixedTime(&time_str);
}

//Added by JJANG 20080519
//Parameter 625 : 2 && 확장메모리 有
//Back up 시점에서 현재 시간을 넣는다. 
void Report_SetPLUReportDate(INT8U rType)
{
	RTC_STRUCT time_str;   
	INT32U addr;

	Report_GetCurTime(&time_str,0);

	if (rType == 1) addr = NVEXT2_REPORT1_PLU_UPDATE_DATE;
	else if (rType == 2) addr = NVEXT2_REPORT2_PLU_UPDATE_DATE;

	Report_SetResetTime(addr, &time_str, 0);

	set_nvram_bparam(NVEXT2_REPORT_PLU_UPDATE_DATE, time_str.year);
	set_nvram_bparam(NVEXT2_REPORT_PLU_UPDATE_DATE+1, time_str.month);
	set_nvram_bparam(NVEXT2_REPORT_PLU_UPDATE_DATE+2, time_str.date);
}

//Added by JJANG 20080519
//현재시간과 최종 백업된 시간을 비교하여 같으면 1, 다르면 0을 리턴해준다.
INT8U Report_GetPLUReportDate(void)
{
	RTC_STRUCT time_str;   
	INT8U reportDate[3];
	INT8U i = 0;

	Report_GetCurTime(&time_str,0);	
	for (i=0; i<3; i++)
	{
		reportDate[i] = get_nvram_bparam(NVEXT2_REPORT_PLU_UPDATE_DATE+i);
	}
	if (reportDate[2] != time_str.date) return 0;
	if (reportDate[1] != time_str.month) return 0;
	if (reportDate[0] != time_str.year) return 0;
	return 1;
}

INT8U PrtFormPrice(char *dest, INT8U cur_type, char *price_str, char *cur_sign1, char *cur_sign2, char flagCentOnly)
{
	switch (cur_type) {
		default:
		case 0: 					// 12.34
			sprintf((char *)dest, "%s", price_str);
			break;
		case 1: 					// $ 12.34
			sprintf((char *)dest, "%s %s", cur_sign1, price_str);	
			break;
		case 2: 					// 12.34 $
			sprintf((char *)dest, "%s %s", price_str, cur_sign1);
			break;
		case 3: 					// $ 12.34 c
			sprintf((char *)dest, "%s %s %s", cur_sign1, price_str, cur_sign2);
			break;
		case 4: 					// $ 12.34 c,	12 c
			if(flagCentOnly == 0) {
				sprintf((char *)dest, "%s %s %s", cur_sign1, price_str, cur_sign2);
			} else {
				sprintf((char *)dest, "%s %s", price_str, cur_sign2);
			}
			break;
		case 5: 					// $12.34,	$0.12
			sprintf((char *)dest, "%s%s", cur_sign1, price_str);	
			break;
		case 6: 					// 12.34$,	0.12$
			sprintf((char *)dest, "%s%s", price_str, cur_sign1);
			break;
		case 7: 					// $12.34c,	$0.12c
			sprintf((char *)dest, "%s%s%s", cur_sign1, price_str, cur_sign2);
			break;
		case 8: 					// $12.34c,	12c
			if(flagCentOnly == 0) {
				sprintf((char *)dest, "%s%s%s", cur_sign1, price_str, cur_sign2);
			} else {
				sprintf((char *)dest, "%s%s", price_str, cur_sign2);
			}
			break;
	}
	return strlen((char *)dest);
}

INT8U PrtFormUnitPrice(char *dest, INT8U cur_type, char *price_str, char *cur_sign1, char *w_sign, char *qty_sign, INT8U flag_pcs_unit, INT8U plu_type, INT16S unit_cnt)
{
	INT8U v8_1;
	
	switch (cur_type) {
		default:
		case 0: 					// 12.34,	12.34/5
			sprintf((char *)dest, "%s", price_str);
			break;
		case 1: 					// $/kg 12.34,	$/PCS 12.34,	$/PCS 12.34/5
			if (plu_type == PLU_BY_WEIGHT)
			{
				sprintf((char *)dest, "%s/%s %s", cur_sign1, w_sign, price_str);
			}
			else if ((plu_type == PLU_BY_COUNT) || (plu_type == PLU_BY_PCS))
			{
				sprintf((char *)dest, "%s/%s %s", cur_sign1, qty_sign, price_str);
				if(!prtsel_get_table(FIELD_ID_UNIT_PCS)) break;
				if(flag_pcs_unit && plu_type == PLU_BY_PCS) {			// by-count mode
					v8_1 = strlen(dest);
					sprintf((char *)dest+v8_1,"/%d", unit_cnt);
				}
			}
			break;
		case 2: 					// 12.34 $/kg,	12.34 $/PCS,	12.34/5 $/PCS
			if (plu_type == PLU_BY_WEIGHT)
			{
				sprintf((char *)dest, "%s %s/%s", price_str, cur_sign1, w_sign);
			}
			else if ((plu_type == PLU_BY_COUNT) || (plu_type == PLU_BY_PCS))
			{
				sprintf((char *)dest, "%s", price_str);
				if(prtsel_get_table(FIELD_ID_UNIT_PCS))
				{
					if(flag_pcs_unit && plu_type == PLU_BY_PCS) {			// by-count mode
						v8_1 = strlen(dest);
						sprintf((char *)dest+v8_1,"/%d", unit_cnt);
					}
				}
				v8_1 = strlen(dest);
				sprintf((char *)dest+v8_1, " %s/%s", cur_sign1, qty_sign);
			}
			break;
		case 3: 					// $/kg12.34,	$/PCS12.34,		$/PCS12.34/5
			if (plu_type == PLU_BY_WEIGHT)
			{
				sprintf((char *)dest, "%s/%s%s", cur_sign1, w_sign, price_str);
			}
			else if ((plu_type == PLU_BY_COUNT) || (plu_type == PLU_BY_PCS))
			{
				sprintf((char *)dest, "%s/%s%s", cur_sign1, qty_sign, price_str);
				if(!prtsel_get_table(FIELD_ID_UNIT_PCS)) break;
				if(flag_pcs_unit && plu_type == PLU_BY_PCS) {			// by-count mode
					v8_1 = strlen(dest);
					sprintf((char *)dest+v8_1,"/%d", unit_cnt);
				}
			}
			break;
		case 4: 					// 12.34$/kg,	12.34$/PCS,		12.34/5$/PCS
			if (plu_type == PLU_BY_WEIGHT)
			{
				sprintf((char *)dest, "%s%s/%s", price_str, cur_sign1, w_sign);
			}
			else if ((plu_type == PLU_BY_COUNT) || (plu_type == PLU_BY_PCS))
			{
				sprintf((char *)dest, "%s", price_str);
				if(prtsel_get_table(FIELD_ID_UNIT_PCS))
				{
					if(flag_pcs_unit && plu_type == PLU_BY_PCS) {			// by-count mode
						v8_1 = strlen(dest);
						sprintf((char *)dest+v8_1,"/%d", unit_cnt);
					}
				}
				v8_1 = strlen(dest);
				sprintf((char *)dest+v8_1, "%s/%s", cur_sign1, qty_sign);
			}
			break;
		case 5: 					// $12.34/kg,	$12.34/PCS,		$12.34/5/PCS
			if (plu_type == PLU_BY_WEIGHT)
			{
				sprintf((char *)dest, "%s%s/%s", cur_sign1, price_str, w_sign);
			}
			else if ((plu_type == PLU_BY_COUNT) || (plu_type == PLU_BY_PCS))
			{
				sprintf((char *)dest, "%s%s/", cur_sign1, price_str);
				if(prtsel_get_table(FIELD_ID_UNIT_PCS))
				{
					if(flag_pcs_unit && plu_type == PLU_BY_PCS) {			// by-count mode
						v8_1 = strlen(dest);
						sprintf((char *)dest+v8_1,"%d", unit_cnt);
					}
				}
				v8_1 = strlen(dest);
				sprintf((char *)dest+v8_1, "%s", qty_sign);
			}
			break;
	}
	return strlen((char *)dest);
}

INT8U PrtGenCurPriceStr(char *dest, long data, char cur_type, char *cur_sign1, char *cur_sign2, char decimal, char decimal_ch)
{
	char price_str[20];
	char flagCentOnly, len;

	flagCentOnly = 0;
	if(data < power10(decimal) && (cur_type == 4 || cur_type == 8)) {
		decimal = 0;
		flagCentOnly = 1;
	}

	format_value((HUGEDATA INT8U *)price_str, data, decimal, decimal_ch);
	len = PrtFormPrice(dest, cur_type, price_str, cur_sign1, cur_sign2, flagCentOnly);
	return len;
}

INT8U PrtGenCurUnitPriceStr(char *dest, long data, char cur_type, char decimal, char decimal_ch, INT8U flag_pcs_unit, INT8U plu_type, INT8U pcssym_no, INT16S unit_cnt)
{
	char price_str[20];
	char qty_sign[17];
	char w_sign[5];
	char cur_sign[5];
	char len;
	INT32U addr;
	INT16U qty_symbol, v16_1;

	// Weight Symbol
	if(PrtWeightUnit == WEIGHT_UNIT_KG || PrtWeightUnit == WEIGHT_UNIT_G) {
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)w_sign, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)w_sign, 4);
	}
	w_sign[4] = 0;

	// Unit Price Symbol
	get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)cur_sign,4);
	cur_sign[4] = 0;
	
	// Qty Symbol
	if(pcssym_no) {
		addr =  DFLASH_BASE + FLASH_PCS_UNIT_AREA;
		addr =  addr + PCS_UNIT_STRUCT_SIZE * (INT32U)(pcssym_no-1);	// code-1 (?)
		v16_1 = Flash_wread(addr);
		if(pcssym_no == (INT8U)v16_1) {
			addr += 2;
			Flash_sread(addr, (HUGEDATA INT8U *)qty_sign, PCS_UNIT_STRUCT_SIZE-2);
		}
	} else {
		get_global_sparam(GLOBAL_PCS_SYMBOL,(INT8U *)qty_sign,16);
		qty_sign[16] = 0;
	}

	format_value((HUGEDATA INT8U *)price_str, data, decimal, decimal_ch);
	len = PrtFormUnitPrice(dest, cur_type, price_str, cur_sign, w_sign, qty_sign, flag_pcs_unit, plu_type, unit_cnt);
	return len;
}

INT8U PrtGenCurPriceStr_2long(char *dest, long data_h8, long data_l8, char cur_type, char *cur_sign1, char *cur_sign2, char decimal, char decimal_ch)
{
	char price_str[20];
	char flagCentOnly, len;

	flagCentOnly = 0;
	if(data_h8 == 0 && data_l8 < power10(decimal) && (cur_type == 4 || cur_type == 8)) {
		decimal = 0;
		flagCentOnly = 1;
	}
	format_value_2long((HUGEDATA INT8U *)price_str, data_h8, data_l8, decimal, decimal_ch);
	len = PrtFormPrice(dest, cur_type, price_str, cur_sign1, cur_sign2, flagCentOnly);
	return len;
}


INT8U PrtGenSystemCurPriceStr(INT8U mode, HUGEDATA INT8U *dest, long data, INT8U decimal, char decimal_ch)
{
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN	
	char currency_sign1[6];
	char currency_sign2[4];
#else
	char currency_sign1[5];
	char currency_sign2[5];
#endif
	INT8U currency_type;
	INT8U ret;

#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,5);
	get_global_sparam(GLOBAL_AREA_PRICE_LASTSIGN+1, (INT8U *)currency_sign2,3);
	currency_sign1[5] = ' ';	// 아랍어 마지막 문자 종성 변환
	currency_sign1[6] = 0;
	currency_sign2[3] = 0;
#else
	get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
	get_global_sparam(GLOBAL_AREA_PRICE_LASTSIGN   ,(INT8U *)currency_sign2,4);
	currency_sign1[4] = 0;
	currency_sign2[4] = 0;
#endif

	currency_type = get_global_bparam(GLOBAL_AREA_PRICE_TYPE);
	if(mode == OUTMODE_LABEL) {
		currency_type /= 0x10;
		//if((currency_type > 8) || (currency_type < 0)) currency_type = 0;
		//ret = PrtGenCurPriceStr((char*)dest, data, currency_type, currency_sign1, currency_sign2, decimal, decimal_ch);
	}
#ifdef USE_ARAB_TICKET_PRT
	else if (mode == OUTMODE_TICKET_REPORT) { // 이란의 경우 화폐기호가 길어서 리포트에서 화폐기호없이  금액만 인쇄
		currency_type = 0x00; 
	}	
#endif
    else {	// OUTMODE_TICKET, OUTMODE_DISPLAY
		currency_type &= 0x0f;
		//if((currency_type > 8) || (currency_type < 0)) currency_type = 0;
		//ret = PrtGenCurPriceStr((char*)dest, data, currency_type, currency_sign1, currency_sign2, decimal, decimal_ch);
	}
	if(currency_type > 8)currency_type = 0;
	ret = PrtGenCurPriceStr((char*)dest, data, currency_type, currency_sign1, currency_sign2, decimal, decimal_ch);
	return ret;
}

#ifdef USE_KOREAN_IN_GBR
//USE_KOREAN IN GBR 버전, 작은 폰트 화폐단위에서 GBP을 표기하도록 수정됨. report 메뉴에서 2byte fonts 시 께짐 
INT8U PrtGenSystemCurPriceStr2(INT8U mode, HUGEDATA INT8U *dest, long data, INT8U decimal, char decimal_ch)
{
	char currency_sign1[5];
	char currency_sign2[5];
	INT8U currency_type;
	INT8U ret;

//	get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
	sprintf(currency_sign1, "GBP");
	get_global_sparam(GLOBAL_AREA_PRICE_LASTSIGN   ,(INT8U *)currency_sign2,4);
	currency_sign1[4] = 0;
	currency_sign2[4] = 0;

	currency_type = get_global_bparam(GLOBAL_AREA_PRICE_TYPE);
	if(mode == OUTMODE_LABEL) {
		currency_type /= 0x10;
		//if((currency_type > 8) || (currency_type < 0)) currency_type = 0;
		//ret = PrtGenCurPriceStr((char*)dest, data, currency_type, currency_sign1, currency_sign2, decimal, decimal_ch);
	} else {	// OUTMODE_TICKET, OUTMODE_DISPLAY
		currency_type &= 0x0f;
		//if((currency_type > 8) || (currency_type < 0)) currency_type = 0;
		//ret = PrtGenCurPriceStr((char*)dest, data, currency_type, currency_sign1, currency_sign2, decimal, decimal_ch);
	}
	if((currency_type > 8) || (currency_type < 0)) currency_type = 0;
	ret = PrtGenCurPriceStr((char*)dest, data, currency_type, currency_sign1, currency_sign2, decimal, decimal_ch);
	return ret;
}
#endif	//#ifdef USE_KOREAN_IN_GBR

INT8U PrtGenWeightStr(char *dest, long weight, char weight_type, char *weight_sign1, char *weight_sign2, char decimal, char decimal_ch)
{
	char weight_str[20];
	long lb_val, oz_val, rate;
	INT8U temp;

	//if(ADM_GetData.CurUnit == WEIGHT_UNIT_KG || ADM_GetData.CurUnit == WEIGHT_UNIT_G) {
	if(CurPluWeightUnit == WEIGHT_UNIT_KG || CurPluWeightUnit == WEIGHT_UNIT_G) {
		rate = 1000;
		if(weight_type == 3 || weight_type == 6) weight_type -= 1;//weight_type = 0;	// Modified by CJK 20051111
	} else {
		rate = 16;
	}
	lb_val = weight / power10(decimal);
	oz_val = weight % power10(decimal);
	oz_val = oz_val * rate;
	oz_val = oz_val * 10 + 5;
	oz_val = oz_val / power10(decimal+1);

	if (!status_scale.display_dummydigit)
	{
		if(weight_type == 0 || weight_type == 2 || weight_type == 5) {
			if(decimal > 0) {
				//if(ADM_Status.Dual == 1 && (ADM_Status.Capa%3) == 2) {	     	// 30kg
				if(ADM_Status.Dual == 1 && ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 0) == 30000) {	// 30kg, 30lb
					if(weight >= 15000) {
						decimal--;
						weight/=10;
					}
				}
				//SG070419 weight print value cut  
				else  
				{
					temp = get_global_bparam(GLOBAL_WEIGHT_ROUND);
					temp = (temp >> 6) & 0x03;
					decimal -= temp ;
					weight/=power10(temp);
				}
			}

		}
	}
	////////////////////////
	format_value((HUGEDATA INT8U *)weight_str, weight, decimal, decimal_ch);
	switch (weight_type) {
		default:
		case 0: 					// 12.345	// 12.345
			sprintf((char *)dest, "%s", weight_str);
			break;
		case 1: 					// 198		// 12345
			sprintf((char *)dest, "%ld", lb_val*rate+oz_val);
			break;
		case 2: 					// 12.345 lb	// 12.345 kg
			sprintf((char *)dest, "%s %s", weight_str, weight_sign1);
			break;
		case 3: 					// 12 lb 6 oz	// 12 kg 345 g
			sprintf((char *)dest, "%ld %s %ld %s", lb_val, weight_sign1, oz_val, weight_sign2);
			break;
		case 4: 					// 198 oz	// 12345 g
			sprintf((char *)dest, "%ld %s", lb_val*rate+oz_val, weight_sign2);
			break;
		case 5: 					// 12.345lb	// 12.345kg
			sprintf((char *)dest, "%s%s", weight_str, weight_sign1);
			break;
		case 6: 					// 12lb 6oz	// 12kg 345g
			sprintf((char *)dest, "%ld%s %ld%s", lb_val, weight_sign1, oz_val, weight_sign2);
			break;
		case 7: 					// 198oz	// 12345g
			sprintf((char *)dest, "%ld%s", lb_val*rate+oz_val, weight_sign2);
			break;
#ifdef USE_USA_FIXEDWEIGHT_UNIT_OZ	
		case 8: 					// 12 lb 6 oz, 5613g  USA 전용 
			lb_val= weight/rate;
			oz_val= weight%rate;
//			rate = ad_cnv_weight_unit(weight,WEIGHT_UNIT_LB,WEIGHT_UNIT_KG);
			rate= longmuldiv(weight, 283495L, 10000L);  //oz to g
			sprintf((char *)dest, " (%ld %s %ld %s) %ldg", lb_val, weight_sign1, oz_val, weight_sign2, rate);
			break;
		case 9: 					// 198oz	// 12345g USA 전용 
			sprintf((char *)dest, "%ld%s", weight, weight_sign2);
			break;
#endif
	}
	return strlen((char *)dest);
}


void PrtMakePriceString(char *dst_str, long value, INT8U decimal, char dec_ch, INT8U digit)
{
	char temp_string[50];
	char form_string[10];

#ifdef USE_ARAB_TICKET_PRT
    PrtGenSystemCurPriceStr(OUTMODE_TICKET_REPORT, ( INT8U *)temp_string, value, decimal, dec_ch);
#else
	PrtGenSystemCurPriceStr(OUTMODE_TICKET, (HUGEDATA INT8U *)temp_string, value, decimal, dec_ch);
#endif

	if(digit) {
		sprintf(form_string, "%%%ds", digit);
		sprintf(dst_str, form_string, temp_string);
	} else {
		strcpy(dst_str, temp_string);
	}
}

void GetWeightSymbol(INT8U unit, char *unit_str)
{
	if(unit == WEIGHT_UNIT_KG) {
#ifdef USE_WEIGHT_UNIT_500G
		if(PrtWeightUnitId==3) get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)unit_str, 4);
		else get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)unit_str, 4);
#else
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)unit_str, 4);
#endif
	} else if(unit == WEIGHT_UNIT_G) {
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (INT8U *)unit_str, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)unit_str, 4);
	}
	unit_str[4] = 0;
}


void PrtMakeWeightString(char *dst_str, long value, INT8U decimal, char dec_ch, INT8U digit)
{
	INT8U len;
	char temp_string[50];
	char form_string[10];
	char ad_unit_str[5];

	GetWeightSymbol(CurPluWeightUnit, ad_unit_str);// Modified by CJK 20051223

	format_value((HUGEDATA INT8U *)temp_string, value, decimal, dec_ch);
	len = strlen(temp_string);
	temp_string[len++] = ' ';
	strcpy(temp_string+len, ad_unit_str);

	if(digit) {
		sprintf(form_string, "%%%ds", digit);
		sprintf(dst_str, form_string, temp_string);
	} else {
		sprintf(dst_str, temp_string);
	}
}

void get_summary_sum_addr(INT32U base, INT16U start_no, INT16U end_no, INT16S period, INT16S *transaction, long *volume)
{
	SALES_SUMMARY_TYPE sst;
	INT16U i;

	*transaction = 0;
	*volume      = 0;
	for(i = start_no; i <= end_no; i++) {
		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(base, i);
		get_nvram_summary_addr(base, i, &sst);
		*transaction += sst.sstTransactions[period];
		*volume      += sst.sstVolume[period];
	}
}

#ifdef USE_REPORT_PAY	//#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
void get_pay_summary_sum_addr(INT16U start_no, INT16U end_no, INT16U paytype, INT16S period, INT16S *transaction, long *volume)
{
	SALES_SUMMARY_TYPE sst;
	INT16U i;

	*transaction = 0;
	*volume      = 0;
	for(i = start_no; i <= end_no; i++) {
		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_SCALE, i*(MAX_PAID_TYPE)+paytype);
		get_nvram_summary_addr(NVRAM_PAY_SCALE, i*(MAX_PAID_TYPE)+paytype, &sst);
		*transaction += sst.sstTransactions[period];
		*volume      += sst.sstVolume[period];
	}
}
#endif


#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
void get_tax_summary_sum(INT16U start_no, INT16U end_no, INT16U taxtype, INT16S period, long *value)
{
	INT16U i;
	INT32U addr;

	*value = 0;
	//addr = NVRAM_TAX + (INT32U)(i*MAX_TAX_NO + (taxtype-1)*MAX_REPORT_PERIOD + period) * 4;
	addr = NVRAM_TAX;
	addr += period*4;
	addr += (taxtype-1)*MAX_REPORT_PERIOD*4;
	for(i = start_no; i <= end_no; i++) {
		*value += get_nvram_lparam(addr+(INT32U)i*MAX_TAX_NO*MAX_REPORT_PERIOD*4);
	}
}

void get_tax_gross_summary_sum(INT16U start_no, INT16U end_no, INT16U taxtype, int period, long *value)
{
	INT16U i;
	INT32U addr;

	*value = 0;
	//addr = NVRAM_TAX + (INT32U)(i*MAX_TAX_NO + (taxtype-1)*MAX_REPORT_PERIOD + period) * 4;
	addr = NVRAM_TAX_GROSS;
	addr += period*4;
	addr += (taxtype-1)*MAX_REPORT_PERIOD*4;
	for(i = start_no; i <= end_no; i++) {
		*value += get_nvram_lparam(addr+(INT32U)i*MAX_TAX_NO*MAX_REPORT_PERIOD*4);
	}
}
#endif

void get_cashopen_summary_sum(INT16U start_no, INT16U end_no, INT16S period, INT16S *value)
{
	INT16U i;

	*value = 0;
	for(i = start_no; i <= end_no; i++) {
		*value += get_nvram_wparam(NVRAM_CASH_SCALE+(i*MAX_REPORT_PERIOD+period)*2);
	}
}

void Report_PrintBasicData(BASIC_REPORT_TYPE *br)
{
	char string_buf[100], digit_form[10];
	INT8U decimal, dec_ch, w_decimal;
	INT8S variable_len, common_len;

	w_decimal=get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);
	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
	if(variable_len < 0) variable_len = 0;
	common_len = REPORT_PRICE_LENGTH + REPORT_COUNT_LENGTH + variable_len;
	sprintf(digit_form, "%%%dd", common_len);

	//RcptCmd_CharTab(TICKET_MARGIN);
	//RcptCmd_PrintLine(1, 0);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_TRAN_COUNT, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharTab(6);
	sprintf(string_buf, digit_form, br->brtTransactions);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_WEIGHT, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharTab(6);
	PrtMakeWeightString(string_buf, br->brtWeight, w_decimal, dec_ch, common_len);
	PutDataRxBuf(&CommBufPrt,string_buf,common_len);
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_QUANTITY, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharTab(6);
	sprintf(string_buf, digit_form, br->brtQty);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_PRICE, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharTab(6);
	PrtMakePriceString(string_buf, br->brtVolume, decimal, dec_ch, common_len);
	PutDataRxBuf(&CommBufPrt,string_buf,common_len);
	RcptCmd_CharLF(1);

	//RcptCmd_CharTab(TICKET_MARGIN);
	//RcptCmd_PrintLine(1, 0);
	//RcptCmd_CharLF(1);
}

extern INT8U prev_just;
void  Report_LabelText(INT16S x,INT16S y,INT16S fontsize_w,INT16S fontsize_h,INT16S h,char *str)
{
	char line[70];
	INT16S  w;

	w=448-x;	// 56*8
	LabCmd_SetStrField(x, y, h, w);
	prev_just=-1;
	RcptCmd_StrJust(LEFT);
	LabCmd_CharSize(fontsize_w,fontsize_h);
	LabCmd_StrStart();
	sprintf((char *)line,"%s",str);
	PutDataRxBuf(&CommBufPrt,(HUGEDATA char*)line,strlen(line));
	LabCmd_StrEnd();
}

INT8U Report_SaveUsbMem(INT8U rep_type,INT8U mode, INT16S period, INT16U start_dept, INT16U end_dept, long start_no, long end_no)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	PLU_SALES_SUMMARY_TYPE *psr;
	PLU_SALES_SUMMARY_TYPE pst;
	INT16U next;
	INT16U i;
	INT16S  uz,line;
	INT8U printon[4];
	char report_header[40];
	char report_start[55]; // pluname(20) + 시간,날짜(24) + 6 + NULL(1)
	char report_end[75]; // 시간,날짜(24) + report_start(50)
	char report_mid[40];
	char stemp[40],pluname[40];
	char usb_buf[1024];

	INT8U decimal, dec_ch,w_decimal;
	RTC_STRUCT time_str;
	static long t_sales,t_price,t_weight,t_qty;
	INT16U index;
	INT16U plu_key;
	INT16S j;
	POINT disp_p;
	INT8U ret;

	disp_p.x = 0;
	disp_p.y = 0;
	Dsp_Write_String(disp_p, "waiting..");
	Dsp_Diffuse();
	
	
	sr = &sst;
	psr = &pst;

	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	//w_dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	w_decimal=get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);

	if (rep_type == REPORT_SCALE && start_no == 0) {
		start_no = 1;
		end_no = MAX_SCALE - 1;
	}
	t_sales  =0;
	t_price  =0;
	t_weight=0;
	t_qty    =0;

	sprintf(report_header, "%c%d ", ReportTypeChar[mode], period+1);

	receipt_caption_read(RCPTCAP_ID_REP_LAST_RESET, pluname);
	pluname[20] = 0;
	Report_ReadResetTime(NVRAM_RESET_TIME_PLU, period, &time_str);
	i=strlen(report_start);
	Print_getTimeStr(&time_str,stemp,1); // 24 byte
	sprintf(report_start,"   %s : %s",pluname,stemp); // 3+20+3+24 = 50 byte
	Report_GetCurTime(&time_str,0);
	Print_getTimeStr(&time_str,report_end,1); // 24 byte
	strcat(report_end, report_start);
	for (j=0; j<4; j++) printon[j]=1;

	switch (rep_type) {
//--- PLU
		case REPORT_MISCPLU:
		case REPORT_PLU:
			if(GlbOper.PluReportType) return ON;	//Parameter 715 : Y, PLU정산 영역 사용안함
			index = get_nvram_wparam(NVRAM_PLUSUM_HEADINDEX);
			receipt_caption_read(RCPTCAP_ID_REP_PLU_NO,stemp);
			if (rep_type==REPORT_MISCPLU) {
				start_no+=MISC_PLU_WEIGHT;
				end_no  +=MISC_PLU_WEIGHT;
			}
			if (start_no >= MISC_PLU_WEIGHT) {	// Misc PLU.
				receipt_caption_read(RCPTCAP_ID_REP_TITLE_MISC, report_header+3);
				uz=0;
				line  =(INT16S)(start_no-MISC_PLU_WEIGHT);
				j     =(INT16S)(end_no-MISC_PLU_WEIGHT);
				for (i=line; i<=j; i++) {
					receipt_caption_read(RCPTCAP_ID_COM_MISC_WEIGHT+i,pluname);
					if (i!=j)
						sprintf(&report_mid[uz],"%s,",pluname);
					else
						sprintf(&report_mid[uz],"%s",pluname);
					uz=strlen(report_mid);
				}
			} else {				// PLU
				receipt_caption_read(RCPTCAP_ID_REP_TITLE_PLU, report_header+3);
	 			if ((start_dept==end_dept) && (start_no==end_no)) {
					get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, psr);
 					plu_table_search(start_dept, start_no,&plu_key,0);
 					plu_get_field(plu_key-1,PTYPE_PLU_NAME1,(INT8U*)pluname,(INT16S*)&uz,32);
					sprintf(report_mid,"%s: %ld, %s",stemp,start_no,pluname);
				} else {
					sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
				}
			}
			for (j = start_dept; j <= end_dept; j++) {
				//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index);
				get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, psr);
				while (psr->pstPLU < start_no ) {
					if(psr->pstNext == 0) break;
					//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
					next = psr->pstNext;
					get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
				}
				while(psr->pstPLU >= start_no && psr->pstPLU <= end_no) {
					if(psr->pstDept == j) {
						if(plu_table_search(psr->pstDept, psr->pstPLU,&plu_key,0) || psr->pstPLU >= MISC_PLU_WEIGHT) {	// Added by CJK 20060510
							t_price  +=psr->pstVolume[period];
							t_qty    +=psr->pstQty[period];
							t_weight+=psr->pstWeight[period];
							t_sales  +=psr->pstTransactions[period];
						}
					}
					if(psr->pstNext == 0) break;
					//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
					next = psr->pstNext;
					get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
				}
			}
			break;
//--  Scale
		case REPORT_SCALE:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_SCALE, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i, sr);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
			}
//			printon[2]=printon[3]=0;
			break;
//-- MISC
//	plu인경우와 동일
//-- Group
		case REPORT_GROUP:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_GROUP, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_GROUP_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i, sr);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
			}
//			printon[2]=printon[3]=0;
			break;
//-- Dept	
		case REPORT_DEPT:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_DEPT, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_DEPT_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i, sr);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
			}
//			printon[2]=printon[3]=0;
			break;
//-- Hour	
		case REPORT_HOURLY:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_HOUR, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_HOUR_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i, sr);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
			}
//			printon[2]=printon[3]=0;
			break;
		case REPORT_CLERK:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_CLERK, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_CLERK_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i, sr);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
			}
	}
	
	sprintf(usb_buf, "%s\r\n%s\r\n%s\r\n", report_header, report_end, report_mid);

	if (printon[0]) {
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT,stemp);
		sprintf(report_end,"%-12s: %ld\r\n",stemp,t_sales);
		strcat(usb_buf, report_end);
	}
	if (printon[1]) {
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL,stemp);
		PrtMakePriceString(report_mid,t_price, decimal, dec_ch,14);
		sprintf(report_end,"%-12s: %s\r\n",stemp,str_trimleft(report_mid));
		strcat(usb_buf, report_end);
	}
	line+=32;
	if (printon[2]) {
		receipt_caption_read(RCPTCAP_ID_REP_QUANTITY,stemp);
		sprintf(report_end,"%-12s: %ld\r\n",stemp,t_qty);
		strcat(usb_buf, report_end);
	}
	if (printon[3]) {
		receipt_caption_read(RCPTCAP_ID_REP_WEIGHT,stemp);
		PrtMakeWeightString(report_mid,t_weight, w_decimal, dec_ch,14);
		sprintf(report_end,"%-12s: %s\r\n\r\n",stemp,str_trimleft(report_mid));
		strcat(usb_buf, report_end);
	}
	
	ret = usbmem_save_report((INT8U*)"report.txt", (INT8U*)usb_buf);
	
	Delay_100ms(10); // "waiting.."표시를 위해 1초 지연
	Dsp_Write_String(disp_p, "         ");
	Dsp_Diffuse();
	
	if (!ret) 
	{
		BuzOn(2);
		return OFF;
	}
	return ON;
}

extern INT8U PrtFontType;
extern void LabCmd_ClearLabel(void);
extern void StrInit4UserLabel(void);

INT8U Report_PrintLabel(INT8U rep_type,INT8U mode, INT16S period, INT16U start_dept, INT16U end_dept, long start_no, long end_no)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	PLU_SALES_SUMMARY_TYPE *psr;
	PLU_SALES_SUMMARY_TYPE pst;
	INT16U next;
	INT16U i;
	INT16S  uz,line;
	INT32U pnt;
	INT8U h_form;
	INT8U printon[4];
	char report_header[40];
	char report_start[45];
	char report_end[70];
	char report_mid[40];
	char stemp[40],pluname[40];
#ifdef USE_PRT_SCALE_NUM_IN_REPORT
	char report_scale_num[40];
#endif
#ifdef USE_INDIA_TAX_REPORT_MENU
	long volume;
#endif
	INT8U decimal, dec_ch,w_decimal;
	RTC_STRUCT time_str;
	static long t_sales,t_price,t_weight,t_qty;
	INT16U index;
	INT16U plu_key;
	INT16S j;

	sr = &sst;
	psr = &pst;

	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	//w_dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	w_decimal=get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);

	if (rep_type == REPORT_SCALE && start_no == 0) {
		start_no = 1;
		end_no = MAX_SCALE - 1;
	}
	t_sales  =0;
	t_price  =0;
	t_weight=0;
	t_qty    =0;

	//decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	//dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	sprintf(report_header, "%c%d ", ReportTypeChar[mode], period+1);

	receipt_caption_read(RCPTCAP_ID_REP_LAST_RESET, pluname);
	pluname[12] = 0;
	Report_ReadResetTime(NVRAM_RESET_TIME_PLU, period, &time_str);
	i=strlen(report_start);

#ifdef USE_PRT_SCALE_NUM_IN_REPORT
	receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, stemp);	//"SCALE #"
	sprintf(report_scale_num, "%s%02d", stemp, status_scale.scaleid);
#endif

	Print_getTimeStr(&time_str,stemp,1); // 24 byte
	sprintf(report_start,"   %s : %s",pluname,stemp); // 3+12+3+24 = 42 byte
	Report_GetCurTime(&time_str,0);
	Print_getTimeStr(&time_str,report_end,1); // 24 byte
	strcat(report_end, report_start);
	for (j=0; j<4; j++) printon[j]=1;

	switch (rep_type) {
//--- PLU
		case REPORT_MISCPLU:
		case REPORT_PLU:
			if(GlbOper.PluReportType) return ON;	//Parameter 715 : Y, PLU정산 영역 사용안함
			index = get_nvram_wparam(NVRAM_PLUSUM_HEADINDEX);
			receipt_caption_read(RCPTCAP_ID_REP_PLU_NO,stemp);
			if (rep_type==REPORT_MISCPLU) {
				start_no+=MISC_PLU_WEIGHT;
				end_no  +=MISC_PLU_WEIGHT;
			}
			if (start_no >= MISC_PLU_WEIGHT) {	// Misc PLU.
				receipt_caption_read(RCPTCAP_ID_REP_TITLE_MISC, report_header+3);
				uz=0;
				line  =(INT16S)(start_no-MISC_PLU_WEIGHT);
				j     =(INT16S)(end_no-MISC_PLU_WEIGHT);
				for (i=line; i<=j; i++) {
					receipt_caption_read(RCPTCAP_ID_COM_MISC_WEIGHT+i,pluname);
					if (i!=j)
						sprintf(&report_mid[uz],"%s,",pluname);
					else
						sprintf(&report_mid[uz],"%s",pluname);
					uz=strlen(report_mid);
				}
			} else {				// PLU
				receipt_caption_read(RCPTCAP_ID_REP_TITLE_PLU, report_header+3);
	 			if ((start_dept==end_dept) && (start_no==end_no)) {
					//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index);
					get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, psr);
//-[lim060317
 					plu_table_search(start_dept, start_no,&plu_key,0);
//-[end
 					plu_get_field(plu_key-1,PTYPE_PLU_NAME1,(INT8U*)pluname,(INT16S*)&uz,32);
					sprintf(report_mid,"%s: %ld, %s",stemp,start_no,pluname);
				} else {
					sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
				}
			}
			for (j = start_dept; j <= end_dept; j++) {
				//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index);
				get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, psr);
				while (psr->pstPLU < start_no ) {
					if(psr->pstNext == 0) break;
					//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
					next = psr->pstNext;
					get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
				}
				while(psr->pstPLU >= start_no && psr->pstPLU <= end_no) {
					if(psr->pstDept == j) {
						if(plu_table_search(psr->pstDept, psr->pstPLU,&plu_key,0) || psr->pstPLU >= MISC_PLU_WEIGHT) {	// Added by CJK 20060510
							t_price  +=psr->pstVolume[period];
							t_qty    +=psr->pstQty[period];
							t_weight+=psr->pstWeight[period];
							t_sales  +=psr->pstTransactions[period];
						}
					}
					if(psr->pstNext == 0) break;
					//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
					next = psr->pstNext;
					get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
				}
			}
			break;
//--  Scale
		case REPORT_SCALE:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_SCALE, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i);
				get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i, sr);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
			}
//			printon[2]=printon[3]=0;
			break;
//-- MISC
//	plu인경우와 동일
//-- Group
		case REPORT_GROUP:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_GROUP, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_GROUP_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i);
				get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i, sr);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
			}
//			printon[2]=printon[3]=0;
			break;
//-- Dept	
		case REPORT_DEPT:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_DEPT, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_DEPT_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				// Sale
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i);
				get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i, sr);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
				// Prepack
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_DEPT, i);
				// Return
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_DEPT, i);
			}
//			printon[2]=printon[3]=0;
			break;
//-- Hour	
		case REPORT_HOURLY:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_HOUR, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_HOUR_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i);
				get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i, sr);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
			}
//			printon[2]=printon[3]=0;
			break;
		case REPORT_CLERK:
			receipt_caption_read(RCPTCAP_ID_REP_TITLE_CLERK, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_CLERK_NO,stemp);
			if (start_no==end_no) {
				sprintf(report_mid,"%s: %ld",stemp,start_no);
			} else {
				sprintf(report_mid,"%s: %ld ~ %ld",stemp,start_no,end_no);
			}
			for(i = start_no; i <= end_no; i++) {
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i);
				get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i, sr);
//				sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, i);
				t_sales += sr->sstTransactions[period];
				t_price += sr->sstVolume[period];
				t_weight+= sr->sstWeight[period];
				t_qty	+= sr->sstQty[period];
			}
			break;
#ifdef USE_INDIA_TAX_REPORT_MENU
		case REPORT_TAX:
			receipt_caption_read(RCPTCAP_ID_REP_TAX_SUMMARY, report_header+3);
			receipt_caption_read(RCPTCAP_ID_REP_TAX_PRICE,stemp);
			sprintf(report_mid,"%s: %ld ~ %ld",stemp,1,MAX_TAX_NO);
			for(j = 1; j < MAX_TAX_NO; j++) {
				get_tax_summary_sum(1, MAX_SCALE-1, j, period, &volume);
				t_price += volume;
			}
			break;
#endif
	}
// Initial
	LabCmd_ClearLabel();
	StrInit4UserLabel();	// Added by CJK 20060426
	RcptCmd_SetFontType(PrtFontType);//0=one byte font, 1=two byte font //SG060313
	RcptCmd_SetFontId(3);
	RcptCmd_IntercharSP(0);
//	RcptCmd_StrJust(0);
//	LabCmd_SetStrField(40, 80, 200, 428);
//	LabCmd_CharSize(1, 1);
// Data
//	LabCmd_CharSize(1, 1);
	line=8;
#ifdef USE_ARAB_CONVERT
	Report_LabelText(224,line,2,2,32,report_header);	line+=32;	// 28 * 8
	Report_LabelText(85,line,1,2,32,report_end);	line+=32;		// 10 * 8
	Report_LabelText(288,line,2,2,32,report_mid);	line+=32;		// 36 * 8
#else
	Report_LabelText(20,line,2,2,32,report_header);	line+=32;
#ifdef USE_PRT_SCALE_NUM_IN_REPORT
	if(rep_type != REPORT_SCALE)
		Report_LabelText(20,line,1,2,32,report_scale_num);  line+=32;
#endif
	Report_LabelText(20,line,1,2,32,report_end);	line+=32;
//	Report_LabelText(10,line,1,2,32,report_start);	line+=32;
	Report_LabelText(20,line,2,2,32,report_mid);	line+=32;
//	Report_LabelText(10,line,2,2,32,report_header);	line+=32;
//	Report_LabelText(10,line,1,2,32,report_end);	line+=32;
////	Report_LabelText(10,line,1,2,32,report_start);	line+=32;
//	Report_LabelText(10,line,2,2,32,report_mid);	line+=32;
#endif

	if (printon[0]) {
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT,stemp);
		sprintf(report_header,"%-12s: %ld",stemp,t_sales);
#ifdef USE_ARAB_CONVERT
		Report_LabelText(40,line,1,2,32,report_header);
#else
		Report_LabelText(10,line,1,2,32,report_header);
#endif
	}
	//line+=32;
	if (printon[1]) {
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL,stemp);
		PrtMakePriceString(report_mid,t_price, decimal, dec_ch,14);
//		str_trimleft(report_mid);
		sprintf(report_header,"%-12s: %s",stemp,str_trimleft(report_mid));
#ifdef USE_ARAB_CONVERT
		Report_LabelText(254,line,1,2,32,report_header);	// 32
#else
		Report_LabelText(224,line,1,2,32,report_header);	// 28
#endif
	}
	line+=32;
	if (printon[2]) {
		receipt_caption_read(RCPTCAP_ID_REP_QUANTITY,stemp);
		sprintf(report_header,"%-12s: %ld",stemp,t_qty);
#ifdef USE_ARAB_CONVERT
		Report_LabelText(40,line,1,2,32,report_header);
#else
		Report_LabelText(10,line,1,2,32,report_header);
#endif
	}
	//line+=32;
	if (printon[3]) {
		receipt_caption_read(RCPTCAP_ID_REP_WEIGHT,stemp);
		PrtMakeWeightString(report_mid,t_weight, w_decimal, dec_ch,14);
//		str_trimleft(report_mid);
		sprintf(report_header,"%-12s: %s",stemp,str_trimleft(report_mid));
#ifdef USE_ARAB_CONVERT
		Report_LabelText(254,line,1,2,32,report_header);	// 32
#else
		Report_LabelText(224,line,1,2,32,report_header);	// 28
#endif
	}

// Printing
	RcptCmd_TypeNone();
	LabCmd_PrintStart(1);
	PrintListWaitEnd();
	return ON;
}

INT8U Report_PrintPluSalesSummary(INT8U mode, INT16S period, INT16U start_dept, INT16U end_dept, long start_no, long end_no, RTC_STRUCT *cur_time, BASIC_REPORT_TYPE *total_br)
{
	PLU_SALES_SUMMARY_TYPE *psr;
	PLU_SALES_SUMMARY_TYPE pst;
	INT16U next;
	INT16U j,uz;
	INT16U index;
	char string_buf[100], digit_form[10];
	INT32U flash_addr_dept, base;
	INT8U decimal, dec_ch, w_decimal, len;
	INT16U plu_key;
	DEPARTMENT_STRUCT dept_str;
	RTC_STRUCT time_str;
	INT8U flagStopPrint = OFF;
	INT8U flagReport;
	INT8S variable_len, common_len;
	INT16U shop_no;  //SG060105

	psr = &pst;

	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;
	PrtSensorFlagInit();

	flash_addr_dept = DFLASH_BASE + FLASH_DEPARTMENT_AREA;

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	//w_decimal=get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
	//w_decimal=get_decimal_pt(3, ADM_Status.UseUnit);
	//w_dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	flagReport =get_global_bparam(GLOBAL_USE_REPORT);
//	flash_addr = DFLASH_BASE + FLASH_PLU_AREA;
//	flash_addr+= plu_get_ptype_point(PTYPE_PLU_NAME1, NULL, &name_size);		// Name

	RcptCmd_ChangeMode(TO_RECEIPT, ON);	// Change to Receipt Mode
	RcptCmd_DefaultSet();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharLF(2);

	//SG060105_S
	if(GlbPrint.reportStoreName) {
		shop_no = get_net_wparam(NETWORK_STORE_ID);
		TotalRcpt_PrintShopName(shop_no);
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_PrintLine(1, 0);
	} //SG060105_E

	variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
	if(variable_len < 0) variable_len = 0;
	common_len = REPORT_PRICE_LENGTH + REPORT_COUNT_LENGTH + variable_len;
	sprintf(digit_form, "%%%dd", common_len);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, LEFT);
#endif
#ifdef USE_ARAB_TICKET_PRT
    if (start_no >= MISC_PLU_WEIGHT) {  // Misc PLU.
        receipt_caption_read(RCPTCAP_ID_REP_TITLE_MISC, string_buf);
    } else {                // PLU
        receipt_caption_read(RCPTCAP_ID_REP_TITLE_PLU, string_buf);
    }
    sprintf(string_buf+strlen(string_buf), " %c%d", ReportTypeChar[mode], period+1);
#else
	sprintf(string_buf, "%c%d ", ReportTypeChar[mode], period+1);

	if (start_no >= MISC_PLU_WEIGHT) {	// Misc PLU.
		receipt_caption_read(RCPTCAP_ID_REP_TITLE_MISC, string_buf+3);
	} else {				// PLU
		receipt_caption_read(RCPTCAP_ID_REP_TITLE_PLU, string_buf+3);
	}
#endif
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));

	RcptCmd_CharLF(2);
//	RcptCmd_CharLF(1);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintFixedTime(cur_time);
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_LAST_RESET, string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	Report_ReadResetTime(NVRAM_RESET_TIME_PLU, period, &time_str);
	PrintFixedTime(&time_str);
	RcptCmd_CharLF(1);

#ifdef USE_PRT_SCALE_NUM_IN_REPORT
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, string_buf);	//"SCALE #"
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", status_scale.scaleid);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
#endif

	base = 0;
	if(start_no >= MISC_PLU_WEIGHT) {
		base = MISC_PLU_WEIGHT;
	}
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
    RcptCmd_CharTab(TICKET_MARGIN);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
    RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%05ld, ", start_no-base);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%05ld", end_no-base);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
		
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
		
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);			
#endif
	if (GlbPrint.reportType)// transaction count, price, weight, quantity
	{
		Report_PrintBasicData(total_br);
	}
	else
	{
#ifdef USE_ARAB_CONVERT
		RcptCmd_CharTab(TICKET_MARGIN);
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(5);
#endif
		sprintf(string_buf, digit_form, total_br->brtTransactions);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	   	RcptCmd_CharLF(1);

		//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#ifdef USE_ARAB_CONVERT
		RcptCmd_CharTab(TICKET_MARGIN);
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(5);
#endif
		PrtMakePriceString(string_buf, total_br->brtVolume, decimal, dec_ch, common_len);
		PutDataRxBuf(&CommBufPrt,string_buf,common_len);
	   	RcptCmd_CharLF(1);
	}
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	for(j = start_dept; j <= end_dept && !flagStopPrint; j++) {
		if(start_no < MISC_PLU_WEIGHT) {
			Flash_sread(flash_addr_dept+(j-1)*DEPARTMENT_STRUCT_SIZE, (HUGEDATA INT8U *)&dept_str, sizeof(DEPARTMENT_STRUCT));
			if(dept_str._code != j) dept_str._description[0] = 0;
#ifdef USE_ARAB_TICKET_PRT
            RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
            RcptCmd_CharTab(TICKET_MARGIN);
#else
            RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
            RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_DEPT_NO, string_buf);
			len = strlen(string_buf);
			sprintf(string_buf+len, "%02d", j);
			_strupr(string_buf);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(5);
#endif
			PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)dept_str._description,strlen((HUGEDATA char *)dept_str._description));
			RcptCmd_CharLF(1);
			RcptCmd_TypeNone();
		//}
		}
		RcptCmd_CharTab(TICKET_MARGIN);
		RcptCmd_PrintLine(1, 0);

		index = get_nvram_wparam(NVRAM_PLUSUM_HEADINDEX);
		//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index);
		get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, psr);
		while(psr->pstPLU < start_no && !flagStopPrint) {
			if(psr->pstNext == 0) break;
			//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
			next = psr->pstNext;
			get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
		}
		while(psr->pstPLU >= start_no && psr->pstPLU <= end_no && !flagStopPrint) {
			if(KEY_Read()) {
				if(KeyDrv.CnvCode == KP_CLEAR) {
					BuzOn(1);
					flagStopPrint = ON;
					break;
				}
			}
			if(psr->pstDept == j) {
				if(psr->pstTransactions[period] || psr->pstWeight[period] || psr->pstQty[period] || psr->pstVolume[period] || (flagReport&0x01)) {
//					plu_key = plu_table_search(psr->pstDept, psr->pstPLU);
					plu_table_search(psr->pstDept, psr->pstPLU,&plu_key,0);
					if(plu_key || psr->pstPLU >= MISC_PLU_WEIGHT) {
#ifdef USE_ARAB_TICKET_PRT
						RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
						RcptCmd_CharTab(TICKET_MARGIN);
#else
						RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif

						receipt_caption_read(RCPTCAP_ID_REP_PLU_NO, string_buf);
						if(psr->pstPLU < MISC_PLU_WEIGHT) {
							len = strlen(string_buf);
							sprintf(string_buf+len, "%05ld", psr->pstPLU);
						}
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						w_decimal=get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);
						switch(psr->pstPLU) {
							case MISC_PLU_WEIGHT :
								receipt_caption_read(RCPTCAP_ID_COM_MISC_WEIGHT, string_buf);
								break;
							case MISC_PLU_PCS :
								receipt_caption_read(RCPTCAP_ID_COM_MISC_PCS, string_buf);
								break;
							case MISC_PLU_COUNT :
								receipt_caption_read(RCPTCAP_ID_COM_MISC_COUNT, string_buf);
								break;
							default:
//								if (name_size == 0) {
//									string_buf[0] = 0;
//								} else {
//									Flash_sread(flash_addr+plu_record_size*(plu_key-1),(HUGEDATA INT8U*)string_buf, 32);
#ifdef USE_ARAB_CONVERT
                                RcptCmd_CharLF(1);	
								string_buf[0] = ' ';
								plu_get_field(plu_key-1, PTYPE_PLU_NAME1,(INT8U*)&string_buf[1],(INT16S*)&uz,32);
								string_buf[strlen(string_buf)] = ' ';
#else
								plu_get_field(plu_key-1,PTYPE_PLU_NAME1,(INT8U*)string_buf,(INT16S*)&uz,32);
#endif
								string_buf[uz]=0;
//								}
								w_decimal=get_decimal_pt(4, plu_key);
								break;
						}
						string_buf[31] = 0;
#ifdef USE_ARAB_CONVERT
                        RcptCmd_CharTab(1);
#else
                        RcptCmd_CharTab(6);
#endif
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						RcptCmd_CharLF(1);
						if(GlbPrint.reportZeroValue || psr->pstTransactions[period] != 0)
						{
#ifdef USE_ARAB_TICKET_PRT						
							RcptCmd_CharTab(TICKET_MARGIN);
#else
							RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
							receipt_caption_read(RCPTCAP_ID_REP_TRAN_COUNT, string_buf);	//"TRANSACTION COUNT"
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
							RcptCmd_PrintArabAllign(string_buf,25);
#else
							RcptCmd_CharTab(6);
#endif
							sprintf(string_buf, digit_form, psr->pstTransactions[period]);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
							RcptCmd_CharLF(1);
						}

						if(GlbPrint.reportZeroValue || psr->pstWeight[period] != 0)
						{
#ifdef USE_ARAB_TICKET_PRT						
							RcptCmd_CharTab(TICKET_MARGIN);
#else
							RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
							receipt_caption_read(RCPTCAP_ID_REP_WEIGHT, string_buf);	//"WEIGHT"
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
							RcptCmd_PrintArabAllign(string_buf,25);
#else
							RcptCmd_CharTab(6);
#endif
							PrtMakeWeightString(string_buf, psr->pstWeight[period], w_decimal, dec_ch, common_len);
							PutDataRxBuf(&CommBufPrt,string_buf,common_len);
							RcptCmd_CharLF(1);
						}
						
						if(GlbPrint.reportZeroValue || psr->pstQty[period] != 0)
						{
							RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
							receipt_caption_read(RCPTCAP_ID_REP_QUANTITY, string_buf);	//"QUANTITY"
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
							RcptCmd_PrintArabAllign(string_buf,25);
#else
							RcptCmd_CharTab(6);
#endif
							sprintf(string_buf, digit_form, psr->pstQty[period]);
							PutDataRxBuf(&CommBufPrt,string_buf,common_len);
							RcptCmd_CharLF(1);
						}

						if(GlbPrint.reportZeroValue || psr->pstVolume[period] != 0)
						{
#ifdef USE_ARAB_TICKET_PRT						
							RcptCmd_CharTab(TICKET_MARGIN);
#else
							RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
							receipt_caption_read(RCPTCAP_ID_REP_PRICE, string_buf);	//"PRICE"
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
							RcptCmd_PrintArabAllign(string_buf,25);
#else
							RcptCmd_CharTab(6);
#endif
							PrtMakePriceString(string_buf, psr->pstVolume[period], decimal, dec_ch, common_len);
							PutDataRxBuf(&CommBufPrt,string_buf,common_len);
							RcptCmd_CharLF(1);
						}

#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
#ifdef USE_TAX_RATE                        
						if(GlbPrint.reportZeroValue || psr->pstTax[period] != 0)
						{
#ifdef USE_ARAB_TICKET_PRT						
							RcptCmd_CharTab(TICKET_MARGIN);
#else
							RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
							receipt_caption_read(RCPTCAP_ID_REP_TAX_PRICE, string_buf);	//"TAX PRICE"
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
							RcptCmd_PrintArabAllign(string_buf,25);
#else
							RcptCmd_CharTab(6);
#endif
							PrtMakePriceString(string_buf, psr->pstTax[period], decimal, dec_ch, common_len);
							PutDataRxBuf(&CommBufPrt,string_buf,common_len);
							RcptCmd_CharLF(1);
						}
#endif                        

#if !defined(USE_INDIA_FUNCTION)
						if(GlbPrint.reportZeroValue || psr->pstCountOfLabel[period] != 0)
						{
#ifdef USE_ARAB_TICKET_PRT						
							RcptCmd_CharTab(TICKET_MARGIN);
#else
							RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
							receipt_caption_read(RCPTCAP_ID_REP_COUNT_LABEL, string_buf);	//"COUNT OF LABEL SALE"
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
							RcptCmd_PrintArabAllign(string_buf,25);
#else
							RcptCmd_CharTab(6);
#endif
							sprintf(string_buf, digit_form, psr->pstCountOfLabel[period]);
							//PrtMakePriceString(string_buf, psr->pstCountOfLabel[period], decimal, dec_ch, common_len);
							PutDataRxBuf(&CommBufPrt,string_buf,common_len);
							RcptCmd_CharLF(1);
						}
						
						if(GlbPrint.reportZeroValue || psr->pstCountOfLabelVolume[period] != 0)
						{
#ifdef USE_ARAB_TICKET_PRT						
							RcptCmd_CharTab(TICKET_MARGIN);
#else
							RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
							receipt_caption_read(RCPTCAP_ID_REP_COUNT_LABEL_VOL, string_buf);	//"VOLUME OF LABEL SALE"
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
							RcptCmd_PrintArabAllign(string_buf,25);
#else
							RcptCmd_CharTab(6);
#endif
							PrtMakePriceString(string_buf, psr->pstCountOfLabelVolume[period], decimal, dec_ch, common_len);
							PutDataRxBuf(&CommBufPrt,string_buf,common_len);
							RcptCmd_CharLF(1);
						}
#endif
#endif
						if(GlbPrint.reportZeroValue || psr->pstPrepackWeight[period] != 0)
						{
#ifdef USE_ARAB_TICKET_PRT						
							RcptCmd_CharTab(TICKET_MARGIN);
#else
							RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
							receipt_caption_read(RCPTCAP_ID_REP_PREPACK_WEIGHT, string_buf);	//"PREPACK WEIGHT"
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
							RcptCmd_PrintArabAllign(string_buf,25);
#else
							RcptCmd_CharTab(6);
#endif
							PrtMakeWeightString(string_buf, psr->pstPrepackWeight[period], w_decimal, dec_ch, common_len);
							//PrtMakePriceString(string_buf, psr->pstPrepackWeight[period], decimal, dec_ch, common_len);
							PutDataRxBuf(&CommBufPrt,string_buf,common_len);
							RcptCmd_CharLF(1);
						}
							
						if(GlbPrint.reportZeroValue || psr->pstPrepackVolume[period] != 0)
						{
#ifdef USE_ARAB_TICKET_PRT						
							RcptCmd_CharTab(TICKET_MARGIN);
#else
							RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
							receipt_caption_read(RCPTCAP_ID_REP_PREPACK_VOL, string_buf);	//"PREPACK VOLUME"
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
							RcptCmd_PrintArabAllign(string_buf,25);
#else
							RcptCmd_CharTab(6);
#endif
							PrtMakePriceString(string_buf, psr->pstPrepackVolume[period], decimal, dec_ch, common_len);
							PutDataRxBuf(&CommBufPrt,string_buf,common_len);
						}

						RcptCmd_CharLF(2);

//						RcptCmd_CharLF(1);
						PrintListWaitEnd();

						if(!Prt_CheckOutOfPaper()) {
							FlushRxBuf(&CommBufPrt);
							RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
							return OFF;
						}
						if(!Prt_CheckGapErr()) {
							FlushRxBuf(&CommBufPrt);
							RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
							return OFF;
						}
					}
				}
			}
			if(psr->pstNext == 0) break;
			//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
			next = psr->pstNext;
			get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
		}

		RcptCmd_CharTab(TICKET_MARGIN);
		RcptCmd_PrintLine(1, 0);
		RcptCmd_CharLF(1);
	}
			     
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);
#ifndef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	} 
#endif
	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	if(flagStopPrint == ON) return OFF;
	return ON;
}

//Added by JJANG 20080529 parameter 721 : 2   
INT8U Report_PrintTicketGroupSalesSummary(INT8U mode, INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	INT16U i;
	char string_buf[100], digit_form[10];
	char string_buf1[100], digit_form1[20];
	INT32U flash_addr;
//	INT8U decimal, dec_ch, w_decimal;
	INT8U dec_ch, w_decimal;    
	INT8U len;
	INT8U flagReport;
	INT16U code;
	INT8S variable_len, common_len;
	INT16U shop_no;
	INT32U store_addr;

	STORE_NAME_STRUCT store_str;
	RTC_STRUCT time_str;
	INT32S count;

	if(end_no >= MAX_GROUP) end_no = MAX_GROUP - 1;

	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	sr = &sst;

	PrtSensorFlagInit();

//	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
//	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	flagReport =get_global_bparam(GLOBAL_USE_REPORT);
//	w_decimal=get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);
	flash_addr = DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA;

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharLF(2);

	if(GlbPrint.reportStoreName) 
	{
		shop_no = get_net_wparam(NETWORK_STORE_ID);
		TotalRcpt_PrintShopName(shop_no);
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_PrintLine(1, 0);
	}

	variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
	if(variable_len < 0) variable_len = 0;
	sprintf(digit_form, "%%%dd", REPORT_COUNT_LENGTH+variable_len);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, LEFT);
#endif
#ifdef USE_ARAB_TICKET_PRT
    receipt_caption_read(RCPTCAP_ID_REP_TITLE_GROUP, string_buf);
    sprintf(string_buf+strlen(string_buf), " %c%d", ReportTypeChar[mode], period+1);
#else
	sprintf(string_buf, "%c%d ", ReportTypeChar[mode], period+1);
	receipt_caption_read(RCPTCAP_ID_REP_TITLE_GROUP, string_buf+3);
#endif
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(2);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintFixedTime(cur_time);
	RcptCmd_CharLF(1);

	receipt_caption_read(RCPTCAP_ID_REP_LAST_RESET, string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	Report_ReadResetTime(NVRAM_RESET_TIME_GROUP, period, &time_str);
	PrintFixedTime(&time_str);
	RcptCmd_CharLF(1);

#ifdef USE_PRT_SCALE_NUM_IN_REPORT
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, string_buf);	//"SCALE #"
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", status_scale.scaleid);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
#endif

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d, ", start_no);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", end_no);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);

	shop_no = get_net_wparam(NETWORK_STORE_ID);
	store_addr = DFLASH_BASE + FLASH_SHOPNAME_AREA;
	store_addr = store_addr + SHOPNAME_STRUCT_SIZE * (INT32U)(shop_no-1);
	Flash_sread(store_addr, (HUGEDATA INT8U *)&store_str, sizeof(STORE_NAME_STRUCT));
	if(shop_no == store_str.code)
	{
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)store_str.name,sizeof(store_str.name));
		RcptCmd_CharLF(1);
	}

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	//SCALE NO, Ticket No
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_CharTab(TICKET_MARGIN);
#else
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
	receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d:", status_scale.scaleid);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

	receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, " %c%d", ReportTypeChar[1], period+1);
	len = strlen(string_buf);
	count = get_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*period);
	sprintf(string_buf+len, "%06ld", count);

	common_len = REPORT_PRICE_LENGTH + REPORT_COUNT_LENGTH + variable_len;
	sprintf(digit_form1, "%%%ds", common_len);
	RcptCmd_CharTab(6);
	sprintf(string_buf1, digit_form1, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf1,common_len);
	RcptCmd_CharLF(1);

	//GROUP NO + Weight
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_CharTab(TICKET_MARGIN);
#else
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
	receipt_caption_read(RCPTCAP_ID_REP_GROUP_NO, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

	common_len = REPORT_PRICE_LENGTH + REPORT_COUNT_LENGTH + variable_len;
	sprintf(digit_form, "%%%ds", common_len);
	RcptCmd_CharTab(6);

	receipt_caption_read(RCPTCAP_ID_REP_WEIGHT, string_buf);
	sprintf(string_buf1, digit_form, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf1,common_len);
	RcptCmd_CharLF(1);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	for(i = start_no; i <= end_no; i++) 
	{
		if(KEY_Read()) 
		{
			if(KeyDrv.CnvCode == KP_CLEAR) 
			{
				BuzOn(1);
				break;
			}
		}
		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i, sr);
		if(sr->sstTransactions[period] || sr->sstVolume[period] || sr->sstWeight[period] || sr->sstQty[period] || (flagReport&0x01)) 
		{
#ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			sprintf(string_buf, "%02d. ", i);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			string_buf[0] = 0;
			if(i) 
			{
				code = Flash_wread(flash_addr+(INT32U)(i-1)*GROUP_STRUCT_SIZE);
				if(code == i) 
				{
					Flash_sread(flash_addr+(INT32U)(i-1)*GROUP_STRUCT_SIZE+2, (HUGEDATA INT8U *)&string_buf, GROUP_STRUCT_SIZE-2);
					string_buf[GROUP_STRUCT_SIZE-2] = 0;
				}
			} 
			else receipt_caption_read(RCPTCAP_ID_REP_NON_GROUP, string_buf);
			PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)string_buf,strlen((HUGEDATA char *)string_buf));

			common_len = REPORT_PRICE_LENGTH + REPORT_COUNT_LENGTH + variable_len;
			sprintf(digit_form, "%%%dd", common_len);

#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(6);
#endif
			PrtMakeWeightString(string_buf, sr->sstWeight[period], w_decimal, dec_ch, common_len);
			PutDataRxBuf(&CommBufPrt,string_buf,common_len);
			RcptCmd_CharLF(1);

			PrintListWaitEnd();

			if(!Prt_CheckOutOfPaper()) 
			{
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
			if(!Prt_CheckGapErr()) 
			{
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
		}
	}
			     
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

#ifndef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	} 
#endif
	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	if(i <= end_no) return OFF;
	return ON;
}

INT8U Report_PrintGroupSalesSummary(INT8U mode, INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time, BASIC_REPORT_TYPE *total_br)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	BASIC_REPORT_TYPE br;
	INT16U i;
	char string_buf[100], digit_form[10];
	INT32U flash_addr;
	INT8U decimal, dec_ch;
	RTC_STRUCT time_str;
	INT8U len;
	INT8U flagReport;
	INT16U code;
	INT8S variable_len;
	INT16U shop_no;  //SG060105
#ifdef USE_TAX_RATE
	long totalVolume, volume;
	TAX_STRUCT tax_str;
	INT8U j,common_len;
#endif
	sr = &sst;

	if(end_no >= MAX_GROUP) end_no = MAX_GROUP - 1;

	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	flagReport =get_global_bparam(GLOBAL_USE_REPORT);

	flash_addr = DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA;

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharLF(2);

	//SG060105_S
	if(GlbPrint.reportStoreName) {
		shop_no = get_net_wparam(NETWORK_STORE_ID);
		TotalRcpt_PrintShopName(shop_no);
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_PrintLine(1, 0);
	} //SG060105_E

	variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
	if(variable_len < 0) variable_len = 0;
	//common_len = REPORT_PRICE_LENGTH + REPORT_COUNT_LENGTH + variable_len;
	sprintf(digit_form, "%%%dd", REPORT_COUNT_LENGTH+variable_len);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, LEFT);
#endif
#ifdef USE_ARAB_TICKET_PRT
    receipt_caption_read(RCPTCAP_ID_REP_TITLE_GROUP, string_buf);
    sprintf(string_buf+strlen(string_buf), " %c%d", ReportTypeChar[mode], period+1);
#else
    sprintf(string_buf, "%c%d ", ReportTypeChar[mode], period+1);
    receipt_caption_read(RCPTCAP_ID_REP_TITLE_GROUP, string_buf+3);
#endif
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(2);
//	RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintFixedTime(cur_time);
	RcptCmd_CharLF(1);

	receipt_caption_read(RCPTCAP_ID_REP_LAST_RESET, string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	Report_ReadResetTime(NVRAM_RESET_TIME_GROUP, period, &time_str);
	PrintFixedTime(&time_str);
	RcptCmd_CharLF(1);

#ifdef USE_PRT_SCALE_NUM_IN_REPORT
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, string_buf);	//"SCALE #"
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", status_scale.scaleid);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
#endif

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d, ", start_no);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", end_no);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);

	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);

	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
			
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	if (GlbPrint.reportType == 1)// transaction count, price, weight, quantity
	{
		Report_PrintBasicData(total_br);
	}
	else
	{
		//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT, string_buf);	//"COUNT"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		format_fixed((HUGEDATA INT8U*)string_buf, total_br->brtTransactions, 0, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len, '.');
		PutDataRxBuf(&CommBufPrt, string_buf, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
		
		//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);	//"PRICE"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		PrtMakePriceString(string_buf, total_br->brtVolume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
#ifdef USE_TAX_RATE
		totalVolume = 0;
		volume = 0;
		for(j = 1; j < MAX_TAX_NO; j++)
		{
			flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(j-1));
			Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
			if(j != tax_str.tax_code) continue;

			if(tax_str.tax_type == 1)
			{
				if(i == 0)
					get_tax_summary_sum(1, MAX_SCALE-1, j, period, &volume);
				else
					get_tax_summary_sum(1, 1, j, period, &volume);
				totalVolume += volume;
	}
		}
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_TAX_PRICE, string_buf); //"TAX PRICE"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(6);
#endif
		variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
		if(variable_len < 0) variable_len = 0;
		common_len = REPORT_PRICE_LENGTH + REPORT_COUNT_LENGTH + variable_len;

		PrtMakePriceString(string_buf, totalVolume, decimal, dec_ch, common_len);
		PutDataRxBuf(&CommBufPrt,string_buf, common_len);
		RcptCmd_CharLF(1);
#endif // USE_TAX_RATE
	}
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	for(i = start_no; i <= end_no; i++) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i, sr);
		if(sr->sstTransactions[period] || sr->sstVolume[period] || sr->sstWeight[period] || sr->sstQty[period] || (flagReport&0x01)) {
#ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_GROUP_NO, string_buf);
			len = strlen(string_buf);
			sprintf(string_buf+len, "%02d", i);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

			RcptCmd_CharTab(5);
			string_buf[0] = 0;
			if(i) {
				code = Flash_wread(flash_addr+(INT32U)(i-1)*GROUP_STRUCT_SIZE);
				if(code == i) {
					Flash_sread(flash_addr+(INT32U)(i-1)*GROUP_STRUCT_SIZE+2, (HUGEDATA INT8U *)&string_buf, GROUP_STRUCT_SIZE-2);
					string_buf[GROUP_STRUCT_SIZE-2] = 0;
				}
			} else {
				receipt_caption_read(RCPTCAP_ID_REP_NON_GROUP, string_buf);
				//sprintf(string_buf, "No Assigned PLU Group");
			}
			PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)string_buf,strlen((HUGEDATA char *)string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN);
			RcptCmd_PrintLine(1, 0);

			if (GlbPrint.reportType == 1)// transaction count, price, weight, quantity
			{
				br.brtTransactions = sr->sstTransactions[period];
				br.brtVolume = sr->sstVolume[period];
				br.brtWeight = sr->sstWeight[period];
				br.brtQty = sr->sstQty[period];
				Report_PrintBasicData(&br);
			}
			else
			{
				if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
				{
#ifdef USE_ARAB_TICKET_PRT
        			RcptCmd_CharTab(TICKET_MARGIN);
#else
        			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_SUBTOTAL, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(6);
#endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(2);
				}

				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_GROUP, i);
				get_nvram_summary_addr(NVRAM_PREPACK_GROUP, i, sr);
				
				if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
				{
#ifdef USE_ARAB_TICKET_PRT
        			RcptCmd_CharTab(TICKET_MARGIN);
#else
        			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_PREPACK_COUNT, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(6);
#endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(1);
				}
			}

			RcptCmd_CharTab(TICKET_MARGIN);
			RcptCmd_PrintLine(1, 0);
			RcptCmd_CharLF(1);

			PrintListWaitEnd();

			if(!Prt_CheckOutOfPaper()) {
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
			if(!Prt_CheckGapErr()) {
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
		}
	}
			     
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

#ifndef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	} 
#endif
	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	if(i <= end_no) return OFF;
	return ON;
}

INT8U Report_PrintDeptSalesSummary(INT8U mode, INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time, BASIC_REPORT_TYPE *total_br)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	BASIC_REPORT_TYPE br;
	INT16U i;
	char string_buf[100], digit_form[10];
	INT32U flash_addr;
	INT8U decimal, dec_ch;
	DEPARTMENT_STRUCT dept_str;
	RTC_STRUCT time_str;
	INT8U len;
	INT8U flagReport;
	INT8S variable_len;
	INT16U shop_no;  //SG060105
#ifdef USE_TAX_RATE
	long totalVolume, volume;
	TAX_STRUCT tax_str;
	INT8U j,common_len;
#endif
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	sr = &sst;

	PrtSensorFlagInit();


	if(start_no <= 0) start_no = 1;
	if(end_no >= MAX_DEPT_NO) end_no = MAX_DEPT_NO-1;

	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	flagReport =get_global_bparam(GLOBAL_USE_REPORT);

	flash_addr = DFLASH_BASE + FLASH_DEPARTMENT_AREA;

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharLF(2);

	//SG060105_S
	if(GlbPrint.reportStoreName) {
		shop_no = get_net_wparam(NETWORK_STORE_ID);
		TotalRcpt_PrintShopName(shop_no);
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_PrintLine(1, 0);
	} //SG060105_E

	variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
	if(variable_len < 0) variable_len = 0;
	//common_len = REPORT_PRICE_LENGTH + REPORT_COUNT_LENGTH + variable_len;
	sprintf(digit_form, "%%%dd", REPORT_COUNT_LENGTH+variable_len);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, LEFT);
#endif
#ifdef USE_ARAB_TICKET_PRT
    receipt_caption_read(RCPTCAP_ID_REP_TITLE_DEPT, string_buf);
    sprintf(string_buf+strlen(string_buf), " %c%d", ReportTypeChar[mode], period+1);
#else
	sprintf(string_buf, "%c%d ", ReportTypeChar[mode], period+1);
	receipt_caption_read(RCPTCAP_ID_REP_TITLE_DEPT, string_buf+3);
#endif
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(2);
//	RcptCmd_CharLF(1);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintFixedTime(cur_time);
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_LAST_RESET, string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	Report_ReadResetTime(NVRAM_RESET_TIME_DEPT, period, &time_str);
	PrintFixedTime(&time_str);
	RcptCmd_CharLF(1);

#ifdef USE_PRT_SCALE_NUM_IN_REPORT
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, string_buf);	//"SCALE #"
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", status_scale.scaleid);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
#endif

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d, ", start_no);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", end_no);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
					
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);	
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
					
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
#endif
	if (GlbPrint.reportType)// transaction count, price, weight, quantity
	{
		Report_PrintBasicData(total_br);
	}
	else
	{
		if(GlbPrint.reportZeroValue || total_br->brtTransactions != 0)
		{
			//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT, string_buf);	//"COUNT"
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(5);
#endif
			format_fixed((HUGEDATA INT8U*)string_buf, total_br->brtTransactions, 0, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len, '.');
			PutDataRxBuf(&CommBufPrt, string_buf, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		   	RcptCmd_CharLF(1);
		}
		if(GlbPrint.reportZeroValue || total_br->brtVolume != 0)
		{
			//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
 #ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);	//"PRICE"
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(5);
#endif
			PrtMakePriceString(string_buf, total_br->brtVolume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
			PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		   	RcptCmd_CharLF(1);
		}
	}
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	for(i = start_no; i <= end_no; i++) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i, sr);
		if(sr->sstTransactions[period] || sr->sstVolume[period] || sr->sstWeight[period] || sr->sstQty[period] || (flagReport&0x01)) {
#ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_DEPT_NO, string_buf);
			len = strlen(string_buf);

			sprintf(string_buf+len, "%02d", i);		// 0x9c09
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(5);
#endif
			Flash_sread(flash_addr+(INT32U)(i-1)*DEPARTMENT_STRUCT_SIZE, (HUGEDATA INT8U *)&dept_str, sizeof(DEPARTMENT_STRUCT));
			if(dept_str._code != i) dept_str._description[0] = 0;
			PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)dept_str._description,strlen((HUGEDATA char *)dept_str._description));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN);
			RcptCmd_PrintLine(1, 0);

			if (GlbPrint.reportType)// transaction count, price, weight, quantity
			{
				br.brtTransactions = sr->sstTransactions[period];
				br.brtVolume = sr->sstVolume[period];
				br.brtWeight = sr->sstWeight[period];
				br.brtQty = sr->sstQty[period];
				Report_PrintBasicData(&br);
			}
			else
			{
				if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
				{
 #ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
#else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_SUBTOTAL, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(6);
#endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
#ifdef USE_TAX_RATE
					RcptCmd_CharLF(1);
					totalVolume = 0;
					volume = 0;
					for(j = 1; j < MAX_TAX_NO; j++) 
					{
						flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(j-1));
						Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
						if(j != tax_str.tax_code) continue;
					
						if(tax_str.tax_type == 1)
						{
							if(i == 0)
								get_tax_summary_sum(1, MAX_SCALE-1, j, period, &volume);
							else
								get_tax_summary_sum(1, 1, j, period, &volume);
							totalVolume += volume;
						}
					}
#ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
#else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_TAX_PRICE, string_buf); //"TAX PRICE"
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(6);
#endif
					PrtMakePriceString(string_buf, totalVolume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
					PutDataRxBuf(&CommBufPrt,string_buf, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
#endif // USE_TAX_RATE
					RcptCmd_CharLF(2);
				}

				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_DEPT, i);
				get_nvram_summary_addr(NVRAM_PREPACK_DEPT, i, sr);
				
				if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
				{
 #ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
#else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_PREPACK_COUNT, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(6);
#endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(1);
				}
#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_DEPT, i);
				get_nvram_summary_addr(NVRAM_RETURN_DEPT, i, sr);

				if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
				{
  #ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
  #else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
  #endif
					receipt_caption_read(RCPTCAP_ID_REP_RETURN_COUNT, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
  #ifdef USE_ARAB_CONVERT
                    RcptCmd_PrintArabAllign(string_buf,25);
  #else
					RcptCmd_CharTab(6);
  #endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(1);
				}
#endif
			}
			RcptCmd_CharTab(TICKET_MARGIN);
			RcptCmd_PrintLine(1, 0);
			RcptCmd_CharLF(1);

			PrintListWaitEnd();

			if(!Prt_CheckOutOfPaper()) {
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
			if(!Prt_CheckGapErr()) {
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
		}
	}
			     
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

#ifndef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	} 
#endif
	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	if(i <= end_no) return OFF;
	return ON;
}

INT8U Report_PrintScaleSalesSummary(INT8U mode, INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time, BASIC_REPORT_TYPE *total_br)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	BASIC_REPORT_TYPE br;
	INT16U i;
	char string_buf[100], digit_form[10];
	INT8U decimal, dec_ch;
	RTC_STRUCT time_str;
	INT8U len;
	INT8U flagReport;
	//INT32U tax_value;
	INT16U prt_end_no, prt_start_no;
	INT16S wtemp;
	INT8S variable_len;
	long RptRoundPrice, RptRoundPrice_Scale, volume;
//	INT16S  RptRoundCount, RptRoundCount_Scale, transaction;
	INT16S  RptRoundCount, transaction;    
	INT16U shop_no;  //SG060105
#ifdef USE_REPORT_PAY
	long RptPaidPrice;
	INT16S  RptPaidCount;
	INT16S chg_transactions;
	long chg_volume;
	INT16S ret_transactions;	//Added by JJANG 20071220 
	long ret_volume;
	INT16U j;
	INT32U flash_addr;
	TAX_STRUCT tax_str;	
#endif
#ifdef USE_PRINT_GROSS_AMOUNT_FOR_TAX
	long taxGross;
	long totalTax;
	long totalGross;
#endif
#ifdef USE_INDIA_TAX
	char temp_string[35];
	char currency_sign1[5];
#endif
#ifdef USE_ARAB_TICKET_PRT
    char currency_sign1[5];
    INT8S temp_len;
#endif
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	sr = &sst;

	if(end_no > MAX_SCALE-1) end_no = MAX_SCALE-1;
	prt_start_no = start_no;
	prt_end_no   = end_no;
	if(start_no == 0) {
		end_no = 0;
		prt_start_no = 1;
		prt_end_no   = MAX_SCALE - 1;
	}
	PrtSensorFlagInit();

	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	flagReport =get_global_bparam(GLOBAL_USE_REPORT);

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharLF(2);

	//SG060105_S
	if(GlbPrint.reportStoreName) {
		shop_no = get_net_wparam(NETWORK_STORE_ID);
		TotalRcpt_PrintShopName(shop_no);
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_PrintLine(1, 0);
	} //SG060105_E

	variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
	if(variable_len < 0) variable_len = 0;
	sprintf(digit_form, "%%%dd", REPORT_COUNT_LENGTH+variable_len);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, LEFT);
#endif
#ifdef USE_ARAB_TICKET_PRT
    receipt_caption_read(RCPTCAP_ID_REP_TITLE_SCALE, string_buf);
    sprintf(string_buf+strlen(string_buf), " %c%d", ReportTypeChar[mode], period+1);
#else
	sprintf(string_buf, "%c%d ", ReportTypeChar[mode], period+1);
	receipt_caption_read(RCPTCAP_ID_REP_TITLE_SCALE, string_buf+3);
#endif
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(2);

//	RcptCmd_CharLF(1);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintFixedTime(cur_time);
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_LAST_RESET, string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	Report_ReadResetTime(NVRAM_RESET_TIME_SCALE, period, &time_str);
	PrintFixedTime(&time_str);
	RcptCmd_CharLF(1);

	if(start_no == 0) {
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_SCALE_TOTAL, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	   	RcptCmd_CharLF(1);
	}

//	if (ethernet_list.status!=2) {	// if Master
	if (ethernet_list.status==1) {	// if Master
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
		len = strlen(string_buf);
		sprintf(string_buf+len, "%02d, ", prt_start_no);
		len = strlen(string_buf);
		receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
		len = strlen(string_buf);
		sprintf(string_buf+len, "%02d", prt_end_no);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	   	RcptCmd_CharLF(1);
	}
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);	//"============================="
	
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);

	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);	//"Total"
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);	//"============================="
	
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
					
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);		
#endif
	if (GlbPrint.reportType)// transaction count, price, weight, quantity
	{
		Report_PrintBasicData(total_br);
	}
	else
	{
		//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#ifdef USE_ARAB_TICKET_PRT
        RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
        receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT, string_buf);	//"COUNT"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		format_fixed((HUGEDATA INT8U*)string_buf, total_br->brtTransactions, 0, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len, '.');
		PutDataRxBuf(&CommBufPrt, string_buf, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
		
		//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#ifdef USE_ARAB_TICKET_PRT
        RcptCmd_CharTab(TICKET_MARGIN);
#else
        RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);	//"PRICE"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		PrtMakePriceString(string_buf, total_br->brtVolume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
	}

	if(GlbPrint.reportRoundedSummary) {
		// Rounded Price
		RptRoundCount = 0;
		RptRoundPrice = 0;		
		for(i = start_no; i <= end_no; i++) {
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, i);
			get_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, i, sr);
			RptRoundCount += sr->sstTransactions[period];
			RptRoundPrice += sr->sstVolume[period];
		}
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
		receipt_caption_read(RCPTCAP_ID_REP_ROUNDED_TOTAL, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
        RcptCmd_CharTab(TICKET_TXT_MARGIN);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
		receipt_caption_read(RCPTCAP_ID_REP_ROUNDED_TOTAL, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	   	RcptCmd_CharLF(1);

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
        RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		PrtMakePriceString(string_buf, total_br->brtVolume-RptRoundPrice, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
	}
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);	//"============================="

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);	//"============================="

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_TypeNone();
	for(i = start_no; i <= end_no; i++) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
		if(i == 0) {
//			RptRoundCount_Scale = total_br->brtTransactions;
			RptRoundPrice_Scale = total_br->brtVolume;
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, i);
			get_summary_sum_addr(NVRAM_ROUND_TOTAL_SCALE, 1, MAX_SCALE-1, period, &transaction, &volume);
			RptRoundPrice_Scale -= volume;
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_BYPLU, i);
		} else {
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i);
			get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i, sr);
			if(sr->sstTransactions[period] || sr->sstVolume[period] || sr->sstWeight[period] || sr->sstQty[period] || (flagReport&0x01)) ;
			else continue;

#ifdef USE_ARAB_TICKET_PRT
            RcptCmd_CharTab(TICKET_MARGIN);
#else
            RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, string_buf);	//"SCALE #"
			len = strlen(string_buf);
#ifdef USE_MAX_SCALE_NO_255
			sprintf(string_buf+len, "%02d~%03d", MAX_SCALE - 1, 255);
#else
			sprintf(string_buf+len, "%02d", i);
#endif
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN);
			RcptCmd_PrintLine(1, 0);
#ifdef USE_ARAB_TICKET_PRT
            RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
  #ifdef USE_ARAB_CONVERT
            RcptCmd_PrintArabAllign(string_buf,25);
  #endif

            receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT, string_buf);   //"COUNT" 
            temp_len = strlen(string_buf);

            get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
            currency_sign1[4] = ' ';    // 아랍어 마지막 문자 종성 변환
            currency_sign1[5] = 0;
            len = strlen(currency_sign1);
            sprintf(string_buf+temp_len, "   %s", currency_sign1);
            PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
            RcptCmd_CharLF(1);
            
            RcptCmd_CharTab(TICKET_MARGIN);
            RcptCmd_PrintLine(1, 0);
#endif

			if (GlbPrint.reportType)// transaction count, price, weight, quantity
			{
				br.brtTransactions = sr->sstTransactions[period];
				br.brtVolume = sr->sstVolume[period];
				br.brtWeight = sr->sstWeight[period];
				br.brtQty = sr->sstQty[period];
				Report_PrintBasicData(&br);
			}
			else
			{
#ifdef USE_ARAB_TICKET_PRT
                RcptCmd_CharTab(TICKET_MARGIN);
#else
                RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_SUBTOTAL_SUMMARY, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				len = strlen(string_buf);
				RcptCmd_PrintChar(' ');
				len++;
				RcptCmd_PrintCharLine('-', Rcpt_MaxWidthChar-len-REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
                RcptCmd_CharTab(TICKET_MARGIN);
#else
                RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_SUBTOTAL, string_buf);  //"SUBTOTAL"
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, sr->sstTransactions[period]);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}
//			RptRoundCount_Scale = sr->sstTransactions[period];
			RptRoundPrice_Scale = sr->sstVolume[period];

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_SCALE, i);
			get_summary_sum_addr(NVRAM_ROUND_TOTAL_SCALE, i, i, period, &transaction, &volume);
			RptRoundPrice_Scale -= volume;
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_BYPLU, i);
//DEL CJK070319_1			get_summary_sum_addr(NVRAM_ROUND_BYPLU, i, i, period, &transaction, &volume);
//DEL CJK070319_1			RptRoundPrice_Scale -= volume;
			if(GlbPrint.reportRoundedSummary) {
#ifdef USE_ARAB_TICKET_PRT
                RcptCmd_CharTab(TICKET_MARGIN);
#else
                RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_ROUNDED_SUBTOTAL, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharTab(5);
				memset(string_buf, 0x20, REPORT_COUNT_LENGTH+variable_len);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_COUNT_LENGTH+variable_len);
				PrtMakePriceString(string_buf, RptRoundPrice_Scale, decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}

			RcptCmd_CharLF(1);
		}


#ifdef USE_ARAB_TICKET_PRT
        RcptCmd_CharTab(TICKET_MARGIN);
#else
        RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif

		//sprintf(string_buf, "SALE SUMMARY");
		receipt_caption_read(RCPTCAP_ID_REP_SALE_SUMMARY, string_buf);  //"SALE SUMMARY"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		len = strlen(string_buf);
		RcptCmd_PrintChar(' ');
		len++;
		RcptCmd_PrintCharLine('-', Rcpt_MaxWidthChar-len-REPORT_PRICE_LENGTH);
		RcptCmd_CharLF(1);

		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_SCALE, i);
		if(i == 0) {
			get_summary_sum_addr(NVRAM_VOID_SCALE, 1, MAX_SCALE-1, period, &transaction, &volume);
		} else {
			get_summary_sum_addr(NVRAM_VOID_SCALE, i, i, period, &transaction, &volume);
		}
		if(GlbPrint.reportZeroValue ||transaction != 0)
		{
#ifdef USE_ARAB_TICKET_PRT
            RcptCmd_CharTab(TICKET_MARGIN);
#else
            RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_VOID_COUNT, string_buf);  //"VOID"
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(5);
#endif
			sprintf(string_buf, digit_form, transaction);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
			PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
			RcptCmd_CharLF(1);
		}

		if(GlbPrint.reportRepack) {
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_SCALE, i);
			if(i == 0) {
				get_summary_sum_addr(NVRAM_REPACK_SCALE, 1, MAX_SCALE-1, period, &transaction, &volume);
			} else {
				get_summary_sum_addr(NVRAM_REPACK_SCALE, i, i, period, &transaction, &volume);
			}
			if(GlbPrint.reportZeroValue || transaction != 0)
			{
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_REPACK_COUNT, string_buf);  //"REPACK"
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, transaction);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}
		}

		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, i);
		if(i == 0) {
			get_summary_sum_addr(NVRAM_OVERRIDE_SCALE, 1, MAX_SCALE-1, period, &transaction, &volume);
		} else {
			get_summary_sum_addr(NVRAM_OVERRIDE_SCALE, i, i, period, &transaction, &volume);
		}
		if(GlbPrint.reportZeroValue || transaction != 0)
		{
#ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_OVERRIDE_COUNT, string_buf);	//"OVERRIDE"
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(5);
#endif
			sprintf(string_buf, digit_form, transaction);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
			PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
			RcptCmd_CharLF(1);
		}
		
		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, i);
		if(i == 0) {
			get_summary_sum_addr(NVRAM_DISCOUNT_SCALE, 1, MAX_SCALE-1, period, &transaction, &volume);
		} else {
			get_summary_sum_addr(NVRAM_DISCOUNT_SCALE, i, i, period, &transaction, &volume);
		}
		if(GlbPrint.reportZeroValue ||transaction != 0)
		{
#ifdef USE_ARAB_TICKET_PRT
            RcptCmd_CharTab(TICKET_MARGIN);
#else
            RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_DISCOUNT_COUNT, string_buf);	//"DISCOUNT"
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(5);
#endif
			sprintf(string_buf, digit_form, transaction);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
			PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
			RcptCmd_CharLF(1);
		}	
		
		if(GlbPrint.reportPrepack) {
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_SCALE, i);
			if(i == 0) {
				get_summary_sum_addr(NVRAM_PREPACK_SCALE, 1, MAX_SCALE-1, period, &transaction, &volume);
			} else {
				get_summary_sum_addr(NVRAM_PREPACK_SCALE, i, i, period, &transaction, &volume);
			}
			if(GlbPrint.reportZeroValue || transaction != 0)
			{
#ifdef USE_ARAB_TICKET_PRT
    			RcptCmd_CharTab(TICKET_MARGIN);
#else
    			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_PREPACK_COUNT, string_buf);	//"PREPACK"
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, transaction);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}
		}

		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_SCALE, i);
		if(i == 0) {
			get_summary_sum_addr(NVRAM_RETURN_SCALE, 1, MAX_SCALE-1, period, &transaction, &volume);
		} else {
			get_summary_sum_addr(NVRAM_RETURN_SCALE, i, i, period, &transaction, &volume);
		}
		if(GlbPrint.reportZeroValue || transaction != 0)
		{
#ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_RETURN_COUNT, string_buf);	//"RETURN"
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(5);
#endif
			sprintf(string_buf, digit_form, transaction);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
			PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
			RcptCmd_CharLF(1);
		}		

		if(GlbPrint.reportOpenCash) {
			//wtemp = get_nvram_wparam(NVRAM_CASH_SCALE+i*4+period*2);
			if(i == 0) {
				get_cashopen_summary_sum(1, MAX_SCALE-1, period, &wtemp);
			} else {
				get_cashopen_summary_sum(i, i, period, &wtemp);
			}
			if(GlbPrint.reportZeroValue || wtemp != 0)
			{
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_NOSALEOPEN_COUNT, string_buf);	//"NO SALE OPEN COUNT"
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, wtemp);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharLF(1);
			}
		}

		RcptCmd_CharLF(1);


#ifdef USE_REPORT_PAY	//#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
		if(GlbPrint.reportPay) {
#ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_PAY_SUMMARY, string_buf);	//"PAY SUMMARY"
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			len = strlen(string_buf);
			RcptCmd_PrintChar(' ');
			len++;
			RcptCmd_PrintCharLine('-', Rcpt_MaxWidthChar-len-REPORT_PRICE_LENGTH);
			RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
			//sprintf(string_buf, "PAY");
			receipt_caption_read(RCPTCAP_ID_REP_PAY, string_buf);	//"PAY"
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			// Change Amount 미리 계산
			if(i == 0) {
				get_summary_sum_addr(NVRAM_CHANGE_SCALE, 1, MAX_SCALE-1, period, &chg_transactions, &chg_volume);
			} else {
				get_summary_sum_addr(NVRAM_CHANGE_SCALE, i, i, period, &chg_transactions, &chg_volume);
			}
			/////////////////////////////

			//Added by JJANG 20071220  RETURN일때 Cash '-'처리
			if(i == 0) {
				get_summary_sum_addr(NVRAM_RETURN_SCALE, 1, MAX_SCALE-1, period, &ret_transactions, &ret_volume);
			} else {
				get_summary_sum_addr(NVRAM_RETURN_SCALE, i, i, period, &ret_transactions, &ret_volume);
			}

			RptPaidCount = 0;
			RptPaidPrice = 0;
			for(j = 0; j < MAX_PAID_TYPE; j++) {
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_SCALE, i*(MAX_PAID_TYPE)+j);
				if(i == 0) {
					get_pay_summary_sum_addr(1, MAX_SCALE-1, j, period, &transaction, &volume);
				} else {
					get_pay_summary_sum_addr(i, i, j, period, &transaction, &volume);
				}
				
				if (j == 0 && !GlbPrint.reportChange) {
					volume -= chg_volume;	// change를 출력하지 않을 때, cash에서 change를 빼줌
				}
				
				if (j == 0 && ret_transactions) //Added by JJANG 20071220  RETURN일때 Cash '-'처리
				{
					transaction -=ret_transactions; 
					volume -= ret_volume;
				}
				
				if(GlbPrint.reportZeroValue || volume != 0)
				{
#ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
					string_buf[0]=' ';
					string_buf[1]='-';
					PutDataRxBuf(&CommBufPrt,string_buf,2);
#else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
					string_buf[0]='-';
					PutDataRxBuf(&CommBufPrt,string_buf,1);
#endif                       
					receipt_caption_read(RCPTCAP_ID_COM_PAY_CASH+j, string_buf);	//"CASH, PIN/CHIP, CHECK, CREDIT CARD, CREDIT NOTE, COUPON, BONUS POINT, CREDIT SALES"
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(4);
#endif
					sprintf(string_buf, digit_form, transaction);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(1);
				}				
				RptPaidCount += transaction;
				RptPaidPrice += volume;
			}    	      

			if (GlbPrint.reportChange) {
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_PAY_SUBTOTAL, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(4);
#endif
				sprintf(string_buf, digit_form, RptPaidCount);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, RptPaidPrice, decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_TKT_CHANGE, string_buf);	//"CHECK"
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, chg_transactions);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, chg_volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
			}
			RcptCmd_CharLF(2);

  //			RcptCmd_CharLF(1);
		}
#endif


#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
		if(GlbPrint.reportTax) {
#ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_TAX_SUMMARY, string_buf);	//"TAX SUMMARY"
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			len = strlen(string_buf);
			RcptCmd_PrintChar(' ');
			len++;
			RcptCmd_PrintCharLine('-', Rcpt_MaxWidthChar-len-REPORT_PRICE_LENGTH);
			RcptCmd_CharLF(1);

			if(get_global_bparam(GLOBAL_TAX_FLAG) != 0) {
#ifdef USE_PRINT_GROSS_AMOUNT_FOR_TAX
/*#ifdef USE_INDIA_TAX
				temp_string[0] = 0;
				RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
				get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
				currency_sign1[4] = 0;
				
				sprintf(string_buf, "GST(%%)");
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

				sprintf(string_buf, "		CGST(%s)", currency_sign1);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

				sprintf(string_buf, "	  SGST(%s)", currency_sign1);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

				sprintf(string_buf, "	   GST(%s)", currency_sign1);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharLF(1);
				temp_string[0] = 0;
#endif
*/
			  	totalTax = 0;
				totalGross = 0;
				for(j = 1; j <= MAX_TAX_NO; j++) {
					if(i == 0) {
						get_tax_summary_sum(1, MAX_SCALE-1, j, period, &volume);
						get_tax_gross_summary_sum(1, MAX_SCALE-1, j, period, &taxGross);
					} else {
						get_tax_summary_sum(i, i, j, period, &volume);
						get_tax_gross_summary_sum(i, i, j, period, &taxGross);
					}
					totalTax += volume;
					totalGross += taxGross;
					//if(GlbPrint.reportZeroValue || taxGross != 0)
					if(taxGross != 0)
					{
						flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(j-1));
						Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));

#ifdef USE_ARAB_TICKET_PRT
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#else
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
						if (j == MAX_TAX_NO) {	// tax = 0%인 경우
							sprintf(string_buf, "0:0.00%%");
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));					
						} else if(j != tax_str.tax_code) {
						  	sprintf(string_buf, "%d:", j);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
							receipt_caption_read(RCPTCAP_ID_COM_NOT_AVAILABLE , string_buf);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						} else {
/*#ifdef USE_INDIA_TAX
							RcptCmd_PutStringInit(string_buf);
							sprintf(string_buf, "%d:%5.2f%%", j, (float)(tax_str.tax_rate)/100.0);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

							PrtMakePriceString(temp_string, volume/2, decimal, dec_ch, REPORT_PRICE_LENGTH-1);
							sprintf(string_buf, "%s", temp_string);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

							PrtMakePriceString(temp_string, volume/2, decimal, dec_ch, REPORT_PRICE_LENGTH-1);
							sprintf(string_buf, "%s", temp_string);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

							PrtMakePriceString(temp_string, volume, decimal, dec_ch, REPORT_PRICE_LENGTH-2);
							sprintf(string_buf, "%s", temp_string);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#else
							sprintf(string_buf, "%d:%5.2f%%,", j, (float)(tax_str.tax_rate)/100.0);
#endif*/
#ifdef USE_ARAB_TICKET_PRT
                            sprintf(string_buf, "%d:%6.2f%%,", j, (float)(tax_str.tax_rate)/100.0); // ':'뒤에 공백 없으면 숫자 정렬 방향 바뀜 (입력기 변환 규칙) 
#else
							sprintf(string_buf, "%d:%5.2f%%,", j, (float)(tax_str.tax_rate)/100.0);
#endif
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
							if(tax_str.tax_type == 0) { // INTERNAL
								receipt_caption_read(RCPTCAP_ID_COM_INT_TAX , string_buf);
							} else {
#ifdef USE_INDIA_TAX
								sprintf(string_buf, " ");
#else
								receipt_caption_read(RCPTCAP_ID_COM_EXT_TAX , string_buf);
#endif
							}
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						}
						RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
                        // Net Amount
                        RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#else
                        // Net Amount
                        RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
#endif
						//sprintf(string_buf, "NET AMOUNT");
						receipt_caption_read(RCPTCAP_ID_REP_TAX_NET_AMOUNT, string_buf);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
						RcptCmd_PrintArabAllign(string_buf,25);
#else
						RcptCmd_CharTab(4);
#endif
						PrtMakePriceString(string_buf, taxGross-volume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
						PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
						RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
						// TAX Amount
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#else
						// TAX Amount
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
#endif
						//sprintf(string_buf, "TAX AMOUNT");
						receipt_caption_read(RCPTCAP_ID_REP_TAX_AMOUNT, string_buf);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
						RcptCmd_PrintArabAllign(string_buf,25);
#else
						RcptCmd_CharTab(4);
#endif
						PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
						PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
						RcptCmd_CharLF(1);

						// Gross Amount
#ifdef USE_ARAB_TICKET_PRT
						// Gross Amount
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#else
						// Gross Amount
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
#endif
						//sprintf(string_buf, "GROSS AMOUNT");
						receipt_caption_read(RCPTCAP_ID_REP_TAX_GROSS_AMOUNT, string_buf);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
						RcptCmd_PrintArabAllign(string_buf,25);
#else
						RcptCmd_CharTab(4);
#endif
						PrtMakePriceString(string_buf, taxGross, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
						PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
						RcptCmd_CharLF(1);

					}
				}

#ifdef USE_ARAB_TICKET_PRT
				// Total
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				// Total
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				//sprintf(string_buf, "GRAND TOTAL");
				receipt_caption_read(RCPTCAP_ID_REP_TAX_GRAND_TOTAL, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				len = strlen(string_buf);
				RcptCmd_PrintChar(' ');
				len++;
				RcptCmd_PrintCharLine('-', Rcpt_MaxWidthChar-len-8);
				RcptCmd_CharLF(1);
		
#ifdef USE_ARAB_TICKET_PRT
				// Net Amount
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#else
				// Net Amount
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
#endif
				//sprintf(string_buf, "NET AMOUNT");
				receipt_caption_read(RCPTCAP_ID_REP_TAX_NET_AMOUNT, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(4);
#endif
				PrtMakePriceString(string_buf, totalGross-totalTax, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
				RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
				// TAX Amount
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#else
				// TAX Amount
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
#endif
				//sprintf(string_buf, "TAX AMOUNT");
				receipt_caption_read(RCPTCAP_ID_REP_TAX_AMOUNT, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(4);
#endif
				PrtMakePriceString(string_buf, totalTax, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
				RcptCmd_CharLF(1);
				
#ifdef USE_ARAB_TICKET_PRT
				// Gross Amount
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#else
				// Gross Amount
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
#endif
				//sprintf(string_buf, "GROSS AMOUNT");
				receipt_caption_read(RCPTCAP_ID_REP_TAX_GROSS_AMOUNT, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(4);
#endif
				PrtMakePriceString(string_buf, totalGross, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
				RcptCmd_CharLF(1);
#else
/*
#ifdef USE_INDIA_TAX
				temp_string[0] = 0;
				RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
				get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
				currency_sign1[4] = 0;
				
				sprintf(string_buf, "GST(%%)");
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

				sprintf(string_buf, "       CGST(%s)", currency_sign1);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

				sprintf(string_buf, "     SGST(%s)", currency_sign1);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

				sprintf(string_buf, "      GST(%s)", currency_sign1);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharLF(1);
				temp_string[0] = 0;
#endif
 */
				for(j = 1; j < MAX_TAX_NO; j++) {
					if(i == 0) {
						get_tax_summary_sum(1, MAX_SCALE-1, j, period, &volume);
					} else {
						get_tax_summary_sum(i, i, j, period, &volume);
					}
					if(GlbPrint.reportZeroValue || volume != 0)
					{
						flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(j-1));
						Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
#ifdef USE_ARAB_TICKET_PRT
						RcptCmd_CharTab(TICKET_MARGIN);
#else
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
						if(j != tax_str.tax_code) {
							sprintf(string_buf, "%d:", j);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
							receipt_caption_read(RCPTCAP_ID_COM_NOT_AVAILABLE , string_buf);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						} else {
/*#ifdef USE_INDIA_TAX
							RcptCmd_PutStringInit(string_buf);
							sprintf(string_buf, "%d:%5.2f%%", j, (float)(tax_str.tax_rate)/100.0);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

							PrtMakePriceString(temp_string, volume/2, decimal, dec_ch, REPORT_PRICE_LENGTH-1);
							sprintf(string_buf, "%s", temp_string);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

							PrtMakePriceString(temp_string, volume/2, decimal, dec_ch, REPORT_PRICE_LENGTH-1);
							sprintf(string_buf, "%s", temp_string);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

							PrtMakePriceString(temp_string, volume, decimal, dec_ch, REPORT_PRICE_LENGTH-2);
							sprintf(string_buf, "%s", temp_string);
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#else
							sprintf(string_buf, "%d:%5.2f%%,", j, (float)(tax_str.tax_rate)/100.0);
#endif*/
#ifdef USE_ARAB_TICKET_PRT
                            sprintf(string_buf, "%d:%6.2f%%,", j, (float)(tax_str.tax_rate)/100.0); // ':'뒤에 공백 없으면 숫자 정렬 방향 바뀜 (입력기 변환 규칙) 
#else
							sprintf(string_buf, "%d:%5.2f%%,", j, (float)(tax_str.tax_rate)/100.0);
#endif
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
							if(tax_str.tax_type == 0) { // INTERNAL
								receipt_caption_read(RCPTCAP_ID_COM_INT_TAX , string_buf);
							} else {
#ifdef USE_INDIA_TAX
								sprintf(string_buf, " ");
#else
								receipt_caption_read(RCPTCAP_ID_COM_EXT_TAX , string_buf);
#endif
							}
							PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						}
#ifdef USE_ARAB_CONVERT
						RcptCmd_PrintArabAllign(string_buf,25);
#else
						RcptCmd_CharTab(5);
#endif
						memset(string_buf, 0x20, REPORT_COUNT_LENGTH+variable_len);
						PutDataRxBuf(&CommBufPrt,string_buf,REPORT_COUNT_LENGTH+variable_len);
						//tax_value = get_nvram_lparam(NVRAM_TAX+(INT32U)i*(INT32U)((j-1)*MAX_REPORT_PERIOD+period)*4);
						PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
						PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
						RcptCmd_CharLF(1);
					}
				}
#endif
			}
		}
#endif

		RcptCmd_CharTab(TICKET_MARGIN);
		RcptCmd_PrintLine(1, 0);

		RcptCmd_CharLF(1);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			FlushRxBuf(&CommBufPrt);
			RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			FlushRxBuf(&CommBufPrt);
			RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
			return OFF;
		}
	}
			     
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

#ifndef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	} 
#endif
	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	if(i <= end_no) return OFF;
	return ON;
}

INT8U Report_PrintHourlySalesSummary(INT8U mode, INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time, BASIC_REPORT_TYPE *total_br)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	BASIC_REPORT_TYPE br;
	INT16U i;
	char string_buf[100], digit_form[10];
	INT8U decimal, dec_ch;
	RTC_STRUCT time_str;
	INT8U len;
	INT8U flagReport;
	INT8S variable_len;
	INT16U shop_no;  //SG060105
#ifdef USE_TAX_RATE
	INT8U j;
	long volume, totalVolume;
	INT32U flash_addr;
	TAX_STRUCT tax_str;
#endif

	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	sr = &sst;

	PrtSensorFlagInit();

	if(end_no > 23) end_no = 23;

	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	flagReport =get_global_bparam(GLOBAL_USE_REPORT);

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharLF(2);

	//SG060105_S
	if(GlbPrint.reportStoreName) {
		shop_no = get_net_wparam(NETWORK_STORE_ID);
		TotalRcpt_PrintShopName(shop_no);
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_PrintLine(1, 0);
	} //SG060105_E

	variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
	if(variable_len < 0) variable_len = 0;
	//common_len = REPORT_PRICE_LENGTH + REPORT_COUNT_LENGTH + variable_len;
	sprintf(digit_form, "%%%dd", REPORT_COUNT_LENGTH+variable_len);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, LEFT);
#endif
#ifdef USE_ARAB_TICKET_PRT
    receipt_caption_read(RCPTCAP_ID_REP_TITLE_HOUR, string_buf);
    sprintf(string_buf+strlen(string_buf), " %c%d", ReportTypeChar[mode], period+1);
#else
	sprintf(string_buf, "%c%d ", ReportTypeChar[mode], period+1);
	receipt_caption_read(RCPTCAP_ID_REP_TITLE_HOUR, string_buf+3);
#endif
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(2);
//	RcptCmd_CharLF(1);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	//PrintCurrentTime();
	PrintFixedTime(cur_time);
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_LAST_RESET, string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	//Report_GetResetTime(NVRAM_RESET_TIME_TOTAL+6*period, &time_str);
	Report_ReadResetTime(NVRAM_RESET_TIME_HOURLY, period, &time_str);
	PrintFixedTime(&time_str);
	RcptCmd_CharLF(1);

#ifdef USE_PRT_SCALE_NUM_IN_REPORT
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, string_buf);	//"SCALE #"
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", status_scale.scaleid);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
#endif

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#ifdef USE_PERSIA_CAPTION
	sprintf(string_buf, Caption_StartTime);
#else
    receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
#endif
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d, ", start_no);
	len = strlen(string_buf);
#ifdef USE_PERSIA_CAPTION
    sprintf(string_buf+len, Caption_EndTime);
#else
    receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
#endif
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", end_no);
    len = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,len);
   	RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
#endif
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
			
	if (GlbPrint.reportType)// transaction count, price, weight, quantity
	{
		Report_PrintBasicData(total_br);
	}
	else
	{
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
        RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
        RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else        
		RcptCmd_CharTab(5);
#endif
		format_fixed((HUGEDATA INT8U*)string_buf, total_br->brtTransactions, 0, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len, '.');
		PutDataRxBuf(&CommBufPrt, string_buf, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
		
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
        RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
        RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		PrtMakePriceString(string_buf, total_br->brtVolume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
#ifdef USE_TAX_RATE
		totalVolume = 0;
		volume = 0;
		for(j = 1; j < MAX_TAX_NO; j++) 
		{
			flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(j-1));
			Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
			if(j != tax_str.tax_code) continue;

			if(tax_str.tax_type == 1)
			{
				if(i == 0)
					get_tax_summary_sum(1, MAX_SCALE-1, j, period, &volume);
				else
					get_tax_summary_sum(1, 1, j, period, &volume);
				totalVolume += volume;
			}
		}
 #ifdef USE_ARAB_TICKET_PRT
		RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_TAX_PRICE, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		PrtMakePriceString(string_buf, totalVolume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		RcptCmd_CharLF(1);
#endif // USE_TAX_RATE
	}
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	for(i = start_no; i <= end_no; i++) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i, sr);
		if(sr->sstTransactions[period] || sr->sstVolume[period] || sr->sstWeight[period] || sr->sstQty[period] || (flagReport&0x01)) {
 #ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_HOUR_NO, string_buf);
			len = strlen(string_buf);
			sprintf(string_buf+len, "%02d", i);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_TypeNone();
			RcptCmd_CharTab(TICKET_MARGIN);
			RcptCmd_PrintLine(1, 0);

			if (GlbPrint.reportType)// transaction count, price, weight, quantity
			{
				br.brtTransactions = sr->sstTransactions[period];
				br.brtVolume = sr->sstVolume[period];
				br.brtWeight = sr->sstWeight[period];
				br.brtQty = sr->sstQty[period];
				Report_PrintBasicData(&br);
			}
			else
			{
				if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
				{
 #ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
#else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_SUBTOTAL, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(6);
#endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(2);
				}

				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_HOUR, i);
				get_nvram_summary_addr(NVRAM_PREPACK_HOUR, i, sr);

				if(GlbPrint.reportZeroValue || sr->sstVolume[period] != 0)
				{
 #ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
#else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_PREPACK_COUNT, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(6);
#endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(1);
				}

			}

			RcptCmd_CharTab(TICKET_MARGIN);
			RcptCmd_PrintLine(1, 0);
			RcptCmd_CharLF(1);

			PrintListWaitEnd();

			if(!Prt_CheckOutOfPaper()) {
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
			if(!Prt_CheckGapErr()) {
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
		}
	}
			     
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

#ifndef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	} 
#endif
	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	if(i <= end_no) return OFF;
	return ON;
}

INT8U Report_PrintClerkSalesSummary(INT8U mode, INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time, BASIC_REPORT_TYPE *total_br)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	BASIC_REPORT_TYPE br;
	INT16U i;
	char string_buf[100], digit_form[10];
	INT8U decimal, dec_ch;
	RTC_STRUCT time_str;
	INT8U len;
	INT8U flagReport;
//	INT32U tax_value;
//	TAX_STRUCT tax_str;
	INT32U flash_addr;
	INT16S wtemp;
	INT8S variable_len;

	long RptRoundPrice, RptRoundPrice_Scale;
	INT16S  RptRoundCount;//, RptRoundCount_Scale;
    

//	long RptPaidPrice;
//	INT16S  RptPaidCount;
	CLERK_STRUCT clerk_str;
	INT16U shop_no;  //SG060105

#ifdef USE_REPORT_PAY
	INT16U j;
	long volume;
	INT16S  transaction;

	INT16S chg_transactions;
	long chg_volume;

	INT16S ret_transactions;	//Added by JJANG 20071220
	long ret_volume;
#endif
#ifdef USE_TAX_RATE
	long totalVolume, taxVolume;
	TAX_STRUCT tax_str;
	INT8U k;
#endif
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	sr = &sst;

	if(end_no > MAX_CLERK_NO-1) return OFF;

	PrtSensorFlagInit();

	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	flagReport =get_global_bparam(GLOBAL_USE_REPORT);
	flash_addr = DFLASH_BASE + FLASH_CLERK_AREA;

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharLF(2);

	//SG060105_S
	if(GlbPrint.reportStoreName) {
		shop_no = get_net_wparam(NETWORK_STORE_ID);
		TotalRcpt_PrintShopName(shop_no);
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_PrintLine(1, 0);
	} //SG060105_E

	variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
	if(variable_len < 0) variable_len = 0;
	sprintf(digit_form, "%%%dd", REPORT_COUNT_LENGTH+variable_len);

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, LEFT);
#endif
#ifdef USE_ARAB_TICKET_PRT
    receipt_caption_read(RCPTCAP_ID_REP_TITLE_CLERK, string_buf);
    sprintf(string_buf+strlen(string_buf), "%c%d ", ReportTypeChar[mode], period+1);
#else
	sprintf(string_buf, " %c%d", ReportTypeChar[mode], period+1);
	receipt_caption_read(RCPTCAP_ID_REP_TITLE_CLERK, string_buf+3);
#endif
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharLF(1);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintFixedTime(cur_time);
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_LAST_RESET, string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	Report_ReadResetTime(NVRAM_RESET_TIME_CLERK, period, &time_str);
	PrintFixedTime(&time_str);
	RcptCmd_CharLF(1);

#ifdef USE_PRT_SCALE_NUM_IN_REPORT
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, string_buf);	//"SCALE #"
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", status_scale.scaleid);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
#endif

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d, ", start_no);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", end_no);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN );
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	if (GlbPrint.reportType)// transaction count, price, weight, quantity
	{
		Report_PrintBasicData(total_br);
	}
	else
	{
		//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
 #ifdef USE_ARAB_TICKET_PRT
		RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		format_fixed((HUGEDATA INT8U*)string_buf, total_br->brtTransactions, 0, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len, '.');
		PutDataRxBuf(&CommBufPrt, string_buf, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
		
		//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
 #ifdef USE_ARAB_TICKET_PRT
		RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		PrtMakePriceString(string_buf, total_br->brtVolume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
#ifdef USE_TAX_RATE
		totalVolume = 0;
		taxVolume = 0;
		for(k = 1; k < MAX_TAX_NO; k++) 
		{
			flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(k-1));
			Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
			if(k != tax_str.tax_code) continue;
		
			if(tax_str.tax_type == 1)
			{
				get_tax_summary_sum(1, 1, k, period, &taxVolume);
				totalVolume += taxVolume;
			}
		}
 #ifdef USE_ARAB_TICKET_PRT
		RcptCmd_CharTab(TICKET_MARGIN);
#else
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_TAX_PRICE, string_buf); //"TAX PRICE"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(6);
#endif
		PrtMakePriceString(string_buf, totalVolume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len-2);
		PutDataRxBuf(&CommBufPrt,string_buf, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len-2);
		RcptCmd_CharLF(1);
#endif // USE_TAX_RATE
	}

	if(GlbPrint.reportRoundedSummary) {
		// Rounded Price
		RptRoundCount = 0;
		RptRoundPrice = 0;		
		for(i = start_no; i <= end_no; i++) {
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, i);
			get_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, i, sr);
			RptRoundCount += sr->sstTransactions[period];
			RptRoundPrice += sr->sstVolume[period];
		}
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
		RcptCmd_CharTab(TICKET_MARGIN );
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
		receipt_caption_read(RCPTCAP_ID_REP_ROUNDED_TOTAL, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	   	RcptCmd_CharLF(1);

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
		receipt_caption_read(RCPTCAP_ID_REP_ROUNDED_TOTAL, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	   	RcptCmd_CharLF(1);

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
#ifdef USE_ARAB_TICKET_PRT
        RcptCmd_CharTab(TICKET_MARGIN);
#else
        RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
		RcptCmd_PrintArabAllign(string_buf,25);
#else
		RcptCmd_CharTab(5);
#endif
		PrtMakePriceString(string_buf, total_br->brtVolume-RptRoundPrice, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
	   	RcptCmd_CharLF(1);
	}
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);	
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_TypeNone();
	for(i = start_no; i <= end_no; i++) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i, sr);
		if(sr->sstTransactions[period] || sr->sstVolume[period] || sr->sstWeight[period] || sr->sstQty[period] || (flagReport&0x01)) {
 #ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			receipt_caption_read(RCPTCAP_ID_REP_CLERK_NO, string_buf);
			len = strlen(string_buf);
			sprintf(string_buf+len, "%02d", i);	//0x9c24
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
			RcptCmd_PrintArabAllign(string_buf,25);
#else
			RcptCmd_CharTab(5);
#endif
			Flash_sread(flash_addr+(INT32U)i*CLERK_STRUCT_SIZE, (HUGEDATA INT8U *)&clerk_str, sizeof(CLERK_STRUCT));
			if(clerk_str.code != i) clerk_str.name[0] = 0;
			sprintf(string_buf, "%s", clerk_str.name);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN);
			RcptCmd_PrintLine(1, 0);

			if (GlbPrint.reportType)// transaction count, price, weight, quantity
			{
				br.brtTransactions = sr->sstTransactions[period];
				br.brtVolume = sr->sstVolume[period];
				br.brtWeight = sr->sstWeight[period];
				br.brtQty = sr->sstQty[period];
				Report_PrintBasicData(&br);
			}
			else
			{
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
				//sprintf(string_buf, "TOTAL SUMMARY");
				receipt_caption_read(RCPTCAP_ID_REP_SUBTOTAL_SUMMARY, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				len = strlen(string_buf);
				RcptCmd_PrintChar(' ');
				len++;
				RcptCmd_PrintCharLine('-', Rcpt_MaxWidthChar-len-REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);

#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_SUBTOTAL, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, sr->sstTransactions[period]);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}
//			RptRoundCount_Scale = sr->sstTransactions[period];
			RptRoundPrice_Scale = sr->sstVolume[period];

			if(GlbPrint.reportRoundedSummary) {
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, i);
                get_nvram_summary_addr(NVRAM_ROUND_TOTAL_CLERK, i, sr);
				RptRoundPrice_Scale -= sr->sstVolume[period];
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_ROUNDED_SUBTOTAL, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				memset(string_buf, 0x20, REPORT_COUNT_LENGTH+variable_len);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_COUNT_LENGTH+variable_len);
				PrtMakePriceString(string_buf, RptRoundPrice_Scale, decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}

			RcptCmd_CharLF(1);


 #ifdef USE_ARAB_TICKET_PRT
			RcptCmd_CharTab(TICKET_MARGIN);
#else
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
			//sprintf(string_buf, "SALE SUMMARY");
			receipt_caption_read(RCPTCAP_ID_REP_SALE_SUMMARY, string_buf);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			len = strlen(string_buf);
			RcptCmd_PrintChar(' ');
			len++;
			RcptCmd_PrintCharLine('-', Rcpt_MaxWidthChar-len-REPORT_PRICE_LENGTH);
			RcptCmd_CharLF(1);

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_CLERK, i);
			get_nvram_summary_addr(NVRAM_VOID_CLERK, i, sr);
			if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
			{
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_VOID_COUNT, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, sr->sstTransactions[period]);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}
			if(GlbPrint.reportRepack) {
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_CLERK, i);
                get_nvram_summary_addr(NVRAM_REPACK_CLERK, i, sr);
				if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
				{
#ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
#else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_REPACK_COUNT, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(5);
#endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(1);
				}
			}

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i);
			get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i, sr);
			if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
			{
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_OVERRIDE_COUNT, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, sr->sstTransactions[period]);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}
			
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, i);
			get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, i, sr);
			
			if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
			{
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_DISCOUNT_COUNT, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, sr->sstTransactions[period]);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}
			if(GlbPrint.reportPrepack) {
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_CLERK, i);
				get_nvram_summary_addr(NVRAM_PREPACK_CLERK, i, sr);
				{
#ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
#else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_PREPACK_COUNT, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(5);
#endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(1);
				}
			}

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_CLERK, i);
			get_nvram_summary_addr(NVRAM_RETURN_CLERK, i, sr);
			if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
			{
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_RETURN_COUNT, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, sr->sstTransactions[period]);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
				PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);
			}
			if(GlbPrint.reportNegative) {
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, i);
				get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, i, sr);
				if(GlbPrint.reportZeroValue || sr->sstTransactions[period] != 0)
				{
#ifdef USE_ARAB_TICKET_PRT
					RcptCmd_CharTab(TICKET_MARGIN);
#else
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
					receipt_caption_read(RCPTCAP_ID_REP_NEGATIVE_COUNT, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
					RcptCmd_PrintArabAllign(string_buf,25);
#else
					RcptCmd_CharTab(5);
#endif
					sprintf(string_buf, digit_form, sr->sstTransactions[period]);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(1);
				}
			}

			wtemp = get_nvram_wparam(NVRAM_CASH_CLERK+i*4+period*2);
			if(GlbPrint.reportZeroValue || wtemp != 0)
				{
#ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_NOSALEOPEN_COUNT, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
				RcptCmd_PrintArabAllign(string_buf,25);
#else
				RcptCmd_CharTab(5);
#endif
				sprintf(string_buf, digit_form, wtemp);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharLF(1);
			}


			RcptCmd_CharLF(1);

#ifdef USE_REPORT_PAY	//#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
			if(GlbPrint.reportPay) {
 #ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
				receipt_caption_read(RCPTCAP_ID_REP_PAY_SUMMARY, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				len = strlen(string_buf);
				RcptCmd_PrintChar(' ');
				len++;
				RcptCmd_PrintCharLine('-', Rcpt_MaxWidthChar-len-REPORT_PRICE_LENGTH);
				RcptCmd_CharLF(1);

 #ifdef USE_ARAB_TICKET_PRT
				RcptCmd_CharTab(TICKET_MARGIN);
#else
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif
				//sprintf(string_buf, "PAY");
				receipt_caption_read(RCPTCAP_ID_REP_PAY, string_buf);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharLF(1);

				// Change Amount 미리 계산
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CHANGE_CLERK, i);
				get_nvram_summary_addr(NVRAM_CHANGE_CLERK, i, sr);
				chg_transactions = sr->sstTransactions[period];
				chg_volume = sr->sstVolume[period];
				/////////////////////////////
				//Added by JJANG 20071220  RETURN일때 Cash '-'처리
				//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_CLERK, i);
				get_nvram_summary_addr(NVRAM_RETURN_CLERK, i, sr);
				ret_transactions = sr->sstTransactions[period];
				ret_volume = sr->sstVolume[period];

				RptRoundCount = 0;
				RptRoundPrice = 0;
				for(j = 0; j < MAX_PAID_TYPE; j++) {
					//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_CLERK, i*(MAX_PAID_TYPE)+j);
					get_nvram_summary_addr(NVRAM_PAY_CLERK, i*(MAX_PAID_TYPE)+j, sr);
					volume = sr->sstVolume[period];
					transaction = sr->sstTransactions[period];
					if (j == 0 && !GlbPrint.reportChange) {
						volume -= chg_volume;	// change를 출력하지 않을 때, cash에서 change를 빼줌
					}

					if (j == 0 && ret_transactions)	//Added by JJANG 20071220  RETURN일때 Cash '-'처리
					{
						transaction -=ret_transactions; 
						volume -= ret_volume;
					}
					if(GlbPrint.reportZeroValue || transaction != 0)
					{			
 #ifdef USE_ARAB_TICKET_PRT
						RcptCmd_CharTab(TICKET_MARGIN);
#else
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
#endif
						string_buf[0]='-';
						PutDataRxBuf(&CommBufPrt,string_buf,1);
						receipt_caption_read(RCPTCAP_ID_COM_PAY_CASH+j, string_buf);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
						RcptCmd_PrintArabAllign(string_buf,25);
#else
						RcptCmd_CharTab(4);
#endif
						sprintf(string_buf, digit_form, transaction);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
						PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
						RcptCmd_CharLF(1);
					}
					RptRoundCount += sr->sstTransactions[period];
					RptRoundPrice += sr->sstVolume[period];
				}

				if (GlbPrint.reportChange) {
					if(GlbPrint.reportZeroValue || RptRoundCount != 0)
					{			
 #ifdef USE_ARAB_TICKET_PRT
						RcptCmd_CharTab(TICKET_MARGIN);
#else
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
#endif
						receipt_caption_read(RCPTCAP_ID_REP_PAY_SUBTOTAL, string_buf);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
						RcptCmd_PrintArabAllign(string_buf,25);
#else
						RcptCmd_CharTab(4);
#endif
						sprintf(string_buf, digit_form, RptRoundCount);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						PrtMakePriceString(string_buf, RptRoundPrice, decimal, dec_ch, REPORT_PRICE_LENGTH);
						PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
						RcptCmd_CharLF(1); 
					}
					//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CHANGE_CLERK, i);
					if(GlbPrint.reportZeroValue || chg_transactions != 0)
					{			
#ifdef USE_ARAB_TICKET_PRT
						RcptCmd_CharTab(TICKET_MARGIN);
#else
						RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
#endif
						receipt_caption_read(RCPTCAP_ID_TKT_CHANGE, string_buf);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_ARAB_CONVERT
						RcptCmd_PrintArabAllign(string_buf,25);
#else
						RcptCmd_CharTab(5);
#endif
						//sprintf(string_buf, digit_form, sr->sstTransactions[period]);
						sprintf(string_buf, digit_form, chg_transactions);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						//PrtMakePriceString(string_buf, sr->sstVolume[period], decimal, dec_ch, REPORT_PRICE_LENGTH);
						PrtMakePriceString(string_buf, chg_volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
						PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
						RcptCmd_CharLF(1);
					}
				}
			}
#endif

			RcptCmd_CharTab(TICKET_MARGIN);
			RcptCmd_PrintLine(1, 0);
			RcptCmd_CharLF(1);

			PrintListWaitEnd();

			if(!Prt_CheckOutOfPaper()) {
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
			if(!Prt_CheckGapErr()) {
				FlushRxBuf(&CommBufPrt);
				RcptCmd_ChangeMode(TO_RESTORE_FORCE, ON);
				return OFF;
			}
		}
	}
			     
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

#ifndef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	} 
#endif
	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	if(i <= end_no) return OFF;
	return ON;
}

#ifdef USE_INDIA_TAX_REPORT_MENU
INT8U Report_PrintTaxSalesSummary(INT8U mode, INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time, BASIC_REPORT_TYPE *total_br)
{
		SALES_SUMMARY_TYPE *sr;
		BASIC_REPORT_TYPE br;
		INT16U i, j;
		char string_buf[100], digit_form[10];
		INT8U decimal, dec_ch;
		RTC_STRUCT time_str;
		INT8U len;
		INT8U flagReport;
		INT16U prt_end_no, prt_start_no;
		INT16S wtemp;
		INT8S variable_len;
		long RptRoundPrice, RptRoundPrice_Scale, volume, totalVolume;
		INT16S	RptRoundCount, transaction;
		INT32U flash_addr;
#ifdef USE_INDIA_TAX
		char temp_string[35];
		char currency_sign1[5];
		TAX_STRUCT tax_str;
#endif
		if(!Prt_CheckPeelAndHead(OFF)) return OFF;
		if(!Prt_CheckOutOfPaper()) return OFF;
	
		if(end_no > MAX_SCALE-1) end_no = MAX_SCALE-1;
		prt_start_no = start_no;
		prt_end_no	 = end_no;
		if(start_no == 0) {
			end_no = 0;
			prt_start_no = 1;
			prt_end_no	 = MAX_SCALE - 1;
		}
		PrtSensorFlagInit();

		totalVolume = 0;
		volume = 0;
		for(j = 1; j < MAX_TAX_NO; j++)
		{
			flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(j-1));
			Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
			if(j != tax_str.tax_code) continue;
	
			if(tax_str.tax_type == 1)
			{
				if(i == 0)
					get_tax_summary_sum(1, MAX_SCALE-1, j, period, &volume);
				else
					get_tax_summary_sum(1, 1, j, period, &volume);
				totalVolume += volume;
			}
		}

	
		decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
		dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
		flagReport =get_global_bparam(GLOBAL_USE_REPORT);
		RcptCmd_ChangeMode(TO_RECEIPT, ON);
		RcptCmd_DefaultSet();
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharLF(2);
	
	
		variable_len = Rcpt_MaxWidthChar - REPORT_PRICE_LENGTH - REPORT_COUNT_LENGTH - REPORT_TITLE_LENGTH - REPORT_RMARGIN_LENGTH - REPORT_LMARGIN_LENGTH;
		if(variable_len < 0) variable_len = 0;
		sprintf(digit_form, "%%%dd", REPORT_COUNT_LENGTH+variable_len);
	
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, LEFT);
		sprintf(string_buf, "%c%d TAX REPORT", ReportTypeChar[mode], period+1);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);
	
	//	RcptCmd_CharLF(1);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		PrintFixedTime(cur_time);
		RcptCmd_CharLF(1);

#ifdef USE_PRT_SCALE_NUM_IN_REPORT
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, string_buf);	//"SCALE #"
		len = strlen(string_buf);
		sprintf(string_buf+len, "%02d", status_scale.scaleid);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
#endif
	
		if(start_no == 0) {
			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);

			receipt_caption_read(RCPTCAP_ID_REP_GRAND_SCALE_TOTAL, string_buf);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);
		}
	
	//	if (ethernet_list.status!=2) {	// if Master
		if (ethernet_list.status==1) {	// if Master

			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
			len = strlen(string_buf);
			sprintf(string_buf+len, "%02d, ", prt_start_no);
			len = strlen(string_buf);
			receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
			len = strlen(string_buf);
			sprintf(string_buf+len, "%02d", prt_end_no);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);
		}

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN);
		RcptCmd_PrintLine(2, 0);	//"============================="
	
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_TOTAL, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
	
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);

		//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_COUNT, string_buf);	//"COUNT"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_CharTab(5);

		format_fixed((HUGEDATA INT8U*)string_buf, total_br->brtTransactions, 0, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len, '.');
		PutDataRxBuf(&CommBufPrt, string_buf, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		RcptCmd_CharLF(1);

		//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
		receipt_caption_read(RCPTCAP_ID_REP_GRAND_PRICE, string_buf);	//"PRICE"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_CharTab(5);

		PrtMakePriceString(string_buf, totalVolume, decimal, dec_ch, REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH+REPORT_COUNT_LENGTH+variable_len);
		RcptCmd_CharLF(1);
		
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN);
		RcptCmd_PrintLine(2, 0);	//"============================="


#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
	if(GlbPrint.reportTax) {
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_REP_TAX_SUMMARY, string_buf);	//"TAX SUMMARY"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		if(get_global_bparam(GLOBAL_TAX_FLAG) != 0) {
			temp_string[0] = 0;
			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+2);
			get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
			currency_sign1[4] = 0;
			
			sprintf(string_buf, " GST(%%)     CGST(%s)      SGST(%s)      GST(%s)", currency_sign1, currency_sign1, currency_sign1);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

			RcptCmd_CharLF(1);
			temp_string[0] = 0;
			for(j = 1; j < MAX_TAX_NO; j++) {
				if(i == 0) {
					get_tax_summary_sum(1, MAX_SCALE-1, j, period, &volume);
				} else {
					get_tax_summary_sum(i, i, j, period, &volume);
				}
				if(GlbPrint.reportZeroValue || volume != 0)
				{
					flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(j-1));
					Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
					if(j != tax_str.tax_code) {
						sprintf(string_buf, "%d:", j);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
						receipt_caption_read(RCPTCAP_ID_COM_NOT_AVAILABLE , string_buf);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					} else {
						RcptCmd_PutStringInit(string_buf);
						sprintf(string_buf, "%d:%5.2f%%", j, (float)(tax_str.tax_rate)/100.0);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

						PrtMakePriceString(temp_string, volume/2, decimal, dec_ch, REPORT_PRICE_LENGTH-1);
						sprintf(string_buf, "%s", temp_string);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

						PrtMakePriceString(temp_string, volume/2, decimal, dec_ch, REPORT_PRICE_LENGTH-1);
						sprintf(string_buf, "%s", temp_string);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

						PrtMakePriceString(temp_string, volume, decimal, dec_ch, REPORT_PRICE_LENGTH-2);
						sprintf(string_buf, "%s", temp_string);
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

						if(tax_str.tax_type == 0) { // INTERNAL
							receipt_caption_read(RCPTCAP_ID_COM_INT_TAX , string_buf);
						} else {
#ifdef USE_INDIA_TAX
							sprintf(string_buf, " ");
#else
							receipt_caption_read(RCPTCAP_ID_COM_EXT_TAX , string_buf);
#endif
						}
						PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					}
					RcptCmd_CharTab(5);

					memset(string_buf, 0x20, REPORT_COUNT_LENGTH+variable_len);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_COUNT_LENGTH+variable_len);
					//tax_value = get_nvram_lparam(NVRAM_TAX+(INT32U)i*(INT32U)((j-1)*MAX_REPORT_PERIOD+period)*4);
					PrtMakePriceString(string_buf, volume, decimal, dec_ch, REPORT_PRICE_LENGTH);
					PutDataRxBuf(&CommBufPrt,string_buf,REPORT_PRICE_LENGTH);
					RcptCmd_CharLF(1);
				}
			}
		}
	}
#endif

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	if(i <= end_no) return OFF;
	return ON;


}

#endif


INT8U DisplayPluSalesSummary(INT16S period, INT16U start_dept, INT16U end_dept, long start_no, long end_no, INT8U noprice, BASIC_REPORT_TYPE *br)
{
	PLU_SALES_SUMMARY_TYPE *psr;
	PLU_SALES_SUMMARY_TYPE pst;
	INT16U next;
	INT16U j;
	char string_buf[100];
	INT8U decimal;
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	INT8U dec_ch;
#endif
	POINT disp_p;
	INT8U size;//, len;
	char backup_direct_draw;
	INT8U ret;
	INT16S plu_tran;
	INT16U index,plukey;
	CAPTION_STRUCT cap;
#ifdef USE_TAX_RATE
	INT8U i;
	long volume, totalVolume;
	INT32U flash_addr;
	TAX_STRUCT tax_str;
#endif
	psr = &pst;

	ret=0;
	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
#endif

	backup_direct_draw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;

#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_PRICE);
#else
	memset((INT8U *)string_buf, ' ', 32);
	string_buf[32] = 0;
	disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
	hebrew_codetemp[strlen(string_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0);
	arab_codetemp[strlen(string_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
#endif

	caption_split_by_code(0x1aa5, &cap,0);
	sprintf(string_buf, (char*)cap.form);
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_string(DISPLAY_PRICE,(INT8U *)string_buf);
#else
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
	hebrew_codetemp[strlen(cap.form)] = 0;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0);
	arab_codetemp[strlen(cap.form)] = 0;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);	
#else
	Dsp_Write_String(disp_p, (HUGEDATA char *)cap.form);
#endif
#endif

	br->brtTransactions = 0;
	br->brtVolume = 0;
	br->brtWeight = 0;
	br->brtQty = 0;
	plu_tran = 0;

	for(j = start_dept; j <= end_dept; j++) {
		index = get_nvram_wparam(NVRAM_PLUSUM_HEADINDEX);
		//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index);
		get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, psr);
		while(psr->pstPLU < start_no) {
			if(psr->pstNext == 0) break;
			//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
			next = psr->pstNext;
			get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
		}
		while(psr->pstPLU >= start_no && psr->pstPLU <= end_no) {
			if(psr->pstDept == j) {
//				if(plu_table_search(psr->pstDept, psr->pstPLU) || psr->pstPLU >= MISC_PLU_WEIGHT) {
				if(plu_table_search(psr->pstDept, psr->pstPLU,&plukey,0) || psr->pstPLU >= MISC_PLU_WEIGHT) {
					//if(psr->pstTransactions[period]) {
						br->brtTransactions += psr->pstTransactions[period];
						br->brtVolume += psr->pstVolume[period];
						br->brtWeight += psr->pstWeight[period];
						br->brtQty += psr->pstQty[period];
						plu_tran += psr->pstCountNumber;
					//}
					ret=1;
				}
			}
			if(psr->pstNext == 0) break;
			//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
			next = psr->pstNext;
			get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
		}
	}
#ifdef USE_TAX_RATE
	totalVolume = 0;
	volume = 0;
	for(i = 1; i < MAX_TAX_NO; i++) 
	{
		flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(i-1));
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
		if(i != tax_str.tax_code) continue;

		if(tax_str.tax_type == 1)
		{
			if(i == 0)
				get_tax_summary_sum(1, MAX_SCALE-1, i, period, &volume);
			else
				get_tax_summary_sum(1, 1, i, period, &volume);
			totalVolume += volume;
		}
	}
	if(start_no < MISC_PLU_WEIGHT)
	br->brtVolume += totalVolume;
#endif // USE_TAX_RATE
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_UNITPRICE);
	display_clear(DISPLAY_PRICE);
	if(noprice) {
		caption_split_by_code(0x1aa6, &cap,0);
		display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);
		display_ulong(DISPLAY_PRICE, plu_tran, 0);
	} else {
		caption_split_by_code(0x1aa7, &cap,0);
		display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);
		display_ulong(DISPLAY_PRICE, br->brtVolume, decimal);
	}
#else
	memset((INT8U *)string_buf, ' ', 32);
	string_buf[32] = 0;
	disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
	hebrew_codetemp[strlen(string_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
	arab_codetemp[strlen(string_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif

	caption_split_by_code(0x1aa6, &cap,0);
	strcat((char *)cap.form,"%d");
	if(!noprice) {
		sprintf(string_buf, (char*)cap.form, br->brtTransactions);
	} else {
		sprintf(string_buf, (char*)cap.form, plu_tran);
	}
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
	hebrew_codetemp[strlen(string_buf)] = 0;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
	arab_codetemp[strlen(string_buf)] = 0;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
//	len = strlen(string_buf);

	memset((INT8U *)string_buf, ' ', 32);
	string_buf[32] = 0;
	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
	hebrew_codetemp[strlen(string_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
	arab_codetemp[strlen(string_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif

	if(!noprice) {
		caption_split_by_code(0x1aa7, &cap,0);
		sprintf(string_buf, (char*)cap.form, 0);
		size = strlen(string_buf);
#ifdef USE_KOREAN_IN_GBR
		PrtGenSystemCurPriceStr2(OUTMODE_DISPLAY, (HUGEDATA INT8U *)string_buf+size, br->brtVolume, decimal, dec_ch);
#else
		PrtGenSystemCurPriceStr(OUTMODE_DISPLAY, (HUGEDATA INT8U *)string_buf+size, br->brtVolume, decimal, dec_ch);
#endif	//#ifdef USE_KOREAN_IN_GBR
		disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
	}
#endif
	Dsp_Diffuse();
	DspStruct.DirectDraw = backup_direct_draw;
	return ret;
}

void DisplaySalesSummary(INT8U rep_mode, INT16S period, INT16U start_no, INT16U end_no, INT32U nv_addr, BASIC_REPORT_TYPE *br)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	INT16U i;
	char string_buf[100];
	INT8U decimal;
	POINT disp_p;
	INT8U size;//, len;
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	INT8U dec_ch;
	char backup_direct_draw;
#endif
	CAPTION_STRUCT cap;
#ifdef USE_TAX_RATE
	INT8U j;
	long volume, totalVolume;
	INT32U flash_addr;
	TAX_STRUCT tax_str;
#endif
	decimal=get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	
	backup_direct_draw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;
#endif

	if(rep_mode == REPORT_SCALE && start_no == 0) {
		start_no = 1;
		end_no = MAX_SCALE - 1;
	}

	sr = &sst;

	br->brtTransactions = 0;
	br->brtVolume = 0;
	br->brtWeight = 0;
	br->brtQty = 0;
	for(i = start_no; i <= end_no; i++) {
		//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(nv_addr, i);
		get_nvram_summary_addr(nv_addr, i, sr);
		br->brtTransactions += sr->sstTransactions[period];
		br->brtVolume += sr->sstVolume[period];
		br->brtWeight += sr->sstWeight[period];
		br->brtQty += sr->sstQty[period];
	}
#ifdef USE_TAX_RATE
	totalVolume = 0;
	volume = 0;
	for(j = 1; j < MAX_TAX_NO; j++) 
	{
		flash_addr = DFLASH_BASE + FLASH_TAX_AREA + (TAX_STRUCT_SIZE * (INT32U)(j-1));
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
		if(j != tax_str.tax_code) continue;

		if(tax_str.tax_type == 1)
		{
			if(i == 0)
				get_tax_summary_sum(1, MAX_SCALE-1, j, period, &volume);
			else
				get_tax_summary_sum(1, 1, j, period, &volume);
			totalVolume += volume;
		}
	}
	if(nv_addr != NVRAM_HOURLY_SUMMARY)
		br->brtVolume += totalVolume;
#endif // USE_TAX_RATE
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_UNITPRICE);
	display_clear(DISPLAY_PRICE);

	caption_split_by_code(0x1aa7, &cap,0);
	display_string(DISPLAY_UNITPRICE, (INT8U*)cap.form);
	display_ulong(DISPLAY_PRICE, br->brtVolume, decimal);
#else
	memset((INT8U *)string_buf, ' ', 32);
	string_buf[32] = 0;
	disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
	hebrew_codetemp[strlen(string_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
	arab_codetemp[strlen(string_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif

	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
	hebrew_codetemp[strlen(string_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
	arab_codetemp[strlen(string_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif

	caption_split_by_code(0x1aa6, &cap,0);
	strcat((char *)cap.form,"%d");
	sprintf(string_buf, (char*)cap.form, br->brtTransactions);
	disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
	hebrew_codetemp[strlen(string_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
	arab_codetemp[strlen(string_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
//	len = strlen(string_buf);

	caption_split_by_code(0x1aa7, &cap,0);
	sprintf(string_buf, (char*)cap.form, 0);
	size = strlen(string_buf);
#ifdef USE_KOREAN_IN_GBR
	PrtGenSystemCurPriceStr2(OUTMODE_DISPLAY, (HUGEDATA INT8U *)string_buf+size, br->brtVolume, decimal, dec_ch);
#else
#ifdef USE_INDIA_TAX_REPORT_MENU
	if(rep_mode == REPORT_TAX)
		PrtGenSystemCurPriceStr(OUTMODE_DISPLAY, (HUGEDATA INT8U *)string_buf+size, totalVolume, decimal, dec_ch);
	else
#endif
	PrtGenSystemCurPriceStr(OUTMODE_DISPLAY, (HUGEDATA INT8U *)string_buf+size, br->brtVolume, decimal, dec_ch);
#endif	//#ifdef USE_KOREAN_IN_GBR
	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
	hebrew_codetemp[strlen(string_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
	arab_codetemp[strlen(string_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
	Dsp_Diffuse();
	DspStruct.DirectDraw = backup_direct_draw;
#endif
}

extern INT16S ClearPluSummaryRecord(PLU_SALES_SUMMARY_TYPE* tp,INT16S counter);
extern INT16S ClearSummaryRecord(SALES_SUMMARY_TYPE* tp,INT16S counter);
void Report_ClearPLUReport(INT16S period, long start_dept, long end_dept, long start_no, long end_no, RTC_STRUCT *cur_time)
{
	long dept;
	INT16U index;
	PLU_SALES_SUMMARY_TYPE* psr;
	PLU_SALES_SUMMARY_TYPE pst;
	INT16U next;

	psr = &pst;

	Report_SetResetTime(NVRAM_RESET_TIME_PLU, cur_time, period);

	for(dept = start_dept; dept <= end_dept; dept++) 
	{
		index = get_nvram_wparam(NVRAM_PLUSUM_HEADINDEX);
		//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index);
		get_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, psr);
		next = index;
		while(psr->pstPLU < start_no) 
		{
			if(psr->pstNext == 0) break;
			//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
			next = psr->pstNext;
			get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
		}
		while(psr->pstPLU >= start_no && psr->pstPLU <= end_no) 
		{
			if(psr->pstDept == dept) 
			{
				ClearPluSummaryRecord(psr, period);
				set_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, &pst);
			}
			if(psr->pstNext == 0) break;
			//psr = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, psr->pstNext);
			next = psr->pstNext;
			get_nvram_plu_summary(NVRAM_PLU_SUMMARY, next, psr);
		}
	}
}

void Report_ClearGroupReport(INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time)
{
	INT16U i;
	SALES_SUMMARY_TYPE* tp;
	SALES_SUMMARY_TYPE sst;
	INT32S count;

	tp = &sst;

	Report_SetResetTime(NVRAM_RESET_TIME_GROUP, cur_time, period);
	for (i = start_no; i <= end_no; i++) 
	{
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_GROUP_SUMMARY, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_GROUP, i);
		get_nvram_summary_addr(NVRAM_PREPACK_GROUP, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_PREPACK_GROUP, i, tp);
#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_GROUP, i);
		get_nvram_summary_addr(NVRAM_RETURN_GROUP, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_RETURN_GROUP, i, tp);
#endif
	}
	//Added by JJANG 20080529
	if (GlbPrint.reportType == 2)
	{
		//CounterTicketGroupZ[period]++;
		//if (CounterTicketGroupZ[period] > 999999L) CounterTicketGroupZ[period] = 1L;
		count = get_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*period);
		count++;
		if (count > 999999L) count = 1L;
		set_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*period, count);
	}
}

void Report_ClearDeptReport(INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time)
{
	INT16U i;
	SALES_SUMMARY_TYPE* tp;
	SALES_SUMMARY_TYPE sst;

	tp = &sst;

	Report_SetResetTime(NVRAM_RESET_TIME_DEPT, cur_time, period);
	for (i = start_no; i <= end_no; i++) {
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_DEPT_SUMMARY, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_DEPT, i);
		get_nvram_summary_addr(NVRAM_PREPACK_DEPT, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_PREPACK_DEPT, i, tp);
#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_DEPT, i);
		get_nvram_summary_addr(NVRAM_RETURN_DEPT, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_RETURN_DEPT, i, tp);
#endif
	}
}

void Report_ClearScaleReport(INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time)
{
	INT16U i;
#if (defined(USE_REPORT_PAY) || (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE)))	// REPORT
	INT16U j;
#endif
	SALES_SUMMARY_TYPE* tp;
	SALES_SUMMARY_TYPE sst;

	tp = &sst;

	Report_SetResetTime(NVRAM_RESET_TIME_SCALE, cur_time, period);
	Report_SetResetTime(NVRAM_RESET_TIME_TOTAL, cur_time, period);
	for(i = start_no; i <= end_no; i++) {
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_SCALE_SUMMARY, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_SCALE, i);
		get_nvram_summary_addr(NVRAM_VOID_SCALE, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_VOID_SCALE, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_SCALE, i);
		get_nvram_summary_addr(NVRAM_REPACK_SCALE, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_REPACK_SCALE, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, i);
		get_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_OVERRIDE_SCALE, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, i);
		get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_SCALE, i);
		get_nvram_summary_addr(NVRAM_PREPACK_SCALE, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_PREPACK_SCALE, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_SCALE, i);
		get_nvram_summary_addr(NVRAM_RETURN_SCALE, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_RETURN_SCALE, i, tp);
		set_nvram_wparam(NVRAM_CASH_SCALE+i*2, 0);
#ifdef USE_REPORT_PAY	//#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
		for(j = 0; j < MAX_PAID_TYPE; j++) {
			//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PAY_SCALE, i*(MAX_PAID_TYPE)+j);
			get_nvram_summary_addr(NVRAM_PAY_SCALE, i*(MAX_PAID_TYPE)+j, tp);
			ClearSummaryRecord(tp,period);
			set_nvram_summary_addr(NVRAM_PAY_SCALE, i*(MAX_PAID_TYPE)+j, tp);
		}
#endif
#if (defined(USE_REPORT_TAX) && defined(USE_TAX_RATE))
		for (j=0;j<MAX_TAX_NO;j++) {
			set_nvram_lparam(NVRAM_TAX+(INT32U)(i*MAX_TAX_NO*MAX_REPORT_PERIOD+j*MAX_REPORT_PERIOD+period)*4L, 0);
			set_nvram_lparam(NVRAM_TAX_GROSS+(INT32U)(i*MAX_TAX_NO*MAX_REPORT_PERIOD+j*MAX_REPORT_PERIOD+period)*4L, 0);
		}
#endif
	}
}

void Report_ClearHourlyReport(INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time)
{
	INT16U i;
	SALES_SUMMARY_TYPE* tp;
	SALES_SUMMARY_TYPE sst;

	tp = &sst;

	Report_SetResetTime(NVRAM_RESET_TIME_HOURLY, cur_time, period);
	for (i = start_no; i <= end_no; i++) {
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_HOUR, i);
		get_nvram_summary_addr(NVRAM_PREPACK_HOUR, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_PREPACK_HOUR, i, tp);
	}
}

void Report_ClearClerkReport(INT16S period, INT16U start_no, INT16U end_no, RTC_STRUCT *cur_time)
{
	INT16U i;
#ifdef USE_REPORT_PAY	// REPORT
	INT16U j;
#endif
	SALES_SUMMARY_TYPE* tp;
	SALES_SUMMARY_TYPE sst;

	tp = &sst;

	Report_SetResetTime(NVRAM_RESET_TIME_CLERK, cur_time, period);
	for (i = start_no; i <= end_no; i++) {
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i);
		get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_CLERK_SUMMARY, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_VOID_CLERK, i);
		get_nvram_summary_addr(NVRAM_VOID_CLERK, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_VOID_CLERK, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_REPACK_CLERK, i);
		get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i);
		get_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_OVERRIDE_CLERK, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, i);
		get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_PREPACK_CLERK, i);
		get_nvram_summary_addr(NVRAM_PREPACK_CLERK, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_PREPACK_CLERK, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_RETURN_CLERK, i);
		get_nvram_summary_addr(NVRAM_RETURN_CLERK, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_RETURN_CLERK, i, tp);
		//tp = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, i);
		get_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, i, tp);
		ClearSummaryRecord(tp,period);
		set_nvram_summary_addr(NVRAM_NEGATIVE_CLERK, i, tp);
		set_nvram_wparam(NVRAM_CASH_CLERK+i*2, 0);
#ifdef USE_REPORT_PAY	//#if !(ROM_COUNTRY == ROM_RUS)	// REPORT
		for(j = 0; j < MAX_PAID_TYPE; j++) {
			ClearSummaryRecord(tp,period);
			get_nvram_summary_addr(NVRAM_PAY_CLERK, i*(MAX_PAID_TYPE)+j, tp);
		}
#endif
	}
}

INT8U Report_Confirm(char *msg)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
//	INT32U	addr_cap;
	INT8U loop, result;
	INT8U y;

	result = OFF;
	loop = ON;


	BuzOnAdd(1);
	Dsp_SetPage(DSP_ERROR_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);

	Dsp_Fill_Buffer(0);
//	addr_cap = caption_search_by_code(0x9500);
//	_memcpy((INT8U*)&cap,(INT8U *)addr_cap, sizeof(cap));
	y = 0;
	if (msg)
	{
		disp_p = point((Startup.menu_type + y)*Startup.menu_ygap, 0);
		Dsp_Write_String(disp_p, (HUGEDATA char*)msg);
		y++;
	}
	caption_split_by_code(0x9500,&cap,0);
	disp_p = point((Startup.menu_type + y)*Startup.menu_ygap, 0);
#ifdef USE_ARAB_CONVERT
    Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
    arab_codetemp[strlen(cap.form)] = 0;
    disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
    Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)&cap.form);
#endif

	Dsp_Diffuse();

	while(loop) {
		if(KEY_Read()) {
			switch(KeyDrv.CnvCode)
			{
				case KP_ESC :
				case KEY_NUM_0 :
				case KP_ASC_N :
					BuzOn(1);
					loop = OFF;
					result = OFF;
					break;
				case KEY_NUM_1 :
				case KP_ASC_Y :
					BuzOn(1);
					loop = OFF;
					result = ON;
					break;
				default :
					BuzOn(2);
					break;
			}
		}
	}

	Dsp_SetPage(DSP_DEFAULT_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);
	Dsp_Diffuse();

	return result;
}


INT8U Report_ConfirmReset(INT8U warning_msg)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	INT8U result;


	BuzOnAdd(1);
	Dsp_SetPage(DSP_ERROR_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);

	Dsp_Fill_Buffer(0);
	if(warning_msg && PrtPaperType == 0 && !EnablePrintOnLabel && ConfirmPrintOnLabel != 0) {
		caption_split_by_code(0x9c83,&cap,0);
		disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)&cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)&cap.form);
#endif
	}

	Dsp_Diffuse();
	result = Menu_ConfirmDisplay(0);

	Dsp_SetPage(DSP_DEFAULT_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);
	Dsp_Diffuse();

	EnablePrintOnLabel = result;
	if(ConfirmPrintOnLabel >= 3) EnablePrintOnLabel = OFF;

	return result;
}


INT8U Report_ConfirmPrint(void)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
//	INT32U	addr_cap;
	INT8U loop, result;

	//if(PrtStruct.Mode == RECEIPT || PrtDrvStruct.LabelGapMode == NO_GAP || ConfirmPrintOnLabel == 0 || EnablePrintOnLabel) return ON;
	if(PrtPaperType != 0 || ConfirmPrintOnLabel == 0 || EnablePrintOnLabel) return ON;
	
	result = OFF;
	loop = ON;

	BuzOnAdd(1);
	Dsp_SetPage(DSP_ERROR_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);

	Dsp_Fill_Buffer(0);
//	addr_cap = caption_search_by_code(0x9c81);
//	_memcpy((INT8U*)&cap,(INT8U *)addr_cap, sizeof(cap));
	caption_split_by_code(0x9c81,&cap,0);
	disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
	Dsp_Write_String(disp_p,(HUGEDATA char*)&cap.form);

//	addr_cap = caption_search_by_code(0x9c82);
//	addr_cap = caption_search_by_code(0xf034);
//	_memcpy((INT8U*)&cap,(INT8U *)addr_cap, sizeof(cap));
	caption_split_by_code(0xf034,&cap,0);

	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
	Dsp_Write_String(disp_p,(HUGEDATA char*)&cap.form);

	Dsp_Diffuse();

	while(loop) {
		if(KEY_Read()) {
			switch(KeyDrv.CnvCode)
			{
				case KP_ESC :
				case KEY_NUM_0 :
				case KP_ASC_N :
					BuzOn(1);
					loop = OFF;
					result = OFF;
					break;
				case KEY_NUM_1 :
				case KP_ASC_Y :
					BuzOn(1);
					loop = OFF;
					result = ON;
					break;
				default :
					BuzOn(2);
					break;
			}
		}
	}

	Dsp_SetPage(DSP_DEFAULT_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);
	Dsp_Diffuse();

	EnablePrintOnLabel = result;
	if(ConfirmPrintOnLabel >= 3) EnablePrintOnLabel = OFF;

	return result;
}

#if defined(USE_SINGLE_LINE_GRAPHIC)
INT16U Report_PluInputRange(long *pStartDept, long *pEndDept, long *pStartNo, long *pEndNo)
{
	char string_buf[40],format[40];
	CAPTION_STRUCT cap;
	INT16U result;

	if(Startup.disable_dept) {
		*pStartDept = 1;
	}

	*pEndDept = *pStartDept;

	while(1)
	{
		Menu_Init();
		caption_split_by_code(0x1612, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(1,&cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (INT32U*)pStartDept, NULL, NULL, NULL);

		caption_split_by_code(2, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (INT32U*)pStartNo, NULL, NULL, NULL);

		caption_get_name(0x1ab2, cap.form);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (INT32U*)pEndNo, NULL, NULL, NULL);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE)
		{
			if(*pStartNo > *pEndNo)
			{
				BuzOn(2);
			}
			else
			{
				return KP_ENTER;
			}
		}
		else if(result == MENU_RET_ESC)
		{
			return KP_ESC;
		}
	}
}
#else
INT16U Report_PluInputRange(long *pStartDept, long *pEndDept, long *pStartNo, long *pEndNo)
{
	char string_buf[40],format[40];
	POINT disp_p;
	CAPTION_STRUCT cap;
	INT16U result;
	INT8U idx, input_x, input_y, dsp_y, len;
	INT8U digit0, digit1;

	digit0=2;
	//digit1=5;
	digit1=6;   //SG070316

	if(Startup.disable_dept) {
		idx = 1;
		*pStartDept = 1;
	} else {
		idx = 0;
	}

	*pEndDept = *pStartDept;

	while(1) {
		memset((INT8U *)string_buf, ' ', 32);
		string_buf[32] = 0;
		disp_p = point(Startup.menu_type*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
		disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif

		dsp_y = Startup.menu_type;
		if(!Startup.disable_dept) {
			caption_split_by_code( 1 , &cap,0);
			caption_adjust(&cap,"%2ld");
			strcpy(format, (char*)cap.form);
			if(idx == 0) {
				input_x = cap.input_x;
				input_y = dsp_y;
			}

			sprintf(string_buf, format, *pStartDept);

			disp_p = point(dsp_y*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
			hebrew_codetemp[strlen(string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
			arab_codetemp[strlen(string_buf)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
			dsp_y++;
		}
#ifdef USE_ARAB_CONVERT
        caption_split_by_code(0x1ab5, &cap,0);
#else
		caption_split_by_code( 2 , &cap,0);
#endif
		caption_adjust(&cap,"%6ld");
		strcpy(format, (char*)cap.form);
		if(idx == 1) {
			input_x = cap.input_x;
			input_y = dsp_y;
		}
		len = cap.input_x + 6 + 1;

		caption_split_by_code(0x1ab2, &cap,0);
		caption_adjust(&cap,"%6ld");
		strcat(format, (char*)cap.form);
		if(idx == 2) {
			input_x = cap.input_x + len;
			input_y = dsp_y;
		}

		sprintf(string_buf, format, *pStartNo, *pEndNo);

		disp_p = point(dsp_y*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
		Dsp_Diffuse();

		switch(idx)
		{
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
			case 0: result = keyin_ulong(KI_GRAPHIC, 
							 KI_FORM_ILNN, 
							 0, 
							 pStartDept, 
							 digit0, 
							 input_y*Startup.menu_ygap, 
							 input_x*Startup.menu_xgap, 
							 0, 
							 KI_NOPASS);
				break;
			case 1: result = keyin_ulong(KI_GRAPHIC, 
							 KI_FORM_ILNN, 
							 0, 
							 pStartNo, 
							 digit1, 
							 input_y*Startup.menu_ygap,
							 input_x*Startup.menu_xgap, 
							 0, 
							 KI_NOPASS);
				break;
			case 2: result = keyin_ulong(KI_GRAPHIC, 
							 KI_FORM_ILNN, 
							 0, 
							 pEndNo, 
							 digit1, 
							 input_y*Startup.menu_ygap,
							 input_x*Startup.menu_xgap, 
							 0, 
							 KI_NOPASS);
				break;
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			case 0:	result = keyin_ulong(KI_GRAPHIC,
						     KI_FORM_NORMAL, 
						     0, 
						     pStartDept, 
						     digit0, 
						     input_y*Startup.menu_ygap, 
						     input_x*Startup.menu_xgap, 
						     0, 
						     KI_NOPASS);
				break;
			case 1: result = keyin_ulong(KI_GRAPHIC,
						     KI_FORM_NORMAL, 
						     0, 
						     pStartNo, 
						     digit1, 
						     input_y*Startup.menu_ygap,
						     input_x*Startup.menu_xgap, 
						     0, 
						     KI_NOPASS);
				break;
			case 2:	result = keyin_ulong(KI_GRAPHIC,
						     KI_FORM_NORMAL, 
						     0, 
						     pEndNo, 
						     digit1, 
						     input_y*Startup.menu_ygap,
						     input_x*Startup.menu_xgap, 
						     0, 
						     KI_NOPASS);
				break;
#else
			case 0:	result = keyin_ulong(KI_DEFAULT,
						     KI_FORM_NORMAL, 
						     0, 
						     pStartDept, 
						     digit0, 
						     input_y*Startup.menu_ygap, 
						     input_x*Startup.menu_xgap, 
						     0, 
						     KI_NOPASS);
				break;
			case 1: result = keyin_ulong(KI_DEFAULT,
						     KI_FORM_NORMAL, 
						     0, 
						     pStartNo, 
						     digit1, 
						     input_y*Startup.menu_ygap,
						     input_x*Startup.menu_xgap, 
						     0, 
						     KI_NOPASS);
				break;
			case 2:	result = keyin_ulong(KI_DEFAULT,
						     KI_FORM_NORMAL, 
						     0, 
						     pEndNo, 
						     digit1, 
						     input_y*Startup.menu_ygap,
						     input_x*Startup.menu_xgap, 
						     0, 
						     KI_NOPASS);
				break;
#endif
#endif
		}
		switch(result) {
			case KP_ARROW_UP :
				if(idx < 1+Startup.disable_dept) {
					BuzOn(2);
				} else {
					BuzOn(1);
					idx--;
				}
				break;
			case KP_ARROW_DN :
			case KP_ENTER :
				BuzOn(1);
				if(idx >= 2) {
					if(*pStartNo > *pEndNo) {
						BuzOn(2);
						break;;
					}
					return KP_ENTER;
				}
				else idx++;
				break;
			case KP_SAVE :
				if(*pStartNo > *pEndNo) {
					BuzOn(2);
					break;;
				}
				BuzOn(1);
				return result;
			case KP_ESC :
				BuzOn(1);
				return result;
			case KP_ARROW_LF :
			case KP_ARROW_RI :
				BuzOn(2);
				break;
			default :
				BuzOn(2);
				break;
		}
	}
}
#endif //#if defined(USE_SINGLE_LINE_GRAPHIC)


#if defined(USE_SINGLE_LINE_GRAPHIC)
INT16U Report_MiscPluInputRange(long *pStartDept, long *pEndDept, long *pStartNo, long *pEndNo)
{
	char string_buf[40], format[40];
	CAPTION_STRUCT cap;
	INT16U result;

	*pEndDept = *pStartDept = 1;

	while(1)
	{
		Menu_Init();
		caption_split_by_code(0x1613, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x1ab3,&cap,0);
		cap.input_length = 1;
		cap.input_min = 0;
		cap.input_max = 2;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (INT32U*)pStartNo, NULL, NULL, NULL);

		caption_split_by_code(0x1ab2, &cap,0);
		cap.input_length = 1;
		cap.input_min = 0;
		cap.input_max = 2;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (INT32U*)pEndNo, NULL, NULL, NULL);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE)
		{
			if(*pStartNo > *pEndNo)
			{
				BuzOn(2);
			}
			else
			{
				return KP_ENTER;
			}
		}
		else if(result == MENU_RET_ESC)
		{
			return KP_ESC;
		}
	}
}
#else
INT16U Report_MiscPluInputRange(long *pStartDept, long *pEndDept, long *pStartNo, long *pEndNo)
{
	char string_buf[40], format[40];
	POINT disp_p;
	CAPTION_STRUCT cap;
	INT16U result;
	INT8U idx, ygap, input_x, len;
	INT8U digit1;

	idx = 0;
	digit1=1;

	*pEndDept = *pStartDept = 1;
//	ygap = Startup.menu_ygap;
//	if (Startup.menu_type == 0) ygap = 0;
	ygap = (Startup.menu_type) * Startup.menu_ygap;	  //se-hyung 20071004

	while(1) {
		memset((INT8U *)string_buf, ' ', 32);
		string_buf[32] = 0;
		disp_p = point(Startup.menu_type*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
		disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif

		caption_split_by_code(0x1ab3, &cap,0);
		caption_adjust(&cap,"%1ld");
		strcpy(format, (char*)cap.form);
		if(idx == 0) input_x = cap.input_x;
		len = cap.input_x + 1 + 1;

		caption_split_by_code(0x1ab2, &cap,0);
		caption_adjust(&cap,"%1ld");
		strcat(format, (char*)cap.form);
		if(idx == 1) input_x = cap.input_x + len;


		sprintf(string_buf, (char*)format, *pStartNo, *pEndNo);
		disp_p = point(ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
		Dsp_Diffuse();

		switch(idx) {
			case 0: 
				while(result != KP_ESC) {
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
					result = keyin_ulong(KI_GRAPHIC, 
								 KI_FORM_ILNN, 
								 0, 
								 pStartNo, 
								 digit1, 
								 ygap, 
								 input_x*Startup.menu_xgap, 
								 0, 
								 KI_NOPASS);
#else
					result = keyin_ulong(KI_GRAPHIC, 
							     KI_FORM_NORMAL, 
							     0, 
							     pStartNo, 
							     digit1, 
							     ygap, 
							     input_x*Startup.menu_xgap, 
							     0, 
							     KI_NOPASS);
#endif
					if(*pStartNo <= 2 && *pStartNo >= 0) break;
					else BuzOn(2);
				}
				break;
			case 1:
				while(result != KP_ESC) {
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
					result = keyin_ulong(KI_GRAPHIC, 
								 KI_FORM_ILNN, 
								 0, 
								 pEndNo, 
								 digit1, 
								 ygap, 
								 input_x*Startup.menu_xgap, 
								 0, 
								 KI_NOPASS);
#else
					result = keyin_ulong(KI_GRAPHIC, 
							     KI_FORM_NORMAL, 
							     0, 
							     pEndNo, 
							     digit1, 
							     ygap, 
							     input_x*Startup.menu_xgap, 
							     0, 
							     KI_NOPASS);
#endif
					if(*pEndNo <= 2 && *pEndNo >= 0) break;
					else BuzOn(2);
				}
				break;
		}
		switch(result) {
			case KP_ARROW_UP :
				if(idx < 1) {
					BuzOn(2);
				} else {
					BuzOn(1);
					idx--;
				}
				break;
			case KP_ARROW_DN :
			case KP_ENTER :
				BuzOn(1);
				if(idx >= 1) {
					if(*pStartNo > *pEndNo) {
						BuzOn(2);
						break;;
					}
					return KP_ENTER;
				}
				else idx++;
				break;
			case KP_SAVE :
				if(*pStartNo > *pEndNo) {
					BuzOn(2);
					break;;
				}
				BuzOn(1);
				return result;
			case KP_ESC :
				BuzOn(1);
				return result;
			default :
				BuzOn(2);
				break;
		}
	}
}
#endif

#if defined(USE_SINGLE_LINE_GRAPHIC)
INT16U Report_InputRange(INT8U rep_mode, INT16U *pStartNo, INT16U *pEndNo, INT8U digit, INT16U max, INT16U min)
{
	char string_buf[40], format[40];
	CAPTION_STRUCT cap;
	INT16U result;
	long startno, endno;

	startno = (long)*pStartNo;
	endno   = (long)*pEndNo;

	while(1)
	{
		Menu_Init();
		//caption_split_by_code(0x1612, &cap,0);
		//Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x1ab5,&cap,0);
		cap.input_length = digit;
		cap.input_min = min;
		cap.input_max = max;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (INT32U*)&startno, NULL, NULL, NULL);

		caption_split_by_code(0x1ab2, &cap,0);
		cap.input_length = digit;
		cap.input_min = min;
		cap.input_max = max;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (INT32U*)&endno, NULL, NULL, NULL);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE)
		{
			if(startno > endno)
			{
				BuzOn(2);
			}
			else
			{
				*pStartNo = (INT16U)startno;
				*pEndNo   = (INT16U)endno;
				return KP_ENTER;
			}
		}
		else if(result == MENU_RET_ESC)
		{
			return KP_ESC;
		}
	}
}
#else
INT16U Report_InputRange(INT8U rep_mode, INT16U *pStartNo, INT16U *pEndNo, INT8U digit, INT16U max, INT16U min)
{
	char string_buf[40], format[40];
	POINT disp_p;
	CAPTION_STRUCT cap;
	INT16U result;
	INT8U idx, ygap, input_x, len;
	char num_form[10];
	long startno, endno;

	startno = (long)*pStartNo;
	endno   = (long)*pEndNo;

	idx = 0;
//	ygap = Startup.menu_ygap;
	ygap = (Startup.menu_type) * Startup.menu_ygap;	  //se-hyung 20071004
//	if (Startup.menu_type == 0) ygap = 0;

	sprintf(num_form, "%%%dd", digit);

	while(1) {
		memset((INT8U *)string_buf, ' ', 32);
		string_buf[32] = 0;
		disp_p = point(Startup.menu_type*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
		disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
		caption_split_by_code(0x1ab5, &cap,0);
		caption_adjust(&cap,num_form);
		strcpy(format, (char*)cap.form);
		if(idx == 0) input_x = cap.input_x;
		len = cap.input_x + digit + 1;
		caption_split_by_code(0x1ab2, &cap,0);
		caption_adjust(&cap,num_form);
		strcat(format, (char*)cap.form);
		if(idx == 1) input_x = cap.input_x + len;

		sprintf(string_buf, (char*)format, (INT16U)startno, (INT16U)endno);
		disp_p = point(ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
		arab_codetemp[strlen(string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif

		if(rep_mode == REPORT_SCALE) {
//			if (ethernet_list.status != 2) {
			if (ethernet_list.status == 1) {
				caption_split_by_code(0x1ab6, &cap, 0);
				disp_p = point(Startup.menu_ygap*(Startup.menu_type?3:1), Startup.menu_xgap);
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
				hebrew_codetemp[strlen(cap.form)] = 0;
				disp_p.x = 202 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
				arab_codetemp[strlen(cap.form)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
				Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
			}

		}

		Dsp_Diffuse();
	
		switch(idx) {
			case 0: 
				while(result != KP_ESC) {
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
					result = keyin_ulong(KI_GRAPHIC, 
								 KI_FORM_ILNN, 
								 0, 
								 &startno, 
								 digit, 
								 ygap, 
								 input_x*Startup.menu_xgap, 
								 0, 
								 KI_NOPASS);
#else
					result = keyin_ulong(KI_GRAPHIC, 
							     KI_FORM_NORMAL, 
							     0, 
							     &startno, 
							     digit, 
							     ygap, 
							     input_x*Startup.menu_xgap, 
							     0, 
							     KI_NOPASS);
#endif
					if((INT16U)startno <= max && (INT16U)startno >= min) break;
					else BuzOn(2);
				}
				break;
			case 1:
				while(result != KP_ESC) {
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
					result = keyin_ulong(KI_GRAPHIC, 
								 KI_FORM_ILNN, 
								 0, 
								 &endno, 
								 digit, 
								 ygap, 
								 input_x*Startup.menu_xgap, 
								 0, 
								 KI_NOPASS);
#else
					result = keyin_ulong(KI_GRAPHIC, 
							     KI_FORM_NORMAL, 
							     0, 
							     &endno, 
							     digit, 
							     ygap, 
							     input_x*Startup.menu_xgap, 
							     0, 
							     KI_NOPASS);
#endif
					// Added by CJK 20041220
					if(rep_mode == REPORT_SCALE && startno == 0) {
						if(endno == 0) break;
						else BuzOn(2);
					} else if((INT16U)endno <= max && (INT16U)endno >= min) break;
					else BuzOn(2);
				}
				break;
		}
		switch(result) {
			case KP_ARROW_UP :
				if(idx < 1) {
					BuzOn(2);
				} else {
					BuzOn(1);
					idx--;
				}
				break;
			case KP_ARROW_DN :
			case KP_ENTER :
				BuzOn(1);
				if(idx >= 1) {
					if(startno > endno) {
						BuzOn(2);
						break;
					}
					*pStartNo = (INT16U)startno;
					*pEndNo   = (INT16U)endno;
					return KP_ENTER;
				} else {
					// Added by CJK 20041220
					if(rep_mode == REPORT_SCALE && startno == 0 && idx == 0) {
						endno = 0;
					} else {
					}
					///////////////////////
					idx++;
				}
				break;
			case KP_SAVE :
				if(startno > endno) {
					BuzOn(2);
					break;
				}
				BuzOn(1);
				*pStartNo = (INT16U)startno;
				*pEndNo   = (INT16U)endno;
				return result;
			case KP_ESC :
				BuzOn(1);
				return result;
			default :
				BuzOn(2);
				break;
		}
	}
}
#endif

extern INT8U OnOff(INT8U tmp);

INT16U ReportRangeMin[7] = {	0, 		   	// SCALE
				1, 		   	// PLU
				1, 		   	// Misc PLU
				0, 		   	// Group
				1, 		   	// Dept
				0, 		   	// Hourly
				0		   	// Clerk
};
INT16U ReportRangeMax[7] = {	MAX_SCALE-1, 	
				1, 
				1, 
				99, 
				99, 
				23, 
				MAX_CLERK_NO-1
};
INT16U ReportCaption[7] = {	0x9c04, 	
				0x9c00, 
				0x9c01, 
				0x9c02, 
				0x9c03, 
				0x9c05, 
				0x9c06
};

void Report_Menu(INT8U mode, INT16S period, INT8U rpt_mode)
{
	INT16U start_no, end_no;
	INT16U result;
	INT16U min, max;
	POINT disp_p;
	CAPTION_STRUCT cap;
	char string_buf[50];
#ifdef USE_PERSIA_CAPTION
    char temp_buf[20];
#endif
	INT8U backup_direct;
	INT8U ClearReport, PrintReport;
	RTC_STRUCT time_str;
	INT8U prt_result, backup_unit;
	INT32U nv_addr;
	BASIC_REPORT_TYPE total_br;
#ifdef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	INT8U backup_PrtPaperType;
#endif

	min = ReportRangeMin[rpt_mode-1];
	max = ReportRangeMax[rpt_mode-1];
	start_no = min;
	end_no 	 = max;

	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
	menu_display_menucode(CurMenuCode, 1);
#if defined(USE_SINGLE_LINE_GRAPHIC)
	menu_displaytitle((CurMenuCode&0xff0f)|0x0010, 1);
#else
	DspLoadFont1(DSP_MENU_FONT_ID);
  #ifdef USE_PERSIA_CAPTION
    caption_get_name(ReportCaption[rpt_mode-1], temp_buf);
    sprintf(string_buf,"  %s %c%d %s ", Caption_Report, ReportTypeChar[mode], period+1, &temp_buf);
  #else
	sprintf(string_buf, "   %c%d ", ReportTypeChar[mode], period+1);
	caption_get_name(ReportCaption[rpt_mode-1], string_buf+6);
  #endif  
	menu_writetitle_str(string_buf);
#endif
	menu_writepage(1, 1);
	Dsp_Diffuse();

//Added by JJANG 20080529, Parameter 595 : Y로 세팅했을 때 GROUP,CLERK REPORT가 0으로 표시되는 현상 수정
	if(mode == REPORT_MODE_Z && PrtOper.IndividualClear) 
	{
		if (rpt_mode == REPORT_GROUP || rpt_mode == REPORT_CLERK) end_no = max;
		result = KP_ENTER;
	} 
	else result = Report_InputRange(rpt_mode, &start_no, &end_no, 2, max, min);

	switch(result) {
		case KP_SAVE:
		case KP_ENTER :
			switch(rpt_mode) {
				case REPORT_GROUP :  nv_addr = NVRAM_GROUP_SUMMARY;
					break;
				case REPORT_DEPT :   nv_addr = NVRAM_DEPT_SUMMARY;
					break;
				case REPORT_HOURLY : nv_addr = NVRAM_HOURLY_SUMMARY;
					break;
				case REPORT_CLERK :  nv_addr = NVRAM_CLERK_SUMMARY;
					break;
			}
			DisplaySalesSummary(rpt_mode, period, start_no, end_no, nv_addr, &total_br);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
			if(Startup.menu_type) {
				//disp_p = point(3*Startup.menu_ygap, 0);
				disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, 0); //se-hyung 20071004  
				memset((INT8U *)string_buf, ' ', 32);
				string_buf[32] = 0;
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
				hebrew_codetemp[strlen(string_buf)] = 0;
				disp_p.x = 202 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
				arab_codetemp[strlen(string_buf)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
				Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
				if(mode == REPORT_MODE_Z) {
					if(PrtOper.IndividualClear) {
						if(PrtOper.EnPrtZReport) {
							caption_split_by_code(0x1aab, &cap,0);	//"TEST = print & clear"
						} else {
							caption_split_by_code(0x1aa9, &cap,0);	//"TEST = clear data"
						}
					}
				} else {
					caption_split_by_code(0x1aa8, &cap,0);			//"TEST = print"
				}
				//disp_p = point(3*Startup.menu_ygap, Startup.menu_xgap);
				disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, Startup.menu_xgap); //se-hyung 20071004  
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
				hebrew_codetemp[strlen(cap.form)] = 0;
				disp_p.x = 202 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
				arab_codetemp[strlen(cap.form)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
				Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
			}
#endif
			Dsp_Diffuse();
			while(1) {
				if(KEY_Read()) {
					switch(KeyDrv.CnvCode) {
						case KP_ESC :
							BuzOn(1);
							return;
						case KP_ENTER:
						case KP_TEST :
							BuzOn(1);
							Report_GetCurTime(&time_str,0);
							if(mode == REPORT_MODE_Z) {
								if(PrtOper.IndividualClear) {
									if(!Report_ConfirmReset(OFF)) continue;
									if(PrtOper.EnPrtZReport) {
										//if(!Report_ConfirmReset(ON)) continue;
										PrintReport = 1;
									} else {
										//if(!Report_ConfirmReset(OFF)) continue;
										PrintReport = 0;
									}
									ClearReport = 1;
								} else continue;
							} else {
								//if(!Report_ConfirmPrint()) {
								//	continue;
								//}
								ClearReport = 0;
								PrintReport = 1;
							}
							prt_result = ON;
							if(PrintReport) {
								backup_direct = DspStruct.DirectDraw;
								DspStruct.DirectDraw = 1;
								//disp_p = point(3*Startup.menu_ygap, Startup.menu_xgap);							
#if !defined(USE_SINGLE_LINE_GRAPHIC)
								if(Startup.menu_type) {
									disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, Startup.menu_xgap); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
									memset((INT8U *)string_buf, ' ', 32);
									string_buf[32] = 0;
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
									hebrew_codetemp[strlen(string_buf)] = 0;
									disp_p.x = 202 - disp_p.x;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
									arab_codetemp[strlen(string_buf)] = 0;
									disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
									caption_split_by_code(0x1aa5, &cap,0);	//"TEST = print"
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
									hebrew_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
									arab_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
								}
#endif
								backup_unit = CurPluWeightUnit;
								CurPluWeightUnit = ADM_Status.UseUnit;
#ifdef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
								// Save PrtPapertype
								backup_PrtPaperType = PrtPaperType;
								PrtPaperType = 1; // Ticket Report Print
#endif
								if (PrtPaperType==0) {
									prt_result = Report_PrintLabel(rpt_mode,mode,period,0,0,start_no,end_no);
								} else {
									switch(rpt_mode) {
										case REPORT_GROUP :
											if (GlbPrint.reportType == 2)
											{
												//Modified by JJANG 20080529, Parameter 721 : 2 
												prt_result = Report_PrintTicketGroupSalesSummary(mode, period, start_no, end_no, &time_str);
											}
											else prt_result = Report_PrintGroupSalesSummary(mode, period, start_no, end_no, &time_str, &total_br);
											break;

										case REPORT_DEPT :
											prt_result = Report_PrintDeptSalesSummary(mode, period, start_no, end_no, &time_str, &total_br);
											break;
										case REPORT_HOURLY :
											prt_result = Report_PrintHourlySalesSummary(mode, period, start_no, end_no, &time_str, &total_br);
											break;
										case REPORT_CLERK :
											prt_result = Report_PrintClerkSalesSummary(mode, period, start_no, end_no, &time_str, &total_br);
											break;
									}
								}
#ifdef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
								// Restore PrtPapertype
								PrtPaperType = backup_PrtPaperType;
#endif
								CurPluWeightUnit = backup_unit;
#if !defined(USE_SINGLE_LINE_GRAPHIC)
								if(Startup.menu_type) {
									memset((INT8U *)string_buf, ' ', 32);
									string_buf[32] = 0;
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
									hebrew_codetemp[strlen(string_buf)] = 0;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
									arab_codetemp[strlen(string_buf)] = 0;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
									if(mode == REPORT_MODE_Z && PrtOper.IndividualClear) {
										if(PrtOper.EnPrtZReport) {
											caption_split_by_code(0x1aab, &cap,0);	//"TEST = print & clear"
										} else {
											caption_split_by_code(0x1aa9, &cap,0);	//"TEST = clear data"
										}
									} else {
										caption_split_by_code(0x1aa8, &cap,0);	//"TEST = print"
									}
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
									hebrew_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
									arab_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
								}
#endif
								DspStruct.DirectDraw = backup_direct;
							}
							if(ClearReport && prt_result) {
#if defined(_USE_LCD20848_)
								Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
								DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
								DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
#endif
#if defined(USE_SINGLE_LINE_GRAPHIC) || defined(_USE_LCD20848_)
								caption_split_by_code(0x1AA5, &cap,0);
								disp_p = point(0, 0);
  #ifdef USE_ARAB_CONVERT
                                Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
                                arab_codetemp[strlen(cap.form)] = 0;
                                disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
                                Dsp_Write_1stringLong(disp_p, arab_codetemp);
  #else
								Dsp_Write_String(disp_p, cap.form);
  #endif
								Dsp_Diffuse();
#endif
								switch(rpt_mode) {
									case REPORT_PLU :
										break;
									case REPORT_MISCPLU :
										break;
									case REPORT_GROUP :
										Report_ClearGroupReport(period, start_no, end_no, &time_str);
										break;
									case REPORT_DEPT :
										Report_ClearDeptReport(period, start_no, end_no, &time_str);
										break;
									case REPORT_HOURLY :
										Report_ClearHourlyReport(period, start_no, end_no, &time_str);
										break;
									case REPORT_CLERK :
										Report_ClearClerkReport(period, start_no, end_no, &time_str);
										break;
								}
								return;
							}
							break;
						case KP_FEED :
							BuzOn(1);
							keyapp_feed(OFF);
							break;
						case KP_ZERO:
							BuzOn(1);
							Report_SaveUsbMem(rpt_mode,mode,period,0,0,start_no,end_no);
							break;
						default :
							BuzOn(2);
							break;
					}
				}
			}
			//break;
		case KP_ESC :
			return;
		case KP_FEED :
			BuzOn(1);
			keyapp_feed(OFF);
			break;
		default : 
			BuzOn(2);
			break;
	}
}


void Report_PLUs(INT8U mode, INT16S period, INT8U rpt_mode)
{
	long start_no, end_no, start_dept, end_dept;
	INT16U result;
	POINT disp_p;
	CAPTION_STRUCT cap;
	char string_buf[50];
#ifdef USE_PERSIA_CAPTION
    char temp_buf[20];
#endif
	INT8U backup_direct;
	INT8U ClearReport, PrintReport;
	RTC_STRUCT time_str;
	INT8U prt_result, backup_unit;
	BASIC_REPORT_TYPE total_br;
#ifdef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	INT8U backup_PrtPaperType;
#endif

	if (GlbOper.PluReportType) return;

	start_dept = 1;
	end_dept   = 1;
	switch(rpt_mode) 
	{
		case REPORT_PLU :
			start_no   = 1;
			end_no     = MAX_PLU_NUMBER;  //SG070315
			break;
		case REPORT_MISCPLU :
			start_no   = 0;
			end_no     = 2;
			break;
	}

	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
	menu_display_menucode(CurMenuCode, 1);
#if defined(USE_SINGLE_LINE_GRAPHIC)
	menu_displaytitle((CurMenuCode&0xff0f)|0x0010, 1);
#else
	DspLoadFont1(DSP_MENU_FONT_ID);
  #ifdef USE_PERSIA_CAPTION
    caption_get_name(ReportCaption[rpt_mode-1], temp_buf);
    sprintf(string_buf,"  %s %c%d %s ", Caption_Report, ReportTypeChar[mode], period+1, &temp_buf);
  #else
    sprintf(string_buf, "   %c%d ", ReportTypeChar[mode], period+1);
	caption_get_name(ReportCaption[rpt_mode-1], string_buf+6);
  #endif
	menu_writetitle_str(string_buf);
#endif
	menu_writepage(1, 1);
	Dsp_Diffuse();

	if(mode == REPORT_MODE_Z && PrtOper.IndividualClear) 
	{
		result = KP_ENTER;
	} 
	else 
	{
		switch(rpt_mode) 
		{
			case REPORT_PLU :
				result = Report_PluInputRange(&start_dept, &end_dept, &start_no, &end_no);
				end_dept = start_dept;
				break;
			case REPORT_MISCPLU :
				result = Report_MiscPluInputRange(&start_dept, &end_dept, &start_no, &end_no);
				break;
		}
	}

	switch(result) 
	{
		case KP_SAVE:
		case KP_ENTER :
			switch(rpt_mode) 
			{
				case REPORT_PLU :
					DisplayPluSalesSummary(period, (INT16U)start_dept, (INT16U)end_dept, start_no, end_no, 0, &total_br);
					break;
				case REPORT_MISCPLU :
					DisplayPluSalesSummary(period, (INT16U)start_dept, (INT16U)end_dept, start_no+MISC_PLU_WEIGHT, end_no+MISC_PLU_WEIGHT, 0, &total_br);
					break;
			}
#if !defined(USE_SINGLE_LINE_GRAPHIC)
			if(Startup.menu_type) 
			{
				//disp_p = point(3*Startup.menu_ygap, 0);
				disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, 0); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
				memset((INT8U *)string_buf, ' ', 32);
				string_buf[32] = 0;
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
				hebrew_codetemp[strlen(string_buf)] = 0;
				disp_p.x = 202 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
				arab_codetemp[strlen(string_buf)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
				Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
				if(mode == REPORT_MODE_Z) 
				{
					if(PrtOper.IndividualClear) 
					{
						if(PrtOper.EnPrtZReport) 
						{
							caption_split_by_code(0x1aab, &cap,0);	//"TEST = print & clear"
						} 
						else {
							caption_split_by_code(0x1aa9, &cap,0);	//"TEST = clear data"
						}
					}
				} 
				else 
				{
					caption_split_by_code(0x1aa8, &cap,0);			//"TEST = print"
				}
				//disp_p = point(3*Startup.menu_ygap, Startup.menu_xgap);
				disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, Startup.menu_xgap); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
				hebrew_codetemp[strlen(cap.form)] = 0;
				disp_p.x = 202 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
				arab_codetemp[strlen(cap.form)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
				Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
			}
#endif
			Dsp_Diffuse();
			while(1) 
			{
				if(KEY_Read()) 
				{
					switch(KeyDrv.CnvCode) 
					{
						case KP_ESC :
							BuzOn(1);
							return;
						case KP_ENTER:
						case KP_TEST :
							BuzOn(1);
							Report_GetCurTime(&time_str,0);
							if(mode == REPORT_MODE_Z) 
							{
								if(!Report_ConfirmReset(OFF)) continue;
								if(PrtOper.IndividualClear) 
								{
									if(PrtOper.EnPrtZReport) 
									{
										//if(!Report_ConfirmReset(ON)) continue;
										PrintReport = 1;
									} 
									else 
									{
										//if(!Report_ConfirmReset(OFF)) continue;
										PrintReport = 0;
									}
									ClearReport = 1;
								} 
								else continue;
							} 
							else 
							{
								//if(!Report_ConfirmPrint()) {
								//	continue;
								//}
								ClearReport = 0;
								PrintReport = 1;
							}
							prt_result = ON;
							if(PrintReport) 
							{
								backup_direct = DspStruct.DirectDraw;
								DspStruct.DirectDraw = 1;
							       //disp_p = point(3*Startup.menu_ygap, Startup.menu_xgap);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
								if(Startup.menu_type) 
								{
									disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, Startup.menu_xgap); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
									memset((INT8U *)string_buf, ' ', 32);
									string_buf[32] = 0;
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
									hebrew_codetemp[strlen(string_buf)] = 0;
									disp_p.x = 202 - disp_p.x;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
									arab_codetemp[strlen(string_buf)] = 0;
									disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
									caption_split_by_code(0x1aa5, &cap,0);	//"TEST = print"
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
									hebrew_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
									arab_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
								}
#endif
								backup_unit = CurPluWeightUnit;
								CurPluWeightUnit = ADM_Status.UseUnit;
#ifdef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
								// Save PrtPapertype
								backup_PrtPaperType = PrtPaperType;
								PrtPaperType = 1; // Ticket Report Print
#endif
								if (PrtPaperType==0) 
								{
									prt_result = Report_PrintLabel(rpt_mode,mode,period,(INT16U)start_dept,(INT16U)end_dept, start_no, end_no);
								} 
								else 
								{
									switch(rpt_mode) 
									{
										case REPORT_PLU :
											prt_result = Report_PrintPluSalesSummary(mode, period, (INT16U)start_dept, (INT16U)end_dept, start_no, end_no, &time_str, &total_br);
											break;
										case REPORT_MISCPLU :
											prt_result = Report_PrintPluSalesSummary(mode, period, (INT16U)start_dept, (INT16U)end_dept, start_no+MISC_PLU_WEIGHT, end_no+MISC_PLU_WEIGHT, &time_str, &total_br);
											break;
									}
								}
#ifdef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
								// Restore PrtPapertype
								PrtPaperType = backup_PrtPaperType;
#endif
								CurPluWeightUnit = backup_unit;
#if !defined(USE_SINGLE_LINE_GRAPHIC)
								if(Startup.menu_type) 
								{
									memset((INT8U *)string_buf, ' ', 32);
									string_buf[32] = 0;
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
									hebrew_codetemp[strlen(string_buf)] = 0;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
									arab_codetemp[strlen(string_buf)] = 0;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
									if(mode == REPORT_MODE_Z && PrtOper.IndividualClear) 
									{
										if(PrtOper.EnPrtZReport) 
										{
											caption_split_by_code(0x1aab, &cap,0);	//"TEST = print & clear"
										} 
										else 
										{
											caption_split_by_code(0x1aa9, &cap,0);	//"TEST = clear data"
										}
									} 
									else 
									{
										caption_split_by_code(0x1aa8, &cap,0);	//"TEST = print"
									}
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
									hebrew_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
									arab_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
								}
#endif
								DspStruct.DirectDraw = backup_direct;
							}
							if(ClearReport && prt_result)	//Modified by JJANG 20070626 MISC-PLU 개별삭제 안되던 문제 해결 
							{
#if defined(_USE_LCD20848_)
								Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
								DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
								DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
#endif
#if defined(USE_SINGLE_LINE_GRAPHIC) || defined(_USE_LCD20848_)
								caption_split_by_code(0x1AA5, &cap,0);
								disp_p = point(0, 0);
  #ifdef USE_ARAB_CONVERT
                                Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
                                arab_codetemp[strlen(cap.form)] = 0;
                                disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
                                Dsp_Write_1stringLong(disp_p, arab_codetemp);
  #else                      
								Dsp_Write_String(disp_p, cap.form);
  #endif
								Dsp_Diffuse();
#endif
								switch(rpt_mode) 
								{
									case REPORT_PLU :
										Report_ClearPLUReport(period, start_dept, end_dept, start_no, end_no, &time_str);
										return;
									case REPORT_MISCPLU :
										Report_ClearPLUReport(period, start_dept, end_dept, start_no+MISC_PLU_WEIGHT, end_no+MISC_PLU_WEIGHT, &time_str);
										return;
								}
							}
							break;
						case KP_FEED :
							BuzOn(1);
							keyapp_feed(OFF);
							break;
						case KP_ZERO:
							BuzOn(1);
							Report_SaveUsbMem(rpt_mode,mode,period,(INT16U)start_dept,(INT16U)end_dept, start_no, end_no);
							break;
						default :
							BuzOn(2);
							break;
					}
				}
			}
			//break;
		case KP_ESC :
			return;
		case KP_FEED :
			BuzOn(1);
			keyapp_feed(OFF);
			break;
		default : 
			BuzOn(2);
			break;
	}
}

//#ifdef USE_TICKETCLOSE_BEFORE_ZREPORT 
//extern void CheckClerkTransaction_Print(void);
//#endif
void Report_Scale(INT8U mode, INT16S period)
{
	INT16U start_no, end_no;
	INT16U result;
	INT16U min, max;
	POINT disp_p;
	CAPTION_STRUCT cap;
	char string_buf[50];
#ifdef USE_PERSIA_CAPTION
    char temp_buf[20];
#endif
//	INT8U flag_r;
	INT8U backup_direct;
//	INT8U IndividualClear, EnPrtZReport;
	INT8U ClearReport, PrintReport;
	RTC_STRUCT time_str;
	INT8U prt_result, backup_unit;
	BASIC_REPORT_TYPE total_br;
#ifdef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
	INT8U backup_PrtPaperType;
#endif
	INT32S count;

//	flag_r = get_global_bparam(GLOBAL_REPORT_FLAG);	// Report Individual Clear
//	IndividualClear = OnOff(flag_r&0x01);
//	EnPrtZReport    = OnOff(flag_r&0x08);
	//min = 1;
	min = 0;
	//max = MAX_SCALE+1;
	max = MAX_SCALE - 1;
	start_no = status_scale.scaleid;
	if (start_no > MAX_SCALE - 1) start_no = MAX_SCALE - 1;	// scale no 32~255는 scale 31로 정산함
	end_no 	 = start_no;
//	if (ethernet_list.status!=2) {	// if Master
	if (ethernet_list.status==1) {	// if Master
		start_no = min;
		end_no = max;
	} 
	
	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
	menu_display_menucode(CurMenuCode, 1);
#if defined(USE_SINGLE_LINE_GRAPHIC)
	menu_displaytitle((CurMenuCode&0xff0f)|0x0010, 1);
#else
	DspLoadFont1(DSP_MENU_FONT_ID);
  #ifdef USE_PERSIA_CAPTION
    caption_get_name(0x1611, temp_buf);
    sprintf(string_buf,"  %s %c%d %s ", Caption_Report, ReportTypeChar[mode], period+1, &temp_buf[3]);
  #else
	sprintf(string_buf, "   %c%d ", ReportTypeChar[mode], period+1);
	caption_get_name(0x9c04, string_buf+6);
  #endif
    menu_writetitle_str(string_buf);
#endif
	menu_writepage(1, 1);
	Dsp_Diffuse();

	if(mode == REPORT_MODE_Z) {
		result = KP_ENTER;
	} else {
//		if (ethernet_list.status != 2) {	// if Master
		if (ethernet_list.status == 1) {	// if Master
			result = Report_InputRange(REPORT_SCALE, &start_no, &end_no, 2, max, min);
		} else {
			result = KP_ENTER;
		}
	}


	switch(result) {
		case KP_SAVE:
		case KP_ENTER :
DISP_SALE_SUMMARY:
			DisplaySalesSummary(REPORT_SCALE, period, start_no, end_no, NVRAM_SCALE_SUMMARY, &total_br);
			if(Startup.menu_type) {
		      //	disp_p = point(3*Startup.menu_ygap, 0);
				disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, 0); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
				memset((INT8U *)string_buf, ' ', 32);
				string_buf[32] = 0;
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
				hebrew_codetemp[strlen(string_buf)] = 0;
				disp_p.x = 202 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
				arab_codetemp[strlen(string_buf)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
				Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
				if(mode == REPORT_MODE_Z) {
					if(PrtOper.EnPrtZReport) {
						caption_split_by_code(0x1aab, &cap,0);	//"TEST = print & clear"
					} else {
						caption_split_by_code(0x1aa9, &cap,0);	//"TEST = clear data"
					}
				} else {
					caption_split_by_code(0x1aa8, &cap,0);			//"TEST = print"
				}
			     //	disp_p = point(3*Startup.menu_ygap, Startup.menu_xgap);
				disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, Startup.menu_xgap); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
				hebrew_codetemp[strlen(cap.form)] = 0;
				disp_p.x = 202 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
				arab_codetemp[strlen(cap.form)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
				Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
			}
			Dsp_Diffuse();
			while(1) {
				if(KEY_Read()) {
					switch(KeyDrv.CnvCode) {
						case KP_ESC :
							BuzOn(1);
							return;
						case KP_ENTER:
						case KP_TEST :
							BuzOn(1);
//#ifdef USE_TICKETCLOSE_BEFORE_ZREPORT 
//							CheckClerkTransaction_Print();
//#endif
							Report_GetCurTime(&time_str,0);
							if(mode == REPORT_MODE_Z) {
								if(!Report_ConfirmReset(OFF)) goto DISP_SALE_SUMMARY;
								if(PrtOper.EnPrtZReport) {
									//if(!Report_ConfirmReset(ON)) continue;
									PrintReport = 1;
								} else {
									//if(!Report_ConfirmReset(OFF)) continue;
									PrintReport = 0;
								}
								ClearReport = 1;
							} else {
								//if(!Report_ConfirmPrint()) continue;
								ClearReport = 0;
								PrintReport = 1;
							}
							prt_result = ON;
							if(PrintReport) {
								backup_direct = DspStruct.DirectDraw;
								DspStruct.DirectDraw = 1;
							 //    	disp_p = point(3*Startup.menu_ygap, Startup.menu_xgap);
								if(Startup.menu_type) {
									disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, Startup.menu_xgap); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
									memset((INT8U *)string_buf, ' ', 32);
									string_buf[32] = 0;
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
									hebrew_codetemp[strlen(string_buf)] = 0;
									disp_p.x = 202 - disp_p.x;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
									arab_codetemp[strlen(string_buf)] = 0;
									disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
									caption_split_by_code(0x1aa5, &cap,0);	//"TEST = print"
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
									hebrew_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
									arab_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
								}
								backup_unit = CurPluWeightUnit;
								CurPluWeightUnit = ADM_Status.UseUnit;
#ifdef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
								// Save PrtPapertype
								backup_PrtPaperType = PrtPaperType;
								PrtPaperType = 1; // Ticket Report Print
#endif
								if (PrtPaperType==0) {
									prt_result = Report_PrintLabel(REPORT_SCALE,mode,period,0,0,start_no,end_no);
								} else {
									prt_result = Report_PrintScaleSalesSummary(mode, period, start_no, end_no, &time_str, &total_br);
								}
#ifdef USE_SALE_PRINT_LABLE_REPORT_PRINT_TICKET
								// Restore PrtPapertype
								PrtPaperType = backup_PrtPaperType;
#endif
								CurPluWeightUnit = backup_unit;
								if(Startup.menu_type) {
									memset((INT8U *)string_buf, ' ', 32);
									string_buf[32] = 0;
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
									hebrew_codetemp[strlen(string_buf)] = 0;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
									arab_codetemp[strlen(string_buf)] = 0;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
#endif
									if(mode == REPORT_MODE_Z && PrtOper.IndividualClear) {
										if(PrtOper.EnPrtZReport) {
											caption_split_by_code(0x1aab, &cap,0);	//"TEST = print & clear"
										} else {
											caption_split_by_code(0x1aa9, &cap,0);	//"TEST = clear data"
										}
									} else {
										caption_split_by_code(0x1aa8, &cap,0);	//"TEST = print"
									}
#ifdef HEBREW_FONT
									Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
									hebrew_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
									Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
									arab_codetemp[strlen(cap.form)] = 0;
									Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
									Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
								}
								DspStruct.DirectDraw = backup_direct;
							}
							if(ClearReport && prt_result) {
//								if (ethernet_list.status==1) {
//									command_set(0x40,0x10,(unsigned char)period,0,0,0,NULL);
//								}
#if defined(_USE_LCD20848_)
								Dsp_Fill_Buffer(0);
#ifdef USE_CHN_FONT
								DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
								DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
#endif
#if defined(USE_SINGLE_LINE_GRAPHIC) || defined(_USE_LCD20848_)
								caption_split_by_code(0x1AA5, &cap,0);
								disp_p = point(0, 0);
#ifdef USE_ARAB_CONVERT
                                Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
                                arab_codetemp[strlen(cap.form)] = 0;
                                disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
                                Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
								Dsp_Write_String(disp_p, cap.form);
#endif
								Dsp_Diffuse();
#endif
								if(PrtOper.IndividualClear) 
								{
									Report_ClearScaleReport(period, start_no, end_no, &time_str);
								} 
								else 
								{
									Report_PeriodResetTime(period, &time_str);
									ClearSalesSummaries(period);
									if (GlbOper.ClearOpenedClerkTransaction)
									{
										ClerkClearTransaction();
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION) 
										ClearAdditionalData();
#endif
									}
									//Added by JJANG 20080529	   
									if (GlbPrint.reportType == 2)
									{
										count = get_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*period);
										count++;
										if (count > 999999L) count = 1L;
										set_nvram_lparam(NVRAM_COUNTER_GROUP_Z+4*period, count);
									}
								}
								return;
							}
							break;
						case KP_FEED :
							BuzOn(1);
							keyapp_feed(OFF);
							break;
						case KP_ZERO:
							BuzOn(1);
							Report_SaveUsbMem(REPORT_SCALE,mode,period,0,0,start_no,end_no);
							break;
						default :
							BuzOn(2);
							break;
					}
				}
			}
			//break;
		case KP_ESC :
			return;
		case KP_FEED :
			BuzOn(1);
			keyapp_feed(OFF);
			break;
		default : 
			BuzOn(2);
			break;
	}
}


void Report_ClearAll(void)
{
	POINT disp_p;
	CAPTION_STRUCT cap;
	INT8U result;
//	char string_buf[50];

	if(!permission_check(PMS_Z_REPORT,0)) return;

	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	VFD7_Fill_Buffer(0);
#endif
	menu_display_menucode(CurMenuCode, 1);
	DspLoadFont1(DSP_MENU_FONT_ID);	
	caption_split_by_code(0x1650, &cap,0);
	menu_writetitle_str((char *)cap.form);
	menu_writepage(1, 1);
	
//	caption_split_by_code(0x9500, &cap,0);
//	display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);
//	
//	caption_split_by_code(0xf034, &cap,0); // "(1=Yes/0=No)"
//	disp_p = point(0, 0);
//	Dsp_Write_String(disp_p, cap.form);
	Dsp_Diffuse();
	result = Menu_ConfirmDisplay(0);
	if (result)
	{
		BuzOn(1);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		Dsp_Fill_Buffer(0);
#endif
#if defined(_USE_LCD20848_)
  #ifdef USE_CHN_FONT
		DspLoadFont1(DSP_MEDIUM_FONT_ID);
  #else
		DspLoadFont1(DSP_LARGE_FONT_ID);
  #endif
#endif
#if defined(USE_SINGLE_LINE_GRAPHIC) || defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1AA5, &cap,0);
		disp_p = point(0, 0);
  #ifdef USE_ARAB_CONVERT
        Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
        arab_codetemp[strlen(cap.form)] = 0;
        disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
        Dsp_Write_1stringLong(disp_p, arab_codetemp);
  #else
		Dsp_Write_String(disp_p, cap.form);
  #endif
		Dsp_Diffuse();
#endif
				Report_InitResetTime();
				ClerkClearTransaction();
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION) 
				ClearAdditionalData();
#endif
				//Modified by JJANG 20080121
				set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,0);
				set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,0);
				set_nvram_wparam(NVRAM_TOTAL_SEND_SALE,0);
				set_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE,0);

				set_nvram_bparam(NVRAM_TRANS_PAY_HEAD,0);
				set_nvram_bparam(NVRAM_TRANS_PAY_TAIL,0);

				set_nvram_bparam(NVRAM_SLAVECHECK_FLAG,0);	//Added by JJANG 20080130
				set_nvram_bparam(NVRAM_TRANSACTION_CLERK,0);
				set_nvram_lparam(NVRAM_BUSINESS_DATE_FLAG,0);	//NVRAM_BUSINESS_DATE_FLAG 1 byte + NVRAM_BUSINESS_DATE 3 byte

				set_nvram_wparam(NVRAM_REOPEN_HEAD,0);

				ClearSalesSummaries(99);
				ClearClerkTareList();

				commun_net_mask(0, NULL, 0xff, 98);	// sync plu
				commun_net_mask(1, NULL, 0xff, 98);	// sync d/c
				commun_net_mask(2, NULL, 0xff, 98);	// sync ingredient
				commun_net_mask(3, NULL, 0xff, 98);	// sync scroll msg
			#ifdef USE_CHN_EMART_TRACE
				chn_emart_trace_delete_all();
			#endif
#ifdef USE_CHN_EMART_BIRDS_TRACE
				chn_emart_qr_trace_delete_all(); //20140911
#endif

				// Deleted by CJK 20060510
				//set_nvram_sparam(NVRAM_VERSION,Startup.fwversion,3);
#ifdef USE_TRACE_STANDALONE
				TNT_InitTraceNoTable();	// Trace No Table 초기화
#endif
#ifdef USE_CHN_CART_SCALE
				trace_init();
#endif
				return;
	}
	else
	{
		BuzOn(1);
	}

}


void Report_ClearSendingTransaction(void)
{
#ifdef USE_EMART_NETWORK
	INT8U result;
	char string_buf[50];
	INT16U head, tail, total_cnt;
	INT16U total_send_sale, total_send_change;
	INT8U key_mode;

	key_mode = KEY_GetMode();
	KEY_SetMode(1);

 	total_send_sale = get_nvram_wparam(NVRAM_TOTAL_SEND_SALE);
 	total_send_change = get_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE);

	head = get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
	tail = get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
	if (head >= tail) total_cnt = head - tail;
	else              total_cnt = head + Startup.max_send_buf - tail;

	if (total_cnt)
	{
		sprintf(string_buf, "잔량 LOG가 남아있습니다");
		display_message_page_mid(string_buf);
	}
	else if (total_send_sale == 0 && total_send_change == 0)
	{
		sprintf(string_buf, "집계된 LOG파일이 없습니다");
		display_message_page_mid(string_buf);
	}
	else
	{
		sprintf(string_buf, "전체 LOG집계 삭제");
		result = Report_Confirm(string_buf);
		if (result)
		{
		 	set_nvram_wparam(NVRAM_TOTAL_SEND_SALE, 0);
	 		set_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE, 0);
			// store reset time
		}
	}
	KEY_SetMode(key_mode);
#else
	INT8U result;
	result = Report_Confirm(NULL);
	if (result)
	{
		set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,0);
		set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,0);
		set_nvram_wparam(NVRAM_TOTAL_SEND_SALE,0);
		set_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE,0);

		set_nvram_bparam(NVRAM_TRANS_PAY_HEAD,0);
		set_nvram_bparam(NVRAM_TRANS_PAY_TAIL,0);
		// store reset time
	}
#endif
}

#ifdef USE_INDIA_TAX_REPORT_MENU
void Report_Tax(INT8U mode, INT16S period)
{
	INT16U start_no, end_no;
		INT16U result;
		INT16U min, max;
		POINT disp_p;
		CAPTION_STRUCT cap;
		char string_buf[50];
		INT8U backup_direct;
		INT8U ClearReport, PrintReport;
		RTC_STRUCT time_str;
		INT8U prt_result, backup_unit;
		BASIC_REPORT_TYPE total_br;

	
		min = 0;
		max = MAX_SCALE - 1;
		start_no = status_scale.scaleid;
		if (start_no > MAX_SCALE - 1) start_no = MAX_SCALE - 1; 
		end_no	 = start_no;
		if (ethernet_list.status==1) {
			start_no = min;
			end_no = max;
		}
	
		DspStruct.DirectDraw = 0;
	    Dsp_Fill_Buffer(0);
     	menu_display_menucode(CurMenuCode, 1);
	    DspLoadFont1(DSP_MENU_FONT_ID);
	    sprintf(string_buf, "   %c%d TAX SALES REPORT", ReportTypeChar[mode], period+1);
		menu_writetitle_str(string_buf);
		menu_writepage(1, 1);
		Dsp_Diffuse();
	
		if(mode == REPORT_MODE_Z) {
		result = KP_ENTER;
		} else {
	//		if (ethernet_list.status != 2) {	// if Master
			if (ethernet_list.status == 1) {	// if Master
				result = Report_InputRange(REPORT_SCALE, &start_no, &end_no, 2, max, min);
			} else {
				result = KP_ENTER;
			}
		}
		
	
		switch(result) {
			case KP_SAVE:
			case KP_ENTER :
				DisplaySalesSummary(REPORT_TAX, period, start_no, end_no, NVRAM_SCALE_SUMMARY, &total_br);
				if(Startup.menu_type) {
				  //	disp_p = point(3*Startup.menu_ygap, 0);
					disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, 0); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
					memset((INT8U *)string_buf, ' ', 32);
					string_buf[32] = 0;
					Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
					if(mode == REPORT_MODE_Z) {
						if(PrtOper.EnPrtZReport) {
							caption_split_by_code(0x1aab, &cap,0);	//"TEST = print & clear"
						} else {
							caption_split_by_code(0x1aa9, &cap,0);	//"TEST = clear data"
						}
					} else {
						caption_split_by_code(0x1aa8, &cap,0);			//"TEST = print"
					}
					 // disp_p = point(3*Startup.menu_ygap, Startup.menu_xgap);
					disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, Startup.menu_xgap); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
					Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
				}
				Dsp_Diffuse();
				while(1) {
					if(KEY_Read()) {
						switch(KeyDrv.CnvCode) {
							case KP_ESC :
								BuzOn(1);
								return;
							case KP_ENTER:
							case KP_TEST :
								BuzOn(1);
								Report_GetCurTime(&time_str,0);
								if(mode == REPORT_MODE_Z) {
									if(!Report_ConfirmReset(OFF)) continue;
									if(PrtOper.EnPrtZReport) {
										//if(!Report_ConfirmReset(ON)) continue;
										PrintReport = 1;
									} else {
										//if(!Report_ConfirmReset(OFF)) continue;
										PrintReport = 0;
									}
									ClearReport = 1;
								} else {
									//if(!Report_ConfirmPrint()) continue;
									ClearReport = 0;
									PrintReport = 1;
								}
								prt_result = ON;
								if(PrintReport) {
									backup_direct = DspStruct.DirectDraw;
									DspStruct.DirectDraw = 1;
								 // 	disp_p = point(3*Startup.menu_ygap, Startup.menu_xgap);
									if(Startup.menu_type) {
										disp_p = point((Startup.menu_type+2) * Startup.menu_ygap, Startup.menu_xgap); //se-hyung 20071004  리포트 메뉴에서 글자 겹치는 현상 수정
										memset((INT8U *)string_buf, ' ', 32);
										string_buf[32] = 0;

										Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
										caption_split_by_code(0x1aa5, &cap,0);	//"TEST = print"
										Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
									}
									backup_unit = CurPluWeightUnit;
									CurPluWeightUnit = ADM_Status.UseUnit;

									if (PrtPaperType==0) {
										prt_result = Report_PrintLabel(REPORT_TAX,mode,period,0,0,start_no,end_no);
									} else {
										prt_result = Report_PrintTaxSalesSummary(mode, period, start_no, end_no, &time_str, &total_br);
									}

									CurPluWeightUnit = backup_unit;
									if(Startup.menu_type) {
										memset((INT8U *)string_buf, ' ', 32);
										string_buf[32] = 0;
										Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);

										if(mode == REPORT_MODE_Z && PrtOper.IndividualClear) {
											if(PrtOper.EnPrtZReport) {
												caption_split_by_code(0x1aab, &cap,0);	//"TEST = print & clear"
											} else {
												caption_split_by_code(0x1aa9, &cap,0);	//"TEST = clear data"
											}
										} else {
											caption_split_by_code(0x1aa8, &cap,0);	//"TEST = print"
										}

										Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
									}
									DspStruct.DirectDraw = backup_direct;
								}
								if(ClearReport && prt_result) {
	//								if (ethernet_list.status==1) {
	//									command_set(0x40,0x10,(unsigned char)period,0,0,0,NULL);
	//								}
									if(PrtOper.IndividualClear)
									{
										Report_ClearScaleReport(period, start_no, end_no, &time_str);
									}
									else
									{
										Report_PeriodResetTime(period, &time_str);
										ClearSalesSummaries(period);
										if (GlbOper.ClearOpenedClerkTransaction)
										{
											ClerkClearTransaction();
#if defined(USE_INDIA_FUNCTION) || defined(USE_SEND_TICKET_DATA) || defined(USE_SETPRODUCT_SALE) || defined(USE_HYUNDAI_SET_FUNCTION) 
											ClearAdditionalData();
#endif
										}
										//Added by JJANG 20080529
										if (GlbPrint.reportType == 2)
										{
											//CounterTicketGroupZ[period]++;
											//if (CounterTicketGroupZ[period] > 999999L) CounterTicketGroupZ[period] = 1L;
										}
									}
									return;
								}
								break;
							case KP_FEED :
								BuzOn(1);
								keyapp_feed(OFF);
								break;
							default :
								BuzOn(2);
								break;
						}
					}
				}
				//break;
			case KP_ESC :
				return;
			case KP_FEED :
				BuzOn(1);
				keyapp_feed(OFF);
				break;
			default :
				BuzOn(2);
				break;
		}

}
#endif

void Report_SendingTransaction(void)
{
#ifdef USE_INDIA_FUNCTION
	TRANSACTION_SALE_DATA_TYPE8 trans_sdt;
#else
	TRANSACTION_SALE_DATA_TYPE trans_sdt;
#endif
	TRANSACTION_CHANGE_DATA_TYPE trans_cdt;
	TRANSACTION_RECORD_TYPE trans;
#ifdef USE_CHN_CART_SCALE
	REPORT_LOG_STRUCT trans_log;
#endif
#ifdef USE_ARAB_CONVERT
	CAPTION_STRUCT cap;
#endif
	INT8U backup_directdraw;
	INT16U head, tail, disp_tail, total_cnt;
	INT16U cur_id;
	INT16U start_id;
	INT16U i;
	INT8U loop;
	INT32U addr;
	INT16U plu_key;
	INT16U name_size;
	INT8U string_buf[100];
	INT8U plu_name[60];
	POINT disp_p;
	INT8U old_font;
	char char_h;
	INT16U send_buf_size;
	INT8U dept;
	INT32U pluno;
	INT8U start_pt;
	INT8U type, month, day, hour, min;
	long price;
#ifdef USE_EMART_NETWORK
	INT16U total_cnt_sale, total_cnt_change;
	INT16U total_send_sale, total_send_change;
#endif
#ifdef USE_LOTTEMART_NETWORK
		INT16U total_cnt_change, total_send_change;
#endif
	INT16U cur_time;
	INT8U key_mode;
	char tran_type_str[10];
	char	priceBuf1[30];
	char	priceBuf2[30];

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;

	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	//char_w = display_font_get_width();
	char_h = display_font_get_height();

READ_START:

	head = get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
	tail = get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
	if (head >= tail) total_cnt = head - tail;
	else              total_cnt = head + Startup.max_send_buf - tail;

	if (GlbOper.realtimeTransactionType == 4 || GlbOper.realtimeTransactionType == 6 || GlbOper.realtimeTransactionType == 9)
	{
#ifdef USE_INDIA_FUNCTION
		send_buf_size = MAX_TRANSACTION_TYPE8_SALE_SIZE;
#else
		send_buf_size = MAX_TRANSACTION_TYPE4_SALE_SIZE;
#endif
	}
#ifdef USE_CHN_CART_SCALE
	else if (GlbOper.realtimeTransactionType == 7)
	{
		send_buf_size = MAX_TRANSACTION_TYPE7_SALE_SIZE;
	}
#endif
#ifdef USE_SEND_TRACE_PRICE
	else if (GlbOper.realtimeTransactionType == 8)
	{
		send_buf_size = MAX_TRANSACTION_TYPE10_SALE_SIZE;
	}
#endif
#ifdef USE_HYUNDAI_MEAT_FUNCTION
	else if (GlbOper.realtimeTransactionType == 14)
	{
		send_buf_size = MAX_TRANSACTION_TYPE14_SALE_SIZE;
	}
#endif
	else
	{
		send_buf_size = MAX_TRANSACTION_TYPE1_SALE_SIZE;
	}

// 	total_send_sale = 0;
// 	total_send_change = 0;

PAGE_VIEW:
	Dsp_Fill_Buffer(0);
#ifdef USE_EMART_NETWORK
	if (GlbOper.realtimeTransactionType == 4 && UseEmartNetwork)
	{
	 	total_send_sale = get_nvram_wparam(NVRAM_TOTAL_SEND_SALE);
	 	total_send_change = get_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE);
		total_cnt_sale = 0;
		total_cnt_change = 0;
		cur_id = 0;
		while(1) 
		{
			disp_tail = tail + cur_id;
			disp_tail %= Startup.max_send_buf;
			if (disp_tail == head) break;
			addr = disp_tail;
			addr *= send_buf_size;
			addr += Startup.addr_send_buf;
			type = get_nvram_bparam(addr);
			if (type&0x80)	// 매가 변경
			{
				total_cnt_change++;
			}
			else		// 생산 등록
			{
				total_cnt_sale++;
			}
			cur_id++;
		}

		sprintf((char*)string_buf, "생산:%06u  잔량:%04u", total_send_sale, total_cnt_sale);	// caption추가
		disp_p = point(0, 0);	       		 
		Dsp_Write_String(disp_p, (HUGEDATA char *)&string_buf);
		sprintf((char*)string_buf, "변경:%06u  잔량:%04u", total_send_change, total_cnt_change);	// caption추가
		disp_p = point(Startup.menu_ygap, 0);	       		 
		Dsp_Write_String(disp_p, (HUGEDATA char *)&string_buf);
	}
	else
	{
#elif defined (USE_LOTTEMART_NETWORK)
	if (GlbOper.realtimeTransactionType == 4)
	{
		//total_send_sale = get_nvram_wparam(NVRAM_TOTAL_SEND_SALE);
		total_send_change = get_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE);
		//total_cnt_sale = 0;
		total_cnt_change = 0;
		cur_id = 0;
		while(1) 
		{
			disp_tail = tail + cur_id;
			disp_tail %= Startup.max_send_buf;
			if (disp_tail == head) break;
			addr = disp_tail;
			addr *= send_buf_size;
			addr += Startup.addr_send_buf;
			type = get_nvram_bparam(addr);
			if (type&0x80)	// 매가 변경
			{
				total_cnt_change++;
			}
			//else		// 생산 등록
			//{
			//	total_cnt_sale++;
			//}
			cur_id++;
		}

		//sprintf((char*)string_buf, "생산:%06u  잔량:%04u", total_send_sale, total_cnt_sale);	// caption추가
		//disp_p = point(0, 0); 			 
		//Dsp_Write_String(disp_p, (HUGEDATA char *)&string_buf);
		sprintf((char*)string_buf, "변경:%06u  잔량:%04u", total_send_change, total_cnt_change);	// caption추가
		disp_p = point(0, 0);	
		//disp_p = point(Startup.menu_ygap, 0); 			 
		Dsp_Write_String(disp_p, (HUGEDATA char *)&string_buf);
	}
	else
	{
#endif
#if defined(USE_SINGLE_LINE_GRAPHIC)
		display_clear(DISPLAY_UNITPRICE);
		display_string(DISPLAY_UNITPRICE,(INT8U *)"Remain");
		display_clear(DISPLAY_PRICE);
		display_ulong(DISPLAY_PRICE, total_cnt, 0);
#else
#ifdef USE_ARAB_CONVERT
		caption_split_by_code(0x9c82, &cap,0);	//"Remained Data"
		sprintf((char*)string_buf, "%s%u", cap.form, total_cnt);
#else
		sprintf((char*)string_buf, "Remained Data : %u", total_cnt);	// caption추가
#endif
		disp_p = point(0, 0);	       		 
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)&string_buf, strlen(string_buf), 0, 0); 
		hebrew_codetemp[strlen(string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)string_buf, strlen((char *)string_buf), 0, 0, 0); 
		arab_codetemp[strlen((char *)string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p, (HUGEDATA char *)&string_buf);
#endif
#endif
#if defined(USE_EMART_NETWORK) || defined(USE_LOTTEMART_NETWORK) 
	}
#endif
	Dsp_Diffuse();

	cur_time = SysTimer100ms;
	while(!KEY_Read())
	{
		if ((INT16U)(SysTimer100ms - cur_time) > 10) goto READ_START;
	}
	BuzOn(1);

#if defined(USE_SINGLE_LINE_GRAPHIC)
	switch(KeyDrv.CnvCode) 
	{
#ifndef USE_EMART_NETWORK
		case KP_TEST:
			Report_ClearSendingTransaction();
			goto PAGE_VIEW;
#endif
		case KP_ESC:
			DspStruct.DirectDraw = backup_directdraw;
			DspLoadFont1(old_font);
			KEY_SetMode(key_mode);
			return;
		default :
			BuzOn(2);
			goto PAGE_VIEW;
	}
#else
	switch(KeyDrv.CnvCode)
	{
		case KP_ARROW_DN :
		case KP_PAGE_DN :
			goto PAGE_VIEW2;
#ifndef USE_EMART_NETWORK
		case KP_TEST:
			Report_ClearSendingTransaction();
			break;
#endif
		case KP_ESC:
			DspStruct.DirectDraw = backup_directdraw;
			DspLoadFont1(old_font);
			KEY_SetMode(key_mode);
			return;
		default :
			BuzOn(2);
			goto PAGE_VIEW;
	}
#endif

#if !defined(USE_SINGLE_LINE_GRAPHIC)
PAGE_VIEW2:
#endif
	loop = ON;
	if (KeyDrv.CnvCode == KP_ESC || total_cnt == 0) loop = OFF;
	else if (KeyDrv.CnvCode == KP_ENTER || KeyDrv.CnvCode == KP_ARROW_DN || KeyDrv.CnvCode == KP_PAGE_DN);
	else goto READ_START;

	cur_id = 0;
	start_id = 0;
	start_pt = 0;
	while(loop)
	{
		Dsp_Fill_Buffer(0);
		cur_id = start_id;
		i = 0;
		while(1) 
		{
			disp_tail = tail + cur_id;
			disp_tail %= Startup.max_send_buf;
			if (disp_tail == head) break;
			addr = disp_tail;
			addr *= send_buf_size;
			addr += Startup.addr_send_buf;

#ifdef USE_INDIA_FUNCTION
			if (GlbOper.realtimeTransactionType == 4)
			{
				get_nvram_sparam(addr,(INT8U *)&trans_sdt, sizeof(TRANSACTION_SALE_DATA_TYPE8));
				tran_type_str[0] = 0;
				dept = trans_sdt.sdtDept;
				pluno = trans_sdt.sdtPLUNo;
				month = trans_sdt.sdtDate[1];
				day = trans_sdt.sdtDate[2];
				hour = trans_sdt.sdtDate[3];
				min = trans_sdt.sdtDate[4];
				price = trans_sdt.sdtTotalPrice;
			}
#else
			if (GlbOper.realtimeTransactionType == 4)
			{
				type = get_nvram_bparam(addr);
				if (UseEmartNetwork && (type&0x80))
				{
					get_nvram_sparam(addr,(INT8U *)&trans_cdt, sizeof(TRANSACTION_CHANGE_DATA_TYPE));
					strcpy(tran_type_str, "변경");
					dept = trans_cdt.cdtDept;
					pluno = trans_cdt.cdtPLUNo;
					month = trans_cdt.cdtDate[1];
					day = trans_cdt.cdtDate[2];
					hour = trans_cdt.cdtDate[3];
					min = trans_cdt.cdtDate[4];
					price = trans_cdt.cdtResultUnitPrice;
				}
				else
				{
					get_nvram_sparam(addr,(INT8U *)&trans_sdt, sizeof(TRANSACTION_SALE_DATA_TYPE));
					strcpy(tran_type_str, "생산");
					if (!UseEmartNetwork) tran_type_str[0] = 0;
					dept = trans_sdt.sdtDept;
					pluno = trans_sdt.sdtPLUNo;
					month = trans_sdt.sdtDate[1];
					day = trans_sdt.sdtDate[2];
					hour = trans_sdt.sdtDate[3];
					min = trans_sdt.sdtDate[4];
					price = trans_sdt.sdtTotalPrice;
				}
			}
#endif
#ifdef USE_CHN_CART_SCALE
			else if (GlbOper.realtimeTransactionType == 7)
			{
				get_nvram_sparam(addr,(INT8U *)&trans_log, sizeof(REPORT_LOG_STRUCT));
				tran_type_str[0] = 0;
				dept = 1;
				pluno = trans_log.pluNo;
				month = trans_log.month;
				day = trans_log.day;
				hour = trans_log.hours;
				min = trans_log.minutes;
				price = trans_log.tprice;
			}
#endif
			else
			{
				get_nvram_sparam(addr+2,(INT8U *)&trans, sizeof(TRANSACTION_RECORD_TYPE));
				dept = trans.trtDept;
				pluno = trans.trtPLU;
			}
			plu_table_search(dept, pluno, &plu_key, 0);
			if(plu_key == 0) plu_name[0] = 0;
			else 
			{
				plu_get_field(plu_key-1,PTYPE_PLU_NAME1,(HUGEDATA INT8U*)plu_name,(INT16S*)&name_size, 48);
				if (name_size > 20) name_size = 20;
				if (IsHanCharMid((char*)plu_name, name_size) == CHAR_HAN_MID) name_size--;
				plu_name[name_size] = 0;
			}
			if (GlbOper.realtimeTransactionType == 4 || GlbOper.realtimeTransactionType == 7)
			{
				sprintf((char*)string_buf, "%2d %02d.%02d %02d:%02d %s%6ld,%6ld,%s", cur_id+1, month, day, hour, min, tran_type_str, pluno, price, plu_name);
			}
			else
			{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				FloatToString(TO_STR,priceBuf1,display_parameter.mode_len[DISPLAY_PRICE],DISPLAY_PRICE,&trans.trtVolume,0,1);
#else
				FloatToString(TO_STR,priceBuf1,display_parameter.mode_len[DISPLAY_PRICE],DISPLAY_PRICE,&trans.trtVolume,0,0);
#endif
				GetPriceString(priceBuf2,priceBuf1,OFF,PRICE_DISPLAY);

//				sprintf((char*)string_buf, "%2d %6ld,%6ld %s", cur_id+1, trans.trtPLU, trans.trtVolume, plu_name);
				sprintf((char*)string_buf, "%2d %s, %6ld %s", cur_id+1,  priceBuf2, trans.trtPLU, plu_name);
			}
						
			disp_p = point(i*char_h, 0);	       		 
			Dsp_Write_String(disp_p, (HUGEDATA char *)&string_buf[start_pt]);

			cur_id++;
			i++;
			if (i > Startup.line_max) break;
		}
		Dsp_Diffuse();

		while(!KEY_Read());
		BuzOn(1);
		
		switch(KeyDrv.CnvCode) 
		{
			case KP_ESC:
				loop = OFF;
				break;
			case KP_ARROW_UP:
			case KP_PAGE_UP:
				if (start_id <= 0)
				{
					goto PAGE_VIEW;
				}
				else 
				{
					start_id -= Startup.line_max;
				}
				break;
			case KP_ARROW_DN:
			case KP_PAGE_DN:
			case KP_ENTER:
				if (start_id + Startup.line_max >= total_cnt)
				{
					BuzOn(2);
				}
				else 
				{
					start_id += Startup.line_max;
				}
				break;
			case KP_FEED:
				keyapp_feed(OFF);
				break;
			case KP_ARROW_RI:
				if (start_pt == 0)
				{
					start_pt = 15;
				}
				else if (start_pt == 15)
				{
					start_pt = 33;
				}
				else
				{
					BuzOn(2);
				}
				break;
			case KP_ARROW_LF:
				if (start_pt == 33)
				{
					start_pt = 15;
				}
				else if (start_pt == 15)
				{
					start_pt = 0;
				}
				else
				{
					BuzOn(2);
				}
				break;
			default :	
				BuzOn(2);
				break;
		} //switch(KeyDrv.CnvCode)							
	}
	DspStruct.DirectDraw = backup_directdraw;
	DspLoadFont1(old_font);
	KEY_SetMode(key_mode);
}


//void TotalRcpt_PrintShopName(INT16U shop_no, INT8U flgREPORT) //SG060105
void TotalRcpt_PrintShopName(INT16U shop_no)
{
	INT32U flash_addr;

	STORE_NAME_STRUCT store_str;

	flash_addr =  DFLASH_BASE + FLASH_SHOPNAME_AREA;
	flash_addr =  flash_addr + SHOPNAME_STRUCT_SIZE * (INT32U)(shop_no-1);
	Flash_sread(flash_addr, (HUGEDATA INT8U *)&store_str, sizeof(STORE_NAME_STRUCT));

	RcptCmd_TypeNone();

	if(shop_no == store_str.code) {
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_TITLE, TKT_LAY_N, 0, CENTER);
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
			RcptCmd_StrCenterJust((char*)store_str.name);
#endif//USE_DOUBLE_PRINT
		RcptCmd_PrintMultiLine((char*)store_str.name, sizeof(store_str.name), 0);

		if(store_str.text[0]) {
			//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, CENTER);
#ifdef USE_DOUBLE_PRINT
			if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
				RcptCmd_StrCenterJust((char*)store_str.text);
#endif//USE_DOUBLE_PRINT
			//RcptCmd_PrintMultiLine((char*)store_str.text, sizeof(store_str.text), TICKET_MARGIN+TICKET_TXT_MARGIN);
			RcptCmd_PrintMultiLine((char*)store_str.text, sizeof(store_str.text), 0);
		}
		if(store_str.tel_no[0]) {
			//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, CENTER);
#ifdef USE_DOUBLE_PRINT
			if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
				RcptCmd_StrCenterJust((char*)store_str.tel_no);
#endif//USE_DOUBLE_PRINT
			//RcptCmd_PrintMultiLine((char*)store_str.tel_no, sizeof(store_str.tel_no), TICKET_MARGIN+TICKET_TXT_MARGIN);
			RcptCmd_PrintMultiLine((char*)store_str.tel_no, sizeof(store_str.tel_no), 0);
		}
		RcptCmd_CharLF(1);
	}
}

void TotalRcpt_PrintListTitle(INT8U w_unit)
{
	char string_buf[100], temp_string[35];
//	INT8U cur_unit;
	char ad_unit_str[10], unit_weight_str[34];
	INT8U enable_gst;
	enable_gst = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2) >> 4 & 0x01;

#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	INT8U currency_sign[6];
#else
	INT8U currency_sign[4];
#endif
	INT16U unitweight;

#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN, currency_sign, 5);
	currency_sign[5] = 0;
#else
	get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN, currency_sign, 4);
	currency_sign[4] = 0;	
#endif
	//cur_unit = ad_get_kglb();
	GetWeightSymbol(w_unit, ad_unit_str);// Modified by CJK 20051223
//	if(ADM_GetData.CurUnit == WEIGHT_UNIT_KG) {
//		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)ad_unit_str, 4);
//	} else if(ADM_GetData.CurUnit == WEIGHT_UNIT_G) {
//		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (INT8U *)ad_unit_str, 4);
//	} else {
//		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)ad_unit_str, 4);
//	}
//	ad_unit_str[4] = 0;

	//cur_unit = ad_get_kglb();
	if(w_unit == WEIGHT_UNIT_LB) {
		//switch(unitweight)  {
		//	default:
		//	case 1:	
				label_caption_read(LABCAP_ID_TXT_1LB, unit_weight_str);	
		//		break;
		//}
	} else  {
		unitweight = get_global_wparam(GLOBAL_DEFAULTUNIT);
		switch(unitweight)  {
			default:
			case 1:	
				label_caption_read(LABCAP_ID_TXT_1KG, unit_weight_str);	
				break;
			case 2:	
				label_caption_read(LABCAP_ID_TXT_100G, unit_weight_str);	
				break;
#ifdef USE_WEIGHT_UNIT_500G
			case 3:	
				label_caption_read(LABCAP_ID_TXT_500G, unit_weight_str);
				break;
#endif
		}
	}

	RcptCmd_TypeNone();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_0, 0, RIGHT);
    RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_0, 0, LEFT);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
#endif
	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#ifdef USE_ARAB_TICKET_PRT
  #ifdef USE_PERSIA_CAPTION
	RcptCmd_PutString(0, string_buf, Caption_Amount, LEFT);

	RcptCmd_PutString(1, string_buf, Caption_Price, CENTER);

	receipt_caption_read(RCPTCAP_ID_TKT_TOTAL, temp_string);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
  #else
	sprintf(temp_string, "%s", ad_unit_str);
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	sprintf(temp_string, "%s/%s", unit_weight_str, currency_sign);
	RcptCmd_PutString(1, string_buf, temp_string, CENTER);

	sprintf(temp_string, "%s", currency_sign);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
  #endif
#else
  #if defined(USE_INDIA_TAX) || defined(USE_INDIA_DADUS)
	sprintf(temp_string, "Name    (%s)Rate", currency_sign, unit_weight_str);
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	sprintf(temp_string, "QTY");
	RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

	sprintf(temp_string, "PRICE(%s)", currency_sign);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);

    #ifdef USE_INDIA_TAX
	if(enable_gst)
	{
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));

		RcptCmd_CharLF(1);

		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		sprintf(temp_string, "HSN code");
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
	
		sprintf(temp_string, "CGST%%");
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

		sprintf(temp_string, "SGST%%");
		RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
	}
	#endif
  #else
	sprintf(temp_string, "%s", ad_unit_str);
	//RcptCmd_PutString(0, string_buf, temp_string, CENTER);
	RcptCmd_PutString(0, string_buf, temp_string, RIGHT);

	sprintf(temp_string, "%s/%s", currency_sign, unit_weight_str);
	//RcptCmd_PutString(1, string_buf, temp_string, CENTER);
	RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

	sprintf(temp_string, "%s", currency_sign);
	//RcptCmd_PutString(2, string_buf, temp_string, CENTER);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
  #endif

  #ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf,(char *)string_buf,strlen(string_buf));
	}
	else
	{
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	}
  #else
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
  #endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
#endif
}

#ifdef USE_PRT_SCALE_NUM_IN_TICKET
void TotalRcpt_PrintScaleNo(void)
{
	char caption_buf[40];
	char scale_no_str[40];

	receipt_caption_read(RCPTCAP_ID_REP_SCALE_NO, caption_buf);
	sprintf(scale_no_str, "%s%02d", caption_buf, status_scale.scaleid);

	RcptCmd_TypeNone();
#ifdef USE_ARAB_CONVERT
	RcptCmd_SetFontStyle(3, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(3, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PutDataRxBuf(&CommBufPrt, scale_no_str, strlen(scale_no_str));
	RcptCmd_CharLF(1);
}
#endif

void TotalRcpt_PrintClerkName(INT16U clerkNum)
{
	char string_buf[100];
	CLERK_STRUCT clerk_str;
	INT32U flash_addr;

	flash_addr = DFLASH_BASE + FLASH_CLERK_AREA;
	Flash_sread(flash_addr+(INT32U)clerkNum*CLERK_STRUCT_SIZE, (HUGEDATA INT8U *)&clerk_str, sizeof(CLERK_STRUCT));
	if(clerk_str.code != clerkNum) clerk_str.name[0] = 0;
	sprintf(string_buf, "%s(%d)", clerk_str.name, clerkNum);

	RcptCmd_TypeNone();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		PutDataTxBuf(&CommBuf, (char *)string_buf, strlen(string_buf));
	}
	else
	{
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	}
#else
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
#endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);
}

void TotalRcpt_PrintVoid(void)
{
	char string_buf[100], temp_string[35];
	INT8U length;
	INT8U decimal, dec_ch;

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);

	RcptCmd_TypeNone();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_1, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_1, 0, LEFT);
#endif
	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	// Print Void Count
	receipt_caption_read(RCPTCAP_ID_TKT_VOID, temp_string);
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	sprintf(temp_string, "%2d", PrtTotalStr.VoidCount);
	length = strlen(temp_string);
	temp_string[length++] = ' ';
	receipt_caption_read(RCPTCAP_ID_TKT_ARTICLES, temp_string+length);
	RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

	// Print Void Price
	format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.VoidPrice, decimal, dec_ch);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
	length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf, (char *)string_buf, length);
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
	PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);
}


void TotalRcpt_PrintSubTotal(void)
{
	char string_buf[100], temp_string[35];
	INT8U length;
	INT8U decimal, dec_ch;
	long Sum_savedprice=0;
#ifdef USE_ARAB_TICKET_PRT
    INT8U i;
#endif
	INT8U	Discount_flag;
	Discount_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);

	RcptCmd_TypeNone();

	// Print SubTotal Count
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_1, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_1, 0, LEFT);
#endif
	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_TKT_SUBTOTAL, temp_string);
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	sprintf(temp_string, "%2d", PrtTotalStr.TotalCount);
	length = strlen(temp_string);
	temp_string[length++] = ' ';
	receipt_caption_read(RCPTCAP_ID_TKT_ARTICLES, temp_string+length);
	RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

	// Print SubTotal Price
//ORG	format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.TotalPrice, decimal, dec_ch);
//Modified by JJANG 20080104
#ifdef USE_INDIA_FUNCTION
	if(!(Discount_flag&0x01)) PrtGenSystemCurPriceStr(OUTMODE_TICKET, (HUGEDATA INT8U *)temp_string, PrtTotalStr.TotalPrice, decimal, dec_ch);
	else PrtGenSystemCurPriceStr(OUTMODE_TICKET, (HUGEDATA INT8U *)temp_string, PrtTotalStr.TotalPrice + PrtTotalStr.TotalSavedPrice, decimal, dec_ch);
#else
	if(!(Discount_flag&0x01)) format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.TotalPrice, decimal, dec_ch);
	else format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.TotalPrice + PrtTotalStr.TotalSavedPrice, decimal, dec_ch);
#endif
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
	length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf, (char *)string_buf, length);
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
	PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);	

#ifdef USE_ARAB_TICKET_PRT
    if(PrtTotalStr.TotalSavedPrice)
    {
        RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
		RcptCmd_CharTab(2);

		RcptCmd_PutStringInit(string_buf);

  #ifdef USE_PERSIA_CAPTION
        sprintf(string_buf, "%s", Caption_PluDiscount);
  #else
  		receipt_caption_read(RCPTCAP_ID_TKT_SAVED_PRICE, string_buf);
  #endif
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

        RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
        format_value(( INT8U *)string_buf, PrtTotalStr.TotalSavedPrice, decimal, dec_ch);

        RcptCmd_CharTab(TICKET_MARGIN);			
		length = strlen(string_buf);
        for(i=0;i<Rcpt_MaxWidthChar-10-length;i++) RcptCmd_PrintChar(' ');
		PutDataRxBuf(&CommBufPrt, string_buf, length);
        RcptCmd_CharLF(1);
    }
#endif

//Added by JJANG 20080104
	if(!(Discount_flag&0x01))
	{
		if(PrtTotalStr.SummarySavedPrice)
		{
#ifdef USE_ARAB_TICKET_PRT
            RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
			RcptCmd_CharTab(2);

			RcptCmd_PutStringInit(string_buf);
  #ifdef USE_PERSIA_CAPTION
            sprintf(string_buf, "%s", Caption_SubtotalDiscount);
  #else
  			receipt_caption_read(RCPTCAP_ID_TKT_SAVED_PRICE, string_buf);
  #endif
        	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

            RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
            format_value(( INT8U *)string_buf, PrtTotalStr.SummarySavedPrice, decimal, dec_ch);

            RcptCmd_CharTab(TICKET_MARGIN);			
			length = strlen(string_buf);
            for(i=0;i<Rcpt_MaxWidthChar-10-length;i++) RcptCmd_PrintChar(' ');
			PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_1, 0, LEFT);

			RcptCmd_PutStringInit(string_buf);

			temp_string[0] = 0;
			RcptCmd_PutString(0, string_buf, temp_string, LEFT);

			receipt_caption_read(RCPTCAP_ID_TKT_SAVED_PRICE, temp_string);
			RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

#ifdef USE_INDIA_FUNCTION
			PrtGenSystemCurPriceStr(OUTMODE_TICKET, (HUGEDATA INT8U *)temp_string, PrtTotalStr.SummarySavedPrice, decimal, dec_ch);
#else
			format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.SummarySavedPrice, decimal, dec_ch);
#endif
			RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
			length = strlen(string_buf);
			
			PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif
			RcptCmd_CharLF(1);
		}
	}
//Added by JJANG 20080104
	else	//PARAMETER 621 : YES
	{
		if (PrtTotalStr.SummarySavedPrice || PrtTotalStr.TotalSavedPrice)	//두 종류의 Saved price를 모두 화면에 찍어줌
		{
#ifdef USE_ARAB_TICKET_PRT
            RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
            RcptCmd_CharTab(2);

            RcptCmd_PutStringInit(string_buf);
  #ifdef USE_PERSIA_CAPTION
            sprintf(string_buf, "%s", Caption_SubtotalDiscount);
  #else
  			receipt_caption_read(RCPTCAP_ID_TKT_SAVED_PRICE, string_buf);
  #endif
        	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

            RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
            format_value(( INT8U *)string_buf, PrtTotalStr.SummarySavedPrice, decimal, dec_ch);

            RcptCmd_CharTab(TICKET_MARGIN);         
            length = strlen(string_buf);
            for(i=0;i<Rcpt_MaxWidthChar-10-length;i++) RcptCmd_PrintChar(' ');
            PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_1, 0, LEFT);
			RcptCmd_PutStringInit(string_buf);

			temp_string[0] = 0;
			RcptCmd_PutString(0, string_buf, temp_string, LEFT);

			receipt_caption_read(RCPTCAP_ID_TKT_SAVED_PRICE, temp_string);
			RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

			Sum_savedprice = PrtTotalStr.SummarySavedPrice + PrtTotalStr.TotalSavedPrice;
#ifdef USE_INDIA_FUNCTION
			PrtGenSystemCurPriceStr(OUTMODE_TICKET, (HUGEDATA INT8U *)temp_string, Sum_savedprice, decimal, dec_ch);
#else
			format_value((HUGEDATA INT8U *)temp_string, Sum_savedprice, decimal, dec_ch);
#endif

			RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
			length = strlen(string_buf);
			
			PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif
			RcptCmd_CharLF(1);
		}
	}
}


#ifdef USE_TRACEABILITY
void TotalRcpt_PrintTraceability(INT16U trace_no)
{
	char string_buf[100], temp_string[35];
	INT8U length;
//	INT8U decimal, dec_ch;
	TRACEABILITY_STRUCT traceability_str;
	INT16U born_country_no, bred_country_no, slaught_no, cutting_no, slaught_country_no, cutting_country_no;
	INT32U addr;

	if(!trace_no) return;

//	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
//	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);


	born_country_no = 0;
	bred_country_no = 0;
	slaught_no	= 0;
	slaught_country_no = 0;
	cutting_no	= 0;
	cutting_country_no = 0;


	addr = DFLASH_BASE + FLASH_TRACEABILITY_AREA;
	addr = addr + TRACEABILITY_STRUCT_SIZE * (INT32U)(trace_no - 1);
	Flash_sread(addr, (HUGEDATA INT8U *)&traceability_str, TRACEABILITY_STRUCT_SIZE);
	if(traceability_str.code != trace_no) return;
	else {
		born_country_no = traceability_str.idata[0];
		bred_country_no = traceability_str.idata[1];
		slaught_no	= traceability_str.idata[2];
		cutting_no	= traceability_str.idata[3];
	}

	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+3);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_N, 0, LEFT);

	//RcptCmd_PutStringInit(string_buf);
	
	RcptCmd_TypeUnderline();
	receipt_caption_read(RCPTCAP_ID_TKT_TRACEABILITY, string_buf);
	sprintf(temp_string, " %d : ", trace_no);
	strcat(string_buf, temp_string);

	length = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, length);

	get_traceability_name(trace_no, string_buf);
	length = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, length);
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

// Born in Country
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_2, 0, LEFT);

	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+3);
	receipt_caption_read(RCPTCAP_ID_TKT_BORN_COUNTRY, temp_string);
	//sprintf(temp_string, "Born In Country:");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	if(get_country_name(born_country_no, temp_string)) {
		length = strlen(temp_string);
		sprintf(temp_string+length, "(%d)", born_country_no);
	} else {
		temp_string[0] = 0;
	}
	RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	length = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, length);
	RcptCmd_CharLF(1);

// Bred in Country
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_2, 0, LEFT);

	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+3);
	receipt_caption_read(RCPTCAP_ID_TKT_BRED_COUNTRY, temp_string);
	//sprintf(temp_string, "Bred In Country:");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	if(get_country_name(bred_country_no, temp_string)) {
		length = strlen(temp_string);
		sprintf(temp_string+length, "(%d)", bred_country_no);
	} else {
		temp_string[0] = 0;
	}
	RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	length = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, length);
	RcptCmd_CharLF(1);
	
// Slaughter House
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_2, 0, LEFT);

	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+3);
	receipt_caption_read(RCPTCAP_ID_TKT_SLA_HOUSE, temp_string);
	//sprintf(temp_string, "Slaughter House:");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	if(get_slaughterhouse_name(slaught_no, temp_string)) {
		length = strlen(temp_string);
		sprintf(temp_string+length, "(%d)", slaught_no);
	} else {
		temp_string[0] = 0;
	}
	RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	length = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, length);
	RcptCmd_CharLF(1);

// Slaughtered in Country
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_2, 0, LEFT);

	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+3);
	receipt_caption_read(RCPTCAP_ID_TKT_SLA_COUNTRY, temp_string);
	//sprintf(temp_string, "Slaughter Country:");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	if(get_slaughtercountry_name(slaught_no, temp_string, &slaught_country_no)) {
		length = strlen(temp_string);
		sprintf(temp_string+length, "(%d)", slaught_country_no);
	} else {
		temp_string[0] = 0;
	}
	RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	length = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, length);
	RcptCmd_CharLF(1);

// Cutting Hall
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_2, 0, LEFT);

	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+3);
	receipt_caption_read(RCPTCAP_ID_TKT_CUT_HALL, temp_string);
	//sprintf(temp_string, "Cutting Hall:");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	if(get_cuttinghall_name(cutting_no, temp_string)) {
		length = strlen(temp_string);
		sprintf(temp_string+length, "(%d)", cutting_no);
	} else {
		temp_string[0] = 0;
	}
	RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	length = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, length);
	RcptCmd_CharLF(1);

// Cut in Country
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_2, 0, LEFT);

	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+3);
	receipt_caption_read(RCPTCAP_ID_TKT_CUT_COUNTRY, temp_string);
	//sprintf(temp_string, "Cutting Country:");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	if(get_cuttingcountry_name(cutting_no, temp_string, &cutting_country_no)) {
		length = strlen(temp_string);
		sprintf(temp_string+length, "(%d)", cutting_country_no);
	} else {
		temp_string[0] = 0;
	}
	RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	length = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, length);
	RcptCmd_CharLF(1);
}
#endif

#ifdef USE_CHN_CART_SCALE
void TotalRcpt_PrintTraceCode(INT8U *traceCode)
{
	char string_buf[100], temp_string[35];
	INT8U length;
	INT8U decimal, dec_ch;
	TRACEABILITY_STRUCT traceability_str;
	INT16U born_country_no, bred_country_no, slaught_no, cutting_no, slaught_country_no, cutting_country_no;
	INT32U addr;

	if(!traceCode[0]) return;

	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);

	//RcptCmd_PutStringInit(string_buf);
	
	//RcptCmd_TypeUnderline();
	//receipt_caption_read(RCPTCAP_ID_TKT_TRACEABILITY, string_buf);
	string_buf[0] = 0;
	sprintf(temp_string, "瀏巨쯤:%s", traceCode);
	strcat(string_buf, temp_string);

	length = strlen(string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, length);
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();
}
#endif

void TotalRcpt_PrintPayment(void)
{
	char string_buf[100], temp_string[35];
	INT8U length;
	INT8U decimal, dec_ch;
	INT8U i, j;

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);

	RcptCmd_TypeNone();
    
#ifdef USE_ARAB_TICKET_PRT
    RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
    RcptCmd_CharTab(TICKET_MARGIN);
    RcptCmd_PutStringInit(string_buf);

    RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
    receipt_caption_read(RCPTCAP_ID_TKT_PAYMENT, string_buf);
    PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    RcptCmd_CharLF(1);

    RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
    PrtMakePriceString(string_buf, PrtTotalStr.PayPrice, decimal, dec_ch, 0);
    length = strlen(string_buf);

    RcptCmd_CharTab(TICKET_MARGIN);
    // ( 1 line max 17 char )- Ticket_margin(1) = 16
    for(j=0;j<16-length;j++) RcptCmd_PrintChar(' '); 
    
    
  #ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf, (char *)string_buf, length);
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
  #else
	PutDataRxBuf(&CommBufPrt, string_buf, length);
  #endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);

	for(i = 0; i < MAX_PAID_TYPE; i++) {
		if(PrtTotalStr.PaidTypePrice[i] == 0) continue;
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
		RcptCmd_CharTab(TICKET_MARGIN);

        RcptCmd_PutStringInit(string_buf);
		string_buf[0] = ' ';
		string_buf[1] = ' ';
		receipt_caption_read(RCPTCAP_ID_COM_PAY_CASH+i, string_buf+2);
        PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

        RcptCmd_CharTab(TICKET_MARGIN);
        RcptCmd_PutStringInit(string_buf);
           
        PrtMakePriceString(string_buf, PrtTotalStr.PaidTypePrice[i], decimal, dec_ch, 0);
        length = strlen(string_buf);
        for(j=0;j<Rcpt_MaxWidthChar-TICKET_MARGIN-6-length;j++) RcptCmd_PrintChar(' '); 
  #ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8){
			RcptCmd_PrintPosition(0);
			commun_outleng(&CommBuf, (char *)string_buf, length);
		}
		else
			PutDataRxBuf(&CommBufPrt, string_buf, length);
  #else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
  #endif//USE_DOUBLE_PRINT
		RcptCmd_CharLF(1);
	}
#else
        RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
        RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
        RcptCmd_PutStringInit(string_buf);
    
#ifndef	USE_AU_TICKET_DEFAULT
        RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_5, 0, LEFT);
#else
        RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_2, 0, LEFT);
#endif
        receipt_caption_read(RCPTCAP_ID_TKT_PAYMENT, temp_string);
        RcptCmd_PutString(0, string_buf, temp_string, LEFT);
        //format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.PayPrice, decimal, dec_ch);
#ifndef	USE_AU_TICKET_DEFAULT
        PrtMakePriceString(temp_string, PrtTotalStr.PayPrice, decimal, dec_ch, 0);
#else
        format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.PayPrice, decimal, dec_ch);
#endif
        RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
    
        length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf, (char *)string_buf, length);
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
        PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif//USE_DOUBLE_PRINT
        RcptCmd_CharLF(1);
    
        for(i = 0; i < MAX_PAID_TYPE; i++) {
            if(PrtTotalStr.PaidTypePrice[i] == 0) continue;
            RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_2, 0, LEFT);
    
            RcptCmd_PutStringInit(string_buf);
            RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
            temp_string[0] = ' ';
            temp_string[1] = ' ';
            receipt_caption_read(RCPTCAP_ID_COM_PAY_CASH+i, temp_string+2);
            RcptCmd_PutString(0, string_buf, temp_string, LEFT);
    
            //format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.PaidTypePrice[i], decimal, dec_ch);
#ifndef	USE_AU_TICKET_DEFAULT
            PrtMakePriceString(temp_string, PrtTotalStr.PaidTypePrice[i], decimal, dec_ch, 0);
#else
            format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.PaidTypePrice[i], decimal, dec_ch);
#endif
            length = strlen(temp_string);
#ifndef	USE_AU_TICKET_DEFAULT
            temp_string[length++] = ' ';
            temp_string[length++] = ' ';
#endif
            temp_string[length++] = 0;
            RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
            length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8){
			RcptCmd_PrintPosition(0);
			commun_outleng(&CommBuf, (char *)string_buf, length);
		}
		else
			PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
        PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif//USE_DOUBLE_PRINT
            RcptCmd_CharLF(1);
        }
#endif
}

void TotalRcpt_PrintChange(long changePrice)
{
	char string_buf[100], temp_string[35];
	INT8U length;
	INT8U decimal, dec_ch, i;

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);
 
	RcptCmd_TypeNone();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PutStringInit(string_buf);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
	receipt_caption_read(RCPTCAP_ID_TKT_CHANGE, string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
    RcptCmd_CharLF(1);

    RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
    PrtMakePriceString(string_buf, changePrice, decimal, dec_ch, 0);
    // ( 1 line max 17 char )- Ticket_margin(1) = 16
    for(i=0;i<16-strlen(string_buf);i++) RcptCmd_PrintChar(' ');
    PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_PutStringInit(string_buf);	
//SG	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_1, 0, LEFT);
#ifndef	USE_AU_TICKET_DEFAULT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_5, 0, LEFT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_2, 0, LEFT);
#endif
	receipt_caption_read(RCPTCAP_ID_TKT_CHANGE, temp_string);
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	//format_value((HUGEDATA INT8U *)temp_string, changePrice, decimal, dec_ch);
#ifndef	USE_AU_TICKET_DEFAULT				
	PrtMakePriceString(temp_string, changePrice, decimal, dec_ch, 0);
#else
	format_value((HUGEDATA INT8U *)temp_string, changePrice, decimal, dec_ch);
#endif
	RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
#endif

	length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8){
		RcptCmd_PrintPosition(0);
		commun_outleng(&CommBuf, (char *)string_buf, length);
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
	PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);
}

void TotalRcpt_PrintGrandTotal(void)
{
	char string_buf[100], temp_string[35];
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	char	currency_sign1[6];
#else
	char	currency_sign1[5];
#endif
	INT8U decimal, dec_ch, len, i;//, flag_tmp;

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);

	RcptCmd_TypeNone();

	// Print Total Price
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
    RcptCmd_CharTab(TICKET_MARGIN);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
    RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#endif	
	RcptCmd_PutStringInit(string_buf);
#ifdef USE_AU_TICKET_DEFAULT
	if(PrtTotalStr.SummaryRound != 0) {
#ifdef USE_AU_ROUND	//Print round difference
        RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_2, 0, LEFT);		
		receipt_caption_read(RCPTCAP_ID_TKT_ROUND_DIFFERENCE, temp_string);
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		format_value((HUGEDATA INT8U *)temp_string, - PrtTotalStr.SummaryRound, decimal, dec_ch);		
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
#endif
// Print Round Total Price
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_5, 0, LEFT);
		RcptCmd_PutStringInit(string_buf);
		receipt_caption_read(RCPTCAP_ID_TKT_TOTAL, temp_string);
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		PrtMakePriceString(temp_string, PrtTotalStr.SummaryTotalPrice, decimal, dec_ch, 0);
		//format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.SummaryTotalPrice, decimal, dec_ch);
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
	}
	else
	{
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[2], TKT_LAY_5, 0, LEFT);	
		if(Operation.operationMode == MODE_REPACK && (PrtOper.PrtRepackSymbol)) {//
			receipt_caption_read(RCPTCAP_ID_TKT_TOTAL_REPACK, temp_string);
		} else {
			receipt_caption_read(RCPTCAP_ID_TKT_TOTAL, temp_string);
		}
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		
		PrtMakePriceString(temp_string, PrtTotalStr.SummaryTotalPrice+transPay.summaryround, decimal, dec_ch, 0);
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
		
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
	}
#else
#ifdef USE_ARAB_TICKET_PRT
    RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[2], TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[2], TKT_LAY_5, 0, LEFT);
#endif
	if(Operation.operationMode == MODE_REPACK && (PrtOper.PrtRepackSymbol)) {//
		receipt_caption_read(RCPTCAP_ID_TKT_TOTAL_REPACK, temp_string);
	} else {
#if defined(USE_INDIA_TAX) || defined(USE_INDIA_DADUS)
		sprintf(temp_string, "NET Amt. :");
#else
		receipt_caption_read(RCPTCAP_ID_TKT_TOTAL, temp_string);
#endif
	}
#ifdef USE_ARAB_TICKET_PRT
    PutDataRxBuf(&CommBufPrt, temp_string, strlen(temp_string));
#else
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);
#endif
	
	//format_value((HUGEDATA INT8U *)temp_string, TotalData.TotalPrice-Price.RoundedPrice, decimal, dec_ch);
#ifdef USE_INDIA_FUNCTION
	PrtGenSystemCurPriceStr(OUTMODE_TICKET, (HUGEDATA INT8U *)temp_string, PrtTotalStr.SummaryTotalPrice+transPay.summaryround, decimal, dec_ch);
#else
	format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.SummaryTotalPrice+transPay.summaryround, decimal, dec_ch);
#endif
#ifdef USE_ARAB_TICKET_PRT
    RcptCmd_CharLF(1);
    RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[2], TKT_LAY_N, 0, RIGHT);

	RcptCmd_CharTab(TICKET_MARGIN);
    // ( 1 line max 17 char )- Ticket_margin(1) = 16
#ifdef USE_ARAB 
	// 이란 폰트에서는 17글자가 최대로 표시되는데 아랍의 경우 더 표시될 수 있어서 
	// 최대 24 글자 정도로 공백을 넣어서 표시하니 적당하였기에 24로 정함
    for(i=0;i<24-strlen(temp_string);i++) RcptCmd_PrintChar(' ');
#else
    for(i=0;i<16-strlen(temp_string);i++) RcptCmd_PrintChar(' ');
#endif
    PutDataRxBuf(&CommBufPrt, temp_string, strlen(temp_string));
    
  #ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf, (char *)temp_string, strlen(temp_string));
	}
	else
		PutDataRxBuf(&CommBufPrt, temp_string, strlen(temp_string));
  #endif
#else
	RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
  #ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf, (char *)string_buf, strlen(string_buf));
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
  #else
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
  #endif//USE_DOUBLE_PRINT
#endif
	RcptCmd_CharLF(1);

	if(PrtTotalStr.SummaryRound != 0) {
#ifdef USE_AU_ROUND	//Print round difference
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
        RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_5, 0, LEFT);		
		RcptCmd_PutStringInit(string_buf);
		receipt_caption_read(RCPTCAP_ID_TKT_ROUND_DIFFERENCE, temp_string);
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		PrtMakePriceString(temp_string, - PrtTotalStr.SummaryRound, decimal, dec_ch, 0);
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
#endif
// Print Round Total Price
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_5, 0, LEFT);

		RcptCmd_PutStringInit(string_buf);

		receipt_caption_read(RCPTCAP_ID_TKT_ROUNDED_TOTAL, temp_string);
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		PrtMakePriceString(temp_string, PrtTotalStr.SummaryTotalPrice, decimal, dec_ch, 0);
		//format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.SummaryTotalPrice, decimal, dec_ch);
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
	}
#endif
	if(PrtTotalStr.CPrice.CurrencyRate) {
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_2, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		RcptCmd_PutStringInit(string_buf);
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
		get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,5);
		currency_sign1[5] = 0;
#else
		get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
		currency_sign1[4] = 0;
#endif
		//dec_ch  = get_global_bparam(GLOBAL_DECIMAL_SIGN);
		len = 0;
		temp_string[len++] = ' ';
		temp_string[len++] = ' ';
		len += PrtGenCurPriceStr(temp_string+len, 1, 2, currency_sign1, currency_sign1, 0, dec_ch);
		temp_string[len++] = ' ';
		temp_string[len++] = '=';
		temp_string[len++] = ' ';
		PrtGenCurPriceStr(temp_string+len, PrtTotalStr.CPrice.CurrencyRate, 2, 
						PrtTotalStr.CPrice.CurrencyString1, PrtTotalStr.CPrice.CurrencyString2, 
						PrtTotalStr.CPrice.CurrencyRateDiv, dec_ch);
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);

		//if(TotalData.CPrice.CurrencyPrice != 0xffffffff) {
			PrtGenCurPriceStr_2long(temp_string, PrtTotalStr.CPrice.CurrencyPriceH8, PrtTotalStr.CPrice.CurrencyPriceL8, 1,//TotalData.CPrice.CurrencyPrtForm, 
							PrtTotalStr.CPrice.CurrencyString1, PrtTotalStr.CPrice.CurrencyString2, 
							PrtTotalStr.CPrice.CurrencyPriceDiv, dec_ch);
			RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
		//}
	
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
	}
}


void TotalRcpt_PrintCreditTotal(long totalPrice)
{
	char string_buf[100], temp_string[35];
	INT8U decimal, dec_ch;

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);

	RcptCmd_TypeNone();

	// Print Total Price Caption
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_2, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_2, 0, LEFT);
#endif
	RcptCmd_PutStringInit(string_buf);

	receipt_caption_read(RCPTCAP_ID_TKT_TOTAL, temp_string);
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	// Print Total Price
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_2, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_2, 0, LEFT);
#endif
	RcptCmd_PutStringInit(string_buf);
	//PrtMakePriceString(temp_string, totalPrice, decimal, dec_ch, 10);
	//gen_price((HUGEDATA INT8U *)temp_string, totalPrice, decimal, dec_ch);
	PrtMakePriceString(temp_string, totalPrice, decimal, dec_ch, 0);
	RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
	
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);
}

#ifdef USE_TAX_RATE
#ifdef USE_INDIA_TAX
void TotalRcpt_PrintTaxInfo(void)
{
	char string_buf[100], temp_string[35];
	TAX_STRUCT tax_str;
	INT32U addr, flash_addr;
	INT8U i;
	INT8U length;
	char decimal, dec_ch;
	INT32U totalTax = 0;
	char currency_sign1[5];
	INT8U printGSTsummary;
	float discountRate;
	float halfGST;

	printGSTsummary = get_global_bparam(GLOBAL_SALE_SETUP_FLAG17)>>7&0x01;
	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN,(INT8U *)currency_sign1,4);
	currency_sign1[4] = 0;

	totalTax = 0;	
	for(i = 0; i < MAX_TAX_NO; i++)
	{
		totalTax += PrtTotalStr.TaxTypePrice[i];
	}
	RcptCmd_TypeNone();

	if(printGSTsummary)
	{	
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_6, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_PutStringInit(string_buf);
	
		sprintf(temp_string, "GST SUMMARY");
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		length = strlen(string_buf);
		PutDataRxBuf(&CommBufPrt, string_buf, length);
		RcptCmd_CharLF(1);

		temp_string[0] = 0;
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		sprintf(temp_string, "CATEGORY");
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		PutDataRxBuf(&CommBufPrt, string_buf, length);
		RcptCmd_PutStringInit(string_buf);
		sprintf(string_buf, "   CGST(%s)  ", currency_sign1);
		length = strlen(string_buf);
		sprintf(string_buf+length, "   SGST(%s)  ", currency_sign1);
		length = strlen(string_buf);
		sprintf(string_buf+length, "   GST(%s)   ", currency_sign1);
		length = strlen(string_buf);
		PutDataRxBuf(&CommBufPrt, string_buf, length);
		RcptCmd_CharLF(1);
		temp_string[0] = 0;
		
		addr = DFLASH_BASE + FLASH_TAX_AREA;
		for(i = 0; i < MAX_TAX_NO-1; i++) 
		{
			if(!PrtTotalStr.TaxTypePrice[i]) continue;
			flash_addr = addr + (TAX_STRUCT_SIZE * (INT32U)(i));
			Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
			if(i != tax_str.tax_code-1) continue;

			temp_string[0] = 0;

			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_PutStringInit(string_buf);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			sprintf(temp_string, "%c: %5.2f%%", i+'A', (float)(tax_str.tax_rate)/100.0);
			RcptCmd_PutString(0, string_buf, temp_string, LEFT);
			length = strlen(string_buf);
			PutDataRxBuf(&CommBufPrt, string_buf, length);
			RcptCmd_PutStringInit(string_buf);

			
			halfGST = (float)PrtTotalStr.TaxTypePrice[i]/2 + 0.5;
			
			
			format_value((HUGEDATA INT8U *)temp_string, halfGST, decimal, dec_ch);
			sprintf(string_buf, "  %7s", temp_string);
			length = strlen(string_buf);

			format_value((HUGEDATA INT8U *)temp_string, halfGST, decimal, dec_ch);
			sprintf(string_buf+length, "     %7s", temp_string);
			length = strlen(string_buf);

			format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.TaxTypePrice[i], decimal, dec_ch);
			sprintf(string_buf+length, "     %7s", temp_string);
			length = strlen(string_buf);
			PutDataRxBuf(&CommBufPrt, string_buf, length);
			RcptCmd_CharLF(1);
		}

		temp_string[0] = 0;
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		sprintf(temp_string, "TOTAL(%s)", currency_sign1);
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		length = strlen(string_buf);
		PutDataRxBuf(&CommBufPrt, string_buf, length);
		RcptCmd_PutStringInit(string_buf);

		halfGST = (float)totalTax/2+0.5;
			
		PrtGenCurPriceStr(temp_string, halfGST, 1, currency_sign1, currency_sign1, decimal, dec_ch);
		sprintf(string_buf, "%9s", temp_string);
		length = strlen(string_buf);

		PrtGenCurPriceStr(temp_string, halfGST, 1, currency_sign1, currency_sign1, decimal, dec_ch);
		sprintf(string_buf+length, "   %9s", temp_string);
		length = strlen(string_buf);

		PrtGenCurPriceStr(temp_string, totalTax, 1, currency_sign1, currency_sign1, decimal, dec_ch);
		sprintf(string_buf+length, "   %9s", temp_string);

		length = strlen(string_buf);
		PutDataRxBuf(&CommBufPrt, string_buf, length);
		RcptCmd_CharLF(1);
	}
	else //do not print GST summary
	{
		
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_PutStringInit(string_buf);
		
		sprintf(string_buf, "   CGST(%s)", currency_sign1);
		length = strlen(string_buf);
		sprintf(string_buf+length, "       SGST(%s)", currency_sign1);
		length = strlen(string_buf);
		sprintf(string_buf+length, "        GST(%s)", currency_sign1);
		length = strlen(string_buf);

		PutDataRxBuf(&CommBufPrt, string_buf, length);
		RcptCmd_CharLF(1);
		temp_string[0] = 0;
		
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_PutStringInit(string_buf);

		halfGST = (float)totalTax/2+0.5;
		
		PrtGenCurPriceStr(temp_string, halfGST, 1, currency_sign1, currency_sign1, decimal, dec_ch);
		sprintf(string_buf, "%10s", temp_string);
		length = strlen(string_buf);

		PrtGenCurPriceStr(temp_string, halfGST, 1, currency_sign1, currency_sign1, decimal, dec_ch);
		sprintf(string_buf+length, "    %10s", temp_string);
		length = strlen(string_buf);

		PrtGenCurPriceStr(temp_string, totalTax, 1, currency_sign1, currency_sign1, decimal, dec_ch);
		sprintf(string_buf+length, "    %10s", temp_string);

		length = strlen(string_buf);
		PutDataRxBuf(&CommBufPrt, string_buf, length);
		RcptCmd_CharLF(1);
	}
	if(No_taxable_total != 0)
	{
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		sprintf(temp_string, "NO TAXABLE");
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		sprintf(temp_string, "");
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
		PrtGenCurPriceStr(temp_string, No_taxable_total, 1, currency_sign1, currency_sign1, decimal, dec_ch);
		RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
		length = strlen(string_buf);
		PutDataRxBuf(&CommBufPrt, string_buf, length);
		RcptCmd_CharLF(1);
		No_taxable_total = 0;
	}
}

#else
void TotalRcpt_PrintTaxInfo(void)
{
	char string_buf[100], temp_string[35];
	TAX_STRUCT tax_str;
	INT32U addr, flash_addr;
	INT8U i;
	INT8U length;
	INT8U decimal, dec_ch;
	long total;
	long totalTax = 0;
#ifdef USE_ARAB_TICKET_PRT
    char temp_str1[35], temp_str2[10], temp_str3[10];
#endif

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	totalTax = 0;	
	for(i = 0; i < MAX_TAX_NO; i++)
	{
		totalTax += PrtTotalStr.TaxTypePrice[i];
	}
	RcptCmd_TypeNone();

	// Print TAX NO 
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_3, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_PutStringInit(string_buf);
#ifdef USE_AU_TICKET_DEFAULT
	sprintf(temp_string, "GST included in Total");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	format_value((HUGEDATA INT8U *)temp_string, totalTax, decimal, dec_ch);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
#elif defined(USE_ARAB_TICKET_PRT)
    receipt_caption_read(RCPTCAP_ID_TKT_TAX_INFO, temp_string);
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf,(char *)string_buf,length);
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
	PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);

    // Tax List Print
	addr = DFLASH_BASE + FLASH_TAX_AREA;
	for(i = 0; i < MAX_TAX_NO-1; i++) {
		if(!PrtTotalStr.TaxTypePrice[i]) continue;
		flash_addr = addr + (TAX_STRUCT_SIZE * (INT32U)(i));
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
		if(i != tax_str.tax_code-1) continue;
        

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	    RcptCmd_CharTab(2);
		RcptCmd_PutStringInit(string_buf);

        
        // Total Price x TAX Percent     Tax Caption:A
        // Tax Caption
		sprintf(temp_str1, "%c:", i+'A'); 
		if(tax_str.tax_type == 0) {
			receipt_caption_read(RCPTCAP_ID_COM_INT_TAX, temp_str1+2);
		} else {
			receipt_caption_read(RCPTCAP_ID_COM_EXT_TAX, temp_str1+2); 
		}

        // TAX Percent
		sprintf(temp_str2, "%5.2f%% %c", (float)(tax_str.tax_rate)/100.0, 0xD7);

        // Total Price
        total = PrtTotalStr.TaxTypeTotal[i];
		if(GlbOper.priceAddExTax && (tax_str.tax_type == 0)) // INTERNAL TAX 인 경우에만 total 에서 TAX 금액을 빼서 계산
            total -= PrtTotalStr.TaxTypePrice[i];
		format_value((HUGEDATA INT8U *)temp_str3, total, decimal, dec_ch);    

        // Total Price x TAX Percent     Tax Caption:A
	    sprintf(string_buf, "%-10s  %9s %9s", temp_str1,temp_str2, temp_str3);
		length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		{
			RcptCmd_PrintPosition(0);
			PutDataTxBuf(&CommBuf,(char *)string_buf,length);
		}
		else
			PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif
        RcptCmd_CharLF(1);

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
	    RcptCmd_CharTab(2);
		
		format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.TaxTypePrice[i], decimal, dec_ch);

        sprintf(string_buf, "%31s", temp_string); // MaxChar 34 (except margin)
		length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf,(char *)string_buf,length);
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif//USE_DOUBLE_PRINT
        RcptCmd_CharLF(1);
	}

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);

//SG	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_1, 0, LEFT);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, RIGHT);
	RcptCmd_PutStringInit(string_buf);
	receipt_caption_read(RCPTCAP_ID_TKT_TAX_TOTAL, temp_str1);
    
	PrtMakePriceString(temp_str2, totalTax, decimal, dec_ch, 0);
    // Total TAX price  Total TAX caption
    sprintf(string_buf, "%-9s%8s ", temp_str1, temp_str2);//MAX char22 (except margin)

#else
	receipt_caption_read(RCPTCAP_ID_TKT_TAX_INFO, temp_string);
	RcptCmd_PutString(0, string_buf, temp_string, CENTER);

	length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf,(char *)string_buf,length);
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
	PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);

	addr = DFLASH_BASE + FLASH_TAX_AREA;
	for(i = 0; i < MAX_TAX_NO-1; i++) {
		if(!PrtTotalStr.TaxTypePrice[i]) continue;
		flash_addr = addr + (TAX_STRUCT_SIZE * (INT32U)(i));
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));
		if(i != tax_str.tax_code-1) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_4, 0, LEFT);

		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		sprintf(temp_string, "%c:", i+'A');
		if(tax_str.tax_type == 0) {
			receipt_caption_read(RCPTCAP_ID_COM_INT_TAX, temp_string+2);
		} else {
			receipt_caption_read(RCPTCAP_ID_COM_EXT_TAX, temp_string+2);
		}
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);

		total = PrtTotalStr.TaxTypeTotal[i];
		if(GlbOper.priceAddExTax && (tax_str.tax_type == 0)) // INTERNAL TAX 인 경우에만 total 에서 TAX 금액을 빼서 계산
            total -= PrtTotalStr.TaxTypePrice[i];
		format_value((HUGEDATA INT8U *)temp_string, total, decimal, dec_ch);
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

		sprintf(temp_string, "x %5.2f%% ", (float)(tax_str.tax_rate)/100.0);
		RcptCmd_PutString(2, string_buf, temp_string, RIGHT);

		length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		{
			RcptCmd_PrintPosition(0);
			PutDataTxBuf(&CommBuf,(char *)string_buf,length);
		}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif
		RcptCmd_CharLF(1);


		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_4, 0, LEFT);

		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		sprintf(temp_string, "");
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);

		sprintf(temp_string, "=");
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);

		format_value((HUGEDATA INT8U *)temp_string, PrtTotalStr.TaxTypePrice[i], decimal, dec_ch);
		RcptCmd_PutString(2, string_buf, temp_string, RIGHT);

		length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		{
			RcptCmd_PrintPosition(0);
			PutDataTxBuf(&CommBuf,(char *)string_buf,length);
		}
		else
			PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif
		RcptCmd_CharLF(1);
	}

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

//SG	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_1, 0, LEFT);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_5, 0, LEFT);

	RcptCmd_PutStringInit(string_buf);
	receipt_caption_read(RCPTCAP_ID_TKT_TAX_TOTAL, temp_string);
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);

	//format_value((HUGEDATA INT8U *)temp_string, totalTax, decimal, dec_ch);
	PrtMakePriceString(temp_string, totalTax, decimal, dec_ch, 0);
	RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
#endif	
	length = strlen(string_buf);
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf,(char *)string_buf,length);
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, length);
#else
	PutDataRxBuf(&CommBufPrt, string_buf, length);
#endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);
}
#endif
#endif


void TotalRcpt_PrintBarcode(void)
{
	STRUCT_STRFORM_PARAM str_param;
	INT16U barid;

	prt_set_strform_parameter_total(&str_param);

#ifdef USE_INDIA_FUNCTION
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_N, 2, CENTER);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, CENTER);
#endif
#ifdef USE_QR_ADDTOTAL
	if (QrAddTotalFlag)
	{
	  #ifdef USE_QR_SIZE_SETTING
	  if(QrAddTotalFlag == 1 && PrtPaperType == PRINT_MODE_TICKET)
	  {
		RcptCmd_BarHeight(300, 72, 16);
	  }
	  #else
		RcptCmd_BarHeight(200, 72, 16);
	  #endif
		RcptCmd_BarWidth(3);
#ifdef USE_QR_BARCODE_ERASE_GAP
		/* Erase LF */
#else
		RcptCmd_CharLF(2);
#endif
	}
	else
	{
		RcptCmd_BarHeight(80, 72, 16);
		RcptCmd_BarWidth(2);
	}
#else
	RcptCmd_BarHeight(80, 72, 16);
	RcptCmd_BarWidth(2);
#endif

#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_3, 0, LEFT);
		RcptCmd_BarHRI(HRI_POS_DOWN);
		RcptCmd_BarFont(HRI_FONT_B);
	}
	else
		RcptCmd_BarGuard(1);
#else
	RcptCmd_BarGuard(1);
#endif//USE_DOUBLE_PRINT
	barid=get_global_bparam(GLOBAL_BARCODE_TOTAL_ID);

	if(GlbFlagPCnMasterConnection==OFF) barid=99;
	prt_make_barcode(RECEIPT, barid, &str_param, 0);
	RcptCmd_CharLF(1);
}

void TotalRcpt_PrintTicketSerial(void)
{
	char string_buf[64], temp_string[64];
	char str_form[32];
	STRUCT_STRFORM_PARAM str_param;

	RcptCmd_TypeNone();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

	//SG070508
        //GlbFloat.printFontSize[3] : ticket serial font size
	//layer = TKT_LAY_0;
	//if(GlbFloat.printFontSize[3]>1) layer = TKT_LAY_4;
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[3] , TKT_LAY_6, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[3] , TKT_LAY_6, 0, LEFT);
#endif
	//RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_0, 0, LEFT);
	//SG RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

	temp_string[0] = 0;
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);
	RcptCmd_PutString(1, string_buf, temp_string, LEFT);

	prt_set_strform_parameter_total(&str_param);
	get_global_sparam(GLOBAL_TICKET_SERIAL_FORM,(INT8U *)str_form,32);
	str_form[31] = 0;
	prt_make_strform(temp_string, str_form, &str_param, NO_BARCODE);	// serial format
#ifdef USE_PERSIA_CAPTION
    str_form[31] = 0;
	receipt_caption_read(RCPTCAP_ID_LST_NO, str_form);
	strcat(str_form," = ");
	strcat(str_form,temp_string);
	RcptCmd_PutString(1, string_buf, str_form, RIGHT);
#else
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
#endif
#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf, (char *)string_buf, strlen(string_buf));
	}
	else
	{
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
	}
#else
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);
#endif//USE_DOUBLE_PRINT
}

void TotalRcpt_CustomerName(INT8U customerNo)
{
	char string_buf[64];
	CUSTOMER_STRUCT customer;

	RcptCmd_TypeNone();
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

	//sprintf(string_buf, "Customer Name");
	receipt_caption_read(RCPTCAP_ID_TKT_CUSTOM_NAME, string_buf);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	string_buf[0] = 0;
	customer_read(customerNo, &customer);
	if(customerNo == customer.code) {
//		_memcpy((INT8U*)string_buf, customer.name, 32);
		memcpy((INT8U*)string_buf, customer.name, 32);
		string_buf[32] = 0;
	}
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);
}


extern SALE_G_FLOAT_TYPE	GlbFloat;
extern SALE_G_ADD_TYPE		GlbAddup;


//INT8U flagTaxInfoPrint = 1;

void PrintTotalHead(INT16U clerkNum, RTC_STRUCT *cur_time)
{
	INT16U shop_no;
#ifdef USE_CHN_CART_SCALE
	INT8U string_buf[100];
#endif


////	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_N, 0, LEFT);
////	RcptCmd_CharLF(1);
#ifdef USE_PRT_SCALE_NUM_IN_TICKET
	TotalRcpt_PrintScaleNo();
#endif

#ifndef USE_CHN_CART_SCALE
	if(GlbFloat.printStoreName) {
		shop_no = get_net_wparam(NETWORK_STORE_ID);
		TotalRcpt_PrintShopName(shop_no);
		//TotalRcpt_PrintShopName(shop_no,0); //SG010605
	}
#endif
	if(GlbFloat.printClerkName && clerkNum) {
		TotalRcpt_PrintClerkName(clerkNum);
	}

	// Print Current Time
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintFixedTime(cur_time);
	RcptCmd_CharLF(1);
	
#ifdef USE_CHN_CART_SCALE
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
	
	shop_no = get_net_wparam(FLASH_MARKET_ID);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	sprintf(string_buf, "懇끝긍뵀:%d", shop_no);	// market no
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);
	
	shop_no = get_net_wparam(NETWORK_STORE_ID);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	sprintf(string_buf, "?듦긍뵀:%d", shop_no);	// shop no
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	sprintf(string_buf, "든綾넛뵀:%d", status_scale.scaleid);	// scale id
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);
#endif

}

void PrintTotalTail(INT8U copy_ticket)	// Modified by CJK 20050726
{
	INT8U enable_gst;
	enable_gst = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2) >> 4 & 0x01;
#ifdef USE_AU_TICKET_DEFAULT
	char string_buf[100], temp_string[35];
#endif
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(2, 0);
	
	if(!copy_ticket || (copy_ticket && GlbFloat.printCopyList)) {
		if(PrtTotalStr.VoidCount) {
			TotalRcpt_PrintVoid();
		}
		
		TotalRcpt_PrintSubTotal();
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_CharTab(TICKET_MARGIN);
		RcptCmd_PrintLine(1, 0);

#ifdef USE_TAX_RATE
#ifdef USE_INDIA_TAX
		if(enable_gst)
		{
		TotalRcpt_PrintTaxInfo();
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN);
		RcptCmd_PrintLine(1, 0);
		}
#else
		if(GlbFloat.printTaxInfo) {
			if(PrtTotalStr.InTaxPrice + PrtTotalStr.ExTaxPrice) {
				TotalRcpt_PrintTaxInfo();

				RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
				RcptCmd_CharTab(TICKET_MARGIN);
				RcptCmd_PrintLine(1, 0);
			}
		}
#endif
#endif
	}
	
	TotalRcpt_PrintGrandTotal();
	
	if(PrtTotalStr.PayPrice) {
		TotalRcpt_PrintPayment();
		if(PrtTotalStr.ChangePrice) {
			TotalRcpt_PrintChange(PrtTotalStr.ChangePrice);
		}
	}
#ifdef USE_AU_TICKET_DEFAULT
	RcptCmd_TypeNone();
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_3, 0, LEFT);
	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	sprintf(temp_string, "*:taxable items");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));	
	RcptCmd_CharLF(1);
#endif
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
#else
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
#endif
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
}

void PrintCreditSign(void)
{
	char string_buf[64], temp_string[64];

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_1, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

	receipt_caption_read(RCPTCAP_ID_TKT_CUSTOMER, temp_string);
	//sprintf(temp_string, "Customer");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);
	temp_string[0] = 0;
	RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	RcptCmd_PrintLine(1, Rcpt_MaxWidthChar-4);
	RcptCmd_CharLF(1);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	RcptCmd_PrintLine(1, Rcpt_MaxWidthChar-4);
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_0, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_TKT_AUTOGRAPH, temp_string);
	//sprintf(temp_string, "Autograph");
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);
	temp_string[0] = 0;
	RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	RcptCmd_PrintLine(1, Rcpt_MaxWidthChar-4);
	RcptCmd_CharLF(1);
}


extern INT8U OtherWeightUnit;
extern void prt_set_strform_parameter_item(STRUCT_STRFORM_PARAM *param, TRANSACT_CHAIN  *cp);
INT8U PrintOneTransaction(TRANSACT_CHAIN  *cp, INT8U w_unit)
{
	char string_buf[100], temp_string[35];
	INT16U plu_key=0;
	INT16U name_size, length;
	INT8U w_decimal;
	STRUCT_STRFORM_PARAM bar_param;
	INT16U barid;
	long temp_totalprice;
	INT8U decimal, dec_ch;
	INT16U trace_no,sz;
	INT8U linked = OFF;
	
#ifdef USE_AU_TICKET_DEFAULT
	INT8U temp_pos;
#endif
#ifdef USE_PLUNAME2_IN_TICKET
	char plu_name2[40];
	INT16U name2_size;
#endif
#ifdef USE_INDIA_TAX
	char plu_name3[40];
	INT16U name3_size;
	TAX_STRUCT tax_str;
	INT32U flash_addr, addr;;
	float halfGST;
	INT8U enable_gst;
	enable_gst = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2) >> 4 & 0x01;
#endif
	INT8U	Discount_flag;
	Discount_flag = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7);
	if(w_unit != cp->content.trtMultiple) {
		OtherWeightUnit = cp->content.trtMultiple;
		return ON;
	}

	decimal  = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	
#ifdef USE_CHN_CART_SCALE
	TotalRcpt_PrintTraceCode(cp->content.trtTraceCode);
#endif

	RcptCmd_TypeNone();

	if(cp->delete) {
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) != 8)
			RcptCmd_TypeThroughLine(0);
#else
		RcptCmd_TypeThroughLine();
#endif//USE_DOUBLE_PRINT
	}

	if(linked) {
#ifdef USE_ARAB_TICKET_PRT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_N, 0, RIGHT);
#else
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SMALL, TKT_LAY_N, 0, LEFT);
#endif
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		sprintf(string_buf, "* Linked Plu");
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));

		RcptCmd_CharLF(1);
	}

	// Print PLU Name
	w_decimal=get_decimal_pt(3, (INT16U)w_unit);
	name_size = 0;
	trace_no = 0;
#ifdef USE_PLUNAME2_IN_TICKET
	plu_name2[0] = 0;
#endif
#ifdef USE_AU_TICKET_DEFAULT
	temp_pos = 0;
	if(GlbFloat.printTaxInfo && cp->content.trtTax) 
	{
		string_buf[0]= '*';
		temp_pos =1;
	}
	switch(cp->content.trtPLU) 
	{
		case MISC_PLU_WEIGHT :
			receipt_caption_read(RCPTCAP_ID_COM_MISC_WEIGHT, &string_buf[temp_pos]);
			name_size = strlen((char *)string_buf);
			break;
		case MISC_PLU_PCS :
			receipt_caption_read(RCPTCAP_ID_COM_MISC_PCS, &string_buf[temp_pos]);
			name_size = strlen((char *)string_buf);
			break;
		case MISC_PLU_COUNT :
			receipt_caption_read(RCPTCAP_ID_COM_MISC_COUNT, &string_buf[temp_pos]);
			name_size = strlen((char *)string_buf);
			break;
		default :
			plu_table_search(cp->content.trtDept, cp->content.trtPLU,&plu_key,0);
			if(plu_key == 0) string_buf[0] = 0;
			else 
			{
				plu_get_field(plu_key-1,PTYPE_PLU_NAME1,(HUGEDATA INT8U*)&string_buf[temp_pos],(INT16S*)&name_size,48);
				if (name_size > 40) name_size = 40;
#ifdef USE_PLUNAME2_IN_TICKET
				plu_get_field(plu_key-1,PTYPE_PLU_NAME2,(HUGEDATA INT8U*)plu_name2,(INT16S*)&name2_size,48);
				if (name2_size > 40) name2_size = 40;
#endif
				plu_get_field(plu_key-1, PTYPE_TRACE_NO, (INT8U *)&trace_no, (INT16S *)&sz, sizeof(trace_no));
				w_decimal=get_decimal_pt(4, plu_key);
			}
			break;
	}
#else
	switch(cp->content.trtPLU) {
		case MISC_PLU_WEIGHT :
			receipt_caption_read(RCPTCAP_ID_COM_MISC_WEIGHT, string_buf);
			name_size = strlen((char *)string_buf);
			break;
		case MISC_PLU_PCS :
			receipt_caption_read(RCPTCAP_ID_COM_MISC_PCS, string_buf);
			name_size = strlen((char *)string_buf);
			break;
		case MISC_PLU_COUNT :
			receipt_caption_read(RCPTCAP_ID_COM_MISC_COUNT, string_buf);
			name_size = strlen((char *)string_buf);
			break;
		default :
			plu_table_search(cp->content.trtDept, cp->content.trtPLU,&plu_key,0);
			if(plu_key == 0) string_buf[0] = 0;
			else 
			{
 #ifdef USE_ARAB_TICKET_PRT
				string_buf[0] = ' ';
				plu_get_field(plu_key-1,PTYPE_PLU_NAME1,(HUGEDATA INT8U*)&string_buf[1],(INT16S*)&name_size,48);
				string_buf[strlen(string_buf)] = ' ';
 #else
				plu_get_field(plu_key-1,PTYPE_PLU_NAME1,(HUGEDATA INT8U*)string_buf,(INT16S*)&name_size,48);
 #endif
				if (name_size > 40) name_size = 40;
#ifdef USE_PLUNAME2_IN_TICKET
				plu_get_field(plu_key-1,PTYPE_PLU_NAME2,(HUGEDATA INT8U*)plu_name2,(INT16S*)&name2_size,48);
				if (name2_size > 40) name2_size = 40;
#endif
#ifdef USE_INDIA_TAX
				plu_get_field(plu_key-1,PTYPE_PLU_NAME3,(HUGEDATA INT8U*)plu_name3,(INT16S*)&name3_size,48);
				if (name3_size > 40) name3_size = 40;
#endif
				plu_get_field(plu_key-1, PTYPE_TRACE_NO, (INT8U *)&trace_no, (INT16S *)&sz, sizeof(trace_no));
				w_decimal=get_decimal_pt(4, plu_key);
			}
			break;
	}
#endif	//USE_AU_TICKET_DEFAULT

#ifdef USE_DOUBLE_PRINT
		if(cp->delete) 
		{
			if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)	//Double Print 모드 설정 시 PLU 출력 후 ThrougLine 출력
			{
				RcptCmd_PrintPosition(0);
				RcptCmd_TypeThroughLine(strlen(string_buf));
			}
		}
#endif
	string_buf[name_size] = 0;
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[1], TKT_LAY_N, 0, RIGHT);
	RcptCmd_PrintMultiLine(string_buf, name_size, 1);	
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[1], TKT_LAY_0 , 0, RIGHT);
	RcptCmd_CharTab(TICKET_MARGIN);
#else
  #ifdef USE_ARAB_FONT
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
  #endif
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[0], TKT_LAY_N, 0, LEFT);
	
	RcptCmd_CharTab(1);
  #ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		RcptCmd_PrintPosition(0);
  #endif//USE_DOUBLE_PRINT
	RcptCmd_PrintMultiLine(string_buf, name_size, 1);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, GlbFloat.printFontSize[1], TKT_LAY_0 , 0, LEFT);
#endif
#ifdef USE_PLUNAME2_IN_TICKET
	if(plu_name2[0]!=0){
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		RcptCmd_PrintMultiLine(plu_name2, name2_size, 1);
	}
#endif

	RcptCmd_PutStringInit(string_buf);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	if(cp->delete) {
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		{	
			RcptCmd_PrintPosition(0);
			RcptCmd_TypeThroughLine(33);	//58mm paper max: 33
		}
		else
			RcptCmd_TypeThroughLine(0);
#else
		RcptCmd_TypeThroughLine();
#endif//USE_DOUBLE_PRINT
	}
#ifdef USE_ARAB_TICKET_PRT
  #ifndef USE_PERSIA_TICKET_PRT_FORMAT
	// Print Total Price
	if( (cp->content.trtStatus & FLAG_TRANS_NEGATIVE) || (cp->content.trtStatus & FLAG_TRANS_RETURN) ) //SG070214
	{
		temp_totalprice = -(cp->content.trtVolume + cp->content.trtDiscountVolume);
	} 
	else 
	{
		temp_totalprice = cp->content.trtVolume + cp->content.trtDiscountVolume;
	}

	format_value((HUGEDATA INT8U *)temp_string, temp_totalprice, decimal, dec_ch);
	RcptCmd_PutString(0, string_buf, temp_string, LEFT);
  #endif
	if(cp->content.trtPLUtype == PLU_BY_WEIGHT) {
		// Weight
		format_value(( INT8U *)temp_string, (cp->content.trtWeight), w_decimal, dec_ch);
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		// Unit Price
		format_value(( INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
		RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	} else if(cp->content.trtPLUtype == PLU_BY_PCS) {
		// Quantity
		sprintf(temp_string, "%d  %c", (cp->content.trtQty), 0xD7); 
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		// Unit Price
		format_value(( INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
		length = strlen(temp_string);
		sprintf(temp_string+length, "/%d",cp->content.trtPCS);
		RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	} else {
        // Quantity
		sprintf(temp_string, "%d  %c", (cp->content.trtQty), 0xD7); // 알파벳 x 사용할 경우, 이란어 변환 규칙에 따라 위치가 변경되어 이란 문자로 대체
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
		// Unit Price
		format_value(( INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
		RcptCmd_PutString(1, string_buf, temp_string, LEFT);
	}
	barid = 0;
	if (plu_key) {
		barid = prt_get_plu_barid(cp->content.trtPLU, plu_key, cp->content.trtPLUtype, 1);	// Modified by CJK 20050503
	}
  
    // Print Total Price
	if( (cp->content.trtStatus & FLAG_TRANS_NEGATIVE) || (cp->content.trtStatus & FLAG_TRANS_RETURN) ) //SG070214
	{
		temp_totalprice = -(cp->content.trtVolume + cp->content.trtDiscountVolume);
	}
	else
	{
		temp_totalprice = cp->content.trtVolume;
	}

	format_value(( INT8U *)temp_string, temp_totalprice, decimal, dec_ch);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
#else	//USE_PERSIA_TICKET_PRT
	if(cp->content.trtPLUtype == PLU_BY_WEIGHT) {
#if defined (USE_INDIA_TAX) || defined (USE_INDIA_DADUS)
		// Unit Price
		format_value((HUGEDATA INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
		// Weight
		format_value((HUGEDATA INT8U *)temp_string, (cp->content.trtWeight), w_decimal, dec_ch);
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
#else
		// Weight
#ifdef USE_GERMANY_TICKET_DISCOUNT		
		format_value((HUGEDATA INT8U *)temp_string, (cp->content.trtWeight + cp->content.trtDiscountSavedvalue), w_decimal, dec_ch);
#else
		format_value((HUGEDATA INT8U *)temp_string, (cp->content.trtWeight), w_decimal, dec_ch);
#endif
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
		// Unit Price
		format_value((HUGEDATA INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
#endif // USE_INDIA_TAX
	} else if(cp->content.trtPLUtype == PLU_BY_PCS) {
#if defined (USE_INDIA_TAX) || defined (USE_INDIA_DADUS)
		// Unit Price
		format_value((HUGEDATA INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
		length = strlen(temp_string);
		sprintf(temp_string+length, "/%d",cp->content.trtPCS);
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
		// Quantity
		sprintf(temp_string, "x  %d", (cp->content.trtQty));
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
#else
		// Quantity
#ifdef USE_GERMANY_TICKET_DISCOUNT				
		sprintf(temp_string, "%d  x", (cp->content.trtQty+ cp->content.trtDiscountSavedvalue));
#elif defined (USE_AU_TICKET_DEFAULT)
		sprintf(temp_string, "Quantity:%d", cp->content.trtQty);
#else
		sprintf(temp_string, "%d  x", (cp->content.trtQty));
#endif
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
		// Unit Price
#ifdef	USE_AU_TICKET_DEFAULT
		PrtMakePriceString(temp_string, cp->content.trtUnitPrice, decimal, dec_ch, 0);
		length = strlen(temp_string);
		sprintf(temp_string+length, "/%dEA",cp->content.trtPCS);
#else
		format_value((HUGEDATA INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
		length = strlen(temp_string);
		sprintf(temp_string+length, "/%d",cp->content.trtPCS);
#endif
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
#endif // USE_INDIA_TAX
	} else {
#if defined (USE_INDIA_TAX) || defined (USE_INDIA_DADUS)
		// Unit Price
		format_value((HUGEDATA INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
		// Quantity
		sprintf(temp_string, "x  %d", (cp->content.trtQty));
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
#else
		// Quantity
#ifdef USE_GERMANY_TICKET_DISCOUNT				
		sprintf(temp_string, "%d  x", (cp->content.trtQty + cp->content.trtDiscountSavedvalue));
#elif defined (USE_AU_TICKET_DEFAULT)
		sprintf(temp_string, "Quantity:%d", cp->content.trtQty);
#else
		sprintf(temp_string, "%d  x", (cp->content.trtQty));
#endif
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
		// Unit Price
#ifdef	USE_AU_TICKET_DEFAULT
		PrtMakePriceString(temp_string, cp->content.trtUnitPrice, decimal, dec_ch, 0);
		length = strlen(temp_string);
		sprintf(temp_string+length, "/EA");
#else
		format_value((HUGEDATA INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
#endif
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
#endif // USE_INDIA_TAX
	}
	barid = 0;
	if (plu_key) {
		barid = prt_get_plu_barid(cp->content.trtPLU, plu_key, cp->content.trtPLUtype, 1);	// Modified by CJK 20050503
	}
	// Print Total Price
//	if(cp->content.trtStatus & FLAG_TRANS_NEGATIVE) 
	if( (cp->content.trtStatus & FLAG_TRANS_NEGATIVE) || (cp->content.trtStatus & FLAG_TRANS_RETURN) ) //SG070214
	{
		temp_totalprice = -(cp->content.trtVolume + cp->content.trtDiscountVolume);		
	} 
	else 
	{
		temp_totalprice = cp->content.trtVolume + cp->content.trtDiscountVolume;		
	}

	format_value((HUGEDATA INT8U *)temp_string, temp_totalprice, decimal, dec_ch);
	RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
#endif	//USE_ARAB_CONVERT

#ifdef USE_TAX_RATE
#ifndef USE_INDIA_TAX
#ifdef USE_AU_TICKET_DEFAULT
	temp_string[0] = 0;
#else
	if(GlbFloat.printTaxInfo && cp->content.trtTax) {
		sprintf(temp_string, "%c", cp->content.trtTaxNumber+'A'-1);
	} else {
		temp_string[0] = 0;
	}
#endif
#else
	temp_string[0] = 0;
#endif
#ifdef USE_ARAB_TICKET_PRT
	RcptCmd_PutString(3, string_buf, temp_string, LEFT);
#else
	RcptCmd_PutString(3, string_buf, temp_string, RIGHT);
#endif
#endif // USE_TAX_RATE

#ifdef USE_DOUBLE_PRINT
	if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
	{
		RcptCmd_PrintPosition(0);
		PutDataTxBuf(&CommBuf, (char *)string_buf, strlen(string_buf));
	}
	else
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
#else
	PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
#endif//USE_DOUBLE_PRINT
	RcptCmd_CharLF(1);

#ifdef USE_INDIA_TAX
	if(enable_gst)
	{
		temp_string[0] = 0;
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		sprintf(temp_string,"%s", plu_name3);
		RcptCmd_PutString(0, string_buf, temp_string, LEFT);
	
		addr = DFLASH_BASE + FLASH_TAX_AREA;
		flash_addr = addr + (TAX_STRUCT_SIZE * (INT32U)(cp->content.trtTaxNumber -1));
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&tax_str, sizeof(TAX_STRUCT));

		if(cp->content.trtTaxNumber == 0 || tax_str.tax_rate == 0)
			No_taxable_total += temp_totalprice;
	
			halfGST = (tax_str.tax_rate/100.0)*0.5;

		if(cp->content.trtTaxNumber == 0) halfGST = 0;
			sprintf(temp_string, "%5.2f%%", halfGST);
			RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
			sprintf(temp_string, "%5.2f%%", halfGST);
			RcptCmd_PutString(2, string_buf, temp_string, RIGHT);

			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharLF(1);
	}
#endif

#ifdef USE_TARE_IN_TICKET		
	if((cp->content.trtPLUtype == PLU_BY_WEIGHT) && (cp->content.trtPCS != 0))	//TARE EXISTS
	{
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_DSP_TITLE_TARE, temp_string);
		format_value((HUGEDATA INT8U *)string_buf, cp->content.trtPCS, w_decimal, dec_ch);
		strcat(temp_string,":");
		strcat(temp_string,string_buf);
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
	}
#endif

	if(cp->content.trtDiscountVolume)
	{
#ifdef USE_GERMANY_TICKET_DISCOUNT							
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_0, 0, LEFT);

		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		receipt_caption_read(RCPTCAP_ID_TKT_SAVED_PRICE, temp_string);
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);		
		temp_string[0] = 0;
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_0, 0, LEFT);

		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		if(cp->content.trtPLUtype == PLU_BY_WEIGHT) {
			// Weight
			format_value((HUGEDATA INT8U *)temp_string, -(cp->content.trtDiscountSavedvalue), w_decimal, dec_ch);
			RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
			// Unit Price
			format_value((HUGEDATA INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
			RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
		} else if(cp->content.trtPLUtype == PLU_BY_PCS) {
			// Quantity
			sprintf(temp_string, "%d  x", -(cp->content.trtDiscountSavedvalue));
			RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
			// Unit Price
			format_value((HUGEDATA INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
			length = strlen(temp_string);
			sprintf(temp_string+length, "/%d",cp->content.trtPCS);
			RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
		} else {
			// Quantity
			sprintf(temp_string, "%d  x", -(cp->content.trtDiscountSavedvalue));
			RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
			// Unit Price
			format_value((HUGEDATA INT8U *)temp_string, cp->content.trtUnitPrice, decimal, dec_ch);
			RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
		}
		// Print Total Price
		if( (cp->content.trtStatus & FLAG_TRANS_NEGATIVE) || (cp->content.trtStatus & FLAG_TRANS_RETURN) ) //SG070214
		{
			temp_totalprice = cp->content.trtDiscountVolume;		
		} 
		else 
		{
			temp_totalprice = -(cp->content.trtDiscountVolume);
		}
		format_value((HUGEDATA INT8U *)temp_string, temp_totalprice, decimal, dec_ch);
		RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
		temp_string[0] = 0;
		RcptCmd_PutString(3, string_buf, temp_string, RIGHT);		
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);
#elif defined(USE_ARAB_TICKET_PRT)
        RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, RIGHT);
		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN);
        
		receipt_caption_read(RCPTCAP_ID_TKT_SAVED_PRICE, string_buf);        
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));

        // Weight
        RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_0, 0, RIGHT);
        RcptCmd_PutStringInit(string_buf);
	    RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		temp_string[0] = 0;
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
        
 
//Modified by JJANG 20080104 RETURN시 '-'처리
		if((cp->content.trtStatus & FLAG_TRANS_NEGATIVE) || (cp->content.trtStatus & FLAG_TRANS_RETURN))
		{
			format_value(( INT8U *)temp_string, -(cp->content.trtDiscountVolume), decimal, dec_ch);
		}
		else format_value(( INT8U *)temp_string, cp->content.trtDiscountVolume, decimal, dec_ch);
        
		RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
 
#ifdef USE_DOUBLE_PRINT
 		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		{
			RcptCmd_PrintPosition(0);	
			PutDataTxBuf(&CommBuf,(char *)string_buf,strlen(string_buf));	
		}
		else
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
#else
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
#endif
 		RcptCmd_CharLF(1);
//Added by JJANG 20080104
		if(Discount_flag&0x01) //PARAMETER 621 : YES, "YOU PAY" 메세지 출력
		{
			RcptCmd_PutStringInit(string_buf);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
			receipt_caption_read(RCPTCAP_ID_TKT_YOU_PAY, temp_string);
			RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
 //Modified by JJANG 20080104 '-'처리
			if((cp->content.trtStatus & FLAG_TRANS_NEGATIVE) || (cp->content.trtStatus & FLAG_TRANS_RETURN))
			{
				format_value(( INT8U *)temp_string, -(cp->content.trtVolume), decimal, dec_ch);
			}
			else format_value(( INT8U *)temp_string, cp->content.trtVolume, decimal, dec_ch);
			RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
			temp_string[0] = 0;
			RcptCmd_PutString(3, string_buf, temp_string, RIGHT);
 			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharLF(1);
		}
#else	//USE_GERMANY_TICKET_DISCOUNT
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_0, 0, LEFT);
		RcptCmd_PutStringInit(string_buf);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		temp_string[0] = 0;
		RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
		receipt_caption_read(RCPTCAP_ID_TKT_SAVED_PRICE, temp_string);
		RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
//Modified by JJANG 20080104 RETURN시 '-'처리
		if((cp->content.trtStatus & FLAG_TRANS_NEGATIVE) || (cp->content.trtStatus & FLAG_TRANS_RETURN))
		{
			format_value((HUGEDATA INT8U *)temp_string, -(cp->content.trtDiscountVolume), decimal, dec_ch);
		}
		else format_value((HUGEDATA INT8U *)temp_string, cp->content.trtDiscountVolume, decimal, dec_ch);
		RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
		temp_string[0] = 0;
		RcptCmd_PutString(3, string_buf, temp_string, RIGHT);
 #ifdef USE_DOUBLE_PRINT
 		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		{
			RcptCmd_PrintPosition(0);	
			PutDataTxBuf(&CommBuf,(char *)string_buf,strlen(string_buf));	
		}
		else
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
 #else
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
 #endif//USE_DOUBLE_PRINT
		RcptCmd_CharLF(1);
//Added by JJANG 20080104
		if(Discount_flag&0x01) //PARAMETER 621 : YES, "YOU PAY" 메세지 출력
		{		 
			RcptCmd_PutStringInit(string_buf);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			RcptCmd_PutString(0, string_buf, temp_string, RIGHT);
			receipt_caption_read(RCPTCAP_ID_TKT_YOU_PAY, temp_string);
			RcptCmd_PutString(1, string_buf, temp_string, RIGHT);
//Modified by JJANG 20080104 '-'처리
			if((cp->content.trtStatus & FLAG_TRANS_NEGATIVE) || (cp->content.trtStatus & FLAG_TRANS_RETURN))
			{
				format_value((HUGEDATA INT8U *)temp_string, -(cp->content.trtVolume), decimal, dec_ch);
			}
			else format_value((HUGEDATA INT8U *)temp_string, cp->content.trtVolume, decimal, dec_ch);
			RcptCmd_PutString(2, string_buf, temp_string, RIGHT);
			temp_string[0] = 0;
			RcptCmd_PutString(3, string_buf, temp_string, RIGHT);
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharLF(1);
		}
#endif		
	}

	RcptCmd_TypeNone();

#ifdef USE_TRACEABILITY //gm 090407
	// Traceability
	if(get_global_bparam(GLOBAL_PRINT_CLERK_FLAG2)&0x04) 	//menu 1762, prt. traceability(Y/N)
	{
		switch(cp->content.trtPLU) 
		{
			case MISC_PLU_WEIGHT :
			case MISC_PLU_PCS :
			case MISC_PLU_COUNT :
				//Do not print traceability
				break;
			default :
				TotalRcpt_PrintTraceability(trace_no);
				break;
		}
	}
#endif
	// Barcode
	if(GlbFloat.printBarcodeForList) {
		RcptCmd_TypeNone();
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, CENTER);
		RcptCmd_BarHeight(64, 56, 16);
		RcptCmd_BarWidth(2);
		RcptCmd_BarGuard(1);
#ifdef USE_DOUBLE_PRINT
		if(get_global_bparam(GLOBAL_SALERUN_MODE) == 8)
		{
			RcptCmd_BarHRI(HRI_POS_DOWN);
			RcptCmd_BarFont(HRI_FONT_B);
		}
#endif
		prt_set_strform_parameter_item(&bar_param, cp);
		prt_make_barcode(RECEIPT, barid, &bar_param, 0);
		RcptCmd_CharLF(1);
	}
// GenerateQrTotalStringbyTransaction() 함수로 이동
//#ifdef USE_QR_ADDTOTAL
//	else if(QrAddTotalFlag) // 티켓모드에서 QR ADD Total(Parmeter726)이 활성화 되어 있을때 판매 정보를 저장하는 부분
//	{
//		if(!cp->delete)
//		{
//			prt_set_strform_parameter_item(&bar_param, cp);
//			prt_make_barcode(RECEIPT, barid, &bar_param, 0);
//		}
//	}
//#endif

	PrintListWaitEnd();

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

extern INT32U ClerkLastHead;

INT16S ReadPluTransactionCount(INT16U deptno, INT32U pluno)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	if(GlbOper.PluReportType) return 0; 
	tp = FindPluSummary(0, pluno, deptno, 0, &index);
	if(tp == NULL) return 0;

	return tp->pstCountNumber;
}

extern INT16S ClearPluCounter(PLU_SALES_SUMMARY_TYPE *tp);

void ClearPluTransactionCount(INT16U deptno, INT32U pluno)
{
	PLU_SALES_SUMMARY_TYPE* tp;
	INT16U index;

	tp = FindPluSummary(0, pluno, deptno, 0, &index);
	if(tp == NULL) return;

	ClearPluCounter(tp);
	set_nvram_plu_summary(NVRAM_PLU_SUMMARY, index, tp);
}

void ClearPluTransactionCountAll(void)
{
	PLU_SALES_SUMMARY_TYPE pst;
	INT16U i;

	for (i=0;i< MAX_PLU_REPORT_NO;i++)
	{
		//tp = (PLU_SALES_SUMMARY_TYPE *)get_nvram_plu_summary(NVRAM_PLU_SUMMARY, i);
		//ClearPluCounter(tp);
		get_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, &pst);
		ClearPluCounter(&pst);
		set_nvram_plu_summary(NVRAM_PLU_SUMMARY, i, &pst);
	}


}

//Added by JJANG 20080123 파라미터 세팅 변경되었을 때 초기화 루틴
extern void global_load_ext_nvram(void);
void ClearTransactionHeadTail(void)
{
	set_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD,0);
	set_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL,0);

	set_nvram_wparam(NVRAM_TOTAL_SEND_SALE,0);
	set_nvram_wparam(NVRAM_TOTAL_SEND_CHEANGE,0);

	set_nvram_bparam(NVRAM_TRANS_PAY_HEAD,0);
	set_nvram_bparam(NVRAM_TRANS_PAY_TAIL,0);

	set_nvram_wparam(NVRAM_REOPEN_HEAD,0);

	global_load_ext_nvram();	//Added by JJANG 20080312
}

//Added by JJANG 20080313
//count = 미전송된 개수 
INT16U GetTransactioncount(INT8U mode)	//0 = sale, 1 = pay
{
	INT16U  head=0, tail=0;
	INT16U	count=0;
	if(!mode)
	{
		head=get_nvram_wparam(NVRAM_SEND_REMAINDED_HEAD);
		tail=get_nvram_wparam(NVRAM_SEND_REMAINDED_TAIL);
	}
	else
	{
		head=get_nvram_wparam(NVRAM_TRANS_PAY_HEAD);
		tail=get_nvram_wparam(NVRAM_TRANS_PAY_TAIL);
	}
	if(head>=tail) count = head - tail;
	else count = (Startup.max_send_buf + head) - tail;
	return count;
}


//현재 판매한 데이터를 버퍼에 넣을 수 있는 지 체크
//mode (0:sale, 1:pay)
//bufdata_count : 버퍼에 넣을 data 개수
//buf_type (0:linear, 1:circular)
//   linear(0) : buffer가 차면 판매 금지
//   circular(1) : buffer가 차면, 오래된 데이터를 덮어씌움
extern void display_message_page(char *str);//
INT8U GetCheckAddTransactionData(INT8U mode, INT16U bufdata_count, INT8U buf_type)
{
	INT16U count=0;
	INT16U max_count=0;

	if(buf_type) return 1;

	count =  GetTransactioncount(mode);
	max_count = (Startup.max_send_buf-1) - count;		// max_count = 남은 버퍼의 개수

	if (max_count >= bufdata_count) return 1;		//OK
	else
	{
		display_message_page(global_message[MSG_MEMORY_FULL]);
		return 0;
	}
}

//Added by JJANG 20080416
//1. 최종 할인 된 가격이 있을 때, 전체 금액에서 할인 금액을 빼준다.
//2. report에 사용되는 discount 항목의 count값과 data값 갱신
void Get_SalesSummaryRenewal(INT8U count, long discount_price, INT16U clerk_id, INT16U scale_id, INT8U reopenFlag)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	INT8U i;

	sr = &sst;

	if (scale_id > MAX_SCALE - 1) scale_id = MAX_SCALE - 1;
    
	if(!discount_price) return;
	for (i = 0; i < MAX_REPORT_PERIOD; i++)		//X1, X2
	{
		if(reopenFlag)
		{
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, clerk_id);
			get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, clerk_id, sr);
			sr->sstVolume[i] += discount_price;
			set_nvram_summary_addr(NVRAM_CLERK_SUMMARY, clerk_id, sr);

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, scale_id);
			get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, scale_id, sr);
			sr->sstVolume[i] += discount_price;
			set_nvram_summary_addr(NVRAM_SCALE_SUMMARY, scale_id, sr);

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, scale_id);
			get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, scale_id, sr);
			sr->sstTransactions[i] -= count;
			sr->sstVolume[i] -= discount_price;
			set_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, scale_id, sr);

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id);
			get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id, sr);
			sr->sstTransactions[i] -= count;
			sr->sstVolume[i] -= discount_price;
			get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id, sr);
		}
		else
		{
			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, clerk_id);
			get_nvram_summary_addr(NVRAM_CLERK_SUMMARY, clerk_id, sr);
			sr->sstVolume[i] -= discount_price;
			set_nvram_summary_addr(NVRAM_CLERK_SUMMARY, clerk_id, sr);

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, scale_id);
			get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, scale_id, sr);
			sr->sstVolume[i] -= discount_price;
			set_nvram_summary_addr(NVRAM_SCALE_SUMMARY, scale_id, sr);

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, scale_id);
			get_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, scale_id, sr);
			sr->sstTransactions[i] += count;
			sr->sstVolume[i] += discount_price;
			set_nvram_summary_addr(NVRAM_DISCOUNT_SCALE, scale_id, sr);

			//sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id);
			get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id, sr);
			sr->sstTransactions[i] += count;
			sr->sstVolume[i] += discount_price;
			set_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id, sr);
		}
	}
}
#ifdef USE_TAX_RATE
void Get_PLUSaleSummaryRenewal(INT8U count, float discountRate, INT16U clerk_id, INT16U scale_id, INT8U reopenFlag)
{
	SALES_SUMMARY_TYPE *sr;
	SALES_SUMMARY_TYPE sst;
	INT8U i;
	TRANSACT_CHAIN  *cp;
	TRANSACT_CHAIN  tc;
	PLU_SALES_SUMMARY_TYPE* tp1;
	PLU_SALES_SUMMARY_TYPE* plu_summary;
	INT16U h_index, index, next;
	long discountVolume;
	long discountTax;
	INT16U Hour;
	
	sr = &sst;
	
	if (scale_id > MAX_SCALE - 1) scale_id = MAX_SCALE - 1;

//	cp = (TRANSACT_CHAIN *)get_nvram_lparam(NVRAM_TRANS_HEAD+clerk_id*4);
	h_index = get_nvram_lparam(NVRAM_TRANS_HEAD+clerk_id*4);
	if (h_index)
	{
		next = h_index;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, h_index - 1, &tc);
		cp = &tc;
	}
	else
	{
		cp = 0;
	}
	if(!cp) return;
	for(;cp;)
	{
		if(cp)
		{
			discountVolume = cp->content.trtVolume * discountRate + 0.5;
			discountTax = cp->content.trtTax * discountRate + 0.5;
			cp->content.trtDiscountVolume = discountVolume;
			cp->content.trtTax = discountTax;
		}
		if(discountRate == 1)
		{
			for (i = 0; i < 2; i++)
			{
				Hour= bcd2hex(RTCStruct.hour); 
//				sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour);
				get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour, sr);
				if(reopenFlag)
					sr->sstVolume[i] -= cp->content.trtTax;
				else
					sr->sstVolume[i] += cp->content.trtTax;
				set_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour, sr);
			}
		}
		else
		{
			for (i = 0; i < 2; i++)
			{
				if(reopenFlag)
				{
					tp1= FindPluSummary(1,cp->content.trtPLU,cp->content.trtDept,cp->content.trtPLUtype,&index);
					tp1->pstVolume[i] += cp->content.trtDiscountVolume;
					tp1->pstTax[i] += cp->content.trtTax;

//					sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, cp->content.trtGroup);
					get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, cp->content.trtGroup, sr);
					sr->sstVolume[i] += discountVolume;
					set_nvram_summary_addr(NVRAM_GROUP_SUMMARY, cp->content.trtGroup, sr);

//					sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, cp->content.trtDept);
					get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, cp->content.trtDept, sr);
					sr->sstVolume[i] += discountVolume;
					set_nvram_summary_addr(NVRAM_DEPT_SUMMARY, cp->content.trtDept, sr);

					Hour= bcd2hex(RTCStruct.hour); 
//					sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour);
					get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour, sr);
					sr->sstVolume[i] += discountVolume;
					sr->sstVolume[i] -= discountTax;
					set_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour, sr);

//					sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id);
					get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id, sr);
					sr->sstVolume[i] -= discountTax;
					set_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id, sr);
				}
				else
				{
					tp1= FindPluSummary(1,cp->content.trtPLU,cp->content.trtDept,cp->content.trtPLUtype,&index);
					tp1->pstVolume[i] -= cp->content.trtDiscountVolume;
					tp1->pstTax[i] -= cp->content.trtTax;

//					sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, cp->content.trtGroup);
					get_nvram_summary_addr(NVRAM_GROUP_SUMMARY, cp->content.trtGroup, sr);
					sr->sstVolume[i] -= discountVolume;
					set_nvram_summary_addr(NVRAM_GROUP_SUMMARY, cp->content.trtGroup, sr);

//					sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, cp->content.trtDept);
					get_nvram_summary_addr(NVRAM_DEPT_SUMMARY, cp->content.trtDept, sr);
					sr->sstVolume[i] -= discountVolume;
					set_nvram_summary_addr(NVRAM_DEPT_SUMMARY, cp->content.trtDept, sr);

					Hour= bcd2hex(RTCStruct.hour); 
//					sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour);
					get_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour, sr);
					sr->sstVolume[i] -= discountVolume;
					sr->sstVolume[i] += discountTax;
					set_nvram_summary_addr(NVRAM_HOURLY_SUMMARY, Hour, sr);

//					sr = (SALES_SUMMARY_TYPE *)get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id);
					get_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id, sr);
					sr->sstVolume[i] += discountTax;
					set_nvram_summary_addr(NVRAM_DISCOUNT_CLERK, clerk_id, sr);
				}
			}
		}
		if (!(cp->next)) break;
		next = cp->next;
		get_nvram_trans_addr(NVRAM_TRANS_DATA, next - 1, &tc);
		cp = &tc;
//		cp = cp->next;
	}
}
#endif // USE_TAX_RATE

#ifdef USE_PERSIAN_SALE_AMOUNT_LIMIT
INT8U sale_check_sale_total_amount(long total_price)
{
	INT16S i;
	INT8U error = 0;
	long total_sale_price;
	SALES_SUMMARY_TYPE *tp;
    SALES_SUMMARY_TYPE sst;

    tp = &sst;

    for (i=0;i<MAX_REPORT_PERIOD;i++)
	{
		total_sale_price = 0;
		get_nvram_summary_addr(NVRAM_SCALE_SUMMARY, status_scale.scaleid, tp);
		total_sale_price += tp->sstVolume[i];

		if(( total_sale_price + total_price ) > MAX_SALES_VOLUME ) // OVER THAN MAX SALES VOLUME
			error = 1;// sale error
		else if((total_sale_price > 0 && total_price > 0) && (total_sale_price + total_price < 0)) // OVERFLOW
			error = 1;// sale error
	}
	if(error)
	{
		error_display_caption(0xe201);//"Check sales summary overflow"
		return error;
	}

	return 0; // no error
}
#endif