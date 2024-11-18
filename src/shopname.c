/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	shopname.c
|*  Version			:	1.0
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created			:	2003/12/08
|*  Description		:	shop_name   
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
#include "pgm_app.h"
#include "menu_code_text1.h"
#include "shopname.h"
#include "prt_list.h"
#include "mode_error.h"
#include "barcode_file.h"
#include "traceability.h"
#include "plu_device.h"

extern INT8U plu_input_department(INT16U *deptno, INT32U _PACKED_DEF *pluno, INT8U dc);

//SG070118
SCANNER_DATA_STR  Scanner;


void store_name_dataUpdate(INT32U num)	   //se-hyung 20071011
{
	STORE_NAME_STRUCT  store_str;
	INT16U  strSize;
	INT32U	addr;
	INT16U  storeNameIndex;

	strSize = sizeof(store_str);		      
	addr = DFLASH_BASE;
	addr += FLASH_SHOPNAME_AREA;
	addr += ((num - 1) * strSize);
	storeNameIndex = Flash_wread(addr);
	if(storeNameIndex == num){
		Flash_sread(addr, (HUGEDATA INT8U *)&store_str, strSize );
	} else {
		memset(&store_str.code, 0, strSize );
		store_str.code = num;		
	}

	*Menu[0].pval = store_str.code; 
 	memcpy(Menu[1].pstr, store_str.name, Menu[1].length); 
       	memcpy(Menu[2].pstr, store_str.tel_no, Menu[2].length); 
      	memcpy(Menu[3].pstr, store_str.text, Menu[3].length);

      	MenuRefresh = ON;
}
	   
void	store_name_create(void)		 //se-hyung 20071011
{
	STORE_NAME_STRUCT  store_str;
	CAPTION_STRUCT 	  cap;	
 	INT16U	str_size; 
	INT16U	result;
	INT8U	input_max;
	INT8U	i;
	INT8U	name_str[26];
	INT8U	telno_str[20];
	INT8U	text_str[150];
		 
	INT32U	addr_start; 
	INT32U	addr_flash;
	INT32U set_val[4];


	set_val[0] = 1;

	str_size = sizeof(store_str);
	addr_start =  DFLASH_BASE + FLASH_SHOPNAME_AREA;
	// Get Last Number
	caption_split_by_code(0x141A,&cap,0);
	for (i=0; i<cap.input_max; i++) {
		addr_flash = addr_start + (str_size * i);
		set_val[0] = Flash_wread(addr_flash);
		if ((set_val[0]<cap.input_min) || (set_val[0]>cap.input_max)) {
			set_val[0]=i+1;
			break;
		}
	}
	if ((set_val[0]<cap.input_min) || (set_val[0]>cap.input_max)) set_val[0]=cap.input_min;

	while(1)
	{    
		str_size = sizeof(store_str);
		addr_start =  DFLASH_BASE + FLASH_SHOPNAME_AREA;
		addr_flash = addr_start + (str_size * (set_val[0]-1));
		Flash_sread(addr_flash, (HUGEDATA INT8U *)&store_str, str_size); 

		if(store_str.code != set_val[0])  {
			memset((INT8U*)&store_str, 0x00, str_size);
		}
	       	store_str.code = set_val[0];
		set_val[0] = store_str.code;
		strcpy((HUGEDATA char*)name_str, (HUGEDATA char*)store_str.name);   
		strcpy((HUGEDATA char*)telno_str, (HUGEDATA char*)store_str.tel_no);
		strcpy((HUGEDATA char*)text_str, (HUGEDATA char*)store_str.text);

		Menu_Init();
		caption_split_by_code(0x1411, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x141A,&cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);

		input_max = cap.input_max;
 		Menu_AddEndFunction(1, store_name_dataUpdate);
		
		caption_split_by_code(0x141B, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, name_str, NULL, NULL);

		caption_split_by_code(0x141C, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, telno_str, NULL, NULL);

		caption_split_by_code(0x141D, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, text_str, NULL, NULL);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			addr_flash = addr_start + (str_size * (set_val[0]-1));
			memset((INT8U*)&store_str, 0x00, str_size);
			
			store_str.code = set_val[0];
			strcpy((HUGEDATA char*)store_str.name, (HUGEDATA char*)name_str);
			strcpy((HUGEDATA char*)store_str.tel_no,(HUGEDATA char*)telno_str);   
			strcpy((HUGEDATA char*)store_str.text, (HUGEDATA char*)text_str);    
			Flash_swrite(addr_flash, (HUGEDATA INT8U *)&store_str, str_size);
		
			if(set_val[0] <input_max) set_val[0]++;
			else   set_val[0] =input_max; 
		}
		else if(result ==MENU_RET_ESC)
		{
			break;
		}
	}
}

INT16S store_callback_position;
INT16S store_callback_pos_y;

void store_call_back(long num)
{
	CAPTION_STRUCT 	cap;
	STORE_NAME_STRUCT  store_str;	
 	POINT disp_p;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT16U  w;
	INT16U str_size;
	INT32U addr;

	str_size = sizeof(store_str);

#ifdef USE_ARAB_CONVERT
    disp_p = point((CallbackEditPosition_y + 1)*Startup.menu_ygap, 0);
#else
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
#endif
	w = Dsp_GetChars();
#ifndef USE_ARAB_CONVERT
	w -= CallbackEditPosition_x; // caption length > w 인경우에 마지막 글자가 지워지지 않고 잔상으로 남음.
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

	caption_split_by_code(0x141A,&cap,0);
	if ((num<cap.input_min) || (num>cap.input_max)) {		
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0); 
		hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
        DspStruct.Reverse = 1;
		Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
		arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
        DspStruct.Reverse = 0;
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
#endif
		return;
	}

	addr  = str_size * (num-1);
	addr += DFLASH_BASE + FLASH_SHOPNAME_AREA;
	Flash_sread(addr, (HUGEDATA INT8U *)&store_str, str_size);
	if(store_str.code>cap.input_max || store_str.code<cap.input_min) {	
		caption_split_by_code(0xE00C,&cap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
        DspStruct.Reverse = 1;
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
        DspStruct.Reverse = 0;
#else
		Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
	} else {
		memcpy((INT8U*)buffer, store_str.name,26);
		buffer[w]=0;    
		DspStruct.Reverse = 1;
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
		DspStruct.Reverse = 0;
	}
}


void    store_name_delete(void)			// command code  0x1413
{
	menu1_delete(0x1413, 0x141A, FLASH_SHOPNAME_AREA, SHOPNAME_STRUCT_SIZE);
}

#ifndef DISABLE_LIST_MENU
void  store_name_list(void)	// command code 0x1213
{		
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	STORE_NAME_STRUCT 	store_str;
	INT16U dlength;

//	INT8U	y_gab=8;	  	// 폰트의 y 크기
//	INT8U	x_gab=6;	  	// 폰트의 x 크기
	char   	string_buf[34], end_flag = 0;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, str_size, reverse_num;
	INT16S	j = 0, result, min_num, max_num;
	INT16S	line_ct, lcd_size, start_id;
	INT16U	read_code;
	INT32U	addr, addr_flash;//, cap_addr;
	INT8U	linenum;			    //se-hyung 20071002	     list볼 범위 입력창 아래 쓰레기값 나오는거 수정

	str_size = sizeof(store_str);

LIST_START:
//	cap_addr = caption_search_by_code(0x141A);			// Decide inpput boundary
//	_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));		//
	caption_split_by_code(0x141A, &cap,0);
	min_num	= cap.input_min;					//
	max_num = cap.input_max; 					// 
	dlength = cap.input_dlength;
	if(min_num <= 0)  {
//		sprintf(string_buf, "caption error");
		disp_p = point(0,0);
//		Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
		message_error(disp_p);
	}
	Dsp_Fill_Buffer(0);
	//display_title_page(0x1412);
	display_title_page_change(0x1412,1,1);
//	addr = caption_search_by_code(0x1E00);
//	for(i = 0; i<Startup.line_max; i++)  {		      //se-hyung 20071002  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
	linenum = Startup.line_max;			      //se-hyung 20071002  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
	if (linenum > 3) linenum = 3;			      //se-hyung 20071002  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
	for(i = 0;i<linenum; i++)  {			      //se-hyung 20071002  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
//		cap_addr = addr + CAP_MSG_LENGTH * i;
//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
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
	end_num   = max_num;
	for(i=0; i<2; i++)  {
REN1:
		if(i == 0)	ret_long = start_num;		
		if(i == 1)	ret_long = end_num;
		caption_split_by_code(0x1E00+i, &cap,0);
//		cap_addr = addr + CAP_MSG_LENGTH * i;
//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));
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
		 
		 result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long,cap.input_dlength, 
  				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
			 Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
			 arab_codetemp[strlen(cap.form)] = 0;
			 disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			 Dsp_Write_1stringLong(disp_p, arab_codetemp);

			 result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long,cap.input_dlength, 
  				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long,cap.input_dlength, 
  				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
			case KP_ARROW_UP:  // <CODE> JHC, 040512, Modify
					   // [
					   // $1: JHC, 040531, Modify
					   if(i > 0)	i--;
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
	addr_flash = DFLASH_BASE + FLASH_SHOPNAME_AREA;
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
//		caption_split_by_code(0x1419, &cap,0);
		caption_split_by_code(0x71FF, &cap,0);
//		disp_p = point(0,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071002     no. reference(descript.) 한줄 띄우기
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
			Flash_sread(addr, (HUGEDATA INT8U*)&store_str, str_size ); 

			store_str.name[10]=0;
			store_str.tel_no[18]=0;
		 	sprintf(string_buf, 
		 		"%2d %10s %s", 
		 		store_str.code, 
		 		store_str.name, 
		 		store_str.tel_no);
			string_buf[lcd_size] = 0;	       		
			
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);	 
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
			hebrew_codetemp[strlen(string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)//list, 기존 아랍ROM 적용되어 있음
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
			if (j == line_ct) break;
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
		store_name_print_list(start_num, end_num);
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

void callback_end_jobbatch_no(INT32U num)
{
	JOB_BATCH_STRUCT  job_batch_str;
	INT32U  flash_addr;
	INT16U  read_code;
	INT8U dept_id, plu_id, quantity_id;

	if(!Startup.disable_dept) dept_id = Menu_GetMenuId(2);
	plu_id = Menu_GetMenuId(3);
	quantity_id = Menu_GetMenuId(4);

	flash_addr =  DFLASH_BASE + FLASH_JOBBATCH_AREA;
	flash_addr += JOB_BATCH_STRUCT_SIZE * (num-1) ; 
	read_code = Flash_wread(flash_addr);
	if(read_code == (INT16U)num)
	{		
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&job_batch_str, JOB_BATCH_STRUCT_SIZE);
	}
	else 
	{
		memset(&job_batch_str, 0, JOB_BATCH_STRUCT_SIZE);
	}
	job_batch_str.code = (INT16U)num;

	if(!job_batch_str.deptNO) job_batch_str.deptNO = status_scale.departmentid;
	if(Startup.disable_dept) job_batch_str.deptNO = 1;
	if(!job_batch_str.pluNO) job_batch_str.pluNO = 1;

	if(!Startup.disable_dept) *Menu[dept_id].pval = job_batch_str.deptNO;
	*Menu[plu_id].pval = job_batch_str.pluNO;
	*Menu[quantity_id].pval = job_batch_str.quantity;
	
	MenuRefresh = ON;	
}

extern INT16U Menu_CallbackDeptNo;
extern void callback_end_chageunit_dept(INT32U num);
extern void callback_end_chageunit_plu(INT32U num);
extern INT8U plu_get_plu_type(INT16U dept_no, INT32U plu_no);
void job_batch_create(void)
{
	INT32U  currentCode;
	JOB_BATCH_STRUCT  job_batch_str;
	INT32U  addr_origin,flash_addr;
	INT16U  result, read_code;
	CAPTION_STRUCT cap;
	INT32U  dept_no;
	INT32U  plu_no;
	INT32U  quantity;
	INT8U 	plu_ptype;


	addr_origin =  DFLASH_BASE + FLASH_JOBBATCH_AREA;

	while (1)
	{
		currentCode = (INT8U)search_pgm_next_id(0x145A, JOB_BATCH_STRUCT_SIZE, addr_origin);
		if(!currentCode) {
			error_display_caption(0xF035);
			currentCode = 1;
		}

		flash_addr = addr_origin + JOB_BATCH_STRUCT_SIZE * (currentCode-1) ; 
		read_code = Flash_wread(flash_addr);
		if(read_code == (INT16U)currentCode)
		{		
			Flash_sread(flash_addr, (HUGEDATA INT8U*)&job_batch_str, JOB_BATCH_STRUCT_SIZE);
		}
		else 
		{
			memset(&job_batch_str, 0, JOB_BATCH_STRUCT_SIZE);
		}
		job_batch_str.code = (INT16U)currentCode;

		if(!job_batch_str.deptNO) job_batch_str.deptNO = status_scale.departmentid;
		if(Startup.disable_dept) job_batch_str.deptNO = 1;
		if(!job_batch_str.pluNO) job_batch_str.pluNO = 1;
		
		dept_no = job_batch_str.deptNO;
		plu_no = job_batch_str.pluNO;
		quantity = job_batch_str.quantity;



		Menu_Init();

		caption_split_by_code(0x1451, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x145A, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &currentCode, NULL, NULL, NULL);
		Menu_AddEndFunction(1, callback_end_jobbatch_no);

#ifdef USE_ARAB_CONVERT
        caption_split_by_code(0x145B, &cap,0);
        caption_adjust(&cap,NULL);
        Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
                        cap.input_max, cap.input_min, 0, &quantity, NULL, NULL, NULL);

        if (Startup.disable_dept) 
		{
			Menu_CallbackDeptNo = 1;
		}
		else
		{
			//caption_split_by_code(1, &cap,0);
			//caption_adjust(&cap,NULL);
			plu_get_display_info(PTYPE_DEPARTMENT_NO, &cap, 0);
			Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &dept_no, NULL, NULL, plucall_department);
			Menu_AddEndFunction(3, callback_end_chageunit_dept);
		}

		//caption_split_by_code(2, &cap,0);
		//caption_adjust(&cap,NULL);
		plu_get_display_info(PTYPE_PLU_NO, &cap, 0);
		Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &plu_no, NULL, NULL, plucall_plu_number);
		Menu_AddEndFunction(4, callback_end_chageunit_plu);

		
#else
		if (Startup.disable_dept) 
		{
			Menu_CallbackDeptNo = 1;
		}
		else
		{
			//caption_split_by_code(1, &cap,0);
			//caption_adjust(&cap,NULL);
			plu_get_display_info(PTYPE_DEPARTMENT_NO, &cap, 0);
			Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &dept_no, NULL, NULL, plucall_department);
			Menu_AddEndFunction(2, callback_end_chageunit_dept);
		}

		//caption_split_by_code(2, &cap,0);
		//caption_adjust(&cap,NULL);
		plu_get_display_info(PTYPE_PLU_NO, &cap, 0);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &plu_no, NULL, NULL, plucall_plu_number);
		Menu_AddEndFunction(3, callback_end_chageunit_plu);

		caption_split_by_code(0x145B, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &quantity, NULL, NULL, NULL);
#endif
PAGE_VIEW:
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{	

			plu_ptype =plu_get_plu_type(dept_no, plu_no);

			if (plu_ptype == 1)
			{
				error_display_caption(0xF052);	//Check PLU type
				goto PAGE_VIEW;
			}

			job_batch_str.code = currentCode;
			job_batch_str.deptNO = dept_no;
			job_batch_str.pluNO = plu_no;
			job_batch_str.quantity = quantity;
			job_batch_str.salesMsgNO = 0;
			job_batch_str.shopNameNO = 0;
			job_batch_str.unitPrice = 0;
			flash_addr = addr_origin + JOB_BATCH_STRUCT_SIZE * (currentCode-1) ;
			Flash_swrite(flash_addr, (HUGEDATA INT8U *)&job_batch_str, JOB_BATCH_STRUCT_SIZE);
		}
		else if(result == MENU_RET_ESC)
		{
			break;
		}
	}
}

#ifndef DISABLE_LIST_MENU
void  job_batch_list(void)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	JOB_BATCH_STRUCT  batch_str;
	INT16U dlength;

	char   	string_buf[34], end_flag = 0;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, str_size, reverse_num;
	INT16S	j = 0, result, min_num, max_num;
	INT16S	line_ct, lcd_size, start_id;
	INT16U	read_code;
	INT32U	addr, addr_flash;
	INT8U	linenum;			    //se-hyung 20071002	     

	str_size = JOB_BATCH_STRUCT_SIZE;

LIST_START:
	caption_split_by_code(0x145A, &cap,0);
	min_num	= cap.input_min;					
	max_num = cap.input_max; 					 
	dlength = cap.input_dlength;
	if(min_num <= 0)  {
		disp_p = point(0,0);
		message_error(disp_p);
	}
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1452,1,1);     
	linenum = Startup.line_max;			      //se-hyung 20071002 
	if (linenum > 3) linenum = 3;			      //se-hyung 20071002  
	for(i = 0;i<linenum; i++)  {			      //se-hyung 20071002 
//	for(i = 0; i<Startup.line_max; i++)  {		      //delete by se-hyung  20071002
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
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long,cap.input_dlength, 
				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
			arab_codetemp[strlen(cap.form)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);

			result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long,cap.input_dlength, 
				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long,cap.input_dlength, 
  				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
			case KP_ARROW_UP:  
					   if(i > 0)	i--;
					   else 	BuzOn(2);
					   goto REN1;    					   
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
		}
	}

	
	lcd_size   = Dsp_GetChars();
	line_ct    = Startup.line_max;
	addr_flash = DFLASH_BASE + FLASH_JOBBATCH_AREA;
	cur_id     = start_num;
	start_id   = start_num;

READ_START:
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FE, &cap,0);
//		disp_p = point(0,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071002    
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
		if(read_code == cur_id)  
		{	
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&batch_str, str_size ); 
		 	sprintf(string_buf, 
		 		"%3d  %6ld  %2d", 
		 		batch_str.code, 
		 		batch_str.pluNO, 
		 		batch_str.quantity);
			string_buf[lcd_size] = 0;	       		
			
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);
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
			//Dsp_Diffuse();
			if (j == 1)
				start_id = cur_id;	
			if (j == line_ct) break;
		}
		
		cur_id++;	
	}
	Dsp_Diffuse();
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
			read_code = Flash_wread(addr);
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
			addr =  addr_flash + (long)str_size * (cur_id-1);			
			read_code = Flash_wread(addr);
			if(read_code == cur_id )  
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
			addr =  addr_flash + (long)str_size * (reverse_num-1);			
			read_code = Flash_wread(addr);
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
		//job_batch_print_list(start_num, end_num);
		goto KEY_INPUT;	
	case KP_FEED:
		keyapp_feed(OFF);
		goto KEY_INPUT;	
	default :	
		BuzOn(2);
		goto KEY_INPUT;
	} 

	RET_ESC:    return;

}
#endif //#ifndef DISABLE_LIST_MENU


void job_batch_delete(void)			// command code  0x1453
{	
	menu1_delete(0x1453, 0x145A, FLASH_JOBBATCH_AREA, JOB_BATCH_STRUCT_SIZE);
}

/*
void job_batch_delete(void)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	JOB_BATCH_STRUCT  	batch_str;

	INT32U	Flash_addr=0;
	long 	ret_long=1;
	INT16S  	result, current_num;


	Dsp_Fill_Buffer(0);	

	menu_display_menucode(CurMenuCode, 1);
	menu_displaytitle(0x1453,0);
	menu_writepage(1, 1);
    	
	caption_split_by_code(0x145A, &cap,0);
	caption_adjust(&cap,NULL);
	disp_p = point(Startup.menu_type*Startup.menu_ygap,0);
	Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	Dsp_Diffuse();

REN1:	DspStruct.DirectDraw = 1;   
	result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_length, 
			   				Startup.menu_type*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );   
	switch (result) 
	    {			
		case KP_ENTER:	if ((ret_long>cap.input_max) || (ret_long<cap.input_min)) {	
				BuzOn(2);	goto REN1;	}
				current_num = ret_long - 1;	  	
				break;
		case KP_ESC:	 goto RET_ESC; 
			
	     	default:	BuzOn(2);	
				goto REN1;		     	 	
	    }

	memset((INT8U*)&batch_str, 0x00, JOB_BATCH_STRUCT_SIZE);

	Flash_addr =  DFLASH_BASE + FLASH_JOBBATCH_AREA;
	Flash_addr += JOB_BATCH_STRUCT_SIZE * current_num;
	Flash_swrite(Flash_addr, (HUGEDATA INT8U *)&batch_str, JOB_BATCH_STRUCT_SIZE);

RET_ESC:
	Dsp_Fill_Buffer(0);					
	Dsp_Diffuse();
 	return;


}
*/

void callback_end_convtable_no(INT32U num)
{	
	SCANNER_CONV_TABLE_STR  scannerConvTable;
	INT32U  flash_addr;
	INT16U  read_code;
	INT8U dept_id, plu_id, scanCode_id;

	if(!Startup.disable_dept) dept_id = Menu_GetMenuId(3);
	plu_id = Menu_GetMenuId(4);
	scanCode_id = Menu_GetMenuId(2);

#ifdef USE_10000_SCANNER_CONVTABLE
	flash_addr =  EXT_MEMCARD2_ADDR_X + FLASH_SCANNER_TABLE_AREA;
#else
	flash_addr =  DFLASH_BASE + FLASH_SCANNER_TABLE_AREA;
#endif
	flash_addr += SCANNER_CONV_TABLE_STR_SIZE * (num-1) ; 
	read_code = Flash_wread(flash_addr);
	if(read_code == (INT16U)num)
	{		
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&scannerConvTable, SCANNER_CONV_TABLE_STR_SIZE);
	}
	else 
	{
		memset(&scannerConvTable, 0, SCANNER_CONV_TABLE_STR_SIZE);
	}
	scannerConvTable.code = (INT16U)num;
	if(!scannerConvTable.dept_no) scannerConvTable.dept_no = 1;
	if(Startup.disable_dept) scannerConvTable.dept_no = 1;
	if(!scannerConvTable.plu_no) scannerConvTable.plu_no = 1;
	memcpy(Menu[scanCode_id].pstr, scannerConvTable.scanCode, sizeof(scannerConvTable.scanCode));
	if(!Startup.disable_dept) *Menu[dept_id].pval = scannerConvTable.dept_no;
	*Menu[plu_id].pval = scannerConvTable.plu_no;
	
	MenuRefresh = ON;
}

void scanner_convtable_create(void)
{
	CAPTION_STRUCT 	   cap;	
	SCANNER_CONV_TABLE_STR  scannerConvTable;
	INT8U 	result;
	INT32U  current_num;
	INT16U  str_size;
 	INT32U	addr_origin, flash_addr;
	INT32U  dept_no;
	INT32U  plu_no;
	INT8U   scan_code[14];
	INT16U  read_code; 

#ifdef USE_10000_SCANNER_CONVTABLE
	addr_origin =  EXT_MEMCARD2_ADDR_X + FLASH_SCANNER_TABLE_AREA;
#else
	addr_origin =  DFLASH_BASE + FLASH_SCANNER_TABLE_AREA;
#endif
	str_size = SCANNER_CONV_TABLE_STR_SIZE;

	while (1)
	{	
		current_num = search_pgm_next_id(0x146A, str_size, addr_origin);
		if(!current_num) {
			error_display_caption(0xF035);
			current_num = 1;
		}

		flash_addr = addr_origin + str_size * (current_num - 1); 
		read_code = Flash_wread(flash_addr);
		if(read_code == (INT16U)current_num)
		{		
			Flash_sread(flash_addr, (HUGEDATA INT8U*)&scannerConvTable, str_size);
		}
		else 
		{
			memset(&scannerConvTable, 0, str_size);
		}
		scannerConvTable.code = current_num;
		if(!scannerConvTable.dept_no) scannerConvTable.dept_no = 1;
		if(Startup.disable_dept) scannerConvTable.dept_no = 1;
		if(!scannerConvTable.plu_no) scannerConvTable.plu_no = 1;
		dept_no = scannerConvTable.dept_no;
		plu_no = scannerConvTable.plu_no;
		memcpy(scan_code, scannerConvTable.scanCode, sizeof(scannerConvTable.scanCode));


		Menu_Init();

		caption_split_by_code(0x1461, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x146A, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &current_num, NULL, NULL, NULL);
		Menu_AddEndFunction(1, callback_end_convtable_no);

		caption_split_by_code(0x146B, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, scan_code, NULL, NULL);

////////////// dept, plu
		if (Startup.disable_dept) 
		{
			Menu_CallbackDeptNo = 1;
		}
		else
		{
			//caption_split_by_code(1, &cap,0);
			//caption_adjust(&cap,NULL);
			plu_get_display_info(PTYPE_DEPARTMENT_NO, &cap, 0);
			Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &dept_no, NULL, NULL, plucall_department);
			Menu_AddEndFunction(3, callback_end_chageunit_dept);
		}

		//caption_split_by_code(2, &cap,0);
		//caption_adjust(&cap,NULL);
		plu_get_display_info(PTYPE_PLU_NO, &cap, 0);
		Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &plu_no, NULL, NULL, plucall_plu_number);
		Menu_AddEndFunction(4, callback_end_chageunit_plu);
///////////////////////////

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			scannerConvTable.code = current_num;
			scannerConvTable.dept_no = dept_no;
			scannerConvTable.plu_no = plu_no;
			memcpy(scannerConvTable.scanCode, scan_code, sizeof(scannerConvTable.scanCode));
			flash_addr = addr_origin + str_size * (current_num - 1) ;
			Flash_swrite(flash_addr, (HUGEDATA INT8U *)&scannerConvTable, str_size);
		}
		else if(result == MENU_RET_ESC)
		{
			break;
		}
	}
}

#ifndef DISABLE_LIST_MENU
void  scanner_convtable_list(void)
{

	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	SCANNER_CONV_TABLE_STR  scannerConvTable;
	INT16U dlength;
	INT8U	linenum;			    //se-hyung 20071002

	char   	string_buf[34], end_flag = 0;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, str_size, reverse_num;
	INT16S	j = 0, result, min_num, max_num;
	INT16S	line_ct, lcd_size, start_id;
	INT16U	read_code;
	INT32U	addr, addr_flash;

	str_size = SCANNER_CONV_TABLE_STR_SIZE;

LIST_START:
	caption_split_by_code(0x146A, &cap,0);
	min_num	= cap.input_min;					
	max_num = cap.input_max; 					 
	dlength = cap.input_dlength;
	if(min_num <= 0)  {
		disp_p = point(0,0);
		message_error(disp_p);
	}
	
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1462,1,1);
	linenum = Startup.line_max;			      //se-hyung 20071002 
	if (linenum > 3) linenum = 3;			      //se-hyung 20071002  
	for(i = 0;i<linenum; i++)  {			      //se-hyung 20071002
//	for(i = 0; i<Startup.line_max; i++)  {
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
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long,cap.input_dlength, 
				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long,cap.input_dlength, 
				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long,cap.input_dlength, 
  				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
			case KP_ARROW_UP:  
					   if(i > 0)	i--;
					   else 	BuzOn(2);
					   goto REN1;    					   
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
		}
	}

	
	lcd_size   = Dsp_GetChars();
	line_ct    = Startup.line_max;
#ifdef USE_10000_SCANNER_CONVTABLE
	addr_flash = EXT_MEMCARD2_ADDR_X + FLASH_SCANNER_TABLE_AREA;
#else
	addr_flash = DFLASH_BASE + FLASH_SCANNER_TABLE_AREA;
#endif
	cur_id     = start_num;
	start_id   = start_num;

READ_START:
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FD, &cap,0);
//		disp_p = point(0,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071002
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
		if(read_code == cur_id)  
		{	
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&scannerConvTable, str_size ); 

		 	sprintf(string_buf, 
		 		"%4d %-13s%5d%6ld", 
		 		scannerConvTable.code, 
		 		scannerConvTable.scanCode,
		 		scannerConvTable.dept_no,
		 		scannerConvTable.plu_no);

			string_buf[lcd_size] = 0;	       		
			
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);	       		 
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0); 
			hebrew_codetemp[strlen(string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//list, 기존 아랍ROM 적용되어 있음
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
			//Dsp_Diffuse();
			if (j == 1)
				start_id = cur_id;	
			if (j == line_ct) break;
		}
		
		cur_id++;	
	}
	Dsp_Diffuse();
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
			read_code = Flash_wread(addr);
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
			addr =  addr_flash + (long)str_size * (cur_id-1);			
			read_code = Flash_wread(addr);
			if(read_code == cur_id )  
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
			addr =  addr_flash + (long)str_size * (reverse_num-1);			
			read_code = Flash_wread(addr);
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
		scanner_convtable_print_list(start_num, end_num);
		goto KEY_INPUT;	
	case KP_FEED:
		keyapp_feed(OFF);
		goto KEY_INPUT;	
	default :	
		BuzOn(2);
		goto KEY_INPUT;
	} 

	RET_ESC:    return;


}
#endif


void scanner_convtable_delete(void)			// command code  0x1463
{
	menu1_delete(0x1463, 0x146A, FLASH_SCANNER_TABLE_AREA, SCANNER_CONV_TABLE_STR_SIZE);
}

void scanner_config(void)	//파라미터 558의 세팅에 따라 입력 FORM의 개수 1개,10개 
{
	CAPTION_STRUCT cap;
	INT16U result;
	INT8U bar_size;
#ifdef USE_LOTTEMART_MEATSCANNER
	INT8U scannerBarForm[8][SCANNER_BARCODE_DATA_LEN+1];	//8개 41
#else
	INT8U scannerBarForm[10][SCANNER_BARCODE_DATA_LEN+1];	//10개, 사이즈 변경 14 -> 30
#endif
	INT8U  Scanform;
	INT8U  modeScanner;
	INT8U i;		
	INT32U addr;
	INT8U  ucTemp;
	INT8U form_string[56];
#if defined(USE_SINGLE_LINE_GRAPHIC)
	INT8U temp_string[30];
#endif

	ucTemp = get_global_bparam(GLOBAL_SCANNER_FLAG1);
	Scanform = ucTemp&0x10;
	modeScanner = ucTemp&0x07;

#ifdef USE_LOTTEMART_MEATSCANNER
	if(modeScanner == 4) bar_size = SCANNER_BARCODE_DATA_LEN+1;
#else
	if(modeScanner == 4) bar_size = 32;
#endif
	else bar_size = 13;  

	strcpy((char*)form_string, SCAN_STRING);	// 'I','W','D','N','T','X','P','e','S','0'~'9'

#ifdef USE_LOTTEMART_ITEMCODE13
	strcat((char*)form_string, SCAN_STRING_ITEMCODE13); //13자리 아이템 코드
#endif
#ifdef USE_LOTTEMART_MEATSCANNER
	strcat((char*)form_string, SCAN_STRING_SL_NO2); //도축장2
	strcat((char*)form_string, SCAN_STRING_SL_NO3); //도축장3
#endif
#ifdef USE_TRACE_STANDALONE
	strcat((char*)form_string, SCAN_STRING_KOR);	// 도축장,용도,원산지
#ifdef USE_TRACE_MEATPART
	strcat((char*)form_string, SCAN_STRING_KOR_MEATPART);	// 부위
#endif
#ifdef USE_TRACE_MEATGRADE
	strcat((char*)form_string, SCAN_STRING_KOR_MEATGRADE);	// 축산등급
#endif
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	strcat((char*)form_string, SCAN_STRING_RECEIPT_NO);	// 영수증 번호
#endif
#ifdef USE_HYUNDAI_CHECK_REASON_CODE
	strcat((char*)form_string, SCAN_STRING_REASON_CODE); // 사유코드
#endif
	Menu_Init();
	caption_split_by_code(0x1464, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	if(Scanform)
	{
		addr = DFLASH_BASE  + FLASH_SCANNER_FORMAT;
#ifdef USE_LOTTEMART_MEATSCANNER
		for(i=0;i<8;i++)
#else
		for(i=0;i<10;i++)
#endif
		{
			Flash_sread(addr+(i*BAR_FORMAT_SIZE), scannerBarForm[i], bar_size);
			scannerBarForm[i][bar_size] = 0;
			if(scannerBarForm[i][0]==0xff) memset(&scannerBarForm[i][0], 0, sizeof(scannerBarForm[0]));		//초기화
			caption_split_by_code(0x146C, &cap, 0);
#if defined(USE_SINGLE_LINE_GRAPHIC)
			sprintf((char*)temp_string, "%02d", i+1);
			strcat(cap.form, (char*)temp_string);
#else
			sprintf(cap.form,"%02d : ",i+1);
#endif

			cap.input_length = bar_size;
			caption_adjust(&cap,NULL);
			Menu_InputCreate(i+1, (HUGEDATA char *)cap.form, MENU_TYPE_BAR, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min,0,NULL, scannerBarForm[i], form_string, NULL);
			Menu_SetProperties(i+1, MENU_P_ENGLISH_MODE);
		}
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE)
		{
#ifdef USE_LOTTEMART_MEATSCANNER
			for(i=0;i<8;i++)
#else
			for(i=0;i<10;i++)
#endif
			{
				Flash_swrite(addr+(i*BAR_FORMAT_SIZE), scannerBarForm[i], bar_size + 1);	// NULL을 저장하기위해 (bar_size+1)
			}
		}
	}
	else
	{
		get_global_sparam(GLOBAL_SCANNER_FORMAT, scannerBarForm[0] ,14);
		scannerBarForm[0][13] = 0;	 //EAN13 Only
		
		caption_split_by_code(0x146C, &cap, 0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_BAR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min,0,NULL, scannerBarForm[0], form_string, NULL);
		Menu_SetProperties(1, MENU_P_ENGLISH_MODE);
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) set_global_sparam(GLOBAL_SCANNER_FORMAT, scannerBarForm[0], 14);
	}
}

//extern INT8U LabelIdSetPosition;

void callback_end_labelkey(INT32U num)//gm 090419, 라벨포맷 변경 = 라벨 + 바코드 (menu 1471)
{	
	INT32U  addr;

	addr = FLASH_LABEL_KEY_TABLE_AREA + DFLASH_BASE;
//#ifdef USE_TRACE_STANDALONE
	addr += (num - 1) * sizeof(LABEL_KEY_STRUCT);
	*Menu[1].pval = (long)Flash_wread(addr);
	addr += 2; //barcode
	*Menu[2].pval = (long)Flash_wread(addr);
//#else
//	addr += (num - 1) * 2;
//	*Menu[1].pval = (long)Flash_wread(addr);
//#endif
	MenuRefresh = ON;
}

void label_format_key_table(void)
{
	CAPTION_STRUCT 	   	cap;
	INT8U 	result;
	INT32U  addr;
//	INT16S	inputMaxKeyNum = 8;
	INT16S	inputMaxKeyNum = 20;
	INT8U	strSize;
	INT32U	index;
	INT32U	labelfmtNo;
	INT32U	barfmtNo;

	index = 1;
	strSize = sizeof(LABEL_KEY_STRUCT);
	
	while(1)
	{
		addr = FLASH_LABEL_KEY_TABLE_AREA + DFLASH_BASE;
		if(index > inputMaxKeyNum) index = inputMaxKeyNum;
		addr += (index - 1) * strSize;
		labelfmtNo = Flash_wread(addr);
		barfmtNo = Flash_wread(addr+2);
	    
		Menu_Init();
		caption_split_by_code(0x1470, &cap, 0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x147A,&cap,0);
		caption_adjust(&cap,NULL);
        
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &index, NULL, NULL, NULL); 		
		Menu_AddEndFunction(1, callback_end_labelkey);	       
		Menu_SetKeyMode(1, 3);
        
		caption_split_by_code(0x2018, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &labelfmtNo, NULL, NULL, plucall_label_id);
		caption_split_by_code(0x352B, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &barfmtNo, NULL, NULL, NULL);//don't use callback_barcode
		Menu_SetCallbackMode(2, 0);	// 0: right, 1:below

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			addr = FLASH_LABEL_KEY_TABLE_AREA + DFLASH_BASE;
//#ifdef USE_TRACE_STANDALONE
			addr += (index - 1) * strSize;
			Flash_wwrite(addr, (INT16U)labelfmtNo);
			Flash_wwrite(addr+2, (INT16U)barfmtNo);
			index++;
//#else
//			addr += (input_value[0] - 1) * 2;
//			Flash_wwrite(addr, (INT16U)input_value[1]);
//			input_value[0]++;
//#endif
		}
		else if(result == MENU_RET_ESC)
		{
			break;
		}
	}
}

INT16U get_labelid_from_labelkey_num(INT32U num)
{	
	INT32U  addr;
	INT16U	labelfmtNo;
	INT16S	inputMaxKeyNum = 20; // 최대 라벨 포맷 키 갯수 현재 20 (KS_LABEL_01 ~ KS_LABEL_20)

	if(num > inputMaxKeyNum) return 0;
	addr = FLASH_LABEL_KEY_TABLE_AREA + DFLASH_BASE;
	addr += (num - 1) * sizeof(LABEL_KEY_STRUCT);
	labelfmtNo = Flash_wread(addr);

	return labelfmtNo;
}
