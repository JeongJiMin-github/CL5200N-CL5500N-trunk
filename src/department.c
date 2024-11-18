/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	department.c
|*  Version			:	v 0.5
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created			:	2003/12/11
|*  Description		:	input department  data
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
#include "department.h"
#include "menu_code_text1.h"
#include "pgm_app.h"
#include "plu_edit.h"
#include "prt_cmd.h"
#include "prt_list.h"
#include "plu_device.h"


#ifndef USE_TRACE_MEATGRADE

void	group_create(void)		// command code  0x1221
{
//	INT16U cap_code[2]={0x122A,0x122B};
	INT16U cap_code[2]={9,0x3211};
	menu1_create(0x1221,cap_code,FLASH_SUB_DEPARTMENT_AREA, GROUP_STRUCT_SIZE, OFF);			}

void    group_delete(void)		// command code  0x1223
{
//	menu1_delete(0x1223, 0x122A, FLASH_SUB_DEPARTMENT_AREA, GROUP_STRUCT_SIZE);
	menu1_delete(0x1223, 9, FLASH_SUB_DEPARTMENT_AREA, GROUP_STRUCT_SIZE);
}

#ifndef DISABLE_LIST_MENU
void 	group_list(void)		// command code  0x1222
{
	INT16U input_cap[3]={0x1E00,0x1E01,0x1E02};
	display_pgm_list(0x1222, 9, input_cap, 0x71FF, FLASH_SUB_DEPARTMENT_AREA, GROUP_STRUCT_SIZE);
}
#endif

#endif

//extern INT32U plu_get_display_info(INT16U id, CAPTION_STRUCT *cap, INT8U x);
void department_dataUpdate(INT32U num)
{
	DEPARTMENT_STRUCT  dept_str;
	INT16U  strSize;
	INT32U	addr;

	strSize = sizeof(dept_str);		      
	addr = DFLASH_BASE;
	addr += FLASH_DEPARTMENT_AREA;
	addr += ((num-1) * (INT32U)strSize);

	Flash_sread(addr, (HUGEDATA INT8U *)&dept_str, strSize);
	if (dept_str._code != num) {
		memset((INT8U *)&dept_str, 0, strSize);
		dept_str._code = num;
		dept_str._speedkey = 1;
	}

	*Menu[0].pval = num; 	
 	memcpy(Menu[1].pstr, dept_str._description, sizeof(dept_str._description)); 
	*Menu[2].pval = dept_str._speedkey;

      	MenuRefresh = ON;
}

void	department_create(void)		// command code  0x1211
{
	CAPTION_STRUCT 		cap;	
	DEPARTMENT_STRUCT 	department_str;
//	INT16U caption_id[2]={0x3211,0x3161};
	INT16S 	result;
	INT16U	cur_id;
	INT32U	addr, flash_addr;

	INT32U  set_val1, set_val2;
	INT8U   set_str[30]; 

	flash_addr = DFLASH_BASE + FLASH_DEPARTMENT_AREA;

	while (1)
	{
		cur_id = search_pgm_next_id(1, DEPARTMENT_STRUCT_SIZE, flash_addr);
		if(cur_id) 
		{
			memset((INT8U*)&department_str, 0x00, sizeof(department_str));	
			department_str._code = cur_id;
			department_str._speedkey = 1;
		}  
		else 
		{
			error_display_caption(0xF035);
			Flash_sread(flash_addr, (HUGEDATA INT8U *)&department_str, sizeof(department_str));
			department_str._code = 1;				
		}
		//if (department_str._speedkey < 1 || department_str._speedkey > 5) department_str._speedkey = 1;
		set_val1 = department_str._code;
		set_val2 = department_str._speedkey;
		memcpy(set_str, department_str._description, sizeof(department_str._description));

		Menu_Init();

		caption_split_by_code(0x1211, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		//caption_split_by_code(0x3211, &cap,0);//""
		plu_get_display_info(PTYPE_DEPARTMENT_NO, &cap, 0);
		//caption_adjust(&cap, NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val1, NULL, NULL, NULL);//plucall_department);
		Menu_AddEndFunction(1, department_dataUpdate);
		Menu_SetCallbackMode(1, 0);

		caption_split_by_code(0x3211, &cap,0);
		caption_adjust(&cap, NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, set_str, NULL, NULL);	

		caption_split_by_code(0x3161, &cap,0);//""
		caption_adjust(&cap, NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val2, NULL, NULL, NULL);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			department_str._code = set_val1;
			department_str._speedkey = set_val2;
			memcpy(department_str._description, set_str, sizeof(department_str._description));

			addr = flash_addr + (set_val1-1) * DEPARTMENT_STRUCT_SIZE;			
			Flash_swrite(addr, (HUGEDATA INT8U *)&department_str, sizeof(department_str));

			if (status_scale.departmentid==department_str._code) {
				status_scale.cur_speedkeyid=department_str._speedkey;
			}
		}
		else if (result == MENU_RET_ESC)
		{
			break;
		}
	}
}


/*
extern INT32U plu_table_seq_search(INT16S deptno,INT32U *pluno,INT32U max_id);

INT8U 	IsUsingDeptNum(INT16U deptno)
{
	INT32U key;
	//unsigned char strTemp[256];
	long	plu_num[2];  
	plu_num[0]  = 1;
	plu_num[1]  = 99999;

    	key=plu_table_seq_search((INT16S)deptno,(INT32U *)&plu_num[0],(INT32U)plu_num[1]);

 	if(key){
		//SGDebug
		//sprintf(strTemp, "Department member Exist!! \r\n");
		//MsgOut(strTemp);
 		return 1;
	}
	else {
		//SGDebug
		//sprintf(strTemp, "Department Number Not Used !!\r\n");
		//MsgOut(strTemp);
		return 0;
	}
}
*/

void    department_delete(void)		// command code  0x1213
{
	INT16U deptno,result;
	INT32U addr;
	DEPARTMENT_STRUCT 	dept_str;
#ifdef USE_PERSIA_DEFAULT
	CAPTION_STRUCT      cap;
	POINT           disp_p;
#endif


	if (Startup.disable_dept) {
		error_display_caption(0xE402);
		return;
	}
PAGE_KI_IN:
	deptno = 1;

	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1213,1,1);
	result=plu_input_department_only(&deptno, 1);
	if (result==2) return;
	if (deptno==1) {
		BuzOn(2);
		error_display_caption(0x3213);
		goto PAGE_KI_IN;
	} else if (result == KP_ARROW_DN || result == KP_PAGE_DN || result == KP_SAVE) {
		BuzOn(2);
		 goto PAGE_KI_IN;
	}

	//SG.060213.
//	if( IsUsingDeptNum(deptno)){
//		BuzOn(2);
//		error_display_caption(0x3214);
//		return;
//	}
#ifdef USE_PERSIA_DEFAULT
    Dsp_Fill_Buffer(0);                     // LCD Buffer clear
    Dsp_Diffuse();
    caption_split_by_code(0x9500,&cap,0);
    disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
    Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0);
    arab_codetemp[strlen(cap.form)] = 0;
    disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
    Dsp_Write_1stringLong(disp_p, arab_codetemp);
    Dsp_Diffuse();
    
    while(!KEY_Read());
    switch(KeyDrv.CnvCode) {
        case KEY_NUM_1 :
        case KP_ASC_Y :
            BuzOn(1);
            break;
        case KEY_NUM_0 :
        case KP_ASC_N :
        case KP_ESC :
            BuzOn(1);
            return;
        default :
            BuzOn(2);
    }
#endif
	
	addr = deptno;
	addr --;
	addr *=DEPARTMENT_STRUCT_SIZE;
	addr+=DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	memset((INT8U *)&dept_str,0,sizeof(DEPARTMENT_STRUCT));
	Flash_swrite(addr, (HUGEDATA INT8U *)&dept_str, sizeof(dept_str));
}

#ifndef DISABLE_LIST_MENU
void 	department_list(void)		// command code  0x1212
{	
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	DEPARTMENT_STRUCT	dept_str;
	
	char   	string_buf[64], end_flag, reverse_num;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id;//, str_size;
	INT16S	j, result, min_num, max_num, lcd_size, line_ct, start_id;	
	INT16U  input_dlength;
	INT16U	read_code, title_cap, boundary_cap, input_cap;//, list_title_cap;
	INT32U	addr, addr_flash;

	if (Startup.disable_dept) {
		error_display_caption(0xE402);
		return;
	}
	title_cap = 0x1212;
	boundary_cap = 1;
	input_cap = 0x1E00;
//	list_title_cap = 0x1158;

	addr_flash = DFLASH_BASE + FLASH_DEPARTMENT_AREA;

	lcd_size = Dsp_GetChars();
	line_ct = Startup.line_max; // (Startup.menu_type?3:2);

	caption_split_by_code(boundary_cap, &cap,0);     			// Decide inpput boundary
	min_num	= cap.input_min;					//
	max_num = cap.input_max; 					// 
	input_dlength=cap.input_dlength;

LIST_START:
	end_flag=1;	
	
	Dsp_Fill_Buffer(0);
	//display_title_page(title_cap);
	display_title_page_change(title_cap,1,1);
			
	for(i = 0; i<Startup.line_max; i++)  {
		if (i >= 3) break;
		caption_split_by_code(input_cap+i, &cap,0);
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
REN1:		switch(i) {
			case 0:	ret_long = start_num;	
				break;
			case 1:	ret_long = end_num;
				break;
 		}

		caption_split_by_code(input_cap+i, &cap,0);
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
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
#endif
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_dlength, 
				   (i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
			
		switch (result){
			
			// <CODE> JHC, 040528, Modify
			// [
			case KP_ARROW_UP:  if(i > 0) 	i--;
					   else		BuzOn(2);		
					   goto REN1;
			// case KP_PAGE_UP:
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
	for(i=start_num; i<=end_num; i++) {					// There is no data 
		addr = addr_flash + (long)DEPARTMENT_STRUCT_SIZE * (i-1);			//
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
	// <CODE> JHC, 040528, Modify
	// [
	// $MEMO: -modify display list all
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FF, &cap,0);
		disp_p = point((Startup.menu_type-1)*Startup.menu_ygap,0);
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
		addr =  addr_flash + (long)DEPARTMENT_STRUCT_SIZE * (cur_id-1);			
		read_code = Flash_wread(addr);
		if(read_code == cur_id /*>= min_num && read_code <= max_num*/)  
		{	
			j++;
			Flash_sread(addr, (HUGEDATA INT8U*)&dept_str, DEPARTMENT_STRUCT_SIZE ); 

			sprintf(string_buf, "%2d %1d %s", dept_str._code, dept_str._speedkey, dept_str._description);
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
			addr =  addr_flash + (long)DEPARTMENT_STRUCT_SIZE * (cur_id-1);			
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
			addr =  addr_flash + (long)DEPARTMENT_STRUCT_SIZE * (cur_id-1);			
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
				// <CODE> JHC, 040529, Modify
				// [
				if (reverse_num < line_ct)
					cur_id = start_id;
				else
					cur_id = end_num;
				// ]
				goto READ_START;
			}
			else
				cur_id++;
			addr =  addr_flash + (long)DEPARTMENT_STRUCT_SIZE * (cur_id-1);			
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
		// <CODE> JHC, 040529, Modify
		// [
		reverse_num = cur_id;
		while(reverse_num <= end_num)  
		{
			reverse_num++;
			addr =  addr_flash + (long)DEPARTMENT_STRUCT_SIZE * (reverse_num-1);			
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
		// ]					
	case KP_TEST:
		department_print_list(start_num, end_num);
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

BOOLEAN	check_dept_exist(INT16U dept_no)
{
	INT16U	read_dept;
	INT32U	addr;


	addr =  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	addr = addr + DEPARTMENT_STRUCT_SIZE * (dept_no-1);
	read_dept = Flash_wread(addr);

	if(read_dept == dept_no) {
		return 1;
	} else {
		return 0;
	}
}

void set_department_speedkey(INT16U dept_no)
{
	DEPARTMENT_STRUCT  dept_str;
	INT16U  strSize;
	INT32U	addr;

	strSize = sizeof(dept_str);		      
	addr = DFLASH_BASE;
	addr += FLASH_DEPARTMENT_AREA;
	addr += ((dept_no-1) * (INT32U)strSize);

	Flash_sread(addr, (HUGEDATA INT8U *)&dept_str, strSize);

	if ((dept_str._speedkey>=1) && (dept_str._speedkey<=5))
		status_scale.cur_speedkeyid = dept_str._speedkey;
	else status_scale.cur_speedkeyid=1;
}
