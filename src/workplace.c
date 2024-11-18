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
#include "workplace.h"
#include "menu_code_text1.h"
#include "country.h"
#include "pgm_app.h"
#include "prt_cmd.h"
#include "prt_list.h"

#ifdef USE_TRACEABILITY
extern void plucall_settrace(INT32U addr,INT16U caption,long rec,INT16S fstart,INT16S flength);
extern void plucall_trace(long num);
		
void	workplace_create(INT16U code, INT16U cap_code, INT32U _MENU_AREA, INT16U str_size);
#ifndef DISABLE_LIST_MENU
void 	workplace_list(INT16U cap_title, INT16U cap_input, INT16U cap_code, INT32U _menu_area, INT16U str_size);
#endif

void	slaughter_create(void)		// command code  0x137
{	workplace_create(0x1351, 0x135A, FLASH_SLAUGHTERHOUSE_AREA, SLAUGHTERHOUSE_STRUCT_SIZE);		}

void    slaughter_delete(void)		// command code  0x1372
{	menu1_delete(0x1353, 0x135A, FLASH_SLAUGHTERHOUSE_AREA, SLAUGHTERHOUSE_STRUCT_SIZE);		}

#ifndef DISABLE_LIST_MENU
void 	slaughter_list(void)		// command code  0x1373
{	workplace_list(0x1352, 0x135A, 0x1E00, 	 FLASH_SLAUGHTERHOUSE_AREA, SLAUGHTERHOUSE_STRUCT_SIZE);		}
#endif

void	cuttinghall_create(void)		// command code  0x1381
{	workplace_create(0x1361, 0x136A, FLASH_CUTTINGHALL_AREA, CUTTINGHALL_STRUCT_SIZE);		}

void    cuttinghall_delete(void)		// command code  0x1382
{	menu1_delete(0x1363, 0x136A, FLASH_CUTTINGHALL_AREA, CUTTINGHALL_STRUCT_SIZE);		}

#ifndef DISABLE_LIST_MENU
void 	cuttinghall_list(void)		// command code  0x1383
{	workplace_list(0x1362, 0x136A, 0x1E00,   FLASH_CUTTINGHALL_AREA, CUTTINGHALL_STRUCT_SIZE);		}
#endif


void callback_end_workplace_no(INT32U num)
{
	WORKPLACE_STRUCT workplace_str;
	INT16U  strSize;
	INT32U	addr;
	INT8U id_text;
	INT8U id_country_no;
	//INT16U cur_id;

	id_text = Menu_GetMenuId(2);
	id_country_no = Menu_GetMenuId(3);

	
	strSize = menu_field_size;		      
	addr = DFLASH_BASE;
	addr += menu_addr;
	addr += ((num-1) * (INT32U)strSize);

	Flash_sread(addr, (HUGEDATA INT8U *)&workplace_str, strSize);
	if (workplace_str.code != num) {
		memset((INT8U *)&workplace_str, 0, strSize);
	}
	workplace_str.code = num;

 	memcpy(Menu[id_text].pstr, workplace_str.text, Menu[id_text].length);
	*Menu[id_country_no].pval = workplace_str.country_number;

      	MenuRefresh = ON;
}

extern void callback_country_no(long num);
void	workplace_create(INT16U code, INT16U cap_code, INT32U _MENU_AREA, INT16U str_size)
{
	CAPTION_STRUCT cap;
	WORKPLACE_STRUCT workplace_str;
	INT16S 	i, result=0;
 	INT32U	start_addr, flash_addr;
	INT32U cur_id, country_no;
	INT8U  description[20];

	start_addr = DFLASH_BASE + _MENU_AREA;
	while(1)
	{	
		caption_split_by_code(cap_code, &cap, 0);
		i = 0;
		while (1)  
		{
			flash_addr = start_addr + (INT32U)str_size * (INT32U)i;
			workplace_str.code = Flash_wread(flash_addr);
			if (workplace_str.code == i + 1) {
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
		menu_callback_init(code, cap_code,(1+Startup.menu_type)*Startup.menu_ygap, _MENU_AREA, str_size, 2);

		flash_addr = start_addr + (str_size * (cur_id - 1));
		Flash_sread(flash_addr, (HUGEDATA INT8U *)&workplace_str, str_size);
		if(workplace_str.code != cur_id)  {
			memset((INT8U*)&workplace_str, 0x00, str_size);
		}
		workplace_str.code = cur_id;
		memcpy(description, workplace_str.text, sizeof(workplace_str.text));
		country_no = workplace_str.country_number;


		Menu_Init();

		caption_split_by_code(code, &cap,0);//""
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(cap_code, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &cur_id, NULL, NULL, NULL);
		Menu_AddEndFunction(1, callback_end_workplace_no);

		caption_split_by_code(cap_code+1, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, description, NULL, NULL);
		//Menu_SetProperties(2, MENU_P_CHECK_BLANKS);
	
		caption_split_by_code(cap_code+2, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &country_no, NULL, NULL, callback_country_no);
						
		result = Menu_InputDisplay();
		if (result == MENU_RET_SAVE) 
		{
			workplace_str.code = cur_id;
			memcpy(workplace_str.text, description, sizeof(workplace_str.text));
			workplace_str.country_number = country_no;

			flash_addr = start_addr + ((INT32U)str_size * (cur_id - 1));
			Flash_swrite(flash_addr, (HUGEDATA INT8U *)&workplace_str, str_size);
		}
		else if (result == MENU_RET_ESC)
		{
			break;
		}
	}
}

#ifndef DISABLE_LIST_MENU
void workplace_list(INT16U cap_title, INT16U cap_input, INT16U cap_code, INT32U _menu_area, INT16U str_size)
{
	CAPTION_STRUCT 	    cap;
	POINT 		    disp_p;
	WORKPLACE_STRUCT   workplace_str;

	
//	INT8U	y_gab=8;				// 폰트의 y 크기
//	INT8U	x_gab=6;				// 폰트의 x 크기

	char   	string_buf[64];//, end_flag = 0;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, reverse_num;
	INT16S	j = 0, result, min_num, max_num;
	INT16S	line_ct, start_id, lcd_size;

	INT16U	read_code;
	INT32U	addr, addr_flash, cap_addr;	

LIST_START:
//	cap_addr = caption_search_by_code(cap_input);			// Decide inpput boundary
//	_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));		//
	caption_split_by_code(cap_input,&cap,0);
	caption_adjust(&cap,NULL);
	min_num	= cap.input_min;					//
	max_num = cap.input_max; 					// 
	if(min_num <= 0)  {
//		sprintf(string_buf, "caption error");
		caption_split_by_code(0x00,&cap,0);
		disp_p = point(0,0);
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
		Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
#endif
	}
	Dsp_Fill_Buffer(0);
	//display_title_page(cap_title);
	display_title_page_change(cap_title,1,1);
	addr = caption_search_by_code(cap_code);
	for(i = 0; i<3; i++)  {
		cap_addr = addr + CAP_MSG_LENGTH * i;
//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
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
						 
			case KP_ARROW_UP:  
					// <CODE> JHC, 040512, Modify
					// [
					// $1: JHC, 040531, Modify
					if(i > 0)	i--;
					else		BuzOn(2);
					goto REN1;
					// ]
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
	addr_flash = DFLASH_BASE + _menu_area;
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
	// $1: JHC, 040621, Modify, -cap_title+7
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FF, &cap,0);
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
			Flash_sread(addr, (HUGEDATA INT8U*)&workplace_str, str_size );
			// <CODE> JHC, 040621, Modify
			// [ 
			workplace_str.text[16]=0;

		 	sprintf(string_buf, 
		 		"%3d.%3d,%.16s", 
				cur_id,
		 		workplace_str.country_number, 
		 		&workplace_str.text);
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
		workplace_table_print_list(cap_title, _menu_area, str_size, start_num, end_num);
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