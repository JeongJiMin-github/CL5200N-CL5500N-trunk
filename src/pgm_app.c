/*****************************************************************************
 |*		       
 |*  Copyright		:	(c) 2004 CAS
 |*  Filename		:	pgm_app.c
 |*  Version		:	v 0.5
 |*  Programmer(s)	:	getwing (Jae-Ok Kim)
 |*  Created		:	2004/01/04
 |*  Description		:	Program mode에서 편리하게 사용할수 있는 함수들
 |*  Edit		:       20040319 -> error_display_message함수추가
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "globals.h"

#include "commbuf.h"
#include "prtdrv.h"
#include "serial.h"
#include "flash.h"
#include "sensor.h"
#include "ram.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
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

#include "pgm_app.h"
#include "adm_app.h"
#include "department.h"
#include "tax.h"
#include "tare_file.h"
#include "pcs_unit.h"
#include "menu_code_text1.h"
#include "discount.h"
#include "origin.h"
#include "traceability.h"
#include "nutrition_fact.h"
#include "barcode_file.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "plu_edit.h"
#include "rtc.h"
#include "common_sunday.h"
#include "clerk_app.h"
#include "mode_sale.h"
#include "prt_list.h"
#include "ingredient.h"
#include "workplace.h"
#include "adm.h"
#include "mode_error.h"
#include "sale_report.h"

extern INT16S plu_record_size;
long	speedkey_set_number;
long	speedkey_plu_number;
long	speedkey_key_number;
INT16S	keycall_key_position_y;
long    skeyin_define;
INT16S	CallbackPluEditPosition_x;
INT16S	CallbackPluEditPosition_y;
INT16U	callback_dept_no=0;
extern  INT8U temp_callback_discount_flag;
extern  INT16S  PutNSmallString(INT16S y, INT16S x,char *data, INT16S n);



void 	display_title_page_change(INT16U cap_code, INT8U current_page, INT8U total_page)
{
	CAPTION_STRUCT cap;
	POINT 	disp_p;
	INT16S	disp_length;
	char	page_view[10];
	
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	if (Startup.menu_type==0) {
		display_clear(DISPLAY_PRICE);
		if (status_run.run_mode == RUN_SALE) {
			display_clear(DISPLAY_TARE);
			display_clear(DISPLAY_WEIGHT);
			display_clear(DISPLAY_UNITPRICE);
			strcpy(page_view,"  EDT");
			//			display_string(DISPLAY_TARE,(INT8U *)page_view);
			display_string(DISPLAY_WEIGHT,(INT8U *)page_view);
			//			display_string(DISPLAY_UNITPRICE,(INT8U *)page_view);
		}
		sprintf(page_view,"%02d-%02d ", current_page, total_page);
		display_string(2,(INT8U *)page_view);
		VFD7_Diffuse();
		return;
	}
#endif
	
	menu_display_menucode(CurMenuCode, 1);
	
	caption_split_by_code(cap_code, &cap,0);
	if(strlen((char *)cap.form) > DISP_TITLE_LENGTH)
		cap.form[DISP_TITLE_LENGTH] = 0;
	_strupr((char *) cap.form);
	
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)&cap.form[3]);
#else
	disp_p = point((Startup.menu_type-1)*Startup.menu_ygap, DISP_TITLE_START_POSITION*Startup.menu_xgap);
#ifdef HEBREW_FONT
	//disp_length = strlen((char *)cap.form);
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
	
	sprintf(page_view, "(%d/%d)", current_page, total_page);
 	disp_length=Dsp_GetChars();
 	disp_length -= strlen(page_view);
	disp_p = point((Startup.menu_type-1)*Startup.menu_ygap, disp_length*Startup.menu_xgap);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)page_view, strlen(page_view), 0, 0);	
	hebrew_codetemp[disp_length] = 0;
	disp_p.x = 202 - disp_p.x;	//X 좌표 변환
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)page_view, strlen(page_view), 0, 0, 0); 
	arab_codetemp[disp_length] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, page_view);
#endif
#endif
}

void parameter_title_page_change(INT16U cap_code, INT8U current_page, INT8U total_page)
{
	CAPTION_STRUCT cap;
	POINT 	disp_p;
	INT16S	strno;
	char	*sp;
	
	/*
	 if (Startup.menu_type==0) {
	 display_clear(2);
	 sprintf(page_view,"%02d-%02d ", current_page, total_page);
	 display_string(2,(INT8U *)page_view);
	 VFD7_Diffuse();
	 Dsp_Diffuse();
	 return;
	 }
	 */
	menu_writepage(total_page, current_page);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	if (Startup.menu_type==0) return;
#endif
	
	menu_display_menucode(CurMenuCode, 1);
	caption_split_by_code(cap_code, &cap,0);
	
	sp=strchr((char *)cap.form,':');
	if (sp) {
		strno=(INT16S)(sp-(char *)cap.form);
		cap.form[strno]=0;
	}
	
	if(strlen((char *)cap.form) > DISP_TITLE_LENGTH-3) {	// Title Code가 아니므로 cap.form[0]부터 string 시작
		cap.form[DISP_TITLE_LENGTH-3] = 0;
	}
	
	//@@	if(cap.input_x > 3)
	//@@		cap.form[cap.input_x-3] = 0;
	//@@
	//@@	if(strlen((char *)cap.form) > DISP_TITLE_LENGTH)
	//@@		cap.form[DISP_TITLE_LENGTH] = 0;
	
	_strupr((char *) cap.form);
	
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)cap.form);
#else
	disp_p = point((Startup.menu_type-1)*Startup.menu_ygap, DISP_TITLE_START_POSITION*Startup.menu_xgap);
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
#endif
}

void 	display_title_num_page(INT16U cap_code,INT8U xmode, INT32U _number, INT8U current_page, INT8U total_page)
{
	CAPTION_STRUCT cap;
	POINT 	disp_p;
	char	buffer[34];
	/*
	 if (Startup.menu_type==0) {
	 display_clear(2);
	 sprintf(buffer,"%02d-%02d ",current_page,total_page);
	 display_string(2,(INT8U *)buffer);
	 VFD7_Diffuse();
	 Dsp_Diffuse();
	 return;
	 }
	 */
	menu_writepage(total_page, current_page);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	if (Startup.menu_type == 0) return;
#endif
	
	menu_display_menucode(CurMenuCode, 1);
	if (cap_code<0x0fff) {
		plu_get_display_info(cap_code,&cap,1);
	} else {
		caption_split_by_code(cap_code, &cap,0);
	}
	if (xmode) {
		sprintf(buffer, "%s %ld", cap.form, _number);
	} else {
		sprintf(buffer,"%s",&cap.form[3]);
	}
	_strupr((char *) buffer);
	
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_WEIGHT);
	display_string(DISPLAY_WEIGHT,(INT8U *)buffer);
#else
	disp_p = point((Startup.menu_type-1)*Startup.menu_ygap, DISP_TITLE_START_POSITION*Startup.menu_xgap);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#endif
}

#ifndef DISABLE_LIST_MENU
void display_pgm_list(INT16U title_cap, INT16U boundary_cap, INT16U *input_cap, INT16U list_cap, INT32U _menu_area, INT16U str_size)
{		
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	MENU_TYPE1_STRUCT	menu_type1_str;
#ifdef USE_VIETNAM_LOTTEMART_DEFAULT
	char   	string_buf[210], end_flag;
#else
	char   	string_buf[64], end_flag;
#endif
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id;
	INT16S	j, result, min_num, max_num, lcd_size;	
	INT16S	line_ct, start_id;
	INT16U	read_code, reverse_num;
	INT32U	addr, addr_flash;//, cap_addr;
	INT8U	linenum;			//se-hyung 20071001
	
	end_flag=1;
	lcd_size = Dsp_GetChars();
	
	LIST_START:
	caption_split_by_code(boundary_cap, &cap,0);
	caption_adjust(&cap,NULL);
	min_num	= cap.input_min;					//
	max_num = cap.input_max; 					// 
	if(min_num <= 0)  {
		//		sprintf(string_buf, "caption error");
		disp_p = point(0,0);
		//		Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
		message_error(disp_p);
	}
	
	Dsp_Fill_Buffer(0);
	//display_title_page(title_cap);
	display_title_page_change(title_cap,1,1);
	//	addr = caption_search_by_code(input_cap);
	//	for(i = 0;i<Startup.line_max; i++)  {	  //se-hyung 20071001
	linenum = Startup.line_max;			      //se-hyung 20071001
	if (linenum > 3) linenum = 3;			      //se-hyung 20071001
	for(i = 0;i<linenum; i++)  {			      //se-hyung 20071001
		caption_split_by_code(input_cap[i], &cap,0);
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
	end_num   = max_num;
	for(i=0; i<2; i++)  {
		REN1:
		if(i == 0)	ret_long = start_num;		
		if(i == 1)	ret_long = end_num;
		caption_split_by_code(input_cap[i], &cap,0);
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
			// <CODE> JHC, 040528, Modify
			// [
			// $1: JHC, 040531, Modify
			case KP_ARROW_UP:  if(i > 0)	i--;
			else		BuzOn(2);
			goto REN1;    					   
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
					// <CODE> JHC, 040524, ADD
					// [
					// $1: JHC, 040607, Modify
					//error_display_caption(0xE00E);
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
	
	addr_flash = DFLASH_BASE + _menu_area;
	lcd_size   = Dsp_GetChars();
	line_ct    = Startup.line_max; // (Startup.menu_type?3:2);
	cur_id     = start_num;
	start_id   = start_num;
	
	// <DEBG> JHC, 040329, New, -DebugCode
	// [
	// $1: JHC, 040529, Comment, -Not use
	// display_ulong(1, list_cap, 0);
	// VFD7_Diffuse();
	// ]
	
	READ_START:
				// <CODE> JHC, 040531, Modify
				// [
				// $MEMO: -modify display list all
				Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(list_cap, &cap, 0);
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
			Flash_sread(addr, (HUGEDATA INT8U*)&menu_type1_str, str_size ); 
			menu_type1_str.text[28] = 0;	// Added by CJK 20040827
			sprintf(string_buf, 
		 		"%3d %s", 
		 		menu_type1_str.code,
		 		menu_type1_str.text);
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
	// $1: JHC, 040607, Modify, -no data message
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
						table_print_list(title_cap, _menu_area, str_size, start_num, end_num);
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

void plucall_department(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	DEPARTMENT_STRUCT	department_str;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT32U	addr;
	INT16S  	str_size,w;
	
	callback_dept_no = num;
	
	str_size = DEPARTMENT_STRUCT_SIZE;
	
	//	disp_p = point(plucall_department_pos_y*Startup.menu_ygap, plucall_department_position*Startup.menu_xgap);
	//	w = Dsp_GetChars();
	//	w -= plucall_department_position;
	//	if (w > MAX_MENU_CHAR_BUF - 1) w = MAX_MENU_CHAR_BUF - 1;
	//
	//	memset((INT8U*)buffer, 0x20, w);
	//
	//	buffer[w] = 0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
  #ifdef USE_ARAB_CONVERT
    disp_p = point((CallbackEditPosition_y+2)*Startup.menu_ygap, 0);
  #else
    disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
  #endif
#else
	disp_p = point(0, 0);
#endif
	w = Dsp_GetChars();
#ifndef USE_ARAB_CONVERT
	w -= CallbackEditPosition_x;
#endif
#ifdef USE_PREVENT_CALLBACK_DISP_OVER    
	if((INT16S)w < 0) w = 0;
#endif  
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
	////DEL071016	DspStruct.DirectDraw = 1;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	display_lcd_clear();
#endif
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
	hebrew_codetemp[25-CallbackEditPosition_x] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	plu_get_display_info(PTYPE_DEPARTMENT_NO,&cap,0);
	caption_adjust(&cap,NULL);
	if (cap.input_max ==0) {
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
	addr += DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&department_str, str_size);
	if(department_str._code>cap.input_max || department_str._code<cap.input_min) {	
		caption_split_by_code(0xE00C,&cap,0);
		strcpy(buffer,cap.form);
	} else {
		memcpy((INT8U*)buffer, department_str._description, sizeof(department_str._description));
	}
	////DEL071016	DspStruct.DirectDraw = 1;
	if (w >	sizeof(department_str._description)) w = sizeof(department_str._description);
	if (IsHanCharMid(buffer, w) == CHAR_HAN_MID) //Modified by JJANG 20081202 한글일 때 글씨 깨지는 현상 방지.		
	{
		w--;
	}
	buffer[w] = 0;
	DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[25-CallbackEditPosition_x] = 0;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)	//기존 아랍ROM 적용되어 있음
#ifndef _USE_LCD37D_40DOT_
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	display_lcd_clear();
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	display_lcd_clear();
#endif
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	DspStruct.Reverse = 0;
}

INT8U check_plu_weightunit(INT8U *num)	// 0:kg, 1:lb, 0xff:range over
{
	INT8U unit;
	
	unit = ADM_GetData.CurUnit;
	if(GlbOper.EnableKgLb) {
		if(*num > 2 || *num == 0) *num = 0xff;//*num = 0xff;
		else {
			if(ADM_Status.UseUnit == WEIGHT_UNIT_LB) {	// 1:1lb, 2:1kg, 3:100g
				if(*num > 1) {
					*num -= 1;	// 2 -> 1(KG)
					unit = WEIGHT_UNIT_KG;
				} else {
					unit = WEIGHT_UNIT_LB;
				}
			} else {					// 1:1kg, 2:100g, 3:1lb
				if(*num > 1) {
					*num -= 1;	// 2 -> 1(LB)
					unit = WEIGHT_UNIT_LB;
				} else {
					unit = WEIGHT_UNIT_KG;
				}
			}
		}
	}
	return unit;
}



void plucall_unitweight(long num)
{	
	POINT 	disp_p;
	char	buffer[64];
	INT8U	unit,temp_num;
	CAPTION_STRUCT 		cap;
	INT16U 	w;
	
	temp_num = (INT8U)num;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	display_lcd_clear();
#endif
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	unit = check_plu_weightunit(&temp_num);
	
	if(temp_num == 0xff) {
		caption_split_by_code(0xe102, &cap,0);
		strcpy(buffer, cap.form);
	} else {
		if(unit == WEIGHT_UNIT_LB) {
			switch(temp_num)  {
				case 1:   //sprintf(buffer, "1 lb");
					//caption_split_by_code(0xc733, &cap,0);	
					//break;
					//case 2:   //sprintf(buffer, "1/2 lb");
					//caption_split_by_code(0xc734, &cap,0);	
					//break;
					//case 3:   //sprintf(buffer, "1/4 lb");
					//caption_split_by_code(0xc735, &cap,0);	
					caption_split_by_code(0xc733+temp_num-1, &cap,0);	
					break;
				default:
					caption_split_by_code(0xe102, &cap,0);	
					break;
			}
			strcpy(buffer, cap.form);
		} else  {
			switch(temp_num)  {
				case 1:	  //sprintf(buffer, "1 kg");
					//caption_split_by_code(0xc731, &cap,0);	
					//break;
					//case 2:	  //sprintf(buffer, "100 g");
					//caption_split_by_code(0xc732, &cap,0);	
					caption_split_by_code(0xc731+temp_num-1, &cap,0);	
					break;
				default:
					caption_split_by_code(0xe102, &cap,0);	
					break;
			}
			strcpy(buffer, cap.form);
		}
	}
	DspStruct.Reverse = 1;
	buffer[w] = 0;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[strlen(buffer)] = 0;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, buffer);
#endif
	DspStruct.Reverse = 0; 
} 

void plucall_plutype(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	DEPARTMENT_STRUCT	department_str;
	char	buffer[64];
	INT8U flag;
	INT8U flag1;
	INT16U 	w;
	
	flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);
	flag1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	display_lcd_clear();
#endif
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
	hebrew_codetemp[25-CallbackEditPosition_x] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[25-CallbackEditPosition_x] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	switch(num) {
		case PLU_BY_WEIGHT:
			caption_split_by_code(0xC710, &cap,0);
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
			strcat((char *)cap.form, "(혼용)");
#endif
			break;
		case PLU_BY_COUNT:
			if (!(flag1&0x04)) goto NO_F;
			caption_split_by_code(0xC711, &cap,0);
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
			strcat((char *)cap.form, "(혼용)");
#endif
			break;
		case PLU_BY_PCS:
			if (!(flag&0x01)) goto NO_F;
			caption_split_by_code(0xC712, &cap,0);
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
			strcat((char *)cap.form, "(혼용)");
#endif
			break;
#ifdef USE_FIXED_PLUTYPE//USE_EMART_NETWORK
		case PLU_BY_WEIGHT_FIX:
			caption_split_by_code(0xC710, &cap,0);
			break;
		case PLU_BY_COUNT_FIX:
			if (!(flag1&0x04)) goto NO_F;
			caption_split_by_code(0xC711, &cap,0);
			break;
		case PLU_BY_PCS_FIX:
			if (!(flag&0x01)) goto NO_F;
			caption_split_by_code(0xC712, &cap,0);
			break;
#endif
		default:
			NO_F:
			//			strcpy(cap.form,global_message[MSG_INVALID_VALUE]);
			caption_split_by_code(0xF039, &cap,0);
			break;
	}
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	display_lcd_clear();
#endif
	DspStruct.Reverse = 1;
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
	DspStruct.Reverse = 0;
}

#if defined (USE_RUSSIA_LOGISTICS_PLU) || defined (USE_EUR_LOGISTICS_PLU)
void plucall_gtintype(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char	buffer[64];
	INT16S  	str_size;
	INT16U 	w;
	char  gtin_form[34];

	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	switch(num) {
		case 0 :
			caption_split_by_code(0xF033, &cap,0);
			break;
		case 1:
			strcpy(cap.form, "GTIN_01");
			break;
		case 2:
			strcpy(cap.form, "GTIN_02");
			break;
		case 3:
			caption_split_by_code(0xF033, &cap,0);
			break;
		default:
			caption_split_by_code(0xE102, &cap,0);
			break;
	}
	DspStruct.Reverse = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
	DspStruct.Reverse = 0;
}
#endif // #ifdef USE_RUSSIA_LOGISTICS_PLU

#ifdef USE_TAX_RATE
void plucall_taxcode(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	TAX_STRUCT		tax_str;
	INT8U	buffer[64];
	INT32U	addr;//, cap_addr;
	INT16S  	str_size;
	INT16U 	w;			    
	
	str_size = sizeof(tax_str);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
	w = Dsp_GetChars();
#ifndef USE_ARAB_CONVERT
	w -= CallbackEditPosition_x; // taxcode caption len > w 인경우 마지막 글자가 지워지지 않고 잔상으로 남음.
#endif
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
        caption_split_by_code(0x1230, &cap,0);
        strcpy((char*)buffer, (char *)&cap.form[2]);
        strcat((char *)buffer, " ");
	#endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		//caption_split_by_code(0xF036, &cap,0);
		caption_split_by_code(0xF033, &cap,0);
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1230, &cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	caption_split_by_code(8, &cap,0);
	
	if (cap.input_max ==0) {
		//		sprintf((char*)buffer, "caption error");
		//		DspStruct.DirectDraw = 1;
		//		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
		message_error(disp_p);
		return;
	}
	
	if ((num<cap.input_min) || (num>cap.input_max)) {		
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	
	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_TAX_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&tax_str, str_size);
	if(tax_str.tax_code>cap.input_max || tax_str.tax_code<cap.input_min) {
		caption_split_by_code(0xE00C, &cap,0);
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
		return;	
	} else {
		sprintf((char*)buffer, "Type%d,%5.2f%%", tax_str.tax_type, (float)tax_str.tax_rate/100);
		buffer[63] = 0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
	}       
}
#endif

void plucall_group(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	MENU_TYPE1_STRUCT	group_str;	
	INT8U	buffer[64];
	INT32U	addr;//, cap_addr;
	INT16S  	str_size;
	INT16U 	w;
	
	str_size = 32;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	//sprintf(MsgBuf, "(gr)[w=%d,x=%d,y=%d]\r\n", w, CallbackEditPosition_x, CallbackEditPosition_y);
	//MsgOut(MsgBuf);    
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1220, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
    #endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		caption_split_by_code(0xF033, &cap,0);
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1220, &cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	
	
	//	cap_addr = caption_search_by_code(0x122A);			// Boundary of input
	//	_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
	caption_split_by_code(9, &cap,0);
	if (cap.input_max ==0) {
		//		sprintf((char*)buffer, "caption error");
		//		DspStruct.DirectDraw = 1;
		//		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
		message_error(disp_p);
		return;
	}
	
	if ((num<cap.input_min) || (num>cap.input_max)) {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	
	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_SUB_DEPARTMENT_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&group_str, str_size);
	
	//if(group_str.code>cap.input_max || group_str.code<cap.input_min) {	
	if(group_str.code != num) {
		caption_split_by_code(0xE00C, &cap,0);
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
		return;	
	} else {
		sprintf((char*)buffer, "  %s", group_str.text);
		buffer[63] = 0;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//기존 아랍ROM 적용되어 있음
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
	    Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
		eng_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#else
		sprintf((char*)buffer, "  %s", group_str.text);
		
		buffer[63] = 0;
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	}       
}

void plucall_fixed(long num)
{
	CAPTION_STRUCT 	 cap;
	POINT 		 disp_p;
	INT8U	buffer[64];
	INT16U  cap_addr;
	INT16U 	w;
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
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
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	if ((num==0) || (num==1)) {
		cap_addr = 0xF040;
		cap_addr+= (INT16U)num;
		caption_split_by_code(cap_addr,&cap,0);
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
		hebrew_codetemp[strlen(cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
		arab_codetemp[strlen((char *)cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	if ((num<0) || (num>1)) {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
}

void plucall_tareid(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	TARE_STRUCT		tare_str;
	char    unit_str[5];
	INT8U	buffer[64],v8_1,v8_2;
	INT32U	addr;
	INT16S  	str_size,r;
	INT16U 	w;
	
	str_size = sizeof(tare_str);
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
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
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1270, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
    #endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		//caption_split_by_code(0xF037, &cap,0);
		caption_split_by_code(0xF033, &cap,0);
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1270, &cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	// [
	//caption_split_by_code(0x3271, &cap,0);
	caption_split_by_code(12, &cap,0);
	//]
	
	if ((num<cap.input_min) || (num>cap.input_max)) {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)&global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0); 
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	
	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_TARE_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&tare_str, str_size);
	
	if(GlbOper.EnableKgLb) {
		tare_str._weightunit = ADM_Status.UseUnit;
	}
	if(tare_str._code>cap.input_max || tare_str._code<cap.input_min) {	
		caption_split_by_code(0xE00C, &cap,0);
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
		return;	
	} else {
		//v8_1 = get_decimal_pt(6, 0);
		v8_1 = get_decimal_pt(3, tare_str._weightunit);
		v8_2 = get_global_bparam(GLOBAL_DECIMAL_SIGN);
		
		//GetWeightSymbol(CurPluWeightUnit, unit_str);// Modified by CJK 20051223
		GetWeightSymbol(tare_str._weightunit, unit_str);// Modified by CJK 20060406
		
		//DEL20060406		switch(tare_str._type)  {
		//DEL20060406			case 1:
		//DEL20060406			case 2: 
		format_value(buffer,tare_str._value,v8_1,v8_2);
		strcat((char *)buffer,unit_str);
		//DEL20060406				break;
		//DEL20060406			case 3: sprintf((char*)buffer, "%6.2f%%",(float)tare_str._value/100);
		//DEL20060406				break;
		//DEL20060406		}
		r=strlen((char *)buffer);
		sprintf((char *)&buffer[r]," %s",tare_str._description);
		buffer[31] = 0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//기존 아랍ROM 적용되어 있음
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
		eng_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else

		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
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
}

void plucall_pcscode(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	MENU_TYPE1_STRUCT	pcscode_str;
	
	//	INT8U	y_gab=8;				// 폰트의 y 크기
	//	INT8U	x_gab=6;				// 폰트의 x 크기
	
	INT8U	buffer[64];
	
	INT32U	addr; //, cap_addr;
	INT16S  	str_size;
	INT16U 	w;
	
	str_size = PCS_UNIT_STRUCT_SIZE;
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
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
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1280, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
    #endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		caption_split_by_code(0xF033,&cap,0);
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1280,&cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	caption_split_by_code(15,&cap,0);
	if (cap.input_max ==0) {
		message_error(disp_p);
		return;
	}
	
	if ((num<cap.input_min) || (num>cap.input_max)) {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)&global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	
	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_PCS_UNIT_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&pcscode_str, str_size);
	
	if(pcscode_str.code>cap.input_max || pcscode_str.code<cap.input_min) {	
		//		cap_addr = caption_search_by_code(0xE00C);
		//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		caption_split_by_code(0xE00C,&cap,0);
		DspStruct.Reverse = 1;
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
		DspStruct.Reverse = 0;
		return;	
	} else {
		sprintf((char*)buffer, "%s", pcscode_str.text);
		buffer[63] = 0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
		eng_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else

		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
	}       
}

void plucall_ingredient(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	INT8U	buffer[64];
	INT16U  ret;
	INT16U 	w;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
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
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1310, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
    #endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		caption_split_by_code(0xF033,&cap,0);
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1310,&cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	caption_split_by_code(25,&cap,0);
	if (cap.input_max ==0) {
		message_error(disp_p);
		return;
	}
	
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
	if ((num<cap.input_min) || (num>MAX_INGREDIENT_INDEX_NUMBER)) 
#else
	if ((num<cap.input_min) || (num>MAX_INGREDIENT_NO))
#endif
	{
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}

	ret=ingredient_get_data((INT16U)num,(INT8U *)&buffer,32);
	if (!ret) {
		caption_split_by_code(0xE00C,&cap,0);
		DspStruct.Reverse = 1;
#ifdef USE_ARAB_CONVERT
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0);
		arab_codetemp[strlen((char *)cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
		DspStruct.Reverse = 0;
	} else {
		buffer[63] = 0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//callback ingredient, 기존 아랍ROM 적용되어 있음
#ifndef _USE_LCD37D_40DOT_
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
	}       
}

#ifdef USE_TRACEABILITY
void plucall_traceability(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	TRACEABILITY_STRUCT	traceability_str;
	
	//	INT8U	y_gab=8;				// 폰트의 y 크기
	//	INT8U	x_gab=6;				// 폰트의 x 크기
	
	INT8U	buffer[64];
	
	INT32U	addr;//, cap_addr;
	INT16S  	str_size;
	INT16U 	w;
	
	str_size = TRACEABILITY_STRUCT_SIZE;
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
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
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1330, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
    #endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		//		cap_addr = caption_search_by_code(0xF033);
		//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		caption_split_by_code(0xF033,&cap,0);
		strcpy((char *)buffer, (char *) cap.form);
		//		cap_addr = caption_search_by_code(0x1330);
		//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1330,&cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);	// " Tracability"
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	
	
	caption_split_by_code(35,&cap,0);
	caption_adjust(&cap,NULL);
	if (cap.input_max ==0) {
		message_error(disp_p);
		return;
	}
	
	if ((num<cap.input_min) || (num>cap.input_max)) {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	
	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_TRACEABILITY_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&traceability_str, str_size);
	
	if(traceability_str.code>cap.input_max || traceability_str.code<cap.input_min) {	
		//		cap_addr = caption_search_by_code(0xE00C);
		//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		caption_split_by_code(0xE00C,&cap,0);
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
		return;	
	} else {
		sprintf((char*)buffer, "%s",traceability_str.contents);
		buffer[63] = 0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//callback tracebility
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
		eng_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
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
}
#endif


void plucall_origin(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	ORIGIN_STRUCT		origin_str;
	
	INT8U	buffer[64];
	
	INT32U	addr;//, cap_addr;
	INT16S  	str_size;
	INT16U 	w;
	
	str_size = ORIGIN_STRUCT_SIZE;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
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
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1250, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
    #endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		caption_split_by_code(0xF033,&cap,0);
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1250,&cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	caption_split_by_code(55,&cap,0);
	if (cap.input_max ==0) {
		caption_split_by_code(0x00,&cap,0);
		sprintf((char*)buffer, "%s",cap.form);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		return;
	}
	
	if ((num<cap.input_min) || (num>cap.input_max)) {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_ORIGIN_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&origin_str, str_size);
	
	if(origin_str.origin_code>cap.input_max || origin_str.origin_code<cap.input_min) {	
		caption_split_by_code(0xE00C,&cap,0);
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
		return;	
	} else {
		sprintf((char*)buffer, "%s",(char*)origin_str.origin_text);
		buffer[63] = 0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)
#ifndef _USE_LCD37D_40DOT_
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		display_lcd_clear();
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		display_lcd_clear();
  #ifdef USE_KOR_FONT
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer+9); // CL3000 디스플레이 시  "원산지 : " 생략 
  #endif
#endif
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
	}       
}

#ifdef USE_NUTRITION_FACT
void plucall_nutrifact(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	NUTRIFACT_STRUCT	nutrifact_str;	
	NUTRIFACT_STRUCT2	nutrifact_str2;	
#ifdef USE_EU_NUTRITIONFACT
	NUTRIFACT_STRUCT3	nutrifact_str3;
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	NUTRIFACT_STRUCT4	nutrifact_str4; 
#endif
	
	
	INT8U	buffer[64];
	INT32U	addr;//, cap_addr;
	INT16S  	str_size;
	INT16U 	w;
	
	INT8U ucTemp, nutri_type=0;
	INT16U max_nutri_no;
	
	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
	nutri_type = ucTemp&0x0f;
#ifdef COUNTRY_USA
	nutri_type == NUTRI_TYPE_2COL_USA;
#endif		
	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
	{
		str_size = sizeof(nutrifact_str2);
		max_nutri_no = MAX_NUTRIFACT2_NO;
	}
#ifdef USE_EU_NUTRITIONFACT
	else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
	{
		str_size = sizeof(nutrifact_str3);
		max_nutri_no = MAX_NUTRIFACT3_NO;
	}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
	{
		str_size = sizeof(nutrifact_str4);
		max_nutri_no = MAX_NUTRIFACT4_NO;
	}
#endif
	else
	{
		str_size = sizeof(nutrifact_str);
		max_nutri_no = MAX_NUTRIFACT_NO;
	}
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
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
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1320, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
    #endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		caption_split_by_code(0xF033,&cap,0);
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1320,&cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	caption_split_by_code( 70,&cap,0);
	caption_adjust(&cap,NULL);
	if (max_nutri_no ==0) {
		message_error(disp_p);
		return;
	}
	
	if ((num<cap.input_min) || (num>max_nutri_no)) {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	addr  = str_size * (num-1);
#ifdef USE_NORTH_AMERICA_INGREDIENT
	addr += EXT_MEMCARD2_ADDR_X + FLASH_SCANNER_TABLE_AREA;
#else
	addr += DFLASH_BASE + FLASH_NUTRIFACT_AREA;
#endif
	
	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
	{
		Flash_sread(addr, (HUGEDATA INT8U *)&nutrifact_str2, str_size);
		if(nutrifact_str2.code>max_nutri_no || nutrifact_str2.code<cap.input_min) {	
			caption_split_by_code(0xE00C,&cap,0);
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
			return;	
		}
		else 
		{
			sprintf((char*)buffer,"%s", nutrifact_str2.description);
			buffer[63] = 0;
			DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
			hebrew_codetemp[strlen(buffer)] = 0;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//callback nutrifact
#ifndef _USE_LCD37D_40DOT_
	#ifdef USE_ENG_BASED_ARAB_SUPPORT
	 	   	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
			eng_codetemp[strlen((char *)buffer)] = 0;
			Dsp_Write_1stringLong(disp_p, eng_codetemp);
	#else

			Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
			arab_codetemp[strlen((char *)buffer)] = 0;
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
	}
#ifdef USE_EU_NUTRITIONFACT
	else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
	{
		Flash_sread(addr, (HUGEDATA INT8U *)&nutrifact_str3, str_size);
		if(nutrifact_str3.code>max_nutri_no || nutrifact_str3.code<cap.input_min) {	
			caption_split_by_code(0xE00C,&cap,0);
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
			return;	
		}

#ifdef USE_ADDITIONAL_NUTRIENT
	else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
	{
		Flash_sread(addr, (INT8U *)&nutrifact_str4, str_size); 
  #ifdef USE_DIRECT_NUTRITION
		if(nutrifact_str4.plu_num == 0) {
  #else
		if(nutrifact_str4.code>max_nutri_no || nutrifact_str4.code<cap.input_min) {
  #endif
			caption_split_by_code(0xE00C,&cap,0);
			Dsp_Write_String(disp_p,(char *)cap.form); 
			return; 
		}
		else
		{
			sprintf((char*)buffer,"%s", nutrifact_str4.description);
			buffer[63] = 0;
			Dsp_Write_String(disp_p,( char *)buffer);
		}		
	}
#endif // USE_ADDITIONAL_NUTRIENT
		else 
		{
			sprintf((char*)buffer,"%s", nutrifact_str3.description);
			buffer[63] = 0;
			DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
			hebrew_codetemp[strlen(buffer)] = 0;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//callback nutrifact
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
			Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 1); 
			eng_codetemp[strlen(cap.form)] = 0;
			Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
			Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
			arab_codetemp[strlen((char *)buffer)] = 0;
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
	}
#endif
	else
	{
		Flash_sread(addr, (HUGEDATA INT8U *)&nutrifact_str, str_size); 
		if(nutrifact_str.code>max_nutri_no || nutrifact_str.code<cap.input_min) {	
			caption_split_by_code(0xE00C,&cap,0);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
			hebrew_codetemp[strlen(cap.form)] = 0;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)
			Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
			arab_codetemp[strlen(cap.form)] = 0;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
			return;	
		}
		else
		{
			sprintf((char*)buffer,"%s", nutrifact_str.description);
			buffer[63] = 0;
			DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
			hebrew_codetemp[strlen(buffer)] = 0;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//callback nutrifact
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
			Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
			eng_codetemp[strlen((char *)buffer)] = 0;
			Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
			Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
			arab_codetemp[strlen((char *)buffer)] = 0;
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
	}
	
}
#endif

#ifdef USE_AUSTRALIA_COOL_FUNCTION
void plucall_CoOL_type(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char	buffer[64];
	INT8U flag;
	INT8U flag1;
	INT16U 	w;

	flag=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3);
	flag1=get_global_bparam(GLOBAL_SALE_SETUP_FLAG4);

	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);

	switch(num) {
		case 0: // Don't use Australia Origin
			caption_split_by_code(0xC710, &cap,0);
			strcpy((char *)cap.form, "Don't use Australian Origin");
			break;
		case 1: // Grown in Australia
			caption_split_by_code(0xC710, &cap,0);
			strcpy((char *)cap.form, "Grown in Australia");
			break;
		case 2: // Produced 
			caption_split_by_code(0xC710, &cap,0);
			strcpy((char *)cap.form, "Produced in Australia");
			break;			
		case 3: // Made in Australia
			caption_split_by_code(0xC710, &cap,0);
			strcpy((char *)cap.form, "Made in Australia");
			break;
		case 4: // Packed in Australia
			caption_split_by_code(0xC710, &cap,0);
			strcpy((char *)cap.form, "Packed in Australia");
			break;		
		case 5: // Imported
			caption_split_by_code(0xC710, &cap,0);
			strcpy((char *)cap.form, "Imported");
			break;			
		default: // invliad value
			caption_split_by_code(0xC710, &cap,0);
			strcpy(cap.form,global_message[MSG_INVALID_VALUE]);
			break;
	}
	DspStruct.Reverse = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
	DspStruct.Reverse = 0;
}
#endif


void plucall_barcodefile(long num)
{	
	CAPTION_STRUCT	cap;
	POINT			disp_p;
	BARCODE_STRUCT	barcode_str;	
	
	INT8U	buffer[BARCODE_FORMAT_LEN+4];
	INT32U	addr;//, cap_addr;
	INT16S  str_size;
	INT16U 	w;
	
	str_size = BARCODE_STRUCT_SIZE;
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
	w = Dsp_GetChars();
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w+1);
	buffer[w+1] = 0;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num==0)  // OK
	{
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1260, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
    #endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		caption_split_by_code(0xF033,&cap,0);	// Don' use
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1260,&cap,0);	//Barcode
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[BARCODE_FORMAT_LEN+3]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	caption_split_by_code(85,&cap,0);
	if (cap.input_max ==0) 
	{
		message_error(disp_p);
		return;
	}
	if ((num<cap.input_min) || (num>cap.input_max)) {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_BARCODE_FORMAT_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&barcode_str, str_size);
	
	if(barcode_str.barcode_num>cap.input_max || barcode_str.barcode_num<cap.input_min) {	
		caption_split_by_code(0xE00C,&cap,0);
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
		return;	
	} else {
		sprintf((char*)buffer, "%2d:", barcode_str.barcode_type);
		memcpy(&buffer[3], barcode_str.barcode_format, BARCODE_FORMAT_LEN);
		buffer[BARCODE_FORMAT_LEN+3] = 0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
	}       
}

void callback_barcode(long num)
{
	CAPTION_STRUCT 		cap;
	//	POINT 			disp_p;
	BARCODE_STRUCT		barcode_str;	
	//	INT8U	buffer[64];
	INT32U	addr;
	INT16S  	str_size;
	
	str_size = BARCODE_STRUCT_SIZE;
	
	//	disp_p = point((1+Startup.menu_type)*Startup.menu_ygap, 3*Startup.menu_xgap);
	//	memset((INT8U *)buffer,0x20, sizeof(buffer) );
	//	buffer[63] = 0;
	//	DspStruct.DirectDraw = 1;
	//	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
	if(num==0) { // OK
		//		caption_split_by_code(0xF033,&cap,0);
		//		strcpy((char *)buffer, (char *) cap.form);
		//		caption_split_by_code(0x1260,&cap,0);
		//		cap.form[0] = 0x20;
		//		cap.form[1] = 0x20;
		//		strcat((char*)buffer, (char *)cap.form);
		//		buffer[63]=0;
		//		DspStruct.DirectDraw = 1;
		//		DspStruct.Reverse = 1;
		//		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
		//		DspStruct.Reverse = 0;
		return;
	}
	caption_split_by_code(85,&cap,0);
	if (cap.input_max ==0) {
		//		message_error(disp_p);
		//		BuzOn(3);
		return;
	}
	if ((num<cap.input_min) || (num>cap.input_max)) {
		//		DspStruct.DirectDraw = 1;
		//		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
		BuzOn(2);
		return;
	}
	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_BARCODE_FORMAT_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&barcode_str, str_size);
	if(barcode_str.barcode_num>cap.input_max || barcode_str.barcode_num<cap.input_min) {	
		//		caption_split_by_code(0xE00C,&cap,0);
		//		DspStruct.DirectDraw = 1;
		//		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
		BuzOn(2);
		return;	
	} else {
		//		sprintf((char*)buffer, "%2d:%s", barcode_str.barcode_type, barcode_str.barcode_format);
		//		buffer[63] = 0;
		//		DspStruct.DirectDraw = 1;
		//		DspStruct.Reverse = 1;
		//		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
		//		DspStruct.Reverse = 0;
	}       
}

void plucall_label_message(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	MENU_TYPE1_STRUCT	label_message_str;	
	INT8U	buffer[64];
	INT32U	addr;
	INT16S  	str_size;
	INT16U 	w;
	
	str_size = PRT_ADVE_STRUCT_SIZE;
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
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
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1240, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
    #endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		caption_split_by_code(0xF033,&cap,0);
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1240,&cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	//caption_split_by_code(0x3241, &cap,0);
	caption_split_by_code(90, &cap,0);
	
	if (cap.input_max ==0) {
		message_error(disp_p);
		return;
	}
	
	if ((num<cap.input_min) || (num>cap.input_max)) {
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0);
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	
	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_PRT_ADVE_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&label_message_str, str_size);
	
	if(label_message_str.code>cap.input_max || label_message_str.code<cap.input_min) {
		caption_split_by_code(0xE00C,&cap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0);
		hebrew_codetemp[strlen(cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
		arab_codetemp[strlen((char *)cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
		return;
	} else {
		sprintf((char*)buffer, "%s", label_message_str.text);
		
		buffer[63] = 0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//callback label message
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
		eng_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
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
}

extern INT8U get_label_name(INT16U labelid, char *str);
extern INT8U get_label_size(INT16U labelid, INT16U *w, INT16U *h);
INT8U LabelIdSetPosition = 0;

void plucall_label_id(long num)
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char name[16];
	INT8U	buffer[64];
	INT8U   result;
	INT16U  width, height;
	INT16U 	w;
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#else
	disp_p = point(0, 0);
#endif
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
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	
	if(num==0) {
#ifdef USE_ARAB_CONVERT
    #ifdef _USE_LCD20848_
		caption_split_by_code(0x1510, &cap,0);
		strcpy((char *)buffer, (char *)&cap.form[2]);
		strcat((char *)buffer, " ");
#endif
		caption_split_by_code(0xF033, &cap,0);
		strcat((char*)buffer, (char *) cap.form);	//Don't use XXX (아랍은 순서 반대)
#else
		//caption_split_by_code(0xF03A,&cap,0);
		caption_split_by_code(0xF033, &cap,0);
		strcpy((char *)buffer, (char *) cap.form);
	#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		caption_split_by_code(0x1510, &cap,0);
		strcat((char*)buffer, (char *)&cap.form[2]);
	#endif
#endif
		buffer[63]=0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
		return;
	}
	if ((num<0) || (num>99)) {
		//		cap_addr = caption_search_by_code(0xF020);
		//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)&global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0); 
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}
	
	result = get_label_name(num, name);
	if(result == FALSE) {	
		//		cap_addr = caption_search_by_code(0xE00C);
		//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		caption_split_by_code(0xE00C,&cap,0);
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
		return;	
	} else {
		get_label_size(num, &width, &height);
		sprintf((char*)buffer, "%s(%dx%d)", name, width, height);
		buffer[63] = 0;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
		hebrew_codetemp[strlen(buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)	//callback label ID
#ifndef _USE_LCD37D_40DOT_
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
 #ifndef USE_ARAB_CONVERT
		disp_p.x += (25-1-CallbackEditPosition_x)*8;
 #endif
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
	}
}

// <FUNC> JHC, 040604, ADD, -plucall_country()
// [
// $Argument	: long num - input key number
// $Return	: void
// $Note	: use the traceability
// ]
extern INT16S	Xgap;
INT32U callback_addr;
INT16U callback_caption;
long   callback_record_size;
INT16S    callback_field_start;
INT16S    callback_field_length;


void plucall_settrace(INT32U addr,INT16U caption,long rec,INT16S fstart,INT16S flength)
{
	callback_addr		=addr;
	callback_caption	=caption;
	callback_record_size	=rec;
	callback_field_start	=fstart;
	callback_field_length	=flength;
}
/*
 void plucall_trace(long num)
 {
 CAPTION_STRUCT		cap;
 char     field_buffer[64];
 POINT 			disp_p;
 
 char	string_buf[64];
 //	long	str_size;
 INT8U	disp_line,
 lcd_size;
 INT16U	read_code;
 INT32U	addr,
 addr_flash;
 
 addr_flash = DFLASH_BASE + callback_addr;
 
 lcd_size  = Dsp_GetChars();
 //	str_size  = callback_record_size;
 disp_line = (Startup.menu_type?3:1);
 disp_p 	  = point(disp_line*Startup.menu_ygap, 0);
 
 memset(string_buf, 0x20, sizeof(string_buf));
 string_buf[63] = 0;
 disp_p = point(disp_line*Startup.menu_ygap, (3+Xgap)*Startup.menu_xgap);
 Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
 
 caption_split_by_code(callback_caption,&cap,0);
 caption_adjust(&cap,NULL);
 
 if (num >= cap.input_min && num <= cap.input_max)
 {
 addr =  addr_flash + (long)callback_record_size * (num-1);
 read_code = Flash_wread(addr);
 if(read_code == num)  {
 addr+=callback_field_start;
 Flash_sread(addr, (HUGEDATA INT8U*)field_buffer, callback_field_length);
 sprintf(string_buf, "%s", field_buffer);
 string_buf[lcd_size] = 0;
 Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
 }
 else
 {
 caption_split_by_code(0xE00C,&cap,0);
 Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
 }
 }
 else
 {
 caption_split_by_code(0xE00C,&cap,0);
 DspStruct.Reverse = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
 DspStruct.Reverse = 0;
 }
 }
 */
/*********************
 void plucall_country(long num)
 {
 CAPTION_STRUCT		cap;
 MENU_TYPE1_STRUCT	st_country;
 POINT 			disp_p;
 
 char	string_buf[64];
 long	str_size;
 INT8U	disp_line,
 lcd_size;
 INT16U	read_code;
 INT32U	addr,
 //		cap_addr,
 addr_flash;
 
 addr_flash = DFLASH_BASE + FLASH_COUNTRY_AREA;
 
 lcd_size  = Dsp_GetChars();
 str_size  = COUNTRY_STRUCT_SIZE;
 disp_line = (Startup.menu_type?3:1);
 disp_p 	  = point(disp_line*Startup.menu_ygap, 0);
 
 memset(string_buf, 0x20, sizeof(string_buf));
 string_buf[63] = 0;
 DspStruct.DirectDraw = 1;
 disp_p = point(disp_line*Startup.menu_ygap, (3+Xgap)*Startup.menu_xgap);
 Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
 
 //	cap_addr = caption_search_by_code(0x134A);
 //	_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
 caption_split_by_code(0x134A,&cap,0);
 caption_adjust(&cap,NULL);
 
 if (num >= cap.input_min && num <= cap.input_max)
 {
 addr =  addr_flash + (long)str_size * (num-1);
 read_code = Flash_wread(addr);
 if(read_code == num)  
 {	
 Flash_sread(addr, (HUGEDATA INT8U*)&st_country, str_size );
 sprintf(string_buf, 
 "%s",
 st_country.text);
 string_buf[lcd_size] = 0;
 DspStruct.DirectDraw = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
 }
 else
 {
 //			cap_addr = caption_search_by_code(0xF029);
 //			_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
 caption_split_by_code(0xE00C,&cap,0);
 DspStruct.DirectDraw = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
 }
 }
 else
 {
 caption_split_by_code(0xE00C,&cap,0);
 DspStruct.DirectDraw = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
 }
 }
 
 // <FUNC> JHC, 040604, ADD, -plucall_slaughter()
 // [
 //-$Argument	: long num - input key number
 //-$Return	: void
 //-$Note	: use the traceability
 // ]
 void plucall_slaughter(long num)
 {
 CAPTION_STRUCT		cap;
 WORKPLACE_STRUCT	st_slaughter;
 POINT 			disp_p;
 
 char	string_buf[64];
 long	str_size;
 INT8U	disp_line,
 lcd_size;
 INT16U	read_code;
 INT32U	addr,
 //		cap_addr,
 addr_flash;
 
 addr_flash = DFLASH_BASE + FLASH_SLAUGHTERHOUSE_AREA;
 
 lcd_size  = Dsp_GetChars();
 str_size  = SLAUGHTERHOUSE_STRUCT_SIZE;
 disp_line = (Startup.menu_type?3:1);
 disp_p	  = point(disp_line*Startup.menu_ygap, 0);
 
 memset(string_buf, 0x20, sizeof(string_buf));
 string_buf[63] = 0;
 DspStruct.DirectDraw = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
 disp_p = point(disp_line*Startup.menu_ygap, 3*Startup.menu_xgap);
 
 //	cap_addr = caption_search_by_code(0x135A);
 //	_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
 caption_split_by_code(0x135A,&cap,0);
 caption_adjust(&cap,NULL);
 
 if (num >= cap.input_min && num <= cap.input_max)
 {
 addr =  addr_flash + (long)str_size * (num-1);
 read_code = Flash_wread(addr);
 if(read_code == num)  
 {	
 Flash_sread(addr, (HUGEDATA INT8U*)&st_slaughter, str_size );
 sprintf(string_buf, "%s",st_slaughter.text);
 string_buf[lcd_size] = 0;
 DspStruct.DirectDraw = 1;
 DspStruct.Reverse = 0;
 Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
 DspStruct.Reverse = 0;
 }
 else
 {
 //			cap_addr = caption_search_by_code(0xF029);
 //			_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
 caption_split_by_code(0xE00C,&cap,0);
 DspStruct.DirectDraw = 1;
 DspStruct.Reverse = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
 DspStruct.Reverse = 0;
 }
 }
 else
 {
 //		cap_addr = caption_search_by_code(0xF029);
 //		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
 caption_split_by_code(0xE00C,&cap,0);
 DspStruct.DirectDraw = 1;
 DspStruct.Reverse = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
 DspStruct.Reverse = 0;
 }
 }
 
 // <FUNC> JHC, 040604, ADD, -plucall_cutting()
 // [
 // $Argument	: long num - input key number
 // $Return	: void
 // $Note	: use the traceability
 // ]
 void plucall_cutting(long num)
 {
 CAPTION_STRUCT		cap;
 WORKPLACE_STRUCT	st_cutting;
 POINT 			disp_p;
 
 char	string_buf[64];
 long	str_size;
 INT8U	disp_line,
 lcd_size;
 INT16U	read_code;
 INT32U	addr,
 //		cap_addr,
 addr_flash;
 
 addr_flash = DFLASH_BASE + FLASH_CUTTINGHALL_AREA;
 
 lcd_size  = Dsp_GetChars();
 str_size  = CUTTINGHALL_STRUCT_SIZE;
 disp_line = (Startup.menu_type?3:1);
 disp_p 	  = point(disp_line*Startup.menu_ygap, 0);
 
 memset(string_buf, 0x20, sizeof(string_buf));
 string_buf[63] = 0;
 DspStruct.DirectDraw = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
 disp_p = point(disp_line*Startup.menu_ygap, 3*Startup.menu_xgap);
 
 //	cap_addr = caption_search_by_code(0x136A);
 //	_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
 caption_split_by_code(0x136A,&cap,0);
 caption_adjust(&cap,NULL);
 
 if (num >= cap.input_min && num <= cap.input_max)
 {
 addr =  addr_flash + (long)str_size * (num-1);
 read_code = Flash_wread(addr);
 if(read_code == num)  
 {	
 Flash_sread(addr, (HUGEDATA INT8U*)&st_cutting, str_size );
 sprintf(string_buf, 
 "%s",
 st_cutting.text);
 string_buf[lcd_size] = 0;
 DspStruct.DirectDraw = 1;
 DspStruct.Reverse = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)string_buf);
 DspStruct.Reverse = 0;
 }
 else
 {
 //			cap_addr = caption_search_by_code(0xF029);
 //			_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
 caption_split_by_code(0xE00C,&cap,0);
 DspStruct.DirectDraw = 1;
 DspStruct.Reverse = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
 DspStruct.Reverse = 0;
 }
 }
 else
 {
 //		cap_addr = caption_search_by_code(0xF029);
 //		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
 caption_split_by_code(0xE00C,&cap,0);
 DspStruct.DirectDraw = 1;
 DspStruct.Reverse = 1;
 Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
 DspStruct.Reverse = 0;
 }
 }
 ********/

void error_display_form1(INT8U number, INT32U min, INT32U max)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	
	//	INT16U	x_gab = 6;
	//	INT16U	y_gab = 8;
	char old_font;
	char buffer[34];
	char msg[64];
	INT8U backup_directdraw;
	INT8U old_page;
	
	number = 0;
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;
	
	BuzOn(2);
	old_page = DspStruct.Page;
	old_font=DspStruct.Id1;
	Dsp_SetPage(DSP_ERROR_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);
	
	Dsp_Fill_Buffer(0);
	caption_split_by_code(0xF031,&cap,0);
	memcpy(msg,cap.form,sizeof(cap.form)); msg[34]=0;
	strcat((char *)msg," %ld-%ld");
	sprintf ((char *) buffer, (char *)msg, min, max);
	
	disp_p = point(  (Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)buffer);
#endif
	Dsp_Diffuse();
	//Delay_100ms(10);
	Key_DelaySec(10, 0);
	
	Dsp_SetPage(old_page);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	Dsp_Fill_Buffer(0); // 향후 PAGE 로 대체
#endif
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	
	DspStruct.DirectDraw = backup_directdraw;
}


//DEL071206void error_display_invalidkey(void)
//DEL071206{
//DEL071206	POINT 			disp_p;
//DEL071206
//DEL071206	Dsp_SetPage(DSP_ERROR_PAGE);
//DEL071206	DspLoadFont1(DSP_PLU_FONT_ID);
//DEL071206
//DEL071206	Dsp_Fill_Buffer(0);
//DEL071206
//DEL071206	disp_p = point( Startup.menu_type*Startup.menu_ygap, 0);
//DEL071206	Dsp_Write_String(disp_p,(HUGEDATA char*)global_message[MSG_INVALID_VALUE]);
//DEL071206	Dsp_Diffuse();
//DEL071206	//Delay_100ms(10);
//DEL071206	Key_DelaySec(10, 0);
//DEL071206
//DEL071206	Dsp_SetPage(DSP_DEFAULT_PAGE);
//DEL071206	DspLoadFont1(DSP_MENU_FONT_ID);
//DEL071206	Dsp_Diffuse();
//DEL071206}

//DEL071206void error_display_message(char *str)
//DEL071206{
//DEL071206	POINT 			disp_p;
//DEL071206
//DEL071206	BuzOn(2);
//DEL071206	Dsp_SetPage(DSP_ERROR_PAGE);
//DEL071206	DspLoadFont1(DSP_PLU_FONT_ID);
//DEL071206
//DEL071206	Dsp_Fill_Buffer(0);
//DEL071206
//DEL071206	disp_p = point( Startup.menu_type*Startup.menu_ygap, 0);
//DEL071206	Dsp_Write_String(disp_p,(HUGEDATA char*)str);
//DEL071206	Dsp_Diffuse();
//DEL071206	//Delay_100ms(10);
//DEL071206	Key_DelaySec(10, 0);
//DEL071206
//DEL071206	Dsp_SetPage(DSP_DEFAULT_PAGE);
//DEL071206	DspLoadFont1(DSP_MENU_FONT_ID);
//DEL071206	Dsp_Diffuse();
//DEL071206
//DEL071206}


void error_display_caption(INT16U code)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char old_font;
	INT8U old_page;
	
	old_page = DspStruct.Page;
	old_font = DspStruct.Id1;
	Dsp_SetPage(DSP_ERROR_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);
	
	Dsp_Fill_Buffer(0);
	caption_split_by_code(code,&cap,0);
	disp_p = point(  Startup.menu_type*Startup.menu_ygap, 0);
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
	//Delay_100ms(10);
	Key_DelaySec(10, 0);
	
	Dsp_SetPage(old_page);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	Dsp_Fill_Buffer(0); // 향후 PAGE 로 대체
#endif
	DspLoadFont1(old_font);
	Dsp_Diffuse();
}

void keycall_key_number(long num)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	long 		plu_num;
	//INT32U		addr_key;
	char 		temp[36];
	char		form[36];

	//addr_key = DFLASH_BASE + FLASH_KEY_SPEED_AREA + (speedkey_set_number-1)*MAX_SPEEDKEY_SIZE;
	//addr_key += (num-1) * 4; 
	//plu_num = Flash_lread(addr_key);
	plu_num = plu_get_speedkey((INT16U)speedkey_set_number, (INT16U)num);
	
	if (plu_num < 1l || plu_num > MAX_PLU_NUMBER) plu_num = 0;
	plu_get_display_info(2,&cap,1);
	memset(temp, 0x20, 36);
#ifdef HEBREW_FONT
	sprintf(form, "%%s]%%%dli[", cap.input_length);
#else
	sprintf(form, "%%s[%%%dli]", cap.input_length);
#endif
	sprintf(temp, form, cap.form, plu_num);
	speedkey_plu_number = plu_num;
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap,0); 
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)temp, strlen(temp), 0, 0); 
	hebrew_codetemp[strlen(temp)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)temp, strlen(temp), 0, 0, 0); 
	arab_codetemp[strlen(temp)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)temp);
#endif
	plucall_plu_number(plu_num);	
}

extern INT16S IsBlankString(char *p,INT16S max);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
extern BOOLEAN		keyin_get_speed_key;
extern INT32U PrevSpeedKeyPluNo;
extern INT16U PrevKeySetNo;
extern INT16U PrevKeyNo;
#endif
void plucall_plu_number(long num)
{	
	CAPTION_STRUCT		cap;
	POINT 			disp_p;
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
	char buffer[MAX_MENU_CHAR_BUF*2];
#else
	char buffer[MAX_MENU_CHAR_BUF];
#endif
#else
	char buffer[MAX_MENU_CHAR_BUF];
#endif
	INT16U	 key, usize;
	INT16S      w;
	long     plu_num;
	
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if (KeyDrv.Type==KEY_TYPE_SPEEDPLU) {
		if (keyin_get_speed_key) {
			if (PrevSpeedKeyPluNo)//(KeyDrv.PrevType == KEY_TYPE_NUMERIC)
			{
				plu_set_speedkey(PrevKeySetNo, PrevKeyNo, PrevSpeedKeyPluNo);
				PrevSpeedKeyPluNo = 0;
			}
			caption_split_by_code(0x3162,&cap,0);
			disp_p = point(keycall_key_position_y*Startup.menu_ygap,0);
			sprintf(buffer, "%s[%3li]", cap.form, num);
			skeyin_define=num;
	////DEL071016			DspStruct.DirectDraw = 1;
			plu_num = plu_get_speedkey((INT16U)speedkey_set_number, (INT16U)num);
			if (plu_num < 1l || plu_num > MAX_PLU_NUMBER) plu_num = 0;
			speedkey_plu_number = plu_num;
			////
			PrevKeySetNo = (INT16U)speedkey_set_number;
			PrevKeyNo    = (INT16U)num;
			//PrevPluNo    = plu_num;
			////
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
			hebrew_codetemp[strlen(buffer)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
			arab_codetemp[strlen((char *)buffer)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char*)buffer);
#endif
	//			Dsp_Diffuse();
			num = plu_num;	//num가 keynum 에서 pluno로 바뀌는 시점
			goto NEXT;
		}
		return;
	}
	
NEXT:
#ifdef USE_ARAB_CONVERT
    disp_p = point((CallbackEditPosition_y+1)*Startup.menu_ygap, 0);
#else
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#endif	
#else
	disp_p.x = 0;
	disp_p.y = 0;
#endif
	w = Dsp_GetChars();
	if (w < CallbackEditPosition_x) w = 0;	//Modified by JJANG 20081126
#ifndef USE_ARAB_CONVERT // 이란의 경우 빈 줄에 데이터 출력해서 CallbackEditPosition_x 값 제하지 않음.
	else w -= CallbackEditPosition_x;
#endif
#ifdef _USE_LCD20848_//ck
	w += 1; //display size 208인데 LCD_Y_MARGIN을 202로 디파인하여 사용하면서  마지막 부분 1자가  지워지지 않는 현상 수정 
#endif
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
	display_lcd_clear();
#endif
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
		hebrew_codetemp[strlen(buffer)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)buffer);
#endif
	if (Startup.disable_dept) {
		callback_dept_no=1;
	}
	if (callback_dept_no==0) goto M1;
	plu_table_search((INT8U)callback_dept_no, num,&key,0);
	if (key==0) {
#if defined(CL5200_PROJECT) && (defined(USE_ARAB_FONT) || defined(HEBREW_FONT))  //20120914 for CL5200 Arab version by kiho
M1: 	caption_split_by_code(0xC907,&cap,0);	//No data
#else
M1:		caption_split_by_code(0xE00C,&cap,0);
#endif
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		display_lcd_clear();
#endif
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((HUGEDATA char *)cap.form, strlen(cap.form), 0, 0);
		hebrew_codetemp[strlen(cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
		DspStruct.Reverse = 0;
		return;
	} else {
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
		plu_get_field(key-1,PTYPE_PLU_NAME1,(INT8U*)buffer,(INT16S*)&usize,16);
#else
		plu_get_field(key-1,PTYPE_PLU_NAME1,(INT8U*)buffer,(INT16S*)&usize,sizeof(buffer));
#endif
#else
		plu_get_field(key-1,PTYPE_PLU_NAME1,(INT8U*)buffer,(INT16S*)&usize,sizeof(buffer));
#endif
		if (usize) {
			INT16S blankFlag;	//new
			if (IsHanCharMid(buffer, w) == CHAR_HAN_MID) //Modified by JJANG 20081202 한글일 때 글씨 깨지는 현상 방지.		
			{
				w--;
			}
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
#ifndef _USE_LCD37D_40DOT_
			buffer[16]=0;
#else
			buffer[w]=0;
#endif
#else
			buffer[w]=0;
#endif
			DspStruct.Reverse = 1;
			blankFlag = IsBlankString(buffer,40);
			if(blankFlag)
			{
				memcpy(buffer, global_message[MSG_NO_PLUNAME], w);
				buffer[w]=0;
			}
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0);
			hebrew_codetemp[strlen(buffer)] = 0;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)
#ifndef _USE_LCD37D_40DOT_
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
			Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 1); 
 #else
			Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
 #endif
 #ifdef USE_ARAB_CONVERT
			arab_codetemp[strlen((char *)buffer)] = 0;
 #else
  #ifndef USE_ENG_BASED_ARAB_SUPPORT
			//display_lcd_clear();
			//disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			arab_codetemp[8] = 0;
  			if(CallbackEditPosition_x < 25)
				disp_p.x += (25-1-CallbackEditPosition_x)*8;
			else
				disp_p.x += CallbackEditPosition_x-1;
  #endif
 #endif
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
			Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
 #endif
#else
			display_lcd_clear();
			Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
#else
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
			display_lcd_clear();
#endif
			Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
			DspStruct.Reverse = 0;
		} else goto M1;
 	}
}

void plucall_plu_dcnumber(long num)
{	
	CAPTION_STRUCT		cap;
	POINT 			disp_p;
	char buffer[MAX_MENU_CHAR_BUF];
	INT16U	 key, usize,dc_no;
	INT16S      w;

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
  #ifdef USE_ARAB_CONVERT
    disp_p = point((CallbackEditPosition_y + 1)*Startup.menu_ygap, 0);
  #else
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
  #endif
#else
	disp_p = point(0, 0);
#endif
	w = Dsp_GetChars();
#ifndef USE_ARAB_CONVERT
	w -= CallbackEditPosition_x;
#endif
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
	////DEL071016	DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
	hebrew_codetemp[25-CallbackEditPosition_x] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
	arab_codetemp[strlen((char *)buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
	plu_table_search((INT8U)callback_dept_no, num,&key,0);
	if (key==0) { 
		caption_split_by_code(0xE00C, &cap,0);
		////DEL071016       		DspStruct.DirectDraw = 1;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
		DspStruct.Reverse = 0;
		return;
	} else {
		dc_no=_get_dc_number(callback_dept_no, num);
		if (dc_no) {
			plu_get_field(key-1,PTYPE_PLU_NAME1,(INT8U*)buffer,(INT16S*)&usize,sizeof(buffer));
		} else {
			//caption_split_by_code(0x1b25, &cap,0);
			caption_split_by_code(0xE00C, &cap,0);	// Modified by CJK 20051026
			//strcpy(buffer,cap.form);
			memcpy(buffer, cap.form, w);
		}
		buffer[w] = 0;
		////DEL071016		DspStruct.DirectDraw = 1;
		DspStruct.Reverse = 1;
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)buffer, strlen(buffer), 0, 0); 
		hebrew_codetemp[25-CallbackEditPosition_x] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)buffer, strlen((char *)buffer), 0, 0, 0); 
		arab_codetemp[strlen((char *)buffer)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif
		DspStruct.Reverse = 0;
 	}
}

INT16U search_pgm_next_id(INT16U code, INT16U str_size, INT32U flash_addr)
{
	CAPTION_STRUCT	cap;
	
	INT16U	i=0;
	INT16U	read_id;
	INT32U	addr;
	
	// 2004.10.14 bmlim
	//	caption_split_by_code(code, &cap);
	if (code<0x0fff) {
		plu_get_display_info(code,&cap,0);
	} else {
		caption_split_by_code(code, &cap,0);
	}
	while(1)  
	{	
		addr = flash_addr + (str_size * i);
		read_id = Flash_wread(addr);
		
		if( read_id ==(i+1) ) {
			i++;
		} else {
			break;
		}
	}//while(1)
	
	// <CODE> JHC, 040628, Modify, -modify max_id
	// [
	if(i >= cap.input_max) 
		return 0;
	else 
		return i+1;
	// ]
}

BOOLEAN	input_range_check(long input_value, long min, long max)
{
	if(input_value<min || input_value>max) {
		return 0;
	} else 	{
		return 1;
	}
}

extern HUGEDATA PACKDATE_STRUCT mPackDate; //SG060309
BOOLEAN input_time_check_str(char *buffer, INT8U mode)
{
	INT8U i;
	INT8U temp_hour, temp_min, temp_sec;
	INT8U temp_buf[10];
	
	for(i=0; i<8; i++) {
		temp_buf[i] = buffer[i] - '0';
	}
	
	temp_hour = temp_buf[0]*10 + temp_buf[1];
	if(temp_hour<0 || temp_hour>23) {
		return 0;
	} 		
	
	temp_min = temp_buf[3]*10 + temp_buf[4];
	if(temp_min<0 || temp_min>59) {
		return 0;
	}
	
	temp_sec = temp_buf[6]*10 + temp_buf[7];
	if(temp_sec<0 || temp_sec>59) {
		return 0;
	}
	
	//	i=temp_hour+temp_min+temp_sec;
	//	if(i==0)
	//		return 1;
	if(mode == 1){
		RTCStruct.hour=hex2bcd(temp_hour);
		RTCStruct.min=hex2bcd(temp_min);
		RTCStruct.sec=hex2bcd(temp_sec);
	}
	else if (mode == 0){
		mPackDate.hour = temp_hour;
		mPackDate.min  = temp_min;
		mPackDate.sec  = temp_sec;
	} // mode == 2
	
	return 1;
}

INT8U	rtc_get_date_format(void)
{
	INT8U date_format;
	INT8U v8;
	
	date_format=get_global_bparam(GLOBAL_DATETIME_MODE );
	v8  = date_format>>4;
	v8 &= 0x03;
	date_format=v8;
	return date_format;
}


BOOLEAN input_day_check(INT8U year, INT8U month, INT8U day)
{
	INT8U max_day;
	
	max_day = year + month + day;
	if(!max_day)	return 1;
	
	max_day=date_getmonthdays(year, month);
	
	if(day<1 || day>max_day) {
		return 0;
	} else {
		return 1;
	}
}

BOOLEAN input_day_check_str(INT8U day_type, char *buffer, INT8U mode)
{
	INT8U max_day;
	INT8U str[10];
	INT8U i;
	INT8U y, m, d;
#ifdef USE_PERSIAN_CALENDAR
	INT16U perCentury;
#endif
	
	for(i=0; i<8; i++) {
		str[i] = buffer[i] - '0';
	}
	
	switch(day_type) {
		case 0: 
			y = str[0]*10 + str[1];
			m = str[3]*10 + str[4];
			d = str[6]*10 + str[7];
			break;			
		case 1:	
			m = str[0]*10 + str[1];
			d = str[3]*10 + str[4];
			y = str[6]*10 + str[7];
			break;
		case 2:
		case 3:	
			d = str[0]*10 + str[1];
			m = str[3]*10 + str[4];
			y = str[6]*10 + str[7];
			break;
	}
	
	if (mode == 2) // 체크만 할 때
	{
		max_day = y + m + d;
		if(!max_day)	return 1;
	}
	
	if (m > 12) return 0;
	
	max_day=date_getmonthdays(y, m);
	
	if(d<1 || d>max_day) {
		return 0;
	}
	
	if(mode == 1){
		RTCStruct.year=hex2bcd(y);
		RTCStruct.month=hex2bcd(m);
		RTCStruct.date=hex2bcd(d);
#ifdef USE_PERSIAN_CALENDAR
		if(RTCStruct.year < 0x5a) 	perCentury = 1400; //90년 이전: 1400년대, 90년 이후: 1300년대
		else perCentury = 1300;
		
		RTCStruct.day = date_getweek((short)bcd2hex(RTCStruct.year)+perCentury,
				(short)bcd2hex(RTCStruct.month),(short)bcd2hex(RTCStruct.date));
#else
		RTCStruct.day = date_getweek((short)bcd2hex(RTCStruct.year)+2000,
				(short)bcd2hex(RTCStruct.month),(short)bcd2hex(RTCStruct.date));
#endif
	}
	else if (mode == 0){
		mPackDate.year  = y;
		mPackDate.month = m;
		mPackDate.date  = d;
	} // mode == 2
	
	return 1;
}

void input_convert_date(INT8U day_type, INT8U *buffer, INT8U *y, INT8U *m, INT8U *d)
{
	INT8U i, size;
	INT8U str[10];
	
	size = strlen((char*)buffer);
	
	
	for(i=0; i<size; i++) {
		str[i] = buffer[i] - '0';
	}
	
	switch(day_type) {
		case 0: *y = str[0]*10 + str[1];
		*m = str[3]*10 + str[4];
		*d = str[6]*10 + str[7];
		return;			
		
		case 1:	*m = str[0]*10 + str[1];
		*d = str[3]*10 + str[4];
		*y = str[6]*10 + str[7];
		return;
		case 2:
		case 3:	*d = str[0]*10 + str[1];
		*m = str[3]*10 + str[4];
		*y = str[6]*10 + str[7];
		return;
	}
	
}

void input_convert_time(INT8U *str, INT8U *hour, INT8U *min, INT8U *sec)
{
	INT8U i, size, buffer[10];
	
	size = strlen((char*)str);
	
	for(i=0; i<size; i++) {
		buffer[i] = str[i] - '0';
	}
	
	*hour = buffer[0]*10 + buffer[1];
	*min = buffer[3]*10 + buffer[4];
	*sec = buffer[6]*10 + buffer[7];
}

