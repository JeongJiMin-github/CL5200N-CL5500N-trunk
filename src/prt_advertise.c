/*****************************************************************************
 |*		       
 |*  Copyright		:	(c) 2003 CAS
 |*  Filename		:	prt_advertise.c
 |*  Version			:	v 0.5
 |*  Programmer(s)	:	getwing (Jae-Ok Kim)
 |*  Created			:	2003/12/10
 |*  Description		:	(sale message) advertisement by print   
 |*              				
 ****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "initial.h"

#include "commbuf.h"
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
#include "bar_common.h"
#include "prt_interpreter.h"

#include "rtc.h"

#include "adm.h"
#include "main.h"
#include "initial.h"
#include "key.h"
#include "key_typedef.h"
#include "key_function.h"
#include "mode_main.h"
#include "caption_app.h"
#include "common.h"
#include "prt_common.h"
#include "menu.h"
#include "flash_app.h"
#include "dsp_app.h"
#include "dsp_vfd7.h"
#include "key_app.h"

#include "math.h" 
#include "mode_program.h"
#include "cursor.h"
#include "prt_advertise.h"  
#include "menu_code_text1.h"
#include "pgm_app.h"
#include "initial_glb.h"
#include "prt_list.h"
#include "mode_error.h"
#include "mode_commun.h"
#include "ethernet_comm.h"

extern void input_day_format(INT8U day_type, char *str, INT8U year, INT8U month, INT8U date);
extern void input_time_format(char *str, INT16S hour, INT16S min, INT16S sec);
extern INT8U menu_readmode;

void	label_message_create(void)		// command code  0x1241
{	
	//INT16U cap_code[2]={0x3241,0x3242};
	INT16U cap_code[2]={90,0x3242};
	menu1_create(0x1241,cap_code, FLASH_PRT_ADVE_AREA, PRT_ADVE_STRUCT_SIZE, ON);
	// ]		
}

void    label_message_delete(void)		// command code  0x1243
{	
	// <CODE> JHC, 040414, -change code
	// [
	//menu1_delete(0x1243, 0x3241, FLASH_PRT_ADVE_AREA, PRT_ADVE_STRUCT_SIZE);
	menu1_delete(0x1243, 90, FLASH_PRT_ADVE_AREA, PRT_ADVE_STRUCT_SIZE);
	// ]
}

#ifndef DISABLE_LIST_MENU
void 	label_message_list(void)		// command code  0x1242
{	
	INT16U input_cap[3]={0x1E00,0x1E01,0x1E02};
	display_pgm_list(0x1242, 90, input_cap, 0x71FF, FLASH_PRT_ADVE_AREA, PRT_ADVE_STRUCT_SIZE);
}
#endif

//DEL071119void scroll_message_callback(INT16S num)
//DEL071119{
//DEL071119	CAPTION_STRUCT cap;
//DEL071119	POINT disp_p;
//DEL071119
//DEL071119	INT32U	cap_code1 = 0x2706;
//DEL071119//	INT32U	cap_addr1;
//DEL071119
//DEL071119//	if (Startup.menu_type==0) return;
//DEL071119	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 2*Startup.menu_xgap);
//DEL071119	memset((INT8U*)&cap.form, 0x20, sizeof(cap.form));
//DEL071119	
//DEL071119	DspStruct.DirectDraw = 1;
//DEL071119	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
//DEL071119
//DEL071119	if(num < 0 || num > 2)	{
//DEL071119		strcpy(cap.form,global_message[MSG_INVALID_VALUE]);
//DEL071119	} else  {
//DEL071119//		cap_addr1=caption_search_by_code(cap_code1 + (INT32U) num );
//DEL071119		caption_split_by_code(cap_code1 + (INT16U)num,&cap,0);
//DEL071119	}
//DEL071119	DspStruct.DirectDraw = 1;
//DEL071119	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
//DEL071119}

#define MAX_SCROLL_DATA		80
#ifdef USE_SINGLE_LINE_GRAPHIC
#define MAX_SCROLL_LINE	1
#else
#define MAX_SCROLL_LINE	5
#endif


#define M_ID_SCROLL_NO		1
#define M_ID_SCROLL_MSG		2
#define M_ID_SCROLL_EFFECT	3
#define M_ID_SCROLL_USE_MSG	4
#define M_ID_SCROLL_USE_SCHED	5
#define M_ID_SCROLL_STATIC_TEXT	6
#define M_ID_SCROLL_DAYOFWEEK	7
#define M_ID_SCROLL_START_DATE	8
#define M_ID_SCROLL_END_DATE	9
#define M_ID_SCROLL_START_TIME	10
#define M_ID_SCROLL_END_TIME	11
#define M_ID_SCROLL_IMG_ID	12
#define M_ID_SCROLL_FONT_SIZE	13
#define M_ID_SCROLL_ON_TIME	14
#define M_ID_SCROLL_BLINK_COUNT	15

void callback_end_usedate(INT32U num)
{
	INT8U id_scr_static;
	INT8U id_scr_day_week;
	INT8U id_scr_s_date;
	INT8U id_scr_e_date;
	INT8U id_scr_s_time;
	INT8U id_scr_e_time;
	INT8U onoff;
	
	id_scr_static = Menu_GetMenuId(M_ID_SCROLL_STATIC_TEXT);
	id_scr_day_week = Menu_GetMenuId(M_ID_SCROLL_DAYOFWEEK);
	id_scr_s_date = Menu_GetMenuId(M_ID_SCROLL_START_DATE);
	id_scr_e_date = Menu_GetMenuId(M_ID_SCROLL_END_DATE);
	id_scr_s_time = Menu_GetMenuId(M_ID_SCROLL_START_TIME);
	id_scr_e_time = Menu_GetMenuId(M_ID_SCROLL_END_TIME);
	
	onoff = ON;
	if (num) onoff = OFF;
	
	Menu[id_scr_static].inhibit = onoff;
	Menu[id_scr_day_week].inhibit = onoff;
	Menu[id_scr_s_date].inhibit = onoff;
	Menu[id_scr_e_date].inhibit = onoff;
	Menu[id_scr_s_time].inhibit = onoff;
	Menu[id_scr_e_time].inhibit = onoff;
	
	MenuRefresh = ON;
}

void callback_end_usemsg(INT32U num)
{
	INT8U id_scr_use_date;
	INT8U id_scr_static;
	INT8U id_scr_day_week;
	INT8U id_scr_s_date;
	INT8U id_scr_e_date;
	INT8U id_scr_s_time;
	INT8U id_scr_e_time;
	INT8U onoff;
	
	id_scr_use_date = Menu_GetMenuId(M_ID_SCROLL_USE_SCHED);
	id_scr_static = Menu_GetMenuId(M_ID_SCROLL_STATIC_TEXT);
	id_scr_day_week = Menu_GetMenuId(M_ID_SCROLL_DAYOFWEEK);
	id_scr_s_date = Menu_GetMenuId(M_ID_SCROLL_START_DATE);
	id_scr_e_date = Menu_GetMenuId(M_ID_SCROLL_END_DATE);
	id_scr_s_time = Menu_GetMenuId(M_ID_SCROLL_START_TIME);
	id_scr_e_time = Menu_GetMenuId(M_ID_SCROLL_END_TIME);
	
	onoff = ON;
	if (num) onoff = OFF;
	
	Menu[id_scr_use_date].inhibit = onoff;
	Menu[id_scr_static].inhibit = onoff;
	Menu[id_scr_day_week].inhibit = onoff;
	Menu[id_scr_s_date].inhibit = onoff;
	Menu[id_scr_e_date].inhibit = onoff;
	Menu[id_scr_s_time].inhibit = onoff;
	Menu[id_scr_e_time].inhibit = onoff;
	
	if (Menu[id_scr_use_date].inhibit == OFF)
	{
		callback_end_usedate(*Menu[id_scr_use_date].pval);
	}
	
	MenuRefresh = ON;
}


void callback_end_scroll_no(INT32U scroll_id)
{				
	SCROLL_MESSAGE_STRUCT scroll;
	INT8U day_type;
	INT32U addr, addr_start;
	INT16S text_size;
	INT16S scroll_size;
	
	INT8U id_scr_msg_str;
	INT8U id_scr_effect;
	INT8U id_scr_use_msg;
	INT8U id_scr_use_date;
	INT8U id_scr_day_week;
	INT8U id_scr_s_date;
	INT8U id_scr_e_date;
	INT8U id_scr_s_time;
	INT8U id_scr_e_time;
	INT8U id_scr_on_time;
	INT8U id_scr_blink_ct;
	
	id_scr_msg_str = Menu_GetMenuId(M_ID_SCROLL_MSG);
	id_scr_effect = Menu_GetMenuId(M_ID_SCROLL_EFFECT);
	id_scr_use_msg = Menu_GetMenuId(M_ID_SCROLL_USE_MSG);
	id_scr_use_date = Menu_GetMenuId(M_ID_SCROLL_USE_SCHED);
	id_scr_day_week = Menu_GetMenuId(M_ID_SCROLL_DAYOFWEEK);
	id_scr_s_date = Menu_GetMenuId(M_ID_SCROLL_START_DATE);
	id_scr_e_date = Menu_GetMenuId(M_ID_SCROLL_END_DATE);
	id_scr_s_time = Menu_GetMenuId(M_ID_SCROLL_START_TIME);
	id_scr_e_time = Menu_GetMenuId(M_ID_SCROLL_END_TIME);
	id_scr_on_time = Menu_GetMenuId(M_ID_SCROLL_ON_TIME);
	id_scr_blink_ct = Menu_GetMenuId(M_ID_SCROLL_BLINK_COUNT);
	day_type = rtc_get_date_format();
	
	/////////////
	addr_start =FLASH_SCROLL_AREA;
	scroll_size = sizeof(SCROLL_MESSAGE_STRUCT);
	text_size = sizeof(scroll.text)-1;
	
	if (scroll_id == 0) {
		scroll_id=1;
		//memset(&scroll,0,scroll_size);
		//scroll.week=0xff;
	}
	addr = scroll_id-1;
	addr*= scroll_size;
	addr+= addr_start;
	get_base_sparam(addr,(INT8U *)&scroll,scroll_size);
	if ((scroll.stype < 1) || (scroll.stype > MAX_SCROLL_EFFECT)) {
		memset(&scroll, 0, scroll_size);
		scroll.stype=1;
		scroll.week=0xff;
	}
	/////////////////
	memcpy(Menu[id_scr_msg_str].pstr, scroll.text, text_size);
	Menu[id_scr_msg_str].pstr[MAX_SCROLL_DATA] = 0; 
	*Menu[id_scr_effect].pval = scroll.stype;
	
	*Menu[id_scr_on_time].pval = scroll.on_time;
	*Menu[id_scr_blink_ct].pval = scroll.blink_count;
	*Menu[id_scr_use_msg].pval = scroll.use;
	*Menu[id_scr_use_date].pval = scroll.smode;
	binary2str(scroll.week, Menu[id_scr_day_week].pstr);
	input_day_format(day_type, (char *)Menu[id_scr_s_date].pstr, scroll.start_year, scroll.start_month, scroll.start_day);
	input_day_format(day_type, (char *)Menu[id_scr_e_date].pstr, scroll.end_year, scroll.end_month, scroll.end_day);
	input_time_format((char *)Menu[id_scr_s_time].pstr, scroll.start_hour, scroll.start_min, scroll.start_sec);
	input_time_format((char *)Menu[id_scr_e_time].pstr, scroll.end_hour, scroll.end_min, scroll.end_sec);
	
	callback_end_usemsg(*Menu[id_scr_use_msg].pval);
	//	if (Menu[id_scr_use_date].inhibit == OFF)
	//	{
	//		callback_end_usedate(*Menu[id_scr_use_date].pval);
	//	}
	/////
	MenuRefresh = ON;
}

void callback_scroll_effect(long num)
{
	
 	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT16U 	w;
	
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num < 1 || num > MAX_SCROLL_EFFECT)	{
		strcpy(buffer, global_message[MSG_INVALID_VALUE]);
	} else if (num == 3) {		    
		caption_split_by_code(0x2711, &cap, 0);
		strcpy(buffer, cap.form);
	} else {
		caption_split_by_code(0x2706 + (INT16U)num, &cap, 0);
		strcpy(buffer, cap.form);
	}
	//	if (num >= 4 && num <= 6) {
	//		strcat(buffer, "(Big)");
	//	}
	//	if (w >	COUNTRY_STRUCT_SIZE-2) w = COUNTRY_STRUCT_SIZE-2;
	buffer[w] = 0;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
	hebrew_codetemp[strlen(buffer)] = 0;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
}

void  scroll_message_input(void)
{
	CAPTION_STRUCT 	cap;	
	SCROLL_MESSAGE_STRUCT scroll;
	char form_string[32];
	char scroll_str[MAX_SCROLL_DATA+1];
	INT32U effect;
	INT32U use_scroll;
	INT32U use_schedule;
	INT32U image_id;
	INT32U blink_count;
	INT32U on_time;
	INT32U font_size;
	INT32U addr, addr_start;
	INT8U  date_sep, time_sep;
	INT16S scroll_size, i;
	INT32U scroll_id;
	INT16U result;
#ifndef USE_SINGLE_LINE_GRAPHIC
	char date_form[14],time_form[14];
#endif
	INT8U day_type;
	INT16S text_size;
#ifndef USE_SINGLE_LINE_GRAPHIC
	char daytype_str[15];
#endif
	char dayofweek_str[10];
	char startdate_str[10];
	char enddate_str[10];
	char starttime_str[10];
	char endtime_str[10];
	
	day_type = rtc_get_date_format();
#if defined(USE_SINGLE_LINE_GRAPHIC)
	date_sep = '.'; //7-seg display용
	time_sep = '.'; //7-seg display용
#else
	date_sep = get_global_bparam(GLOBAL_DATE_SEPARATOR);
	time_sep = get_global_bparam(GLOBAL_TIME_SEPARATOR);
#endif
#ifndef USE_SINGLE_LINE_GRAPHIC
	sprintf(date_form, "[  %c  %c  ]", date_sep, date_sep);
	sprintf(time_form, "[  %c  %c  ]", time_sep, time_sep);
	switch(day_type) {
		case 0:	caption_split_by_code(0x384A, &cap,0); 	break;
		case 1:	caption_split_by_code(0x384B, &cap,0);	break;
		case 3:
		case 4: caption_split_by_code(0x384D, &cap,0);	break;
	}
	cap.form[14]=0;
	sprintf(daytype_str, "%s", cap.form);
#endif
	/////////////
	addr_start =FLASH_SCROLL_AREA;
	addr = addr_start;
	scroll_size = sizeof(SCROLL_MESSAGE_STRUCT);
	text_size=sizeof(scroll.text)-1;
	scroll_id=0;
	for (i=0; i<10; i++) {
		get_base_sparam(addr,(INT8U *)&scroll,scroll_size);
		if ((scroll.stype < 1) || (scroll.stype > MAX_SCROLL_EFFECT)) {
			scroll_id=i+1;
			break;
		}
		addr+=scroll_size;
	}
	
	if (scroll_id==0) {
			scroll_id=1;
		//memset(&scroll,0,scroll_size);
		//scroll.week=0xff;
	}
	else if( scroll_id > MAX_SCROLL_NO){
		scroll_id = MAX_SCROLL_NO;
	}
	addr = scroll_id-1;
	addr*= scroll_size;
	addr+= addr_start;
	get_base_sparam(addr,(INT8U *)&scroll,scroll_size);
	if ((scroll.stype < 1) || (scroll.stype > MAX_SCROLL_EFFECT)) {
		memset(&scroll,0,scroll_size);
		scroll.stype=1;
		scroll.week=0xff;
	}
	/////////////////
	memcpy(scroll_str, scroll.text, text_size);
	scroll_str[MAX_SCROLL_DATA] = 0; 
	effect = scroll.stype;
	image_id = scroll.image_id;
	on_time = scroll.on_time;
	blink_count = scroll.blink_count;
	font_size = scroll.font_size;
	use_scroll = scroll.use;
	use_schedule = scroll.smode;
	binary2str(scroll.week, (INT8U *)dayofweek_str);
	input_day_format(day_type, startdate_str, scroll.start_year, scroll.start_month, scroll.start_day);
	input_day_format(day_type, enddate_str, scroll.end_year, scroll.end_month, scroll.end_day);
	input_time_format(starttime_str, scroll.start_hour, scroll.start_min, scroll.start_sec);
	input_time_format(endtime_str, scroll.end_hour, scroll.end_min, scroll.end_sec);
	/////
	
	Menu_Init();
	
	caption_split_by_code(0x1432, &cap,0);//""
	Menu_HeadCreate((HUGEDATA char *)cap.form);
	
	caption_split_by_code(0x2703, &cap,0);//"Message No."
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_NO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &scroll_id, NULL, NULL, NULL);
	Menu_AddEndFunction(M_ID_SCROLL_NO, callback_end_scroll_no);
	
	caption_split_by_code(0x2704, &cap,0);//"Message"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_MSG, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)scroll_str, NULL, NULL);
	Menu_SetProperties(M_ID_SCROLL_MSG, MENU_P_USE_LINEFEED);
	Menu_SetNextPage();
	
	caption_split_by_code(0x2702, &cap,0);//"Speed     :"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_ON_TIME, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &on_time, NULL, NULL, NULL);
	
	caption_split_by_code(0x270C, &cap,0);//"Blink Counts    :"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_BLINK_COUNT, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &blink_count, NULL, NULL, NULL);
    Menu_SetCallbackMode(M_ID_SCROLL_BLINK_COUNT, 1);	// 1:below
	Menu_SetNextPage();
	
	caption_split_by_code(0x2705, &cap,0);//"Effect     :"
	caption_adjust(&cap,NULL);
	cap.input_max = MAX_SCROLL_EFFECT;
	Menu_InputCreate(M_ID_SCROLL_EFFECT, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &effect, NULL, NULL, callback_scroll_effect);
    Menu_SetCallbackMode(M_ID_SCROLL_EFFECT, 1);	// 1:below
    Menu_SetNextPage();
	
	caption_split_by_code(0x2709, &cap,0);//"Use message(Y/N)"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_USE_MSG, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &use_scroll, NULL, NULL, NULL);
	Menu_AddEndFunction(M_ID_SCROLL_USE_MSG, callback_end_usemsg);
	Menu_SetNextPage();
	
	caption_split_by_code(0x2710, &cap,0);//"Date Setting (Y/N):"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_USE_SCHED, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &use_schedule, NULL, NULL, NULL);
	Menu_AddEndFunction(M_ID_SCROLL_USE_SCHED, callback_end_usedate);
	Menu_SetNextPage();
	
#ifndef USE_SINGLE_LINE_GRAPHIC
	caption_split_by_code(0x1C13,&cap,0);	// "Input '1' or '0'  [SMTWTFS]"
	Menu_InputCreate(M_ID_SCROLL_STATIC_TEXT, (HUGEDATA char *)cap.form, MENU_TYPE_FIX, cap.input_x, cap.input_dlength, cap.input_length, 
			0, 0, 0, NULL, NULL, NULL, NULL);
#endif
	
	strcpy(form_string,"1111111");
	caption_split_by_code(0x1C14,&cap,0);	// "Day of Week      :"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_DAYOFWEEK, (HUGEDATA char *)cap.form, MENU_TYPE_FORM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)dayofweek_str, (HUGEDATA INT8U *)form_string, NULL);
	Menu_SetNextPage();
	
	sprintf(form_string,"99%c99%c99", date_sep, date_sep);
	caption_split_by_code(0x1C15, &cap, 0);	// "Start Date :"
	cap.input_x=strlen((char *)cap.form)+1;
#ifndef USE_SINGLE_LINE_GRAPHIC
	strcat((char *)cap.form, date_form);
	strcat((char *)cap.form, daytype_str);
#endif
	//caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_START_DATE, (HUGEDATA char *)cap.form, MENU_TYPE_DATE, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)startdate_str, (HUGEDATA INT8U *)form_string, NULL);
	//	Menu_AddEndFunction(11, callback_end_dc_checkdate);
	Menu_SetGeneralFlag(M_ID_SCROLL_START_DATE, day_type);
	Menu_SetFixedCharacter(M_ID_SCROLL_START_DATE, date_sep);
	
	caption_split_by_code(0x1C17, &cap, 0);	// "End   Date : "
	cap.input_x=strlen((char *)cap.form)+1;
#ifndef USE_SINGLE_LINE_GRAPHIC
	strcat((char *)cap.form, date_form);
	strcat((char *)cap.form, daytype_str);
#endif
	//caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_END_DATE, (HUGEDATA char *)cap.form, MENU_TYPE_DATE, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)enddate_str, (HUGEDATA INT8U *)form_string, NULL);
	//	Menu_AddEndFunction(12, callback_end_dc_checkdate);
	Menu_SetGeneralFlag(M_ID_SCROLL_END_DATE, day_type);
	Menu_SetFixedCharacter(M_ID_SCROLL_END_DATE, date_sep);
	Menu_SetNextPage();
	
	sprintf(form_string,"99%c99%c99", time_sep, time_sep);
	caption_split_by_code(0x1C16, &cap, 0);	// "Start Time :"
#ifndef USE_SINGLE_LINE_GRAPHIC
	cap.input_x=strlen((char *)cap.form)+1;
	strcat((char *)cap.form,time_form);
	//	strcat((char *)cap.form,timetype_str);
#endif
	//caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_START_TIME, (HUGEDATA char *)cap.form, MENU_TYPE_TIME, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)starttime_str, (HUGEDATA INT8U *)form_string, NULL);
	//	Menu_AddEndFunction(13, callback_end_dc_checktime);
	Menu_SetFixedCharacter(M_ID_SCROLL_START_TIME, time_sep);
	
	caption_split_by_code(0x1C18, &cap, 0);	// "End   Time :"
#ifndef USE_SINGLE_LINE_GRAPHIC
	cap.input_x=strlen((char *)cap.form)+1;
	strcat((char *)cap.form,time_form);
	//	strcat((char *)cap.form,timetype_str);
#endif
	//caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_SCROLL_END_TIME, (HUGEDATA char *)cap.form, MENU_TYPE_TIME, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)endtime_str, (HUGEDATA INT8U *)form_string, NULL);
	//	Menu_AddEndFunction(14, callback_end_dc_checktime);
	Menu_SetFixedCharacter(M_ID_SCROLL_END_TIME, time_sep);
	
	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		memcpy(scroll.text, scroll_str, text_size);
		scroll.stype = effect;
		scroll.use = use_scroll;
		scroll.image_id = image_id;
		scroll.on_time = on_time;
		scroll.blink_count = blink_count;
		scroll.font_size = font_size;
		scroll.smode = use_schedule;
		scroll.week = str2binary((INT8U *)dayofweek_str);
		//MsgOut(startdate_str);
		input_convert_date(day_type, (INT8U *)startdate_str, &scroll.start_year, &scroll.start_month, &scroll.start_day);
		//sprintf(MsgBuf, "y=%d, m=%d, d=%d\r\n", scroll.start_year, scroll.start_month, scroll.start_day);
		//MsgOut(MsgBuf);
		input_convert_date(day_type, (INT8U *)enddate_str, &scroll.end_year, &scroll.end_month, &scroll.end_day);
		input_convert_time((INT8U*)starttime_str, &scroll.start_hour, &scroll.start_min, &scroll.start_sec);
		input_convert_time((INT8U*)endtime_str, &scroll.end_hour, &scroll.end_min, &scroll.end_sec);
		addr = scroll_id - 1;
		addr*= scroll_size;
		addr+= addr_start;
		set_base_sparam(addr,(INT8U *)&scroll,scroll_size);
		if (network_status.service_flag&0x20)	// send plu
		{
			commun_net_mask(3, (INT16U*)&scroll_id, 0xff, 1);
		}
	}
}

#ifndef DISABLE_LIST_MENU
void  scroll_message_list(void)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	SCROLL_MESSAGE_STRUCT scroll;
	INT16U dlength;
	
	char   	string_buf[34];//, end_flag = 0;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, str_size, reverse_num;
	INT16S	j = 0, result, min_num, max_num;
	INT16S	line_ct, lcd_size, start_id;
	INT16U	read_code;
	INT32U	addr,addr_flash;//, cap_addr;
	char  *sc_mode[]={"a","d"};
	INT8U	linenum;			    //se-hyung 20071002	     list볼 범위 입력창 아래 쓰레기값 나오는거 수정
	
	str_size = sizeof(scroll);
LIST_START:
	caption_split_by_code(0x2703, &cap,0);
	min_num	= cap.input_min;					//
	max_num = cap.input_max; 					// 
	dlength = cap.input_dlength;
	if(min_num <= 0)  {
		disp_p = point(0,0);
		message_error(disp_p);
	}
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1433,1,1);
	linenum = Startup.line_max;			      //se-hyung 20071002  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
	if (linenum > 3) linenum = 3;			      //se-hyung 20071002  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
	for(i = 0;i<linenum; i++)  {			      //se-hyung 20071002  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
		caption_split_by_code(0x1E00+i, &cap,0);
		cap.input_length=dlength;
		cap.input_dlength=dlength;
		cap.input_min=min_num;
		cap.input_max=max_num;
		if(i != 2) caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
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
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
	}
	Dsp_Diffuse();
	start_num = min_num;
	end_num = max_num;
	for(i=0; i<2; i++)  {
REN1:
		if(i == 0)	ret_long = start_num;		
		if(i == 1)	ret_long = end_num;
		caption_split_by_code(0x1E00+i, &cap,0);
		cap.input_length=dlength;
		cap.input_dlength=dlength;
		cap.input_min=min_num;
		cap.input_max=max_num;
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		 Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		 hebrew_codetemp[strlen(cap.form)] = 0;
		 disp_p.x = 202 - disp_p.x;
		 Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
		 
		 result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,  0,(long*)&ret_long,cap.input_dlength, 
				 (i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
			 Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
			 arab_codetemp[strlen(cap.form)] = 0;
			 disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			 Dsp_Write_1stringLong(disp_p, arab_codetemp);

			 result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,  0,(long*)&ret_long,cap.input_dlength, 
				 (i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long,cap.input_dlength, 
  				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
			case KP_ARROW_UP:  if(i > 0)	i--;
						else 	BuzOn(2);
						goto REN1;    					   
			case KP_ESC:	   goto RET_ESC;
			case KP_SAVE:	   start_num = min_num;
						end_num = max_num;
						i++;
						break;
			case KP_PAGE_DN :
			case KP_ARROW_DN:
			case KP_ENTER   :
				if ( ret_long > max_num || ret_long < min_num ) 
				{
					error_display_form1(0, min_num, max_num);
					goto REN1;	
				}
				if(i == 0)	start_num = ret_long;
				if(i == 1)	end_num = ret_long;
				break;
				
			default:	BuzOn(2);	goto REN1;		     	 	
		}
	}
	
	
	lcd_size   = Dsp_GetChars();
	line_ct    = Startup.line_max; // (Startup.menu_type?3:2);
	addr_flash = DFLASH_BASE + FLASH_SCROLL_AREA;
	cur_id     = start_num;
	start_id   = start_num;
	
READ_START:
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FF, &cap,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071002     no. reference(descript.) 한줄 띄우기
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
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
	}
	
	j = 0;
	while (cur_id <= end_num)
	{
		addr =  addr_flash + (long)str_size * (cur_id-1);			
		read_code = Flash_bread(addr);
		if ((read_code >= 1) && (read_code <= MAX_SCROLL_EFFECT)) {
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&scroll, str_size ); 
			scroll.text[26]=0;
#ifdef HEBREW_FONT
		 	sprintf(string_buf,
		 		"%2d )%s(%s", 
				cur_id, 
		 		sc_mode[scroll.smode], 
		 		scroll.text);
#else
			sprintf(string_buf,
		 		"%2d (%s)%s", 
				cur_id, 
		 		sc_mode[scroll.smode], 
		 		scroll.text);
#endif
			string_buf[lcd_size] = 0;	       		
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);	 
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
			hebrew_codetemp[strlen(string_buf)] = 0;	 
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//기존 아랍ROM 적용되어 있음
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
			Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 1); 
			eng_codetemp[strlen(string_buf)] = 0;
			Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
			Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
			arab_codetemp[strlen(string_buf)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#else
			Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
#endif            
			Dsp_Diffuse();
			if (j == 1)
				start_id = cur_id;	
			
			if (j == line_ct)
				break;
		}
		cur_id++;	
	}
	if (cur_id >= end_num && j == 0) 
	{
		error_display_caption(0xE00C);
		goto  LIST_START;
	}
	cur_id = start_id;
	
KEY_INPUT:
	while(!KEY_Read());
	BuzOn(1);
	
	switch(KeyDrv.CnvCode) 
	{
		case KP_ESC:
			goto RET_ESC;
		case KP_ARROW_UP:
			while(1)  
			{
				if(cur_id <= start_num) 
				{
					BuzOn(2);
					cur_id = start_num;
					goto KEY_INPUT;
				}
				cur_id--;
				addr =  addr_flash + (long)str_size * (cur_id-1);			
				read_code = Flash_bread(addr);
				if ((read_code >= 1) && (read_code <= MAX_SCROLL_EFFECT)) goto READ_START;
			}
		case KP_PAGE_UP :
			reverse_num = 0;
			while(1)  
			{
				if(cur_id <= start_num) 
				{
					BuzOn(2);
					cur_id = start_num;
					goto READ_START;
				}
				cur_id--;
				addr =  addr_flash + (long)str_size * (cur_id-1);			
				read_code = Flash_bread(addr);
				if ((read_code >= 1) && (read_code <= MAX_SCROLL_EFFECT)) {
					reverse_num++;
					if(reverse_num == line_ct) 
						goto READ_START;
				}
			}
		case KP_PAGE_DN :
			reverse_num = 0;
			while(1)  
			{
				if(cur_id >= end_num) 
				{
					BuzOn(2);
					error_display_caption(0xE00E);
					if (reverse_num < line_ct)
						cur_id = start_id;
					else
						cur_id = end_num;
					goto READ_START;
				}
				else
					cur_id++;
				addr =  addr_flash + (long)str_size * (cur_id-1);			
				read_code = Flash_bread(addr);
				if ((read_code >= 1) && (read_code <= MAX_SCROLL_EFFECT)) {
					reverse_num++;
					if(reverse_num == line_ct) 
						goto READ_START;
				}
			}
		case KP_ENTER:		
		case KP_ARROW_DN:
			reverse_num = cur_id;
			while(reverse_num <= end_num)  
			{
				reverse_num++;
				addr =  addr_flash + (long)str_size * (reverse_num-1);			
				read_code = Flash_bread(addr);
				if ((read_code >= 1) && (read_code <= MAX_SCROLL_EFFECT)) {
					cur_id = reverse_num;
					goto READ_START;
				}
			}
			BuzOn(2);
			error_display_caption(0xE00E);
			goto KEY_INPUT;
		case KP_FEED:
			keyapp_feed(OFF);
			goto KEY_INPUT;
		case KP_TEST:
			scroll_print_list(start_num, end_num);
			goto KEY_INPUT;		
		default :	
			BuzOn(2);
			goto KEY_INPUT;
	}
RET_ESC:    return;
}
#endif //#ifndef DISABLE_LIST_MENU

void  scroll_message_config(void)
{
	INT32U wait_time;
	////	INT32U speed;
	INT8U result;
	CAPTION_STRUCT cap;
	
	wait_time=(INT32U)get_global_wparam(GLOBAL_SCROLL_WAIT_TIME);
	wait_time /= 10;
	////	speed = (INT32U)get_global_bparam(GLOBAL_SCROLL_SPEED);
	
	
	Menu_Init();
	
	caption_split_by_code(0x1431, &cap, 0);//""
	Menu_HeadCreate((HUGEDATA char *)cap.form);
	
	caption_split_by_code(0x2701, &cap, 0);//""
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &wait_time, NULL, NULL, NULL);
	
	////	caption_split_by_code(0x2702, &cap, 0);
	////	caption_adjust(&cap,NULL);
	////	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
	////						cap.input_max, cap.input_min, 0, &speed, NULL, NULL, NULL);						
	
	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		wait_time *= 10;
		set_global_wparam(GLOBAL_SCROLL_WAIT_TIME, (INT16U)wait_time);
		Startup.scroll_timeout = (INT16U)wait_time;
		////		set_global_bparam(GLOBAL_SCROLL_SPEED, (INT8U)speed);
	}
}


INT8U  scroll_effect = 0;
INT16S    scroll_leng,scroll_data_leng;
//INT8U  scroll_status = 0;
INT16U scroll_timer;
//INT16U scroll_speed;
INT16U scroll_speed_timer;
INT16S    scroll_pnt=-1;
INT16S	scroll_pt = 0;
INT16S    scroll_startid=0;
INT8U  scroll_exist=1;
INT8U  scroll_msg[MAX_SCROLL_LINE][MAX_SCROLL_DATA+MAX_SIDE_CHAR*2+1];
INT8U  scroll_blink_timer = 0;
INT8U  scroll_blink_counts = 0;
INT8U  scroll_blink_status = 1;
INT8U  scroll_max_side_char = MAX_SIDE_CHAR;
INT8U  scroll_line;
INT8U  scroll_image;
INT8U  scroll_fontsize;
INT8U  scroll_ontime;
INT8U  scroll_blinkcount;
INT8U  scroll_checktime = 3;	//0.3s
INT8U  scroll_next = ON;
PLU_SCROLL_STRUCT PLU_SCROLL;
extern void scroll_message_init(void);

void scroll_message_start(INT8U v)
{
	
	//	INT8U	temp;	 //se-hyung 080124
	
	if (v) {
		//		temp	= get_global_bparam(GLOBAL_SALE_SETUP_FLAG7); 
		//		if (!(temp & 0x04) && status_scale.cur_pluid)		return;	   //se-hyung 080124
		//scroll_message_init();
		//		if (scroll_exist) {
		//			//scroll_speed_timer=SysTimer100ms;
		//			LCD_DrawBackgroundScrollMsg();
		//			//Dsp_Fill_Buffer(0x00);
		//			//Dsp_Diffuse();
		//		} else {
		//			//scroll_timer=SysTimer100ms;
		//			scroll_effect = 0;
		//		}
	} else {
		if (scroll_effect)
		{
			sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
			//LCD_DrawBackgroundSaleMode();
			//sale_display_update(UPDATE_ALL);
			//Dsp_ChangeMode(DSP_SALE_MODE);
		}
		//scroll_timer=SysTimer100ms;
		scroll_effect = 0;
	}
	scroll_timer=SysTimer100ms;
}

long scroll_date_cur(INT8U kind)
{
	long week,year,month,day,hour,min,sec;
	
	week	=(long)bcd2hex(RTCStruct.day);
	year	=(long)bcd2hex(RTCStruct.year);
	month	=(long)bcd2hex(RTCStruct.month);
	day    	=(long)bcd2hex(RTCStruct.date);
	hour	=(long)bcd2hex(RTCStruct.hour);
	min    	=(long)bcd2hex(RTCStruct.min);
	sec    	=(long)bcd2hex(RTCStruct.sec);
	
	switch (kind) {
		case 2: return week;
		case 1: return ((year*10000L)+(month*100L)+day);
		case 0: return (hour*10000L+min*100L+sec);
	}
    return 0;
}

long scroll_date_construct(INT8U a,INT8U b,INT8U c)
{
	long la,lb;
	la=(long)a;	la*=10000l;
	lb=(long)b;	lb*=100L;	la+=lb;
	lb=(long)c;	la+=lb;
	return la;
}

void scroll_message_init(void)
{
	SCROLL_MESSAGE_STRUCT scroll;
	char	readMsg[MAX_SCROLL_LINE][MAX_SCROLL_DATA+1]={0,};
	INT16S    i;
	INT32U  addr;
	INT32U  date,time,start_date,end_date,start_time,end_time;
	INT8U   week;
	INT16U  total_leng;
	INT16U  leng[MAX_SCROLL_LINE];
	INT16U  j, k;
	
	//	char msg[32];
	//new
	if (scroll_startid>MAX_SCROLL_NO) scroll_startid=0;
	scroll_exist=0;
	// Check Daily Schedule
	week=(INT8U )scroll_date_cur(2);
	date=(INT32U)scroll_date_cur(1);
	time=(INT32U)scroll_date_cur(0);
	//for (i=0; i<MAX_SCROLL_NO; i++) {
	//for (i=scroll_startid; i<MAX_SCROLL_NO; i++) {
	for (j = 0; j < MAX_SCROLL_NO; j++) {
		i = scroll_startid + j;
		i %= MAX_SCROLL_NO;
		addr = i;
		//addr%= (MAX_SCROLL_NO);
		addr*=sizeof(scroll);
		addr+=FLASH_SCROLL_AREA;
		get_base_sparam(addr,(INT8U *)&scroll,sizeof(scroll));
		if ((scroll.stype >= 1) && (scroll.stype <= MAX_SCROLL_EFFECT)) {
		    //if (scroll.stype == 7 && !isdigit(scroll.text[0])) continue;
		    if (scroll.use==1) {
				if (scroll.smode==1) {
					start_date=(INT32U)scroll_date_construct(scroll.start_year,scroll.start_month,scroll.start_day);
					end_date  =(INT32U)scroll_date_construct(scroll.end_year,scroll.end_month,scroll.end_day);
					start_time=(INT32U)scroll_date_construct(scroll.start_hour,scroll.start_min,scroll.start_sec);
					end_time  =(INT32U)scroll_date_construct(scroll.end_hour,scroll.end_min,scroll.end_sec);
					if (end_time==0) end_time=240000l;
					if (end_date==0) end_date=999999l;
					//h1=start_time>>16;
					//h2=end_time>>16;
					//if (h1>h2) {
					//	h3=time>>16;
					//}
					if ((date>=start_date) && (date<=end_date)) {
						if ((time>=start_time) && (time<=end_time)) {
							//week = (0x80 >> week);
							if (scroll.week&(0x80 >> week)) 
							{
								scroll_exist = 1;
								scroll_startid = i;//+1;
								//break;
							}
						} //if ((time>=start_time) && (time<=end_time)) {
					} //if ((date>=start_date) && (date<=end_date)) {
				} else {
					scroll_exist = 1;
					scroll_startid = i;
					//break;
				}
		    }
		}
		if (scroll_exist)
		{
			if (!scroll.text[0])
			{
				scroll_exist = 0;
				continue;
			}
			break;
		}
	}
	if(!scroll_exist)
	{
		scroll_startid = 0;
		scroll_effect = 0;
		//scroll_timer=SysTimer100ms + Startup.scroll_timeout;
		return;
	}
	scroll_effect=scroll.stype;
	scroll_image = 0;
	scroll_fontsize = 1;
	if (scroll_effect <= 2) scroll_image = 0;	// effect 1,2는 text만 가능
	scroll_ontime = scroll.on_time;
	scroll_checktime = scroll_ontime;
	scroll_blinkcount = scroll.blink_count;
	if (scroll_fontsize <= 0) scroll_fontsize = DSP_SCROLL_MSG_MAGX1;
	scroll.text[MAX_SCROLL_DATA-1]=0;
	scroll_line = 1;
	j = 0;
	k = 0;
	for (i = 0; i < MAX_SCROLL_DATA; i++)
	{
		if (scroll.text[i] == ASC_LF)
		{
			if (scroll_line >= MAX_SCROLL_LINE) break;
			readMsg[j][k] = 0x00;
			scroll_line++;
			j++;
			k = 0;
			continue;
		}
		else if (scroll.text[i] == 0x00)
		{
			break;
		}
		readMsg[j][k] = scroll.text[i]; 
		k++;
	}
	readMsg[j][k] = 0x00;
	
	scroll_max_side_char = MAX_SIDE_CHAR;
	if (scroll_fontsize) scroll_max_side_char /= scroll_fontsize;

//#if !defined(USE_ARAB_FONT) && !defined(HEBREW_FONT)
	total_leng = MAX_SCROLL_DATA + scroll_max_side_char * 2;
//#endif
	scroll_data_leng = 0;
	for (j = 0; j < MAX_SCROLL_LINE; j++)
	{
		str_trim(readMsg[j]);
/*#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
		if (scroll_effect == 3)	// blink 일 때, 아랍어일때는 오른쪽 정렬, 영문일때는 왼쪽 정렬를 맞추기 위함
		{
			str_fill((char *)readMsg[j], scroll_max_side_char);	// side_char equal window size
		}
#endif
#endif*/
		leng[j] = strlen((char *)readMsg[j]);
		if (leng[j] > scroll_data_leng) scroll_data_leng = leng[j];	// check maximum length
/*#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
		memset(scroll_msg[j], 0x20, scroll_max_side_char);
		scroll_msg[j][scroll_max_side_char] = 0;
#else
		scroll_msg[j][0] = 0;
#endif
#else*/
		scroll_msg[j][0] = 0;
//#endif
		strcat((char *)scroll_msg[j], (char *)readMsg[j]);
/*#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
		//str_fill((char *)scroll_msg[j], total_leng);
	}		 
	total_leng = scroll_data_leng + scroll_max_side_char * 2;
	for (j = 0; j < MAX_SCROLL_LINE; j++)
	{ 
#endif
#endif*/
		str_fill((char *)scroll_msg[j], total_leng);
	}
	
	scroll_blink_status = 1;
	switch (scroll_effect) {
		default:
			scroll_effect = 1;
		case 1:
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
			//scroll_pt = scroll_max_side_char * 2 + scroll_data_leng;
			scroll_pt = -(scroll_data_leng);
#else
			scroll_pt = scroll_max_side_char;
#endif
#else
			scroll_pt = scroll_max_side_char;
#endif
			break;
		case 2:
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
			scroll_pt = scroll_max_side_char;
#else
			scroll_pt = -(scroll_data_leng);
#endif
#else
			scroll_pt = -(scroll_data_leng);
#endif
			break;
		case 3:
/*#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
			scroll_pt = scroll_max_side_char + scroll_data_leng;
#else
			scroll_pt = 0;
#endif
#else*/
			scroll_pt = 0;
//#endif
			scroll_blink_counts = 0;
			break;
	}
	////	scroll_speed=(INT16U)get_global_bparam(GLOBAL_SCROLL_SPEED);
	
	scroll_startid++;
}

void	CheckScroll(void)// 메뉴로 들어갈때
{
	if(scroll_effect)
	{
		//Operation.pressKey=
		//Operation.pressKey = OFF;
		scroll_message_start(0);
	}
}
//extern INT16U	MsgDisplayTime;
extern INT16S	arab_codetemp_pnt;
#ifdef USE_PLU_SCROLLING	// PLU 길이가 길 경우 스크롤 됨
extern INT8U KeyFlagForDispRst;
void scroll_PLU_init(void)
{
	INT8U ret;
	ret=get_global_bparam(GLOBAL_SALE_SETUP_FLAG14);
	memset(&PLU_SCROLL,0,sizeof(PLU_SCROLL));
	PLU_SCROLL.pt=0;
	PLU_SCROLL.next=ON;
	PLU_SCROLL.onoff=(ret&0x01);
	PLU_SCROLL.state=PLU_SCROLL_TURNOFF;
}

#define PLU_SCROLL_WAIT_TIME	15 // 1.5sec
#define PLU_SCROLL_SPEED		5 // 0.5sec
void scroll_PLU(void)
{
	char direct;
	POINT disp_p;
	INT16U cur_timer;
	INT8U old_font;
	INT16U j;
	char w, h;
	INT8U temp;
	
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	if(PLU_SCROLL.onoff == PLU_SCROLL_OFF || PLU_SCROLL.state == PLU_SCROLL_TURNOFF){
		return;
	}
#endif
	if (status_run.run_mode!=RUN_SALE) {
		return;
	}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	if (DateTimeDispTime) return;
	if (scroll_effect) return;
#endif
	
	
	PLU_SCROLL.delay = PLU_SCROLL_SPEED;
	if(PLU_SCROLL.state == PLU_SCROLL_TURNON) {
		PLU_SCROLL.state = PLU_SCROLL_START;	
		return;
	}
	if(PLU_SCROLL.state == PLU_SCROLL_START) PLU_SCROLL.delay = PLU_SCROLL_WAIT_TIME;
	cur_timer = SysTimer100ms - PLU_SCROLL.speed_timer;
	
	if (cur_timer < PLU_SCROLL.delay) return;	
	
	PLU_SCROLL.state = PLU_SCROLL_SCROLL;
	if (PLU_SCROLL.next)
	{
		PLU_SCROLL.pt= 0;
		PLU_SCROLL.next = OFF;
		PLU_SCROLL.state = PLU_SCROLL_TURNON;
		KeyFlagForDispRst = TRUE;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		Dsp_Fill_Buffer_without_TOP_Msg(0x00);
#endif		
	}
	PLU_SCROLL.speed_timer = SysTimer100ms;
	
	old_font = DspStruct.Id1;
	direct = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	Dsp_Fill_Buffer(0x00);
#endif
	KeyFlagForDispRst = TRUE;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	Dsp_Fill_Buffer_without_TOP_Msg(0x00);
	DspLoadFont1(DSP_PLU_FONT_ID);
#else
	DspLoadFont1(DSP_SCROLL_MSG_FONT_ID);
#endif
	Dsp_FontSize(0, 0);
	w = display_font_get_width();
	h = display_font_get_height();
	disp_p = point(DSP_SCROLL_MSG_Y, DSP_SCROLL_MSG_X + PLU_SCROLL.pt * w);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)&PLU_SCROLL.msg[0], strlen(&PLU_SCROLL.msg[0]), 0, 0); 
	hebrew_codetemp[strlen(&PLU_SCROLL.msg[0])] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)
	Convert_ArabPreview((INT8U *)&PLU_SCROLL.msg[0], strlen(&PLU_SCROLL.msg[0]), 0, 0, 0); 
	arab_codetemp[strlen(&PLU_SCROLL.msg[0])] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, (HUGEDATA char *)&PLU_SCROLL.msg[0]);
#endif
	DspLoadFont1(old_font);
	
	// Next Condition
	PLU_SCROLL.pt--;
	if (PLU_SCROLL.pt < -(PLU_SCROLL.data_leng-MAX_SIDE_CHAR/2)){
		PLU_SCROLL.next = ON;
	}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	sale_display_update(UPDATE_CENTER);
#endif
}
#endif
void scroll_message(void)
{
	char direct;
	POINT disp_p;
	INT16U cur_timer;
	INT8U old_font;
	INT16U j;
	char w, h;
	INT8U temp;
#ifdef USE_ARAB_FONT
#ifndef CPU_PIC32MZ
	INT8U i;
	INT16U arab_buf[100];
	memset((INT16U *)arab_buf, 0, 100);
#endif
#endif

#ifdef USE_PLU_SCROLLING	// PLU 길이가 길 경우 스크롤 됨
	temp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG14);
	PLU_SCROLL.onoff=(temp&0x01);
	if(PLU_SCROLL.onoff != PLU_SCROLL_OFF && PLU_SCROLL.state != PLU_SCROLL_TURNOFF){
		scroll_PLU();
		scroll_message_start(0);
		return;
	}
#endif
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	if (DateTimeDispTime) return;
#endif
	if (Startup.scroll_timeout==0) return;
	
	if (!ADM_GetData.Stable || Operation.pressKey) 
	{
		if (Operation.pressKey && MsgDisplayTime)	return;
		scroll_message_start(0);
		Operation.pressKey = OFF;
		return;
	}
	
	if (status_run.run_mode!=RUN_SALE) {
		return;
	}
	
	if (Clerk_Subtotal_List == 1) {
		return;
	}
	temp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG7); 
	if (!(temp & 0x04) && status_scale.cur_pluid) return;
	
	
	if (scroll_effect == 0) {
		cur_timer = SysTimer100ms - scroll_timer;
		if (cur_timer < Startup.scroll_timeout) return;
		scroll_message_start(1);
		scroll_next = ON;
	} else {
		cur_timer = SysTimer100ms - scroll_speed_timer;
		if (cur_timer < scroll_checktime) return;
	}
	//if (scroll_pt > scroll_max_side_char || scroll_pt < -(scroll_data_leng) || (scroll_effect == 3 && scroll_blink_counts >= scroll_blinkcount))
	if (scroll_next)
	{
		scroll_next = OFF;
		scroll_message_init();
		if (scroll_exist == 0)	return;
		LCD_DrawBackgroundScrollMsg();
	}
	scroll_speed_timer = SysTimer100ms;
	//if (scroll_exist == 0) return;
	
	old_font = DspStruct.Id1;
	direct = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;
	
	Dsp_Fill_Buffer(0x00);
	if (scroll_blink_status)
	{
		if (scroll_image)	// image가 text보다 우선
		{
		}
		else
		{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			DspLoadFont1(DSP_PLU_FONT_ID);
#else
			DspLoadFont1(DSP_SCROLL_MSG_FONT_ID);
#endif
			Dsp_FontSize(scroll_fontsize, scroll_fontsize);
			w = display_font_get_width();
			h = display_font_get_height();
			for (j = 0; j < scroll_line; j++)
			{
				if (j * h > LCD_X_MARGIN) break;
				disp_p = point(DSP_SCROLL_MSG_Y + j * h, DSP_SCROLL_MSG_X + scroll_pt * w);
#ifdef HEBREW_FONT
				disp_p.x = DSP_SCROLL_MSG_X + (scroll_pt - 1) * w;                
				Convert_HebrewPreview((INT8U *)&scroll_msg[j][0], strlen(&scroll_msg[j][0]), 0, 0);
				hebrew_codetemp[strlen(&scroll_msg[j][0])] = 0;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//scroll message, 기존 아랍ROM 적용되어 있음
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
				Convert_ArabPreview((INT8U *)&scroll_msg[j][0], strlen((char *)&scroll_msg[j][0]), 0, 0, 1); 
				eng_codetemp[strlen((char *)&scroll_msg[j][0])] = 0;
				Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
				Convert_ArabPreview((INT8U *)&scroll_msg[j][0], strlen((char *)&scroll_msg[j][0]), 0, 0, 0); 
				arab_codetemp[strlen((char *)&scroll_msg[j][0])] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#else
				Dsp_Write_String(disp_p, (HUGEDATA char *)&scroll_msg[j][0]);
#endif
#else
				Dsp_Write_String(disp_p, (HUGEDATA char *)&scroll_msg[j][0]);
#endif
			}
			DspLoadFont1(old_font);
		}
	}
	Dsp_Diffuse();
	DspStruct.DirectDraw = direct;
	
	// Next Condition
	switch (scroll_effect) {
		default:
		case 1:				
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
			scroll_pt++;
			if (scroll_pt > scroll_max_side_char) scroll_next = ON;
#else
			scroll_pt--;
			if (scroll_pt < -(scroll_data_leng)) scroll_next = ON;
#endif
#else
			scroll_pt--;
			if (scroll_pt < -(scroll_data_leng)) scroll_next = ON;
#endif
			scroll_checktime = scroll_ontime;
			break;
		case 2:
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
			scroll_pt--;
			if (scroll_pt < -(scroll_data_leng)) scroll_next = ON;
#else
			scroll_pt++;
			if (scroll_pt > scroll_max_side_char) scroll_next = ON;
#endif
#else
			scroll_pt++;
			if (scroll_pt > scroll_max_side_char) scroll_next = ON;
#endif
			scroll_checktime = scroll_ontime;
			break;
		case 3:
			if (scroll_blink_status) 
			{
				scroll_checktime = scroll_ontime;
			}
			else
			{
				scroll_blink_counts++;
				scroll_checktime = 3;	// 0.3sec
			}
			scroll_blink_status ^= 1;
			if (scroll_blink_counts >= scroll_blinkcount) scroll_next = ON;
			break;
	}
}

INT8U scroll_message_check_number(INT16U scr_no)
{
	INT32U 	flash_addr;
	INT8U	effect_type;
	
	flash_addr = DFLASH_BASE + FLASH_SCROLL_AREA;
	flash_addr += sizeof(SCROLL_MESSAGE_STRUCT)*(scr_no-1);
	effect_type = Flash_bread(flash_addr);
	if(effect_type >= 1 && effect_type <= MAX_SCROLL_EFFECT) 
	{
		return ON;
	}
	return OFF;
}
