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

#include "adm.h"
#include "key.h"
#include "key_typedef.h"
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
#include "customer_app.h" 
#include "pgm_app.h"
#include "prt_cmd.h"
#include "prt_list.h"
#include "key_app.h"


void customer_data_init(CUSTOMER_STRUCT *custom)
{
	INT8U* paddr;
	INT16U i;

	paddr = (INT8U *)custom;
	for(i = 0; i < sizeof(CUSTOMER_STRUCT); i++) {
		*paddr++ = 0;
	}
}


void customer_read(INT16U code, CUSTOMER_STRUCT *custom)
{
	INT32U addr;
	// <CODE> JHC, 040512, Modify
	// [
	addr = DFLASH_BASE + FLASH_CUSTOMER_AREA;
	addr += ((INT32U)code*CUSTOMER_STRUCT_SIZE);
	// ]

	Flash_sread(addr, (HUGEDATA INT8U *)custom, sizeof(CUSTOMER_STRUCT));
}

void customer_save(CUSTOMER_STRUCT *custom)
{
	INT32U addr;
	addr = (DFLASH_BASE + FLASH_CUSTOMER_AREA);
	addr += (INT32U)custom->code*CUSTOMER_STRUCT_SIZE;
	Flash_swrite(addr, (HUGEDATA INT8U *)custom, sizeof(CUSTOMER_STRUCT));

}

void customer_delete(INT16U code)
{
	INT32U addr;

	if (code<0) return;
	addr = (DFLASH_BASE + FLASH_CUSTOMER_AREA);
	addr += (INT32U)code*CUSTOMER_STRUCT_SIZE;
					   
	Flash_clear(addr, 0xff, sizeof(CUSTOMER_STRUCT));
}
 
#ifndef DISABLE_LIST_MENU
void customer_list(void)
{	
	CUSTOMER_STRUCT custom;
	CAPTION_STRUCT 	cap;
	POINT 	disp_p;
	INT16U 	j, start_num, end_num, cur_id;
	INT16S 	str_size;
	char 	string_buf[50];
	INT8U 	result, lcd_size;
	INT32U 	addr_flash, addr, first_id, end_id;
	INT16U 	read_code, reverse_num, first_min, first_max, end_min, end_max;
	// <CODE> JHC, 040630, Modify
	// [
	INT16U  start_id, line_ct;
	// ]

	first_id = 1;
	end_id = MAX_CUSTOMER_NO;

LIST_START:
	Menu_Init();
	caption_split_by_code(0x1422, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x342A, &cap,0);	//No. From
	first_min = cap.input_min;
	first_max = cap.input_max;
	// ]
	caption_adjust(&cap,NULL);
	Menu_InputCreate(0, 
			 (HUGEDATA char *)cap.form, 
			 MENU_TYPE_NUM, 
			 cap.input_x, 
			 cap.input_dlength, 
			 cap.input_length, 
			 cap.input_max, 
			 cap.input_min, 
			 0, 
			 (HUGEDATA INT32U *)&first_id, 
			 NULL, NULL, NULL);

	caption_split_by_code(0x342B, &cap,0);	//No. To
	// <CODE> JHC, 040607, ADD
	// [
	end_min = cap.input_min;
	end_max = cap.input_max;
	// ]
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, 
			 (HUGEDATA char *)cap.form, 
			 MENU_TYPE_NUM, 
			 cap.input_x, 
			 cap.input_dlength, 
			 cap.input_length, 
			 cap.input_max, 
			 cap.input_min, 
			 0, 
			 (HUGEDATA INT32U *)&end_id, 
			 NULL, NULL, NULL);

	// <CODE> JHC, 040520, Modify
	// [
	Menu_return_last_item_arrow_dn = 1;
	result = Menu_InputDisplay();
	Menu_return_last_item_arrow_dn = 0;
	if(result == MENU_RET_SAVE ||
	   result == KP_ARROW_DN) {
		// <CODE> JHC, 040607, Modify
		// [
		if (first_min > (INT16U)first_id || first_max < (INT16U)first_id ||
		    end_min > (INT16U)end_id || end_max < (INT16U)end_id)
		{
			error_display_form1(0, (INT16U)first_id, (INT16U)end_id);
			goto LIST_START;
		}
		// ]
	} else if(result == MENU_RET_ESC) {
		return;
	}
	// ]

	lcd_size   = Dsp_GetChars();
	str_size   = CUSTOMER_STRUCT_SIZE;
	line_ct    = Startup.line_max; //(Startup.menu_type?3:2);
	addr_flash = DFLASH_BASE + FLASH_CUSTOMER_AREA;
	cur_id     = (INT16U)first_id;
	start_id   = (INT16U)first_id;
	start_num  = (INT16U)first_id;
	end_num	   = (INT16U)end_id;

READ_START:
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FF, &cap,0);
//		disp_p = point(0,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071002     no. reference(descript.) ÇÑÁÙ ¶ç¿ì±â
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
		addr =  addr_flash + (INT32U)str_size * (INT32U)cur_id;
		read_code = Flash_wread(addr);
		
		if(read_code == cur_id /*>= min_num && read_code <= max_num*/)  
		{	
			char priceBuf[20];
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&custom, str_size ); 
//		 	custom.id[8]=0;
#ifdef _USE_LCD20848_//ck
			FloatToString(TO_STR,(char *)priceBuf,9,2,(long *)&custom.creditlimit,0,1);
#else
			FloatToString(TO_STR,(char *)priceBuf,9,2,(long *)&custom.creditlimit,0,0);
#endif
			custom.name[8]=0;
			if (Startup.menu_type) 
			{
				sprintf(string_buf, 
					"%3d %s %s",
					custom.code,
					custom.name,
					priceBuf);
			} 
			else 
			{
				sprintf(string_buf, 
					"%s %s",
					custom.name,
					priceBuf);
			}
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
	case KP_ESC: return;
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
			addr =  addr_flash + (INT32U)str_size * (INT32U)(cur_id);
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
			addr =  addr_flash + (INT32U)str_size * (INT32U)(cur_id);			
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
			addr =  addr_flash + (INT32U)str_size * (INT32U)(cur_id);			
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
			addr =  addr_flash + (INT32U)str_size * (INT32U)(reverse_num);			
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
		customer_print_list(start_num, end_num);
		goto KEY_INPUT;	
	case KP_FEED:
		keyapp_feed(ON);
		goto KEY_INPUT;	
	default :	
		BuzOn(2);
		goto KEY_INPUT;
	}
}
#endif //#ifndef DISABLE_LIST_MENU


void customer_dataUpdate(INT32U num)	   //se-hyung 20071011
{
	CUSTOMER_STRUCT  custom_str;
	INT16U  strSize;
	INT32U	addr;
	INT16U  customerIndex;

	strSize = CUSTOMER_STRUCT_SIZE;//sizeof(custom_str);		      
	addr = DFLASH_BASE;
	addr += FLASH_CUSTOMER_AREA;
	addr += ((num) * (INT32U)strSize);
	customerIndex = Flash_wread(addr);
	if(customerIndex == num){
		Flash_sread(addr, (HUGEDATA INT8U *)&custom_str, strSize );
	} else {
		memset((INT8U*)&custom_str, 0x00, strSize);   
		custom_str.code = num;		
	}

	*Menu[0].pval = custom_str.code; 
 	memcpy(Menu[1].pstr, custom_str.name, Menu[1].length); 
   	memcpy(Menu[2].pstr, custom_str.address[0], Menu[2].length); 
  	memcpy(Menu[3].pstr, custom_str.address[1], Menu[3].length);
   	memcpy(Menu[4].pstr, custom_str.address[2], Menu[4].length); 
  	memcpy(Menu[5].pstr, custom_str.tel, Menu[5].length);
	*Menu[6].pval = custom_str.creditlimit;
      	MenuRefresh = ON;
}

void customer_edit(void)	 //se-hyung 20071011
{
	CUSTOMER_STRUCT  custom_str;
	CAPTION_STRUCT 	  cap;	
//	INT8U	max_item;
 	INT16U	str_size; 
	INT16U	result;
	INT8U	input_max;
	INT8U	i;
	INT8U 	address_str[3][50];
	INT8U 	name_str[32];
	INT8U 	tel_str[24];
		 
	INT32U	addr_start; 
	INT32U	addr_flash;
	INT32U set_val[2];

//	max_item = 7;
	set_val[0] = 1;

	str_size = CUSTOMER_STRUCT_SIZE;
	addr_start =  DFLASH_BASE + FLASH_CUSTOMER_AREA;
	// Get Last Number
	caption_split_by_code(0x3421,&cap,0);
	for (i=0; i<cap.input_max; i++) {
		addr_flash = addr_start + ((INT32U)str_size * (INT32U)i);
		set_val[0] = Flash_wread(addr_flash);
		//if( i != custom_str.code){
		if ((set_val[0]<cap.input_min) || (set_val[0]>cap.input_max)) {
			set_val[0]=i;
			break;
		}
	}
	if ((set_val[0]<cap.input_min) || (set_val[0]>cap.input_max)) set_val[0]=cap.input_min;

	while(1)
	{    
		str_size = CUSTOMER_STRUCT_SIZE;
		addr_start =  DFLASH_BASE + FLASH_CUSTOMER_AREA;
		addr_flash = addr_start + ((INT32U)str_size * (set_val[0]));
		Flash_sread(addr_flash, (HUGEDATA INT8U *)&custom_str, str_size); 

		if(custom_str.code != set_val[0])  {
			memset((INT8U*)&custom_str, 0x00, str_size);
		}
	       	custom_str.code = set_val[0];
		set_val[0] = custom_str.code;
		memcpy((HUGEDATA char*)name_str, (HUGEDATA char*)custom_str.name, sizeof(custom_str.name));   
		memcpy((HUGEDATA char*)address_str[0], (HUGEDATA char*)custom_str.address[0], sizeof(custom_str.address[0]));
		memcpy((HUGEDATA char*)address_str[1], (HUGEDATA char*)custom_str.address[1], sizeof(custom_str.address[1]));
		memcpy((HUGEDATA char*)address_str[2], (HUGEDATA char*)custom_str.address[2], sizeof(custom_str.address[2]));   
		memcpy((HUGEDATA char*)tel_str, (HUGEDATA char*)custom_str.tel, sizeof(custom_str.tel));
		set_val[1] = custom_str.creditlimit;
			
		Menu_Init();
		caption_split_by_code(0x1421, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x3421,&cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);

		input_max = cap.input_max;
 		Menu_AddEndFunction(1, customer_dataUpdate);
		
		caption_split_by_code(0x3423, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, name_str, NULL, NULL);

		caption_split_by_code(0x3424, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, address_str[0], NULL, NULL);

		caption_split_by_code(0x3425, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, address_str[1], NULL, NULL);
		
		caption_split_by_code(0x3426, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(5, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, address_str[2], NULL, NULL);

		caption_split_by_code(0x3427, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(6, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, tel_str, NULL, NULL);

		caption_split_by_code(0x3428, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(7, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 2, &set_val[1], NULL, NULL, NULL);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			addr_flash = addr_start + ((INT32U)str_size * (set_val[0]));
			memset((INT8U*)&custom_str, 0x00, str_size);
			
		    	custom_str.code = set_val[0];
		    	strcpy((HUGEDATA char*)custom_str.name, (HUGEDATA char*)name_str);
			strcpy((HUGEDATA char*)custom_str.address[0],(HUGEDATA char*)address_str[0]);   
			strcpy((HUGEDATA char*)custom_str.address[1], (HUGEDATA char*)address_str[1]);
			strcpy((HUGEDATA char*)custom_str.address[2],(HUGEDATA char*)address_str[2]);   
			strcpy((HUGEDATA char*)custom_str.tel, (HUGEDATA char*)tel_str);    
		    	custom_str.creditlimit = set_val[1];
			Flash_swrite(addr_flash, (HUGEDATA INT8U *)&custom_str, str_size);
		
			if(set_val[0] <input_max) set_val[0]++;
			else   set_val[0] =input_max; 
		}
		else if(result ==MENU_RET_ESC)
		{
			break;
		}
	}
}

INT16U customer_search_empty_code(void)
{
	CUSTOMER_STRUCT custom;
	INT16U cur_id;

	cur_id = 1;
	while(1){
		customer_read(cur_id, &custom);
		if(cur_id != custom.code) return cur_id;
		if(cur_id >= MAX_CUSTOMER_NO) return 0xffff;
		cur_id++;

	}
}


void customer_delete_proc(void)
{
	CAPTION_STRUCT cap;
	INT32U del_id = 1;
	INT8U result;

	Menu_Init();

	caption_split_by_code(0x1423, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x3421, &cap,0);	//User No.
	caption_adjust(&cap,NULL);
	Menu_InputCreate(0, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
					 cap.input_max, cap.input_min, 0, (HUGEDATA INT32U *)&del_id, NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		customer_delete(del_id);
	} else if(result == MENU_RET_ESC) {
	}
	return;
}