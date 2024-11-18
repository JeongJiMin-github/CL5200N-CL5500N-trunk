/*****************************************************************************
|*			
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	menu.c
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/09/16
|*  Description		:	CLP Menu routine    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "key.h"
#include "key_typedef.h"
#include "key_app.h"
#include "main.h"
#include "prt_common.h"
#include "menu.h"
#include "dsp_vfd7.h"
#include "common.h"
#include "ram.h"
#include "initial.h"
#include "flash.h"
#include "flash_app.h"
#include "caption_app.h"
#include "cursor.h"
#include "pgm_app.h"
#include "dsp_app.h"
#include "initial_glb.h"
#include "adm_app.h"
#include "common_sunday.h"

#define	SCROLL_MSG_SIZE	32
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#define	SCROLL_MSG_MAX_LINE	4
#else
#define	SCROLL_MSG_MAX_LINE	1
#endif
INT16S NUM_MENU_PER_PAGE;

char Scrl_Msg[SCROLL_MSG_MAX_LINE][SCROLL_MSG_SIZE];
INT8U Scrl_InId, Scrl_OutId;
INT8U Scrl_FondId;
INT8U Scrl_Msg_UseLogo;

MENU_STR Menu[MAX_MENU_INDEX];
INT8U Menu_order_table[MAX_MENU_INDEX];
char MenuHead[50];
INT8U MenuIndex;//, MenuInhibitCnt;
INT8U MenuStartPage;

INT8U Menu_return_last_item_arrow_dn = 0;
INT8U Menu_return_first_item_arrow_up = 0;
INT16U CallbackEditPosition_x;
INT16U CallbackEditPosition_y;

INT8U CallbackMenuId;

void  (*end_function)(long a);
void  (*end_function_str)(INT8U *str);
void (*menu_key_function)(void);
INT8U (*check_function)(INT32U a);
INT8U (*check_function_str)(INT8U *str);
INT8U MenuRefresh;
INT8U MenuCapAdjust = OFF;
INT8U MenuNumLinePerPage;
INT8U MenuDefaultCallbackMode;
INT8U MenuReorder;

void Menu_Init(void)
{
	MenuIndex = 0;
	MenuStartPage = 0;
	MenuRefresh = OFF;
	MenuHead[0] = 0;
	//MenuDateType = rtc_get_date_format();
	MenuCapAdjust = OFF;
	MenuNumLinePerPage = Startup.line_max;
	MenuDefaultCallbackMode = 0; // 0:right
	MenuReorder = 0;
}

void Menu_Init_WithPage(INT8U start_page)
{
	MenuIndex = 0;
	MenuStartPage = start_page;
	MenuRefresh = OFF;
	MenuHead[0] = 0;
	//MenuDateType = rtc_get_date_format();
	MenuCapAdjust = OFF;
	MenuNumLinePerPage = Startup.line_max;
	MenuDefaultCallbackMode = 0; // 0:right
}

void Menu_HeadCreate(HUGEDATA char *str)
{
	strcpy(MenuHead, (const char *)str);
}

//void Menu_TreeCreate(INT8U id, HUGEDATA char *str)
//{
//	Menu[MenuIndex].id = id;
//	strcpy(Menu[MenuIndex].str, (const char *)str);
//	MenuIndex++;
//}

//INT8U Menu_TreeDisplay(HUGEDATA INT8U *page)
//{
//	POINT disp_p;
//	INT8U start_idx;		// 각 페이지의 첫 Menu index
//	INT8U i;
//	char string_buf[50];
//
//	start_idx = *page * NUM_MENU_PER_PAGE;
//
//	while(1) {
//		Dsp_Fill_Buffer(0);
//
//		menu_writetitle_str(MenuHead);
//		menu_writepage((MenuIndex-1)/NUM_MENU_PER_PAGE+1, start_idx/NUM_MENU_PER_PAGE+1);
//
//		for(i = 0; i < NUM_MENU_PER_PAGE; i++) {
//			if(start_idx + i >= MenuIndex) break;
//			sprintf(string_buf, "%d.%s", start_idx + i + 1, Menu[start_idx + i].str);
//			disp_p = point(8*(i+1), 0);
//			Dsp_Write_String(disp_p,string_buf);
//		}
//		Dsp_Diffuse();
//		while (!KEY_Read());
//		switch(KeyDrv.Type) {
//			case KEY_TYPE_NUMERIC :
//				if(KeyDrv.CnvCode > MenuIndex) BuzOn(2);
//				else if(KeyDrv.CnvCode == KEY_NUM_0) {
//					return Menu[9].id;
//				} else {
//					return Menu[KeyDrv.CnvCode - 1].id;
//				}
//				break;
//			case KEY_TYPE_FUNCTION :
//				switch(KeyDrv.CnvCode) {
//					case KP_PAGE_DN :
//					case KP_ARROW_DN :
//						if(start_idx + NUM_MENU_PER_PAGE >= MenuIndex) BuzOn(2);
//						else {
//							BuzOn(1);
//							start_idx += NUM_MENU_PER_PAGE;
//							*page = *page + 1;
//						}
//						break;
//					case KP_PAGE_UP :
//					case KP_ARROW_UP :
//						if(start_idx == 0) BuzOn(2);
//						else {
//							BuzOn(1);
//							start_idx -= NUM_MENU_PER_PAGE;
//							*page = *page - 1;
//						}
//						break;
//					case KP_ESC :
//						BuzOn(1);
//						return 0xff;
//					default :
//						BuzOn(2);
//						break;
//				}
//				break;
//			default :
//				BuzOn(2);
//				break;
//		}
//	}
//}

/*void Menu_InputCreate(INT8U id, HUGEDATA char *str, INT8U type, INT8U start, INT16U w_length, INT16U length, INT32U max, INT32U min, INT8U decimal, HUGEDATA INT32U *pval, HUGEDATA INT8U *pstr, HUGEDATA INT8U *form)
{
	Menu[MenuIndex].id = id;
	strcpy(Menu[MenuIndex].str, (const char *)str);
	Menu[MenuIndex].type = type;
	Menu[MenuIndex].start = start;
	Menu[MenuIndex].length = length;
	Menu[MenuIndex].w_length = w_length;
	Menu[MenuIndex].max = max;
	Menu[MenuIndex].min = min;
	Menu[MenuIndex].pval = pval;
	Menu[MenuIndex].pstr = pstr;
	Menu[MenuIndex].decimal = decimal;
	Menu[MenuIndex].function = NULL;
	strcpy((char *)Menu[MenuIndex].form, (const char *)form);
	MenuIndex++;
}
*/
void Menu_InputCreate(INT8U id, HUGEDATA char *str, INT8U type, INT8U start, INT16U w_length, 
                               INT16U length, INT32U max, INT32U min, INT8U decimal, HUGEDATA INT32U *pval, 
                               HUGEDATA INT8U *pstr, HUGEDATA INT8U *form, void (*function)(long))
{
	INT8U i;
	
	if (MenuIndex >= MAX_MENU_INDEX) return;
	Menu[MenuIndex].id 	 = id;
	Menu[MenuIndex].type 	 = type;
	Menu[MenuIndex].start 	 = start;
	Menu[MenuIndex].length 	 = length;
	Menu[MenuIndex].w_length = w_length;
	Menu[MenuIndex].max 	 = max;
	Menu[MenuIndex].min 	 = min;
	Menu[MenuIndex].pval 	 = pval;
	Menu[MenuIndex].pstr 	 = pstr;
	Menu[MenuIndex].decimal  = decimal;
	Menu[MenuIndex].function = function;

	Menu[MenuIndex].inhibit  = OFF;	// Added by CJk 20041126
	Menu[MenuIndex].disable  = OFF;
	Menu[MenuIndex].end_function = NULL;
	for (i = 0; i < MAX_MENU_KEY_FUNCTION; i++)
	{
		Menu[MenuIndex].callback_key[i] = 0;
		Menu[MenuIndex].key_function[i] = NULL;
	}
	Menu[MenuIndex].clerk_one_touch = OFF;
	Menu[MenuIndex].properties = 0;
#ifdef USE_CHINA_PINYIN
	if (type == MENU_TYPE_STR)
		Menu[MenuIndex].properties |= MENU_P_CHINA_PINY;
#endif
	Menu[MenuIndex].page_end = OFF;
	Menu[MenuIndex].symbol   = 0;
	Menu[MenuIndex].str_adjustable = OFF;
	Menu[MenuIndex].callback_mode = MenuDefaultCallbackMode; // 0:right, 1:below
	Menu[MenuIndex].general_flag = 0;
	Menu[MenuIndex].check_function = NULL;
	Menu[MenuIndex].key_mode = 0xff;
	Menu[MenuIndex].char_fixed = '.';

	if (str) strcpy(Menu[MenuIndex].str, (const char *)str);
	if (form) strcpy((char *)Menu[MenuIndex].form, (const char *)form);
	MenuIndex++;
}

INT8U Menu_GetMenuId(INT8U id)
{
	INT8U ret_id, i;

	ret_id = 0xff;
	for(i = 0; i < MenuIndex; i++) {
		if(Menu[i].id == id) ret_id = i;
	}
	return ret_id;
}

INT8U Menu_AddEndFunction(INT8U id, void (*function)())
{
	INT8U i;

//	for(i = 0; i < MenuIndex; i++) {
//		if(Menu[i].id == id) {
//			break;
//		}
//	}
	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].end_function = function;

	return i;
}

INT8U Menu_AddCheckFunction(INT8U id, INT8U (*function)())
{
	INT8U i;

//	for(i = 0; i < MenuIndex; i++) {
//		if(Menu[i].id == id) {
//			break;
//		}
//	}
	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].check_function = function;

	return i;
}

INT8U Menu_AddKeyFunction(INT8U id, INT16U key, void (*function)())
{
	INT8U i;
	INT8U key_id;

//	for(i = 0; i < MenuIndex; i++) {
//		if(Menu[i].id == id) {
//			break;
//		}
//	}
	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	for (key_id = 0; key_id < MAX_MENU_KEY_FUNCTION; key_id++)
	{
		if (Menu[i].callback_key[key_id] == 0)
		{
			Menu[i].callback_key[key_id] = key;
			Menu[i].key_function[key_id] = function;
			break;
		}
	}

	return i;
}

void Menu_SetNextPage(void)
{
	Menu[MenuIndex-1].page_end = ON;
}

/////////////////
// MENU_P_USE_LINEFEED	0x0001
// MENU_P_DSP_POSITION	0x0002
// MENU_P_PAGEUPDN_KEY	0x0004
INT8U Menu_SetProperties(INT8U id, INT16U properties)
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].properties = properties;

	return i;
}

INT8U Menu_SetDisable(INT8U id, INT8U onoff)
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].disable = onoff;

	return i;
}

INT8U Menu_SetOneTouch(INT8U id, INT8U onoff)
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].clerk_one_touch = onoff;

	return i;
}

INT8U Menu_SetInhibit(INT8U id, INT8U onoff)
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].inhibit = onoff;

	return i;
}

INT8U Menu_SetSymbol(INT8U id, INT8U symbol_id) //1:kg, 2:lb, 3:g, 11:$, 21:pcs, 31:%
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].symbol = symbol_id;

	return i;
}

INT8U Menu_SetStrAdjustable(INT8U id, INT8U onoff)
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].str_adjustable = onoff;

	return i;
}

INT8U Menu_SetCallbackMode(INT8U id, INT8U mode)	// 0 = right, 1 = below
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].callback_mode = mode;

	return i;
}

INT8U Menu_SetGeneralFlag(INT8U id, INT8U mode)
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].general_flag = mode;

	return i;
}

INT8U Menu_SetKeyMode(INT8U id, INT8U mode)
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].key_mode = mode;

	return i;
}

INT8U Menu_SetFixedCharacter(INT8U id, INT8U character)
{
	INT8U i;

	i = Menu_GetMenuId(id);
	if(i == 0xff) return 0xff;
	Menu[i].char_fixed = character;

	return i;
}

void Menu_ModifyRetVal(INT8U id, INT32U val)
{
	INT8U i;
	for(i = 0; i < MenuIndex; i++) {
		//if(Menu[i].id == id && Menu[i].pval != NULL) {
		if(Menu[i].id == id && Menu[i].type < MENU_TYPE_STR) {
			*Menu[i].pval = val;
			break;
		}
	}
}

void Menu_ModifyRetStr(INT8U id, HUGEDATA INT8U *str, INT8U size)
{
	INT8U i;
	for(i = 0; i < MenuIndex; i++) {
		if(Menu[i].id == id && Menu[i].pstr != NULL) {
			memcpy(Menu[i].pstr, str, size);	// Menu[i].length
			break;
		}
	}
}


void Menu_StrReAdjust(char *str, INT16U len, INT16U max_size)
{
	INT16S k,j;
	INT8U form_len, input_x;

	form_len = strlen(str);
#ifdef HEBREW_FONT
	strcat((char *)str,"]");
#else
	strcat((char *)str,"[");
#endif

	for (j=0; j<len; j++) {
		k=form_len+j;
		if (k>=max_size) break;
		str[k]=0x20;
	}
	k=form_len+len;
	if (k>=max_size) {
		k=max_size;
		len=k-input_x;
	}
#ifdef HEBREW_FONT
	str[k++]='[';
#else
	str[k++]=']';
#endif
	str[k  ]=0;
}


extern INT8U check_barcode_str(char *str, char *form_str);
//INT8U MenuDateType = 0;
extern INT8U   keyin_clerk_onetouch;
extern char barcodetype_length[];
extern BOOLEAN keyin_get_speed_key;
INT16U CurMenuCode;
extern INT8U barcode_form_len(char *str);
extern  INT16U weighing_tare_input(INT8U dsp_mode, INT8U decimal, long *ret_val, INT8U leng, INT8U y, INT16U x);
extern  long str_2sellbydate(char *str);
extern  void date_2str(INT32U date,char *str);
//extern void  menu_display_menucode(INT16U code,INT8U set);
#ifdef USE_CHINA_PINYIN
extern int    china_pos_x,china_pos_y;
extern INT8U  china_pinyin;
#endif
INT8U Menu_InputDisplay(void)
{
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	POINT disp_p;
#endif
	INT8S v_start;		// 각 페이지의 첫 Menu index
	INT8S cur_idx;
	INT8S dsp_idx;
	INT8S i, j, k, l;
	char string_buf[50];
	INT8U x_gab,y_gab;
	INT16U result;
	INT16S  start_line;
	char loop;
	INT8U backup_directdraw;
	INT8U pass_flag;
	INT8U val_1byte;
	INT16U val_2byte;
	INT32U val;
	INT32U backup_val; //se-hyung 080109
	INT16U backup_val2;
	INT8U  backup_val1;
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
	char symbol[6];
#else
	char symbol[5];
#endif	
	INT8U dsp2org_idx[MAX_MENU_INDEX];
	INT8U view_idx;
	INT8U MenuInhibitCnt;
	INT8U backup_pgmmode;
#ifdef CL5200_PROJECT
	INT8U tare_val_type;
#endif
	INT32S temp_value;
	INT8U check_result;
	INT8U old_keymode;
	INT8S prev_page;
	INT8S cur_page;
	INT8S total_page;
	INT8S start_idx_by_page[MAX_MENU_INDEX];	// page별 시작 idx
	INT8S page_no_by_idx[MAX_MENU_INDEX];		// idx별 page no.
	INT8U dstring[10];
	INT8U key_excution;
#ifdef USE_CHINA_PINYIN
	INT8S temp_y;
#endif

	MenuInhibitCnt = 0;
	v_start = 0;
	loop = ON;
	cur_idx = 0;
	cur_page = 0;
	for(i = 0; i < MenuIndex; i++) {
		//if(Menu[i].type != MENU_TYPE_FIX && Menu[i].disable == OFF) {
		if(Menu[i].type != MENU_TYPE_FIX) {
			cur_idx = i;
			break;
		}
	}

//	start_line=1;
	y_gab  = display_font_get_height();
	x_gab  = display_font_get_width();
	if (Startup.menu_type==0) start_line=0;
	else start_line = Startup.menu_type;
	//// Assign Flash Memeory. 3 bits
	//    0. Error
	//    1. No Round
	//    2. Round Off
	//    3. Round Down
	//    4. Round Up
#ifdef CL5200_PROJECT
	tare_val_type = get_global_bparam(GLOBAL_TAREINPUT_FLAG);
#endif
 	while(1) {

		loop = ON;
		backup_directdraw = DspStruct.DirectDraw;
		DspStruct.DirectDraw = 0;
 		Dsp_Fill_Buffer(0);

		menu_display_menucode(CurMenuCode, 1);

		MenuInhibitCnt = 0;
		l = 0;
		total_page = 0;
		for(k = 0, j = 0; k < MenuIndex; k++) {
			if(Menu[k].inhibit) 
			{
				MenuInhibitCnt++;
			}
			else
			{
				if (l == 0) 
				{
					start_idx_by_page[total_page] = j;
					total_page++;
				}
				dsp2org_idx[j] = k;
				page_no_by_idx[j] = total_page - 1;
				if (Menu[k].page_end)	// page내에서의 마지막 위치 
				{
					l = 0;
				}
				else
				{
					l++;
				}
				if (l >= MenuNumLinePerPage) 
				{
					l = 0;
				}
				j++;
			}
		}
		dsp2org_idx[j] = 0xff;
#ifdef COUNTRY_USA
		if(MenuReorder) {
			for(i = 0; i < MAX_MENU_INDEX ; i++)
				dsp2org_idx[i] = Menu_order_table[i];
		}
#endif
		if (MenuHead[0])
		{
			if (status_run.run_mode!=RUN_SALE)
			{
				menu_writetitle_str(MenuHead);
				//menu_writepage((((MenuIndex-MenuInhibitCnt)-1)/MenuNumLinePerPage)+1+MenuStartPage, (cur_idx/MenuNumLinePerPage)+1+MenuStartPage);
				menu_writepage(total_page+MenuStartPage, cur_page+1+MenuStartPage);
			}
		}

		dsp_idx = v_start;
#if defined(USE_CL5200_DISPLAY_DEFAULT) && defined(USE_TRACE_STANDALONE)
		if(KeyDrv.CnvCode == KS_EDIT_INDIVIDUAL)
			MenuNumLinePerPage = 1;
#endif	
#ifdef USE_ARAB_CONVERT
        if(CurMenuCode == 0x1451)
            MenuNumLinePerPage = 2;
#endif
		for(i = 0; i < MenuNumLinePerPage; i++) {
			view_idx = dsp2org_idx[dsp_idx];
			if(dsp_idx >= (MenuIndex-MenuInhibitCnt)) break;
			CallbackMenuId = Menu[view_idx].id;
#if !defined(USE_SINGLE_LINE_GRAPHIC)
			disp_p = point((i+start_line)*y_gab, 0);
#endif
			strcpy(string_buf, Menu[view_idx].str);
			KeyInSet.char_fixed = Menu[view_idx].char_fixed;
			
#ifndef USE_THAI_FONT //원래 버그 있음, 태국은 그냥 스킵함. 
			if (MenuCapAdjust && Menu[view_idx].str_adjustable)
			{
				string_buf[Menu[view_idx].start] = 0;
				Menu_StrReAdjust(string_buf, Menu[view_idx].w_length, sizeof(string_buf));
			}
#endif

			// Added by CJK 20041126
			if(Menu[view_idx].disable == ON) {
				j = 0;
				while(string_buf[j] != 0) {
#ifdef HEBREW_FONT
					if(string_buf[j] == ']' || string_buf[j] == '[') string_buf[j] = ' ';
#else
					if(string_buf[j] == '[' || string_buf[j] == ']') string_buf[j] = ' ';
#endif
					j++;
					if(j >= 50) break;
				}
				//Dsp_Write_String(disp_p, string_buf);
				//continue;
			}
#ifndef USE_SINGLE_LINE_GRAPHIC
			
			switch (Menu[view_idx].symbol)
			{
				case 1:	//WEIGHT_UNIT_KG:
					//Flash_sread(addr_global+GLOBAL_WEIGHT_SYMBOL_1,(HUGEDATA INT8U*)symbol,4);
					get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (HUGEDATA INT8U *)symbol, 4);
					break;
				case 2: //WEIGHT_UNIT_LB:
					//Flash_sread(addr_global+GLOBAL_WEIGHT_LBSYMBOL_1,(HUGEDATA INT8U*)symbol,4);
					get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (HUGEDATA INT8U *)symbol, 4);
					break;
				case 3: //WEIGHT_UNIT_G:
					//Flash_sread(addr_global+GLOBAL_WEIGHT_SYMBOL_2,(HUGEDATA INT8U*)symbol,4);
					get_global_sparam(GLOBAL_WEIGHT_SYMBOL_2, (HUGEDATA INT8U *)symbol, 4);
					break;
				case 4: // WEIGHT_UNIT_oz: 
					//Flash_sread(addr_global+GLOBAL_WEIGHT_LBSYMBOL_2,(HUGEDATA INT8U*)symbol,4);				
					get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_2, (HUGEDATA INT8U *)symbol, 4);
					break;
				case 11: // Price Symbol 
					//Flash_sread(addr_global+GLOBAL_AREA_PRICE_PRIMARYSIGN,(HUGEDATA INT8U*)symbol,4);
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
					get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN, (HUGEDATA INT8U *)symbol, 5);
#else
					get_global_sparam(GLOBAL_AREA_PRICE_PRIMARYSIGN, (HUGEDATA INT8U *)symbol, 4);
#endif
					break;
				case 21: // pcs 
					//Flash_sread(addr_global+GLOBAL_AREA_PRICE_PRIMARYSIGN,(HUGEDATA INT8U*)symbol,4);
					get_global_sparam(GLOBAL_PCS_SYMBOL, (HUGEDATA INT8U *)symbol, 4);
					break;
				case 31: // Percent 
					//Flash_sread(addr_global+GLOBAL_AREA_PRICE_PRIMARYSIGN,(HUGEDATA INT8U*)symbol,4);
					symbol[0] = '%';
					symbol[1] = 0;
					break;
				case 41: //Length(mm)
					symbol[0] = 'm';
					symbol[1] = 'm';
					break;
				default:
					symbol[0] = 0;
					break;
			}
#ifdef USE_EXTEND_PRICE_PRIMARYSIGN
			if(Menu[view_idx].symbol == 11) symbol[5] = 0;
			else symbol[4] = 0;
#else
			symbol[4] = 0;
#endif
			strcat(string_buf, symbol);
			/////////////////////////
#endif
#if defined(USE_SINGLE_LINE_GRAPHIC)
			if (Menu[view_idx].type == MENU_TYPE_IP)
			{
				display_clear(DISPLAY_WEIGHT);
				display_clear(DISPLAY_UNITPRICE);
				display_string(DISPLAY_WEIGHT,(INT8U *)string_buf);
			}
			else
			{
				if (status_run.run_mode==RUN_SALE)
				{
					display_clear(DISPLAY_WEIGHT);
					display_clear(DISPLAY_TARE);
				}
				display_clear(DISPLAY_UNITPRICE);
				display_string(DISPLAY_UNITPRICE,(INT8U *)string_buf);
			}
			display_clear(DISPLAY_PRICE);
#else
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
			hebrew_codetemp[strlen(string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
			arab_codetemp[strlen(string_buf)-Arab_combine_cnt] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#elif defined(USE_ENG_BASED_ARAB_SUPPORT)
			Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 1); 
			eng_codetemp[strlen(string_buf)] = 0;
			Dsp_Write_1stringLong(disp_p, eng_codetemp);
#else
			Dsp_Write_String(disp_p, string_buf);
#endif
#endif
			
#if defined(USE_SINGLE_LINE_GRAPHIC)
			CallbackEditPosition_x = 0;
			CallbackEditPosition_y = 0;
#else
			if (Menu[view_idx].callback_mode == 0)	// 0 = right
			{
				CallbackEditPosition_x = (strlen(Menu[view_idx].str) + 1);
				CallbackEditPosition_y = (i+start_line);
			}
			else if(Menu[view_idx].callback_mode == 1)	// 1 = below
			{
#ifdef USE_PERSIA_DEFAULT
				CallbackEditPosition_x = 1;
#else
				CallbackEditPosition_x = 3;
#endif
				CallbackEditPosition_y = (i+start_line) + 1;
			}
			else 	// 2 = bottom
			{
#ifdef USE_PERSIA_DEFAULT
				CallbackEditPosition_x = 1;
#else
				CallbackEditPosition_x = 3;
#endif
				CallbackEditPosition_y = Startup.menu_type + MenuNumLinePerPage;
			}
#endif
			switch(Menu[view_idx].type) {
				case MENU_TYPE_NUM :
					if(Menu[view_idx].function != NULL){
						keyin_setcallback(Menu[view_idx].function);
					}
					//if(Menu[v_start+i].function != NULL && cur_idx == 0){
					//keyin_ulong(KI_DEFAULT, KI_FORM_NORMAL, Menu[v_start+i].decimal, (long*)Menu[v_start+i].pval, Menu[v_start+i].w_length, (i+start_line)*y_gab, Menu[v_start+i].start*x_gab, 0, KI_PASS);
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
					keyin_value(4, KI_GRAPHIC, KI_FORM_ILNN, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_value(4, KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#else
					keyin_value(4, KI_DEFAULT, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#endif
#endif
					memcpy(&val, (void*)Menu[view_idx].pval, 4);
					keyin_clearcallback();
					break;
				case MENU_TYPE_NUM_1BYTE :
					if(Menu[view_idx].function != NULL){
						keyin_setcallback(Menu[view_idx].function);
					}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_value(1, KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#else
					keyin_value(1, KI_DEFAULT, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#endif
					memcpy(&val_1byte, (void*)Menu[view_idx].pval, 1);
					val = (INT32U)val_1byte;
					keyin_clearcallback();
					break;
				case MENU_TYPE_NUM_2BYTE :
					if(Menu[view_idx].function != NULL){
						keyin_setcallback(Menu[view_idx].function);
					}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_value(2, KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#else
					keyin_value(2, KI_DEFAULT, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#endif
					memcpy(&val_2byte, (void*)Menu[view_idx].pval, 2);
					val = (INT32U)val_2byte;
					keyin_clearcallback();
					break;
				case MENU_TYPE_HEX :
					if(Menu[view_idx].function != NULL){
						keyin_setcallback(Menu[view_idx].function);
					}
					//if(Menu[v_start+i].function != NULL && cur_idx == 0){
#ifdef USE_ARAB_CONVERT
					keyin_ulong_hex(KI_DEFAULT, KI_FORM_ILNN, Menu[view_idx].decimal, (INT32U*)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_ulong_hex(KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U*)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#else
					keyin_ulong_hex(KI_DEFAULT, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U*)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#endif
#endif
					memcpy(&val, (void*)Menu[view_idx].pval, 4);
					keyin_clearcallback();
					break;
				case MENU_TYPE_IP :
#if defined(USE_SINGLE_LINE_GRAPHIC)
					keyin_string(KI_IP_MODE, KI_SEGMENT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start, 0, 0, KI_PASS);
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_string(KI_IP_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else
					keyin_string(KI_IP_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
#endif
					break;
				case MENU_TYPE_DATE :
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_string(KI_IP_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else
					keyin_string(KI_IP_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
					break;
				case MENU_TYPE_TIME :
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_string(KI_IP_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else					
					keyin_string(KI_IP_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
					break;
				case MENU_TYPE_FORM :
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_string(KI_FORM_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else
					keyin_string(KI_FORM_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
					break;
				case MENU_TYPE_YN :
					if(Menu[view_idx].function != NULL){
						keyin_setcallback(Menu[view_idx].function);
					}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_yesno(KI_GRAPHIC, (long*)Menu[view_idx].pval, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, KI_PASS);
#else
					keyin_yesno(KI_DEFAULT, (long*)Menu[view_idx].pval, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, KI_PASS);
#endif
					memcpy(&val, Menu[view_idx].pval, 4);
					keyin_clearcallback();
					break;
				case MENU_TYPE_STR :
					keyin_string(KI_FREE_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, Menu[view_idx].properties, 0, KI_PASS);
					break;
				case MENU_TYPE_FIX :
					if(Menu[view_idx].w_length) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						keyin_string(KI_FREE_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else
						keyin_string(KI_FREE_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
					}
					break;
				case MENU_TYPE_BAR :
					keyin_string(KI_BARCODE_FORM, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
					break;
				case MENU_TYPE_WEIGHT:
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_value(4, KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#else
					keyin_value(4, KI_DEFAULT, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
#endif
					//weighing_tare_input(KI_DEFAULT, Menu[view_idx].decimal, Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab);
					break;
				case MENU_TYPE_DATENUM:
					memcpy(&val, (void*)Menu[view_idx].pval, 4);
					//use pstr for string buffer for date string
					Menu[view_idx].pstr = dstring;
					date_2str(val,(char *)Menu[view_idx].pstr);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_string(KI_SNUMBER, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else
					keyin_string(KI_SNUMBER, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
					break;
				case MENU_TYPE_PASSWORD :
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
					keyin_string(KI_FREE_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, Menu[view_idx].properties, 1, KI_PASS);
#else
					keyin_string(KI_FREE_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, Menu[view_idx].properties, 1, KI_PASS);
#endif
					break;
			}
			// Added by CJK 20041221
			if(MenuRefresh == OFF && Menu[view_idx].end_function && dsp_idx >= cur_idx	// dsp_idx check. 현재 cursor보다 위의 필드는 end_function 업데이트 안 함. "dsp_idx >= cur_idx" 없을 경우 end_function에 의해 입력중인 값이 사라질 수 있음(재호출)
				&& (Menu[view_idx].type == MENU_TYPE_NUM || Menu[view_idx].type == MENU_TYPE_NUM_1BYTE || Menu[view_idx].type == MENU_TYPE_NUM_2BYTE || Menu[view_idx].type == MENU_TYPE_HEX || Menu[view_idx].type == MENU_TYPE_YN || Menu[view_idx].type == MENU_TYPE_STR)) {
				if (Menu[view_idx].type == MENU_TYPE_STR)
				{
					end_function_str = Menu[view_idx].end_function;
					(*end_function_str)(Menu[view_idx].pstr);
				}
				else
				{
					end_function = Menu[view_idx].end_function;
					(*end_function)(val);
				}
				if (MenuRefresh)
				{
					MenuInhibitCnt = 0;
					l = 0;
					total_page = 0;
					for(k = 0, j = 0; k < MenuIndex; k++) {
						if (Menu[k].inhibit) 
						{
							MenuInhibitCnt++;
						}
						else
						{
							if (l == 0) 
							{
								start_idx_by_page[total_page] = j;
								total_page++;
							}
							dsp2org_idx[j] = k;
							page_no_by_idx[j] = total_page - 1;
							if (Menu[k].page_end)	// page내에서의 마지막 위치 
							{
								l = 0;
							}
							else
							{
								l++;
							}
							if (l >= MenuNumLinePerPage) 
							{
								l = 0;
							}
							j++;
						}
					}
					dsp2org_idx[j] = 0xff;
					//menu_writepage((((MenuIndex-MenuInhibitCnt)-1)/MenuNumLinePerPage)+1+MenuStartPage, (cur_idx/MenuNumLinePerPage)+1+MenuStartPage);
					menu_writepage(total_page+MenuStartPage, cur_page+1+MenuStartPage);
					MenuRefresh = OFF;
				}
			}
			if(Menu[view_idx].page_end) break;
			dsp_idx++;
		}
		Dsp_Diffuse();
		MenuCapAdjust = OFF;
		DspStruct.DirectDraw = backup_directdraw;
		while(loop) {
			view_idx = dsp2org_idx[cur_idx];
			CallbackMenuId = Menu[view_idx].id;
			if(Menu[view_idx].disable == ON)
			{
				//cur_idx++;
				for(i = 0; cur_idx + i < (MenuIndex-MenuInhibitCnt) && cur_idx + i < start_idx_by_page[cur_page] + MenuNumLinePerPage; i++) {
					if(Menu[dsp2org_idx[cur_idx + i]].type != MENU_TYPE_FIX && Menu[dsp2org_idx[cur_idx + i]].disable == OFF) {
						break;
					}
				}
				if(cur_idx + i >= (MenuIndex-MenuInhibitCnt)) {	// 이후 데이터가 모두 MENU_TYPE_FIX이면
					//cur_idx = cur_idx - 1;	// 원래 위치
					if(result == KP_ENTER) {
						BuzOn(1);
						return MENU_RET_SAVE;
					}
					BuzOn(2);
					i = 0;// 원래 위치
				}
				else if (cur_idx + i >= start_idx_by_page[cur_page] + MenuNumLinePerPage) // 1page를 넘어갔을 경우
				{
					i = 0;// 원래 위치
				}
				cur_idx = cur_idx + i;
				view_idx = dsp2org_idx[cur_idx];
				CallbackMenuId = Menu[view_idx].id;
			}
			KeyInSet.char_fixed = Menu[view_idx].char_fixed;
#if defined(USE_SINGLE_LINE_GRAPHIC)
			CallbackEditPosition_x = 0;
			CallbackEditPosition_y = 0;
#else
			if (Menu[view_idx].callback_mode == 0)	// 0 = right
			{
				CallbackEditPosition_x = (strlen(Menu[view_idx].str) + 1);
				CallbackEditPosition_y = (cur_idx-(INT8U)v_start+start_line);
			}
			else if(Menu[view_idx].callback_mode == 1)	// 1 = below
			{
#ifdef USE_PERSIA_DEFAULT
				CallbackEditPosition_x = 1;
#else
				CallbackEditPosition_x = 3;
#endif
				CallbackEditPosition_y = (cur_idx-(INT8U)v_start+start_line) + 1;
			}
			else 	// 2 = bottom
			{
#ifdef USE_PERSIA_DEFAULT
				CallbackEditPosition_x = 1;
#else
				CallbackEditPosition_x = 3;
#endif
				CallbackEditPosition_y = Startup.menu_type + MenuNumLinePerPage;
			}
#endif
			if(Menu[view_idx].key_mode != 0xff){
				old_keymode = KEY_GetMode();
				KEY_SetMode(Menu[view_idx].key_mode);
				if (Menu[view_idx].key_mode == 9) keyin_get_speed_key = 1; // set speedkey
				if (Menu[view_idx].key_mode == 2) keyin_get_speed_key = 2; // set speedkey(M1160)
			}
			if(MenuRefresh == OFF) {	// Added by CJK 20041221
				// Added by CJK 20041126
				pass_flag = KI_NOPASS;
				if(Menu[view_idx].disable == ON) pass_flag = KI_NOEDIT;//pass_flag = KI_PASS;
				/////////////////////////
				switch(Menu[view_idx].type) {
					case MENU_TYPE_NUM :
						//func=input_date_check_year_format;
						if(Menu[view_idx].function != NULL){
							keyin_setcallback(Menu[view_idx].function);
						}
						if(Menu[view_idx].clerk_one_touch) keyin_clerk_onetouch = ON;
						while(1) {
							memcpy(&backup_val, (void*)Menu[view_idx].pval, 4);
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
							result = keyin_value(4, KI_GRAPHIC, KI_FORM_ILNN, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = keyin_value(4, KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#else
							result = keyin_value(4, KI_DEFAULT, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#endif
#endif
							memcpy(&val, (void*)Menu[view_idx].pval, 4);
/////////////
							if (val < Menu[view_idx].min || val > Menu[view_idx].max) {
								//복구
								memcpy((void*)Menu[view_idx].pval, &backup_val, 4);
								if (result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) {
									error_display_form1(0 , Menu[view_idx].min, Menu[view_idx].max);
									continue;
								} else {
									break;
								}
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) {
									break;
								} else {
									//복구
									memcpy((void*)Menu[view_idx].pval, &backup_val, 4);
									if (result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) {
										continue;
									} else {
										break;
									}
								}
							} else {
								break;
							}
/*						       						
							if((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && (val < Menu[view_idx].min || val > Menu[view_idx].max)) {
								//BuzOn(2);
								error_display_form1(0 , Menu[view_idx].min, Menu[view_idx].max);
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if(result==KP_ESC) break;
								//if (check_result == FALSE) check_result = (*check_function)(backup_val);
								if (check_result) break;
							} else {
								break;
							}
*/
						}
						if(Menu[view_idx].clerk_one_touch) keyin_clerk_onetouch = OFF;
						keyin_clearcallback();
						break;
					case MENU_TYPE_NUM_1BYTE :
						if(Menu[view_idx].function != NULL){
							keyin_setcallback(Menu[view_idx].function);
						}
						if(Menu[view_idx].clerk_one_touch) keyin_clerk_onetouch = ON;
						while(1) {
							memcpy(&backup_val1, (void*)Menu[view_idx].pval, 1);
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
							result = keyin_value(1, KI_GRAPHIC, KI_FORM_ILNN, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = keyin_value(1, KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#else
							result = keyin_value(1, KI_DEFAULT, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#endif
#endif
							memcpy(&val_1byte, (void*)Menu[view_idx].pval, 1);
							val = (INT32U)val_1byte;
							if (val < Menu[view_idx].min || val > Menu[view_idx].max) {
								//복구
								memcpy((void*)Menu[view_idx].pval, &backup_val1, 1);
								if (result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) {
									error_display_form1(0 , Menu[view_idx].min, Menu[view_idx].max);
									continue;
								} else {
									break;
								}
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) {
									break;
								} else {
									//복구
									memcpy((void*)Menu[view_idx].pval, &backup_val1, 1);
									if (result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) {
										continue;
									} else {
										break;
									}
								}
							} else {
								break;
							}
/*
							val = (INT32U)val_1byte;
							if((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && (val < Menu[view_idx].min || val > Menu[view_idx].max)) {
								//BuzOn(2);
								error_display_form1(0 , Menu[view_idx].min, Menu[view_idx].max);
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) break;
							} else {
								break;
							}
*/						}
						if(Menu[view_idx].clerk_one_touch) keyin_clerk_onetouch = OFF;
						keyin_clearcallback();
						break;
					case MENU_TYPE_NUM_2BYTE :
						if(Menu[view_idx].function != NULL){
							keyin_setcallback(Menu[view_idx].function);
						}
						if(Menu[view_idx].clerk_one_touch) keyin_clerk_onetouch = ON;
						while(1) {
							memcpy(&backup_val2, (void*)Menu[view_idx].pval, 2);
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
							result = keyin_value(2, KI_GRAPHIC, KI_FORM_ILNN, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = keyin_value(2, KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#else
							result = keyin_value(2, KI_DEFAULT, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#endif
#endif
							memcpy(&val_2byte, (void*)Menu[view_idx].pval, 2);
							val = (INT32U)val_2byte;
							if (val < Menu[view_idx].min || val > Menu[view_idx].max) {
								//복구
								memcpy((void*)Menu[view_idx].pval, &backup_val2, 2);
								if (result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) {
									error_display_form1(0 , Menu[view_idx].min, Menu[view_idx].max);
									continue;
								} else {
									break;
								}
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) {
									break;
								} else {
									//복구
									memcpy((void*)Menu[view_idx].pval, &backup_val2, 2);
									if (result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) {
										continue;
									} else {
										break;
									}
								}
							} else {
								break;
							}

/*
							if((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && (val < Menu[view_idx].min || val > Menu[view_idx].max)) {
								//BuzOn(2);
								error_display_form1(0 , Menu[view_idx].min, Menu[view_idx].max);
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) break;
							} else {
								break;
							}
*/						}
						if(Menu[view_idx].clerk_one_touch) keyin_clerk_onetouch = OFF;
						keyin_clearcallback();
						break;
					case MENU_TYPE_HEX :
						if(Menu[view_idx].function != NULL){
							keyin_setcallback(Menu[view_idx].function);
						}
						while(1) {
							memcpy(&backup_val, Menu[view_idx].pval, 4);
							val = backup_val;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = keyin_ulong_hex(KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U*)&val, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#else
							result = keyin_ulong_hex(KI_DEFAULT, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U*)&val, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, pass_flag);
#endif
							memcpy(Menu[view_idx].pval, &val, 4);
							if (val < Menu[view_idx].min || val > Menu[view_idx].max) {
								//복구
								memcpy((void*)Menu[view_idx].pval, &backup_val, 4);
								if (result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) {
									error_display_form1(0 , Menu[view_idx].min, Menu[view_idx].max);
									continue;
								} else {
									break;
								}
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) {
									break;
								} else {
									//복구
									memcpy((void*)Menu[view_idx].pval, &backup_val, 4);
									if (result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) {
										continue;
									} else {
										break;
									}
								}
							} else {
								break;
							}
/*
						      	if((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && (val < Menu[view_idx].min || val > Menu[view_idx].max)) {
								//BuzOn(2);
								error_display_form1(0 , Menu[view_idx].min, Menu[view_idx].max);
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) break;
							} else {
								break;
							}						
*/						}
						//val = (INT32U)*Menu[view_idx].pval;
						keyin_clearcallback();
						break;
					case MENU_TYPE_IP :
#if defined(USE_SINGLE_LINE_GRAPHIC)
						result = keyin_string(KI_IP_MODE, KI_SEGMENT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start, 0, 0, pass_flag);
						keyin_string(KI_IP_MODE, KI_SEGMENT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start, 0, 0, KI_PASS);
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						result = keyin_string(KI_IP_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
						keyin_string(KI_IP_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else
						result = keyin_string(KI_IP_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
						keyin_string(KI_IP_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
#endif
						break;
					case MENU_TYPE_DATE :
						while(1) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = keyin_string(KI_IP_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
							keyin_string(KI_IP_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else							
							result = keyin_string(KI_IP_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
							keyin_string(KI_IP_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
							if((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && !input_day_check_str(Menu[view_idx].general_flag, (HUGEDATA char *)Menu[view_idx].pstr, 2)) {
								BuzOn(2);
							}else {
								break;
							}		
/*							if((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && !input_day_check_str(Menu[view_idx].general_flag, (HUGEDATA char *)Menu[view_idx].pstr)) {
								BuzOn(2);
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) break;
							} else {
								break;
							}
*/						}
						break;
					case MENU_TYPE_TIME :
						while(1) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = keyin_string(KI_IP_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
							keyin_string(KI_IP_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else							
							result = keyin_string(KI_IP_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
							keyin_string(KI_IP_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
						      	if((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && !input_time_check_str((HUGEDATA char *)Menu[view_idx].pstr, 2)) {
								BuzOn(2);
							} else {
								break;
							}
/*						      if((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && !input_time_check_str((HUGEDATA char *)Menu[view_idx].pstr, 2)) {
								BuzOn(2);
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) break;
							} else {
								break;
							}
*/						}
						break;
					case MENU_TYPE_FORM :
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						result = keyin_string(KI_FORM_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
#else
						result = keyin_string(KI_FORM_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
#endif
						//keyin_string(KI_FORM_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
						break;
					case MENU_TYPE_YN :
						if(Menu[view_idx].function != NULL){
							keyin_setcallback(Menu[view_idx].function);
						}
						memcpy(&val, Menu[view_idx].pval, 4);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						result = keyin_yesno(KI_GRAPHIC, (long*)&val, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, pass_flag);
#else
						result = keyin_yesno(KI_DEFAULT, (long*)&val, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, pass_flag);
#endif
						memcpy(Menu[view_idx].pval, &val, 4);
						keyin_clearcallback();
						break;
					case MENU_TYPE_STR :
						while(1) {
#ifdef USE_CHINA_PINYIN
							if (Menu[view_idx].properties&MENU_P_CHINA_PINY)
							{
								temp_y = (cur_idx-(INT8U)v_start);
								temp_y++;	// 다음줄
								if (temp_y >= Startup.line_max) temp_y -= 2; // 윗줄
								china_pos_y = temp_y*y_gab; 
								china_pos_x = (Dsp_GetChars()-8)*x_gab;
#ifdef CL5200_PROJECT	// Adjust Display
								china_pos_y += 4;
								china_pos_x -= 6;
#endif
							}
#endif
							result = keyin_string(KI_FREE_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, Menu[view_idx].properties, 0, pass_flag);
#ifdef USE_CHINA_PINYIN
							china_pos_y = 0; 
							china_pos_x = 0;
#endif
							if (Menu[view_idx].properties&MENU_P_CHECK_BLANKS) {
								if (((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && result!=KP_ARROW_UP) && checkblank((char *)Menu[view_idx].pstr, 0)==0) {	// 0:blank, 1:not blank
									BuzOn(2);
								} else {
									break;
									}
							} else if (Menu[view_idx].check_function) {
								check_function_str = Menu[view_idx].check_function;
								check_result = (*check_function_str)(Menu[view_idx].pstr);
								if (check_result) break;
								else
								{
									if (result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) {
										continue;
									} else {
										break;
									}
								}
							} else {
								break;
							}
						}
						break;
					case MENU_TYPE_FIX :
						if(Menu[view_idx].w_length) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = keyin_string(KI_FREE_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#else
							result = keyin_string(KI_FREE_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, KI_PASS);
#endif
						}
						result = KP_ARROW_DN;
						break;
					case MENU_TYPE_BAR :
						//backup_pgmmode = KeyDrv.PgmMode;
						//KeyDrv.PgmMode = 0;	// => English Character
						//if(Startup.country == COUNTRY_RU) KeyDrv.PgmMode = 1;	// => English Character
						while(1) {
							result = keyin_string(KI_BARCODE_FORM, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, Menu[view_idx].properties, 0, pass_flag);
							if(result == KP_ESC || check_barcode_str((char *)Menu[view_idx].pstr, (char *)Menu[view_idx].form)) break;
							else if(Menu[view_idx].general_flag && barcode_form_len((char *)Menu[view_idx].pstr) <= barcodetype_length[Menu[view_idx].general_flag]) break;
							else BuzOn(2);
						}
						//KeyDrv.PgmMode = backup_pgmmode;
						break;
					case MENU_TYPE_WEIGHT:
						while(1) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = weighing_tare_input(KI_GRAPHIC, Menu[view_idx].decimal, (long *)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab);
#else
							result = weighing_tare_input(KI_DEFAULT, Menu[view_idx].decimal, (long *)Menu[view_idx].pval, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab);
#endif
							val = *Menu[view_idx].pval;
							if (result == KP_ESC) break;
							else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) break;
							} else {
								BuzOn(2);
							}
						}
						break;
					case MENU_TYPE_DATENUM:
						while(1) {
							memcpy(&val, (void*)Menu[view_idx].pval, 4);
							date_2str(val,(char *)Menu[view_idx].pstr);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = keyin_string(KI_SNUMBER, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
#else
							result = keyin_string(KI_SNUMBER, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, 0, pass_flag);
#endif
							Menu[view_idx].pstr[6] = 0;
							val = str_2sellbydate((char *)Menu[view_idx].pstr);
							memcpy((void*)Menu[view_idx].pval, (void*)&val, 4);
							if (result == KP_ESC) {
								break;
							} else if(val < Menu[view_idx].min || val > Menu[view_idx].max) {
								error_display_form1(0 , Menu[view_idx].min, Menu[view_idx].max);
							} else if (Menu[view_idx].check_function) {
								check_function = Menu[view_idx].check_function;
								check_result = (*check_function)(val);
								if (check_result) break;
							} else if (val > 9999 && !date_checkday((short)(val/10000),(short)(val/100%100),(short)(val%100))) {
								BuzOn(2);
							} else {//if(temp_value != -1) {
								break;
							}
						}
						break;
					case MENU_TYPE_PASSWORD :
						while(1) {
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							result = keyin_string(KI_FREE_MODE, KI_GRAPHIC, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, Menu[view_idx].properties, 1, pass_flag);
#else
							result = keyin_string(KI_FREE_MODE, KI_DEFAULT, Menu[view_idx].form, Menu[view_idx].pstr, Menu[view_idx].length, Menu[view_idx].w_length, (cur_idx-(INT8U)v_start+start_line)*y_gab, Menu[view_idx].start*x_gab, Menu[view_idx].properties, 1, pass_flag);
#endif
							if (Menu[view_idx].properties&MENU_P_CHECK_BLANKS) {
								if (((result!=KP_ESC && result!= KP_ARROW_UP && result!= KP_PAGE_UP) && result!=KP_ARROW_UP) && checkblank((char *)Menu[view_idx].pstr, 0)==0) {	// 0:blank, 1:not blank
									BuzOn(2);
								} else {
									break;
								}
							} else {
								break;
							}
						}
						break;
				}
				if(Menu[view_idx].key_mode != 0xff){
					KEY_SetMode(old_keymode);
					keyin_get_speed_key = 0;
				}
				// Added by CJK 20041126
				//if(Menu[cur_idx].disable == ON) result = KP_ARROW_DN;
				// Added by CJK 20041221
				if(Menu[view_idx].end_function && (Menu[view_idx].type == MENU_TYPE_NUM || Menu[view_idx].type == MENU_TYPE_NUM_1BYTE || Menu[view_idx].type == MENU_TYPE_NUM_2BYTE || Menu[view_idx].type == MENU_TYPE_HEX || Menu[view_idx].type == MENU_TYPE_YN || Menu[view_idx].type == MENU_TYPE_STR)) {
	
					if (Menu[view_idx].type == MENU_TYPE_STR)
					{
						end_function_str = Menu[view_idx].end_function;
						(*end_function_str)(Menu[view_idx].pstr);
					}
					else
					{
						end_function = Menu[view_idx].end_function;
						(*end_function)(val);
					}
					if(MenuRefresh) {	// 화면 refresh 기능. End Function 내에서만 사용가능.
						loop = OFF;
						continue;
					}
				}
			} else {	// MenuRefresh == ON
				if(Menu[view_idx].key_mode != 0xff){
					KEY_SetMode(old_keymode);
					keyin_get_speed_key = 0;
				}
				//while(Menu[view_idx].inhibit == ON && cur_idx > (INT8U)v_start) cur_idx--;
				MenuRefresh = OFF;
			}
			prev_page = cur_page;
			switch(result) {
				case KP_ENTER :
					if(cur_idx + 1 >= MenuIndex-MenuInhibitCnt) 
					{
						return MENU_RET_SAVE;
					}
				case KP_ARROW_DN :
					if(cur_idx + 1 >= MenuIndex-MenuInhibitCnt) {
						if (result == KP_ARROW_DN)
						{
							if (Menu_return_last_item_arrow_dn) 
							{
								Menu_return_last_item_arrow_dn = 0;
								return MENU_RET_DN;
							}
						}
						BuzOn(2);
						break;
					}
					cur_idx++;
					for(i = 0; cur_idx + i < (MenuIndex-MenuInhibitCnt); i++) {
						if(Menu[dsp2org_idx[cur_idx + i]].type != MENU_TYPE_FIX && Menu[dsp2org_idx[cur_idx + i]].disable == OFF) {
							break;
						}
					}
					if(cur_idx + i >= (MenuIndex-MenuInhibitCnt)) {	// 이후 데이터가 모두 MENU_TYPE_FIX이면
						cur_idx = cur_idx - 1;	// 원래 위치
						if(result == KP_ENTER) {
							BuzOn(1);
							return MENU_RET_SAVE;
						}
						BuzOn(2);
						break;
					}
					cur_idx = cur_idx + i;
					break;
				case KP_ARROW_UP :
					if(cur_idx - 1 < 0) {
						if (Menu_return_first_item_arrow_up) 
						{
							Menu_return_first_item_arrow_up = 0;
							return MENU_RET_UP;
						}
						BuzOn(2);
						break;
					}
					cur_idx--;
					for(i = 0; cur_idx - i >= 0; i++) {
						if(Menu[dsp2org_idx[cur_idx - i]].type != MENU_TYPE_FIX && Menu[dsp2org_idx[cur_idx - i]].disable == OFF) {//if(cur_idx - i == 0) {
							break;
						}
					}
					if(cur_idx - i < 0) {		// 계속 MENU_TYPE_FIX이면
						cur_idx = cur_idx + 1;
						BuzOn(2);
						break;
					}
					cur_idx = cur_idx - i;
					break;
				case KP_PAGE_DN :
					if(cur_page + 1 >= total_page) {
						BuzOn(2);
						break;
					}
					v_start = start_idx_by_page[cur_page + 1];
					for(i = 0; v_start + i < (MenuIndex-MenuInhibitCnt) && i < MenuNumLinePerPage; i++) {
						if(Menu[dsp2org_idx[v_start + i]].type != MENU_TYPE_FIX && Menu[dsp2org_idx[v_start + i]].disable == OFF) {
							break;
						}
					}
					if(v_start + i >= (MenuIndex-MenuInhibitCnt) || i >= MenuNumLinePerPage) {
						//BuzOn(2);
						v_start = start_idx_by_page[cur_page + 1];
						i = 0;
					}
					cur_idx = v_start + i;
					break;
				case KP_PAGE_UP :
					if(cur_page - 1 < 0) {
						BuzOn(2);
						break;
					}
					v_start = start_idx_by_page[cur_page - 1];
					for(i = 0; (v_start + i < MenuIndex-MenuInhibitCnt) && i < MenuNumLinePerPage; i++) {
						if(Menu[dsp2org_idx[v_start + i]].type != MENU_TYPE_FIX && Menu[dsp2org_idx[v_start + i]].disable == OFF) {
							break;
						}
					}
					if(v_start + i >= (MenuIndex-MenuInhibitCnt)) {
						BuzOn(2);
						v_start = start_idx_by_page[cur_page];
						break;
					}
					if(i >= MenuNumLinePerPage) {
						//BuzOn(2);
						i = 0;
					}
					cur_idx = v_start + i;
					break;
				case KP_ESC :
					return MENU_RET_ESC;
				case KP_SAVE :
					return MENU_RET_SAVE;
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
				if(CurMenuCode == 0x1732)
				{
					case KP_TEST : 
						return MENU_RET_TEST; //gm 090407
					case KP_FEED :
						return MENU_RET_FEED;
				}
				else if(CurMenuCode == 0x1736 || CurMenuCode == 0x8360)
				{
					case KP_ZERO :
						return MENU_RET_ZERO; //gm 090407
				}
#endif
				default :
					key_excution = OFF;
					for (k = 0; k < MAX_MENU_KEY_FUNCTION; k++)
					{
						if (Menu[dsp2org_idx[cur_idx]].callback_key[k])
						{
							if (result == Menu[dsp2org_idx[cur_idx]].callback_key[k]) {
								menu_key_function=Menu[dsp2org_idx[cur_idx]].key_function[k];
								(*menu_key_function)();
								key_excution = ON;
								if (Menu[dsp2org_idx[cur_idx]].properties&MENU_P_EXIT_MENU)
								{
									return MENU_RET_EXIT;
								}
							}
						}
					}
					if (!key_excution) {	// 해당 키가 없으면, 에러음
						BuzOn(2);
					}
					break;
			}
			cur_page = page_no_by_idx[cur_idx];
			v_start = start_idx_by_page[cur_page];
			if(cur_page != prev_page) loop = OFF;
		}
	}
}

/*
INT16U get_center_position(char *str)
{
	INT16U x;

	x = strlen((const char *)str) * 6;
	x = (LCD_Y_MARGIN - x) / 2;

	return x;
}
*/

void menu_display_menucode(INT16U code,INT8U set)
{
	static INT16U save_code;
	INT16U curcode;
	INT8U tarelength;
	char data[20];

	if (status_run.run_mode==RUN_SALE) return;
	curcode=code;
	if (set==0) {
		curcode=save_code;
	}
	tarelength=get_global_bparam(GLOBAL_DISPLAY_TARELENGTH);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	if (curcode>=0x8000) {
		sprintf(data," CAL ");	
		display_string(1,(INT8U *)data);
		sprintf(data," Mode");
		display_string(2,(INT8U *)data);
	}
#endif
	sprintf(data,"%04X",curcode);
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_string(DISPLAY_TARE,(INT8U *)data);
#else
	if(tarelength)
		display_string(DISPLAY_WEIGHT,(INT8U *)data);
	else//preset tare 사용 안하는 경
		display_string(DISPLAY_UNITPRICE,(INT8U *)data);
#endif
	VFD7_Diffuse();
	if (set) save_code=code;
}

void menu_displaytitle(INT16U code,INT8U mode)
{
	CAPTION_STRUCT cap;
	POINT 	disp_p;

#if !defined(USE_SINGLE_LINE_GRAPHIC)
	if (Startup.menu_type==0) {
		return;
	}
#endif
	caption_split_by_code(code, &cap,0);
		  
	if(strlen((char *)cap.form) > DISP_TITLE_LENGTH)
		cap.form[DISP_TITLE_LENGTH] = 0;
	
	_strupr((char *)cap.form);/////

#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)&cap.form[3]);
#else
	//disp_p = point(0, DISP_TITLE_START_POSITION * Startup.menu_xgap);
	disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, DISP_TITLE_START_POSITION * Startup.menu_xgap);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)&cap.form[3], strlen(&cap.form[3]), 0, 0);
	hebrew_codetemp[strlen(&cap.form[3])] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)&cap.form[2], strlen(&cap.form[2]), 0, 0, 0); 
	arab_codetemp[strlen(&cap.form[2])] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)&cap.form[3]);          
#endif

 	if (mode) Dsp_Diffuse();
#endif
}

void menu_writetitle_str(char *temp_string)
{
	POINT 	disp_p;
	INT8U	form_string[34];

#if !defined(USE_SINGLE_LINE_GRAPHIC)
	if (Startup.menu_type == 0)
	{ 
		display_clear(DISPLAY_PRICE);
		if (status_run.run_mode == RUN_SALE) {
			display_clear(DISPLAY_TARE);
			display_clear(DISPLAY_WEIGHT);
			display_clear(DISPLAY_UNITPRICE);
			strcpy((char *)form_string,"  EDT");
			display_string(DISPLAY_WEIGHT,(INT8U *)form_string);
		}
		return;
	}
#endif

	memcpy((INT8U *)form_string, (INT8U*)temp_string, 32);
	if(strlen((char *)form_string) > DISP_TITLE_LENGTH)
		form_string[DISP_TITLE_LENGTH] = 0;

	_strupr((char *)form_string);/////

#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)&form_string[3]);
#else
 	//disp_p = point(0, DISP_TITLE_START_POSITION * Startup.menu_xgap);
	disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, DISP_TITLE_START_POSITION * Startup.menu_xgap);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)&form_string[3], strlen(&form_string[3]), 0, 0); 
	hebrew_codetemp[strlen(&form_string[3])] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)&form_string[2], strlen((char *)&form_string[2]), 0, 0, 0); 
	arab_codetemp[strlen((char *)&form_string[2])] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)&form_string[3]);          
#endif
#endif
// 	Dsp_Diffuse();

}


void menu_writepage(INT8U totalpage, INT8U curpage)
{
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	POINT disp_p;
	char temp_string[32];
//@@	INT8U len;
	INT16U disp_length;

	if (Startup.menu_type==0) {
		sprintf(temp_string,"%02d-%02d ", curpage, totalpage);
		display_string(2,(INT8U *)temp_string);
		VFD7_Diffuse();
		return;
	}
#ifdef HEBREW_FONT
	sprintf(temp_string, ")%d/%d(", curpage, totalpage);
#else
	sprintf(temp_string, "(%d/%d)", curpage, totalpage);
#endif
//@@	len = strlen(temp_string);
//@@	disp_p = point(0, (33-len)*6);
//@@	Dsp_Write_String(disp_p,temp_string);	


 	disp_length = Startup.menu_length;//Dsp_GetChars();
 	disp_length -= strlen(temp_string);

	//disp_p = point(Startup.menu_type-1, disp_length*Startup.menu_xgap);
	disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, disp_length*Startup.menu_xgap);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)temp_string, strlen(temp_string), 0, 0); 
	hebrew_codetemp[strlen(temp_string)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)temp_string, strlen(temp_string), 0, 0, 0); 
	arab_codetemp[strlen(temp_string)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, (HUGEDATA char *)temp_string);
#endif

	//Dsp_Diffuse();
#endif
}


void VertScrollMsg_Init(INT8U fontId)
{
	INT8U i;

	Scrl_InId = 0;
	Scrl_OutId = 0;
	for(i = 0; i < SCROLL_MSG_MAX_LINE; i++) {
		Scrl_Msg[i][0] = 0;
	}
	Scrl_FondId = fontId;
	Scrl_Msg_UseLogo = OFF;
}

// void VertScrollMsg_Add(HUGEDATA char *str)
extern void init_displayLogo(INT16S m, INT8U dispFlag);
void VertScrollMsg_Add(const char *str)
{
	INT8U  i,line_count;
	INT16S    sl;
	POINT  disp_p;
	INT8U  ygap;
	INT8U  backup_directdraw;

	if (str[0] == 0) return; // CJK070626

	sl=strlen(str);
	memset((INT8U*)Scrl_Msg[Scrl_InId],0,SCROLL_MSG_SIZE);
	for (i=0; i<sl; i++) {
		Scrl_Msg[Scrl_InId][i]=(INT8U)str[i];
	}
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);
	if (Scrl_Msg_UseLogo)
	{
	}
	DspLoadFont1(Scrl_FondId);
	ygap = display_font_get_height();
	//if (Startup.menu_type == 0)
	//	line_count = 2;
	//else
	//	line_count = 4;
	line_count = LCD_X_MARGIN / ygap;
	if (line_count > SCROLL_MSG_MAX_LINE) line_count = SCROLL_MSG_MAX_LINE;

	for(i = 0; i < line_count; i++) {
		disp_p = point(i*ygap, 0);
		//DspStruct.DirectDraw=1;
#ifdef USE_ARAB_CONVERT
		Convert_ArabPreview((INT8U *)Scrl_Msg[(Scrl_OutId+i)%line_count], strlen(Scrl_Msg[(Scrl_OutId+i)%line_count]), 0, 0, 0); 
		arab_codetemp[strlen(Scrl_Msg[(Scrl_OutId+i)%line_count])] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,Scrl_Msg[(Scrl_OutId+i)%line_count]);
#endif
	}
	Dsp_Diffuse();
	Scrl_InId++;
	Scrl_InId %= line_count;
	if(Scrl_InId == Scrl_OutId) {
		Scrl_OutId++;
		Scrl_OutId %= line_count;
	}
	DspStruct.DirectDraw = backup_directdraw;
}

INT8U Menu_ConfirmDisplay(INT16U captionCode)
{
	INT32U value;
	CAPTION_STRUCT cap;
	INT8U result;

	Menu_Init();
	if (captionCode)
	{
		caption_split_by_code(captionCode, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);
	}      
	value=0;
	caption_split_by_code(0x9500, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &value, NULL, NULL, NULL);
INPUT:
	result = Menu_InputDisplay();

	if (result == MENU_RET_SAVE)
	{
		if (value) 
		{
			return YES;
		}
		else
		{
			return NO;
		}

	}
	else if(result == MENU_RET_ESC)
	{
		return NO;
	}
	else
	{
		result = 0;
		BuzOn(2);
		goto INPUT;
	}
	
}
