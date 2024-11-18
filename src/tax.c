/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	tax.c
|*  Version		:	1.0
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created		:	2003/12/09
|*  Description		:	Same type is "department.c"
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
#include "prt_list.h"
#include "tax.h"
#include "mode_error.h"

#ifdef USE_TAX_RATE
void tax_dataUpdate(INT32U num)	   //se-hyung 20071010
{
	TAX_STRUCT  	tax_str;
	INT16U  strSize;
	INT32U	addr;
	INT16U  TaxIndex;
	INT8U   k;

	strSize = sizeof(tax_str);		      
	addr = DFLASH_BASE;
	addr += FLASH_TAX_AREA;;
	addr += ((num - 1) * strSize);
	TaxIndex = Flash_wread(addr);
	if(TaxIndex == num){
		Flash_sread(addr, (HUGEDATA INT8U *)&tax_str, strSize );
	} else {
		memset(&tax_str.tax_code, 0, strSize );
		tax_str.tax_code = num;		
	}
	k = 0;
	*Menu[k++].pval = tax_str.tax_code;
	*Menu[k++].pval = tax_str.tax_type;
	*Menu[k++].pval = tax_str.tax_rate;

	MenuRefresh = ON;
}


void  tax_create(void)		 //se-hyung 20071009
{
	TAX_STRUCT  	tax_str;
	CAPTION_STRUCT 	  cap;	
 	INT16U	str_size; 
	INT16U	result, old_font, key_mode;	    
	INT8U	input_max;
	INT8U	i;
	 
	INT32U	addr_start; 
	INT32U	addr_flash;
	INT32U set_val[3];

	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	set_val[0] = 1;

	addr_start =  DFLASH_BASE + FLASH_TAX_AREA;
	// Get Last Number
	caption_split_by_code(8, &cap,0);
	for (i=0; i<MAX_TAX_NO-1; i++) {
		addr_flash = addr_start + (TAX_STRUCT_SIZE * i);
		set_val[0] = Flash_wread(addr_flash);
		if ((set_val[0]<cap.input_min) || (set_val[0]>cap.input_max)) {
			set_val[0]=i+1;
			break;
		}
	}
	if ((set_val[0]<cap.input_min) || (set_val[0]>cap.input_max)) set_val[0]=cap.input_min;

	while(1)
	{    
		str_size = sizeof(tax_str);
		addr_start =  DFLASH_BASE + FLASH_TAX_AREA;
		addr_flash = addr_start + (str_size * (set_val[0]-1));
		Flash_sread(addr_flash, (HUGEDATA INT8U *)&tax_str, str_size); 

		if(tax_str.tax_code != set_val[0])  {
			memset((INT8U*)&tax_str, 0x00, str_size);
		}
	       	tax_str.tax_code = set_val[0];
		set_val[0] = tax_str.tax_code;
	     	set_val[1] = tax_str.tax_type;   
	      	set_val[2] = tax_str.tax_rate; 

		Menu_Init();
		caption_split_by_code(0x1231, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(8, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);

		input_max = cap.input_max;
 		Menu_AddEndFunction(1, tax_dataUpdate);
		
		caption_split_by_code(0x3232, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);

		caption_split_by_code(0x3233, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 2, &set_val[2], NULL, NULL, NULL);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			addr_flash = addr_start + (str_size * (set_val[0]-1));
			memset((INT8U*)&tax_str, 0x00, str_size);
			
		       	tax_str.tax_code = set_val[0];
			tax_str.tax_type = set_val[1];   //serving size
			tax_str.tax_rate = set_val[2];    //serving per container
			Flash_swrite(addr_flash, (HUGEDATA INT8U *)&tax_str, str_size);
		
			if(set_val[0] <input_max) set_val[0]++;
			else   set_val[0] =input_max; 
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

void callback_tax_type(long num)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
 	char		buffer[64];	
 	disp_p = point((1+Startup.menu_type)*Startup.menu_ygap, 1*Startup.menu_xgap);
	memset((INT8U *)&buffer,0x20, sizeof(buffer) );
	buffer[63] = 0;
	DspStruct.DirectDraw = 1;
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)&buffer, strlen(buffer), 0, 0); 
	hebrew_codetemp[strlen(buffer)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)&buffer);
#endif

	// <CODE> JHC, 040628, Modify, -get caption
	// [
	caption_split_by_code(0x2041, &cap,0);
	caption_adjust(&cap,NULL);
	if(num < cap.input_min ||num > cap.input_max) {
	// ]
		DspStruct.DirectDraw = 1;
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

	} else {
		caption_split_by_code((0x2044+num), &cap,0);		//modified by JJANG 200706 
		DspStruct.DirectDraw = 1;
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
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
		DspStruct.Reverse = 0;
	}
}

void  tax_delete(void)		// command code  0x1222
{
	menu1_delete(0x1233, 8, FLASH_TAX_AREA, TAX_STRUCT_SIZE);		
}

#ifndef DISABLE_LIST_MENU
void  tax_list(void)			// command code  0x1223
{		
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	TAX_STRUCT	tax_str;
	
//	INT8U	y_gab=8;				// 폰트의 y 크기
//	INT8U	x_gab=6;				// 폰트의 x 크기

	char   	string_buf[34], taxtype, end_flag = 0, reverse_num;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id;
	INT16S	j = 0, result, min_num, max_num, str_size, lcd_size, line_ct, start_id;
	INT16U	read_code, input_dlength;
	INT32U	addr, addr_flash, cap_addr;	

	// <CODE> JHC, 040414, -tax_list
	// [
	// $1: JHC, 040414, Modify, -0x1242 To 0x1232
	INT16U	code = 0x1232, cap_code = 0x1E00;

	str_size = TAX_STRUCT_SIZE;	
	// $3: JHC, 040531, Modify
LIST_START:
//	cap_addr = caption_search_by_code(0x123A);
//	_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));		//'
	caption_split_by_code(8, &cap,0);

	caption_adjust(&cap,NULL);
	min_num	= cap.input_min;					//
	max_num = cap.input_max;
	input_dlength = cap.input_dlength; 					// 
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
		cap.input_min=min_num;
		cap.input_max=max_num;
		cap.input_dlength=input_dlength;
		cap.input_length =input_dlength;
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

		cap_addr = addr + CAP_MSG_LENGTH * i;
//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		cap.input_min=min_num;
		cap.input_max=max_num;
		cap.input_dlength=input_dlength;
		cap.input_length =input_dlength;
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, input_dlength,//4, 
				   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, input_dlength,//4, 
				   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, input_dlength,//4, 
				   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
			case KP_ARROW_UP:  
					// <CODE> JHC, 040531, Modify
					// [
					if(i > 0) 	i--;
					else		BuzOn(2);
					// ]
					goto REN1;    					   
			case KP_ESC:
				   	goto RET_ESC;

			case KP_SAVE:	start_num = min_num;
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
	addr_flash = DFLASH_BASE + FLASH_TAX_AREA;
	cur_id     = start_num;
	start_id   = start_num;

	// <DEBG> JHC, 040329, New, -DebugCode
	// [
	// $1: JHC, 040529, Comment, -Not use
	// display_ulong(1, end_num, 0);
	// display_primary_diffuse();
	// ]

READ_START:
	// <CODE> JHC, 040531, Modify
	// [
	// $MEMO: -modify display list all
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FF, &cap,0);	// 1239
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
		addr =  addr_flash + (long)TAX_STRUCT_SIZE * (cur_id-1);			
		read_code = Flash_wread(addr);
		if(read_code == cur_id /*>= min_num && read_code <= max_num*/)  
		{	
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&tax_str, TAX_STRUCT_SIZE ); 
		 	switch(tax_str.tax_type)  
		 	{
				case 0: taxtype = 'I'; 	break;
				case 1:	taxtype = 'E';	break;
			}
#ifdef USE_INDIA_TAX
		 	sprintf(string_buf, 
		 		"%2d    %c      %6.2f %%  %6.2f %%", 
				tax_str.tax_code, 
				taxtype, 
				(float)tax_str.tax_rate/100*0.5,(float)tax_str.tax_rate/100*0.5);
#else
		 	sprintf(string_buf, 
		 		"%2d    %c      %6.2f %%", 
				tax_str.tax_code, 
				taxtype, 
				(float)tax_str.tax_rate/100);
#endif
			string_buf[lcd_size] = 0;	       		
			
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);	       		 
			Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
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
			addr =  addr_flash + (long)TAX_STRUCT_SIZE * (cur_id-1);			
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
			addr =  addr_flash + (long)TAX_STRUCT_SIZE * (cur_id-1);			
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
			addr =  addr_flash + (long)TAX_STRUCT_SIZE * (cur_id-1);			
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
			addr =  addr_flash + (long)TAX_STRUCT_SIZE * (reverse_num-1);			
			read_code = Flash_wread(addr);
			if(read_code == reverse_num /*>= min_num && read_code <= max_num*/ )  
			{
			       	cur_id = reverse_num;
				goto READ_START;
			}
		}
		BuzOn(2);
		error_display_caption(0xE00E);
		goto KEY_INPUT;
	case KP_TEST:
		// <CODE> JHC, 040531, Modify
		// [
		tax_print_list(start_num, end_num);
		// ]
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