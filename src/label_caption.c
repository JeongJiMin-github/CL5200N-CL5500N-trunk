/*****************************************************************************
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	label_caption.c
|*  Version		:	0.1
|*  Modified		:	2004/08/20
|*  Modifier		:	CJK
****************************************************************************/
#include <stdio.h>
#include <string.h>
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
#include "prt_cmdtest.h"
#include "programmode.h"
//#include "file.h"
#include "rtc.h"
#include "debug.h"
#include "ethernet.h"
#include "main.h"
#include "initial.h"
#include "flash_app.h"
#include "common.h"
#include "adm_app.h"

//#include "caption_app.h"
#include "menu.h"
#include "label_caption.h"
#include "sale_report.h"
#pragma pack(4)
typedef struct {
	INT16U code;
	INT8U str[30];
} LABEL_CAP_STR;
#pragma pack()
/*

void label_caption_save(INT16U id, char *str)
{
	INT32U addr;
	LABEL_CAP_STR label_cap_str;

	label_cap_str.code = id;
	if (id>=MAX_LABEL_CAP_NO) return;
	memcpy((INT8U*)label_cap_str.str,(INT8U*)str,LABEL_CAP_STRUCT_SIZE-2);

	addr = DFLASH_BASE + FLASH_LABEL_CAPTION_AREA;	//0x395600
	addr += LABEL_CAP_STRUCT_SIZE*id;
	Flash_swrite(addr, (HUGEDATA INT8U *)&label_cap_str, LABEL_CAP_STRUCT_SIZE);
}
*/

INT8U label_caption_read(INT16U id, char *str)
{
	INT32U addr;
//	LABEL_CAP_STR label_cap_str;
	if (id>=MAX_LABEL_CAP_NO) return FALSE;

	addr = DFLASH_BASE + FLASH_LABEL_CAPTION_AREA;	//0x395600
	addr += LABEL_CAP_STRUCT_SIZE*id;
	if(id != Flash_wread(addr)) return FALSE;
	Flash_sread(addr+2, (HUGEDATA INT8U *)str, LABEL_CAP_STRUCT_SIZE-2);
	str[LABEL_CAP_STRUCT_SIZE-2] = 0;

	if(str[0] == 0) return FALSE;
	else return TRUE;
}

extern CURRENCY_PRICE_TYPE	CurrencyPrice;

void label_caption_conversion(char *dst_str, char *src_str, INT8U max_len, INT8U currency_order)
{
	char string_buf[34];
	INT8U i, j, k, ch;

	j = 0;
	i = 0;
	while(1) {
		//ch = Flash_bread(addr+i);
		ch = src_str[i];
		if(ch == 0) break;
		if(ch == '%') {
			i++;
			//ch = Flash_bread(addr+i);
			ch = src_str[i];
			k = 0;
			switch(ch) {
				case 'C' :
				case 'c' :
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
					if(currency_order == 0) {
						get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN, (INT8U*)string_buf, 5);
					} else {
						memcpy((INT8U*)string_buf, (INT8U*)CurrencyPrice.CurrencyString1, 5);
					}
					string_buf[5] = 0;
#else
					if(currency_order == 0) {
						get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN, (INT8U*)string_buf, 4);
					} else {
						//get_global_sparam(GLOBAL_AREA_PRICE_LASTSIGN, (INT8U*)string_buf, 4);
						memcpy((INT8U*)string_buf, (INT8U*)CurrencyPrice.CurrencyString1, 4);
					}
					string_buf[4] = 0;
#endif
					while(string_buf[k] != 0) {
						dst_str[j++] = string_buf[k];
						k++;
					}
					break;
				case 'W' :
				case 'w' :
					GetWeightSymbol(PrtWeightUnit, string_buf);// Modified by CJK 20051223
//					if(PrtWeightUnit == WEIGHT_UNIT_KG) {
//						get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)string_buf, 4);
//					} else if(PrtWeightUnit == WEIGHT_UNIT_G) {
//						get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (INT8U *)string_buf, 4);
//					} else {
//						get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)string_buf, 4);
//					}
//					string_buf[4] = 0;
					while(string_buf[k] != 0) {
						dst_str[j++] = string_buf[k];
						k++;
					}
					break;
				case 'U' :
				case 'u' :
					//unitweight = get_global_wparam(GLOBAL_DEFAULTUNIT);
					if(PrtWeightUnit == WEIGHT_UNIT_KG || PrtWeightUnit == WEIGHT_UNIT_G) {
						switch(PrtWeightUnitId)  {
							default:
							case 1:	label_caption_read(LABCAP_ID_TXT_1KG, string_buf);	break;
							case 2:	label_caption_read(LABCAP_ID_TXT_100G, string_buf);	break;
#ifdef USE_WEIGHT_UNIT_500G
							case 3:	
								if(PluType()==PLU_BY_WEIGHT) label_caption_read(LABCAP_ID_TXT_500G, string_buf);
								else label_caption_read(LABCAP_ID_TXT_1KG, string_buf);
								break;
#endif
						}
					} else  {
						switch(PrtWeightUnitId)  {
							default:
							case 1:	label_caption_read(LABCAP_ID_TXT_1LB, string_buf);	break;
//							case 2:	label_caption_read(LABCAP_ID_TXT_1_2LB, string_buf);	break;
//							case 3:	label_caption_read(LABCAP_ID_TXT_1_4LB, string_buf);	break;
						}
					}
					while(string_buf[k] != 0) {
						dst_str[j++] = string_buf[k];
						k++;
					}
					break;
				case '%' :
					dst_str[j++] = '%';
					break;
				default :
					dst_str[j++] = ch;
					break;
			}
		} else {			
			dst_str[j++] = ch;
		}
		i++;
		if(i >= max_len) break;//return FALSE;
	}
	dst_str[j++] = 0;
}

//extern INT8U PrtWeightUnit, PrtWeightUnitId;
INT8U label_caption_get_string(INT16U id, char *str, INT8U currency_order)
{
//	INT8U i, j, k, ch;// unit;
	//INT16U unitweight;
//	char string_buf[34];
	INT32U addr;
	//INT8U weightunit_id, weightunit;
	char src_str[LABEL_CAP_STRUCT_SIZE];

	if (id>=MAX_LABEL_CAP_NO) return FALSE;

	addr = DFLASH_BASE + FLASH_LABEL_CAPTION_AREA;	//0x395600
	addr += LABEL_CAP_STRUCT_SIZE*id;
	if(id != Flash_wread(addr)) return FALSE;
	addr += 2;
	Flash_sread(addr, (HUGEDATA INT8U *)src_str, LABEL_CAP_STRUCT_SIZE-2);

	label_caption_conversion(str, src_str, LABEL_CAP_STRUCT_SIZE-2, currency_order);
/*
	j = 0;
	i = 0;
	while(1) {
		//ch = Flash_bread(addr+i);
		ch = src_str[i];
		if(ch == 0) break;
		if(ch == '%') {
			i++;
			//ch = Flash_bread(addr+i);
			ch = src_str[i];
			k = 0;
			switch(ch) {
				case 'C' :
				case 'c' :
					if(currency_order == 0) {
						get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN, (INT8U*)string_buf, 4);
					} else {
						//get_global_sparam(GLOBAL_AREA_PRICE_LASTSIGN, (INT8U*)string_buf, 4);
						memcpy((INT8U*)string_buf, (INT8U*)CurrencyPrice.CurrencyString1, 4);
					}
					string_buf[4] = 0;
					while(string_buf[k] != 0) {
						str[j++] = string_buf[k];
						k++;
					}
					break;
				case 'W' :
				case 'w' :
					GetWeightSymbol(PrtWeightUnit, string_buf);// Modified by CJK 20051223
//					if(PrtWeightUnit == WEIGHT_UNIT_KG) {
//						get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)string_buf, 4);
//					} else if(PrtWeightUnit == WEIGHT_UNIT_G) {
//						get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (INT8U *)string_buf, 4);
//					} else {
//						get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)string_buf, 4);
//					}
//					string_buf[4] = 0;
					while(string_buf[k] != 0) {
						str[j++] = string_buf[k];
						k++;
					}
					break;
				case 'U' :
				case 'u' :
					//unitweight = get_global_wparam(GLOBAL_DEFAULTUNIT);
					if(PrtWeightUnit == WEIGHT_UNIT_KG || PrtWeightUnit == WEIGHT_UNIT_G) {
						switch(PrtWeightUnitId)  {
							default:
							case 1:	label_caption_read(LABCAP_ID_TXT_1KG, string_buf);	break;
							case 2:	label_caption_read(LABCAP_ID_TXT_100G, string_buf);	break;
						}
					} else  {
						switch(PrtWeightUnitId)  {
							default:
							case 1:	label_caption_read(LABCAP_ID_TXT_1LB, string_buf);	break;
//							case 2:	label_caption_read(LABCAP_ID_TXT_1_2LB, string_buf);	break;
//							case 3:	label_caption_read(LABCAP_ID_TXT_1_4LB, string_buf);	break;
						}
					}
					while(string_buf[k] != 0) {
						str[j++] = string_buf[k];
						k++;
					}
					break;
				case '%' :
					str[j++] = '%';
					break;
				default :
					str[j++] = ch;
					break;
			}
		} else {			
			str[j++] = ch;
		}
		i++;
		if(i >= LABEL_CAP_STRUCT_SIZE-2) break;//return FALSE;
	}
	str[j++] = 0;
*/

	if(str[0] == 0) return FALSE;
	return TRUE;
}

/*

void receipt_caption_save(INT16U id, char *str)
{
	INT32U addr;
	LABEL_CAP_STR label_cap_str;

	label_cap_str.code = id+RCPTCAP_BASE;
	if (id>=MAX_LABEL_CAP_NO-RCPTCAP_BASE) return;
	memcpy((INT8U*)label_cap_str.str,(INT8U*)str,LABEL_CAP_STRUCT_SIZE-2);

	addr = DFLASH_BASE + FLASH_LABEL_CAPTION_AREA;	//0x395600
	addr += LABEL_CAP_STRUCT_SIZE*(INT32U)(id+RCPTCAP_BASE);
	Flash_swrite(addr, (HUGEDATA INT8U *)&label_cap_str, LABEL_CAP_STRUCT_SIZE);
}
*/

INT8U receipt_caption_read(INT16U id, char *str)
{
	INT32U addr;
	if (id>=MAX_LABEL_CAP_NO-RCPTCAP_BASE) return FALSE;

	addr = DFLASH_BASE + FLASH_LABEL_CAPTION_AREA;	//0x395600
	addr += LABEL_CAP_STRUCT_SIZE*(INT32U)(id+RCPTCAP_BASE);
	if((id+RCPTCAP_BASE) != Flash_wread(addr)) return FALSE;
	Flash_sread(addr+2, (HUGEDATA INT8U *)str, LABEL_CAP_STRUCT_SIZE-2);
	str[LABEL_CAP_STRUCT_SIZE-2] = 0;

	if(str[0] == 0) return FALSE;
	else return TRUE;
}


