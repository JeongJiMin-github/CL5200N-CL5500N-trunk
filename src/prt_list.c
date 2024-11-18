/*****************************************************************************
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	prt_list.c
|*  Version		:	0.1
|*  Modified		:	2004/05/27
|*  Modifier		:	CJK
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "initial.h"
#include "commbuf.h"
#include "prt_common.h"
#include "bar_common.h"
#include "prtdrv.h"
#include "prt_line.h"
#include "prt_font.h"
#include "prt_cmdtest.h"
#include "main.h"
#include "mode_main.h"
#include "mode_commun.h"
#include "mode_sale.h"
#include "flash.h"
#include "flash_app.h"
#include "common.h"
#include "caption_app.h"
#include "common_sunday.h"
#include "plu_manager.h"
#include "rtc.h"
#include "prt_font.h"
#include "plu_auxcall.h"

#include "plu_manager.h"
#include "tax.h"
#include "rtc.h"
#include "rtc_app.h"
#include "common_sunday.h"
#include "origin.h"
#include "prt_advertise.h"
#include "shopname.h"
#include "clerk_app.h"
#include "customer_app.h"
#include "ingredient.h"
#include "tare_file.h"
#include "pcs_unit.h"
#include "department.h"
#include "pcs_unit.h"
#include "traceability.h"
#include "workplace.h"
#include "label_caption.h"
#include "adm.h"
#include "adm_app.h"
#include "nutrition_fact.h"
#include "menu.h"
#include "plu_device.h"
#include "prt_app.h"
#include "prt_cmd.h"

#include "dsp_common.h"
#include "key.h"
#include "key_typedef.h"
#include "pgm_app.h"
#include "key_app.h"

#include "barcode_file.h"
#include "discount.h"
#include "menu_code_text1.h"
#include "mode_calibration.h"
#include "initial_glb.h"
#include "sensor.h"
#include "currency.h"
#include "prt_list.h"
#include "dsp_app.h"

extern void PrintFixedTime(RTC_STRUCT *time_str);
extern char barcodetype[][20];
extern INT32U ing_calc_addr_by_key(INT16U ingid);

#define PRT_LIST_TITLE_LENGTH	14
void prtlist_captitle_adjust(char *string_buf, INT32U cap_addr)
{
	//INT8U kor_flag, i;
	INT8U kor_flag, i;
	INT8U *pcap_addr;
	INT8U cap_buf[PRT_LIST_TITLE_LENGTH+1];

	Flash_sread(cap_addr, cap_buf, PRT_LIST_TITLE_LENGTH);
	pcap_addr = cap_buf;
	//prt_len = PRT_LIST_TITLE_LENGTH;
	kor_flag = 0;
	//_memcpy((INT8U*)string_buf,(INT8U *)cap_addr, 32);
	for(i = 0; i < PRT_LIST_TITLE_LENGTH; i++) {
		if(DspStruct.StrType == TWO_BYTE_FONT_STR) {	// Added by CJK 20060223
			if(i < PRT_LIST_TITLE_LENGTH-3) {
				if(kor_flag == 0) {
					if(pcap_addr[i] >= 0x80) kor_flag = 1;		// 1byte font에 대한 검토필요
				} else {
					kor_flag = 0;
				}
			}
		}
		string_buf[i] = pcap_addr[i];
		if(pcap_addr[i] == 0) return;
		
	}
//	if(i == PRT_LIST_TITLE_LENGTH && pcap_addr[PRT_LIST_TITLE_LENGTH] != 0) {
	if(i == PRT_LIST_TITLE_LENGTH && pcap_addr[PRT_LIST_TITLE_LENGTH-1] != 0) {
		if(DspStruct.StrType == TWO_BYTE_FONT_STR) {	// Added by CJK 20060223
			if(kor_flag == 1) string_buf[PRT_LIST_TITLE_LENGTH-4] = ' ';
		}
		string_buf[PRT_LIST_TITLE_LENGTH-3] = '.';
		string_buf[PRT_LIST_TITLE_LENGTH-2] = '.';
		string_buf[PRT_LIST_TITLE_LENGTH-1] = ':';
		string_buf[PRT_LIST_TITLE_LENGTH] = 0;
	}
	string_buf[PRT_LIST_TITLE_LENGTH] = 0;
}

#ifndef DISABLE_LIST_MENU
INT8U barcodefile_print_list(INT16U start_id, INT16U end_id)
{
	BARCODE_STRUCT barcode_str;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32U cap_addr_type, cap_addr_format;

	base_addr = DFLASH_BASE + FLASH_BARCODE_FORMAT_AREA;
	//if(end_id > 999) return FALSE;
	cap_addr_type   = caption_search_by_code(0x126c) + 2;
	cap_addr_format = caption_search_by_code(0x126d) + 2;

	if(!Report_ConfirmPrint()  ) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()  ) return OFF;

	PrtSensorFlagInit();
	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);

	caption_split_by_code(0x1262, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				//flagStopPrint = ON;
				break;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*BARCODE_STRUCT_SIZE;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&barcode_str, BARCODE_STRUCT_SIZE);
		if(cur_id != barcode_str.barcode_num) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", barcode_str.barcode_num);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char *)barcode_str.barcode_description, sizeof(barcode_str.barcode_description), TICKET_MARGIN+TICKET_TXT_MARGIN+5);


		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_TYPE, string_buf);
prtlist_captitle_adjust(string_buf, cap_addr_type);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%d:%s", barcode_str.barcode_type, barcodetype[barcode_str.barcode_type]);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);


		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_FORMAT, string_buf);
prtlist_captitle_adjust(string_buf, cap_addr_format);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		memcpy(string_buf, barcode_str.barcode_format, BARCODE_FORMAT_LEN);
		string_buf[BARCODE_FORMAT_LEN] = 0;
		RcptCmd_PrintMultiLine((char*)string_buf, BARCODE_FORMAT_LEN, TICKET_MARGIN+TICKET_TXT_MARGIN+6);

		RcptCmd_CharLF(1);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();
	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
///		LabCmd_FormFeed();
///		LabCmd_FormFeed();
///		PrintListWaitEnd();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

INT8U scanner_convtable_print_list(INT16U start_id, INT16U end_id)
{
	SCANNER_CONV_TABLE_STR  scannerConvTable;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32U cap_addr_type, cap_addr_format;

#ifdef USE_10000_SCANNER_CONVTABLE
	base_addr = EXT_MEMCARD2_ADDR_X + FLASH_SCANNER_TABLE_AREA;
#else
	base_addr = DFLASH_BASE + FLASH_SCANNER_TABLE_AREA;
#endif
	cap_addr_type   = caption_search_by_code(0x146B) + 2;
	cap_addr_format = caption_search_by_code(0x146C) + 2;

	if(!Report_ConfirmPrint()  ) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()  ) return OFF;

	PrtSensorFlagInit();
	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);

	caption_split_by_code(0x1462, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

//	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
//	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
//	len = strlen(string_buf);
//	sprintf(string_buf+len, "%d, ", start_id);
//	len = strlen(string_buf);
//	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
//	len = strlen(string_buf);
//	sprintf(string_buf+len, "%d", end_id);
//	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
//	RcptCmd_CharLF(1);
//
//	RcptCmd_CharTab(TICKET_MARGIN);
//	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*SCANNER_CONV_TABLE_STR_SIZE;
		code = Flash_wread(addr);
		Flash_sread(addr, (HUGEDATA INT8U*)&scannerConvTable, SCANNER_CONV_TABLE_STR_SIZE);
		if(cur_id != scannerConvTable.code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", scannerConvTable.code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_CharTab(3);
		sprintf(string_buf, " %-13s %4d %8ld", scannerConvTable.scanCode,scannerConvTable.dept_no,scannerConvTable.plu_no);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();
	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}



INT8U clerk_print_list(INT16U start_id, INT16U end_id)
{
	CLERK_STRUCT clerk_str;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32U cap_addr_nickname, cap_addr_level;

	base_addr = DFLASH_BASE + FLASH_CLERK_AREA;
	cap_addr_nickname = caption_search_by_code(0x1d03) + 2;
	cap_addr_level    = caption_search_by_code(0x1d05) + 2;

	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "CLERK LIST");
	caption_split_by_code(0x1853, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%03d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%03d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id)*CLERK_STRUCT_SIZE;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&clerk_str, CLERK_STRUCT_SIZE);
		if(cur_id != clerk_str.code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%02d", clerk_str.code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_PrintMultiLine((char *)clerk_str.name, sizeof(clerk_str.name), TICKET_MARGIN+TICKET_TXT_MARGIN+5);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_TKT_NICKNAME, string_buf);
		prtlist_captitle_adjust(string_buf, cap_addr_nickname);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		//RcptCmd_CharTab(6);
		//sprintf(string_buf, "%s", clerk_str.nickname);
		//PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		//RcptCmd_CharLF(1);
		RcptCmd_PrintMultiLine((char*)clerk_str.nickname, sizeof(clerk_str.nickname), TICKET_MARGIN+TICKET_TXT_MARGIN+6);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_LEVEL, string_buf);
		prtlist_captitle_adjust(string_buf, cap_addr_level);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%d", clerk_str.permissionLevel);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);
		RcptCmd_TypeNone();

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}

	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
///		LabCmd_FormFeed();
///		LabCmd_FormFeed();
///		PrintListWaitEnd();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

extern void customer_read(INT16U code, CUSTOMER_STRUCT *custom);
INT8U customer_print_list(INT16U start_id, INT16U end_id)
{
	CUSTOMER_STRUCT custom;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100], temp_string[34];
	INT32U custom_addr, addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len, i;
	INT8U decimal, dec_ch;
	INT32U cap_addr_address[3], cap_addr_tel, cap_addr_credit_lmt;

	custom_addr = DFLASH_BASE + FLASH_CUSTOMER_AREA;
	cap_addr_address[0] = caption_search_by_code(0x3424) + 2;
	cap_addr_address[1] = caption_search_by_code(0x3425) + 2;
	cap_addr_address[2] = caption_search_by_code(0x3426) + 2;
	cap_addr_tel        = caption_search_by_code(0x3427) + 2;
	cap_addr_credit_lmt = caption_search_by_code(0x3428) + 2;

	//if(end_id > 999) return OFF;


    	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "CUSTOMER LIST");
	caption_split_by_code(0x1422, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%03d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%03d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = custom_addr+ (INT32U)cur_id*CUSTOMER_STRUCT_SIZE;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		customer_read(cur_id, &custom);
		if(cur_id != custom.code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//sprintf(string_buf, "No. ");
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%02d", custom.code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char *)custom.name, sizeof(custom.name), TICKET_MARGIN+TICKET_TXT_MARGIN+5);
		RcptCmd_TypeNone();
//		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
//		receipt_caption_read(RCPTCAP_ID_ID, string_buf);
//		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
//		RcptCmd_CharTab(6);

//		sprintf(string_buf, "%s", custom.id);
//		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
//		RcptCmd_CharLF(1);

		//receipt_caption_read(RCPTCAP_ID_LST_ADDRESS, temp_string);
		for(i = 0; i < 3; i++) {
			prtlist_captitle_adjust(temp_string, cap_addr_address[i]);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			sprintf(string_buf, "%s%d", temp_string, i+1);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			if(custom.address[i][0]) {
				RcptCmd_PrintMultiLine((char *)custom.address[i], sizeof(custom.address[i]), TICKET_MARGIN+TICKET_TXT_MARGIN+5);
			} else {
			 	RcptCmd_CharLF(1);
			}
		}

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		//receipt_caption_read(RCPTCAP_ID_LST_TEL, string_buf);
		prtlist_captitle_adjust(string_buf, cap_addr_tel);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%s", custom.tel);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		//receipt_caption_read(RCPTCAP_ID_LST_CREDIT_LMT, string_buf);
		prtlist_captitle_adjust(string_buf, cap_addr_credit_lmt);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		PrtMakePriceString(string_buf, custom.creditlimit, decimal, dec_ch, 8);
		//format_fixed((HUGEDATA INT8U *)string_buf, custom.creditlimit, decimal, 8, dec_ch);
		//format_value((HUGEDATA INT8U *)string_buf, custom.creditlimit, decimal, dec_ch);
		string_buf[10] = 0;

		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();

	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
///		LabCmd_FormFeed();
///		LabCmd_FormFeed();
///		PrintListWaitEnd();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}


INT8U department_print_list(INT16U start_id, INT16U end_id)
{
	DEPARTMENT_STRUCT dept_str;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32U cap_addr_speedkey;

	base_addr = DFLASH_BASE + FLASH_DEPARTMENT_AREA;

	cap_addr_speedkey = caption_search_by_code(0x3161) + 2;
	//if(end_id > 999) return OFF;

  	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	caption_split_by_code(0x1212, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
  	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%03d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%03d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*DEPARTMENT_STRUCT_SIZE;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&dept_str, DEPARTMENT_STRUCT_SIZE);
		if(cur_id != dept_str._code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%02d", dept_str._code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char *)dept_str._description, sizeof(dept_str._description), TICKET_MARGIN+TICKET_TXT_MARGIN+5);

		RcptCmd_TypeNone();

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
//receipt_caption_read(RCPTCAP_ID_LST_SPEED_KEY, string_buf);
//_memcpy((INT8U*)&string_buf,(INT8U *)cap_addr_speedkey, 32);
//string_buf[32] = 0;
prtlist_captitle_adjust(string_buf, cap_addr_speedkey);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		sprintf(string_buf, "%d", dept_str._speedkey);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();

	RcptCmd_ReceiptFeed();	
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
///		LabCmd_FormFeed();
///		LabCmd_FormFeed();
///		PrintListWaitEnd();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

#define DCLIST_NONE	0
#define DCLIST_WEIGHT	1
#define DCLIST_COUNT	2
#define DCLIST_PRICE	3
#define DCLIST_PERCENT	4
extern INT16U max_dc_type;
extern INT16U weight_type[];
extern INT16U pcs_type[];
INT8U discount_print_list(INT16S dept_no, long start_id, long end_id, INT16U *data_pos, INT16U num)
{
	DISCOUNT_STRUCT	dc_str;
	CAPTION_STRUCT cap;
	long cur_id;   
	char string_buf[100];
	INT32U addr;
	INT16U plu_addr;
	INT16U dc_no;
	INT8U m;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT8U w_decimal, decimal, dec_ch;
	INT8U target_type, value_type, plu_type;
//	INT16S p_type_offset;
//	INT8U stype;
	INT16U usize;
	INT32U cap_addr_type, cap_addr_target1, cap_addr_target2, cap_addr_value1, cap_addr_value2;
	INT32U cap_addr_dayweek, cap_addr_s_date, cap_addr_e_date, cap_addr_s_time, cap_addr_e_time;
	INT32U *cap_ptr_dayweek, *cap_ptr_s_date, *cap_ptr_e_date, *cap_ptr_s_time, *cap_ptr_e_time;
	INT8U backup_weightunit, wunit_id, new_wunit_id;

	//w_decimal=get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
	w_decimal=get_decimal_pt(1, 0);
	//w_dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);

	cap_addr_type    = caption_search_by_code(0x1c12) + 2;
	cap_addr_target1 = caption_search_by_code(0x1c44) + 2;
	cap_addr_value1  = caption_search_by_code(0x1c45) + 2;
	cap_addr_target2 = caption_search_by_code(0x1c46) + 2;
	cap_addr_value2  = caption_search_by_code(0x1c47) + 2;
	cap_addr_dayweek = caption_search_by_code(0x1c14) + 2;
	cap_addr_s_date  = caption_search_by_code(0x1c15) + 2;
	cap_addr_e_date  = caption_search_by_code(0x1c17) + 2;
	cap_addr_s_time  = caption_search_by_code(0x1c16) + 2;
	cap_addr_e_time  = caption_search_by_code(0x1c18) + 2;

	cap_ptr_dayweek = &cap_addr_dayweek;
	cap_ptr_s_date = &cap_addr_dayweek;
	cap_ptr_e_date = &cap_addr_dayweek;
	cap_ptr_s_time = &cap_addr_dayweek;
	cap_ptr_e_time = &cap_addr_dayweek;

//	p_type_offset = plu_get_ptype_point(4, &stype, &usize);	//plu type offset

	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	caption_split_by_code(0x1132, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
     	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_DEPT_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d  ", dept_no);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%ld, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%ld", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
     	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	backup_weightunit = CurPluWeightUnit;

	for(cur_id = 0; cur_id < num && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
		dc_no = data_pos[cur_id];
		//if(dc_no) {
			addr = get_addr_discount_area(dc_no);
			if (!addr) { /* 에러 처리 */
				break;
			}
			Flash_sread(addr, (HUGEDATA INT8U*)&dc_str, DISCOUNT_STRUCT_SIZE);
			for(m=0; m<=max_dc_type; m++) {
 				if( dc_str.type == weight_type[m] )
					break;
				if( dc_str.type == pcs_type[m] )
					break;
			}								
//			plu_addr = plu_table_search(dc_str.dept_no, dc_str.plu_no);
			plu_table_search(dc_str.dept_no, dc_str.plu_no,&plu_addr,0);
			if(plu_addr > 0) {
				plu_get_field(plu_addr-1, PTYPE_PLU_TYPE,(INT8U *)&plu_type, (INT16S *)&usize, 1);
				plu_get_field(plu_addr-1, PTYPE_UNIT_WEIGHT,(INT8U *)&wunit_id, (INT16S *)&usize, sizeof(wunit_id));	// UnitWeight
				get_weightunit_and_id_fromid(wunit_id, &new_wunit_id, &CurPluWeightUnit);// 1000,100,50,25
			}
			if (plu_type==0) plu_type=1;
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
			if (plu_type >= PLU_BY_WEIGHT_FIX && plu_type <= PLU_BY_PCS_FIX) {  
				plu_type -= 3;
			}
#endif

			//switch(dc_str.type) {
			switch(m) {
				case 1 :
					if(plu_type == 1) target_type = DCLIST_WEIGHT;
					else		  target_type = DCLIST_COUNT;
					value_type  = DCLIST_PRICE;
					break;
				case 2 :
					if(plu_type == 1) target_type = DCLIST_WEIGHT;
					else		  target_type = DCLIST_COUNT;
					value_type  = DCLIST_PERCENT;
					break;
				case 3 :
					if(plu_type == 1) target_type = DCLIST_WEIGHT;
					else		  target_type = DCLIST_COUNT;
					value_type  = DCLIST_PRICE;
					break;
				case 4 :
					if(plu_type == 1) target_type = DCLIST_WEIGHT;
					else		  target_type = DCLIST_COUNT;
					value_type  = DCLIST_PERCENT;
					break;
				case 5 :
					target_type = DCLIST_NONE;
					value_type  = DCLIST_PRICE;
					break;
				case 6 :
					target_type = DCLIST_NONE;
					value_type  = DCLIST_NONE;
					break;
				case 7 :
					if(plu_type == 1) target_type = DCLIST_WEIGHT;
					else		  target_type = DCLIST_COUNT;
					if(plu_type == 1) value_type  = DCLIST_WEIGHT;
					else		  value_type  = DCLIST_COUNT;
					break;
				case 8 :
					target_type = DCLIST_PRICE;
					value_type  = DCLIST_PRICE;
					break;
				case 9 :
					target_type = DCLIST_PRICE;
					value_type  = DCLIST_PERCENT;
					break;
				case 10 :
					target_type = DCLIST_PRICE;
					value_type  = DCLIST_PRICE;
					break;
				case 11 :
					target_type = DCLIST_PRICE;
					if(plu_type == 1) value_type  = DCLIST_WEIGHT;
					else		  value_type  = DCLIST_COUNT;
					break;
			}


			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			receipt_caption_read(RCPTCAP_ID_LST_DEPT_NO, string_buf);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			sprintf(string_buf, "%d, ", dc_str.dept_no);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			receipt_caption_read(RCPTCAP_ID_LST_PLU_NO, string_buf);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			sprintf(string_buf, "%ld", dc_str.plu_no);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			////receipt_caption_read(RCPTCAP_ID_LST_TYPE, string_buf);
			//_memcpy((INT8U*)&string_buf,(INT8U *)cap_addr_type, 32);
			//string_buf[32] = 0;
			prtlist_captitle_adjust(string_buf, cap_addr_type);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharTab(7);
			caption_split_by_code(0x1B50+m, &cap,0);
			sprintf(string_buf, "%s", &cap.form[0]);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			// 1st Target/Price
			if(target_type != DCLIST_NONE) {
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
				////sprintf(string_buf, "1st Target");
				//_memcpy((INT8U*)&string_buf,(INT8U *)cap_addr_target1, 32);
				//string_buf[32] = 0;
				prtlist_captitle_adjust(string_buf, cap_addr_target1);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharTab(7);
				switch(target_type) {
					case DCLIST_WEIGHT :
						PrtMakeWeightString(string_buf, dc_str.target[0], w_decimal, dec_ch, 0);
						break;
					case DCLIST_PRICE :
						PrtMakePriceString(string_buf, dc_str.target[0], decimal, dec_ch, 0);
						break;
					case DCLIST_COUNT :
						sprintf(string_buf, "%ld", dc_str.target[0]);
						break;
				}
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharLF(1);
			}

			if(value_type != DCLIST_NONE) {
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
				////receipt_caption_read(RCPTCAP_ID_LST_TYPE, string_buf);
				////sprintf(string_buf, "1st Value");
				//_memcpy((INT8U*)&string_buf,(INT8U *)cap_addr_value1, 32);
				//string_buf[32] = 0;
				prtlist_captitle_adjust(string_buf, cap_addr_value1);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharTab(7);
				switch(value_type) {
					case DCLIST_WEIGHT :
						PrtMakeWeightString(string_buf, dc_str.price[0], w_decimal, dec_ch, 0);
						break;
					case DCLIST_PRICE :
						PrtMakePriceString(string_buf, dc_str.price[0], decimal, dec_ch, 0);
						break;
					case DCLIST_COUNT :
						sprintf(string_buf, "%ld", dc_str.price[0]);
						break;
					case DCLIST_PERCENT :
						format_value((HUGEDATA INT8U*)string_buf, dc_str.price[0], 2, dec_ch);
						strcat(string_buf, " %");
						//sprintf(string_buf, "%ld%%", dc_str.price[0]);
						break;
				}
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharLF(1);
			}

			// 2nd Target/Price
			if(target_type != DCLIST_NONE) {
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
				////receipt_caption_read(RCPTCAP_ID_LST_TYPE, string_buf);
				////sprintf(string_buf, "2nd Target");
				//_memcpy((INT8U*)&string_buf,(INT8U *)cap_addr_target2, 32);
				//string_buf[32] = 0;
				prtlist_captitle_adjust(string_buf, cap_addr_target2);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharTab(7);
				switch(target_type) {
					case DCLIST_WEIGHT :
						PrtMakeWeightString(string_buf, dc_str.target[1], w_decimal, dec_ch, 0);
						break;
					case DCLIST_PRICE :
						PrtMakePriceString(string_buf, dc_str.target[1], decimal, dec_ch, 0);
						break;
					case DCLIST_COUNT :
						sprintf(string_buf, "%ld", dc_str.target[1]);
						break;
				}
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharLF(1);
			}

			if(value_type != DCLIST_NONE && target_type != DCLIST_NONE) {
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
				////receipt_caption_read(RCPTCAP_ID_LST_TYPE, string_buf);
				////sprintf(string_buf, "2nd Value");
				//_memcpy((INT8U*)&string_buf,(INT8U *)cap_addr_value2, 32);
				//string_buf[32] = 0;
				prtlist_captitle_adjust(string_buf, cap_addr_value2);
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharTab(7);
				switch(value_type) {
					case DCLIST_WEIGHT :
						PrtMakeWeightString(string_buf, dc_str.price[1], w_decimal, dec_ch, 0);
						break;
					case DCLIST_PRICE :
						PrtMakePriceString(string_buf, dc_str.price[1], decimal, dec_ch, 0);
						break;
					case DCLIST_COUNT :
						sprintf(string_buf, "%ld", dc_str.price[1]);
						break;
					case DCLIST_PERCENT :
						format_value((HUGEDATA INT8U*)string_buf, dc_str.price[1], 2, dec_ch);
						strcat(string_buf, " %");
						//sprintf(string_buf, "%ld%%", dc_str.price[1]);
						break;
				}
				PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
				RcptCmd_CharLF(1);
			}

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Day of Week");
			memcpy((INT8U*)&string_buf,(INT8U *)cap_ptr_dayweek, 32);
			string_buf[32] = 0;
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharTab(7);
			//sprintf(string_buf, "%s", dc_str.day_of_week);
			binary2str(dc_str.day_of_week,(INT8U *)string_buf);
			string_buf[7] = 0;
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Start Date");
			memcpy((INT8U*)&string_buf,(INT8U *)cap_ptr_s_date, 32);
			string_buf[32] = 0;
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharTab(7);
			//sprintf(string_buf, "%s", dc_str.start_month);
			rtc_makedate_str(string_buf, dc_str.start_year, dc_str.start_month, dc_str.start_day);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "End Date");
			memcpy((INT8U*)&string_buf,(INT8U *)cap_ptr_e_date, 32);
			string_buf[32] = 0;
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharTab(7);
			//sprintf(string_buf, "%s", dc_str.start_month);
			rtc_makedate_str(string_buf, dc_str.end_year, dc_str.end_month, dc_str.end_day);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Start Time");
			memcpy((INT8U*)&string_buf,(INT8U *)cap_ptr_s_time, 32);
			string_buf[32] = 0;
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharTab(7);
			//sprintf(string_buf, "%s", dc_str.start_month);
			rtc_maketime_str(string_buf, dc_str.start_hour, dc_str.start_minute, dc_str.start_second);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "End Time");
			memcpy((INT8U*)&string_buf,(INT8U *)cap_ptr_e_time, 32);
			string_buf[32] = 0;
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharTab(7);
			//sprintf(string_buf, "%s", dc_str.start_month);
			rtc_maketime_str(string_buf, dc_str.end_hour, dc_str.end_minute, dc_str.end_second);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);


			RcptCmd_CharLF(1);

			PrintListWaitEnd();

			CurPluWeightUnit = backup_weightunit;	// 복귀

			if(!Prt_CheckOutOfPaper()) {
				RcptCmd_ChangeMode(TO_RESTORE, ON);
				FlushRxBuf(&CommBufPrt);
				return OFF;
			}
			if(!Prt_CheckGapErr()) {
				RcptCmd_ChangeMode(TO_RESTORE, ON);
				FlushRxBuf(&CommBufPrt);
				return OFF;
			}
		//}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

#ifdef USE_NUTRITION_FACT
  #ifdef USE_DIRECT_NUTRITION
INT8U nutri_print_list(INT16U start_id, INT16U end_id, INT8U dept_id)
  #else
INT8U nutri_print_list(INT16U start_id, INT16U end_id)
  #endif
{
	NUTRIFACT_STRUCT nutrifact_str;
	NUTRIFACT_STRUCT2 nutrifact_str2;
#ifdef USE_ADDITIONAL_NUTRIENT
	NUTRIFACT_STRUCT4 nutrifact_str4;
#endif	
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT16S str_size;
	INT8U flagStopPrint = OFF;
	INT8U len;
#ifdef USE_DIRECT_NUTRITION
	INT32U key;
#endif
	INT8U ucTemp, nutri_type=0;

	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
	nutri_type = ucTemp&0x0f;
#ifdef COUNTRY_USA
	nutri_type = NUTRI_TYPE_2COL_USA;
#endif

#ifdef USE_DIRECT_NUTRITION
	base_addr = FLASH_DIRECT_NUTRIFACT_AREA;
#else
	base_addr = DFLASH_BASE + FLASH_NUTRIFACT_AREA;
#endif
	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
	{
		str_size = sizeof(nutrifact_str2);
	}
#ifdef USE_ADDITIONAL_NUTRIENT
	else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN) str_size = sizeof(nutrifact_str4);
#endif	
	else str_size = sizeof(nutrifact_str);


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	caption_split_by_code(0x1322, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
       	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
#ifdef USE_DIRECT_NUTRITION
		if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		{
			key = plu_table_seq_search(dept_id,(INT32U *)&cur_id,end_id);
			if(key > 0)
			{
				if (get_direct_nutrition(dept_id, cur_id, &nutrifact_str4) < 0)
				{
					continue;
				}
			}

			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			sprintf(string_buf, "%d", nutrifact_str4.plu_num); 
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		}
		else
		{
#endif
		addr = base_addr + (INT32U)(cur_id-1)*(INT32U)str_size;
		code = Flash_wread(addr);
		if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
		{
			Flash_sread(addr, (HUGEDATA INT8U*)&nutrifact_str2, str_size);
			if(cur_id != nutrifact_str2.code) continue;
		}
#ifdef USE_ADDITIONAL_NUTRIENT
		else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		{
			Flash_sread(addr, (INT8U*)&nutrifact_str4, str_size); 
  #ifdef USE_DIRECT_NUTRITION
  #else
			if(cur_id != nutrifact_str4.code) continue;
  #endif
		}
#endif
		else
		{
			Flash_sread(addr, (HUGEDATA INT8U*)&nutrifact_str, str_size);
			if(cur_id != nutrifact_str.code) continue;
		}
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
		{
			sprintf(string_buf, "%d", nutrifact_str2.code);
		}
#ifdef USE_ADDITIONAL_NUTRIENT
		else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		{
  #ifdef USE_DIRECT_NUTRITION
  #else
			sprintf(string_buf, "%d", nutrifact_str4.code); 
  #endif
		}
#endif
		else sprintf(string_buf, "%d", nutrifact_str.code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
#ifdef USE_DIRECT_NUTRITION
		}
#endif
		if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
		{
			RcptCmd_PrintMultiLine((char*)nutrifact_str2.description, sizeof(nutrifact_str2.description), TICKET_MARGIN+TICKET_TXT_MARGIN+5);
		}
#ifdef USE_ADDITIONAL_NUTRIENT
		else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
  #ifdef USE_DIRECT_NUTRITION
  			RcptCmd_PrintMultiLine((char*)nutrifact_str4.description, sizeof(nutrifact_str4.description), TICKET_MARGIN+TICKET_TXT_MARGIN+7);
  #else
			RcptCmd_PrintMultiLine((char*)nutrifact_str4.description, sizeof(nutrifact_str4.description), TICKET_MARGIN+TICKET_TXT_MARGIN+5);
  #endif
#endif
		else RcptCmd_PrintMultiLine((char*)nutrifact_str.description, sizeof(nutrifact_str.description), TICKET_MARGIN+TICKET_TXT_MARGIN+5);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}
#endif

INT8U origin_print_list(INT16U start_id, INT16U end_id)
{
	ORIGIN_STRUCT origin_str;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;

	base_addr = DFLASH_BASE + FLASH_ORIGIN_AREA;
	//if(end_id > 999) return OFF;


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "ORIGIN LIST");
	caption_split_by_code(0x1252, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*ORIGIN_STRUCT_SIZE;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&origin_str, ORIGIN_STRUCT_SIZE);
		if(cur_id != origin_str.origin_code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//sprintf(string_buf, "No. ");
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", origin_str.origin_code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char*)origin_str.origin_text, ORIGIN_STRUCT_SIZE-2, TICKET_MARGIN+TICKET_TXT_MARGIN+5);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
///		LabCmd_FormFeed();
///		LabCmd_FormFeed();
///		PrintListWaitEnd();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

INT8U table_print_list(INT16U title_cap, INT32U base_addr, INT16U str_size, INT16U start_id, INT16U end_id)
{
	MENU_TYPE1_STRUCT	menu_type1_str;
	CAPTION_STRUCT 		cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U addr;
	//INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;

	base_addr += DFLASH_BASE;

	//if(end_id > 999) return OFF;


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	caption_split_by_code(title_cap, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
  	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*(INT32U)str_size;
		//code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&menu_type1_str, str_size);
		if(cur_id != menu_type1_str.code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//sprintf(string_buf, "No. ");
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", menu_type1_str.code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char*)menu_type1_str.text, str_size-2, TICKET_MARGIN+TICKET_TXT_MARGIN+5);
		///RcptCmd_CharLF(1);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {    
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}


extern INT8U Flash_check_nvbuffer_4_rd(INT32U start_addr, INT32U byte_count);
INT8U ingredient_print_list(INT16U start_id, INT16U end_id)
{
#ifdef CPU_PIC32MZ
	INGREDIENT_STRUCT ing_str;
#endif
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT16U len;
#ifdef USE_9999_INDIRECT_INGREDIENT
	INT32U  ingredient_id;
#endif	

	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	caption_split_by_code(0x1312, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
  	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

#ifdef USE_9999_INDIRECT_INGREDIENT
	for(cur_id = 1; cur_id <= MAX_INGREDIENT_NO-1 && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
		ingredient_id = ingredient_get_table_pos(cur_id-1);
		if(!(ingredient_id & INGREDIENT_EXIST_MASK))
		{
			if(ingredient_id <= end_id && ingredient_id >= start_id)
			{
				code = ingredient_get_id(ingredient_id);
				if(code != 0)
				{
					RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
					RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
					receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
					sprintf(string_buf, "%d", ingredient_id);
					PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

					addr  = ing_calc_addr_by_key(cur_id-1);
					addr += 2;
					if(Flash_check_nvbuffer_4_rd(addr, INGREDIENT_STRUCT_SIZE-2) == 1) {
						addr = (INT32U)(FlashNVRAMStruct.buffer + (addr&FLASH_BLOCK_LADDR_MASK));
					}
					RcptCmd_PrintMultiLine((char*)addr, INGREDIENT_STRUCT_SIZE-2, TICKET_MARGIN+TICKET_TXT_MARGIN+5);
					RcptCmd_CharLF(1);

					PrintListWaitEnd();

					if(!Prt_CheckOutOfPaper()) {
						RcptCmd_ChangeMode(TO_RESTORE, ON);
						FlushRxBuf(&CommBufPrt);
						return OFF;
					}
					if(!Prt_CheckGapErr()) {
						RcptCmd_ChangeMode(TO_RESTORE, ON);
						FlushRxBuf(&CommBufPrt);
						return OFF;
					}
				}
			}
		}
	}
#else
	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
#ifdef CPU_PIC32MZ
		addr = ing_calc_addr_by_key(cur_id-1);
		code = Flash_wread(addr);
		Flash_sread(addr, (HUGEDATA INT8U*)&ing_str, INGREDIENT_STRUCT_SIZE-2);
		if(cur_id != ing_str.no) continue;
#else
		code = ingredient_get_id(cur_id);
		if(cur_id != code) continue;
#endif
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

#ifdef CPU_PIC32MZ
		RcptCmd_PrintMultiLine(ing_str.text, INGREDIENT_STRUCT_SIZE-2, TICKET_MARGIN+TICKET_TXT_MARGIN+5);
#else
		addr  = ing_calc_addr_by_key(cur_id-1);
		addr += 2;
		if(Flash_check_nvbuffer_4_rd(addr, INGREDIENT_STRUCT_SIZE-2) == 1) {
			addr = (INT32U)(FlashNVRAMStruct.buffer + (addr&FLASH_BLOCK_LADDR_MASK));
		}
		RcptCmd_PrintMultiLine((char*)addr, INGREDIENT_STRUCT_SIZE-2, TICKET_MARGIN+TICKET_TXT_MARGIN+5);
#endif
		RcptCmd_CharLF(1);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
#endif
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}


INT8U store_name_print_list(INT16U start_id, INT16U end_id)
{
	STORE_NAME_STRUCT store_str;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT16S str_size;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32U cap_addr_tel, cap_addr_text;

	base_addr = DFLASH_BASE + FLASH_SHOPNAME_AREA;
	str_size = sizeof(STORE_NAME_STRUCT);
	//if(end_id > 999) return OFF;

	cap_addr_tel  = caption_search_by_code(0x141c) + 2;
	cap_addr_text = caption_search_by_code(0x141d) + 2;

	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "STORE NAME LIST");
	caption_split_by_code(0x1412, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
     	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
     	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*(INT32U)str_size;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&store_str, str_size);
		if(cur_id != store_str.code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", store_str.code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char*)store_str.name, sizeof(store_str.name), TICKET_MARGIN+TICKET_TXT_MARGIN+5);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_TEL, string_buf);
prtlist_captitle_adjust(string_buf, cap_addr_tel);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char*)store_str.tel_no, sizeof(store_str.tel_no), TICKET_MARGIN+TICKET_TXT_MARGIN+5);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_TEXT, string_buf);
prtlist_captitle_adjust(string_buf, cap_addr_text);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char*)store_str.text, sizeof(store_str.text), TICKET_MARGIN+TICKET_TXT_MARGIN+5);
		RcptCmd_CharLF(1);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

INT8U tarefile_print_list(INT16U start_id, INT16U end_id)
{
	TARE_STRUCT tare_str;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT8U w_decimal, w_dec_ch, backup_unit;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32U cap_addr_tareweight, cap_addr_taretype;

	base_addr = DFLASH_BASE + FLASH_TARE_AREA;

	cap_addr_tareweight = caption_search_by_code(0x3274) + 2;
	cap_addr_taretype   = caption_search_by_code(0x3273) + 2;

	//if(end_id > 999) return OFF;
	//w_decimal=get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
////	w_decimal=get_decimal_pt(1, 0);
	w_dec_ch =get_global_bparam(GLOBAL_DECIMAL_SIGN);


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "TARE LIST");
	caption_split_by_code(0x1272, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
     	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*TARE_STRUCT_SIZE;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&tare_str, TARE_STRUCT_SIZE);
		if(cur_id != tare_str._code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//sprintf(string_buf, "No. ");
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", tare_str._code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_PrintMultiLine((char *)tare_str._description, sizeof(tare_str._description), TICKET_MARGIN+TICKET_TXT_MARGIN+5);

		w_decimal=get_decimal_pt(3, tare_str._weightunit);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_TARE_WEIGHT, string_buf);
prtlist_captitle_adjust(string_buf, cap_addr_tareweight);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		backup_unit = CurPluWeightUnit;
		CurPluWeightUnit = tare_str._weightunit;
		PrtMakeWeightString(string_buf, tare_str._value, w_decimal, w_dec_ch, 8);
		CurPluWeightUnit = backup_unit;
		//format_value((HUGEDATA INT8U *)string_buf, tare_str._value, w_decimal,w_dec_ch);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_TARE_TYPE, string_buf);
prtlist_captitle_adjust(string_buf, cap_addr_taretype);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%d", tare_str._type);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

#ifdef USE_TAX_RATE
INT8U tax_print_list(INT16U start_id, INT16U end_id)
{
	TAX_STRUCT tax_str;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32U cap_addr_taxrate, cap_addr_taxtype;

	base_addr = DFLASH_BASE + FLASH_TAX_AREA;
	//if(end_id > 999) return OFF;

	cap_addr_taxrate = caption_search_by_code(0x3233) + 2;
	cap_addr_taxtype = caption_search_by_code(0x3232) + 2;

	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "TAX LIST");
	caption_split_by_code(0x1232, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*TAX_STRUCT_SIZE;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&tax_str, TAX_STRUCT_SIZE);
		if(cur_id != tax_str.tax_code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", tax_str.tax_code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
		RcptCmd_TypeNone();

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_TAX_RATE, string_buf);
prtlist_captitle_adjust(string_buf, cap_addr_taxrate);
//_memcpy((INT8U*)&string_buf,(INT8U *)cap_addr_taxrate, 32);
//string_buf[32] = 0;
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		format_value((HUGEDATA INT8U *)string_buf, tax_str.tax_rate, 2,'.');
		len = strlen(string_buf);
		string_buf[len]   = '%';
		string_buf[len+1] = 0;
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_TAX_TYPE, string_buf);
prtlist_captitle_adjust(string_buf, cap_addr_taxtype);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		if(tax_str.tax_type == 0) {
			receipt_caption_read(RCPTCAP_ID_COM_INT_TAX, string_buf);
		} else {
			receipt_caption_read(RCPTCAP_ID_COM_EXT_TAX, string_buf);
		}
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
///		LabCmd_FormFeed();
///		LabCmd_FormFeed();
///		PrintListWaitEnd();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}
#endif

#ifdef USE_TRACEABILITY
////extern INT8U get_country_name(INT16U id, HUGEDATA char *name);
////extern INT8U get_cuttinghall_name(INT16U id, HUGEDATA char *name);
////extern INT8U get_slaughterhouse_name(INT16U id, HUGEDATA char *name);
INT8U traceability_print_list(INT16U start_id, INT16U end_id)
{
	TRACEABILITY_STRUCT trace_str;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;

	base_addr = DFLASH_BASE + FLASH_TRACEABILITY_AREA;
	//if(end_id > 999) return OFF;


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "TRACEABILITY LIST");
	caption_split_by_code(0x1332, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*TRACEABILITY_STRUCT_SIZE;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&trace_str, TRACEABILITY_STRUCT_SIZE);
		if(cur_id != trace_str.code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//sprintf(string_buf, "No. ");
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", trace_str.code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char *)trace_str.contents, sizeof(trace_str.contents), TICKET_MARGIN+TICKET_TXT_MARGIN+5);


		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_BORN_IN, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);

		sprintf(string_buf, "%d:", trace_str.idata[0]);
		len = strlen(string_buf);
		get_country_name(trace_str.idata[0], string_buf+len);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		receipt_caption_read(RCPTCAP_ID_LST_BRED_IN, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);

		sprintf(string_buf, "%d:", trace_str.idata[1]);
		len = strlen(string_buf);
		get_country_name(trace_str.idata[1], string_buf+len);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_SLAUGHTER, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%d:", trace_str.idata[2]);
		len = strlen(string_buf);
		get_slaughterhouse_name(trace_str.idata[2], string_buf+len);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		receipt_caption_read(RCPTCAP_ID_LST_CUTTED_IN, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);

		sprintf(string_buf, "%d:", trace_str.idata[2]);
		len = strlen(string_buf);
		get_cuttinghall_name(trace_str.idata[3], string_buf+len);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

INT8U workplace_table_print_list(INT16U title_cap, INT32U base_addr, INT16U str_size, INT16U start_id, INT16U end_id)
{
	WORKPLACE_STRUCT	workplace_str;
	CAPTION_STRUCT 		cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;

	base_addr += DFLASH_BASE;

	//if(end_id > 999) return OFF;


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	
	caption_split_by_code(title_cap, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*(INT32U)str_size;
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&workplace_str, str_size);
		if(cur_id != workplace_str.code) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//sprintf(string_buf, "No. ");
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", workplace_str.code);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));

		RcptCmd_PrintMultiLine((char *)workplace_str.text, sizeof(workplace_str.text), TICKET_MARGIN+TICKET_TXT_MARGIN+5);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);

		receipt_caption_read(RCPTCAP_ID_LST_COUNTRY_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%d:", workplace_str.country_number);
		len = strlen(string_buf);
		get_country_name(workplace_str.country_number, string_buf+len);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;

}
#endif

extern INT32U plu_table_seq_search(INT16S deptno,INT32U *pluno,INT32U max_id);

INT8U print_plu_list(INT16S dept_no, long start_id, long end_id)
{
	char string_buf[100];
	INT32U	addr;
	//INT16U	pluname_offset, plutype_offset, pluno_offset, price_offset, icode_offset, key, usize;
	INT16U	key, usize;
	INT16U  offset_array[10];
	INT8U stype, decimal, dec_ch;
	char plu_name[65];
	long cur_id;
	INT8U flagStopPrint = OFF;
	INT8U len;

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);
			
////	pluname_offset = plu_get_ptype_point(PTYPE_PLU_NAME1, &stype, &usize);
////	plutype_offset = plu_get_ptype_point(PTYPE_PLU_TYPE, &stype, &usize);
////	pluno_offset   = plu_get_ptype_point(PTYPE_PLU_NO, &stype, &usize);
////	price_offset   = plu_get_ptype_point(PTYPE_PRICE, &stype, &usize);
////	icode_offset   = plu_get_ptype_point(PTYPE_ITEM_CODE, &stype, &usize);			
	offset_array[0] = plu_get_ptype_point(PTYPE_PLU_NAME1, &stype, &usize);	//plu name offset
	offset_array[1] = plu_get_ptype_point(PTYPE_PLU_TYPE, &stype, &usize);	//plu type offset
	offset_array[2] = plu_get_ptype_point(PTYPE_PLU_NO, &stype, &usize);	//plu no offset
	offset_array[3] = plu_get_ptype_point(PTYPE_PRICE, &stype, &usize);	//price offset
	offset_array[4] = plu_get_ptype_point(PTYPE_ITEM_CODE, &stype, &usize);	//item code offset
	offset_array[5] = plu_get_ptype_point(PTYPE_PIECE, &stype, &usize);	//quantity offset


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "PLU LIST");		// CAPADD_CJK
	receipt_caption_read(RCPTCAP_ID_LST_PLULIST, string_buf);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	//sprintf(string_buf, "Dept No:%d", dept_no);
	receipt_caption_read(RCPTCAP_ID_LST_DEPT_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", dept_no);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
      	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%ld, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%ld", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}								
		key=plu_table_seq_search(dept_no,(INT32U*)&cur_id,end_id);

 		if(key > 0) {
//			addr = key -1;
//			if (key>EXT_PLU_NO) {
//	 			addr-= EXT_PLU_NO;
//				addr*= (INT32U)plu_record_size;
//	 			addr+= EXT_MEMCARD_ADDR_X;
//			} else {
//				addr*= (INT32U)plu_record_size;
//	 			addr+= FLASH_PLU_AREA_PNT;
//			}
			addr = plu_calc_addr_by_key((long)(key-1));
			Flash_sread(addr+offset_array[0],(HUGEDATA INT8U*)plu_name, 64);
			plu_name[64] = 0;

			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Plu No. %ld", Flash_lread(addr+offset_array[2]));
			receipt_caption_read(RCPTCAP_ID_LST_PLU_NO, string_buf);
			len = strlen(string_buf);
			sprintf(string_buf+len, "%ld", Flash_lread(addr+offset_array[2]));
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Name");
			receipt_caption_read(RCPTCAP_ID_LST_NAME, string_buf);
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharTab(6);
			PutDataRxBuf(&CommBufPrt, plu_name, strlen(plu_name));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Item Code");
			receipt_caption_read(RCPTCAP_ID_LST_ITEM_CODE, string_buf);
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharTab(6);
			sprintf(string_buf, "%ld", Flash_lread(addr+offset_array[4]));
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Type");
			receipt_caption_read(RCPTCAP_ID_LST_TYPE, string_buf);
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharTab(6);
			sprintf(string_buf, "%d", Flash_bread(addr+offset_array[1]));
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Price");
			receipt_caption_read(RCPTCAP_ID_LST_UNITPRICE, string_buf);
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharTab(6);
			PrtMakePriceString(string_buf, Flash_lread(addr+offset_array[3]), decimal, dec_ch, 8);
			//format_fixed((HUGEDATA INT8U *)string_buf, Flash_lread(addr+offset_array[3]), decimal, 8, dec_ch);
			string_buf[8] = 0;
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharLF(1);

			if(Flash_bread(addr+offset_array[1]) == 3) {	// By-Pcs PLU 일때
				RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
				//sprintf(string_buf, "Quantity");
				receipt_caption_read(RCPTCAP_ID_LST_QUANTITY, string_buf);
				PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
				RcptCmd_CharTab(6);
				sprintf(string_buf, "%d", Flash_bread(addr+offset_array[5]));
				PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
				RcptCmd_CharLF(1);
			}

			RcptCmd_CharLF(1);

			PrintListWaitEnd();

			if(!Prt_CheckOutOfPaper()) {
				RcptCmd_ChangeMode(TO_RESTORE, ON);
				FlushRxBuf(&CommBufPrt);
				return OFF;
			}
			if(!Prt_CheckGapErr()) {
				RcptCmd_ChangeMode(TO_RESTORE, ON);
				FlushRxBuf(&CommBufPrt);
				return OFF;
			}
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}



INT8U print_plu_list_forlabel(INT16S dept_no, long start_id, long end_id)
{
	long cur_id;
	INT8U ret;
	INT32U key;


	ret = ON;

	for(cur_id = start_id; cur_id <= end_id; cur_id++) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}								
		key=plu_table_seq_search(dept_no,(INT32U*)&cur_id,end_id);

 		if(key > 0) {
			//ret = print_out_samplelabel(dept_no, cur_id, key-1);
			//ret = Prt_PrintStart(PRT_ITEM_TEST_MODE, LABEL, 0, ON, dept_no, cur_id);
			ret = Prt_PrintStart(PRT_ITEM_TEST_MODE, PrtStruct.Mode, 0, ON, dept_no, cur_id);    //Modify se-hyung 20070912
			if(ret != 1) {ret = OFF; break;}
		}
	}

	return ret;
}
#endif //#ifndef DISABLE_LIST_MENU

INT8U print_plu_contents(INT16S plu_no, INT16S dept_no, INT16S *enable_list, INT16S max_id)
{
	INT32U pnt,npnt;
	HUGEDATA INT8U *data_point;
	INT16S cur_id;
	INT8U ret_byte;
	INT16U ret_word;
	long ret_long;
	char cap_str[64], string_buf[64], str[4];
	INT8U stype;//, length, lineno, dpoint;	
	INT16U ptype, usize, uitemp;
//	INT32U vdefault, maxvalue;
	INT8U dec_pt, dec_ch;
	INT8U flagStopPrint = OFF;
	INT8U unitweight=0xff;

	pnt  = DFLASH_BASE;
	pnt += FLASH_PLU_STRUCT_AREA;
	pnt += PLU_STRUCT_POINT;


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	receipt_caption_read(RCPTCAP_ID_LST_DEPT_NO, cap_str);
	sprintf(string_buf, "%-8s:%d", cap_str, dept_no);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	receipt_caption_read(RCPTCAP_ID_LST_PLU_NO, cap_str);
	sprintf(string_buf, "%-8s:%d", cap_str, plu_no);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = 0; cur_id < max_id && !flagStopPrint; cur_id++) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		uitemp = plu_get_item_position_from_ptype(enable_list[cur_id]);
		
		pnt  = DFLASH_BASE;
		pnt += FLASH_PLU_STRUCT_AREA;
		pnt += PLU_STRUCT_POINT;
		pnt += PLU_STRUCT_SIZE * (INT32U)uitemp;
		npnt  = DFLASH_BASE;
		npnt += FLASH_PLU_STRUCT_CAPTION_AREA;
		npnt += PLUSTR_NAME_SIZE * (INT32U)uitemp;

		Flash_sread(npnt,(HUGEDATA INT8U*)cap_str,PLUSTR_NAME_SIZE);
		ptype    = Flash_wread(pnt+PLUSTR_PTYPE_POINT);
//		vdefault = Flash_lread(pnt+PLUSTR_VDEFAULT_POINT);
//		maxvalue = Flash_lread(pnt+PLUSTR_MAXVALUE_POINT);
	   	stype    = Flash_bread(pnt+PLUSTR_STYPE_POINT);
//		length   = Flash_bread(pnt+PLUSTR_LENGTH_POINT);
//		lineno   = Flash_bread(pnt+PLUSTR_LINENO_POINT);
//		dpoint   = Flash_bread(pnt+PLUSTR_DPOINT_POINT);
		usize    = Flash_wread(pnt+PLUSTR_USIZE_POINT);

#ifdef USE_DIRECT_NUTRITION
		if(ptype == PTYPE_NUTRITION_NO)
			continue;
#endif

		data_point = NvramTempBuffer[0];
		data_point += plu_get_nth_point((INT16U)uitemp,(INT16U *)&usize);

	
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		PutDataRxBuf(&CommBufPrt,cap_str,strlen(cap_str));

		//RcptCmd_CharTab(8);

		switch (stype) {
			case 'S':
				sprintf(string_buf, "%s", data_point);
				RcptCmd_PrintMultiLine(string_buf, strlen(string_buf), TICKET_MARGIN+TICKET_TXT_MARGIN+8);
				PrintListWaitEnd();
				if(!Prt_CheckOutOfPaper()) {
					RcptCmd_ChangeMode(TO_RESTORE, ON);
					FlushRxBuf(&CommBufPrt);
					return OFF;
				}
				if(!Prt_CheckGapErr()) {
					RcptCmd_ChangeMode(TO_RESTORE, ON);
					FlushRxBuf(&CommBufPrt);
					return OFF;
				}
				continue;
			case 'M':
			case 'B':	// Byte 
				ret_byte=*data_point;
				//ret_long=(long)ret_byte;
				sprintf(string_buf, "%d", ret_byte);
				if(ptype == 5) unitweight = ret_byte;
				break;
			case 'W': 	// Word
//				_memcpy((INT8U*)&ret_word,(INT8U*)data_point,2);
				memcpy((INT8U*)&ret_word,(INT8U*)data_point,2);
				//ret_long=(long)ret_word;
				sprintf(string_buf, "%d", ret_word);
				break;
			case 'L': 	// Long
//				_memcpy((INT8U*)&ret_long,(INT8U*)data_point,4);
				memcpy((INT8U*)&ret_long,(INT8U*)data_point,4);
				if(ret_long < 0) ret_long = 0;
				switch(ptype) {
					case 13 :	// Tare Weight
					case 100 :	// Fixed Weight
						//dec_pt = get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
						dec_pt = get_decimal_pt(5, (INT16U)unitweight);
						//dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);
						format_value((HUGEDATA INT8U *)string_buf, ret_long, dec_pt, dec_ch);
						break;
					case 6 :	// Unit Price
					case 21 :	// Cost Price
#ifndef USE_TOPMART_STRUCTURE
					case 91 :	// Special Price
#endif
						dec_pt = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
						//dec_ch = get_global_bparam(GLOBAL_DECIMAL_SIGN);
						format_value((HUGEDATA INT8U *)string_buf, ret_long, dec_pt, dec_ch);
						break;
					default :
						sprintf(string_buf, "%ld", ret_long);
						break;
				}
				break;
			case 'T':
			case 'D':
//				_memcpy((INT8U *)str,(INT8U *)data_point,3);
				memcpy((INT8U *)str,(INT8U *)data_point,3);
				sprintf((char*)string_buf,"%02d.%02d.%02d",str[0],str[1],str[2]);
				break;
			default: ret_long = 0;
		}
		RcptCmd_CharTab(8);
		PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
		RcptCmd_CharLF(1);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}

	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
///		LabCmd_FormFeed();
///		LabCmd_FormFeed();
///		PrintListWaitEnd();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}


//extern INT16U history_index;

INT8U history_print_list(void)
{
	//CAPTION_STRUCT cap;
	INT16U i;   
	char string_buf[100];
	INT32U addr;
	INT8U flagStopPrint = OFF;
	//INT8U len;
	CAL_HISTORY hist;
	RTC_STRUCT time_str;
	INT16U history_id;


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;
	
	history_id = get_base_wparam(FLASH_CALLOG_COUNT);
	history_id &= 0x7FFF;		// 0x8000으로 History qty update 되없음을 판단

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	sprintf(string_buf, "HISTORY : %d", history_id);	// history count
	//receipt_caption_read(RCPTCAP_ID_HISTORY, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(i = 0; i < HISTORY_QTY && !flagStopPrint; i++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}

		addr =(INT32U)history_id-i;
		if(history_id <= i) break;
		addr%=HISTORY_QTY;
		addr*=HISTORY_STRUCT_SIZE;
		addr+=DFLASH_BASE;
		addr+=FLASH_CALLOG_DATA;

		Flash_sread(addr,(HUGEDATA INT8U *)&hist,sizeof(hist));

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		time_str.year = bcd2hex(hist.year);
		time_str.month = bcd2hex(hist.month);
		time_str.date = bcd2hex(hist.date);
		time_str.hour = bcd2hex(hist.hour);
		time_str.min = bcd2hex(hist.min);
		time_str.sec = bcd2hex(hist.sec);
		time_str.day = date_getweek(time_str.year, time_str.month, time_str.date);
		sprintf(string_buf, "#%d ", i+1);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		PrintFixedTime(&time_str);
		RcptCmd_CharLF(1);

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
		sprintf(string_buf, "AD Capa     : %ld", hist.capa);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
		sprintf(string_buf, "AD Zero     : %ld", hist.ad_zero);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
		//receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		//PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "AD Span     : %ld", hist.ad_span);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		if (hist.ver >= 1)
		{
			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
			sprintf(string_buf, "AD MidUp     : %ld", hist.ad_midup);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);

			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
			sprintf(string_buf, "AD MidDn     : %ld", hist.ad_middn);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);
		}

		RcptCmd_CharLF(1);

//		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
//		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
//		sprintf(string_buf, "Sensor Gap  : %d", hist.sensor_gab);
//		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
//		RcptCmd_CharLF(1);

//		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
//		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
//		sprintf(string_buf, "Sensor Peel : %d", hist.sensor_peel);
//		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
//		RcptCmd_CharLF(2);

		PrintListWaitEnd();
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

extern ROMDATA PARAMETER_SETTING_TYPE	ParameterSetting[];
extern ROMDATA PARAMETER_SETTING_TYPE	MenuParameterSetting[]; //110422 Added by YYC 
extern INT8U get_ret_parameter_value(INT16U arrayid, long *set_val, char *set_str);

INT8U parameter_print_list(INT8U mode)
{

	CAPTION_STRUCT cap;
	INT16U i;   
	char string_buf[100];
	INT8U flagStopPrint = OFF;
	INT8U ret;
	char set_str[34];
	long set_val;
	INT16U start_no, end_no; 
	INT16U strno;

//	INT8U datatype, startbit, bitct;
	INT16U capno, fc;
//	INT32U memaddr;


	switch(mode) {
		case DEALER_MODE :
			start_no = 500;
			end_no   = 800;
			break;
		case FACTORY_MODE :
			start_no = 500;
			end_no   = 999;
			break;
	}

		

	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "PARAMETER LIST");
	receipt_caption_read(RCPTCAP_ID_LST_PARAMETER_LIST, string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(i = 0; i < 1000; i++) {
		if(start_no <= ParameterSetting[i].funcCode) break;
	}

	for( ; i < 1000 && !flagStopPrint; i++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
			}
		}
//		datatype = ParameterSetting[i].bitFlag;
		capno = ParameterSetting[i].captionNum;
//		memaddr = ParameterSetting[i].addrMemory; 
//		startbit = ParameterSetting[i].startBit;
//		bitct = ParameterSetting[i].countBit;
		fc = ParameterSetting[i].funcCode;
		if(fc > end_no) break;
		if(fc == 0) break;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		if (fc >= 922 && fc <= 940) {
			strno = fc-921;
			if (strno > 12) strno -= 12;
			//_global_set_oneline_data_EX(capno, capno, memaddr, type, 0,strno);
			caption_split_by_code(capno, &cap,0);
			if (strno > 0) {
				sprintf(set_str, "%2d :", strno);
				strcat(cap.form, set_str);
			}
		} else {
			caption_split_by_code(capno, &cap,0);
		}
		sprintf(string_buf, "#%d  %s", fc, cap.form);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+4);
		ret = get_ret_parameter_value(i, &set_val, set_str);
		switch(ret) {
			case 0 :
				sprintf(string_buf, "%ld", set_val);
				break;
			case 1 :
				sprintf(string_buf, "%s", set_str);
				break;
			case 2 :
				sprintf(string_buf, "%s", (set_val?"Yes":"No"));
				break;
			case 0xff : 
			default :
				sprintf(string_buf, "N/A");
				break;
		}
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);

		PrintListWaitEnd();
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}


INT8U get_ret_parameter_value(INT16U arrayid, long *set_val, char *set_str)
{
	long ret_val;
	INT8U datatype, startbit, bitct, szmem;
	INT16U capno, fc;
	INT32U memaddr;	    
	INT8U ret_type, i;
	CAPTION_STRUCT cap;
	INT16U t16u;

	fc = ParameterSetting[arrayid].funcCode;
	capno = ParameterSetting[arrayid].captionNum;
	memaddr = ParameterSetting[arrayid].addrMemory; 
	szmem = ParameterSetting[arrayid].sizeMemory;
	datatype = ParameterSetting[arrayid].bitFlag;
	startbit = ParameterSetting[arrayid].startBit;
	bitct = ParameterSetting[arrayid].countBit;

	ret_val = -2;
	ret_type = 0;
	if(datatype == 8) ret_type = 0;
	else if(datatype == 3) ret_type = 1;
	else {
		if(bitct == 1) ret_type = 2;
	}

//	if(fc == 605) {
//		ret_val = ad_get_accumul_tare(); 
//		ret_type = 2;
//	} else if(fc == 606) {
//		ret_val = ad_get_subtract_tare();
//		ret_type = 2;
//	} else 
	if(fc == 998) {
		//restore_factory_default();
//	} else if(fc == 550) {
//		ret_val = SensorStruct.GapOutOfPaperThreshold;
//		ret_type = 0;
	} else {

//		if(fc == 502 || fc == 996 || fc == 995
//			|| (fc >= 640 && fc <= 675)){
//			memaddr += DFLASH_BASE;
//		} else {
//			memaddr += DFLASH_BASE;
//			memaddr += FLASH_GLOBAL_AREA;
//		}
		memaddr += DFLASH_BASE;
		memaddr += ParameterSetting[arrayid].offsetMemory;
		switch(datatype) {
			case 1:	// Byte
				ret_val = Flash_bread(memaddr);
				break;
			case 2: // Word
				ret_val = Flash_wread(memaddr);
				break;
			case 5: // long
				ret_val = Flash_lread(memaddr);
				break;
			case 3: // String
				caption_split_by_code(capno, &cap,0);
				if(!strchr((char *)cap.form,'[')) caption_adjust(&cap,NULL);
				Flash_sread(memaddr, (HUGEDATA INT8U *)set_str, cap.input_length);
				set_str[cap.input_length] = 0;
				break;
			case 8:	// Bit Value(Not Yes/No)
			case 4: // Bit value
				if(bitct < 1) bitct = 1;
				ret_val = 0;
				switch(szmem) {
					case 1: // 1byte
						t16u = Flash_bread(memaddr);
						break;
					case 2: // 2byte
						t16u =Flash_wread(memaddr);
						break;
				}
				//t16u >>= (startbit-(1-bitct));
				t16u >>= (startbit-(bitct-1));
				for(i = 0; i < bitct; i++) {
					if ((t16u >> i) & 0x1)
						ret_val |= (0x1 << i);
					else
						ret_val &= (~(0x1 << i));
				}
				break;
		}
	}

	*set_val = ret_val;
	if(ret_type != 1 && ret_val == -2) ret_type = 0xff;
	return ret_type;
}



#ifndef DISABLE_LIST_MENU
INT8U currency_print_list(INT16U start_id, INT16U end_id)
{
	CURRENCY_STRUCT	cur_str;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32U str_size;
	INT32U cap_addr_rate_dec, cap_addr_cur_rate, cap_addr_cur_sym1, cap_addr_cur_sym2, cap_addr_cur_dec, cap_addr_price_form;

	base_addr = DFLASH_BASE + FLASH_CURRENCY_AREA;
	str_size = sizeof(CURRENCY_STRUCT);
	//if(end_id > 999) return OFF;

	cap_addr_rate_dec = caption_search_by_code(0x3912) + 2;
	cap_addr_cur_rate = caption_search_by_code(0x3913) + 2;
	cap_addr_cur_sym1 = caption_search_by_code(0x3914) + 2;
	cap_addr_cur_sym2 = caption_search_by_code(0x3915) + 2;
	cap_addr_cur_dec  = caption_search_by_code(0x3916) + 2;
	cap_addr_price_form = caption_search_by_code(0x4637) + 2;


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	//sprintf(string_buf, "TAX LIST");
	caption_split_by_code(0x1442, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*str_size;			
		Flash_sread(addr, (HUGEDATA INT8U*)&cur_str, str_size); 
		if((cur_str.rate<=0) || (cur_str.rate>999999l)) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", cur_id);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
		RcptCmd_TypeNone();

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
prtlist_captitle_adjust(string_buf, cap_addr_rate_dec);	
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		format_value((HUGEDATA INT8U *)string_buf, cur_str.ratepoint, 0,'.');
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_TAX_RATE, string_buf);
prtlist_captitle_adjust(string_buf, cap_addr_cur_rate);	
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		format_value((HUGEDATA INT8U *)string_buf, cur_str.rate, cur_str.ratepoint,'.');
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_PRIMARY_SIGN, string_buf);	// Primary Sign
prtlist_captitle_adjust(string_buf, cap_addr_cur_sym1);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		sprintf(string_buf, "%s", cur_str.primary);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_LAST_SIGN, string_buf);	// Last Sign
prtlist_captitle_adjust(string_buf, cap_addr_cur_sym2);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		sprintf(string_buf, "%s", cur_str.last);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_DECIMAL_POINT, string_buf);	// Decimal Point
prtlist_captitle_adjust(string_buf, cap_addr_cur_dec);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		sprintf(string_buf, "%d", cur_str.decimalpoint);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//receipt_caption_read(RCPTCAP_ID_LST_PRICE_FORMAT, string_buf);	// Price Format
prtlist_captitle_adjust(string_buf, cap_addr_price_form);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		sprintf(string_buf, "%d", cur_str.prtform);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

#ifdef PRINT_AD_SETTINGS
INT8U adm_env_print_list(void)
{
	INT32S ltemp;
//	INT16U version;
	INT8U i, j;
	char string_buf[100];


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	sprintf(string_buf, "A/D Environment");
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Version            : %6ld", ltemp%1000);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp = ADM_WaitData(ADM_CMD_GET_CAL_ZERO, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Cal Zero A/D       : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CAL_SPAN, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Cal Span A/D       : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CAL_RATIO, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Cal Ratio          : 0x%04lX", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CAPACITY, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Capa               : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_DUAL, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Interval           : %6s", (ltemp?"Single":"Dual"));
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CURVE_RATIO, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Curve Ratio        : 0x%04lX", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	
	PrintListWaitEnd();
		
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CURVE_ZERO, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Curve Zero A/D     : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CURVE_MID, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Curve Mid A/D      : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CURVE_SPAN, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Curve Span A/D     : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_INI_ZR_FLAG, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Initial Zero Flag  : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	
	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CALZEROMIN, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Min Cal Zero Range : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CALZEROMAX, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Max Cal Zero Range : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CALSPANMIN, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Min Cal Span Range : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CALSPANMAX, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Max Cal Span Range : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	
	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_OVERLD_R, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Overload limit     : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_INIT_ZR_R, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Init-Zero Range    : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_REZERO_R, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Zero-Track Range   : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_TARESET_R, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Tare Limit         : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp = ADM_WaitData(ADM_CMD_GET_GRVTY_CAL, 0, ADM_PARAM_OFF);		// Get Gravity at Cal
	sprintf(string_buf, "Factory Gravity    : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp = ADM_WaitData(ADM_CMD_GET_GRVTY_USE, 0, ADM_PARAM_OFF);		// Get Gravity at Use
	sprintf(string_buf, "Local Gravity      : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_W_UNIT_CAL, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Cal Unit           : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_W_UNIT_USE, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Use Unit           : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_W_UNIT_CUR, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Current Unit       : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_MOTION_BAND, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Motion Band        : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_FILTON_BAND, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Filter On Band     : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_FILTOFF_BAND, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Filter Off Band    : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_FILTER_SIZE, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Filter Sample Times: %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);


	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_ZERO, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Zero Buffer        : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_TARE_INT, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Tare Buffer(Inter) : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_TARE_EXT, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Tare Buffer(Exter) : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	PrintListWaitEnd();

//	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
//	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
//	ltemp =	ADM_WaitData(ADM_CMD_GET_OUTMETHODE, 0, ADM_PARAM_OFF);
//	sprintf(string_buf, "Out Method         : %6ld", ltemp);
//	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
//	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_ETC, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "ETC Settings       :   0x%02lX", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_TIME_PERIOD, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Timer Period       : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CHARGE, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Charge Time        : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_RECHARGE, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Recharge Time      : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_BAUDRATE, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Baudrate           : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_UPDATETIME1, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Update Time1       : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_UPDATETIME2, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Update Time2       : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_UPDATETIME3, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "Update Time3       : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);


	PrintListWaitEnd();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_CHECK_EEP, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "EEPROM Check Status: %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
	ltemp =	ADM_WaitData(ADM_CMD_GET_INIT_STAT, 0, ADM_PARAM_OFF);
	sprintf(string_buf, "EEPROM Init Status : %6ld", ltemp);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);

	ADM_WaitData(ADM_CMD_SET_EEP_ADDR, 0, ADM_PARAM_ON);
	for(i = 0; i < 16; i++) {
		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN+1);
		for(j = 0; j < 16; j++) {
			//ADM_WaitData(ADM_CMD_SET_EEP_ADDR, (i*16)+j, ADM_PARAM_ON);
			ltemp = ADM_WaitData(ADM_CMD_GET_EEP_DATA, 0, ADM_PARAM_OFF);
			sprintf(string_buf, "%02X ", (INT8U)ltemp);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		}
		RcptCmd_CharLF(1);
	}
	RcptCmd_CharLF(1);

	PrintListWaitEnd();

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}
#endif

#ifdef USE_DEBUG_PRINT_AD_VALUE
INT8U ad_test_print_list(long weight, long zerobuf, char *prev_str)
{
	char string_buf[100];

//	if(!Report_ConfirmPrint()) return OFF;
//	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
//	if(!Prt_CheckOutOfPaperFlag()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	Operation.rewindReady = OFF;
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();

	PutDataRxBuf(&CommBufPrt,(HUGEDATA char *)prev_str,strlen(prev_str));
	sprintf(string_buf, "  AD:%6ld(%6ld)", weight-zerobuf, weight);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);
    	RcptCmd_CharLF(1);

//	RcptCmd_CharTab(TICKET_MARGIN);
//	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
//	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();
	RcptCmd_ChangeMode(TO_RESTORE, ON);

//	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
//		LabCmd_AlignmentFeed();
//	}

//	if(!Prt_CheckOutOfPaper()) return OFF;
//	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}
#endif

INT8U scroll_print_list(INT16U start_id, INT16U end_id)
{
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;
	SCROLL_MESSAGE_STRUCT scroll;
	INT32U cap_addr_text,cap_addr_stype,cap_addr_stype_num,cap_addr_usemsg,cap_addr_usedate,cap_addr_week,
		cap_addr_start_date,cap_addr_end_date,cap_addr_start_time,cap_addr_end_time;

	base_addr = DFLASH_BASE + FLASH_SCROLL_AREA;

	cap_addr_text       = caption_search_by_code(0x2704) + 2;	//"Message:"
	cap_addr_stype      = caption_search_by_code(0x2705) + 2;	//"Effect:"
#ifdef CPU_PIC32MZ
	cap_addr_stype_num  = caption_search_by_code(0x2707) + 2;	//"no use this scroll"
#else
	cap_addr_stype_num  = caption_search_by_code(0x2706) + 2;	//"<-Right to Left"
#endif
	cap_addr_usemsg     = caption_search_by_code(0x2709) + 2;	//"Use message..:"
	cap_addr_usedate    = caption_search_by_code(0x2710) + 2;	//"Date Setting..:"
	cap_addr_week       = caption_search_by_code(0x1c14) + 2;	//"Day of Week..:"
	cap_addr_start_date = caption_search_by_code(0x1c15) + 2;	//"Start Date:"
	cap_addr_end_date   = caption_search_by_code(0x1c17) + 2;	//"End Date:"
	cap_addr_start_time = caption_search_by_code(0x1c16) + 2;	//"Start Time:"
	cap_addr_end_time   = caption_search_by_code(0x1c18) + 2;	//"End Time:"

	//if(end_id > 999) return OFF;


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	caption_split_by_code(0x1432, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
     	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*sizeof(SCROLL_MESSAGE_STRUCT);
		code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&scroll, sizeof(SCROLL_MESSAGE_STRUCT));
		if(!(scroll.stype == 1 || scroll.stype == 2)) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		//sprintf(string_buf, "No. ");
		receipt_caption_read(RCPTCAP_ID_LST_NO, string_buf);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		sprintf(string_buf, "%d", cur_id);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		//RcptCmd_PrintMultiLine((char *)tare_str._description, sizeof(tare_str._description), TICKET_MARGIN+TICKET_TXT_MARGIN+5);
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_text);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		//RcptCmd_CharTab(6);
		RcptCmd_PrintMultiLine((char *)scroll.text, sizeof(scroll.text), TICKET_MARGIN+TICKET_TXT_MARGIN+7);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_stype);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		//sprintf(string_buf, "%d", scroll.stype);
		if(scroll.stype > 2) {
			RcptCmd_CharTab(7);
			sprintf(string_buf, "%d", scroll.stype);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);
		} else {
#ifdef CPU_PIC32MZ
			Flash_sread(cap_addr_stype_num, string_buf, 32);
			RcptCmd_PrintMultiLine(string_buf, 32, TICKET_MARGIN+TICKET_TXT_MARGIN+7);
#else
			RcptCmd_PrintMultiLine((char *)(cap_addr_stype_num+(INT32U)scroll.stype*CAP_MSG_LENGTH), 32, TICKET_MARGIN+TICKET_TXT_MARGIN+7);
#endif
		}
				  
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_usemsg);	//"Use message..:"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		sprintf(string_buf, "%d", scroll.use);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
				  
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_usedate);	//"Date Setting..:"
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		sprintf(string_buf, "%d", scroll.smode);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
				  
		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_week);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		binary2str(scroll.week, (INT8U*)string_buf);
		string_buf[7] = 0;
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_start_date);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		rtc_makedate_str(string_buf, scroll.start_year, scroll.start_month, scroll.start_day);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_end_date);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		rtc_makedate_str(string_buf, scroll.end_year, scroll.end_month, scroll.end_day);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_start_time);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		rtc_maketime_str(string_buf, scroll.start_hour, scroll.start_min, scroll.start_sec);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_end_time);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(7);
		rtc_maketime_str(string_buf, scroll.end_hour, scroll.end_min, scroll.end_sec);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(2);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}



#ifdef USE_TRACE_STANDALONE
INT8U individualData_print_list(INT8U start_id, INT8U end_id)
{
	TRACE_INDIVIDUAL_IDENTITY indivStr;
	CAPTION_STRUCT cap;
	INT16U cur_id;   
	char string_buf[100];
	INT32U base_addr, addr;
	//INT16U code;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32U cap_addr_index;
	INT32U cap_addr_ind_no;
	INT32U cap_addr_grade;
	INT32U cap_addr_sl_date;
	INT32U cap_addr_weight;
	INT32U cap_addr_beed;
	INT32U cap_addr_farm;
	INT32U cap_addr_slaumghter;//gm 090404

#ifdef USE_TRACE_1000QTY //sewon 161201
	base_addr = EXT_MEMCARD_ADDR + FLASH_TRACE_STANDALONE_AREA;
#else
	base_addr = DFLASH_BASE + FLASH_TRACE_STANDALONE_AREA;
#endif

	cap_addr_index   = caption_search_by_code(0x1AF0) + 2;
	cap_addr_ind_no  = caption_search_by_code(0x1AF1) + 2;
	cap_addr_grade   = caption_search_by_code(0x1AF2) + 2;
	cap_addr_sl_date = caption_search_by_code(0x1AF3) + 2;
	cap_addr_weight  = caption_search_by_code(0x1AF4) + 2;
	cap_addr_beed    = caption_search_by_code(0x1AF5) + 2;
	cap_addr_farm    	= caption_search_by_code(0x1AF6) + 2;
	cap_addr_slaumghter   	= caption_search_by_code(0x1AF7) + 2;

	//if(end_id > 999) return OFF;
  	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	//if(!Prt_CheckOutOfPaperFlag()) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	caption_split_by_code(0x1372, &cap,0);
	sprintf(string_buf, (char*)cap.form+3);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
  	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%02d", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
   	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++){
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}
		addr = base_addr + (INT32U)(cur_id-1)*TRACE_INDIVIDUAL_IDENTITY_SIZE;
		//code = Flash_wread(addr);
		//if(code > 999) continue;
		Flash_sread(addr, (HUGEDATA INT8U*)&indivStr, TRACE_INDIVIDUAL_IDENTITY_SIZE);
		if(cur_id != indivStr.index) continue;

		RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);

		RcptCmd_TypeNone();

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_index);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%d", indivStr.index);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_ind_no);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%s", indivStr.individualNO);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_weight);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%ldg", get_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)indivStr.index-1)*4));
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_farm);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%s", indivStr.farm);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_slaumghter);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%s", indivStr.slaughtHouse);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_grade);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
#ifdef USE_TRACE_MEATGRADE
		sprintf(string_buf, "%d", indivStr.gradeNo);
#else
		sprintf(string_buf, "%s", indivStr.grade);
#endif
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
        
	#ifdef USE_KOR_TRACE_999
		if (indivStr.lotFlag)
		{
		}
		else
		{
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			prtlist_captitle_adjust(string_buf, cap_addr_sl_date);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharTab(6);
			sprintf(string_buf, "%02d-%02d-%02d", indivStr.slaughtDate[0],indivStr.slaughtDate[1],indivStr.slaughtDate[2]);
			PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
			RcptCmd_CharLF(1);
		}
	#else

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_sl_date);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%02d-%02d-%02d", indivStr.slaughtDate[0],indivStr.slaughtDate[1],indivStr.slaughtDate[2]);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
#endif

		RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
		prtlist_captitle_adjust(string_buf, cap_addr_beed);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharTab(6);
		sprintf(string_buf, "%s", indivStr.breed);
		PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
		RcptCmd_CharLF(1);
        
		RcptCmd_CharLF(2);

		PrintListWaitEnd();

		if(!Prt_CheckOutOfPaper()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
		if(!Prt_CheckGapErr()) {
			RcptCmd_ChangeMode(TO_RESTORE, ON);
			FlushRxBuf(&CommBufPrt);
			return OFF;
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	RcptCmd_Start();

	RcptCmd_ReceiptFeed();	
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
///		LabCmd_FormFeed();
///		LabCmd_FormFeed();
///		PrintListWaitEnd();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

#endif


#ifdef USE_INDIA_FUNCTION
INT8U print_inventory_list(INT16S dept_no, long start_id, long end_id)
{
	char string_buf[100];
	INT32U	addr;
	INT16U	key, usize;
	INT16U  offset_array[10];
	INT8U stype, decimal, dec_ch, w_decimal;
	char plu_name[65];
	long cur_id;
	INT8U flagStopPrint = OFF;
	INT8U len;
	INT32S inventory;

	decimal  =get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);
	dec_ch   =get_global_bparam(GLOBAL_DECIMAL_SIGN);
	w_decimal=get_decimal_pt(1, 0);
			
	offset_array[0] = plu_get_ptype_point(PTYPE_PLU_NAME1, &stype, &usize);	//plu name offset
	offset_array[1] = plu_get_ptype_point(PTYPE_PLU_TYPE, &stype, &usize);	//plu type offset
	offset_array[2] = plu_get_ptype_point(PTYPE_PLU_NO, &stype, &usize);	//plu no offset
	//offset_array[3] = plu_get_ptype_point(PTYPE_PRICE, &stype, &usize);	//price offset
	//offset_array[4] = plu_get_ptype_point(PTYPE_ITEM_CODE, &stype, &usize);	//item code offset
	//offset_array[5] = plu_get_ptype_point(PTYPE_PIECE, &stype, &usize);	//quantity offset


	if(!Report_ConfirmPrint()) return OFF;
	if(!Prt_CheckPeelAndHead(OFF)) return OFF;
	if(!Prt_CheckOutOfPaper()) return OFF;

	PrtSensorFlagInit();

	RcptCmd_ChangeMode(TO_RECEIPT, ON);
	RcptCmd_DefaultSet();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharLF(2);

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_SUBTITLE, TKT_LAY_N, 0, LEFT);
	sprintf(string_buf, "LIST INVENTORY");		// CAPADD_CJK
	//receipt_caption_read(RCPTCAP_ID_LST_PLULIST, string_buf);
	_strupr(string_buf);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	//sprintf(string_buf, "Dept No:%d", dept_no);
	receipt_caption_read(RCPTCAP_ID_LST_DEPT_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%d", dept_no);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
      	RcptCmd_CharLF(1);
	RcptCmd_TypeNone();

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	receipt_caption_read(RCPTCAP_ID_LST_START_NO, string_buf);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%ld, ", start_id);
	len = strlen(string_buf);
	receipt_caption_read(RCPTCAP_ID_LST_END_NO, string_buf+len);
	len = strlen(string_buf);
	sprintf(string_buf+len, "%ld", end_id);
	PutDataRxBuf(&CommBufPrt,string_buf,strlen(string_buf));
    	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
	PrintCurrentTime();
	RcptCmd_CharLF(1);

	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);

	for(cur_id = start_id; cur_id <= end_id && !flagStopPrint; cur_id++) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				break;
				//flagStopPrint = ON;
			}
		}								
		key=plu_table_seq_search(dept_no,(INT32U*)&cur_id,end_id);

 		if(key > 0) {
			addr = plu_calc_addr_by_key((long)(key-1));
			Flash_sread(addr+offset_array[0],(HUGEDATA INT8U*)plu_name, 64);
			plu_name[64] = 0;

			RcptCmd_SetFontStyle(DEFAULT_RRT_FONT_ID, RPT_FONT_NORMAL, TKT_LAY_N, 0, LEFT);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Plu No. %ld", Flash_lread(addr+offset_array[2]));
			receipt_caption_read(RCPTCAP_ID_LST_PLU_NO, string_buf);
			len = strlen(string_buf);
			sprintf(string_buf+len, "%6ld  ", Flash_lread(addr+offset_array[2]));
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			//RcptCmd_CharLF(1);

			//RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			//sprintf(string_buf, "Name");
			//receipt_caption_read(RCPTCAP_ID_LST_NAME, string_buf);
			//PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			//RcptCmd_CharTab(6);
			PutDataRxBuf(&CommBufPrt, plu_name, strlen(plu_name));
			RcptCmd_CharLF(1);

			inventory = 0;
			ReadPluInventory(dept_no, cur_id, &inventory);
			RcptCmd_CharTab(TICKET_MARGIN+TICKET_TXT_MARGIN);
			sprintf(string_buf, "Inventory");
			PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			RcptCmd_CharTab(6);
			if(Flash_bread(addr+offset_array[1]) == PLU_BY_WEIGHT)	// By-Weight PLU 일때
			{
				PrtMakeWeightString(string_buf, inventory, w_decimal, dec_ch, 0);
				PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			}
			else
			{
				sprintf(string_buf, "%ld", inventory);
				PutDataRxBuf(&CommBufPrt, string_buf, strlen(string_buf));
			}
			RcptCmd_CharLF(1);

			RcptCmd_CharLF(1);

			PrintListWaitEnd();

			if(!Prt_CheckOutOfPaper()) {
				RcptCmd_ChangeMode(TO_RESTORE, ON);
				FlushRxBuf(&CommBufPrt);
				return OFF;
			}
			if(!Prt_CheckGapErr()) {
				RcptCmd_ChangeMode(TO_RESTORE, ON);
				FlushRxBuf(&CommBufPrt);
				return OFF;
			}
		}
	}
	RcptCmd_CharTab(TICKET_MARGIN);
	RcptCmd_PrintLine(1, 0);
	RcptCmd_Start();
	
	RcptCmd_ReceiptFeed();
	PrintListWaitEnd();

	RcptCmd_ChangeMode(TO_RESTORE, ON);

	if(PrtStruct.Mode == LABEL && PrtDrvStruct.LabelGapMode == GAP) {
		LabCmd_AlignmentFeed();
	}

	if(!Prt_CheckOutOfPaper()) return OFF;
	if(!Prt_CheckGapErr()) return OFF;

	return ON;
}

#endif

#endif //#ifndef DISABLE_LIST_MENU