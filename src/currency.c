#include <stdio.h>
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
#include "department.h"
#include "tare_file.h"
#include "menu_code_text1.h"
#include "pgm_app.h"
#include "adm_app.h"
#include "prt_list.h"
#include "currency.h"
#include "mode_error.h"


void currency_dataUpdate(INT32U num)	   //se-hyung 20071012
{
	CURRENCY_STRUCT  cur_str;
	INT16U  strSize;
	INT32U	addr;

	
	strSize = sizeof(cur_str);		      
	addr = DFLASH_BASE;
	addr += FLASH_CURRENCY_AREA;
	addr += ((num-1) * (INT32U)strSize);

	Flash_sread(addr,(HUGEDATA INT8U *)&cur_str,strSize);
	if ((cur_str.rate<0) || (cur_str.rate>9999999l)) {
		memset((INT8U *)&cur_str,0,strSize);
		cur_str.ratepoint=4;
	}


	*Menu[0].pval = num;
	*Menu[1].pval = cur_str.ratepoint;
	*Menu[2].pval = cur_str.rate;
	Menu[2].decimal = (INT8U)cur_str.ratepoint; 	
 	memcpy(Menu[3].pstr, cur_str.primary, Menu[3].length); 
   	memcpy(Menu[4].pstr, cur_str.last, Menu[4].length); 
	*Menu[5].pval = cur_str.decimalpoint;
	*Menu[6].pval = cur_str.prtform;

      	MenuRefresh = ON;
}

void currency_rateUpdate(INT32U num)
{
	Menu[2].decimal = (INT8U)num; 	

      	MenuRefresh = ON;
}

void currency_create(void)		 //se-hyung 20071015
{
	CURRENCY_STRUCT	cur_str;
	CAPTION_STRUCT  cap;	
//	INT8U	max_item;
 	INT16U	str_size; 
	INT16U	result;	    
	INT8U	input_max;
	INT8U	i;
	INT8U 	set_str[2][4];
//	INT16S     max;
	INT32U	addr_start; 
	INT32U	addr_flash;
	INT32U 	set_val[5];

//	max_item = 7;
	set_val[0] = 1;
	str_size = sizeof(CURRENCY_STRUCT);
	
	addr_start =  DFLASH_BASE + FLASH_CURRENCY_AREA;
	// Get Last Number
	caption_split_by_code(0x3911, &cap,0);
//	max=(INT16S)cap.input_max;
	for (i=0; i<MAX_CURRENCY_NO; i++) {
		addr_flash = addr_start + (str_size * i);
		Flash_sread(addr_flash,(HUGEDATA INT8U *)&cur_str,str_size);
		if ((cur_str.rate<0) || (cur_str.rate>999999l)) {
			set_val[0]=i+1;
			break;
		}
	}
	
	if ((set_val[0]<cap.input_min) || (set_val[0]>cap.input_max)) set_val[0]=1;
	if (set_val[0]>MAX_CURRENCY_NO) { set_val[0]=1; }

	while(1)
	{    
		str_size = sizeof(cur_str);
		addr_start =  DFLASH_BASE + FLASH_CURRENCY_AREA;
		addr_flash = addr_start + (str_size * (set_val[0]-1));
		Flash_sread(addr_flash, (HUGEDATA INT8U *)&cur_str, str_size); 

		if ((cur_str.rate<=0) || (cur_str.rate>9999999l)) {
			memset((INT8U *)&cur_str,0,str_size);
			cur_str.ratepoint=4;
		}

	        set_val[1] = cur_str.ratepoint;   
	      	set_val[2] = cur_str.rate;
			strcpy((HUGEDATA char*)set_str[0], (HUGEDATA char*)cur_str.primary); 
			strcpy((HUGEDATA char*)set_str[1], (HUGEDATA char*)cur_str.last);				      	 
	     	set_val[3] = cur_str.decimalpoint;
	     	set_val[4] = cur_str.prtform;

		Menu_Init();
		caption_split_by_code(0x1441, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x3911, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);

		input_max = cap.input_max;
 		Menu_AddEndFunction(1, currency_dataUpdate);
		
		caption_split_by_code(0x3912, &cap,0);
		caption_adjust(&cap,NULL);
		cap.input_min = 0;	// 향후 Caption.xls 조정후 삭제
		cap.input_max = 5;	// 향후 Caption.xls 조정후 삭제
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);
 		Menu_AddEndFunction(2, currency_rateUpdate);

		caption_split_by_code(0x3913, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, (INT8U)set_val[1], &set_val[2], NULL, NULL, NULL);

		caption_split_by_code(0x3914, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, set_str[0], NULL, NULL);

		caption_split_by_code(0x3915, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(5, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, set_str[1], NULL, NULL);

		caption_split_by_code(0x3916, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(6, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[3], NULL, NULL, NULL);

		caption_split_by_code(0x4637, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(7, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[4], NULL, NULL, NULL);
							
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			addr_flash = addr_start + (str_size * (set_val[0]-1));
			memset((INT8U*)&cur_str, 0x00, str_size);
						
			cur_str.ratepoint = set_val[1];   
			cur_str.rate = set_val[2];    
			strcpy((HUGEDATA char*)cur_str.primary, (HUGEDATA char*)set_str[0]);
			strcpy((HUGEDATA char*)cur_str.last, (HUGEDATA char*)set_str[1]);			
			cur_str.decimalpoint = set_val[3];
			cur_str.prtform = set_val[4];
			Flash_swrite(addr_flash, (HUGEDATA INT8U *)&cur_str, str_size);
		
			if(set_val[0] <input_max) set_val[0]++;
			else   set_val[0] =input_max; 
		}
			else if(result ==MENU_RET_ESC)
			{
				break;
			}
	}
}

void  currency_delete(void)
{
	menu1_delete(0x1443, 0x3911, FLASH_CURRENCY_AREA, sizeof(CURRENCY_STRUCT));
}

#ifndef DISABLE_LIST_MENU
void currency_list(void)
{
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	CURRENCY_STRUCT	cur_str;
//	char   	string_buf[34], end_flag = 0, reverse_num, currate_str[20];	// Modified by CJK 20050903
	char   	string_buf[34], reverse_num, currate_str[20];	// Modified by CJK 20050903    
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id;
//	INT16S	j = 0, result, min_num, max_num, str_size, lcd_size, line_ct, start_id;
	INT16S	j = 0, result, min_num, max_num, str_size, line_ct, start_id;    
	INT32U	addr, addr_flash, cap_addr;	

	INT16U	code = 0x1442, cap_code = 0x1E00;
	str_size = sizeof(CURRENCY_STRUCT);

LIST_START:
	caption_split_by_code(0x3911, &cap,0);
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
	display_title_page_change(code,1,1);

	addr = caption_search_by_code(cap_code);		
	for(i = 0; i<3; i++)  {
		cap_addr = addr + CAP_MSG_LENGTH * i;
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
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
REN1:		if(i == 0) ret_long = start_num;		
		if(i == 1) ret_long = end_num;
		cap_addr = addr + CAP_MSG_LENGTH * i;
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
		
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,  0,(long*)&ret_long, 4,
					(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,  0,(long*)&ret_long, 4,
				 (i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, 4,
					(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
			case KP_ARROW_UP:  
					if(i > 0) 	i--;
					else		BuzOn(2);
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
					   if ( ret_long > max_num || ret_long < min_num ) {
						error_display_form1(0, min_num, max_num);
						goto REN1;	
				           }
					   if(i == 0)	start_num = ret_long;
					   if(i == 1)	end_num = ret_long;
					   break;
					   				  				
		     	default: BuzOn(2);
		     		goto REN1;		     	 	
		}
	}
//	lcd_size   = Dsp_GetChars();
	line_ct    = Startup.line_max; //(Startup.menu_type?3:2);
	addr_flash = DFLASH_BASE + FLASH_CURRENCY_AREA;
	cur_id     = start_num;
	start_id   = start_num;
READ_START:
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type) {
		caption_split_by_code(0x71FF, &cap,0);	// 1239
//		disp_p = point(0,0);
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
	while (cur_id <= end_num) {
		addr =  addr_flash + (long)str_size * (cur_id-1);			
		Flash_sread(addr, (HUGEDATA INT8U*)&cur_str, str_size); 
		format_value((HUGEDATA INT8U *)currate_str, cur_str.rate, cur_str.ratepoint,'.');		// Added by CJK 20050903
		if ((cur_str.rate>0) && (cur_str.rate<=9999999l)) {
			j++;
		 	sprintf(string_buf, "%2d %8s %3s %3s", 
				cur_id, currate_str, cur_str.primary, cur_str.last);

			string_buf[30] = 0;
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
			Dsp_Diffuse();
			if (j == 1)
				start_id = cur_id;
			if (j == line_ct)
				break;
		}
		
		cur_id++;	
	}
	if (cur_id >= end_num && j == 0) {
		error_display_caption(0xE00C);
		goto  LIST_START;
	}
	cur_id = start_id;
KEY_INPUT:
	while(!KEY_Read());
	BuzOn(1);
	switch(KeyDrv.CnvCode) {
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
			Flash_sread(addr, (HUGEDATA INT8U*)&cur_str, str_size); 
			if ((cur_str.rate>0) && (cur_str.rate<=9999999l)) {
			     goto READ_START;
			}
		}
	case KP_PAGE_UP :
		reverse_num = 0;
		while(1) {
			if(cur_id <= start_num) 
			{
				BuzOn(2);
				cur_id = start_num;
				goto READ_START;
			}
			cur_id--;
			addr =  addr_flash + (long)str_size * (cur_id-1);
			Flash_sread(addr, (HUGEDATA INT8U*)&cur_str, str_size); 
			if ((cur_str.rate>0) && (cur_str.rate<=9999999l)) {
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
			Flash_sread(addr, (HUGEDATA INT8U*)&cur_str, str_size); 
			if ((cur_str.rate>0) && (cur_str.rate<=9999999l)) {
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
			Flash_sread(addr, (HUGEDATA INT8U*)&cur_str, str_size); 
			if ((cur_str.rate>0) && (cur_str.rate<=9999999l)) {
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
		// tax_print_list(start_num, end_num);
		// ]
		currency_print_list(start_num, end_num);
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
