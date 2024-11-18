/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	origin.c
|*  Version			:	1.0
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created			:	2003/12/10
|*  Description		:	place of produce (origin)   
|*              				
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
#include "bar_common.h"
#include "prt_interpreter.h"
#include "prt_cmd.h"

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

#include "origin.h"
#include "pgm_app.h"
#include "menu_code_text1.h"
#include "prt_list.h"
#include "mode_error.h"

HUGEDATA  ORIGIN_STRUCT  origin_str;

INT16S origin_back_search(INT16S start_num, INT16S current_num, INT16S Max, INT16S Min, INT16S ret_num);


void	origin_create(void)			// command code  0x1251
{
	INT16U cap_code[2]={55,0x3252};
	menu1_create(0x1251, cap_code, FLASH_ORIGIN_AREA, ORIGIN_STRUCT_SIZE, OFF);
}

void    origin_delete(void)		// command code  0x1253
{	
	menu1_delete(0x1253, 55, FLASH_ORIGIN_AREA, ORIGIN_STRUCT_SIZE);			
}

#ifndef DISABLE_LIST_MENU
void    origin_list(void)			// command code  0x1223
{		
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	ORIGIN_STRUCT	origin_str;
	
	char   	string_buf[34], end_flag = 0;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, reverse_num;
	INT16S	j = 0, result, min_num, max_num, lcd_size;
	
	INT16U	read_code, line_ct, start_id;
	INT32U	addr, addr_flash;//, cap_addr;	
	INT16U	code = 0x1252;//,cap_code = 0x1E00;

LIST_START:
	caption_split_by_code(55, &cap, 0);
	cap.input_max = MAX_ORIGIN_NO;
	min_num	= cap.input_min;					//
	max_num = cap.input_max; 					// 
	if(min_num <= 0)  {
		disp_p = point(0,0);
		message_error(disp_p);
	}

	Dsp_Fill_Buffer(0);
	//display_title_page(code);
	display_title_page_change(code,1,1);

	for(i = 0; i < 3; i++)  {
		caption_split_by_code(0x1E00+i, &cap,0);
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
REN1:		if(i == 0)	ret_long = start_num;		
		if(i == 1)	ret_long = end_num;
		caption_split_by_code(0x1E00+i, &cap,0);
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

		result=keyin_ulong(KI_GRAPHIC, 
				   KI_FORM_ILNN,	
				   0,
				   (long*)&ret_long, 
				   4, 
				   (i+Startup.menu_type)*Startup.menu_ygap,  
				   cap.input_x*Startup.menu_xgap, 
				   0,
				   KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);

		result=keyin_ulong(KI_GRAPHIC, 
				   KI_FORM_ILNN,	
				   0,
				   (long*)&ret_long, 
				   4, 
				   (i+Startup.menu_type)*Startup.menu_ygap,  
				   cap.input_x*Startup.menu_xgap, 
				   0,
				   KI_NOPASS );
#else
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
#endif
		switch (result){
			case KP_ARROW_UP:  // <CODE> JHC, 040531, Modify
					   // [
					   if(i > 0)  	i--;
					   else		BuzOn(2);
					   // ]
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
	line_ct    = Startup.line_max; // (Startup.menu_type?3:2);
	addr_flash = DFLASH_BASE + FLASH_ORIGIN_AREA;
	cur_id     = start_num;
	start_id   = start_num;

	// <DEBG> JHC, 040329, New, -DebugCode
	// [
	// $1: JHC, 040529, Comment, -Not use
	// display_ulong(1, start_num, 0);
	// VFD7_Diffuse();
	// ]

READ_START:
	// <CODE> JHC, 040528, Modify
	// [
	// $MEMO: -modify display list all
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		// <CODE> JHC, 040617, Modify, -0x1239 to 0x1259
		// [
//		caption_split_by_code(0x1259, &cap);
		caption_split_by_code(0x71FF, &cap,0);
		// ]
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
		addr =  addr_flash + (long)ORIGIN_STRUCT_SIZE * (cur_id-1);			
		read_code = Flash_wread(addr);
		if(read_code == cur_id /*>= min_num && read_code <= max_num*/)  
		{	
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&origin_str, ORIGIN_STRUCT_SIZE ); 
		 	origin_str.origin_text[23] = 0;
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
			sprintf(string_buf, 
		 		"%3d %-14s", 
				origin_str.origin_code, 
				&origin_str.origin_text);
#else
			sprintf(string_buf, 
		 		"%3d %-14s", 
				origin_str.origin_code, 
				&origin_str.origin_text);
#endif
			string_buf[lcd_size] = 0;
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)origin_str.origin_text, strlen(origin_str.origin_text), 0, 0);
			hebrew_codetemp[strlen(origin_str.origin_text)] = 0;
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
			if (j == 1)
				start_id = cur_id;	
			
			if (j == line_ct)
				break;
		}
		
		cur_id++;	
	}
	Dsp_Diffuse();
	// <CODE> JHC, 040531, ADD
	// [
	// $1: JHC, 040607, Modify
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
			addr =  addr_flash + (long)ORIGIN_STRUCT_SIZE * (cur_id-1);			
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
			addr =  addr_flash + (long)ORIGIN_STRUCT_SIZE * (cur_id-1);			
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
			addr =  addr_flash + (long)ORIGIN_STRUCT_SIZE * (cur_id-1);			
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
			addr =  addr_flash + (long)ORIGIN_STRUCT_SIZE * (reverse_num-1);			
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
		origin_print_list(start_num, end_num);
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
#endif
