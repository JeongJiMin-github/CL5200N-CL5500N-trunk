/*****************************************************************************
  Copyright		:	(c) 2004 CAS
  Filename		:	clerk_app.c
  Version		:	v 0.9
  Programmer(s)		:	
  Created		:	2004/03/16
  Description		:	plu edit       				
  edit			:	20040319 -> 1.  manager_permission_edit 함수추가 
                                                     menu_function에 등록하고 cap추가 필요함
			                    2.  clerk_manager_edit 부분수정
****************************************************************************/
#include <stdio.h>
#include <string.h>
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
#include "prt_common.h"
#include "prt_interpreter.h"
#include "dsp_vfd7.h"
#include "dsp_app.h"

#include "adm.h"
#include "key.h"
#include "key_typedef.h"
#include "key_app.h"
#include "key_function.h"
#include "main.h"
#include "cal.h"
#include "debug.h"
#include "menu.h"
#include "initial.h"
#include "common.h"
#include "mode_main.h"
#include "mode_error.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "flash_app.h"
#include "caption_app.h"
#include "clerk_app.h" 
#include "mode_sale.h"
#include "pgm_app.h"
//#include "sale_report.h"	// print cmd용
#include "prt_cmd.h"
#include "login.h"
#include "prt_list.h"
#include "menu_code_text1.h"

//reload 할필요 없음 
INT8U clerk_print_list(INT16U start_id, INT16U end_id);
INT8U clerk_zero_allow;

CLERK_STRUCT	g_clerk_str;

////INT8U clerk_edit_check(CLERK_STRUCT *clerk_str)
////{
////	INT16S i,err;
//// 
////	err=0;
////	if (!((status_scale.clerkid==0) && (status_scale.level==1))) {
////		i=password_input_clerk(clerk_str,0);
////		if (!i) err=1;
////		if (!((clerk_str->code==0) && (clerk_str->permissionLevel==1))) err=1;
////	}
////	if (err) {
////		error_display_caption(0x38D1);
////		return OFF;
////	}
////	return ON;
////}
void callback_clerk_id(INT16S num)
{
	CAPTION_STRUCT 	cap;

	POINT  disp_p;
	INT8U  buffer[64], i;
	INT16U uitemp;
	INT32U addr;

	addr = num;
	addr *= CLERK_STRUCT_SIZE;
	addr += FLASH_CLERK_AREA;
	uitemp = get_base_wparam(addr);
	if (uitemp != num) {
		memset((INT8U*)&g_clerk_str, 0x00, CLERK_STRUCT_SIZE);
	} else {
		get_base_sparam( addr, (INT8U *)&g_clerk_str, CLERK_STRUCT_SIZE); 
	}
	g_clerk_str.code = num;

	disp_p = point((Startup.menu_type?2:1)*Startup.menu_ygap, 0);
	memset((INT8U *)&buffer,0x20, 64);
	memset((INT8U *)&cap.form, 0x20, sizeof(cap.form));
	buffer[63] = 0;

	caption_split_by_code(0x1d02, &cap,0);
	//caption_adjust(&cap,NULL);
#ifdef HEBREW_FONT
	buffer[0] = ']';
#else
	buffer[0] = '[';
#endif
	for (i = 0; i < strlen((HUGEDATA char *)g_clerk_str.name); i++)
		buffer[i+1] = g_clerk_str.name[i];
#ifdef HEBREW_FONT
	buffer[cap.input_dlength] = '[';
#else
	buffer[cap.input_dlength] = ']';
#endif
	buffer[cap.input_dlength+1] = 0;

	strcat((HUGEDATA char *)cap.form, (HUGEDATA char *)buffer);
	DspStruct.DirectDraw = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)&cap.form);
	
	if (Startup.menu_type)
	{
		disp_p = point(3*Startup.menu_ygap, 0);
		memset((INT8U *)&buffer,0x20, 64);
		memset((INT8U *)&cap.form, 0x20, sizeof(cap.form));
		buffer[63] = 0;

		caption_split_by_code(0x1d03, &cap,0);
		//caption_adjust(&cap,NULL);
#ifdef HEBREW_FONT
		buffer[0] = ']';
#else
		buffer[0] = '[';
#endif
		for (i = 0; i < strlen((HUGEDATA char *)g_clerk_str.nickname); i++)
			buffer[i+1] = g_clerk_str.nickname[i];
#ifdef HEBREW_FONT
		buffer[cap.input_dlength] = '[';
#else
		buffer[cap.input_dlength] = ']';
#endif
		buffer[cap.input_dlength+1] = 0;

		strcat((HUGEDATA char *)cap.form, (HUGEDATA char *)buffer);
		DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)&cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
		arab_codetemp[strlen((char *)cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)&cap.form);
#endif
	}

}


void callback_clerk_name(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	CLERK_STRUCT		clerk_str;
	INT8U	buffer[64];
	INT32U	addr;
	INT16S  	str_size;	
	INT16U 	w;
	  
	str_size = CLERK_STRUCT_SIZE;

#ifdef USE_ARAB_CONVERT
    if (CurMenuCode == 0x1856)
        disp_p = point((CallbackEditPosition_y+1)*Startup.menu_ygap, 0);
    else
#endif
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars();
#ifndef USE_ARAB_CONVERT
	w -= CallbackEditPosition_x;
#endif
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

	if (!clerk_zero_allow) {
		if (num == 0) goto M1;
	}
	caption_split_by_code(0x1D01, &cap,0);
	if(num < cap.input_min || num > cap.input_max){
M1:		
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)&global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0); 
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
        DspStruct.Reverse = 1;
		Convert_ArabPreview((INT8U *)&global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0); 
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
        DspStruct.Reverse = 0;
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	} else {
		addr = num;
		addr *= str_size;
		addr += FLASH_CLERK_AREA;
		get_base_sparam(addr, (INT8U*)&clerk_str, str_size);
		if(num != clerk_str.code) {
			caption_split_by_code(0xE00C, &cap,0);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)&cap.form, strlen(cap.form), 0, 0); 
			hebrew_codetemp[strlen(cap.form)] = 0;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
            DspStruct.Reverse = 1;
			Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
			arab_codetemp[strlen(cap.form)] = 0;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
            DspStruct.Reverse = 0;
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)&cap.form);
#endif
			return;
		} else {
			memcpy((INT8U*)buffer, (INT8U *)clerk_str.name, str_size);
			buffer[10]=0;
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)&buffer, strlen(buffer), 0, 0); 
			hebrew_codetemp[strlen(buffer)] = 0;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
            DspStruct.Reverse = 1;
			Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
			arab_codetemp[strlen(buffer)] = 0;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
            DspStruct.Reverse = 0;
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)&buffer);
#endif
			return;
		}
	} 
}

void clerk_dataUpdate(INT32U num)
{
	CLERK_STRUCT clerk_str;
	INT32U	addr;
	INT16U  clerkIndex;

	addr = DFLASH_BASE;
	addr += FLASH_CLERK_AREA;
	addr += (num * CLERK_STRUCT_SIZE);
	clerkIndex = Flash_wread(addr);
	if(clerkIndex == num){
		Flash_sread(addr, (HUGEDATA INT8U *)&clerk_str, CLERK_STRUCT_SIZE);
	} else {
		memset(&clerk_str, 0, CLERK_STRUCT_SIZE);
		clerk_str.code = num;
	}

	*Menu[0].pval = clerk_str.code;
	memcpy(Menu[1].pstr, clerk_str.name, Menu[1].length);
	memcpy(Menu[2].pstr, clerk_str.nickname, Menu[2].length);
	memcpy(Menu[3].pstr, clerk_str.password, Menu[3].length);
	*Menu[4].pval = clerk_str.permissionLevel;
	*Menu[5].pval = clerk_str.training;

	MenuRefresh = ON;
}

void clerk_new(void)
{	
	CAPTION_STRUCT cap;
	CLERK_STRUCT clerk_str;
	INT8U	i;
	INT8U	result;
	INT32U	addr;
	INT32U  clerk_no, level, training;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	POINT 	disp_p;
	//INT16U	uitemp;
	long	ret_long;
	long	ret_yesno;						     //modify by se-hyung for training key 20070627
	INT16S  nitem=6, max_page,k,pageno,z,lineno;			     //modify by se-hyung for training key 20070627
	INT32U  set_val[4];
	INT16U  caption_id[6]={0x1d01,0x1d02,0x1d03,0x1d04,0x1d05,0x1d06};   //modify by se-hyung for training key 20070627

	max_page = nitem/Startup.line_max;
	
	if (nitem%Startup.line_max) max_page++;
#else
	INT8U   name_str[24+1];
	INT8U   nickname_str[8+1];
	INT8U   passwd_str[8+1];
#endif
	
	if(!permission_check(PMS_SECURITY,0))	return;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
CLERK_NEW_START:
	lineno=0;
#endif
	caption_split_by_code(0x1D01, &cap,0); //Clerk No. :
	i=0;
	while(1) {	
		addr = i;
		addr *= CLERK_STRUCT_SIZE;
		addr += FLASH_CLERK_AREA;
		addr += DFLASH_BASE;
		clerk_no = Flash_wread(addr);
		if( clerk_no == i ) {
			i++;
		} else {
			break;
		}
	}
	if(clerk_no == cap.input_max) {
		error_display_caption(0xF035);	// data full일때는 manager의 데이터를 보여준다.
		addr = FLASH_CLERK_AREA;
		addr += DFLASH_BASE;
		Flash_swrite(addr, (INT8U *)&clerk_str, CLERK_STRUCT_SIZE);
	} else {
		memset((INT8U*)&clerk_str, 0x00, CLERK_STRUCT_SIZE);
		clerk_str.code = i;	
	}
	if (clerk_str.code==0) clerk_str.code=1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	pageno=0;
PAGE1_VIEW:
	Dsp_Fill_Buffer(0);
	if (pageno>=max_page) pageno=max_page-1;
	z = pageno * Startup.line_max;
	display_title_page_change(0x1851, pageno+1,max_page);
	for(k=0; k<Startup.line_max; k++) {
		i = z + k;
		if (i>=nitem) break;
		caption_split_by_code(caption_id[i], &cap,0);
// plu_get_display_info(1,&cap,0);
		caption_adjust(&cap,NULL);
		disp_p = point((k + Startup.menu_type)*Startup.menu_ygap,0);
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
		switch(i) {
			case 0: ret_long = clerk_str.code;
				if( !input_range_check(ret_long, cap.input_min, cap.input_max) ) {
					ret_long = 0;
				}
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
				result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&ret_long, cap.input_length, 
							(k+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
#else
				result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length, 
							(k+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
#endif
				break;
			case 1: cap.input_length = sizeof(clerk_str.name)-1;
				clerk_str.name[cap.input_length]=0;
				result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)clerk_str.name, 
						cap.input_length, cap.input_dlength, (k+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0, 0, KI_PASS);
				break;
			case 2: cap.input_length = sizeof(clerk_str.nickname)-1;
				clerk_str.nickname[cap.input_length]=0;
				result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)clerk_str.nickname, 
						cap.input_length, cap.input_dlength, (k+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0, 0, KI_PASS);
				break;
			case 3: clerk_str.password[cap.input_length]=0;
				result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)clerk_str.password, 
						cap.input_length, cap.input_dlength, (k+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0, 0, KI_PASS);
				break;
/*			case 4:
			case 5:
			case 6:
			case 7: result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&set_val[i-4], cap.input_length, 
								(k+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
				break;	  */ //delete - se-hyung 20070627

//modify by se-hyung for training key start 20070627
			case 6:
			case 7:
			case 4: 
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
				result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&set_val[i-4], cap.input_length, 
								(k+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
#else
				result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&set_val[i-4], cap.input_length, 
								(k+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
#endif
				break;
			case 5: ret_yesno = clerk_str.training;
				result = keyin_yesno(KI_GRAPHIC,(long*)&ret_yesno,(k+Startup.menu_type)*Startup.menu_ygap,	
							cap.input_x*Startup.menu_xgap, KI_PASS);
				break;
//modify by se-hyung for training key end 20070627
		}
	}
	Dsp_Diffuse();
PAGE1_KI_IN:
	if (lineno >=Startup.line_max) lineno = Startup.line_max - 1;
	if (pageno>=max_page) pageno=max_page-1;
	z = pageno * Startup.line_max;
	i = z + lineno;
	if (i>=nitem) i = nitem - 1;
	lineno = i%Startup.line_max;

	caption_split_by_code(caption_id[i], &cap,0);
	caption_adjust(&cap,NULL);
	switch (i) {
		case 0:	ret_long = clerk_str.code;
			if( !input_range_check(ret_long, cap.input_min, cap.input_max) ) {
				ret_long = 0;
			}
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
			result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&ret_long, cap.input_length, 
				(lineno+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
			result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length, 
				(lineno+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
			break;	
 		case 1:	cap.input_length = sizeof(clerk_str.name)-1;
 			clerk_str.name[cap.input_length]=0;
 			result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)clerk_str.name, 
 				cap.input_length, cap.input_dlength, (lineno+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
 			break;
 		case 2:	cap.input_length = sizeof(clerk_str.nickname)-1;
 			clerk_str.nickname[cap.input_length]=0;
 			result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)clerk_str.nickname, 
 					cap.input_length, cap.input_dlength, (lineno+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
 			break;
 		case 3: clerk_str.password[cap.input_length]=0;
 			result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)clerk_str.password, 
 				cap.input_length, cap.input_dlength, (lineno+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0, 0, KI_NOPASS);
 			break;
/* 		case 4:
 		case 5:
 		case 6:
 		case 7:	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&set_val[i-4], cap.input_length, 
    					(lineno+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
 			break;		     */ //delete - sehyung 20070627

//modify by se-hyung for training key start 20070627
 		case 6:
 		case 7:
		case 4: 
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
			result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&set_val[i-4], cap.input_length, 
						(lineno+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
  			result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&set_val[i-4], cap.input_length, 
    					(lineno+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
 			break;
 		case 5:	result = keyin_yesno(KI_GRAPHIC,(long*)&ret_yesno,(lineno+Startup.menu_type)*Startup.menu_ygap,  
					 		cap.input_x*Startup.menu_xgap, KI_NOPASS);
 			break;
//modify by se-hyung for training key end 20070627
        }

       	if(result == KP_ESC) goto RET_ESC;
	if(i==0) {
		if( !input_range_check(ret_long, cap.input_min, cap.input_max) ) {
			error_display_form1(0, cap.input_min, cap.input_max);
			goto PAGE1_KI_IN;
		} else if (result != KP_ARROW_UP && result != KP_PAGE_UP){
			addr = ret_long;
			addr *= CLERK_STRUCT_SIZE;
			addr += FLASH_CLERK_AREA;
			clerk_no = get_base_wparam(addr);
			if (clerk_no != ret_long)
			{
				memset((INT8U*)&clerk_str, 0x00, CLERK_STRUCT_SIZE);
			}
			else
			{
				get_base_sparam( addr, (INT8U *)&clerk_str, CLERK_STRUCT_SIZE); 
			}
			clerk_str.code = ret_long;
		}
		set_val[0]=clerk_str.permissionLevel;
//		set_val[1]=clerk_str.dept           ;
		set_val[1]=clerk_str.headid         ;
		set_val[2]=clerk_str.shopid         ;
		if (set_val[0] < 0 || set_val[0] > 3)	set_val[0] = 0;
//		if (set_val[1] < 1 || set_val[1] >= MAX_DEPT_NO)	set_val[1] = 1;
		if (set_val[1] < 0 || set_val[1] > 99)	set_val[1] = 0;
		if (set_val[2] < 0 || set_val[2] > 99)	set_val[2] = 0;
	} else if (i>=4) {
		switch (i) {
			case 4: cap.input_min=0; cap.input_max= 3; break;
//			case 5: cap.input_min=1; cap.input_max=MAX_DEPT_NO-1; break;
			case 5: cap.input_min=0; cap.input_max=99; break;
			case 6: cap.input_min=0; cap.input_max=99; break;
		}
		ret_long = set_val[i-4];
		if( !input_range_check(ret_long, cap.input_min, cap.input_max) ) {
			error_display_form1(0, cap.input_min, cap.input_max);
			goto PAGE1_KI_IN;
		}
	}
	switch(result) {
		case KP_ESC     : goto RET_ESC;
		case KP_SAVE    : goto RET_SAVE;
		case KP_ENTER   :
		case KP_ARROW_DN: lineno ++;
				  if (i==0) goto PAGE1_VIEW;
				  if (i>=nitem-1) goto RET_SAVE;
			          if (lineno>=Startup.line_max) {
					goto PAGE_DN;
				  }
				  goto PAGE1_KI_IN;
//				  break;
		case KP_PAGE_DN : 
			PAGE_DN : pageno++;
				  if (pageno>=max_page) {
				  	pageno=max_page-1;
					goto RET_SAVE;
				  } else {
					lineno = 0;
				  }
				  goto PAGE1_VIEW;
//				  goto PAGE1_KI_IN;
		case KP_ARROW_UP: if (lineno) lineno--;
				  else {
					if (pageno) goto PAGE_UP;
				  }
				  goto PAGE1_KI_IN;
		case KP_PAGE_UP :
			PAGE_UP : if (pageno) {
					lineno=0;
					pageno--;
					goto PAGE1_VIEW;
				  }
				  goto PAGE1_KI_IN;
		default:	  BuzOn(2);
			          goto PAGE1_KI_IN;
	}
RET_SAVE:
	str_trim((char *)clerk_str.password);
	if (strlen((char *)(clerk_str.password))==0) {
	 	error_display_caption(0x38D2);
		pageno = 0;
		lineno = 0;
		goto PAGE1_VIEW;
	}
	clerk_str.permissionLevel = (INT8U)set_val[0];
//	clerk_str.dept            = (INT8U)set_val[1];
	clerk_str.dept            = 0;
	clerk_str.headid          = (INT8U)set_val[1];
	clerk_str.shopid          = (INT8U)set_val[2];
	clerk_str.permissionID    = 0;
	clerk_str.training    = ret_yesno;	   //modify by se-hyung for training key 20070627
	addr = clerk_str.code;
	addr *= CLERK_STRUCT_SIZE;
	addr += FLASH_CLERK_AREA;

	set_base_sparam(addr, (INT8U*) &clerk_str, CLERK_STRUCT_SIZE);
	goto CLERK_NEW_START;
RET_ESC:
	return;
#else
	clerk_no = (INT32U)clerk_str.code;
	strcpy((char*)name_str, (char*)clerk_str.name);
	strcpy((char*)nickname_str, (char*)clerk_str.nickname);
	strcpy((char*)passwd_str, (char*)clerk_str.password);
	level = (INT32U)clerk_str.permissionLevel;
	training = (INT32U)clerk_str.training;
	
	
	Menu_Init();

	caption_split_by_code(0x1851, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x1d01, &cap,0); // Clerk No. :
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &clerk_no, NULL, NULL, NULL);
	Menu_AddEndFunction(1, clerk_dataUpdate);
	
	caption_split_by_code(0x1d02, &cap,0); // Clerk Name:
	caption_adjust(&cap,NULL);
	Menu_InputCreate(2, (char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, name_str, NULL, NULL);

	caption_split_by_code(0x1d03, &cap,0); // Nickname  :
	caption_adjust(&cap,NULL);
	Menu_InputCreate(3, (char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, nickname_str, NULL, NULL);

	caption_split_by_code(0x1d04, &cap,0); // Clerk Password:
	caption_adjust(&cap,NULL);
	Menu_InputCreate(4, (char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, passwd_str, NULL, NULL);

	caption_split_by_code(0x1d05, &cap,0); // Clerk Level  :
	caption_adjust(&cap,NULL);
	Menu_InputCreate(5, (char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &level, NULL, NULL, NULL);

	caption_split_by_code(0x1d06, &cap,0); // Use for Training : 
	caption_adjust(&cap,NULL);
	Menu_InputCreate(6, (char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &training, NULL, NULL, NULL);

	while (1)
	{
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) {
			str_trim((char *)passwd_str);
			if (strlen((char *)(passwd_str))==0) {
				error_display_caption(0x38D2);
				continue;
			}

			clerk_str.code = (INT16U)clerk_no;
			strcpy((char*)clerk_str.name, (char*)name_str);
			strcpy((char*)clerk_str.nickname,(char*)nickname_str);
			str_trim((char *)passwd_str);
			strcpy((char*)clerk_str.password, (char*)passwd_str);
			clerk_str.permissionLevel = (INT8U)level;
			clerk_str.training = (INT8U)training;
			clerk_str.dept            = 0;
			clerk_str.headid          = 0;
			clerk_str.shopid          = 0;
			clerk_str.permissionID    = 0;

			addr = clerk_str.code;
			addr *= CLERK_STRUCT_SIZE;
			addr += FLASH_CLERK_AREA;
			addr += DFLASH_BASE;
			Flash_swrite(addr, (INT8U *)&clerk_str, CLERK_STRUCT_SIZE);
			break;
		} else if(result == MENU_RET_ESC) break;
	}
#endif
}

void clerk_password_dataUpdate(INT32U num)
{
	CLERK_STRUCT clerk_str;
	INT32U	addr;
	INT16U  clerkIndex;

	addr = DFLASH_BASE;
	addr += FLASH_CLERK_AREA;
	addr += (num * CLERK_STRUCT_SIZE);
	clerkIndex = Flash_wread(addr);
	if(clerkIndex == num){
		Flash_sread(addr, (HUGEDATA INT8U *)&clerk_str, CLERK_STRUCT_SIZE);
	} else {
		memset(&clerk_str, 0, CLERK_STRUCT_SIZE);
		clerk_str.code = num;
	}

	memcpy(Menu[1].pstr, clerk_str.password, Menu[1].length);

	MenuRefresh = ON;
}

void clerk_password_change(void)
{
	CAPTION_STRUCT cap;
	CLERK_STRUCT clerk_str;
	INT8U i;
	INT8U result;
	INT32U clerk_id;
	INT32U addr;
	INT8U clerkuse;
	INT8U dealer_password;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	POINT 		disp_p;
	INT16U cap_id[2]={0x1D01,0x1D09};
	INT16S    input_min,input_max;
	char	ret_str[32];//, new_pw[8];
	char    stemp[32];
	INT8U	input_line=0, err;
	long	ret_long;

	err=0;
	input_line = 0;
#else
	char passwd_str[32];
	INT8U string_buf[50];
#endif
	clerkuse=0;
	if ((status_scale.restrict&FUNC_CLERK)) clerkuse=1;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		if (clerkuse) {
		if (!((status_scale.clerkid==0) && (status_scale.level==1))) {
			i=password_input_clerk(&clerk_str, 0);
			memcpy((INT8U*)ret_str, (INT8U*)clerk_str.password, 8);
			if (!i) err=1;
		}
		if (err) {
			error_display_caption(0x38D1);
			return;
		}
	} else {
		clerk_str.code=1;
	}
	clerk_id = clerk_str.code;
	caption_split_by_code(cap_id[0], &cap,0);
	clerk_zero_allow=1;
	input_min=cap.input_min;
	input_max=cap.input_max;

	memset(ret_str,0,8);
	if(clerk_str.code >= input_min && clerk_str.code <=input_max) {
		input_line = 1;
		addr = clerk_str.code;
		addr *=	CLERK_STRUCT_SIZE;
		addr += FLASH_CLERK_AREA;
		get_base_sparam(addr, (INT8U*) &clerk_str, CLERK_STRUCT_SIZE);
	} else if (clerk_id==0) {
		clerk_id=1;
		clerk_str.code=1;
		addr = clerk_str.code;
		addr *=	CLERK_STRUCT_SIZE;
		addr += FLASH_CLERK_AREA;
		get_base_sparam(addr, (INT8U*) &clerk_str, CLERK_STRUCT_SIZE);
	} else {
		memset(&clerk_str,0,sizeof(clerk_str));
	}
	memcpy((INT8U*)ret_str, (INT8U*)clerk_str.password, 8);
PAGE_VIEW:
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1852, 1, 1);
	for(i=0; i<2; i++) {
		caption_split_by_code(cap_id[i], &cap,0);
		if ((!clerkuse) && (i==1)) {
#ifdef HEBREW_FONT
			sprintf(stemp,"%s)%02d(",cap.form,clerk_id);
#else
			sprintf(stemp,"%s(%02d)",cap.form,clerk_id);
#endif
			strcpy(cap.form,stemp);

		}
		if ((!clerkuse) && (i==0)) goto SKIP1;
		caption_adjust(&cap,NULL);
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
SKIP1:
		switch(i) {
			case 0: if (!clerkuse) break;
				ret_long = clerk_id;
				if( !input_range_check(ret_long, cap.input_min, cap.input_max) ){
					ret_long=1;
				}
				CallbackEditPosition_x=(INT16S)strlen((char *)cap.form)+1;
				CallbackEditPosition_y=(i+Startup.menu_type);
				keyin_setcallback(callback_clerk_name);
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
				result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&ret_long, cap.input_length, 
								(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
#else
				result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length, 
								(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_PASS );
#endif
				keyin_clearcallback();
				break;	
				
			case 1:
				result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)ret_str, 
						cap.input_length, cap.input_dlength, (i+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 0, 0, KI_PASS);
				break;
		}
	}
	Dsp_Diffuse();
	for(i=input_line; i<2; i++) {
		caption_split_by_code(cap_id[i], &cap,0);
		if ((!clerkuse) && (i==1)) {
#ifdef HEBREW_FONT
			sprintf(stemp,"%s)%02d(",cap.form,clerk_id);
#else
			sprintf(stemp,"%s(%02d)",cap.form,clerk_id);
#endif
			strcpy(cap.form,stemp);
		}
		caption_adjust(&cap,NULL);
KI_IN:
		switch(i) {
			case 0: if (!clerkuse) break;
				ret_long = clerk_id;				
				if( !input_range_check(ret_long, cap.input_min, cap.input_max) ){
					ret_long=1;
				}
				CallbackEditPosition_x=(INT16S)strlen((char *)cap.form)+1;
				CallbackEditPosition_y=(i+Startup.menu_type);
				keyin_setcallback(callback_clerk_name);
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
				result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&ret_long, cap.input_length, 
								(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else

				result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length, 
								(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
				keyin_clearcallback();
				break;	
				
			case 1:
//				memcpy((INT8U*)ret_str, (INT8U*)new_pw, 8);
//				memcpy((INT8U*)ret_str, (INT8U*)clerk_str.password, 8);
				result = keyin_string(KI_FREE_MODE, KI_GRAPHIC,0, (HUGEDATA INT8U*)ret_str, 
						cap.input_length, cap.input_dlength, (i+Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap, 1, 0, KI_NOPASS);
				break;
		}
		if(result == KP_ESC)
			goto RET_ESC;

		if(cap.input_itype == 'N') {	
			if( !input_range_check(ret_long, input_min, input_max) ) {
				error_display_form1(0, input_min, input_max);
				goto KI_IN;
			} else {
				addr  = ret_long;
				addr *= CLERK_STRUCT_SIZE;
				addr += FLASH_CLERK_AREA;
				get_base_sparam(addr, (INT8U *)&clerk_str, CLERK_STRUCT_SIZE ); 
				memcpy((INT8U*)ret_str, (INT8U*)clerk_str.password, 8);
				if( clerk_str.code != ret_long ) {
					error_display_caption(0xE00C);
					goto KI_IN;
				} else {
					clerk_id = clerk_str.code;
					input_line=1;
					goto PAGE_VIEW;
				}	
			}
		}
		switch(result) {
			case KP_ENTER:
			case KP_ARROW_DN:
			case KP_SAVE:
				strcpy((char *)clerk_str.password,(char *)ret_str);
				str_trim((char *)clerk_str.password);
				if (strlen((char *)(clerk_str.password))==0) {
					error_display_caption(0x38D2);
					goto KI_IN;
				}
				break;

			case KP_ARROW_UP:
				switch(i) {
					case 0:   BuzOn(2);    goto KI_IN;
					case 1:   if(clerk_str.code > 0 && clerk_str.code <=95) {
							goto KI_IN;
						  } else {
							input_line=0;	   
							goto PAGE_VIEW;
						  }
				}

			default:
				BuzOn(2);	goto KI_IN;
		}
	}

	addr = clerk_str.code;
	addr *= CLERK_STRUCT_SIZE;
	addr += FLASH_CLERK_AREA;
	set_base_sparam(addr, (INT8U*) &clerk_str, CLERK_STRUCT_SIZE);
	
RET_ESC:
	Dsp_Fill_Buffer(0); 					// LCD Buffer clear
	Dsp_Diffuse();
#else
	i=password_input_clerk(&clerk_str, 0);
	if (!i) {
		error_display_caption(0x38D1);
		return;
	}
	if (clerkuse) {
	} else {
		clerk_str.code=1;
	}
	clerk_id = clerk_str.code;
	caption_split_by_code(0x1d01, &cap, 0);
	clerk_zero_allow=1;

	memset(passwd_str, 0, 8);
	dealer_password = OFF;
	if(clerk_str.code < cap.input_min || clerk_str.code > cap.input_max) {
		dealer_password = ON;
		clerk_id=1;
		clerk_str.code=1;
	}
	addr = clerk_str.code;
	addr *=	CLERK_STRUCT_SIZE;
	addr += FLASH_CLERK_AREA;
	addr += DFLASH_BASE;
	Flash_sread(addr, (INT8U *)&clerk_str, CLERK_STRUCT_SIZE);
	memcpy((INT8U*)passwd_str, (INT8U*)clerk_str.password, 8);


	Menu_Init();

	caption_split_by_code(0x1852, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	if (clerkuse && dealer_password)
	{
		caption_split_by_code(0x1d01, &cap, 0); // Clerk No. :
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &clerk_id, NULL, NULL, NULL);
		Menu_AddEndFunction(1, clerk_password_dataUpdate);
	}

	caption_split_by_code(0x1d09, &cap,0); // Clerk Password:
#if defined(USE_SINGLE_LINE_GRAPHIC)
	cap.form[4] = 0;
	sprintf((char*)string_buf, "%-4s.%02ld", cap.form, clerk_id);
#else
#ifdef HEBREW_FONT
	sprintf((char*)string_buf, "%s)%02d(", cap.form, clerk_id);
#else
	sprintf((char*)string_buf, "%s(%02d)", cap.form, clerk_id);
#endif
#endif
	caption_adjust(&cap,NULL);
	Menu_InputCreate(2, (char *)string_buf, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, (INT8U*)passwd_str, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		addr = clerk_id;
		addr *= CLERK_STRUCT_SIZE;
		addr += FLASH_CLERK_AREA;
		addr += DFLASH_BASE;
		Flash_sread(addr, (INT8U *)&clerk_str, CLERK_STRUCT_SIZE);

		str_trim((char *)passwd_str);
		strcpy((char*)clerk_str.password, (char*)passwd_str);

		Flash_swrite(addr, (INT8U *)&clerk_str, CLERK_STRUCT_SIZE);
	}
#endif
}

extern INT8U menu_data_from_zero;
void clerk_delete(void)
{
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	CAPTION_STRUCT	cap;
	CLERK_STRUCT	clerk_str;
	POINT		disp_p;
	INT16U	result;
	long	ret_long;
	INT32U	addr;

	if(!permission_check(PMS_SECURITY,0))	return;
	
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1854, 1, 1);
	caption_split_by_code(0x1D01, &cap,0);
	caption_adjust(&cap,NULL);
	cap.input_min = 1;
	disp_p = point(Startup.menu_type*Startup.menu_ygap,0);
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
	Dsp_Diffuse();
	ret_long = 1;
KI_IN:
  #ifdef USE_ARAB_CONVERT
    CallbackEditPosition_x=0;
	CallbackEditPosition_y=(Startup.menu_type+1);
  #else
	CallbackEditPosition_x=(INT16S)strlen((char *)cap.form)+1;
	CallbackEditPosition_y=(Startup.menu_type);
  #endif
	clerk_zero_allow=0;
	keyin_setcallback(callback_clerk_name);  
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
	result = keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN, 0,(long*)&ret_long, cap.input_length, 
					 Startup.menu_type*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
	result = keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL, 0,(long*)&ret_long, cap.input_length, 
					 Startup.menu_type*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
	keyin_clearcallback();
	if(result == KP_ESC)
		goto RET_ESC;
	if( !input_range_check(ret_long, cap.input_min, cap.input_max) ) {
		error_display_form1(0, cap.input_min, cap.input_max);
		goto KI_IN;
	}
	switch(result) {
		case KP_ENTER:
		case KP_ARROW_DN:
		case KP_SAVE:
			break;
		default:
			BuzOn(2);
			goto KI_IN;
	}
	if (ret_long<2) {
		BuzOn(2);
		goto KI_IN;
	}
//	if (ret_long==1) {
//		BuzOn(2);
//	}
	memset((INT8U*)&clerk_str, 0x00, CLERK_STRUCT_SIZE); 
	clerk_str.code = 0xffff; 
	addr = ret_long;
	addr *= CLERK_STRUCT_SIZE;
	addr += FLASH_CLERK_AREA;
	set_base_sparam(addr, (INT8U*) &clerk_str, CLERK_STRUCT_SIZE);
RET_ESC:
	Dsp_Fill_Buffer(0); 					// LCD Buffer clear
	Dsp_Diffuse();
	return;
#else
	if(!permission_check(PMS_SECURITY,0))	return;
	menu_data_from_zero = 1;
	menu1_delete(0x1854, 0x1D01, FLASH_CLERK_AREA, CLERK_STRUCT_SIZE);
	menu_data_from_zero = 0;
#endif
}


void manager_permission_edit(void)
{
	CLERK_PERMISSION_STRUCT 	permission_str;
	// CLERK_STRUCT			clerk_str;

	INT8U	permi_id, sale_mode;	       	//	permi_id  0: manager permission when normal_mode
						// 	permi_id  1: manager permission when clerk_mode	
						//      permi_id  2~8: clerk permission when clerk_mode
	INT16U	result, str_size;
	
	INT32U  addr;

	str_size = PERMISSION_STRUCT_SIZE;
	sale_mode = get_global_bparam(GLOBAL_AREA_SALEMODE);
	if(!permission_check(PMS_SECURITY,0))	return;
	if(sale_mode ==OPER_NORMAL) {
		permi_id = 0;
	} else {
		permi_id = 1;
	}
FUN_START:
	result = permission_setting(permi_id, &permission_str);
	switch(result) {
		case KP_ENTER:
		case KP_ARROW_DN:
		case KP_SAVE:
		case KP_PAGE_DN:
			break;

		case KP_ESC:
			goto RET_ESC;

		default:
			BuzOn(2);
			goto FUN_START;
	}
	addr =(INT32U)permi_id;
	addr*=str_size;
	addr+=FLASH_CLERK_PERMISSION;
	set_base_sparam(addr, (INT8U*)&permission_str, str_size);
RET_ESC:
 	return;	
	// id check
}

/*
void clerk_permission_edit(void)
{
	CAPTION_STRUCT	cap;	
	POINT 		disp_p;
		
	INT8U	y_gab=8, Startup.menu_xgap=6;				// 폰트의 x, y 크기

	CLERK_PERMISSION_STRUCT 	permission_str;
	CLERK_STRUCT			clerk_str;

	char 	buffer[32];
	INT8U	permi_id, sale_mode;	       	//	permi_id  0: manager permission when normal_mode
						// 	permi_id  1: manager permission when clerk_mode	
						//      permi_id  2~8: clerk permission when clerk_mode
	INT16U	result, str_size;
	long 	ret_long;
	INT32U  addr;

	str_size = PERMISSION_STRUCT_SIZE;
	sale_mode = get_global_bparam(GLOBAL_AREA_SALEMODE);
	if(sale_mode ==OPER_NORMAL) {
		sprintf(buffer, "Change Clerk mode");
		error_display_message(buffer);	      
		return;
	}
//	get_base_sparam(FLASH_CLERK_AREA, (INT8U*)&clerk_str, CLERK_STRUCT_SIZE); // load data of manager

	// JHC 20040331 <MODIFY>
	// [
	// <Original>
	// if (!clerk_edit_check(&clerk_str)) return;
	//
	// <Modify>
	if(!permission_check(PMS_SECURITY))	return;
	// ]

	display_title_page_change(0x1855, 1, 1);
	permi_id=2;
INPUT_PERMISSION:
	result = permission_setting(permi_id, &permission_str);
	switch(result) {
		case KP_ENTER:
		case KP_ARROW_DN:
		case KP_SAVE:
		case KP_PAGE_DN:
			break;

		case KP_ARROW_UP:
		case KP_PAGE_UP:
//			goto KI_IN;
			goto INPUT_PERMISSION;
		case KP_ESC:
			goto RET_ESC;

		default:
			BuzOn(2);
			goto INPUT_PERISSION;
	}
	addr =(INT32U)permi_id;
	addr*=str_size;
	addr+=FLASH_CLERK_PERMISSION;
	set_base_sparam(addr, (INT8U*)&permission_str, str_size);
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
 	return;	
	// id check
}
*/

INT16U permission_setting(INT8U permission_id, CLERK_PERMISSION_STRUCT *permission_str) 
{	
	CAPTION_STRUCT	cap;	
	INT8U 	menu_cnt;
	INT16S	i, result, str_size;
	INT32U 	input_data[39];
	INT32U  addr;

	str_size = PERMISSION_STRUCT_SIZE;

	addr = permission_id;
	addr*= str_size;
	addr+=FLASH_CLERK_PERMISSION;
	get_base_sparam(addr,(INT8U*)permission_str, str_size );

	input_data[0]	= permission_str->change_item;
	input_data[1]	= permission_str->change_price;
	input_data[2]	= permission_str->key_dept;
	input_data[3]	= permission_str->key_pay;
	input_data[4]	= permission_str->key_date_chage;
	input_data[5]	= permission_str->key_type_chage;
	input_data[6]	= permission_str->key_inhibit;
	input_data[7]	= permission_str->key_inweight;
	input_data[8]	= permission_str->key_discount;

	input_data[9]	= permission_str->clerk_sales;
	input_data[10]	= permission_str->sale_void;
	input_data[11]	= permission_str->sale_addup;
	input_data[12]	= permission_str->sale_return;
	input_data[13]	= permission_str->sale_override;
	input_data[14]  = permission_str->cash_open;
	input_data[15]	= permission_str->edit_menu;
	input_data[16]	= permission_str->edit_plu;
	input_data[17]	= permission_str->edit_price;	// 기능없음
	input_data[18]	= permission_str->edit_global;
	input_data[19]	= permission_str->edit_x_report;
	input_data[20]	= permission_str->edit_z_report;
	input_data[21]	= permission_str->edit_store_data;
//DEL060912	input_data[22]	= permission_str->edit_dc_table;
	input_data[23]	= permission_str->edit_tax_table;
//DEL060912	input_data[24]	= permission_str->edit_tare_table; /* 7200 - MISC PLU KEY? */
//DEL060912	input_data[25]	= permission_str->edit_trace_table;
	input_data[25]	= permission_str->initialization;
//DEL060912	input_data[26]	= permission_str->edit_custom_table;
	input_data[27]	= permission_str->edit_scale_con;
//	input_data[28]	= permission_str->edit_plu_manage;
	input_data[29]  = permission_str->edit_data_table1;
	input_data[30]  = permission_str->edit_data_table2;
	input_data[31]  = permission_str->print_set       ;
	input_data[32]  = permission_str->oper_mode       ;
	input_data[33]  = permission_str->user_parameter  ;
	input_data[34]  = permission_str->communication   ;
	input_data[35]	= permission_str->key_save;
	input_data[36]	= permission_str->reopen;
	input_data[37]	= permission_str->sale_negative;
	input_data[38]	= permission_str->key_tare;
	menu_cnt = 39;
    	Menu_Init();
	caption_split_by_code(0x1855, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_continuous_search = ON;
	caption_continuous_search_addr = 0;
	for (i = 0; i<menu_cnt; i++) {
		if(i == 22 || i == 24 || i == 26 || i == 28)	// 임시 제거
		{
			continue;
		}
		caption_split_by_code(0x1D51+i, &cap,0);
		caption_adjust(&cap,NULL);
		if (caption_run_check(cap.code,cap.runflag)) {
			Menu_InputCreate(1, 
				 (HUGEDATA char *)cap.form, 
				 MENU_TYPE_NUM, 
				 cap.input_x, 
				 cap.input_dlength, 
				 cap.input_length,
				 cap.input_max, 
				 cap.input_min, 
				 0, 
				 &input_data[i], 
				 NULL, NULL, NULL);
		}
	}
	caption_continuous_search = OFF;

PAGE_VIEW:
       	result = Menu_InputDisplay();
	switch(result)
	{
	case MENU_RET_SAVE:
		permission_str->change_item 		= (INT8U)input_data[0];
		permission_str->change_price 		= (INT8U)input_data[1];
		permission_str->key_dept                = (INT8U)input_data[2];
		permission_str->key_pay                 = (INT8U)input_data[3];
		permission_str->key_date_chage          = (INT8U)input_data[4];
		permission_str->key_type_chage          = (INT8U)input_data[5];
		permission_str->key_inhibit             = (INT8U)input_data[6];
		permission_str->key_inweight            = (INT8U)input_data[7];
		permission_str->key_discount            = (INT8U)input_data[8];
		permission_str->clerk_sales             = (INT8U)input_data[9];
		permission_str->sale_void               = (INT8U)input_data[10];
		permission_str->sale_addup              = (INT8U)input_data[11];
		permission_str->sale_return             = (INT8U)input_data[12];
		permission_str->sale_override           = (INT8U)input_data[13];
		permission_str->cash_open               = (INT8U)input_data[14];
		permission_str->edit_menu               = (INT8U)input_data[15];
		permission_str->edit_plu                = (INT8U)input_data[16];
		permission_str->edit_price              = (INT8U)input_data[17];
		permission_str->edit_global             = (INT8U)input_data[18];
		permission_str->edit_x_report           = (INT8U)input_data[19];
		permission_str->edit_z_report           = (INT8U)input_data[20];
		permission_str->edit_store_data         = (INT8U)input_data[21];
//DEL060912		permission_str->edit_dc_table           = (INT8U)input_data[22];
		permission_str->edit_tax_table          = (INT8U)input_data[23];
//DEL060912		permission_str->edit_tare_table         = (INT8U)input_data[24];
//DEL060912		permission_str->edit_trace_table        = (INT8U)input_data[25];
		permission_str->initialization          = (INT8U)input_data[25];
//DEL060912		permission_str->edit_custom_table       = (INT8U)input_data[26];
		permission_str->edit_scale_con          = (INT8U)input_data[27];
//		permission_str->edit_plu_manage         = (INT8U)input_data[28];
		permission_str->edit_data_table1        = (INT8U)input_data[29];
		permission_str->edit_data_table2        = (INT8U)input_data[30];
		permission_str->print_set               = (INT8U)input_data[31];
		permission_str->oper_mode               = (INT8U)input_data[32];
		permission_str->user_parameter          = (INT8U)input_data[33];
		permission_str->communication           = (INT8U)input_data[34];
		permission_str->key_save		= (INT8U)input_data[35];
		permission_str->reopen			= (INT8U)input_data[36];
		permission_str->sale_negative	= (INT8U)input_data[37];
		permission_str->key_tare	    = (INT8U)input_data[38];
		set_base_sparam(addr,(INT8U*)permission_str, str_size );
		return KP_SAVE;
	case MENU_RET_ESC:
		goto RET_ESC;
	default :
		goto PAGE_VIEW;
	}

RET_ESC:
	return KP_ESC;
	// ]			
}

void clerk_save(CLERK_STRUCT *clerk)
{
	INT32U addr;
	if (clerk->code<0) return;
	addr  = (DFLASH_BASE + FLASH_CLERK_AREA);
	addr += clerk->code*CLERK_STRUCT_SIZE;
	Flash_swrite(addr, (HUGEDATA INT8U *)clerk, sizeof(CLERK_STRUCT));
}

#ifndef DISABLE_LIST_MENU
void clerk_list(void)
{		
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	CLERK_STRUCT	clerk_str;
	
//	INT8U	y_gab=8;				// 폰트의 y 크기
//	INT8U	x_gab=6;				// 폰트의 x 크기

	char   	string_buf[64], end_flag, reverse_num;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, str_size;
	INT16S	j, result, min_num, max_num, lcd_size;	
	INT16S	line_id[4];

	INT16U	read_code, title_cap, boundary_cap, input_cap;//, list_title_cap;
	INT32U	addr, addr_flash;
	INT8U	linenum;			    //se-hyung 20071004
		
	// JHC 20040331	<MODIFY>
	// [
	// <Original>
	// if (!clerk_edit_check(&clerk_str)) return;
	// 
	// <Modify>
	if(!permission_check(PMS_SECURITY,0))	return;
	// ]
		
	title_cap    = 0x1853;
	boundary_cap = 0x1D01;
	input_cap    = 0x1E00;
//	list_title_cap = 0x1859;

	str_size = CLERK_STRUCT_SIZE;
	addr_flash = DFLASH_BASE + FLASH_CLERK_AREA;

	lcd_size = Dsp_GetChars();

	caption_split_by_code(boundary_cap, &cap,0);     	// Decide inpput boundary
	min_num	= 1;					// manager list는 출력하지 않는다. 
	max_num = cap.input_max; 			// 



LIST_START:
	end_flag=1;	
	
	Dsp_Fill_Buffer(0);
	//display_title_page(title_cap);
	display_title_page_change(title_cap,1,1);
	linenum = Startup.line_max;			      //se-hyung 20071004 
	if (linenum > 3) linenum = 3;			      //se-hyung 20071004  
	for(i = 0;i<linenum; i++)  {			      //se-hyung 20071004
//	for(i = 0; i<Startup.line_max; i++)  {
		caption_split_by_code(input_cap+i, &cap,0);
		if (i<2) caption_adjust(&cap,NULL);
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
REN1:		switch(i) {
			case 0:	ret_long = start_num;	
				break;
			case 1:	ret_long = end_num;
				break;
 		}

		caption_split_by_code(input_cap+i, &cap,0);
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		 Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		 hebrew_codetemp[strlen(cap.form)] = 0;
		 disp_p.x = 202 - disp_p.x;
		 Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

		 result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, cap.input_dlength, 
				   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		 Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		 arab_codetemp[strlen(cap.form)] = 0;
		 disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		 Dsp_Write_1stringLong(disp_p, arab_codetemp);

		 result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, cap.input_dlength, 
			   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
			
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_dlength, 
				   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
						 
			case KP_ARROW_UP:  if(i == 0)	
						goto REN1;
					   i--;	
					   goto REN1;

			case KP_PAGE_UP:
			case KP_ESC:
				   	goto RET_ESC;

			case KP_SAVE:	   start_num = min_num;
					   end_num = max_num;
					   i++;
					   break;
			case KP_PAGE_DN:
			case KP_ARROW_DN:
			case KP_ENTER:	   
					   if ( ret_long > max_num || ret_long < min_num ) {
						BuzOn(2);	goto REN1;	
				           }
					   if(i == 0)	start_num = ret_long;
					   if(i == 1)	end_num = ret_long;
					   break;
					   				  				
		     	default:	BuzOn(2);	goto REN1;		     	 	
		} //switch (result)
	}//for(i=1; i<3; i++)

	for(i=start_num; i<=end_num; i++) {					// There is no data 
		
		addr = addr_flash + (long)str_size * i;				//
		read_code = Flash_wread(addr);					//													 
 		if(read_code >= min_num && read_code <= max_num ) {	 	//
 			end_flag = 0;						//
			break;							//
		}		 						//
	}									//	
	if(end_flag == 1) {							//
			error_display_caption(0xE00C);				//
			goto LIST_START;					//
	}									//
	cur_id = start_num;
READ_START:	
	j = 0;
	for(i=cur_id; i<=end_num; i++)
	{						
		addr =  addr_flash + (long)str_size * i;
		read_code = Flash_wread(addr);		
		
 		if(read_code >= min_num && read_code <= max_num )
		{	
			line_id[j] = read_code;
			if(j == 0) {
				Dsp_Fill_Buffer(0);
				caption_split_by_code(0x7853, &cap,0);
//				disp_p = point(0,0);
				disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071004  
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

			Flash_sread(addr, (HUGEDATA INT8U*)&clerk_str, str_size ); 			
		 	sprintf(string_buf, "%2d   %2d    %s", clerk_str.code, clerk_str.permissionLevel, clerk_str.name);
			string_buf[lcd_size] = 0;	       		
			disp_p = point((j+Startup.menu_type)*Startup.menu_ygap, 0);	       		 

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
 			Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
#endif
 			Dsp_Diffuse();	
			j++;
		}
				
		if(i == end_num )  end_flag = 1;
		else	end_flag = 0; 

KEY_WAITE:	
		if(j > 2 || end_flag == 1) {
			while(!KEY_Read());		BuzOn(1);
			if(KeyDrv.CnvCode == KP_ESC)	goto RET_ESC;			
			
			switch(KeyDrv.CnvCode) {

				case KP_ARROW_UP:
						cur_id = line_id[0]-1;						
						while(1)  {
							if(cur_id <= start_num) {
								cur_id = start_num;
								goto READ_START;
							}
							addr =  addr_flash + (long)str_size * (cur_id);			
							read_code = Flash_wread(addr);
							if(read_code >= min_num && read_code <= max_num )  {
								cur_id = read_code;								    
								goto READ_START;
							}
							cur_id--;
						}
									       
				case KP_PAGE_UP :
						cur_id = line_id[0]-1;						
								
						reverse_num = 0;

						while(1)  {
							if(cur_id <= start_num) {
								cur_id = start_num;
								goto READ_START;
							}
							addr =  addr_flash + (long)str_size * (cur_id);			
							read_code = Flash_wread(addr);
  							
   							if(read_code >= min_num && read_code <= max_num )  {
							       	reverse_num++;
								if(reverse_num ==3) {									
									cur_id = read_code;  
									goto READ_START;
								}
							}
							cur_id--;
						}

				case KP_PAGE_DN :
				case KP_ENTER:
						if(end_flag == 1)  {
							error_display_caption(0xE00E); 
							goto KEY_WAITE;
						} else {
							cur_id = line_id[2]+1;
							goto  	READ_START;
						}
						
				case KP_ARROW_DN:
						if(end_flag == 1)  {
							error_display_caption(0xE00E); 
							goto KEY_WAITE;
						} else {
							cur_id = line_id[1];
							goto   	READ_START;
						}						
				
				// Added by CJk
				case KP_TEST:
						clerk_print_list(start_num, end_num);
						goto KEY_WAITE;		
				///////////////////////////////
				case KP_FEED:
						keyapp_feed(OFF);
						goto KEY_WAITE;

				default 	:	
				    BuzOn(1);
				    goto KEY_WAITE;
			} //switch(KeyDrv.CnvCode)

		} //if(j > 2)
	}//for(i=start_num; i<=end_num ; i++)

	RET_ESC:    return;
}
#endif //#ifndef DISABLE_LIST_MENU

#define M_ID_CLERKKEY_KNO	1
#define M_ID_CLERKKEY_TNO	2

void callback_end_clerkkey(INT32U num)
{
	INT32U addr_key;
	INT8U  id_clerk_no;
	INT32U clerk_no;

	id_clerk_no = Menu_GetMenuId(M_ID_CLERKKEY_TNO);

	addr_key = FLASH_KEY_CDTC_AREA + DFLASH_BASE;
	addr_key += (num - 1) * 2;
	clerk_no = Flash_wread(addr_key);

	if(id_clerk_no != 0xff) *Menu[id_clerk_no].pval = clerk_no;

//      	MenuRefresh = ON;
}

void clerk_key_define(void)
{
	CAPTION_STRUCT cap;
	INT16S 	result;
	INT32U  addr_key;
	INT32U  clerk_key_no, clerk_no;
	INT32U  clerk_key_max;

	clerk_key_no = 1;
	clerk_no = 1;

	clerk_zero_allow=0;
	while(1)
	{
		addr_key = FLASH_KEY_CDTC_AREA + DFLASH_BASE;
		addr_key += (clerk_key_no - 1) * 2;
		clerk_no = Flash_wread(addr_key);

		Menu_Init();

		caption_split_by_code(0x1856, &cap,0);//""
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x3429, &cap,0);//""
		clerk_key_max = cap.input_max;
		//if (clerk_key_max > MAX_TARE_KEY) clerk_key_max = MAX_TARE_KEY;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_CLERKKEY_KNO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &clerk_key_no, NULL, NULL, NULL);
		Menu_AddEndFunction(M_ID_CLERKKEY_KNO, callback_end_clerkkey);
		Menu_SetKeyMode(M_ID_CLERKKEY_KNO, 3);

		caption_split_by_code(0x1D01, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_CLERKKEY_TNO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &clerk_no, NULL, NULL, callback_clerk_name);						

		result = Menu_InputDisplay();
		if (result == MENU_RET_SAVE) 
		{
			addr_key = FLASH_KEY_CDTC_AREA + DFLASH_BASE;
			addr_key += (clerk_key_no - 1) * 2;
			Flash_wwrite(addr_key, clerk_no);
			clerk_key_no++;
			if (clerk_key_no > clerk_key_max) clerk_key_no = clerk_key_max;
		}
		else if (result == MENU_RET_ESC)
		{
			break;
		}
	}
}

