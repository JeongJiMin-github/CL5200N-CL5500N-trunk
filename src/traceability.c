/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	department.c
|*  Version			:	v 0.5
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created			:	2003/12/16
|*  Description		:	workplace
|*              				
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"

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
#include "prt_cmd.h"

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
#include "traceability.h"
#include "menu_code_text1.h"
#include "initial.h"
#include "pgm_app.h"
#include "prt_list.h"
#include "mode_error.h"
#include "workplace.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "global_set.h"

#ifdef USE_TRACE_STANDALONE
#include "department.h"
#include "shopname.h"
#endif				      

extern void plucall_settrace(INT32U addr,INT16U caption,long rec,INT16S fstart,INT16S flength);
extern void plucall_trace(long num);
extern void display_message_page_mid(char *str);
#ifdef USE_TOPMART_STRUCTURE
TRACE_BREED_TYPE breed_type[11] ={
	{11, "한우"},
	{12, "한우(암)"}, 
	{13, "한우(수)"}, 
	{14, "한우(거세)"}, 
	{15, "육우"}, 
	{16, "육우(암)"}, 
	{17, "육우(수)"}, 
	{99, "없음"}, 
	{31, "돈육"}, 
	{18, "육우(거세)"}, 
	{19, "젖소"},
};
#endif
#ifdef USE_TRACEABILITY
void    traceability_delete(void)		// command code  0x1372
{   	menu1_delete(0x1333, 35, FLASH_TRACEABILITY_AREA, TRACEABILITY_STRUCT_SIZE);		}


void callback_country_no(long num)
{
 	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT32U	addr;
	INT16U 	w;
	INT16U  read_code;

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

	caption_split_by_code(0x134A, &cap, 0);
	if (cap.input_max ==0) {
		message_error(disp_p);
		return;
	}
	addr  = COUNTRY_STRUCT_SIZE * (num - 1);
	addr += DFLASH_BASE + FLASH_COUNTRY_AREA;
	read_code = Flash_wread(addr);
	if(read_code == num)  {
		addr+=2;
		Flash_sread(addr, (HUGEDATA INT8U*)buffer, COUNTRY_STRUCT_SIZE-2);
	}
	else
	{	
		caption_split_by_code(0xE00C,&cap,0);
		strcpy(buffer,cap.form);
	}
	if (w >	COUNTRY_STRUCT_SIZE-2) w = COUNTRY_STRUCT_SIZE-2;
	buffer[w] = 0;
	DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[25-CallbackEditPosition_x] = 0;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//callback country no, 기존 아랍ROM 적용되어 있음
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
	eng_codetemp[strlen((char *)buffer)] = 0;
 #else
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
 #endif
 #if !defined(USE_ARAB_CONVERT) && !defined(USE_ENG_BASED_ARAB_SUPPORT)
 	disp_p.x += (25-1-CallbackEditPosition_x)*8;
 #endif
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
 	Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	DspStruct.Reverse = 0;
}

void callback_slaughter_no(long num)
{
 	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT32U	addr;
	INT16U 	w;
	INT16U  read_code;

	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);

	caption_split_by_code(0x134A, &cap, 0);
	if (cap.input_max ==0) {
		message_error(disp_p);
		return;
	}
	addr  = SLAUGHTERHOUSE_STRUCT_SIZE * (num - 1);
	addr += DFLASH_BASE + FLASH_SLAUGHTERHOUSE_AREA;
	read_code = Flash_wread(addr);
	if(read_code == num)  {
		addr+=2;
		Flash_sread(addr, (HUGEDATA INT8U*)buffer, SLAUGHTERHOUSE_STRUCT_SIZE-4);
	}
	else
	{	
		caption_split_by_code(0xE00C,&cap,0);
		strcpy(buffer,cap.form);
	}
	if (w >	SLAUGHTERHOUSE_STRUCT_SIZE-4) w = SLAUGHTERHOUSE_STRUCT_SIZE-4;
	buffer[w] = 0;
	DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[25-CallbackEditPosition_x] = 0;
	disp_p.x += (25-1-CallbackEditPosition_x)*8;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//callback slaught no, 기존 아랍ROM 적용되어 있음
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
	eng_codetemp[strlen((char *)buffer)] = 0;
	Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	disp_p.x += (25-1-CallbackEditPosition_x)*8;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	DspStruct.Reverse = 0;
}

void callback_cutting_no(long num)
{
 	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT32U	addr;
	INT16U 	w;
	INT16U  read_code;

	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);

	caption_split_by_code(0x134A, &cap, 0);
	if (cap.input_max ==0) {
		message_error(disp_p);
		return;
	}
	addr  = CUTTINGHALL_STRUCT_SIZE * (num - 1);
	addr += DFLASH_BASE + FLASH_CUTTINGHALL_AREA;
	read_code = Flash_wread(addr);
	if(read_code == num)  {
		addr+=2;
		Flash_sread(addr, (HUGEDATA INT8U*)buffer, CUTTINGHALL_STRUCT_SIZE-4);
	}
	else
	{	
		caption_split_by_code(0xE00C,&cap,0);
		strcpy(buffer,cap.form);
	}
	if (w >	CUTTINGHALL_STRUCT_SIZE-4) w = CUTTINGHALL_STRUCT_SIZE-4;
	buffer[w] = 0;
	DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[25-CallbackEditPosition_x] = 0;
	disp_p.x += (25-1-CallbackEditPosition_x)*8;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//callback cutting no, 기존 아랍ROM 적용되어 있음
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
	eng_codetemp[strlen((char *)buffer)] = 0;
	Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	disp_p.x += (25-1-CallbackEditPosition_x)*8;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	DspStruct.Reverse = 0;
}

#define M_ID_TRACE_NO		1
#define M_ID_TRACE_TEXT		2
#define M_ID_TRACE_BORN		3
#define M_ID_TRACE_BRED		4
#define M_ID_TRACE_SLAUGHT	5
#define M_ID_TRACE_CUTTING	6

void callback_end_trace(INT32U num)
{
	TRACEABILITY_STRUCT	traceability_str;
	INT32U addr;
	INT8U  id_trace_text, id_trace_born, id_trace_bred, id_trace_slaught, id_trace_cutting;

	id_trace_text = Menu_GetMenuId(M_ID_TRACE_TEXT);
	id_trace_born = Menu_GetMenuId(M_ID_TRACE_BORN);
	id_trace_bred = Menu_GetMenuId(M_ID_TRACE_BRED);
	id_trace_slaught = Menu_GetMenuId(M_ID_TRACE_SLAUGHT);
	id_trace_cutting = Menu_GetMenuId(M_ID_TRACE_CUTTING);

	addr = DFLASH_BASE + FLASH_TRACEABILITY_AREA;
	addr += (num - 1) * TRACEABILITY_STRUCT_SIZE;
	Flash_sread(addr, (HUGEDATA INT8U *)&traceability_str, TRACEABILITY_STRUCT_SIZE);
	if(traceability_str.code != num)
		memset((INT8U*)&traceability_str, 0x00, TRACEABILITY_STRUCT_SIZE);
	traceability_str.code = num;

	memcpy(Menu[id_trace_text].pstr, traceability_str.contents, sizeof(traceability_str.contents));
	*Menu[id_trace_born].pval = traceability_str.idata[0];
	*Menu[id_trace_bred].pval = traceability_str.idata[1];
	*Menu[id_trace_slaught].pval = traceability_str.idata[2];
	*Menu[id_trace_cutting].pval = traceability_str.idata[3];

      	MenuRefresh = ON;
}


void traceability_create(void)		// command code  0x137
{	
	TRACEABILITY_STRUCT	traceability_str;
	CAPTION_STRUCT 		cap;
	INT16U	result;
	INT32U  cur_id;
	INT16U  i;
	INT32U	flash_addr, start_addr;
	INT32U	born_no;
	INT32U	bred_no;
	INT32U	slaught_no;
	INT32U	cutting_no;
	INT8U	desc[20];

//	switch(input_num) {
//		case 0: 
//		case 1: plucall_settrace(FLASH_COUNTRY_AREA       ,0x134A,COUNTRY_STRUCT_SIZE       ,2,62); break;
//		case 2: plucall_settrace(FLASH_SLAUGHTERHOUSE_AREA,0x135A,SLAUGHTERHOUSE_STRUCT_SIZE,2,28); break;
//		case 3: plucall_settrace(FLASH_CUTTINGHALL_AREA   ,0x136A,CUTTINGHALL_STRUCT_SIZE   ,2,28); break;
//	}		
//	keyin_setcallback(plucall_trace);
	start_addr = DFLASH_BASE + FLASH_TRACEABILITY_AREA;

	while(1) {	
		caption_split_by_code(35,&cap,0);
		i = 0;
		while (1) {
			flash_addr = start_addr + TRACEABILITY_STRUCT_SIZE * (INT32U)i;
			traceability_str.code = Flash_wread(flash_addr);
			if (traceability_str.code == i + 1) {
				i++;
				if(i > cap.input_max) {
					cur_id = cap.input_min;
					break;
				}
			} else {
				cur_id = i + 1;
		       		break;
			}
		}//while(1)

		flash_addr = start_addr + (TRACEABILITY_STRUCT_SIZE * (cur_id - 1));
		Flash_sread(flash_addr, (HUGEDATA INT8U *)&traceability_str, TRACEABILITY_STRUCT_SIZE);
		if(traceability_str.code != cur_id)
			memset((INT8U*)&traceability_str, 0x00, TRACEABILITY_STRUCT_SIZE);	
		traceability_str.code = cur_id;
		memcpy(desc, traceability_str.contents, sizeof(traceability_str.contents));
		born_no = traceability_str.idata[0];
		bred_no = traceability_str.idata[1];
		slaught_no = traceability_str.idata[2];
		cutting_no = traceability_str.idata[3];

		
		Menu_Init();
#if !defined(USE_SINGLE_LINE_GRAPHIC)
		MenuNumLinePerPage = 2;
		MenuDefaultCallbackMode = 2;	// 2:bottom
#endif
	
		caption_split_by_code(0x1331, &cap,0);//""
		Menu_HeadCreate((HUGEDATA char *)cap.form);
	
		caption_split_by_code(35, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRACE_NO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &cur_id, NULL, NULL, NULL);
		Menu_AddEndFunction(M_ID_TRACE_NO, callback_end_trace);
	
		caption_split_by_code(0x133B, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRACE_TEXT, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, desc, NULL, NULL);	
		Menu_SetProperties(2, MENU_P_CHECK_BLANKS);
	
		caption_split_by_code(0x133C, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRACE_BORN, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &born_no, NULL, NULL, callback_country_no);
	
		caption_split_by_code(0x133D, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRACE_BRED, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &bred_no, NULL, NULL, callback_country_no);
	
		caption_split_by_code(0x133E, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRACE_SLAUGHT, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &slaught_no, NULL, NULL, callback_slaughter_no);
	
		caption_split_by_code(0x133F, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRACE_CUTTING, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &cutting_no, NULL, NULL, callback_cutting_no);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) {
			flash_addr = start_addr + (TRACEABILITY_STRUCT_SIZE * (cur_id - 1));
			traceability_str.code = cur_id;
			memcpy(traceability_str.contents, desc, sizeof(traceability_str.contents));
			traceability_str.idata[0] = born_no;
			traceability_str.idata[1] = bred_no;
			traceability_str.idata[2] = slaught_no;
			traceability_str.idata[3] = cutting_no;
			Flash_swrite(flash_addr, (HUGEDATA INT8U *)&traceability_str, TRACEABILITY_STRUCT_SIZE);
		} else if(result == MENU_RET_ESC) {
			break;
		}
	}




}

#ifndef DISABLE_LIST_MENU
void 	traceability_list(void)		// command code  0x1373
{		
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	TRACEABILITY_STRUCT	traceability_str;
	
	char   	string_buf[34], end_flag = 0;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, reverse_num;
	INT16S	j = 0, result, min_num, max_num, str_size;
	INT16S	line_ct, start_id, lcd_size;

	INT16U	read_code;
	INT32U	addr, addr_flash, cap_addr;

	INT16U	code = 0x1332, cap_code = 0x1E00;

	str_size = TRACEABILITY_STRUCT_SIZE;	

LIST_START:
//	cap_addr = caption_search_by_code(0x133A);			// Decide inpput boundary
//	_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));		//
	caption_split_by_code(35,&cap,0);
	caption_adjust(&cap,NULL);
	min_num	= cap.input_min;
	max_num = cap.input_max;
	if(min_num <= 0)  {
//		sprintf(string_buf, "caption error");
		disp_p = point(0,0);
//		Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
		message_error(disp_p);
	}

	Dsp_Fill_Buffer(0);
	//display_title_page(code);
	display_title_page_change(code,1,1);

			
	addr = caption_search_by_code(cap_code);		
	for(i = 0; i<3; i++)  {
		cap_addr = addr + CAP_MSG_LENGTH * i;
//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		if(i < 2) caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
		Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 1); 
		eng_codetemp[strlen(string_buf)] = 0;
		Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
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

		cap_addr = addr + CAP_MSG_LENGTH * i;
//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));	  
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));	  
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, 4, 
		   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, 4, 
		   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, 4, 
				   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
						 
			case KP_ARROW_UP:  // <CODE> JHC, 040531, MOdify
					   // [
					   if(i > 0)	i--;
					   else		BuzOn(2);
					   // ]
					   goto REN1;    					   
			// <CODE> JHC, 040531, Comments
			// [
			// case KP_PAGE_UP:
			// ]
			case KP_ESC:
				   	goto RET_ESC;

			case KP_SAVE:	   start_num = min_num;
					   end_num = max_num;
					   i++;
					   break;
			case KP_PAGE_DN:
			case KP_ARROW_DN:
			case KP_ENTER:	   
					   if ( ret_long > max_num || ret_long < min_num ) 
					   {
						// <CODE> JHC, 040607, Modify
						// [
						error_display_form1(0, min_num, max_num);
						// ]
						goto REN1;	
				           }
					   if(i == 0)	start_num = ret_long;
					   if(i == 1)	end_num = ret_long;
					   break;
					   				  				
		     	default:	BuzOn(2);	goto REN1;		     	 	
		} //switch (result)
	}//for(i=1; i<3; i++)

	
	lcd_size   = Dsp_GetChars();
	line_ct    = Startup.line_max; //(Startup.menu_type?3:2);
	addr_flash = DFLASH_BASE + FLASH_TRACEABILITY_AREA;
	cur_id     = start_num;
	start_id   = start_num;

	// <DEBG> JHC, 040329, New, -DebugCode
	// [
	// $1: JHC, 040529, Comment, -Not use
	// display_ulong(1, start_num, 0);
	// VFD7_Diffuse();
	// ]

READ_START:
	// <CODE> JHC, 040531, Modify
	// [
	// $MEMO: -modify display list all
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x7332, &cap,0);
//		disp_p = point(0,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071001
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
		read_code = Flash_wread(addr);
		if(read_code == cur_id /*>= min_num && read_code <= max_num*/)  
		{	
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&traceability_str, str_size ); 
		 	sprintf(string_buf, 
		 		"%3d :%03d,%03d,%03d,%03d", 
		 		traceability_str.code, 
		 		traceability_str.idata[0], 
		 		traceability_str.idata[1], 
		 		traceability_str.idata[2], 
		 		traceability_str.idata[3]);
			string_buf[lcd_size] = 0;
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0);
			hebrew_codetemp[strlen(string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//list, 기존 아랍ROM 적용되어 있음
			Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
			arab_codetemp[strlen(string_buf)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
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
	// <CODE> JHC, 040607, Modify
	// [
	if (cur_id >= end_num && j == 0) 
	{
		error_display_caption(0xE00C);
		goto  LIST_START;
	}
	// ]
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
			read_code = Flash_wread(addr);
			if(read_code == cur_id /*>= min_num && read_code <= max_num*/ )  
				goto READ_START;
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
			read_code = Flash_wread(addr);
			if(read_code == cur_id /*>= min_num && read_code <= max_num*/ )  
			{
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
			read_code = Flash_wread(addr);
			if(read_code == cur_id /*>= min_num && read_code <= max_num*/ )  
			{
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
			read_code = Flash_wread(addr);
			if(read_code == reverse_num /*&& (read_code >= min_num && read_code <= max_num)*/ )  
			{
			       	cur_id = reverse_num;
				goto READ_START;
			}
		}
		BuzOn(2);
		error_display_caption(0xE00E);
		goto KEY_INPUT;
	case KP_TEST:
		traceability_print_list(start_num, end_num);
		goto KEY_INPUT;	
	case KP_FEED:
		keyapp_feed(OFF);
		goto KEY_INPUT;	
	default :	
		BuzOn(2);
		goto KEY_INPUT;
	} //switch(KeyDrv.CnvCode)						
	// ]

	RET_ESC:    return;
}
#endif //#ifndef DISABLE_LIST_MENU
#endif

#ifdef USE_TRACE_STANDALONE

#define M_ID_TRS_INDEX		1
#define M_ID_TRS_IND_NO		2	//individual number
#define M_ID_TRS_GRADE_INDEX	3	//grade index
#define M_ID_TRS_SL_DATE	4	//slaught date
#define M_ID_TRS_WEIGHT		5	//weight
#define M_ID_TRS_BEED		6	//beed
#define M_ID_TRS_FARM		7	//farm
#define M_ID_TRS_SLAUGHT	8	//slaught house
#define M_ID_TRS_LOTFLAG	9	//lot flag
#define M_ID_TRS_BIND_IND_NO	10	//bind individual number
#define M_ID_TRS_GRADE		11	//grade
#define M_ID_TRS_HARMFUL	12	//harmfulness
#define M_ID_TRS_LOTPART	13	//lot part(묶음부위)
#ifdef USE_TRACE_1000QTY
#define M_ID_TRS_MEATKIND	14 //육종
//#define M_ID_TRS_HARMFLAG	15 //위해여부
#define M_ID_TRS_NONGJANGNO 16 //농장번호
#define M_ID_TRS_SLAUGHTNO	17 //도축장번호
#endif

extern void individualData_dataUpdate(INT32U num);
#ifdef USE_KOR_TRACE_999
extern void individualData_LotFlagUpdate(INT32U num);
#endif
//extern INT8U  plu_table_search(unsigned char deptno,long pluno,INT16U *idx,INT8U mode);
//extern INT8U  plu_table_individual[]; 

/*
//void grade_call_back(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	MENU_TYPE1_STRUCT	group_str;  // Group Table을 등급 Table로 사용 함	
	char	buffer[34];
	INT32U	addr;
	INT16S  	str_size,w;

	str_size = GROUP_STRUCT_SIZE;

	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)buffer,0x20, 34);

	w=Dsp_GetChars();
	buffer[w-1] = 0;
	DspStruct.DirectDraw = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	caption_split_by_code(0x1AF2,&cap,0);
	caption_adjust(&cap,NULL);
	if (cap.input_min ==0 || cap.input_max ==0) {
		message_error(disp_p);
		return;
	}
	if(num==0) {
		return;
	}
	if ((num<cap.input_min) || (num>cap.input_max)) {
		error_display_form1(0, cap.input_min, cap.input_max);
		return;
	}

	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&group_str, str_size);
	if(group_str.code>cap.input_max || group_str.code<cap.input_min) {	
		caption_split_by_code(0xE00C,&cap,0);
		DspStruct.DirectDraw = 1;
		cap.form[10]=0;
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;	
	} else {
		memcpy((INT8U*)buffer, group_str.text ,INDIV_GRADE_LEN);
		buffer[INDIV_GRADE_LEN-1]=0;
		DspStruct.DirectDraw = 1;
		//buffer[10]=0;
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	}       
}
*/

char IndivMsg_NonTrace[] = {"이력번호대상이 아닙니다"};
char IndivMsg_SameTraceNo[] = {"중복된 이력번호입니다"};
char IndivMsg_TraceMemoryFull[] = {"이력번호기억공간 부족"};

extern TRACE_STATUS_STRUCT CalledTraceStatus;
//[start]gm 090403
void individualNo_call_back(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	TRACE_INDIVIDUAL_IDENTITY 	indivStr;
	INT8U 	buffer[34];// 1 line = max char 34
	INT32U	addr;
	INT16U 	str_size;
	INT16U	w;
		
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)buffer, 0x20, 34);

	w = Dsp_GetChars();
	buffer[w-1] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);

	if(num == 0)
	{
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;
	}
#ifdef USE_CHECKING_HARMFUL_MEAT
	if(num >= MaxIndividualNo) {
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;
	}
#endif    
	//DspStruct.DirectDraw = 1;
	caption_split_by_code(0x1AF1, &cap,0);	 //individual number
	caption_adjust(&cap,NULL);

	str_size = TRACE_INDIVIDUAL_IDENTITY_SIZE;
#ifdef USE_TRACE_1000QTY //sewon 161201
	addr = EXT_MEMCARD_ADDR ;
#else
	addr = DFLASH_BASE;
#endif
	addr += FLASH_TRACE_STANDALONE_AREA;
	addr += ((num-1) * str_size);
	Flash_sread(addr, (HUGEDATA INT8U *)&indivStr, str_size);

	if(indivStr.index != num) {
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;	
	} else {
		memcpy((INT8U*)buffer, indivStr.individualNO, INDIVIDUAL_NO_LEN);
		buffer[INDIVIDUAL_NO_LEN] = 0;
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	}
}
//[end]gm 090403
#ifdef USE_TOPMART_STRUCTURE //sewon 170224
void breed_call_back(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	INT8U 	breed_str[sizeof(breed_type[0].contents)];
	INT8U 	buffer[49];
	INT32U	addr;
	INT16U 	str_size;
	INT16U	w;
#ifdef USE_CL5200_DISPLAY_DEFAULT
	CallbackEditPosition_y = 0;
	CallbackEditPosition_x = 16;
#endif	
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)buffer, 0x20, 48);

	w = Dsp_GetChars();
	buffer[w-1] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);

	caption_split_by_code(0x139A, &cap,0);	 //도축장 #:
	caption_adjust(&cap,NULL);

	trace_lookup_breed_from_table(num, breed_str, sizeof(breed_str));
	if(breed_str[0] == 0) {
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(char *)cap.form);
		return;	
	} else {
		Dsp_Write_String(disp_p,(char *)breed_str);
	}
}
#endif
void slaughHouseNo_call_back(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	TRACE_SLAUGHTHOUSE_STRUCT 	slaughHouseStr;
	INT8U 	buffer[TRACE_SLAUGHTHOUSE_NAME_SIZE];
	INT32U	addr;
	INT16U 	str_size;
	INT16U	w;
	
#ifdef USE_CL5200_DISPLAY_DEFAULT
	CallbackEditPosition_y = 0;
	CallbackEditPosition_x = 16;
#endif	
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)buffer, 0x20, TRACE_SLAUGHTHOUSE_NAME_SIZE);

	w = Dsp_GetChars();
	buffer[w-1] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);

	if(num == 0)
	{
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;
	}

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
	num = trace_slaughthouse_no2index(num);
	if (num == 0)
	{
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;
	}
#endif
	//DspStruct.DirectDraw = 1;
	caption_split_by_code(0x139A, &cap,0);	 //도축장 #:
	caption_adjust(&cap,NULL);

	str_size = TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE;
	addr = DFLASH_BASE;
	addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
	addr += ((num-1) * str_size);
	Flash_sread(addr, (HUGEDATA INT8U *)&slaughHouseStr, str_size);

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
	if(slaughHouseStr.index != trace_slaughthouse_index2no(num)) {
#else
	if(slaughHouseStr.index != num) {
#endif
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;	
	} else {
		memcpy((INT8U*)buffer, slaughHouseStr.slaughtHouse, TRACE_SLAUGHTHOUSE_NAME_SIZE);
		buffer[TRACE_SLAUGHTHOUSE_NAME_SIZE-1]=0;
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	}
}

void meatUseNo_call_back(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	TRACE_SPECIALUSE_STRUCT 	str;
	INT8U 	buffer[49];
	INT32U	addr;
	INT16U 	str_size;
	INT16U	w;

#ifdef USE_CL5200_DISPLAY_DEFAULT
	CallbackEditPosition_y = 0;
	CallbackEditPosition_x = 12;
#endif	

	if(num == 0) return;
		
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)buffer, 0x20, 48);

	w = Dsp_GetChars();
	buffer[w-1] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	caption_split_by_code(0x138A, &cap,0);	 //용도 #:
	caption_adjust(&cap,NULL);

	str_size = TRACE_SPECIALUSE_STRUCT_SIZE;
	addr = DFLASH_BASE;
	addr += FLASH_TRACE_SPECIALUSE_AREA;
	addr += ((num-1) * str_size);
	Flash_sread(addr, (HUGEDATA INT8U *)&str, str_size);

	if(str.index != num) {
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;	
	} else {
		memcpy((INT8U*)buffer, str.meatUse, 48);
		buffer[48]=0;
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	}
}

#ifdef USE_TRACE_MEATPART
void meatPartNo_call_back(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	TRACE_MEATPART_STRUCT 	str;
	INT8U 	buffer[49];
	INT32U	addr;
	INT16U 	str_size;
	INT16U	w;

	if(num == 0) return;
		
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)buffer, 0x20, 48);

	w = Dsp_GetChars();
	buffer[w-1] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	caption_split_by_code(0x13AA, &cap,0);	 //부위 #:
	caption_adjust(&cap,NULL);

	str_size = TRACE_MEATPART_STRUCT_SIZE;
	addr = DFLASH_BASE;
	addr += FLASH_TRACE_MEATPART_AREA;
	addr += ((num-1) * str_size);
	Flash_sread(addr, (HUGEDATA INT8U *)&str, str_size);

	if(str.index != num) {
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;	
	} else {
		memcpy((INT8U*)buffer, str.meatPart, 48);
		buffer[48]=0;
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	}
}
#endif

extern INT8U cksum_indivNo(char *buf);
INT8U check_individual_no(INT8U *str)
{
	INT8U lotFlag;
	INT8U string_buf[32];

	lotFlag = checkIndividualNo(str);
	if (cksum_indivNo((char *)str) == FALSE && UseCheckIndivNo && !lotFlag) 
	{
		BuzOn(2);
		sprintf((char*)string_buf, "이력번호 오류!");
		display_message_page_mid((char*)string_buf);
		return FALSE;
	}
	return TRUE;
}

extern INT8U keyapp_call_individual(INT16U indiv_index, INT8U beep);
void	individualData_create(INT8U mode)
{
	CAPTION_STRUCT cap;
	TRACE_INDIVIDUAL_IDENTITY indivStr;
	INT32U	addr, tempIndex;//, tempGradeNo;
	INT8U   result,old_font,key_mode;
	INT16U  strSize;
	INT16U   indivIndex;//, flag;
	INT16U   start_index=1;
	INT8U   form[10];
	INT8U   stringSet[10];
	INT8U	temp[4];
	INT32U  lotFlag;
	long	indivWeight;
	INT32U  gradeNo;
#ifdef USE_CHECKING_HARMFUL_MEAT
	INT32U  harmful;
#endif
#ifdef USE_TRACE_1000QTY //sewon 161219
	INT32U meatKind;
	INT32U harmfulness;
	//INT32U nongjangNo;
	INT32U slaughtNo;
#endif

	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	while(1)
	{      
		strSize = TRACE_INDIVIDUAL_IDENTITY_SIZE;
#ifdef USE_TRACE_1000QTY //sewon 161201
		addr = EXT_MEMCARD_ADDR ;
#else
		addr = DFLASH_BASE;
#endif
		addr+= FLASH_TRACE_STANDALONE_AREA;

		addr += ((INT32U)(start_index-1)*strSize);
	#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
		indivIndex = Flash_wread(addr);
	#else
		indivIndex = Flash_bread(addr);
	#endif
		if(indivIndex == start_index){
			Flash_sread(addr, (HUGEDATA INT8U *)&indivStr, strSize );
			indivWeight = get_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)indivStr.index-1)*4);
		}else {
			memset(&indivStr, 0, strSize );
			//start_index++;
			set_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)start_index-1)*4, 0);
			indivStr.index = start_index;		
		}
		lotFlag     = (INT32U)indivStr.lotFlag;
// 영역에 따라 고정되어 있으므로, 위해여부 설정은 의미가 없음 => 삭제
//#ifdef USE_CHECKING_HARMFUL_MEAT
//		if (indivIndex < MaxIndividualNo) harmful = 0;
//		else harmful = 1;
//		harmful     = (INT32U)indivStr.harmfulness;
//#endif
		gradeNo = indivStr.gradeNo;
#ifdef USE_TRACE_1000QTY
		meatKind = indivStr.meatKind;
		harmfulness = indivStr.harmfulness;
		slaughtNo = indivStr.slaughtNo;
#endif

		Menu_Init();
		caption_split_by_code(0x1371, &cap,0);	 //Title
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x1AF0, &cap,0);	 //index
		cap.input_max = MAX_INDIVIDUAL_NO - 1;
		cap.input_length = ilog(cap.input_max);
		caption_adjust(&cap,NULL);
	#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
		if (mode) tempIndex = (INT32U)(indivStr.index & 0x0000ffff);
		else tempIndex = (INT32U)(CurTraceStatus.indivStr.index & 0x0000ffff);
	#else
		if (mode) tempIndex = (INT32U)(indivStr.index & 0x000000ff);
		else tempIndex = (INT32U)(CurTraceStatus.indivStr.index & 0x000000ff);	//Added by JJANG 20080708
	#endif

		Menu_InputCreate(M_ID_TRS_INDEX, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&tempIndex, NULL, NULL, NULL);
		Menu_AddEndFunction(M_ID_TRS_INDEX, individualData_dataUpdate);

		caption_split_by_code(0x1AF1, &cap,0);	 //individual number
		caption_adjust(&cap,NULL);
#ifdef USE_IMPORT_LOTNO
		cap.input_length = 24; //수입 LOT는 24자리 이므로 확장 시켜줌
#endif
		Menu_InputCreate(M_ID_TRS_IND_NO, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, indivStr.individualNO, NULL, NULL);
		Menu_AddCheckFunction(M_ID_TRS_IND_NO, check_individual_no);
#ifndef USE_NHMART_SAFE_MEAT
		if (GlbOper.TraceabilityCallType == 1)	// 농협
#else
#ifdef USE_EMART_DEFAULT
		if((GlbOper.TraceabilityCallType == 4) || (GlbOper.TraceabilityCallType == 6 ))	//농협안심한우.
#else
		if(GlbOper.TraceabilityCallType == 4)	//농협안심한우.
#endif
#endif
		{
			caption_split_by_code(0x1AF8, &cap,0);	 //lot flag
			caption_adjust(&cap,NULL);
			Menu_InputCreate(M_ID_TRS_LOTFLAG, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, 0, &lotFlag, NULL,NULL,NULL);
		#ifdef USE_KOR_TRACE_999
			Menu_AddEndFunction(M_ID_TRS_LOTFLAG, individualData_LotFlagUpdate);
		#endif
		}

#ifdef USE_TRACE_MEATGRADE
  #if defined(USE_DISCLOSURE_MEATGRADE) || defined(USE_TRACE_1000QTY) // 개체 및 PLU 등급 동시 사용, 개체가 우선순위를 가짐.
	   caption_split_by_code(0x1AF2, &cap,0);	//grade No
	   caption_adjust(&cap,NULL);
	   Menu_InputCreate(M_ID_TRS_GRADE_INDEX, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						   cap.input_max, cap.input_min, 0, &gradeNo, NULL, NULL, trace_meatgrade_call_back);
  #else
	   if (plu_check_inhibit_ptype(PTYPE_GROUP_NO))    // PLU 등급을 사용하지 않을 때
	   {
		   caption_split_by_code(0x1AF2, &cap,0);	//grade No
		   caption_adjust(&cap,NULL);
		   Menu_InputCreate(M_ID_TRS_GRADE_INDEX, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							   cap.input_max, cap.input_min, 0, &gradeNo, NULL, NULL, trace_meatgrade_call_back);
	   }
  #endif
#else // USE_TRACE_MEATGRADE
		caption_split_by_code(0x1AF2, &cap,0);	 //grade name
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_GRADE, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, indivStr.grade, NULL, NULL);
#endif //USE_TRACE_MEATGRADE

		caption_split_by_code(0x1AF3, &cap,0);	 //slaught date
		caption_adjust(&cap,NULL);

		sprintf((char*)form, "99.99.99");
		sprintf((char*)stringSet, "%02d.%02d.%02d", indivStr.slaughtDate[0],indivStr.slaughtDate[1],indivStr.slaughtDate[2]);
		Menu_InputCreate(M_ID_TRS_SL_DATE, (HUGEDATA char *)cap.form, MENU_TYPE_IP, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, stringSet, form, NULL);


		caption_split_by_code(0x1AF4, &cap,0);	 //weight
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_WEIGHT, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&indivWeight, NULL, NULL, NULL);


		caption_split_by_code(0x1AF5, &cap,0);	 //breed
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_BEED, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, indivStr.breed, NULL, NULL);

		caption_split_by_code(0x1AF6, &cap,0);	 //farm
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_FARM, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, indivStr.farm, NULL, NULL);

		caption_split_by_code(0x1AF7, &cap,0);	 //slaughter house
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_SLAUGHT, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, indivStr.slaughtHouse, NULL, NULL);

// 영역에 따라 고정되어 있으므로, 위해여부 설정은 의미가 없음 => 삭제
//	#ifdef USE_CHECKING_HARMFUL_MEAT
//		caption_split_by_code(0x1AFC, &cap,0);	 //harmfulness
//		//sprintf(cap.form, "위해");
//		//cap.input_length = 1;
//		//cap.input_dlength = 1;
//		caption_adjust(&cap,NULL);
//		Menu_InputCreate(M_ID_TRS_HARMFUL, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
//							cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&harmful, NULL, NULL, NULL);
//	#endif   

#ifdef USE_TRACE_1000QTY //sewon 161219
		caption_split_by_code(0x1AFD, &cap,0);	 //meatkind 육종 
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_MEATKIND, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &meatKind, NULL,NULL,breed_call_back);

		caption_split_by_code(0x1AFE, &cap,0);	 //harmflag 위해개체 여부 
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_HARMFUL, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &harmfulness, NULL,NULL,NULL);
		
		caption_split_by_code(0x1AFF, &cap,0);	 //농장번호
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_NONGJANGNO, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, indivStr.nongjangNo, NULL, NULL);
		
		caption_split_by_code(0x1AF9, &cap,0); //도축장번호 
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_SLAUGHTNO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &slaughtNo, NULL,NULL,slaughHouseNo_call_back);
		
#endif

	#ifdef USE_KOR_TRACE_999
		caption_split_by_code(0x1AF6, &cap,0);	 //lot part
		strcpy(cap.form, "묶음부위:");
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_LOTPART, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, indivStr.farm, NULL, NULL);
	#endif

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
		#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
			indivStr.index = tempIndex&0xffff;
		#else
			indivStr.index = tempIndex&0xff;
		#endif
			if(indivStr.index>0 && indivStr.index <MAX_INDIVIDUAL_NO) 
			{
				indivStr.lotFlag = (INT8U)(lotFlag&0xff);
			#ifdef USE_KOR_TRACE_999
				if (indivStr.lotFlag)
				{
				}
				else
				{
					memcpy(temp,&stringSet[0],2);
					temp[2]=0;
					indivStr.slaughtDate[0] =(INT8U)atoi((char *)temp);
					memcpy(temp,&stringSet[3],2);
					temp[2]=0;
					indivStr.slaughtDate[1] =(INT8U)atoi((char *)temp);
					memcpy(temp,&stringSet[6],2);
					temp[2]=0;
					indivStr.slaughtDate[2] =(INT8U)atoi((char *)temp);
#ifdef USE_CHECKING_HARMFUL_MEAT
					if (indivStr.index < MaxIndividualNo) harmful = 0;	// 영역에 따라 고정
					else harmful = 1;
					indivStr.harmfulness = (INT8U)harmful;
#endif
				}
			#else
				memcpy(temp,&stringSet[0],2);
				temp[2]=0;
				indivStr.slaughtDate[0] =(INT8U)atoi((char *)temp);
				memcpy(temp,&stringSet[3],2);
				temp[2]=0;
				indivStr.slaughtDate[1] =(INT8U)atoi((char *)temp);
				memcpy(temp,&stringSet[6],2);
				temp[2]=0;
				indivStr.slaughtDate[2] =(INT8U)atoi((char *)temp);
#ifdef USE_CHECKING_HARMFUL_MEAT
				if (indivStr.index < MaxIndividualNo) harmful = 0;	// 영역에 따라 고정
				else harmful = 1;
				indivStr.harmfulness = (INT8U)harmful;
#endif                
			#endif
				indivStr.gradeNo = (INT8U)gradeNo;
				// indivStr.grade 에 카피하는 기능 추가

#ifdef USE_TRACE_1000QTY //sewon 161201
				indivStr.meatKind = (INT8U)meatKind;
				indivStr.harmfulness = (INT8U)harmfulness;
				//indivStr.nongjangNo = (INT8U)nongjangNo;
				indivStr.slaughtNo = (INT8U)slaughtNo;
				
				addr = EXT_MEMCARD_ADDR ;
#else
				addr = DFLASH_BASE;
#endif
				addr+= FLASH_TRACE_STANDALONE_AREA;
				addr += (INT32U)(indivStr.index -1) * strSize;
				Flash_swrite(addr, (HUGEDATA INT8U *)&indivStr, strSize);
				set_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)indivStr.index-1)*4, indivWeight);
			}
			if (!mode) //Added by JJANG 20080626
			{
				keyapp_call_individual(indivStr.index, OFF); //새로운 정보를 화면에 업데이트 하기 위해.
				break;
			}

			start_index = indivStr.index;
			if(indivStr.index < MAX_INDIVIDUAL_NO - 1) start_index++;
			else  start_index = MAX_INDIVIDUAL_NO - 1; 

		}
		else if(result ==MENU_RET_ESC)
		{
			break;
		}
	}

	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
		
}

void	individualData_call(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	result, old_font, key_mode;
	INT32U	indivIndex = 1;   
	INT8U string_buf[50];   

#ifdef USE_TOPMART_STRUCTURE //sewon 161129	
	if (!plu_check_inhibit_ptype(PTYPE_TR_COMP_FLAG))
#else
	if (!plu_check_inhibit_ptype(PTYPE_TRACE_FLAG))
#endif
	{
		if (status_scale.cur_pluid)
		{
#ifdef USE_TRACE_FLAG_0_CHECK
			if (!status_scale.Plu.trace_flag)
			{
				BuzOn(2);		
				//sprintf((char*)string_buf, "개체이력대상이 아닙니다");
				sprintf((char*)string_buf, IndivMsg_NonTrace);
				display_message_page_mid((char *)string_buf);
				return;
			}
#endif
		}
		else 
		{
			BuzOn(2);
			return;
		}
	}

	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	while(1)
	{      
		//display_title_page_change(0x1371, 1, 1);
		Menu_Init();

		caption_split_by_code(0x1371, &cap,0);	 //Title
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x1AF0, &cap,0);	 //index
		cap.input_max = MAX_INDIVIDUAL_NO - 1;
		cap.input_length = ilog(cap.input_max);        
		caption_adjust(&cap,NULL);

		Menu_InputCreate(M_ID_TRS_INDEX, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&indivIndex, NULL, NULL, individualNo_call_back);
		Menu_SetCallbackMode(M_ID_TRS_INDEX, 1);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			keyapp_call_individual(indivIndex, OFF); //새로운 정보를 화면에 업데이트 하기 위해.
#ifdef USE_TOPMART_STRUCTURE //sewon 170105
			if (FlagIndividualMultiStart)//복수개체 입력
			{
				if (MultiIndex < MAX_INDIVIAL_MULTI_DATA_CNT)
				{
					memcpy(IndivdualMultiData[MultiIndex].indivNoStr, CurTraceStatus.indivStr.individualNO, INDIVIDUAL_NO_LEN);
					IndivdualMultiData[MultiIndex].slaughterHouse = CurTraceStatus.indivStr.slaughtNo;
			
					MultiIndex++;
				}
				else
				{
					BuzOn(2);
				}
			}
			else
			{
				FlagIndividualMultiRemain = OFF;
				MultiIndex = 0;
				memset(IndivdualMultiData, 0, sizeof(TRACE_INDIVIDUAL_MULTI_DATA) * MAX_INDIVIAL_MULTI_DATA_CNT);
			}
#endif
			break;
		} else if(result ==MENU_RET_ESC) break;
	}

	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff); 	
}

void	slaughHouseNo_call(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	result, old_font, key_mode;
	INT32U	index = 1;   
	INT8U i;   

	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	while(1)
	{      
		//display_title_page_change(0x1371, 1, 1);
		Menu_Init();

		caption_split_by_code(0x1391, &cap,0);	 //Title
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x139A, &cap,0);	 //도축장 #:
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
		cap.input_length = 4;
		cap.input_dlength = 4;
		cap.input_max = 9999;
		cap.input_min = 0;	// 0: clear
#endif
		caption_adjust(&cap,NULL);

		Menu_InputCreate(M_ID_TRS_INDEX, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&index, NULL, NULL, slaughHouseNo_call_back);
		Menu_SetCallbackMode(M_ID_TRS_INDEX, 1);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
			CurTraceStatus.slPlace = (INT8U)trace_slaughthouse_no2index(index);	// slaughter 4digit -> memory index
#else
			CurTraceStatus.slPlace = (INT8U)index;
#endif
			if (FlagIndividualMultiStart || FlagIndividualMultiRemain)
			{
				for (i = 0; i < MultiIndex; i++)
				{
					IndivdualMultiData[i].slaughterHouse = CurTraceStatus.slPlace;
				}
			}
			sale_display_indivFlag_set(INDIV_DISP_MODE_SLAUGHTHOUSE, ON);
			sale_display_individual();
		#ifdef USE_MULTI_TRACE_NO_TABLE
			TNT_SetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, NULL, NULL, NULL, CurTraceStatus.slPlace, CurTraceStatus.slPlace, CurTraceStatus.slPlace, 0xff, 0xff, 0xff, 0xff);
		#else
			TNT_SetTraceNoTable_Single((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart, CurTraceStatus.indivStr.lotFlag, CurTraceStatus.gradeNo);
		#endif
			break;
		} else if(result ==MENU_RET_ESC) break;
	}

	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff); 	
}

void	meatUseNo_call(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	result, old_font, key_mode;
	INT32U	index = 1;   

	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	while(1)
	{      
		//display_title_page_change(0x1371, 1, 1);
		Menu_Init();

		caption_split_by_code(0x1381, &cap,0);	 //Title
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x138A, &cap,0);	 //용도 #:
		cap.input_min = 0;	// 0입력 가능(취소 기능)        
		caption_adjust(&cap,NULL);

		Menu_InputCreate(M_ID_TRS_INDEX, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&index, NULL, NULL, meatUseNo_call_back);
		Menu_SetCallbackMode(M_ID_TRS_INDEX, 1);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			CurTraceStatus.meatUse = (INT8U)index;
			sale_display_indivFlag_set(INDIV_DISP_MODE_SPECIALUSE, ON);
			sale_display_individual();
		#ifdef USE_MULTI_TRACE_NO_TABLE
			TNT_SetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, NULL, NULL, NULL, 0xff, 0xff, 0xff, CurTraceStatus.meatUse, 0xff, 0xff, 0xff);
		#else
			TNT_SetTraceNoTable_Single((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart, CurTraceStatus.indivStr.lotFlag, CurTraceStatus.gradeNo);
		#endif
			break;
		} else if(result ==MENU_RET_ESC) break;
	}

	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff); 	
}

#ifdef USE_TRACE_MEATPART
void	meatPartNo_call(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	result, old_font, key_mode;
	INT32U	index = 1;   

	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	while(1)
	{      
		Menu_Init();

		caption_split_by_code(0x13A1, &cap,0);	 //Title
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x13AA, &cap,0);	 //부위 #:
		cap.input_min = 0;	// 0입력 가능(취소 기능)
		caption_adjust(&cap,NULL);

		Menu_InputCreate(M_ID_TRS_INDEX, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&index, NULL, NULL, meatPartNo_call_back);
		Menu_SetCallbackMode(M_ID_TRS_INDEX, 1);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			CurTraceStatus.meatPart = (INT8U)index;
			sale_display_indivFlag_set(INDIV_DISP_MODE_MEATPART, ON);
			sale_display_individual();
		#ifdef USE_MULTI_TRACE_NO_TABLE
			TNT_SetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, NULL, NULL, NULL, 0xff, 0xff, 0xff, 0xff, CurTraceStatus.meatPart, 0xff, 0xff);
		#else
			TNT_SetTraceNoTable_Single((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart, CurTraceStatus.indivStr.lotFlag, CurTraceStatus.gradeNo);
		#endif
			break;
		} else if(result ==MENU_RET_ESC) break;
	}

	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff); 	
}
#endif

INT8U cksum_indivNo(char *buf)
{
	INT8U i, digit[8+1];
	INT8U str[12];
	INT8U code;

	INT16U v;
	//INT32U k;
	INT16U a =  0;
	INT16U c =  0;
	INT16U d =  0;
	INT16U e = 10;
	INT16U chkNum = 10;
	INT16U sum = 0;

	if (strlen(buf) != 12) 	return TRUE;//return FALSE;

	for (i = 0; i < 12; i++)
	{
		str[i] = *(buf+i) - '0';
	}
//#ifdef USE_EMART_PIG_TRACE //독립형 20141224
	if(str[0] == 1) return TRUE; //1이면 돼지이력번호 이므로 체크 안함.
//#endif 
	code = str[0]*100 + str[1]*10 + str[2];
//for(i=0; i<12; i++)
//{
//sprintf(MsgBuf, "[%d:%02x]", i, str[i]);
//MsgOut(MsgBuf);
//}
	if(code == 0)
	{
		v = str[3]*1000 + str[4]*100 + str[5]*10 + str[6];
		//k = str[7]*10000 + str[8]*1000 + str[9]*100 + str[10]*10 + str[11];

		if ( (v > 1636)) 
		{
			//sprintf(digit, "%d%d%d%d%d%d%d%d", 1, 3, 1, 3, 1, 3, 1, 3);
			digit[0] = 1;
	                digit[1] = 3;
	                digit[2] = 1;
	                digit[3] = 3;
	                digit[4] = 1;
	                digit[5] = 3;
	                digit[6] = 1;
	                digit[7] = 3;
		}
		else 
		{
			//sprintf(digit, "%d%d%d%d%d%d%d%d", 1, 2, 1, 2, 1, 2, 1, 2);
			digit[0] = 1;
	                digit[1] = 2;
	                digit[2] = 1;
	                digit[3] = 2;
	                digit[4] = 1;
	                digit[5] = 2;
	                digit[6] = 1;
	                digit[7] = 2;
		}
		sum = 0;

		for(i = 0; i < 8; i++)
		{
			sum += digit[i] * str[i+3];
		}
		chkNum = 10;
		if (sum > 0) chkNum = sum % 10;
		if (chkNum == 0) chkNum = 10;

//sprintf(MsgBuf, "(000)%d,%d(v=%d)checkNo=%d,str[11]=%d\r\n", digit[0], digit[1], v, chkNum, str[11]);
//MsgOut(MsgBuf);

		if (10 - chkNum == str[11]) return TRUE;
		else return FALSE;
	}
	else if (code >= 990)	// 코드가 99로 시작. Lot(묶음번호)일 때, 유효성 체크 안함
	{
		return TRUE;
	}
	else //코드가 000이 아닐때
	{
		for(i = 0; i < 11; i++)
		{
			a = e + str[i];			//step 1
			if (a > 10) c = a - 10; 	//step 2
			else c = a;
			d = c * 2; 			//step 3
			if (d >= 11) e = d - 11;	//step 4
			else e = d;
		}
		chkNum = 0;
		chkNum = 11 - e;

		if (chkNum == 10) chkNum = 0;

//sprintf(MsgBuf, "(00x)checkNo=%d,str[11]=%d\r\n",chkNum, str[11]);
//MsgOut(MsgBuf);

		if (chkNum == str[11]) return TRUE;
		else return FALSE;
	}	
}

extern SCANNER_DATA_STR  Scanner;
extern void PS2_Write(INT16U code);
extern INT8U	ScanFlag;
void	individualNo_input(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	indivNoStr[40];//21->31
	INT8U   result,old_font,key_mode;
	INT16U  strSize;
	INT16U	indivIndex;
	INT8U   i;
	INT8U string_buf[50];
	INT8U lotFlag;

#ifdef USE_EMART_PIG_TRACE
	ImportLotNumberflag = 0;
#endif
  
#ifdef USE_TOPMART_STRUCTURE //sewon 161129
	if (!plu_check_inhibit_ptype(PTYPE_TR_COMP_FLAG))
#else
	if (!plu_check_inhibit_ptype(PTYPE_TRACE_FLAG))
#endif
	{
		if (status_scale.cur_pluid)
		{
#ifdef USE_TRACE_FLAG_0_CHECK
			if (!status_scale.Plu.trace_flag)
			{
				BuzOn(2);		
				//sprintf((char*)string_buf, "개체이력대상이 아닙니다");
				sprintf((char*)string_buf, IndivMsg_NonTrace);
				display_message_page_mid((char *)string_buf);
				return;
			}
#endif
		}
		else 
		{
			BuzOn(2);
			return;
		}
	}

  	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	memset(indivNoStr, 0, sizeof(indivNoStr));//개체번호 입력키 저장 : 기존 바코드 스캔 저장변수 사용 


	while(1)
	{      
		display_title_page_change(0x1371, 1, 1);

		Menu_Init();
		caption_split_by_code(0x1371, &cap,0);	 //Title
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x1AF1, &cap,0);	 //individual number
		if (ModeIndiv == 3) 
		{
			cap.input_length = 32;
			//cap.input_dlength = 32;
		}
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TRS_IND_NO, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, indivNoStr, NULL, NULL);

		result = Menu_InputDisplay();

		if(result == MENU_RET_SAVE) 
		{
			lotFlag = checkIndividualNo(indivNoStr);
			if (cksum_indivNo((char *)indivNoStr) == FALSE && UseCheckIndivNo && !lotFlag) 
			{
				BuzOn(2);
				sprintf((char*)string_buf, "이력번호 오류!");
				display_message_page_mid((char*)string_buf);
				continue;
			}
			if (ModeIndiv == 3)
			{
				if (indivNoStr[0] == NULL) 
				{
					memcpy((char *)CalledTraceStatus.indivStr.individualNO, indivNoStr, sizeof(CalledTraceStatus.indivStr.individualNO));
					//CalledTraceStatus.indivStr.lotFlag = checkIndividualNo(CalledTraceStatus.indivStr.individualNO);
					CalledTraceStatus.indivStr.lotFlag = lotFlag;
#ifdef USE_TOPMART_DEFAULT
					if(strncmp((char *)CalledTraceStatus.indivStr.individualNO, "777777777777", 12))
					{
					indivIndex = individualData_search(CalledTraceStatus.indivStr.individualNO, 0);
					keyapp_call_individual(indivIndex, OFF); //새로운 정보를 화면에 업데이트 하기 위해.
					}// 이력번호가 777777777777 일 경우 체크 안 함.
#else
					indivIndex = individualData_search(CalledTraceStatus.indivStr.individualNO, 0);
					keyapp_call_individual(indivIndex, OFF); //새로운 정보를 화면에 업데이트 하기 위해.
#endif
				#ifdef USE_MULTI_TRACE_NO_TABLE
					TNT_SetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, NULL, NULL, 0xff, 0xff, 0xff, 0xff, 0xff, CurTraceStatus.indivStr.lotFlag, 0xff);
				#else
					TNT_SetTraceNoTable_Single((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart, CurTraceStatus.indivStr.lotFlag, CurTraceStatus.gradeNo);
				#endif
					break; 
				}

				strSize = strlen((char *)indivNoStr);	//Write Barcode
				for(i = 0; i<strSize ;i++)
				{
					PS2_Write(indivNoStr[i]-0x30+KEY_NUM_0);
				}
				PS2_Write(KS_SCANPRINT);
				ScanFlag = ON;
				break;
			}
			else
			{			 
				memset(&CalledTraceStatus, 0, sizeof(CalledTraceStatus));
				memcpy((char *)CalledTraceStatus.indivStr.individualNO, indivNoStr, sizeof(CalledTraceStatus.indivStr.individualNO));
				CalledTraceStatus.indivStr.individualNO[INDIVIDUAL_NO_LEN] = 0;
				individual_checkZero(CalledTraceStatus.indivStr.individualNO);
				//CalledTraceStatus.indivStr.lotFlag = checkIndividualNo(CalledTraceStatus.indivStr.individualNO);
				CalledTraceStatus.indivStr.lotFlag = lotFlag;
				indivIndex = individualData_search(CalledTraceStatus.indivStr.individualNO, 0);
				keyapp_call_individual(indivIndex, OFF); //새로운 정보를 화면에 업데이트 하기 위해.
			#ifdef USE_MULTI_TRACE_NO_TABLE
				TNT_SetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, NULL, NULL, 0xff, 0xff, 0xff, 0xff, 0xff, CurTraceStatus.indivStr.lotFlag, 0xff);
			#else
				TNT_SetTraceNoTable_Single((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart, CurTraceStatus.indivStr.lotFlag, CurTraceStatus.gradeNo);
			#endif
				FlagIndividualMultiRemain = OFF;
				MultiIndex = 0;
				memset(IndivdualMultiData, 0, sizeof(TRACE_INDIVIDUAL_MULTI_DATA) * MAX_INDIVIAL_MULTI_DATA_CNT);
				break;
			}
		} else if(result ==MENU_RET_ESC) break;
	}
	if (ModeIndiv == 3) 
	{
		Dsp_ChangeMode(DSP_SALE_MODE);
		KEY_SetMode(0);
		sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	}
	else 
	{
		Dsp_Fill_Buffer(0);	// LCD Buffer clear
		DspLoadFont1(old_font);
		Dsp_Diffuse();
		KEY_SetMode(key_mode);
		sale_display_update(0x0fff);
	}	
}
#ifdef USE_TOPMART_IMPORTLOTNO
#define LOT_DATA_LEN	24
#define LOT_DATA_FULL_LEN	34
#else
  #if defined (USE_IMPORT_LOTNO) && defined (USE_MANUAL_INPUT_IMPORT_LOTNO)
#define LOT_DATA_LEN	30
  #else
#define LOT_DATA_LEN	20
  #endif
#define LOT_DATA_FULL_LEN	30
#endif
INT8U	lotNo_input(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	lotNoStr[LOT_DATA_LEN];
	INT8U   result,old_font,key_mode;
	INT8U	lotRefStr[20];
	INT8U	lotFullStr[LOT_DATA_FULL_LEN];
	INT8U	lotInputFlag = ON;
	INT16U	lotIndex = 1;
	INT8U string_buf[50];

#ifdef USE_EMART_PIG_TRACE
	ImportLotNumberflag = 0;
#endif
  
#ifdef USE_TOPMART_STRUCTURE //sewon 161129
	if (!plu_check_inhibit_ptype(PTYPE_TR_COMP_FLAG))
#else
	if (!plu_check_inhibit_ptype(PTYPE_TRACE_FLAG))
#endif
	{
		if (status_scale.cur_pluid)
		{
#ifdef USE_TRACE_FLAG_0_CHECK
			if (!status_scale.Plu.trace_flag)
			{
				BuzOn(2);		
				//sprintf((char*)string_buf, "개체이력대상이 아닙니다");
				sprintf((char*)string_buf, IndivMsg_NonTrace);
				display_message_page_mid((char *)string_buf);
				return MENU_RET_ESC;
			}
#endif
		}
		else 
		{
			BuzOn(2);
			return MENU_RET_ESC;
		}
	}

	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	memset(lotFullStr, 0, LOT_DATA_FULL_LEN);//개체번호 입력키 저장 : 기존 바코드 스캔 저장변수 사용 
	memset(lotNoStr, 0, LOT_DATA_LEN);
	memset(lotRefStr, 0, 10);	 

	RTC_CLK_Burst_Read();
	while(1)
	{      
		//display_title_page_change(0x1371, 1, 1);

		Menu_Init();

		caption_split_by_code(0x1371, &cap,0);	 //Title
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x1AFB, &cap,0);	 //묶음번호 입력

		if (lotInputFlag)
		{			
			lotRefStr[0] = 0;                
			switch(ModeIndiv) {
				default:
				case 0://not used
					break;
				case 1://신세계
				case 4://standard (신세계 + 개체인덱스)
				//case 5://이마트
					//sprintf((char *)lotRefStr, "LOT%02x%02x%02x", RTCStruct.year, RTCStruct.month, RTCStruct.date);  //독립형 20141224
					//strcat(cap.form, (char *)lotRefStr);
					//cap.input_length = 3;
					sprintf((char *)lotRefStr, "L");
#if defined (USE_IMPORT_LOTNO) && defined (USE_MANUAL_INPUT_IMPORT_LOTNO)
					cap.input_length = 23;
#else
					cap.input_length = 14;
#endif
					break;
				case 2://농협
					//sprintf((char *)lotRefStr, "LOT%02x%02x%02x", RTCStruct.year, RTCStruct.month, RTCStruct.date);
					//strcat(cap.form, (char *)lotRefStr);
					//cap.input_length = 9;
					sprintf((char *)lotRefStr, "L"); //hanaro 20150116
					cap.input_length = 14;
					break;
				case 3://GS마트
					//sprintf((char *)lotRefStr, "LOT20%02x%02x%02x", RTCStruct.year, RTCStruct.month, RTCStruct.date); //gspig 20141212
					//strcat(cap.form, (char *)lotRefStr);
					//cap.input_length = 2; //gspig 20141212
#ifdef USE_TOPMART_IMPORTLOTNO
					sprintf((char *)lotRefStr, "L");
					cap.input_length = 23;
#else
					sprintf((char *)lotRefStr, "L");
					cap.input_length = 14;
#endif
					break;
				case 5://이마트 
#ifdef USE_EMART_PIG_TRACE
					sprintf((char *)lotRefStr, "L");
					cap.input_length = 14;
#else
					sprintf((char *)lotRefStr, "LOT");
					cap.input_length = 9;
#endif
					break;
			}
			strcat(cap.form, (char *)lotRefStr);            
			caption_adjust(&cap,NULL);
			Menu_InputCreate(M_ID_TRS_IND_NO, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, lotNoStr, NULL, NULL);
			Menu_AddKeyFunction(M_ID_TRS_IND_NO, KP_TEST, NULL);
			Menu_SetProperties(M_ID_TRS_IND_NO, MENU_P_EXIT_MENU);
		} else {
			cap.input_length = 20; //for 선하증권 or 묶음번호
			caption_adjust(&cap,NULL);
			Menu_InputCreate(M_ID_TRS_IND_NO, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, lotFullStr, NULL, NULL);
		}

		result = Menu_InputDisplay();
#if (defined (USE_IMPORT_LOTNO) && defined (USE_MANUAL_INPUT_IMPORT_LOTNO)) || (defined(USE_TOPMART_IMPORTLOTNO))
		if(strlen((char *)lotNoStr) == 23) sprintf((char *)lotRefStr, "A");
#endif
		if (lotInputFlag && lotNoStr[0] != 0) sprintf((char *)lotFullStr, "%s%s", lotRefStr, lotNoStr);

		if (result == MENU_RET_SAVE) 
		{
#ifdef USE_TOPMART_ASK_INDIV_INFO //sewon 170214
			lotIndex = individualData_search(lotFullStr, 0);

			if(lotIndex == MAX_INDIVIDUAL_NO)
			{
				if (!keyapp_ask_indivInfo(lotFullStr)) break;
			}
#endif
			memset(&CalledTraceStatus, 0, sizeof(CalledTraceStatus));
			memcpy(CalledTraceStatus.indivStr.individualNO, lotFullStr, sizeof(CalledTraceStatus.indivStr.individualNO));
			CalledTraceStatus.indivStr.lotFlag = 1;	// 묶음번호
			//lotIndex = 100; //lot번호 입력은 barcode scanner와 같이 index=100
			lotIndex = individualData_search(CalledTraceStatus.indivStr.individualNO, 0);
			keyapp_call_individual(lotIndex, OFF); //새로운 정보를 화면에 업데이트 하기 위해.
		#ifdef USE_MULTI_TRACE_NO_TABLE
			TNT_SetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, NULL, NULL, 0xff, 0xff, 0xff, 0xff, 0xff, CurTraceStatus.indivStr.lotFlag, 0xff);
		#else
			TNT_SetTraceNoTable_Single((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart, CurTraceStatus.indivStr.lotFlag, CurTraceStatus.gradeNo);
		#endif
			FlagIndividualMultiRemain = OFF;
			MultiIndex = 0;
			memset(IndivdualMultiData, 0, sizeof(TRACE_INDIVIDUAL_MULTI_DATA) * MAX_INDIVIAL_MULTI_DATA_CNT);
			break;
		} 
		else if (result == MENU_RET_EXIT) lotInputFlag = OFF;
		else if (result ==MENU_RET_ESC) break; 		
	}

	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);

	return result;
	
}
//#define MAX_INDIV_BIND_SIZE	10
//TRACE_BIND_INDIVIDUAL_DATA 	bindIndivStr[MAX_INDIV_BIND_SIZE];
void	individualNo_bind(void)
{
//	CAPTION_STRUCT 	cap;
//	INT8U   result;
//	INT8U	old_font;
//	INT8U	key_mode;
//	INT8U 	i;
//	INT8U	bindStr[2];
//
//	key_mode = KEY_GetMode();
//	KEY_SetMode(1);
//	old_font = DspStruct.Id1;
//	DspLoadFont1(DSP_MENU_FONT_ID);
//
//	memset(bindIndivStr, 0, sizeof(TRACE_BIND_INDIVIDUAL_DATA)*MAX_INDIV_BIND_SIZE);
//	memset(bindStr, 0, 2);
//
//	while(1)
//	{      
//		display_title_page_change(0x1371, 1, 1);
//
//		Menu_Init();
//
//		caption_split_by_code(0x1371, &cap,0);	 //Title
//		Menu_HeadCreate((HUGEDATA char *)cap.form);
//
//		for(i=0; i<MAX_INDIV_BIND_SIZE; i++) //상품입력
//		{
//			caption_split_by_code(0x146D, &cap, 0);	
//			sprintf(&bindStr[i], "%d", i+1);
//			strcat(cap.form, &bindStr[i]);
//			caption_adjust(&cap ,NULL);
//			Menu_InputCreate(M_ID_TRS_BIND_IND_NO+i, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
//							cap.input_max, cap.input_min, 0, NULL, bindIndivStr[i].barStr, NULL, NULL);
//		}
//
//		result = Menu_InputDisplay();
//		
//		if (result == MENU_RET_SAVE) {
//			//묶음장부 인쇄위치
//			break;
//		} else if (result ==MENU_RET_ESC) break; 		
//	}
//
//	Dsp_Fill_Buffer(0);						// LCD Buffer clear
//	DspLoadFont1(old_font);
//	Dsp_Diffuse();
//	KEY_SetMode(key_mode);
//	sale_display_update(0x0fff); 	
}

extern void display_message_page(char *str);
TRACE_INDIVIDUAL_MULTI_DATA 	IndivdualMultiData[MAX_INDIVIAL_MULTI_DATA_CNT];
TRACE_LOT_MULTI_DATA		LotMultiData[MAX_LOT_MULTI_DATA_CNT];
INT8U LotMultiDataNo[INDIVIDUAL_NO_LEN + 1];
INT16U 	MultiIndex = 0;
INT8U	LotListMultiIndex = 0;
INT8U 	FlagMultiStart = 0;
INT8U 	FlagIndividualMultiStart = 0;
INT8U 	FlagIndividualMultiRemain = 0;
INT8U 	FlagLotMultiStart = 0;
INT8U UseLotNum = 0;		//묶음번호 입력방법 사용시 ON 됨.
void indivdualMultiStart(void)
{
	INT8U buf[30];

#ifdef USE_EMART_NETWORK
//sprintf(MsgBuf, "[1]mi=%d, ms=%d, mr=%d\r\n", MultiIndex, FlagIndividualMultiStart, FlagIndividualMultiRemain);
//MsgOut(MsgBuf);	// CJK090605
	if (MultiIndex >= MAX_INDIVIAL_MULTI_DATA_CNT || !CurTraceStatus.indivStr.individualNO[0])
	{
		BuzOn(2);
		return;
	}
	if (MultiIndex == 0)
	{
		memset(IndivdualMultiData, 0, sizeof(TRACE_INDIVIDUAL_MULTI_DATA) * MAX_INDIVIAL_MULTI_DATA_CNT);
		memcpy(IndivdualMultiData[MultiIndex].indivNoStr, CurTraceStatus.indivStr.individualNO, INDIVIDUAL_NO_LEN);
		IndivdualMultiData[MultiIndex].slaughterHouse = CurTraceStatus.slPlace;
		MultiIndex++;
		
	}
	FlagIndividualMultiStart = ON;
	FlagIndividualMultiRemain = ON;
	sprintf((char *)buf, "복수개체 %d", MultiIndex + 1);
	display_message_page_mid((char *)buf);

#else
	FlagMultiStart %=2;
	switch(FlagMultiStart) {
		default: break;
		case 0: 
			FlagIndividualMultiStart = ON;
			FlagLotMultiStart = OFF;	
			sprintf((char *)buf, "복수개체 시작");
			memset(IndivdualMultiData, 0, sizeof(TRACE_INDIVIDUAL_MULTI_DATA) * MAX_INDIVIAL_MULTI_DATA_CNT);
			break;
		case 1:
			FlagIndividualMultiStart = OFF;
			FlagLotMultiStart = ON;
			UseLotNum = OFF;
			sprintf((char *)buf, "묶음장부 시작");
			memset(LotMultiData, 0, sizeof(TRACE_LOT_MULTI_DATA) * MAX_LOT_MULTI_DATA_CNT);
			break;
	}
	FlagMultiStart++;

	MultiIndex = 0;
	LotListMultiIndex = 0;
	FlagIndividualMultiRemain = 0;

	display_message_page_mid((char *)buf);
#endif
}


void indivdualLotListStart(void)
{
	INT8U buf[30];
	INT8U result;

	result = lotNo_input();
	if (result == MENU_RET_SAVE)
	{
		FlagIndividualMultiStart = OFF;
		FlagLotMultiStart = ON;
		sprintf((char *)buf, "묶음장부 시작");
		memset(LotMultiData, 0, sizeof(TRACE_LOT_MULTI_DATA) * MAX_LOT_MULTI_DATA_CNT);
		memcpy(LotMultiDataNo, CurTraceStatus.indivStr.individualNO, sizeof(CurTraceStatus.indivStr.individualNO));
		UseLotNum = ON;
		MultiIndex = 0;
		LotListMultiIndex = 0;
		FlagIndividualMultiRemain = 0;

		display_message_page_mid((char *)buf);
	}
}

#ifndef DISABLE_LIST_MENU
void	individualData_list(void)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	TRACE_INDIVIDUAL_IDENTITY indivStr;
	
	char   	string_buf[34], end_flag = 0;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, reverse_num;
	INT16S	j = 0, result, min_num, max_num, lcd_size;
	INT16U	read_code, line_ct, start_id;
	INT32U	addr, addr_flash;	
	INT16U	code = 0x1372;
	INT16U  len;

LIST_START:
	caption_split_by_code(0x1AF0, &cap, 0);  //individual index
//	cap.input_max = MAX_INDIVIDUAL_NO;
	cap.input_max = MAX_INDIVIDUAL_NO - 1;
	cap.input_length = ilog(cap.input_max);    
	min_num	= cap.input_min;					//
	max_num = cap.input_max; 					// 
	if(min_num <= 0)  {
		disp_p = point(0,0);
		message_error(disp_p);
	}

	Dsp_Fill_Buffer(0);
	display_title_page_change(code,1,1);

	for(i = 0; i<Startup.line_max; i++)  {
		caption_split_by_code(0x1E00+i, &cap,0);
		if(i != 2) caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	}
	Dsp_Diffuse();

	start_num = min_num;
	end_num = max_num;
	for(i=0; i<2; i++)  {
REN1:		if(i == 0)	ret_long = start_num;		
		if(i == 1)	ret_long = end_num;
		caption_split_by_code(0x1E00+i, &cap,0);
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		result=keyin_ulong(KI_GRAPHIC, 
				   KI_FORM_NORMAL,	
				   0,
				   (long*)&ret_long, 
				   4, 
				   (i+Startup.menu_type)*Startup.menu_ygap,  
				   cap.input_x*Startup.menu_xgap, 
				   0,
				   KI_NOPASS );
				   				
		switch (result){
			case KP_ARROW_UP:  
					   if(i > 0)  	i--;
					   else		BuzOn(2);
					   goto REN1;
			case KP_ESC:       goto RET_ESC;
			case KP_SAVE:	   start_num = min_num;
					   end_num = max_num;
					   i++;
					   break;
			case KP_PAGE_DN :
			case KP_ARROW_DN:
			case KP_ENTER   :
					   if ( ret_long > max_num || ret_long < min_num ) {
						error_display_form1(0, min_num, max_num);
						goto REN1;	
				           }
					   if(i == 0)	start_num = ret_long;
					   if(i == 1)	end_num = ret_long;
					   break;
					   				  				
		     	default:	BuzOn(2);	goto REN1;		     	 	
		} //switch (result)
	}//for(i=1; i<3; i++)

	lcd_size   = Dsp_GetChars();
	line_ct    = Startup.line_max; 


//	addr = DFLASH_BASE;
//	addr+= FLASH_TRACE_STANDALONE_AREA;
//	Flash_sread(addr, (HUGEDATA INT8U *)&indivStr, strSize );

#ifdef USE_TRACE_1000QTY //sewon 161206
	addr_flash = EXT_MEMCARD_ADDR + FLASH_TRACE_STANDALONE_AREA;
#else
	addr_flash = DFLASH_BASE + FLASH_TRACE_STANDALONE_AREA;
#endif
	cur_id     = start_num;
	start_id   = start_num;

READ_START:

	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FF, &cap,0);
		disp_p = point(0,0);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	}

	j = 0;
	while (cur_id <= end_num)
	{
		addr =  addr_flash + (long)TRACE_INDIVIDUAL_IDENTITY_SIZE * (cur_id-1);			
	#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
		read_code = Flash_wread(addr);
	#else
		read_code = Flash_bread(addr);
	#endif
		if(read_code == cur_id)  
		{	
			j++;
			Flash_sread(addr, (HUGEDATA INT8U *)&indivStr, TRACE_INDIVIDUAL_IDENTITY_SIZE );

		 	indivStr.individualNO[12]=0;
#ifdef USE_TRACE_MEATGRADE
			sprintf(string_buf, "%2d  %-12s  %d", indivStr.index, indivStr.individualNO, indivStr.gradeNo);
#else
			sprintf(string_buf, "%2d  %-12s  %-4s", indivStr.index, indivStr.individualNO, indivStr.grade);
#endif
#ifdef USE_CHECKING_HARMFUL_MEAT
			len = strlen(string_buf);
			string_buf[len++] = ' ';
			string_buf[len++] = indivStr.harmfulness + '0';
			string_buf[len++] = 0x00;
#endif

			string_buf[lcd_size] = 0;			
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);	       		 
			Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
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
			addr =  addr_flash + (long)TRACE_INDIVIDUAL_IDENTITY_SIZE * (cur_id-1);			
#ifdef USE_TRACE_1000QTY //sewon 161216
			read_code = Flash_wread(addr);
#else
			read_code = Flash_bread(addr);
#endif
			if(read_code == cur_id )  
				goto READ_START;
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
			addr =  addr_flash + (long)TRACE_INDIVIDUAL_IDENTITY_SIZE * (cur_id-1);			
#ifdef USE_TRACE_1000QTY //sewon 161216
			read_code = Flash_wread(addr);
#else
			read_code = Flash_bread(addr);
#endif
			if(read_code == cur_id /*>= min_num && read_code <= max_num*/ )  
			{
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
			addr =  addr_flash + (long)TRACE_INDIVIDUAL_IDENTITY_SIZE * (cur_id-1);			
#ifdef USE_TRACE_1000QTY //sewon 161216
			read_code = Flash_wread(addr);
#else
			read_code = Flash_bread(addr);
#endif
			if(read_code == cur_id )  
			{
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
			addr =  addr_flash + (long)TRACE_INDIVIDUAL_IDENTITY_SIZE * (reverse_num-1);			
#ifdef USE_TRACE_1000QTY //sewon 161216
			read_code = Flash_wread(addr);
#else
			read_code = Flash_bread(addr);
#endif
			if(read_code == reverse_num )  
			{
			       	cur_id = reverse_num;
				goto READ_START;
			}
		}
		BuzOn(2);
		error_display_caption(0xE00E);
		goto KEY_INPUT;
	case KP_TEST:
		//individual_print_list(start_num, end_num);
		individualData_print_list(start_num, end_num);
		goto KEY_INPUT;		
	case KP_FEED:
		keyapp_feed(OFF);
		goto KEY_INPUT;
	default :	
		BuzOn(2);
		goto KEY_INPUT;
	} //switch(KeyDrv.CnvCode)						

	RET_ESC:    return;
	
}
#endif //#ifndef DISABLE_LIST_MENU

void    individualData_delete(void)
{
	menu1_delete(0x1373, 0x1AF0, FLASH_TRACE_STANDALONE_AREA, TRACE_INDIVIDUAL_IDENTITY_SIZE);
}

void 	individualData_assign(void)
{

	CAPTION_STRUCT 	   cap;
	POINT 		   disp_p;	
	TRACE_INDIVIDUAL_IDENTITY indivStr;

	char 	input_line;
	INT16S 	i, result,start_line;
	long  	ret_long;
	INT16U	input_value[2];
	INT32U  addr;

	INT32U  addr_key;
	INT16U  cap_no[2]={ 0x1AFA,0x1AF0 };
	input_value[0] = 1;
	input_value[1] = 1;

	start_line=1;
 	if (Startup.menu_type==0) start_line=0;

	input_value[0]=1;
	addr_key = FLASH_TRACE_STANDALONE_KEY_AREA + DFLASH_BASE;
	addr_key += (input_value[0]-1);	 	
#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
	input_value[1] = Flash_wread(addr_key);
#else
	input_value[1] = Flash_bread(addr_key);  
#endif
	input_line=0;
PAGE2_VIEW:
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1374, 1, 1);
	for(i=0; i<2; i++)  {
		caption_split_by_code(cap_no[i],&cap,0);
		if (i==1) {
			cap.input_min    = 1;
			cap.input_max    = MaxIndividualNo-1;
			cap.input_length = ilog((INT32U)cap.input_max);
			cap.input_dlength =cap.input_length;
		}
		caption_adjust(&cap,NULL);
		disp_p = point( (i+start_line)*Startup.menu_ygap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		Dsp_Diffuse();
		ret_long = input_value[i];
		if( !(input_range_check(ret_long, cap.input_min, cap.input_max)) ) {
			input_value[0] = 1;
			input_value[1] = 1;
			ret_long = 1;			
		} 
		if (i == 1) {
//sg		 	callback_clerk_position_x = strlen((char*)cap.form)+1; 
//sg		 	callback_clerk_position_y = start_line+1;
//sg		 	keyin_setcallback(callback_clerk_name);
		} 
		result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length, 
			     	     (i+start_line)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
//sg		keyin_clearcallback();
	}
	for(i=input_line; i<2; i++)  {
		caption_split_by_code(cap_no[i], &cap,0);
		if (i==1) {
			if (input_value[0]==0) input_value[0]=1;
			addr_key = FLASH_TRACE_STANDALONE_KEY_AREA + DFLASH_BASE;
			addr_key += (input_value[0]-1);
		#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
			input_value[i]=Flash_wread(addr_key); 
		#else
			input_value[i]=Flash_bread(addr_key); 
		#endif
			cap.input_min    = 1;
			cap.input_max    = MaxIndividualNo-1;
			cap.input_length = ilog((INT32U)cap.input_max);
			cap.input_dlength =cap.input_length;
		}
		caption_adjust(&cap,NULL);
		disp_p = point( (i+start_line)*Startup.menu_ygap,0); 
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		Dsp_Diffuse();
PAGE2_KI_IN:	ret_long = input_value[i];
		if( !(input_range_check(ret_long, cap.input_min, cap.input_max)) ) {
			ret_long = 1;
		}
//sg	 	callback_clerk_position_x = strlen((char*)cap.form)+1; 
//sg	 	callback_clerk_position_y = start_line+1;
		switch(i) {
			case 0:	KEY_SetMode(3);
				break;
			case 1:	KEY_SetMode(1);
//sg			 	keyin_setcallback(callback_clerk_name);
				break;
		}
		result = keyin_ulong(KI_GRAPHIC, 
				     KI_FORM_NORMAL, 
				     0,
				     (long*)&ret_long, 
				     cap.input_length, 
			     	     (i+start_line)*Startup.menu_ygap,  
			     	     cap.input_x*Startup.menu_xgap, 
			     	     0,
			     	     KI_NOPASS );
		KEY_SetMode(1);
//sg		keyin_clearcallback();
		if(result == KP_ESC) goto RET_ESC;
		if( input_range_check(ret_long, cap.input_min, cap.input_max) ) {
			input_value[i] = ret_long;
		} else { 
			error_display_form1(0, cap.input_min, cap.input_max);
			goto PAGE2_KI_IN;
		}
		switch(result) {
			case 	KP_ARROW_RI:
				if (input_value[i] < cap.input_max)
					input_value[i]++;
				goto PAGE2_KI_IN;
			case 	KP_ARROW_LF:
				if (input_value[i] > cap.input_min)
					input_value[i]--;
				goto PAGE2_KI_IN;
			case	KP_ARROW_DN:
			case 	KP_ENTER:
					if(i==1) goto RET_END;													
					break;
			case	KP_ARROW_UP:
					switch(i) {
						case 0:
							goto PAGE2_VIEW;
						case 1:
							input_line = 0;
							goto PAGE2_VIEW;
					}
			case 	KP_SAVE:	
					goto RET_END;

			default:		
					BuzOn(2);						
					goto PAGE2_KI_IN;
		} // switch(result);
	} // end page2_input
RET_END:
	if ((input_value[1]>0) && (input_value[1]<MaxIndividualNo)) {
		addr = input_value[1]-1;
		addr *= TRACE_INDIVIDUAL_IDENTITY_SIZE;
		addr += FLASH_TRACE_STANDALONE_AREA;
#ifdef USE_TRACE_1000QTY //sewon 161206
		get_ext_base_sparam(addr, (INT8U*)&indivStr, TRACE_INDIVIDUAL_IDENTITY_SIZE);
#else
		get_base_sparam(addr, (INT8U*)&indivStr, TRACE_INDIVIDUAL_IDENTITY_SIZE);
#endif
		if ((indivStr.index > 0) && (indivStr.index < MaxIndividualNo)) {
		     if (input_value[0]) {
			addr_key = FLASH_TRACE_STANDALONE_KEY_AREA + DFLASH_BASE;
			addr_key += (input_value[0]-1);	        
		#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
			Flash_wwrite(addr_key, input_value[1]);
		#else
			Flash_bwrite(addr_key, input_value[1]);	
		#endif
			input_value[0] += 1;
			if(input_value[0] > MAX_INDIVIDUAL_KEY_NO ) input_value[0] = MAX_INDIVIDUAL_KEY_NO;
			input_value[1] += 1;
			if(input_value[1] >= MaxIndividualNo) input_value[1] = MaxIndividualNo - 1;
			input_line = 0;
		     } else {
			input_value[0]=1;
		     }
		} else {
		     BuzOn(2);
		     goto PAGE2_KI_IN;
		}
	} else {
		input_value[0]=1;
	}
	addr_key = FLASH_TRACE_STANDALONE_KEY_AREA + DFLASH_BASE;
	addr_key += (input_value[0]-1);	       
#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
	input_value[1]=Flash_wread(addr_key);
#else
	input_value[1]=Flash_bread(addr_key);
#endif
	goto PAGE2_VIEW;
				       				 
RET_ESC:
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
 	return;
  
}

extern INT8U LotStr[4];
extern INT8U LotStrL[4];
INT8U checkIndividualNo(INT8U *ind_no_str)
{
	INT8U lotFlag;

	lotFlag = 0;
	if(strncmp((char *)ind_no_str, (char*)LotStrL, 3) == 0 || strncmp((char *)ind_no_str, (char*)LotStr, 3) == 0)
	{
		lotFlag = 1;
	}
	return lotFlag;
}

// parameter
//   mode = 0 : 전체 영역 검색
//          1 : 개체리스트 영역 검색
//          2 : 위해리스트 영역 검색
// return
//   :식별번호가 없을 경우 return 값은 MAX_INDIVIDUAL_NO(100)이다.
INT16U individualData_search(INT8U *indStr, INT8U mode)
{
	INT32U addr;
	INT16U indLen, len;
	INT8U savedIndStr[INDIVIDUAL_NO_LEN + 1];
	INT8U result;
	INT16U idx;
	INT16S j;
	INT16U start_idx;

	indLen = strlen((char*)indStr);	
	if (!indLen) return MAX_INDIVIDUAL_NO;	
	if (indLen > INDIVIDUAL_NO_LEN) indLen = INDIVIDUAL_NO_LEN;

//sprintf(MsgBuf, "\r\n[%d](%s)\r\n", mode, indStr);//
//MsgOut(MsgBuf);//
	if (mode == 2 || mode == 0)	// 위해리스트 search
	{
		start_idx = MaxIndividualNo;
#ifdef USE_TRACE_1000QTY //sewon 161201
		addr = EXT_MEMCARD_ADDR;
#else
		addr = DFLASH_BASE;
#endif
		addr+= FLASH_TRACE_STANDALONE_AREA;
		addr += (INT32U)(start_idx - 1) * TRACE_INDIVIDUAL_IDENTITY_SIZE;
		for (idx = start_idx; idx <= MAX_INDIVIDUAL_NO - 1; idx++)
		{
//sprintf(MsgBuf, "(%2d,%08lx) ", idx, addr);//
//MsgOut(MsgBuf);//
			result = OFF;
	#ifdef USE_KOR_TRACE_999
			if (Flash_wread(addr) == idx)
			{
				Flash_sread(addr + 3, (HUGEDATA INT8U *)&savedIndStr, INDIVIDUAL_NO_LEN);
	#else
	  #ifdef USE_TOPMART_STRUCTURE
	  		if (Flash_wread(addr) == idx)
			{
				Flash_sread(addr + 2, (HUGEDATA INT8U *)&savedIndStr, INDIVIDUAL_NO_LEN);
	  #else
			if (Flash_bread(addr) == idx)
			{
				Flash_sread(addr + 1, (HUGEDATA INT8U *)&savedIndStr, INDIVIDUAL_NO_LEN);
	  #endif
	#endif
				savedIndStr[INDIVIDUAL_NO_LEN] = 0;
				len = strlen((char*)savedIndStr);
				if (len != indLen) result = OFF;
				else
				{
					result = ON;
					for (j = 0; j < len; j++)
					{
						if (indStr[j] != savedIndStr[j]) 
						{
							result = OFF;
							break;
						}
					}
				}
			}
			if (result) return idx;
			addr += TRACE_INDIVIDUAL_IDENTITY_SIZE;
		}
	}
	if (mode == 1 || mode == 0)	// 개체리스트 영역 검색
	{
		start_idx = 1;
#ifdef USE_TRACE_1000QTY //sewon 161201
		addr = EXT_MEMCARD_ADDR;
#else
		addr = DFLASH_BASE;
#endif
		addr+= FLASH_TRACE_STANDALONE_AREA;
		addr += (INT32U)(start_idx - 1) * TRACE_INDIVIDUAL_IDENTITY_SIZE;
		for (idx = start_idx; idx <= MaxIndividualNo - 1; idx++)
		{
//sprintf(MsgBuf, "<%2d,%08lx> ", idx, addr);//
//MsgOut(MsgBuf);//
			result = OFF;
	#ifdef USE_KOR_TRACE_999
			if (Flash_wread(addr) == idx)
			{
				Flash_sread(addr + 3, (HUGEDATA INT8U *)&savedIndStr, INDIVIDUAL_NO_LEN);
	#else
	  #ifdef USE_TOPMART_STRUCTURE
	  		if (Flash_wread(addr) == idx)
			{
				Flash_sread(addr + 2, (HUGEDATA INT8U *)&savedIndStr, INDIVIDUAL_NO_LEN);
	  #else
			if (Flash_bread(addr) == idx)
			{
				Flash_sread(addr + 1, (HUGEDATA INT8U *)&savedIndStr, INDIVIDUAL_NO_LEN);
	  #endif
	#endif
				savedIndStr[INDIVIDUAL_NO_LEN] = 0;
				len = strlen((char*)savedIndStr);
				if (len != indLen) result = OFF;
				else
				{
					result = ON;
					for (j = 0; j < len; j++)
					{
						if (indStr[j] != savedIndStr[j]) 
						{
							result = OFF;
							break;
						}
					}
				}
			}
			if (result) return idx;
			addr += TRACE_INDIVIDUAL_IDENTITY_SIZE;
		}
	}
	return MAX_INDIVIDUAL_NO;
}

#ifdef USE_CHECKING_HARMFUL_MEAT
INT8U individualData_searchEmptyIdx(void)
{
	INT32U addr;
	INT8U idx;
       
#ifdef USE_TRACE_1000QTY //sewon 161201
	addr = EXT_MEMCARD_ADDR;
#else
	addr = DFLASH_BASE;
#endif
	addr+= FLASH_TRACE_STANDALONE_AREA;
  	addr += (INT32U)(MaxIndividualNo - 1) * TRACE_INDIVIDUAL_IDENTITY_SIZE;

	for (idx = MaxIndividualNo - 1; idx < MAX_INDIVIDUAL_NO - 1; idx++)
	{
		if (Flash_bread(addr) != (idx + 1))
		{
			return idx + 1;
		}
		addr += TRACE_INDIVIDUAL_IDENTITY_SIZE;
	}
	return MAX_INDIVIDUAL_NO;
}
#endif

void individual_checkZero(INT8U *ind_str)
{
	INT8U nonzero;
	INT8U i;
	INT16U len;

	//ind_str[20] = 0;
	len = strlen((char *)ind_str);
	if (len > INDIVIDUAL_NO_LEN) len = INDIVIDUAL_NO_LEN; 
	nonzero = 0;
	for (i = 0; i < len; i++)
	{
		if (ind_str[i] == 0) break;
		if (ind_str[i] != '0') nonzero = 1;
	}
	if (!nonzero) ind_str[0] = 0;	// '0'으로만 구성되어 있으면, 개체 무시
}

//modified by JJANG, Weight 항목 Flash --> NVRAM
void RemainWeightSummary(long trWeight, INT8U index)
{
	long weight;

	if (index > 0 && index < MaxIndividualNo)
	{
		weight = get_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)index-1)*4);
		weight -= trWeight;
		if(weight<0) weight =0;

		set_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)index-1)*4, weight);
	}
	if (index == CurTraceStatus.indivStr.index) CurTraceStatus.indivWeight = weight;

}


void keyapp_trace_noneid(void)
{
	sale_display_indivFlag_set(INDIV_DISP_MODE_ALL, OFF);
	sale_display_update(UPDATE_TITLE);	// CJK070705
}

void individualData_dataUpdate(INT32U num)
{
#ifdef USE_TRACE_1000QTY //sewon 161219
	INT8U menuId[14];
#else
	INT8U menuId[10];
#endif
#ifdef USE_CHECKING_HARMFUL_MEAT
	CAPTION_STRUCT cap;
#endif
	TRACE_INDIVIDUAL_IDENTITY indivStr;
	INT16U  strSize;
	INT32U	addr;
#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
	INT16U	indivIndex;
#else
	INT8U   indivIndex;
#endif
	INT8U   stringSet[10];
	long	indivWeight;
	      
	strSize = TRACE_INDIVIDUAL_IDENTITY_SIZE;
#ifdef USE_TRACE_1000QTY //sewon 161201
	addr = EXT_MEMCARD_ADDR;
#else
	addr = DFLASH_BASE;
#endif
	addr += FLASH_TRACE_STANDALONE_AREA;
	addr += ((num - 1) * strSize);
#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY)
	indivIndex = Flash_wread(addr);
#else
	indivIndex = Flash_bread(addr);
#endif
	if(indivIndex == num){
		Flash_sread(addr, (HUGEDATA INT8U *)&indivStr, strSize );
		indivWeight = get_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)indivStr.index-1)*4);
	} else {
		memset(&indivStr, 0, strSize );
		indivStr.index = num;		
		//set_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)indivStr.index-1)*4, 0);
		indivWeight = 0;
	}

	menuId[0] = Menu_GetMenuId(M_ID_TRS_IND_NO);
#ifdef USE_TRACE_MEATGRADE
	menuId[1] = Menu_GetMenuId(M_ID_TRS_GRADE_INDEX);
#else
	menuId[1] = Menu_GetMenuId(M_ID_TRS_GRADE);
#endif
	menuId[2] = Menu_GetMenuId(M_ID_TRS_SL_DATE);
	menuId[3] = Menu_GetMenuId(M_ID_TRS_WEIGHT);
	menuId[4] = Menu_GetMenuId(M_ID_TRS_BEED);
	menuId[5] = Menu_GetMenuId(M_ID_TRS_FARM);
	menuId[6] = Menu_GetMenuId(M_ID_TRS_SLAUGHT);//gm 090404
	menuId[7] = Menu_GetMenuId(M_ID_TRS_LOTFLAG);
#ifdef USE_TRACE_1000QTY
	menuId[10] = Menu_GetMenuId(M_ID_TRS_MEATKIND);
	menuId[11] = Menu_GetMenuId(M_ID_TRS_HARMFUL);
	menuId[12] = Menu_GetMenuId(M_ID_TRS_NONGJANGNO);
	menuId[13] = Menu_GetMenuId(M_ID_TRS_SLAUGHTNO);
#endif
// 영역에 따라 고정되어 있으므로, 위해여부 설정은 의미가 없음 => 삭제
//#ifdef USE_CHECKING_HARMFUL_MEAT
//	menuId[8] = Menu_GetMenuId(M_ID_TRS_HARMFUL);
//#endif
#ifdef USE_KOR_TRACE_999
	menuId[9] = Menu_GetMenuId(M_ID_TRS_LOTPART);
#endif

#ifdef USE_CHECKING_HARMFUL_MEAT
	caption_split_by_code(0x1AF1, &cap,0);	 //index
	if (num >= MaxIndividualNo)
	{
		Menu[menuId[1]].inhibit = ON;
		Menu[menuId[2]].inhibit = ON;
		Menu[menuId[3]].inhibit = ON;
		Menu[menuId[4]].inhibit = ON;
		Menu[menuId[5]].inhibit = ON;
		Menu[menuId[6]].inhibit = ON;
		Menu[menuId[7]].inhibit = ON;
	#ifdef USE_KOR_TRACE_999
		//Menu[menuId[9]].inhibit = ON;
		#error "수입육과 농협개체는 같이 사용할 수 없습니다."
	#endif
		strcpy(cap.form, "위해번호");
	}
	else
	{
		Menu[menuId[1]].inhibit = OFF;
		Menu[menuId[2]].inhibit = OFF;
		Menu[menuId[3]].inhibit = OFF;
		Menu[menuId[4]].inhibit = OFF;
		Menu[menuId[5]].inhibit = OFF;
		Menu[menuId[6]].inhibit = OFF;
		Menu[menuId[7]].inhibit = OFF;
	#ifdef USE_KOR_TRACE_999
		//Menu[menuId[9]].inhibit = OFF;
		#error "수입육과 농협개체는 같이 사용할 수 없습니다."
	#endif
	}
	caption_adjust(&cap,NULL);
	strcpy(Menu[menuId[0]].str, cap.form);
#endif

	if(menuId[0] != 0xff) {
		memcpy(Menu[menuId[0]].pstr, indivStr.individualNO, Menu[menuId[0]].length);
	}
#ifdef USE_TRACE_MEATGRADE
	if(menuId[1] != 0xff) {
		*Menu[menuId[1]].pval = indivStr.gradeNo;
	}
#else
	if(menuId[1] != 0xff) {
		memcpy(Menu[menuId[1]].pstr, indivStr.grade, Menu[menuId[1]].length);
	}
#endif
	if(menuId[3] != 0xff) {
		//indivWeight = get_nvram_lparam(NVRAM_TRACE_STANDALONE_WEIGHT+((INT32U)indivStr.index-1)*4);
		*Menu[menuId[3]].pval = indivWeight;
	}
	if(menuId[4] != 0xff) {
		memcpy(Menu[menuId[4]].pstr, indivStr.breed, Menu[menuId[4]].length);
	}
	if(menuId[5] != 0xff) {
		memcpy(Menu[menuId[5]].pstr, indivStr.farm, Menu[menuId[5]].length);
	}
	if(menuId[7] != 0xff) {
		*Menu[menuId[7]].pval = indivStr.lotFlag;
	}
#ifdef USE_KOR_TRACE_999
	if(menuId[9] != 0xff) {
		memcpy(Menu[menuId[9]].pstr, indivStr.farm, Menu[menuId[9]].length);
	}
#endif
#ifdef USE_TRACE_1000QTY //sewon 161219
	if(menuId[10] != 0xff) {
		*Menu[menuId[10]].pval = indivStr.meatKind;
	}
	if(menuId[11] != 0xff) {
		*Menu[menuId[11]].pval = indivStr.harmfulness;
	}
	if(menuId[12] != 0xff) {
		memcpy(Menu[menuId[12]].pstr, indivStr.nongjangNo, Menu[menuId[12]].length);
	}
	if(menuId[13] != 0xff) {
		*Menu[menuId[13]].pval = indivStr.slaughtNo;
	}
#endif

#ifdef USE_KOR_TRACE_999
	if (indivStr.lotFlag)
	{
		Menu[menuId[2]].inhibit = ON;	// slaught date

		Menu[menuId[9]].inhibit = OFF;	// lot part
		     
		Menu[menuId[6]].length = 100;
		if(menuId[6] != 0xff) {//gm 090404
			memcpy(Menu[menuId[6]].pstr, indivStr.slaughtHouse, Menu[menuId[6]].length);
		}
	}
	else
	{
		Menu[menuId[2]].inhibit = OFF;	// slaught date

		Menu[menuId[9]].inhibit = ON;	// lot part

		sprintf((char*)stringSet, "%02d.%02d.%02d", indivStr.slaughtDate[0],indivStr.slaughtDate[1],indivStr.slaughtDate[2]);
		if(menuId[2] != 0xff) {
			memcpy(Menu[menuId[2]].pstr, stringSet,  Menu[menuId[2]].length);
		}
		if(menuId[6] != 0xff) {//gm 090404
			memcpy(Menu[menuId[6]].pstr, indivStr.slaughtHouse, Menu[menuId[6]].length);
		}
	}
#else
	if(menuId[2] != 0xff) {
		sprintf((char*)stringSet, "%02d.%02d.%02d", indivStr.slaughtDate[0],indivStr.slaughtDate[1],indivStr.slaughtDate[2]);
		memcpy(Menu[menuId[2]].pstr, stringSet,  Menu[menuId[2]].length);
	}
	if(menuId[6] != 0xff) {//gm 090404
		memcpy(Menu[menuId[6]].pstr, indivStr.slaughtHouse, Menu[menuId[6]].length);
	}
// 영역에 따라 고정되어 있으므로, 위해여부 설정은 의미가 없음 => 삭제
//#ifdef USE_CHECKING_HARMFUL_MEAT
//	if(menuId[8] != 0xff) {
//		*Menu[menuId[8]].pval = indivStr.harmfulness;
//	}
//#endif
#endif
	MenuRefresh = ON;
}

#ifdef USE_KOR_TRACE_999
void individualData_LotFlagUpdate(INT32U num)
{
	INT8U menuId[6];

	menuId[0] = Menu_GetMenuId(M_ID_TRS_GRADE);
	menuId[1] = Menu_GetMenuId(M_ID_TRS_SL_DATE);
	menuId[2] = Menu_GetMenuId(M_ID_TRS_BEED);
	menuId[3] = Menu_GetMenuId(M_ID_TRS_FARM);
	menuId[4] = Menu_GetMenuId(M_ID_TRS_SLAUGHT);
	menuId[5] = Menu_GetMenuId(M_ID_TRS_LOTPART);

	if (num)
	{
		Menu[menuId[0]].inhibit = OFF;	// grade
		Menu[menuId[1]].inhibit = ON;	// slaught date
		Menu[menuId[2]].inhibit = OFF;	// breed
		Menu[menuId[3]].inhibit = ON;	// farm     
		Menu[menuId[4]].length = 100;
		Menu[menuId[5]].inhibit = OFF;	// lot part
	}
	else
	{
		Menu[menuId[0]].inhibit = OFF;
		Menu[menuId[1]].inhibit = OFF;
		Menu[menuId[2]].inhibit = OFF;
		Menu[menuId[3]].inhibit = OFF;	// farm
		Menu[menuId[4]].length = 48;
		Menu[menuId[5]].inhibit = ON;	// lot part
	}
	MenuRefresh = ON;
}
#endif

/*
//void initGradeData_default(void)
{

//	INT16U cap_code[2]={9,0x3211};
//	menu1_create(0x1221,cap_code,0, FLASH_SUB_DEPARTMENT_AREA, GROUP_STRUCT_SIZE);	

	MENU_TYPE1_STRUCT	group_str;  // Group Table을 등급 Table로 사용 함	
//	INT8U	buffer[64];
	INT32U	addr = 0;
//	INT16S  	str_size;
	INT8U  i;
	//char *gradeList[] = { "1++", "1+", "1", "2++", "2+", "2", "3++", "3+", "3", 0 };
	char *gradeList[] = { "1++A", "1++B", "1++C", 
			      "1+A", "1+B", "1+C",
			      "1A", "1B", "1C",
			      "2A", "2B", "2C",
			      "3A", "3B", "3C",
			      "D",
			      0 };

	for (i = 0; i < 99; i++)
	{
		memset(&group_str, 0, sizeof(group_str));

		if(i >= 0 && i < 16)
		{
			group_str.code = i+1;
			strcpy((char *)group_str.text, gradeList[i]);
		}
		addr  = GROUP_STRUCT_SIZE * i;
		addr += DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA;
		Flash_swrite(addr, (HUGEDATA INT8U *)&group_str, GROUP_STRUCT_SIZE);
	}
}
*/

// Trace No Table(TNT)

#define MAX_TRACE_NO_TABLE_1	3000
#define MAX_TRACE_NO_TABLE_2	5000
//INT16U TNT_TraceNoTable[MAX_TRACE_NO_TABLE + 1];	// 3000 + 1(전체개체영역)

void TNT_InitTraceNoTable(void)
{
	INT32U addr;
	INT16U i;

	addr = NVRAM_CLEARK_LASTSALE;
	//memset((INT8U *)addr, 0, (MAX_TRACE_NO_TABLE_1 + 1) * 2);
	nvram_set(addr, 0, (MAX_TRACE_NO_TABLE_1 + 1) * 2);

	addr += TNT_TRACENO_TABLE_SIZE;
	for (i = 0; i < TNT_TRACENO_TABLE_NO; i++)
	{
		//memset((INT8U*)addr, 0, TNT_PLU_INDIVIDUAL_DATA_SIZE);
		nvram_set(addr, 0, TNT_PLU_INDIVIDUAL_DATA_SIZE);
		addr += TNT_PLU_INDIVIDUAL_DATA_SIZE;
	}
	
	addr = NVEXT0_TRACEINFO_TABLE_2;
	//memset((INT8U *)addr, 0, (MAX_TRACE_NO_TABLE_2) * 2);
	nvram_set(addr, 0, (MAX_TRACE_NO_TABLE_2) * 2);
}

INT16U TNT_getEmptyNode(void)
{
	INT8U *p;
	INT16U i;
	INT32U addr;
	INT8U j;

	addr = NVRAM_CLEARK_LASTSALE;
	addr += TNT_TRACENO_TABLE_SIZE;
	
	//p = (HUGEDATA INT8U *)addr;
	for (i = 0; i < TNT_TRACENO_TABLE_NO; i++)
	{
#ifndef USE_MULTI_TRACE_NO_TABLE
		//if(!*p)	//첫바이트, individual no. 없으면 나머지 data 도 없는지 비교. Single indiv는 useFlag 가 없음.
		if(!get_nvram_bparam(addr))	//첫바이트, individual no. 없으면 나머지 data 도 없는지 비교. Single indiv는 useFlag 가 없음.
		{
			for(j = INDIVIDUAL_NO_LEN ; j < TNT_PLU_INDIVIDUAL_DATA_SIZE ; j++)
			{
				//if(*(p + j)) break;
				if(get_nvram_bparam(addr + j)) break;
				if(j == (TNT_PLU_INDIVIDUAL_DATA_SIZE - 1)) return (i + 1);
			}
		}
#else
		//if (!*p) return i + 1;	// 첫byte가 NULL이면 empty
		if (!get_nvram_bparam(addr)) return i + 1;	// 첫byte가 NULL이면 empty
#endif
		//p += TNT_PLU_INDIVIDUAL_DATA_SIZE;
		addr += TNT_PLU_INDIVIDUAL_DATA_SIZE;
	}
	return 0xffff;	// full
}

// dept와 pluno로 개체정보가 저장되어 있는 위치(index)를 찾아주는 함수
// PLU Table의 3000번 째 이하의 PLU는 NVRAM_CLEARK_LASTSALE에 위치한 table에서 찾고,
// PLU Table의 3000번 째를 초과하는 PLU는 NVEXT0_TRACEINFO_TABLE_2에 위차한 table에서 찾는다.
INT16U TNT_searchTraceNoKey(INT8U dept, INT32U pluno, INT16U key)
{
	//INT16U key;
	INT32U n_addr;

	if (key)
	{
		if (key > MAX_PLU_NO) return 0xfffe;
		if (key > 3000) key += 1;	// 3001은 전체개체 공간
	}
	else
	{
		if (SaveTraceabilityNo == 2) key = 3001;	// 3001:전체개체(공통영역), 1~3000:상품개체(PLU별 영역)
		else 
		{
			plu_table_search(dept, pluno, &key, 0);
			if (key > MAX_PLU_NO) return 0xfffe; // 개체기억기능을 3000개로 제한(메모리 제한 때문)
			if (key > 3000) key += 1;
		}
	}

	if (!key) return 0xffff;
	
	if (key > 3001)	// 확장NVRAM 영역
	{
		n_addr = NVEXT0_TRACEINFO_TABLE_2;
		key -= 3001;
	}
	else
	{
		n_addr = NVRAM_CLEARK_LASTSALE;
	}
	n_addr += (key - 1) * 2;

	return get_nvram_wparam(n_addr);
}

void TNT_writeTraceNoKey(INT8U dept, INT32U pluno, INT16U key, INT16U tableKey)
{
	//INT16U key;
	INT32U n_addr;

	if (key)
	{
		if (key > MAX_PLU_NO) return; // 개체기억기능을 3000개로 제한(메모리 제한 때문)
		if (key > 3000) key += 1;	// 3001은 전체개체 공간
	}
	else
	{
		if (SaveTraceabilityNo == 2) key = 3001;	// 3001:전체개체(공통영역), 1~3000:상품개체(PLU별 영역)
		else 
		{
			plu_table_search(dept, pluno, &key, 0);
			if (key > MAX_PLU_NO) return; // 개체기억기능을 3000개로 제한(메모리 제한 때문)
			if (key > 3000) key += 1;
		}
	}

	if (!key) return;
	
	if (key > 3001)	// 확장NVRAM 영역
	{
		n_addr = NVEXT0_TRACEINFO_TABLE_2;
		key -= 3001;
	}
	else
	{
		n_addr = NVRAM_CLEARK_LASTSALE;
	}
	n_addr += (key - 1) * 2;

//sprintf(MsgBuf, "[wT]d=%d, p=%ld, tkey=%d, addr=%08lx\r\n", dept, pluno, tableKey, n_addr);
//MsgOut(MsgBuf);
	set_nvram_wparam(n_addr, tableKey);
}


void TNT_DelTraceNoTable(INT8U dept, INT32U pluno, INT16U plukey)
{
	//PLU_INDIVIDUAL_NODE_DATA pluTraceNo;
	INT8U buf[100];
	INT32U n_addr;
	INT16U tableKey;

	tableKey = TNT_searchTraceNoKey(0, 0, plukey);
//sprintf(MsgBuf, "[DT]d=%d, p=%ld, tkey=%d, pkey=%d\r\n", dept, pluno, tableKey, plukey);
//MsgOut(MsgBuf);
	if (tableKey > TNT_TRACENO_TABLE_NO) return;	// 0xffff : table max(3001) 초과 or 해당 PLU 없을 때
	if (!tableKey) return;
	
	n_addr = NVRAM_CLEARK_LASTSALE;
	n_addr += TNT_TRACENO_TABLE_SIZE;	// 3001*2
	n_addr += (tableKey - 1) * TNT_PLU_INDIVIDUAL_DATA_SIZE;

	// data 삭제
	memset((char *)buf, 0, TNT_PLU_INDIVIDUAL_DATA_SIZE);
	set_nvram_sparam(n_addr, (INT8U*)buf, TNT_PLU_INDIVIDUAL_DATA_SIZE);

	// table 삭제
	TNT_writeTraceNoKey(0, 0, plukey, 0);
}


#ifdef USE_MULTI_TRACE_NO_DBG
void TNT_TestTraceNoTable(void)
{
	INT32U i;
	INT8U ind_str1[30];
	INT8U ind_str2[30];
	INT8U ind_str3[30];
	INT8U sl_no1;
	INT8U sl_no2;
	INT8U sl_no3;
	INT8U meat_use;
	INT8U meat_part;
	INT8U lot_flag;
	INT8U meat_grade;
	INT8U r_ind_str1[30];
	INT8U r_ind_str2[30];
	INT8U r_ind_str3[30];
	INT8U r_sl_no1;
	INT8U r_sl_no2;
	INT8U r_sl_no3;
	INT8U r_meat_use;
	INT8U r_meat_part;
	INT8U r_lot_flag;
	INT8U r_meat_grade;
	INT16U prev_error, error;
	INT16U r_ind_no;
	

	TNT_InitTraceNoTable();	// 초기화
	
	error = 0;	
	for (i = 1; i < TNT_TRACENO_TABLE_NO + 2; i++)
	{
		sprintf(ind_str1, "0%05ld%06ld", i, i);
		sl_no1 = i%100;
		meat_use = (i+3)%100;
		meat_part = (i+8)%100;
		meat_grade = (i+7)%100;
		lot_flag = i%2;
#ifdef USE_MULTI_TRACE_NO_TABLE
		sprintf(ind_str2, "1%05ld%06ld", i, i);
		sprintf(ind_str3, "2%05ld%06ld", i, i);
		sl_no2 = (i+2)%100;
		sl_no3 = (i+5)%100;
#endif
#ifdef USE_MULTI_TRACE_NO_TABLE
		TNT_SetTraceNoTable(1, i, ind_str1, ind_str2, ind_str3, sl_no1, sl_no2, sl_no3, meat_use, meat_part, lot_flag, meat_grade);
#else
		TNT_SetTraceNoTable_Single(1, i, ind_str1, sl_no1, meat_use, meat_part, lot_flag, meat_grade);
#endif
	}
	
	for (i = 1; i < TNT_TRACENO_TABLE_NO + 2; i++)
	{
#ifdef USE_MULTI_TRACE_NO_TABLE
		TNT_GetTraceNoTable(1, i, &r_ind_no, r_ind_str1, r_ind_str2, r_ind_str3, &r_sl_no1, &r_sl_no2, &r_sl_no3, &r_meat_use, &r_meat_part, &r_lot_flag, &r_meat_grade);
#else
		TNT_GetTraceNoTable_Single(1, i, &r_ind_no, r_ind_str1, &r_sl_no1, &r_meat_use, &r_meat_part, &r_lot_flag, &r_meat_grade);
#endif
		sprintf(ind_str1, "0%05ld%06ld", i, i);
		sl_no1 = i%100;
		meat_use = (i+3)%100;
		meat_part = (i+8)%100;
		meat_grade = (i+7)%100;
		lot_flag = i%2;
#ifdef USE_MULTI_TRACE_NO_TABLE
		sprintf(ind_str2, "1%05ld%06ld", i, i);
		sprintf(ind_str3, "2%05ld%06ld", i, i);
		sl_no2 = (i+2)%100;
		sl_no3 = (i+5)%100;
#endif
		if (strcmp(ind_str1, r_ind_str1)) error++;
		if (sl_no1 != r_sl_no1) error++;
		if (meat_use != r_meat_use) error++;
#ifdef USE_TRACE_MEATPART
		if (meat_part != r_meat_part) error++;
#endif
#ifdef USE_TRACE_MEATGRADE
		if (meat_grade != r_meat_grade) error++;
#endif
#ifdef USE_MULTI_TRACE_NO_TABLE
		if (strcmp(ind_str2, r_ind_str2)) error++;
		if (strcmp(ind_str3, r_ind_str3)) error++;
		if (sl_no2 != r_sl_no2) error++;
		if (sl_no3 != r_sl_no3) error++;
#endif
		if (error != prev_error)
		{
			sprintf(MsgBuf , "%ld, errorcount=%d\r\n", i, error);
			MsgOut(MsgBuf);
		}
		prev_error = error;
	}
	sprintf(MsgBuf , "[DONE]\r\n");
	MsgOut(MsgBuf);
}
#endif

#ifdef USE_MULTI_TRACE_NO_TABLE
void TNT_SetTraceNoTable(INT8U dept, INT32U pluno, INT8U *ind_str1, INT8U *ind_str2, INT8U *ind_str3, INT8U sl_no1, INT8U sl_no2, INT8U sl_no3, INT8U meat_use, INT8U meat_part, INT8U lot_flag, INT8U meat_grade)
{
	PLU_INDIVIDUAL_NODE_DATA pluTraceNo;
	INT32U n_addr;
	INT16U tableKey;
	INT8U string_buf[50];
	INT8U newflag;

	newflag = 0;
	tableKey = TNT_searchTraceNoKey(dept, pluno, 0);
//sprintf(MsgBuf, "[a]d=%d, p=%ld, tkey=%d, maxno=%d\r\n", dept, pluno, tableKey, TNT_TRACENO_TABLE_NO);
//MsgOut(MsgBuf);
	if (tableKey > TNT_TRACENO_TABLE_NO)	// 0xfffe : table max(3001) 초과, 0xffff : 해당 PLU 없을 때
	{
		if (tableKey == 0xfffe)
		{
			//sprintf((char *)string_buf, "개체기억공간 부족(1)"); 
			sprintf((char *)string_buf, IndivMsg_TraceMemoryFull);
			display_message_page_mid((char *)string_buf);
		}
		return;
	}
	if (!tableKey )	// 등록된 개체가 없을 때
	{
		tableKey = TNT_getEmptyNode();	// 빈 주소 할당
		if (tableKey > TNT_TRACENO_TABLE_NO)	// 0xffff : 빈 주소 없을 때
		{ 
			//sprintf((char *)string_buf, "개체기억공간 부족(2)");
			sprintf((char *)string_buf, IndivMsg_TraceMemoryFull); 
			display_message_page_mid((char *)string_buf);
			return;
		}
		newflag = 1;
		TNT_writeTraceNoKey(dept, pluno, 0, tableKey);
	}
	n_addr = NVRAM_CLEARK_LASTSALE;
	n_addr += TNT_TRACENO_TABLE_SIZE;	// 3001*2
	n_addr += (tableKey - 1) * TNT_PLU_INDIVIDUAL_DATA_SIZE;

	if (newflag)
	{
		memset((char *)&pluTraceNo, 0, TNT_PLU_INDIVIDUAL_DATA_SIZE);
	}
	else // load
	{
		get_nvram_sparam(n_addr, (INT8U*)&pluTraceNo, TNT_PLU_INDIVIDUAL_DATA_SIZE);
	}
	pluTraceNo.useFlag = 1;	// 사용
	if (lot_flag != 0xff) pluTraceNo.isLot = lot_flag;
	if (meat_use != 0xff) pluTraceNo.meat_use = meat_use;
#ifdef USE_TRACE_MEATPART
	if (meat_part != 0xff) pluTraceNo.meat_part = meat_part;
#endif    
#ifdef USE_TRACE_MEATGRADE
	if (meat_grade != 0xff) pluTraceNo.meat_grade = meat_grade;
#endif
	if (sl_no1 != 0xff) pluTraceNo.slaughter_place[0] = sl_no1;
	if (sl_no2 != 0xff) pluTraceNo.slaughter_place[1] = sl_no2;
	if (sl_no3 != 0xff) pluTraceNo.slaughter_place[2] = sl_no3;
	if (ind_str1) memcpy(pluTraceNo.indivNo[0], (INT8U *)ind_str1, sizeof(pluTraceNo.indivNo[0]));
	if (ind_str2) memcpy(pluTraceNo.indivNo[1], (INT8U *)ind_str2, sizeof(pluTraceNo.indivNo[1]));
	if (ind_str3) memcpy(pluTraceNo.indivNo[2], (INT8U *)ind_str3, sizeof(pluTraceNo.indivNo[2]));

	// 기억할 항목이 없을 경우 해당 table과 data를 삭제함
	if (pluTraceNo.isLot == 0 && pluTraceNo.meat_use == 0  
#ifdef USE_TRACE_MEATPART
		&& pluTraceNo.meat_part == 0
#endif
#ifdef USE_TRACE_MEATGRADE
		&& pluTraceNo.meat_grade == 0
#endif
		&& pluTraceNo.slaughter_place[0] == 0 && pluTraceNo.slaughter_place[1] == 0 && pluTraceNo.slaughter_place[2] == 0
		&& pluTraceNo.indivNo[0][0] == 0 && pluTraceNo.indivNo[1][0] == 0 && pluTraceNo.indivNo[2][0] == 0)
	{
		// table 삭제
		TNT_writeTraceNoKey(dept, pluno, 0, 0);
		// data 삭제
		memset((char *)&pluTraceNo, 0, TNT_PLU_INDIVIDUAL_DATA_SIZE);
	}
	set_nvram_sparam(n_addr, (INT8U*)&pluTraceNo, TNT_PLU_INDIVIDUAL_DATA_SIZE);
}

void TNT_GetTraceNoTable(INT8U dept, INT32U pluno, INT16U *ind_no, INT8U *ind_str1, INT8U *ind_str2, INT8U *ind_str3, INT8U *sl_no1, INT8U *sl_no2, INT8U *sl_no3, INT8U *meat_use, INT8U *meat_part, INT8U *lot_flag, INT8U *meat_grade)
{
	PLU_INDIVIDUAL_NODE_DATA pluTraceNo;
	INT32U n_addr;
	INT16U tableKey;

	tableKey = TNT_searchTraceNoKey(dept, pluno, 0);
//sprintf(MsgBuf, "[A]d=%d, p=%ld, tkey=%d\r\n", dept, pluno, tableKey);
//MsgOut(MsgBuf);
	if (tableKey > TNT_TRACENO_TABLE_NO) return;	// 0xffff : table max(3001) 초과 or 해당 PLU 없을 때
	n_addr = NVRAM_CLEARK_LASTSALE;
	n_addr += TNT_TRACENO_TABLE_SIZE;	// 3001*2
	n_addr += (tableKey - 1) * TNT_PLU_INDIVIDUAL_DATA_SIZE;

	get_nvram_sparam(n_addr, (INT8U*)&pluTraceNo, TNT_PLU_INDIVIDUAL_DATA_SIZE);

	*lot_flag = pluTraceNo.isLot;
	*meat_use = pluTraceNo.meat_use;
#ifdef USE_TRACE_MEATPART
	*meat_part = pluTraceNo.meat_part;	// 부위
#endif    
#ifdef USE_TRACE_MEATGRADE
	*meat_grade = pluTraceNo.meat_grade;	// 등급
#endif
//	if (pluTraceNo.isLot)
//	{
//		*meat_use = 0;
//	}
	*sl_no1 = pluTraceNo.slaughter_place[0];
	*sl_no2 = pluTraceNo.slaughter_place[1];
	*sl_no3 = pluTraceNo.slaughter_place[2];
	if (ind_str1) memcpy((char *)ind_str1, pluTraceNo.indivNo[0], sizeof(pluTraceNo.indivNo[0]));
	if (ind_str2) memcpy((char *)ind_str2, pluTraceNo.indivNo[1], sizeof(pluTraceNo.indivNo[1]));
	if (ind_str3) memcpy((char *)ind_str3, pluTraceNo.indivNo[2], sizeof(pluTraceNo.indivNo[2]));

//sprintf(MsgBuf, "[B1][%s],sl=%d\r\n", ind_str1, *sl_no1);
//MsgOut(MsgBuf);
//sprintf(MsgBuf, "[B2][%s],sl=%d\r\n", ind_str2, *sl_no2);
//MsgOut(MsgBuf);
//sprintf(MsgBuf, "[B3][%s],sl=%d\r\n", ind_str3, *sl_no3);
//MsgOut(MsgBuf);
	*ind_no = individualData_search(ind_str1, 0);
}

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
void TNT_ClearSlaughtIndex(INT16U clearIndex)	// 도축장 삭제시 기억된 도축장 인덱스 제거
{
	INT32U n_addr;
	INT16U i;

	n_addr = NVRAM_CLEARK_LASTSALE;
	n_addr += TNT_TRACENO_TABLE_SIZE;	// 3001*2
	n_addr += TNT_NODE_SLAUGHT_OFFSET;

	for (i = 0; i <	TNT_TRACENO_TABLE_NO; i++)
	{
		if (get_nvram_bparam(n_addr) == clearIndex) set_nvram_bparam(n_addr, 0);	//slaught_place[0]
		n_addr++;
		if (get_nvram_bparam(n_addr) == clearIndex) set_nvram_bparam(n_addr, 0);	//slaught_place[1]
		n_addr++;
		if (get_nvram_bparam(n_addr) == clearIndex) set_nvram_bparam(n_addr, 0);	//slaught_place[2]
		n_addr += (TNT_PLU_INDIVIDUAL_DATA_SIZE - 2);
	}
}
#endif

#ifdef USE_MULTI_TRACE_NO_DBG
void TNT_ReadTraceNoTable(void)
{
	INT32U addr;
	INT16U i;
	PLU_INDIVIDUAL_NODE_DATA pluTraceNo;
	INT16U key;

	//addr = NVRAM_BASE;
	addr = NVRAM_CLEARK_LASTSALE;

	sprintf(MsgBuf, "<<ind_key>>\r\n");
	MsgOut(MsgBuf);
	for (i = 0; i < MAX_TRACE_NO_TABLE_1 + 1; i++)
	{
		key = get_nvram_lparam(addr);
		sprintf(MsgBuf, "  [%02d] %d\r\n", i, key);
		MsgOut(MsgBuf);
		addr += 2;
		Delay_10ms(1);
	}
	sprintf(MsgBuf, "<<ind_str>>\r\n");
	MsgOut(MsgBuf);

	addr = NVRAM_CLEARK_LASTSALE;
	addr += TNT_TRACENO_TABLE_SIZE;
	
	for (i = 0; i < TNT_TRACENO_TABLE_NO; i++)
	{
		get_nvram_sparam(addr, (INT8U *)&pluTraceNo, TNT_PLU_INDIVIDUAL_DATA_SIZE);
		sprintf(MsgBuf, "  [%02d] %s,%s,%s,s%02d,u%02d,p%02d,g%02d\r\n", i, pluTraceNo.indivNo[0], pluTraceNo.indivNo[1], pluTraceNo.indivNo[2], pluTraceNo.slaughter_place[0], pluTraceNo.meat_use, pluTraceNo.meat_part, pluTraceNo.meat_grade);
		MsgOut(MsgBuf);
		addr += TNT_PLU_INDIVIDUAL_DATA_SIZE;
		Delay_10ms(1);
	}
}
#endif

#else	//#ifdef USE_MULTI_TRACE_NO_TABLE

void TNT_SetTraceNoTable_Single(INT8U dept, INT32U pluno, INT8U *ind_no_str, INT8U sl_no, INT8U meat_use, INT8U meat_part, INT8U lot_flag, INT8U meat_grade)
{
	PLU_INDIVIDUAL_DATA plu_indiv;
	INT32U	addr;
	INT16U	tableKey;
	INT8U string_buf[50];
/*
	if (SaveTraceabilityNo == 2) key = 0;	// 0:전체개체(공통영역), 1~3000:상품개체(PLU별 영역)
	else 
	{
		plu_table_search(dept, pluno, &key, 0);
		if (key > 3000) return; // 개체기억기능을 3000개로 제한(메모리 제한 때문)
	}
	addr = NVRAM_CLEARK_LASTSALE;
	addr += key * sizeof(PLU_INDIVIDUAL_DATA);
*/
	
	tableKey = TNT_searchTraceNoKey(dept, pluno, 0);

	if (tableKey > TNT_TRACENO_TABLE_NO)	// 0xfffe : table max(3001) 초과, 0xffff : 해당 PLU 없을 때
	{
		if (tableKey == 0xfffe)
		{
			//sprintf((char *)string_buf, "개체기억공간 부족(1)"); 
			sprintf((char *)string_buf, IndivMsg_TraceMemoryFull);
			display_message_page_mid((char *)string_buf);
		}
		return;
	}
	if (!tableKey )	// 등록된 개체가 없을 때
	{
		tableKey = TNT_getEmptyNode();	// 빈 주소 할당

		if (tableKey > TNT_TRACENO_TABLE_NO)	// 0xffff : 빈 주소 없을 때
		{ 
			//sprintf((char *)string_buf, "개체기억공간 부족(2)");
			sprintf((char *)string_buf, IndivMsg_TraceMemoryFull); 
			display_message_page_mid((char *)string_buf);
			return;
		}
		TNT_writeTraceNoKey(dept, pluno, 0, tableKey);
	}
	addr = NVRAM_CLEARK_LASTSALE;
	addr += TNT_TRACENO_TABLE_SIZE;	// 3001*2
	addr += (tableKey - 1) * TNT_PLU_INDIVIDUAL_DATA_SIZE;


	//isLot = 0;
	//if(strncmp((char *)ind_no_str, "lot", 3) == 0 || strncmp((char *)ind_no_str, "LOT", 3) == 0)
	//{
	//	isLot = 0x80;
	//}
	//if (GlbOper.TraceabilityCallType == 1)	// NH mart
	//{
	//	memcpy(plu_indiv.indivNo, (INT8U *)ind_no_str, sizeof(plu_indiv.indivNo));
	//	set_nvram_sparam(addr, (INT8U*)&plu_indiv, sizeof(PLU_INDIVIDUAL_DATA));
	//}
	//else
	//{
		memset(&plu_indiv, 0x00, TNT_PLU_INDIVIDUAL_DATA_SIZE);
		plu_indiv.slaughter_place = sl_no;
		plu_indiv.meat_use = meat_use;
#ifdef USE_TRACE_MEATPART
		plu_indiv.meat_part = meat_part;	// 부위
#endif        
#ifdef USE_TRACE_MEATGRADE	//등급번호 기억을 위해 마지막bit만 isLot 사용, 상위 7bit는 gradeNo를 저장한다. 
		plu_indiv.isLot &= 0x01;
		plu_indiv.isLot |= meat_grade << 1;
#endif
		plu_indiv.isLot &= 0xfe;
		plu_indiv.isLot |= lot_flag & 0x01;
		if (ind_no_str) memcpy(plu_indiv.indivNo, (INT8U *)ind_no_str, sizeof(plu_indiv.indivNo));
		
	//}

	// 기억할 항목이 없을 경우 해당 table과 data를 삭제함
	if (plu_indiv.meat_use == 0 && plu_indiv.slaughter_place == 0 
#ifdef USE_TRACE_MEATPART
		&& plu_indiv.meat_part == 0
#endif
#ifdef USE_TRACE_MEATGRADE
		&& (plu_indiv.isLot&0xfe) == 0
#endif
		&& plu_indiv.indivNo[0] == 0)
	{
		// table 삭제
		TNT_writeTraceNoKey(dept, pluno, 0, 0);
		// data 삭제
		memset((char *)&plu_indiv, 0, TNT_PLU_INDIVIDUAL_DATA_SIZE);
	}
	set_nvram_sparam(addr, (INT8U*)&plu_indiv, TNT_PLU_INDIVIDUAL_DATA_SIZE);
}

void TNT_GetTraceNoTable_Single(INT8U dept, INT32U pluno, INT16U *ind_no, INT8U *ind_no_str, INT8U *sl_no, INT8U *meat_use, INT8U *meat_part, INT8U *lot_flag, INT8U *meat_grade)
{
	PLU_INDIVIDUAL_DATA plu_indiv;
	INT32U addr;
	INT16U tableKey;

/*
	if (SaveTraceabilityNo == 2) key = 0;	// 0:전체개체(공통영역), 1~3000:상품개체(PLU별 영역)
	else 
	{
		plu_table_search(dept, pluno, &key, 0);
		if (key > 3000) return; // 개체기억기능을 3000개로 제한(메모리 제한 때문)
	}
	addr = NVRAM_CLEARK_LASTSALE;
	addr += key * sizeof(PLU_INDIVIDUAL_DATA);
*/

	tableKey = TNT_searchTraceNoKey(dept, pluno, 0);
	if (tableKey > TNT_TRACENO_TABLE_NO) return;	// 0xffff : table max(3001) 초과 or 해당 PLU 없을 때
	addr = NVRAM_CLEARK_LASTSALE;
	addr += TNT_TRACENO_TABLE_SIZE;	// 3001*2
	addr += (tableKey - 1) * TNT_PLU_INDIVIDUAL_DATA_SIZE;
		
	get_nvram_sparam(addr, (INT8U*)&plu_indiv, TNT_PLU_INDIVIDUAL_DATA_SIZE);

#ifdef USE_TRACE_MEATGRADE	//등급번호 기억을 위해 마지막bit만 isLot 사용, 상위 7bit는 gradeNo를 저장한다. 
	*meat_grade = plu_indiv.isLot >> 1;
	*meat_grade &= 0x7f;
#endif
	*lot_flag = plu_indiv.isLot & 0x01;
	*sl_no = plu_indiv.slaughter_place;
	*meat_use = plu_indiv.meat_use;
#ifdef USE_TRACE_MEATPART
	*meat_part = plu_indiv.meat_part;	// 부위
#endif    
	//if (plu_indiv.isLot)
	//{
	//	*meat_use = 0;
	//}
	memcpy((char *)ind_no_str, plu_indiv.indivNo, sizeof(plu_indiv.indivNo)); 
	*ind_no = individualData_search(ind_no_str, 0);
}

#endif


void	trace_specialuse_create(void)
{
	static INT16U cap_code[2]={0x138A,0x138B};
	menu1_create(0x1381,cap_code,FLASH_TRACE_SPECIALUSE_AREA, sizeof(TRACE_SPECIALUSE_STRUCT), OFF);
}

void    trace_specialuse_delete(void)
{	
	menu1_delete(0x1383, 0x138A, FLASH_TRACE_SPECIALUSE_AREA, sizeof(TRACE_SPECIALUSE_STRUCT));		
}

#ifndef DISABLE_LIST_MENU
void 	trace_specialuse_list(void)
{
	INT16U input_cap[3]={0x1E00,0x1E01,0x1E02};
	display_pgm_list(0x1382, 0x138A, input_cap, 0x71FF, FLASH_TRACE_SPECIALUSE_AREA, sizeof(TRACE_SPECIALUSE_STRUCT));
}
#endif

#ifdef USE_TRACE_MEATPART
void trace_meatpart_create(void)
{
	static INT16U cap_code[2]={0x13AA,0x13AB};
	menu1_create(0x13A1,cap_code,FLASH_TRACE_MEATPART_AREA, sizeof(TRACE_MEATPART_STRUCT), OFF);
}

void trace_meatpart_delete(void)
{	
	menu1_delete(0x13A3, 0x13AA, FLASH_TRACE_MEATPART_AREA, sizeof(TRACE_MEATPART_STRUCT));		
}

#ifndef DISABLE_LIST_MENU
void trace_meatpart_list(void)
{
	INT16U input_cap[3]={0x1E00,0x1E01,0x1E02};
	display_pgm_list(0x13A2, 0x13AA, input_cap, 0x71FF, FLASH_TRACE_MEATPART_AREA, sizeof(TRACE_MEATPART_STRUCT));
}
#endif

#ifdef USE_TRACE_MEATPART_DEFAULT
INT8U traceMeatPartDefault[11][8] = {	"안심"	,	// 1
					"등심"	,	// 2
					"채끝"	,	// 3
					"목심"	,	// 4
					"앞다리",	// 5
					"우둔"	,	// 6
					"설도"	,	// 7
					"양지"	,	// 8
					"사태"	,	// 9
					"갈비"	, 	// 10
					"기타"  	// 11
				};

void trace_meatpart_default_KR(void)
{
	INT16U i;
	INT32U addr;

	for (i = 0; i < 11; i++)
	{
		addr  = sizeof(TRACE_MEATPART_STRUCT) * i;
		addr += DFLASH_BASE + FLASH_TRACE_MEATPART_AREA;
		Flash_wwrite(addr, i + 1);
		Flash_swrite(addr+2, (HUGEDATA INT8U *)traceMeatPartDefault[i], sizeof(TRACE_MEATPART_STRUCT) - 2);
	}
}
#endif
#endif

#ifdef USE_TOPMART_STRUCTURE
void trace_lookup_breed_from_table(INT8U breed_no, char* breed_str, int breed_str_size)
{
	int i;
	breed_str[0] = 0;
	for (i = 0; i < sizeof(breed_type)/sizeof(breed_type[0]); i++)
	{
		if(breed_type[i].index == breed_no) {
			strncpy(breed_str, breed_type[i].contents, breed_str_size);
			breed_str[breed_str_size - 1] = 0;
			break;
		}
	}
}
void trace_lookup_slaughter_from_table(INT16U num, char* slaughter_name, int slaughter_name_size)
{
	TRACE_SLAUGHTHOUSE_STRUCT 	slaughHouseStr;
	INT32U	addr;
	INT16U 	str_size;

	slaughter_name[0] = 0;
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
	num = trace_slaughthouse_no2index(num);
#endif
	str_size = TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE;
	addr = DFLASH_BASE;
	addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
	addr += ((num-1) * str_size);
	Flash_sread(addr, (HUGEDATA INT8U *)&slaughHouseStr, str_size);

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
	if(slaughHouseStr.index == trace_slaughthouse_index2no(num)) {
#else
	if(slaughHouseStr.index == num) {
#endif
		strncpy(slaughter_name, slaughHouseStr.slaughtHouse, slaughter_name_size);
		slaughter_name[slaughter_name_size - 1] = 0;
	}
}
#endif	//#ifdef USE_TOPMART_STRUCTURE

#ifdef USE_TRACE_MEATGRADE

void trace_meatgrade_dataUpdate(INT32U num)
{
	TRACE_MEATGRADE_STRUCT 	meatGradeStr;
	INT32U	addr;
	INT16U  sIndex;
	INT16U  index, tableId;

	addr = DFLASH_BASE;
	addr += FLASH_SUB_DEPARTMENT_AREA;
	addr += ((num - 1) * sizeof(TRACE_MEATGRADE_STRUCT));
	sIndex = Flash_wread(addr);

	if(sIndex == num){
		Flash_sread(addr, (HUGEDATA INT8U *)&meatGradeStr, sizeof(TRACE_MEATGRADE_STRUCT));
	} else {
		memset(&meatGradeStr, 0, sizeof(TRACE_MEATGRADE_STRUCT));
		meatGradeStr.index = num;		
	}

	*Menu[0].pval = meatGradeStr.index;
       	memcpy(Menu[1].pstr, meatGradeStr.meatGrade, Menu[1].length);
	*Menu[2].pval = (INT32U)meatGradeStr.order;

      	MenuRefresh = ON;
}


void trace_meatgrade_create(void)
{
	CAPTION_STRUCT 		cap;
	TRACE_MEATGRADE_STRUCT 	meatGradeStr;
 	INT32U	start_addr, flash_addr;
	int 	result;
	INT32U	cur_id;
	INT8U   grade[32];
	INT32U  input_max;
	INT32U  order;
	
	start_addr = DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA;
	while (1)
	{
		cur_id = 1;

		flash_addr = start_addr + (sizeof(TRACE_MEATGRADE_STRUCT) * (cur_id - 1));
		Flash_sread(flash_addr, (HUGEDATA INT8U *)&meatGradeStr, sizeof(TRACE_MEATGRADE_STRUCT));
		if(meatGradeStr.index != cur_id)
			memset((INT8U*)&meatGradeStr, 0x00, sizeof(TRACE_MEATGRADE_STRUCT));
		meatGradeStr.index = cur_id;

		memcpy(grade, meatGradeStr.meatGrade, sizeof(meatGradeStr.meatGrade));
		grade[sizeof(meatGradeStr.meatGrade)-1] = 0;
		order = meatGradeStr.order;

		Menu_Init();

		caption_split_by_code(0x1221, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x122A, &cap,0);//""
		input_max = cap.input_max;
		caption_adjust(&cap, NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &cur_id, NULL, NULL, NULL);
		Menu_AddEndFunction(1, trace_meatgrade_dataUpdate);

		caption_split_by_code(0x122B, &cap,0);
		caption_adjust(&cap, NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, grade, NULL, NULL);	

		caption_split_by_code(0x122C, &cap,0);
		caption_adjust(&cap, NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &order, NULL, NULL, NULL);	

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			meatGradeStr.index = cur_id;
			strcpy((HUGEDATA char*)meatGradeStr.meatGrade, (HUGEDATA char*)grade);
			meatGradeStr.order = order;
			flash_addr = start_addr + (sizeof(TRACE_MEATGRADE_STRUCT) * (cur_id - 1));
			Flash_swrite(flash_addr, (HUGEDATA INT8U *)&meatGradeStr, sizeof(TRACE_MEATGRADE_STRUCT));
			if(cur_id <input_max) cur_id++;
			else   cur_id = input_max; 
		}
		else if (result == MENU_RET_ESC)
		{
			break;
		}
	}
	trace_meatgrade_getGradeListText();

}

void trace_meatgrade_delete(void)
{	
	menu1_delete(0x1223, 0x122A, FLASH_SUB_DEPARTMENT_AREA, sizeof(TRACE_MEATGRADE_STRUCT));		
}

#ifndef DISABLE_LIST_MENU
void trace_meatgrade_list(void)
{
	INT16U input_cap[3]={0x1E00,0x1E01,0x1E02};
	display_pgm_list(0x1222, 0x122A, input_cap, 0x71FF, FLASH_SUB_DEPARTMENT_AREA, sizeof(TRACE_MEATGRADE_STRUCT));
//trace_meatgrade_default();
}
#endif


#ifdef USE_TRACE_MEATGRADE_EMART_DEFAULT
TRACE_MEATGRADE_STRUCT traceMeatGradeDefault[15] = {	
	{	1,	"1+",	5,	0},	// 1
	{	2,	"1",	6,	0},	// 2
	{	3,	"2",	7,	0},	// 3
	{	4,	"3",	8,	0},	// 4
	{	5,	"등외",	9,	0},	// 5
	{	6,	"없음",	10,	0},	// 6
	{	7,	"1++",	4,	0},	// 7
	{	8,	"1++(7)",	3,	0},	// 8
	{	9,	"1++(8)",	2,	0},	// 9
	{	10,	"1++(9)",	1,	0},	// 10
	{	11,	"1+",	1,	0},	// 11
	{	12,	"1",	2,	0},	// 12
	{	13,	"2",	3,	0}, 	// 13
	{	14,	"등외",	4,	0},	// 14
	{	19,	"없음",	5,	0},	// 15
};

void trace_meatgrade_default(void)
{
	INT16U i;
	INT32U addr;

	Flash_clear(DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA, 0xff, sizeof(TRACE_MEATGRADE_STRUCT)*100);	// clear

	for (i = 0; i < 15; i++)
	{
		addr  = sizeof(TRACE_MEATGRADE_STRUCT) * (traceMeatGradeDefault[i].index - 1);
		addr += DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA;
		Flash_swrite(addr, (HUGEDATA INT8U *)&traceMeatGradeDefault[i], sizeof(TRACE_MEATGRADE_STRUCT));
	}
}
#endif

#ifdef USE_TRACE_MEATGRADE_STANDARD_DEFAULT
  #ifdef USE_GSMART_TRACE_DEFAULT
TRACE_MEATGRADE_STRUCT traceMeatGradeDefault[10] = {
	{	1,	"1++",	1,	0},	// 1	
	{	2,	"1+",	2,	0},	// 2
	{	3,	"1",	3,	0},	// 3
	{	4,	"2",	4,	0},	// 4
	{	5,	"3",	5,	0},	// 5
	{	6,	"등외",	6,	0},	// 6
	{	11,	"1+",	1,	0},	// 7
	{	12,	"1",	2,	0},	// 8
	{	13,	"2",	3,	0}, 	// 9
	{	14,	"등외",	4,	0},	// 10
};
  #else
TRACE_MEATGRADE_STRUCT traceMeatGradeDefault[13] = {
	{	1,	"1++",	4,	0},	// 4
	{	2,	"1+",	5,	0},	// 5
	{	3,	"1",	6,	0},	// 6
	{	4,	"2",	7,	0},	// 7
	{	5,	"3",	8,	0},	// 8
	{	6,	"등외",	9,	0},	// 9
	{	7,	"1++(7)",	3,	0},	// 3	
	{	8,	"1++(8)",	2,	0},	// 2	
	{	9,	"1++(9)",	1,	0},	// 1	
	{	11,	"1+",	1,	0},	// 10
	{	12,	"1",	2,	0},	// 11
	{	13,	"2",	3,	0}, // 12
	{	14,	"등외",	4,	0},	// 13
};
  #endif
void trace_meatgrade_default(void)
{
	INT16U i;
	INT32U addr;

	Flash_clear(DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA, 0xff, sizeof(TRACE_MEATGRADE_STRUCT)*100);	// clear
  #ifdef USE_GSMART_TRACE_DEFAULT
	for (i = 0; i < 10; i++)
  #else		
	for (i = 0; i < 13; i++)
  #endif
	{
		addr  = sizeof(TRACE_MEATGRADE_STRUCT) * (traceMeatGradeDefault[i].index - 1);
		addr += DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA;
		Flash_swrite(addr, ( INT8U *)&traceMeatGradeDefault[i], sizeof(TRACE_MEATGRADE_STRUCT));
	}
}
#endif
#ifdef USE_TRACE_MEATGRADE_MEATPRICE_DEFAULT   // USE 축산공시 전용 등급 사용 
#define MEAT_GRADE_MAX	18
TRACE_MEATGRADE_STRUCT traceMeatGradeDefault[MEAT_GRADE_MAX] = {
	{	1,	"1++",	4,	0},	// 4	
	{	2,	"1+",	5,	0},	// 5
	{	3,	"1",	6,	0},	// 6
	{	4,	"2",	7,	0},	// 7
	{	5,	"3",	8,	0},	// 8
	{	6,	"등외",	9,	0},	// 9
	{	7,	"1++(7)",	3,	0},	// 3	
	{	8,	"1++(8)",	2,	0},	// 2	
	{	9,	"1++(9)",	1,	0},	// 1	
	{	11,	"1+",	1,	0},	// 7
	{	12,	"1",	2,	0},	// 8
	{	13,	"2",	3,	0}, 	// 9
	{	14,	"등외",	4,	0},	// 10
	{	81,	"소고기-미국산",	1,	0},	// 11
	{	82,	"소고기-호주산",	2,	0},	// 12
	{	91,	"돼지고기-미국산",	1,	0},	// 13
	{	92,	"돼지고기-독일산",	2,	0},	// 14
	{	93,	"돼지고기-스페인산",	3,	0},	// 15
};

void trace_meatgrade_default(void)
{
	INT16U i;
	INT32U addr;

	Flash_clear(DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA, 0xff, sizeof(TRACE_MEATGRADE_STRUCT)*100);	// clear

	for (i = 0; i < MEAT_GRADE_MAX; i++)
	{
		addr  = sizeof(TRACE_MEATGRADE_STRUCT) * (traceMeatGradeDefault[i].index - 1);
		addr += DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA;
		Flash_swrite(addr, ( INT8U *)&traceMeatGradeDefault[i], sizeof(TRACE_MEATGRADE_STRUCT));
	}
}
#endif // USE_TRACE_MEATGRADE_MEATPRICE_DEFAULT   // USE 축산공시 전용 등급 사용 
void trace_meatgrade_call_back(long num)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	TRACE_MEATGRADE_STRUCT 	meatGradeStr;
	INT8U 	buffer[48];
	INT32U	addr;
	INT16U 	str_size;
	INT16U	w;

#ifdef USE_CL5200_DISPLAY_DEFAULT
	CallbackEditPosition_y = 0;
	CallbackEditPosition_x = 12;
#endif	

	if(num == 0) return;
		
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)buffer, 0x20, 48);

	w = Dsp_GetChars();
	buffer[w-1] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	caption_split_by_code(0x122A, &cap,0);	 //등급 #:
	caption_adjust(&cap, NULL);

	str_size = sizeof(TRACE_MEATGRADE_STRUCT);
	addr = DFLASH_BASE;
	addr += FLASH_SUB_DEPARTMENT_AREA;
	addr += ((num-1) * str_size);
	Flash_sread(addr, (HUGEDATA INT8U *)&meatGradeStr, str_size);

	if(meatGradeStr.index != num) {
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		return;	
	} else {
		memcpy((INT8U*)buffer, meatGradeStr.meatGrade, sizeof(meatGradeStr.meatGrade));
		buffer[sizeof(meatGradeStr.meatGrade)]=0;
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	}
}

void trace_meatgrade_getGradeText(INT8U gradeNo, INT8U *gradeText)
{
	INT32U addr;
	INT16U id;
#ifdef USE_DISCLOSURE_MEATGRADE
	if(gradeNo > 80)
	{
		memset(gradeText, 0, INDIV_GRADE_LEN);	
		return;
	}
#endif

	addr = DFLASH_BASE;
	addr += FLASH_SUB_DEPARTMENT_AREA;
	addr += ((gradeNo - 1) * sizeof(TRACE_MEATGRADE_STRUCT));
	id = Flash_wread(addr);
	if (id == gradeNo)
	{
		Flash_sread(addr + 2, (HUGEDATA INT8U *)gradeText, INDIV_GRADE_LEN);
		gradeText[INDIV_GRADE_LEN - 1] = 0;
	}
	else
	{
		memset(gradeText, 0, INDIV_GRADE_LEN);
	}
}

INT8U TraceMeatGradeListText[2][64];	// 0:Beef, 1:Pork
INT8U ExterPrt_TraceMeatGradeListText[2][64];	//외부 PRT 등급 리스트 // 0:Beef, 1:Pork
INT8U ExterPrt_TraceMeatGradeList_Len[2][10];	//외부 PRT 등급 리스트 len // 0:Beef, 1:Pork
#if defined(USE_EMART_MEATGRADE_PRINT) || defined(USE_MARBLING_GRADE_PRINT)
INT8U TraceMeatGradeList_Position[2][10];
INT8U TraceMeatGradeList_Len[2][10];
#endif
INT16U TraceMeatGradeMaxQty;
INT8U TraceMeatGradeListNum[100];

/*
#ifdef USE_EMART_MEATGRADE_PRINT
void trace_meatgrade_getGradeListText(void)
{
	INT32U addr;
	INT32U base_addr;
	INT8U type;
	INT8U gradeBase;
	INT8U grade_index;
	INT8U grade_order;
	INT8U gradeText[28];
	INT8U i;
	INT8U j;
	INT8U max_len;
	INT8U len;
	INT8U qty;

	base_addr = DFLASH_BASE;
	base_addr += FLASH_SUB_DEPARTMENT_AREA;
	gradeBase = 0;
	max_len = 0;
	TraceMeatGradeMaxQty = 0;
	memset(&TraceMeatGradeListNum[0], 0, sizeof(TraceMeatGradeListNum));
	for (type = 0; type < 2; type++)	// 0:Beef, 1:Pork
	{
		qty = 0;
		sprintf((char*)&TraceMeatGradeListText[type][0], "등급: ");
		len = strlen((char*)&TraceMeatGradeListText[type][0]);

		for (i = 1; i <= 10; i++)
		{
			TraceMeatGradeList_Position[type][i - 1] = len - 1;
			for (j = 0; j < 10; j++)
			{

				addr = base_addr + ((gradeBase + j) * sizeof(TRACE_MEATGRADE_STRUCT));
				grade_index = Flash_bread(addr);
				grade_order = Flash_bread(addr + TRACE_MEATGRADE_ORDER_OFFSET);
				if ((grade_index == gradeBase + j + 1) && grade_order == i)	// 1,2,3,4,5,6,7,8,9 순서로 search
				{
					Flash_sread(addr + 2, gradeText, sizeof(gradeText));

					if (strcmp((char*)gradeText, "없음") == 0) continue;	// "없음"은 리스트에 제외
					gradeText[sizeof(gradeText) - 1] = 0;
					TraceMeatGradeList_Len[type][i - 1] = strlen(gradeText);
					sprintf((char*)&TraceMeatGradeListText[type][len], "%s,", gradeText);
					len += (TraceMeatGradeList_Len[type][i - 1] + 1);

					TraceMeatGradeListNum[gradeBase + j] = qty + 1;
					qty++;
				}
			}
		}
		TraceMeatGradeListText[type][len-1] = ' ';	// last one space(',' -> ' ')
		TraceMeatGradeListText[type][len] = 0;
		if (max_len < len) max_len = len;
		if (TraceMeatGradeMaxQty < qty) TraceMeatGradeMaxQty = qty;
		gradeBase += 10;
	}
	str_fill((char*)&TraceMeatGradeListText[0][0], max_len);	// beef
	str_fill((char*)&TraceMeatGradeListText[1][0], max_len);	// pork
}
*/
//#else

void trace_meatgrade_getGradeListText(void)
{
	INT32U addr;
	INT32U base_addr;
	INT8U type;
	INT8U gradeBase;
	INT8U grade_index;
	INT8U grade_order;
	INT8U gradeText[28];
	INT8U i;
	INT8U j;
	INT8U max_len;
	INT8U len;
	INT8U qty;
	//외부Prt 등급 표시에 사용
	INT8U exterPrt_gradeText[28];
	INT8U exterPrt_len;
	INT8U exterPrt_gradeText_len;

	base_addr = DFLASH_BASE;
	base_addr += FLASH_SUB_DEPARTMENT_AREA;
	gradeBase = 0;
	max_len = 0;
	TraceMeatGradeMaxQty = 0;
	memset(&TraceMeatGradeListNum[0], 0, sizeof(TraceMeatGradeListNum));
	for (type = 0; type < 2; type++)	// 0:Beef, 1:Pork
	{
		qty = 0;
#ifdef USE_MEATGRADE_NO_TEXT				
		len = 0;
#else
		sprintf((char*)&TraceMeatGradeListText[type][0], "등급: ");
		len = strlen((char*)&TraceMeatGradeListText[type][0]);
		
		//외부Prt 등급 표시
  		exterPrt_len = 0;
  		sprintf((char*)&ExterPrt_TraceMeatGradeListText[type][0], "등급: ");
		exterPrt_len = strlen((char*)&ExterPrt_TraceMeatGradeListText[type][0]);
#endif
		for (i = 1; i <= 10; i++)
		{
#if defined(USE_EMART_MEATGRADE_PRINT) || defined(USE_MARBLING_GRADE_PRINT)
			TraceMeatGradeList_Position[type][i - 1] = len - 1;
#endif
//sprintf(MsgBuf, "\r\n[order=%d]", i);
//MsgOut(MsgBuf);
			for (j = 0; j < 10; j++)
			{
//sprintf(MsgBuf, "(no=%d)", gradeBase + j);
//MsgOut(MsgBuf);
				addr = base_addr + ((gradeBase + j) * sizeof(TRACE_MEATGRADE_STRUCT));
				grade_index = Flash_bread(addr);
				grade_order = Flash_bread(addr + TRACE_MEATGRADE_ORDER_OFFSET);
				if ((grade_index == gradeBase + j + 1) && grade_order == i)	// 1,2,3,4,5,6,7,8,9 순서로 search
				{
					Flash_sread(addr + 2, gradeText, sizeof(gradeText));
					Flash_sread(addr + 2, exterPrt_gradeText, sizeof(exterPrt_gradeText));	//외부Prt 등급 표시
//sprintf(MsgBuf, "\r\n(%02d)%s\r\n", grade_index, gradeText);
//MsgOut(MsgBuf);
#ifdef USE_EXPAND_MARBLING_LIST
					strcat((char*)gradeText, "  ");	// 등급 간에 간격 벌려줌.
#endif
					if (strcmp((char*)gradeText, "없음") == 0) continue;	// "없음"은 리스트에 제외
					gradeText[sizeof(gradeText) - 1] = 0;

					//외부Prt 등급 표시
					exterPrt_gradeText_len = strlen(exterPrt_gradeText);

  					if ((strcmp(exterPrt_gradeText, "등외") == 0) && (type == 0))	//외부Prt 사용시 등급 표시와 위치 맞춤-"소-등외"시 한 칸 앞으로
						exterPrt_len++;

					strcat((char*)exterPrt_gradeText, "   ");	// 등급 간에 간격 벌려줌.
					if(exterPrt_gradeText_len == 1)		//등급 1, 2, 3: 한 자리 등급은 문자 뒤 공백 2칸
						exterPrt_gradeText[2] = 0;

                    else if(exterPrt_gradeText_len == 2)	//등급 1+ : 문자 뒤 공백 조정
                    {
                        if(type ==1)	//Pork
							exterPrt_gradeText[3] = 0;
                        else if(type ==0)	//Beef
                        	exterPrt_gradeText[2] = 0;
                    }                    
					else	//등급 1++(7), 1++(8), 1++(9), 1++( ): 문자 뒤 공백 조정
						exterPrt_gradeText[6] = 0; 	
                                 
#if defined(USE_EMART_MEATGRADE_PRINT) || defined(USE_MARBLING_GRADE_PRINT)
					TraceMeatGradeList_Len[type][i - 1] = strlen(gradeText);
#ifdef USE_EXPAND_MARBLING_LIST
					sprintf((char*)&TraceMeatGradeListText[type][len], "%s ,", gradeText);
#else
					sprintf((char*)&TraceMeatGradeListText[type][len], "%s,", gradeText);
#endif
					len += (TraceMeatGradeList_Len[type][i - 1] + 1);

  					//외부Prt 등급 표시
  					ExterPrt_TraceMeatGradeList_Len[type][i - 1] = strlen(exterPrt_gradeText);
			 		if(exterPrt_gradeText_len == 1)	//등급 1, 2, 3: 한 자리 등급은 문자 앞 공백 추가-등급 표시와 가운데 맞춤
					{
						sprintf((char*)&ExterPrt_TraceMeatGradeListText[type][exterPrt_len], " %s ", exterPrt_gradeText);
						exterPrt_len += (ExterPrt_TraceMeatGradeList_Len[type][i - 1] + 1);
					}
					else
					{
						sprintf((char*)&ExterPrt_TraceMeatGradeListText[type][exterPrt_len], "%s ", exterPrt_gradeText);
						exterPrt_len += (ExterPrt_TraceMeatGradeList_Len[type][i - 1]);
					}	
//sprintf(MsgBuf, "\r\n(%02d,%2d)%s\r\n", grade_index, TraceMeatGradeList_Len[type][i - 1], gradeText);
//MsgOut(MsgBuf);
#else
					if(strlen((char*)gradeText) < 3)
					{
						sprintf((char*)&TraceMeatGradeListText[type][len], "  %-4s", gradeText);
					}
					else
					{
						sprintf((char*)&TraceMeatGradeListText[type][len], " %-5s", gradeText);
					}
					len += 6;
#endif
					TraceMeatGradeListNum[gradeBase + j] = qty + 1;
					qty++;
				}
			}
		}
#if defined(USE_EMART_MEATGRADE_PRINT) || defined(USE_MARBLING_GRADE_PRINT)
		TraceMeatGradeListText[type][len-1] = ' ';	// last one space(',' -> ' ')
#endif
		TraceMeatGradeListText[type][len] = 0;
		ExterPrt_TraceMeatGradeListText[type][exterPrt_len-1] = 0;	// last one space(',' -> ' ')	//외부Prt 등급 표시

		if (max_len < len) max_len = len;
		if (TraceMeatGradeMaxQty < qty) TraceMeatGradeMaxQty = qty;
		gradeBase += 10;
	}
	str_fill((char*)&TraceMeatGradeListText[0][0], max_len);	// beef
	str_fill((char*)&TraceMeatGradeListText[1][0], max_len);	// pork
	//외부Prt 등급 표시
  	str_fill((char*)&ExterPrt_TraceMeatGradeListText[0][0], exterPrt_len);	// beef	
	str_fill((char*)&ExterPrt_TraceMeatGradeListText[1][0], exterPrt_len);	// pork
//sprintf(MsgBuf, "%s\r\n", &TraceMeatGradeListText[0][0]);
//MsgOut(MsgBuf);
//sprintf(MsgBuf, "%s\r\n", &TraceMeatGradeListText[1][0]);
//MsgOut(MsgBuf);
}
//#endif

#ifdef USE_TRACE_CALL_MEATGRADE_KEY
void	trace_meatgrade_callGradeNo(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	result, old_font, key_mode;
	INT32U	index;
#ifndef USE_DISCLOSURE_MEATGRADE
	if (!plu_check_inhibit_ptype(PTYPE_GROUP_NO)) return;	// PLU 등급을 사용할 때, 등급호출 불가 
#endif
	key_mode = KEY_GetMode();
	KEY_SetMode(1);
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	index = CurTraceStatus.gradeNo;
	while(1)
	{      
		Menu_Init();

		caption_split_by_code(0x1221, &cap,0);	 //Title
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x122A, &cap,0);	 //등급 #:
		cap.input_min = 0;	// 0입력 가능(취소 기능)
		caption_adjust(&cap,NULL);

		Menu_InputCreate(M_ID_TRS_INDEX, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&index, NULL, NULL, trace_meatgrade_call_back);
		Menu_SetCallbackMode(M_ID_TRS_INDEX, 1);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			CurTraceStatus.gradeNo = (INT8U)index;
			trace_meatgrade_getGradeText(CurTraceStatus.gradeNo, CurTraceStatus.curGradeText);
			sale_display_indivFlag_set(INDIV_DISP_MODE_MEATGRADE, ON);
			sale_display_individual();
		#ifdef USE_MULTI_TRACE_NO_TABLE
			TNT_SetTraceNoTable((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, NULL, NULL, NULL, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, CurTraceStatus.gradeNo);
		#else
			TNT_SetTraceNoTable_Single((INT8U)status_scale.Plu.deptid, status_scale.cur_pluid, CurTraceStatus.indivStr.individualNO, CurTraceStatus.slPlace, CurTraceStatus.meatUse, CurTraceStatus.meatPart, CurTraceStatus.indivStr.lotFlag, CurTraceStatus.gradeNo);
		#endif
			break;
		} else if(result ==MENU_RET_ESC) break;
	}

	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff); 	
}
#endif

#endif


#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT

TABLE_INDEX_STRUCT SlaughtHouseTable[MAX_SLAUGHTERHOUSE_KR_QTY];
INT16U SlaughtTableLastId;


INT8U trace_slaughthouse_search_index(INT16U *num, INT16U *tableId, INT16U *index, INT8U nextSearch)
{
	INT16S right;
	INT16S left;
	INT16S middle;

	if (SlaughtTableLastId == 0)
	{
		*tableId = 0;
		*index = SlaughtHouseTable[0].index;
		return 0;
	}
	right = SlaughtTableLastId - 1;
	left = 0;

	while (right >= left)
	{
		middle = (right + left) / 2;

//sprintf(MsgBuf, "(%d) %4d, l=%3ld, r=%3ld, m=%3ld\r\n", *num, SlaughtHouseTable[middle].no, left, right, middle);
//MsgOut(MsgBuf);
		if (*num > SlaughtHouseTable[middle].no)
		{
			left = middle + 1;
		}
		else if (*num < SlaughtHouseTable[middle].no)
		{
			right = middle - 1;
		}		
		else //if (*num == deptPluNo)
		{
			*tableId = middle;
			*index = SlaughtHouseTable[middle].index;
			return 1;
		}
	}

	if (left > SlaughtTableLastId - 1)
	{
		*tableId = SlaughtTableLastId;
	}
	else
	{
		*tableId = left;
	}

	*index = 0;
	if (nextSearch)
	{
		if (*tableId < SlaughtTableLastId)
		{
			*index = SlaughtHouseTable[*tableId].index;
			*num = SlaughtHouseTable[*tableId].no;
//sprintf(MsgBuf, "  [%d %4d, lid=%d, tid=%3d]\r\n", *num, SlaughtHouseTable[*tableId].no, SlaughtTableLastId, *tableId);
//MsgOut(MsgBuf);
			return 1;
		}
	}
	return 0;
	
}

INT16U trace_slaughthouse_no2index(INT16U num)
{
	INT16S right;
	INT16S left;
	INT16S middle;

	if (SlaughtTableLastId == 0)
	{
		return 0;
	}
	right = SlaughtTableLastId - 1;
	left = 0;

	while (right >= left)
	{
		middle = (right + left) / 2;

		if (num > SlaughtHouseTable[middle].no)
		{
			left = middle + 1;
		}
		else if (num < SlaughtHouseTable[middle].no)
		{
			right = middle - 1;
		}		
		else //if (*num == deptPluNo)
		{
			return SlaughtHouseTable[middle].index;
		}
	}
	return 0;
	
}


INT16U trace_slaughthouse_index2no(INT16U index)
{
	INT16S i;

	for (i = 0; i < SlaughtTableLastId; i++)
	{
		if (SlaughtHouseTable[i].index == index)
		{
			return SlaughtHouseTable[i].no;
		}
	}
	return 0;
	
}

void trace_slaughthousedata_sort(void)	// bubble sort
{
	TABLE_INDEX_STRUCT tempStr;
	int i, j;

	for (i = MAX_SLAUGHTERHOUSE_KR_QTY - 1; i > 0; i--)
	{
		for (j = 0; j < i; j++) 
		{
			if (SlaughtHouseTable[j].no > SlaughtHouseTable[j + 1].no)
			{
				tempStr = SlaughtHouseTable[j];
				SlaughtHouseTable[j] = SlaughtHouseTable[j + 1];
				SlaughtHouseTable[j + 1] = tempStr;
			}
		}
	}

}

void trace_slaughthousedata_init(void)
{
	INT16U rcode;
	INT16U i;
	INT32U addr;

	SlaughtTableLastId = 0;

	addr = DFLASH_BASE;
	addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
	for (i = 0; i < MAX_SLAUGHTERHOUSE_KR_QTY; i++) 
	{
		rcode = Flash_wread(addr);
		if ((rcode > 0) && (rcode < MAX_SLAUGHTERHOUSE_KR_NO)) 
		{
			SlaughtTableLastId++;
		}
		else
		{
			rcode = 0xffff;
		}
		SlaughtHouseTable[i].no = rcode;
		SlaughtHouseTable[i].index  = i + 1;
		addr += TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE;
	}
	trace_slaughthousedata_sort();
}

INT8U trace_slaughthousedata_read(INT16U slaughtNo, TRACE_SLAUGHTHOUSE_STRUCT *slaughtHouseStr)
{
	INT8U being;
	INT16U tableId, index;
	INT32U addr;

	being = trace_slaughthouse_search_index(&slaughtNo, &tableId, &index, OFF);

	if (being == 0)
	{
		memset(slaughtHouseStr, 0, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
		return 0;
	}

	addr = DFLASH_BASE;
	addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
	addr += ((index - 1) * TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);

	Flash_sread(addr, (HUGEDATA INT8U *)slaughtHouseStr, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
	return 1;
}

/*
INT16U trace_slaughthousedata_get_index(INT16U slaughtNo, INT16U *index)
{
	INT8U being;
	INT16U tableId;

	being = trace_slaughthouse_search_index(&slaughtNo, &tableId, index, OFF);

	if (being == 0)
	{
		return 0;
	}
	return 1;
}
*/

INT8U trace_slaughthousedata_insert(INT16U slaughtNo, INT8U *slaughtName)
{
	TRACE_SLAUGHTHOUSE_STRUCT slaughtHouseStr;
	INT8U being;
	INT16U tableId, index;
	INT16U i;
	INT32U addr;
	INT16U newIndex;

	being = trace_slaughthouse_search_index(&slaughtNo, &tableId, &index, OFF);

	if (being == 0)
	{
		if (SlaughtTableLastId >= MAX_SLAUGHTERHOUSE_KR_QTY) return 0;
		newIndex = SlaughtHouseTable[SlaughtTableLastId].index;
		for (i = SlaughtTableLastId; i > tableId; i--)
		{
			SlaughtHouseTable[i] = SlaughtHouseTable[i - 1];
		}
		SlaughtHouseTable[tableId].no = slaughtNo;
		SlaughtHouseTable[tableId].index = newIndex;
		SlaughtTableLastId++;
		index = SlaughtHouseTable[tableId].index;
	}

	addr = DFLASH_BASE;
	addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
	addr += ((index - 1) * TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);

	slaughtHouseStr.index = slaughtNo;
	memcpy(slaughtHouseStr.slaughtHouse, slaughtName, sizeof(slaughtHouseStr.slaughtHouse));

	Flash_swrite(addr, (HUGEDATA INT8U *)&slaughtHouseStr, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
	return 1;
}

void trace_slaughthousedata_delete(INT16U slaughtNo)
{
	TRACE_SLAUGHTHOUSE_STRUCT slaughtHouseStr;
	INT8U being;
	INT16U tableId, index;
	INT16U i;
	INT32U addr;
	INT16U newIndex;

//sprintf(MsgBuf, "[1](%d)\r\n", slaughtNo);
//MsgOut(MsgBuf);

	being = trace_slaughthouse_search_index(&slaughtNo, &tableId, &index, OFF);

//sprintf(MsgBuf, "[2](%d)tableId=%d, index=%d, being=%d\r\n", slaughtNo, tableId, index, being);
//MsgOut(MsgBuf);
	if (being)
	{
		newIndex = SlaughtHouseTable[tableId].index;
		for (i = tableId; i < SlaughtTableLastId - 1; i++)
		{
			SlaughtHouseTable[i] = SlaughtHouseTable[i + 1];
		}
		SlaughtTableLastId--;
		SlaughtHouseTable[SlaughtTableLastId].no = 0xffff;
		SlaughtHouseTable[SlaughtTableLastId].index = newIndex;

		addr = DFLASH_BASE;
		addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
		addr += ((index - 1) * TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);

		//memset(&slaughtHouseStr, 0, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
		//slaughtHouseStr.index = 0xffff;

//sprintf(MsgBuf, "[3](%d)tableId=%d, index=%d, being=%d, addr=%ld\r\n", slaughtNo, tableId, index, being, addr);
//MsgOut(MsgBuf);
		Flash_clear(addr, 0xff, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
		TNT_ClearSlaughtIndex(index);
	}
}

void trace_slaughthouse_dataUpdate(INT32U num)
{
	TRACE_SLAUGHTHOUSE_STRUCT  slaughHouseStr;
	INT32U	addr;
	INT16U  sIndex;
	INT16U  index, tableId;
	INT8U   being;

	being = trace_slaughthouse_search_index((INT16U *)&num, &tableId, &index, OFF);
	if (being)
	{
		addr = DFLASH_BASE;
		addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
		addr += ((index - 1) * TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
		sIndex = Flash_wread(addr);
	}
	else
	{
		sIndex = 0;
	}
	if(sIndex == num){
		Flash_sread(addr, (HUGEDATA INT8U *)&slaughHouseStr, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
	} else {
		memset(&slaughHouseStr, 0, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
		slaughHouseStr.index = num;		
	}

	*Menu[0].pval = slaughHouseStr.index;
       	memcpy(Menu[1].pstr, slaughHouseStr.slaughtHouse, Menu[1].length);

      	MenuRefresh = ON;
}

	
void	trace_slaughthouse_create(void)
{
	CAPTION_STRUCT 		cap;
	//MENU_TYPE1_STRUCT	menu_type1_str;
	TRACE_SLAUGHTHOUSE_STRUCT 	slaughHouseStr;
 	INT32U	start_addr;
	int 	result;
	INT32U	cur_id;
	INT8U   s_house[TRACE_SLAUGHTHOUSE_NAME_SIZE];
	INT16U  properties;
	INT32U  id_max, id_min;
	
	properties = 0;
	properties |= MENU_P_USE_LINEFEED;
	properties |= MENU_P_CHECK_BLANKS;

	start_addr = DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
	//caption_split_by_code(0x139A, &cap,0);
	id_max = 9999;//cap.input_max;
	id_min = 1;//cap.input_min;
	while (1)
	{
		/*
		i = 0;	
		while(1)  
		{
			flash_addr = start_addr + (TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * i);
			slaughHouseStr.index = Flash_wread(flash_addr);
			if (slaughHouseStr.index == i + 1) 
			{
				i++;
				if (i >= id_max)  
				{
					cur_id = id_min;
					break;
				}
			}  
			else  
			{
				cur_id = i + 1;
		       		break;
			}
		}//while(1)
		*/
		cur_id = 1;

		trace_slaughthousedata_read(cur_id, &slaughHouseStr);

		//flash_addr = start_addr + (TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (cur_id - 1));
		//Flash_sread(flash_addr, (HUGEDATA INT8U *)&slaughHouseStr, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
		if(slaughHouseStr.index != cur_id)
			memset((INT8U*)&slaughHouseStr, 0x00, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
		slaughHouseStr.index = cur_id;

		memcpy(s_house, slaughHouseStr.slaughtHouse, TRACE_SLAUGHTHOUSE_NAME_SIZE);
		s_house[TRACE_SLAUGHTHOUSE_NAME_SIZE-1] = 0;

		Menu_Init();

		caption_split_by_code(0x1391, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x139A, &cap,0);//""
		cap.input_length = 4;
		cap.input_dlength = 4;
		cap.input_max = 9999;
		cap.input_min = 1;
		caption_adjust(&cap, NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							id_max, id_min, 0, &cur_id, NULL, NULL, NULL);
		Menu_AddEndFunction(1, trace_slaughthouse_dataUpdate);


		caption_split_by_code(0x139B, &cap,0);
		cap.input_length = TRACE_SLAUGHTHOUSE_NAME_SIZE - 1;
		cap.input_dlength = TRACE_SLAUGHTHOUSE_NAME_SIZE - 1;
		caption_adjust(&cap, NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, s_house, NULL, NULL);	
		Menu_SetProperties(3, properties);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			//slaughHouseStr.index = cur_id;
			//memcpy(slaughHouseStr.slaughtHouse, s_house, TRACE_SLAUGHTHOUSE_NAME_SIZE);

			if (!trace_slaughthousedata_insert(cur_id, s_house)) BuzOn(2);	// memory full
		}
		else if (result == MENU_RET_ESC)
		{
			break;
		}
	}
}

void    trace_slaughthouse_delete(void)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	INT32U	Flash_addr=0;
	long 	ret_long=1;
	int  	result, current_num;


	Dsp_Fill_Buffer(0);	

	menu_displaytitle(0x1393,0);
	menu_writepage(1, 1);
    	
//	addr = caption_search_by_code(0x141A);
//	_memcpy((INT8U*)&cap,(INT8U *)addr,sizeof(cap));
	caption_split_by_code(0x139A, &cap,0);
	cap.input_length = 4;
	cap.input_dlength = 4;
	cap.input_max = 9999;
	cap.input_min = 1;
	caption_adjust(&cap,NULL);
	disp_p = point(Startup.menu_type*Startup.menu_ygap,0);
	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	Dsp_Diffuse();

REN1:	DspStruct.DirectDraw = 1;   
	result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_length, 
			   				Startup.menu_type*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );   
	switch (result) 
	{			
		case KP_ENTER:
			if ((ret_long > cap.input_max) || (ret_long < cap.input_min)) 
			{
				BuzOn(2);	
				goto REN1;	
			}
			current_num = ret_long;	  	
			break;
		case KP_ESC:	 
			goto RET_ESC; 
	     	default:	
	     		BuzOn(2);	
			goto REN1;		     	 	
	}

	trace_slaughthousedata_delete(current_num);

RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
 	return;
}

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT_DBG
void trace_slaughthouse_debug(void)
{
	INT16U i;
	TRACE_SLAUGHTHOUSE_STRUCT  slaughHouseStr;
	INT32U	addr;
	INT16U  sIndex;
	INT16U  index, tableId;
	INT8U   being;

	sprintf(MsgBuf, "<Sort Table>********************************\r\n");
	MsgOut(MsgBuf);

	sprintf(MsgBuf, "lastId = %d\r\n", SlaughtTableLastId);
	MsgOut(MsgBuf);
	for(i = 0; i < 202; i++) {
		sprintf(MsgBuf, "%3d,No:%04d,Id:%3d\r\n", i, SlaughtHouseTable[i].no, SlaughtHouseTable[i].index);
		MsgOut(MsgBuf);
		if (!(i%10)) Delay_10ms(3);
	}

/*
	sprintf(MsgBuf, "\r\n<Sort Table Name(search)>\r\n");
	MsgOut(MsgBuf);

	for(i = 0; i < 100; i++) {
		being = trace_slaughthouse_search_index(i, &tableId, &index);
		if (being)
		{
			addr = DFLASH_BASE;
			addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
			addr += ((index - 1) * TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
			Flash_sread(addr, (HUGEDATA INT8U *)&slaughHouseStr, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
		}
		else
		{
			memset(&slaughHouseStr, 0, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
			slaughHouseStr.index = index;
		}

		sprintf(MsgBuf, "[%3d,%04d,%s]\r\n", i, slaughHouseStr.index, slaughHouseStr.slaughtHouse);
		MsgOut(MsgBuf);
		if (!(i%10)) Delay_10ms(2);
	}
*/
	sprintf(MsgBuf, "\r\n<Sort Table Name>\r\n");
	MsgOut(MsgBuf);

	for(i = 0; i < 202; i++) {
		addr = DFLASH_BASE;
		addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
		addr += ((i) * TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
		Flash_sread(addr, (HUGEDATA INT8U *)&slaughHouseStr, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);

		sprintf(MsgBuf, "<%3d,%04d,%s>\r\n", i, slaughHouseStr.index, slaughHouseStr.slaughtHouse);
		MsgOut(MsgBuf);
		if (!(i%10)) Delay_10ms(2);
	}

}
#endif

#ifndef DISABLE_LIST_MENU
void  trace_slaughthouse_list(void)
{		
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	TRACE_SLAUGHTHOUSE_STRUCT slaughHouseStr;
	INT16U dlength;
	char   	string_buf[34], end_flag = 0;
	long 	ret_long, i;
	int	start_num, end_num, cur_id, reverse_num;
	int	j = 0, result, min_num, max_num;
	int	line_ct, lcd_size, start_id;
	INT16U	read_code;
	INT32U	addr, addr_flash;
	INT8U	linenum;
	INT16U  index;

#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT_DBG
	trace_slaughthouse_debug();
#endif

LIST_START:
	caption_split_by_code(0x139A, &cap,0);
	min_num	= 1;					//
	max_num = 9999; 					// 
	dlength = 4;
	if(min_num <= 0)  {
		disp_p = point(0,0);
		message_error(disp_p);
	}
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1392,1,1);
	linenum = Startup.line_max;
	if (linenum > 3) linenum = 3;
	for(i = 0;i<linenum; i++)  {
		caption_split_by_code(0x1E00+i, &cap,0);
		cap.input_length=dlength;
		cap.input_dlength=dlength;
		cap.input_min=min_num;
		cap.input_max=max_num;
		if(i != 2) caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	}
	Dsp_Diffuse();
	start_num = min_num;
	end_num   = max_num;
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
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long,cap.input_dlength, 
  				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
		switch (result){
			case KP_ARROW_UP:  if(i > 0)	i--;
					   else 	BuzOn(2);
					   goto REN1;    					   
					   // ]
			case KP_ESC:	   goto RET_ESC;
			case KP_SAVE:	   start_num = min_num;
					   end_num = max_num;
					   i++;
					   break;
			case KP_PAGE_DN:
			case KP_ARROW_DN:
			case KP_ENTER:	   
					   if ( ret_long > max_num || ret_long < min_num ) 
					   {
						error_display_form1(0, min_num, max_num);
						goto REN1;	
				           }
					   if(i == 0)	start_num = ret_long;
					   if(i == 1)	end_num = ret_long;
					   break;
					   				  				
		     	default:	BuzOn(2);	goto REN1;		     	 	
		} //switch (result)
	}//for(i=1; i<3; i++)

	lcd_size   = Dsp_GetChars();
	line_ct    = Startup.line_max; // (Startup.menu_type?3:2);
	addr_flash = DFLASH_BASE + FLASH_TRACE_SLAUGHTERHOUSE_AREA;
	cur_id     = 0;
	start_id   = 0;

READ_START:
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FF, &cap,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0);

		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	}

	j = 0;
	while (1)
	{
//sprintf(MsgBuf, "(%4d,%3d) sno=%3d, eno=%3d, cid=%3d\r\n", SlaughtHouseTable[cur_id].no, SlaughtHouseTable[cur_id].index, start_num, end_num, cur_id);
//MsgOut(MsgBuf);		
		if (SlaughtHouseTable[cur_id].no >= start_num && SlaughtHouseTable[cur_id].no <= end_num)
		{
			index = SlaughtHouseTable[cur_id].index;
		}
		else
		{
			break;
		}

		addr = addr_flash + (long)TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (index - 1);
		read_code = Flash_wread(addr);
		if(read_code == SlaughtHouseTable[cur_id].no)  
		{	
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&slaughHouseStr, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE); 
		 	sprintf(string_buf, "%4d %s", slaughHouseStr.index, slaughHouseStr.slaughtHouse);
			string_buf[lcd_size] = 0;	       		
			
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);	       		 
			Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
			Dsp_Diffuse();
			if (j == 1)
				start_id = cur_id;	
			if (j == line_ct) break;
		}
		
		cur_id++;	
	}
	if (SlaughtHouseTable[cur_id].no >= end_num && j == 0) 
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
			if(SlaughtHouseTable[cur_id].no <= start_num) 
			{
				BuzOn(2);
				cur_id = start_id;
				goto KEY_INPUT;
			}
			cur_id--;
			index = SlaughtHouseTable[cur_id].index;
			addr = addr_flash + (long)TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (index-1);			
			read_code = Flash_wread(addr);
			if(read_code == SlaughtHouseTable[cur_id].no)  
				goto READ_START;
		}
	case KP_PAGE_UP :
		reverse_num = 0;
		while(1)  
		{
			if(SlaughtHouseTable[cur_id].no <= start_num) 
			{
				BuzOn(2);
				cur_id = start_id;
				goto READ_START;
			}
			cur_id--;
			index = SlaughtHouseTable[cur_id].index;
			addr =  addr_flash + (long)TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (index-1);			
			read_code = Flash_wread(addr);
			if(read_code == SlaughtHouseTable[cur_id].no)  
			{
			       	reverse_num++;
				if(reverse_num == line_ct) 
					goto READ_START;
			}
		}
	case KP_PAGE_DN :
		reverse_num = 0;
		while(1)  
		{
			if(SlaughtHouseTable[cur_id].no >= end_num) 
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
			index = SlaughtHouseTable[cur_id].index;
			addr =  addr_flash + (long)TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (index-1);			
			read_code = Flash_wread(addr);
			if(read_code == SlaughtHouseTable[cur_id].no)  
			{
			       	reverse_num++;
				if(reverse_num == line_ct) 
					goto READ_START;
			}
		}
	case KP_ENTER:		
	case KP_ARROW_DN:
		reverse_num = cur_id;
		while(SlaughtHouseTable[reverse_num].no <= end_num)  
		{
			reverse_num++;
			index = SlaughtHouseTable[reverse_num].index;
			addr =  addr_flash + (long)TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE * (index-1);			
			read_code = Flash_wread(addr);
			if(read_code == SlaughtHouseTable[reverse_num].no)  
			{
			       	cur_id = reverse_num;
				goto READ_START;
			}
		}
		BuzOn(2);
		error_display_caption(0xE00E);
		goto KEY_INPUT;
	case KP_TEST:
		//store_name_print_list(start_num, end_num);
		goto KEY_INPUT;	
	case KP_FEED:
		keyapp_feed(OFF);
		goto KEY_INPUT;	
	default :	
		BuzOn(2);
		goto KEY_INPUT;
	} //switch(KeyDrv.CnvCode)						
	// ]

	RET_ESC:    return;
}
#endif //#ifndef DISABLE_LIST_MENU

#else

void	trace_slaughthouse_create(void)
{	
    static INT16U cap_code[2]={0x139A,0x139B};
	menu1_create(0x1391,cap_code,FLASH_TRACE_SLAUGHTERHOUSE_AREA, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE, OFF);
}

void    trace_slaughthouse_delete(void)
{	
	menu1_delete(0x1393, 0x139A, FLASH_TRACE_SLAUGHTERHOUSE_AREA, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);			
}

#ifndef DISABLE_LIST_MENU
void 	trace_slaughthouse_list(void)
{
	INT16U input_cap[3]={0x1E00,0x1E01,0x1E02};
	display_pgm_list(0x1392, 0x139A, input_cap, 0x71FF, FLASH_TRACE_SLAUGHTERHOUSE_AREA, TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
}
#endif
#endif

void trace_slaughthousedata_delete_all(void)
{
	INT32U addr;

	addr = DFLASH_BASE;
	addr += FLASH_TRACE_SLAUGHTERHOUSE_AREA;
	Flash_clear(addr, 0xff, MAX_SLAUGHTERHOUSE_KR_QTY*TRACE_SLAUGHTERHOUSE_KR_STRUCT_SIZE);
#ifdef USE_KOR_SLAUGHT_CODE_4DIGIT
	trace_slaughthousedata_init();
#endif
}

void	indivMultiAndLotIndexInit(void)
{
	//if (FlagIndividualMultiStart || FlagLotMultiStart)//복수개체, 묶음장부 초기화
	{
		FlagIndividualMultiStart = 0;
		FlagLotMultiStart = 0;
		FlagMultiStart = 0;
		MultiIndex = 0;
		LotListMultiIndex = 0;
	}
}
#endif


#ifdef USE_CHN_EMART_TRACE
INT8U CHNEmart_TraceSelectionOrder = 0;
INT8U CHNEmart_CurTraceCode[CHN_EMART_TRACE_CODE_SIZE + 1];

INT8U trace_compare_data(INT8U *srcBuf, INT8U *destBuf, INT16U length)
{
	INT8U i;

	for (i = 0; i < length; i++) 
	{
		if (srcBuf[i] != destBuf[i]) return 0;
		if (srcBuf[i] == 0) return 1;	// null까지 일치하면 PASS
	}
	return 1;
}

void chn_emart_trace_save(INT16U key, INT8U *trace)
{
	INT32U addr, tmpAddr;
	INT8U cnt, i;
	INT8U head;
	INT8U tmpTrace[CHN_EMART_TRACE_CODE_SIZE + 1];

#ifdef USE_CHN_EMART_BIRDS_TRACE
	INT8U tmpQrTrace[CHN_EMART_TRACE_QR_SIZE + 1];

	memset(tmpQrTrace, 0, sizeof(tmpQrTrace));	
	addr = QR_CODE_TRACE_ADDR + (INT32U)key * CHN_EMART_TRACE_QR_AREA_SIZE;

//shift trace code '0' order가 가장 최근
	for(i=(CHN_EMART_TRACE_QR_NO-1); i > 0; i--)
	{
		tmpAddr = addr + CHN_EMART_TRACE_QR_SIZE*(i-1);
		Flash_sread(tmpAddr, tmpQrTrace, CHN_EMART_TRACE_QR_SIZE);
		Flash_swrite(tmpAddr + CHN_EMART_TRACE_QR_SIZE, tmpQrTrace, CHN_EMART_TRACE_QR_SIZE);
	}
//current trace code save
	Flash_clear(addr,0,CHN_EMART_TRACE_QR_SIZE);
		

#endif

	memset(tmpTrace, 0, sizeof(tmpTrace));	
	
	addr = NVRAM_TRACE_CODE_BACKUP + (INT32U)key * CHN_EMART_TRACE_BACKUP_AREA_SIZE;
	head = get_nvram_bparam(addr);
	cnt = get_nvram_bparam(addr + 1);
//shift trace code '0' order가 가장 최근
	for(i=(CHN_EMART_TRACE_BACKUP_NO-1); i > 0; i--)
	{
		tmpAddr = addr + CHN_EMART_TRACE_CODE_SIZE*(i-1) + 2;
		get_nvram_sparam(tmpAddr, tmpTrace, CHN_EMART_TRACE_CODE_SIZE);
		set_nvram_sparam(tmpAddr + CHN_EMART_TRACE_CODE_SIZE, tmpTrace, CHN_EMART_TRACE_CODE_SIZE);
	}
//current trace code save
	set_nvram_sparam(addr + 2, trace, CHN_EMART_TRACE_CODE_SIZE);
	plu_set_field(status_scale.Plu.address-1, 96, trace);
//increase head & count
	if(cnt)
	{
		head++;
		if (head > CHN_EMART_TRACE_BACKUP_NO) head = CHN_EMART_TRACE_BACKUP_NO;	//지정된 Trace code는 새로운 trace code가 도착해도 유지되다가 더이상 유지할 수 없을시 가장 오래된 trace code로 지정된다.
		set_nvram_bparam(addr, head);
	}
	cnt++;
	if (cnt > CHN_EMART_TRACE_BACKUP_NO) cnt = CHN_EMART_TRACE_BACKUP_NO;
	set_nvram_bparam(addr + 1, cnt);
}

INT8U chn_emart_trace_read(INT16U key, INT8U *trace, INT8U order)// order : 0=today, 1=yesterday, ...
{
	INT32U addr;
	INT8U cnt;
	INT8U head;
	
	addr = NVRAM_TRACE_CODE_BACKUP + (INT32U)key * CHN_EMART_TRACE_BACKUP_AREA_SIZE;
//	head = get_nvram_bparam(addr);
	cnt = get_nvram_bparam(addr + 1);
//	if (order + 1 > cnt) //  backup 수량 체크
	{
//		return FALSE;
	}

//	head += CHN_EMART_TRACE_BACKUP_NO;
//	head -= (order + 1);
//	head %= CHN_EMART_TRACE_BACKUP_NO;
	
	addr += 2;
//	addr += (head * CHN_EMART_TRACE_CODE_SIZE);
	addr += (order * CHN_EMART_TRACE_CODE_SIZE);
	get_nvram_sparam(addr, trace, CHN_EMART_TRACE_CODE_SIZE);
	trace[CHN_EMART_TRACE_CODE_SIZE] = 0;

	return TRUE;
}



#ifdef USE_CHN_EMART_BIRDS_TRACE
void chn_emart_qr_trace_save(INT16U key, INT8U *qrTrace)
{
	INT32U addr, tmpAddr;
	INT8U i;
/*	
#ifdef USE_CHN_EMART_QR_DEBUG_TRACE
	INT8U tmpQrTrace[CHN_EMART_TRACE_QR_SIZE + 1];
	INT8U tmpQrTrace1[CHN_EMART_TRACE_QR_SIZE + 1];
	INT8U tmpQrTrace2[CHN_EMART_TRACE_QR_SIZE + 1];
	INT8U tmpQrTrace3[CHN_EMART_TRACE_QR_SIZE + 1];
	
	memset(tmpQrTrace, 0, sizeof(tmpQrTrace));
	memset(tmpQrTrace1, 0, sizeof(tmpQrTrace1));
	memset(tmpQrTrace2, 0, sizeof(tmpQrTrace2));
	memset(tmpQrTrace3, 0, sizeof(tmpQrTrace3));
#endif
*/
		
	addr = QR_CODE_TRACE_ADDR + (INT32U)key * CHN_EMART_TRACE_QR_AREA_SIZE;

	Flash_swrite(addr, qrTrace, CHN_EMART_TRACE_QR_SIZE);
/*	
#ifdef USE_CHN_EMART_QR_DEBUG_TRACE	
	Flash_sread(addr, tmpQrTrace,CHN_EMART_TRACE_QR_SIZE);
	tmpQrTrace[CHN_EMART_TRACE_QR_SIZE] = 0;
	Flash_sread(addr + CHN_EMART_TRACE_QR_SIZE, tmpQrTrace1,CHN_EMART_TRACE_QR_SIZE);
	tmpQrTrace1[CHN_EMART_TRACE_QR_SIZE] = 0;
	Flash_sread(addr + CHN_EMART_TRACE_QR_SIZE*2, tmpQrTrace2,CHN_EMART_TRACE_QR_SIZE);
	tmpQrTrace2[CHN_EMART_TRACE_QR_SIZE] = 0;
	Flash_sread(addr+ CHN_EMART_TRACE_QR_SIZE*3, tmpQrTrace3,CHN_EMART_TRACE_QR_SIZE);
	tmpQrTrace3[CHN_EMART_TRACE_QR_SIZE] = 0;

	sprintf(MsgBuf, "CL-MART[SAVE1]%s, [SAVE2]%s, [SAVE3]%s,[SAVE4]%s,\r\n", tmpQrTrace ,tmpQrTrace1,tmpQrTrace2,tmpQrTrace3);
	MsgOut(MsgBuf);
#endif
*/
}

void chn_emart_qr_trace_delete_key(INT16U key) //20140911
{
	INT32U addr;
	
	addr = QR_CODE_TRACE_ADDR + (INT32U)key * CHN_EMART_TRACE_QR_AREA_SIZE;

	Flash_clear(addr,0,CHN_EMART_TRACE_QR_AREA_SIZE);

}

void chn_emart_qr_trace_delete_all(void)//20140911
{
	INT32U addr;
	INT16U i;
	
	addr = QR_CODE_TRACE_ADDR;

	for (i = 0; i < MAX_PLU_ADDRESS; i++)
	{
		Flash_clear(addr,0,CHN_EMART_TRACE_QR_AREA_SIZE);
		addr += CHN_EMART_TRACE_QR_AREA_SIZE;
	}
}
#endif

void chn_emart_trace_delete_key(INT16U key)
{
	INT32U addr;
	
	addr = NVRAM_TRACE_CODE_BACKUP + (INT32U)key * CHN_EMART_TRACE_BACKUP_AREA_SIZE;

	//memset((INT8U*)addr, 0, CHN_EMART_TRACE_BACKUP_AREA_SIZE);
	nvram_set(addr, 0, CHN_EMART_TRACE_BACKUP_AREA_SIZE);
}

void chn_emart_trace_delete_all(void)
{
	INT32U addr;
	INT16U i;
	
	addr = NVRAM_TRACE_CODE_BACKUP;

	for (i = 0; i < MAX_PLU_ADDRESS; i++)
	{
		//memset((INT8U*)addr, 0, 2);
		nvram_set(addr, 0, 2);
		addr += CHN_EMART_TRACE_BACKUP_AREA_SIZE;
//sprintf(MsgBuf, "CEMART[Q]{DEL]i=%d\r\n", i);
//MsgOut(MsgBuf);
	}
}

INT8U chn_emart_trace_direct_call_traceNo(INT8U order) // order는 가장 최근 lot가 0
{
	INT16U size;
	INT8U ret, prevKey, nextKey, cnt;
	INT32U addr;

	if (!status_scale.cur_pluid)
	{
		return FALSE;
	}

	addr = status_scale.Plu.address-1;
	addr = NVRAM_TRACE_CODE_BACKUP + addr * CHN_EMART_TRACE_BACKUP_AREA_SIZE;
	prevKey = get_nvram_bparam(addr);
	cnt = get_nvram_bparam(addr+1);
	nextKey = prevKey;
	if(order)
	{
		nextKey++;
		if(nextKey >= cnt) nextKey = 0;
		else nextKey = nextKey % CHN_EMART_TRACE_BACKUP_NO;
	}
//#ifdef USE_CHN_EMART_BIRDS_TRACE
	else
	{
		if (Pluchangeflag) nextKey = 0;
	}
//#endif
//	if (order > CHN_EMART_TRACE_BACKUP_NO - 1)
//	{
//		return FALSE;
//	}

//	if (order)
//	{

		ret = chn_emart_trace_read(status_scale.Plu.address-1, CHNEmart_CurTraceCode, nextKey);
		if (!ret)
	{
			CHNEmart_CurTraceCode[0] = 0;
			return FALSE;
	}

		set_nvram_bparam(addr, nextKey);
//	}
//	else
//	{
//		plu_get_field(status_scale.Plu.address-1, 92, (INT8U *)&CHNEmart_CurTraceCode, (int *)&size, CHN_EMART_TRACE_CODE_SIZE);
//	}
	plu_set_field(status_scale.Plu.address-1, 96, (INT8U *)&CHNEmart_CurTraceCode);

	CHNEmart_CurTraceCode[CHN_EMART_TRACE_CODE_SIZE] = 0;
	sale_display_update(UPDATE_TITLE);
	
	CHNEmart_TraceSelectionOrder = nextKey;
	
	return TRUE;
}
#endif