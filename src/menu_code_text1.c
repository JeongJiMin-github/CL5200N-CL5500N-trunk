/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	menu_code_text1.c
|*  Version			:	v 0.5
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created			:	2003/12/11
|*  Description		:	메뉴항목이 code와 text 1줄로 이루어진경우
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
#include "menu_code_text1.h"
#include "pgm_app.h"
#include "mode_error.h"
#include "traceability.h"
#include "nutrition_fact.h"
#ifdef USE_DIRECT_NUTRITION
#include "plu_edit.h"
#endif

long    menu_addr;
INT16S     menu_field_size;
INT16U  menu_capid1,menu_capid2;
INT16S     menu_call_y;
INT16S     menu_field_start;
INT8U   menu_readmode=0;
INT8U	menu_data_from_zero = 0;
extern INT16U ingredient_get_id(INT16U ingid);
extern INT16U ingredient_get_data(INT16U ingid,INT8U *data,INT16S size);

#define MENU1_1ST 1
#define MENU1_2ND 2

void    menu_callback(long v)
{
	long start_addr,Flash_addr;
	CAPTION_STRUCT 		cap;
	POINT disp_p;
	INT16U code;
	INT16S    disp_x;
	INT16S    disp_l;

	caption_split_by_code(menu_capid2, &cap,0);
	caption_adjust(&cap,NULL);
	disp_x = cap.input_x*Startup.menu_xgap;
	disp_l = cap.input_dlength;

	caption_split_by_code(menu_capid1, &cap,0);
	if ((v<cap.input_min) || (v>cap.input_max)) {
ERR:		memset(cap.form,0x20,30);
		goto DISP;
	}
	if (menu_readmode==1) {
		code=ingredient_get_id((INT16U)v);
	} else {
		start_addr =  DFLASH_BASE + menu_addr;
		Flash_addr = start_addr + (menu_field_size * (v-1));
		if (menu_readmode==2)
			code = (INT16U)Flash_bread(Flash_addr);
		else
			code = Flash_wread(Flash_addr);
	}
	if (menu_readmode==2) {
		if (!((code==1) || (code==2))) goto ERR;
	} else {
		if (code!=v) goto ERR;
	}
	if (menu_readmode==1) {
		ingredient_get_data((INT16U)code,(INT8U *)cap.form,disp_l);
		if (cap.form[0]==0) memset((char *)cap.form,0x20,disp_l);
	} else {
		Flash_addr += menu_field_start;
		Flash_sread(Flash_addr,(HUGEDATA INT8U *)cap.form,disp_l);
	}
DISP:
	disp_p=point(menu_call_y,disp_x);
	_strfill((INT8U *)cap.form, 0x20, disp_l);
	cap.form[disp_l] = 0;
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


void    menu_callback2(long num)
{
 	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT32U	addr;
	INT16U 	w;
	INT16U  read_code;
	INT8U id_menu_2nd;
	INT8U input_x;
	INT16U input_y;


	id_menu_2nd = Menu_GetMenuId(MENU1_2ND);
	input_x = Menu[id_menu_2nd].start;
	input_y = CallbackEditPosition_y + 1;

	disp_p = point(input_y*Startup.menu_ygap, input_x*Startup.menu_xgap);
	w = Menu[id_menu_2nd].w_length;
	memset((INT8U *)buffer, 0x20, w);
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

	caption_split_by_code(menu_capid1, &cap, 0);
	if (cap.input_max ==0) {
		message_error(disp_p);
		return;
	}
	if ((num < cap.input_min) || (num > cap.input_max)) {
		//memset(buffer, 0x20, w);
		return;
	}
	addr  = menu_field_size * (num - 1);
	addr += DFLASH_BASE + menu_addr;
	if (menu_readmode==2)
	{
		read_code = Flash_bread(addr);
	}
	else
	{
		read_code = Flash_wread(addr);
	}
	if(read_code == num)  {
		addr += menu_field_start;
		Flash_sread(addr, Menu[id_menu_2nd].pstr, Menu[id_menu_2nd].length);
	}
	else
	{	
		memset((HUGEDATA char*)Menu[id_menu_2nd].pstr, 0, Menu[id_menu_2nd].length);
	}
	if (w >	menu_field_size-menu_field_start) w = menu_field_size-menu_field_start;
	memcpy(buffer, Menu[id_menu_2nd].pstr, w);
	buffer[w] = 0;
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
}


void menu_callback_init(INT16U c1,INT16U c2,INT16S y,INT32U addr,INT16S fsize, INT16S fstart)
{
	menu_capid1=c1;
	menu_capid2=c2;
	menu_call_y=y;
	menu_addr  =addr;
	menu_field_size = fsize;
	menu_field_start= fstart;
	menu_readmode=0;
//	PutNSmallString(0,0,"ABC",3);
}

//void	menu1_create(INT16U code, INT16U *cap_code, INT32U _MENU_AREA, INT16U str_size)      			// command code  0x1271
void	menu1_create(INT16U code, INT16U *cap_code, INT32U _MENU_AREA, INT16U str_size, INT8U use_LF)      	//SG061226
{
	CAPTION_STRUCT 		cap;
	MENU_TYPE1_STRUCT	menu_type1_str;
	INT16S 	i;
 	INT32U	start_addr, flash_addr;//cap_addr, 
	INT16S 	result;
	INT32U	cur_id;
	INT8U   set_str[200];
	INT16U  properties;
	INT32U  id_max, id_min;
	
	properties = 0;
	if (use_LF) properties |= MENU_P_USE_LINEFEED;
#ifdef USE_EMART_NETWORK
	if(code != 0x1281)	//MENU 1281 : Edit PCS Unit.
	{
		properties |= MENU_P_CHECK_BLANKS;
	}
#else
	properties |= MENU_P_CHECK_BLANKS;
#endif

	start_addr = DFLASH_BASE + _MENU_AREA;
	caption_split_by_code(cap_code[0], &cap,0);
	id_max = cap.input_max;
	id_min = cap.input_min;
	while (1)
	{
		i = 0;	
		while(1)  
		{
			flash_addr = start_addr + (str_size * i);
			menu_type1_str.code = Flash_wread(flash_addr);
			if (menu_type1_str.code == i+1) {
				i++;
				if(i >= id_max)  {
					cur_id = id_min;
					break;
				}
			}  else  {
				cur_id = i + 1;
		       		break;
			}
		}//while(1)
		flash_addr = start_addr + (str_size * (cur_id - 1));
		Flash_sread(flash_addr, (HUGEDATA INT8U *)&menu_type1_str, str_size);
		if(menu_type1_str.code != cur_id)
			memset((INT8U*)&menu_type1_str, 0x00, str_size);
		menu_type1_str.code = cur_id;

		memcpy(set_str, menu_type1_str.text, str_size - 2);

		menu_callback_init(cap_code[0], cap_code[1],(1+Startup.menu_type)*Startup.menu_ygap,
			_MENU_AREA,str_size,2);

		Menu_Init();

		caption_split_by_code(code, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(cap_code[0], &cap,0);//""
		caption_adjust(&cap, NULL);
		Menu_InputCreate(MENU1_1ST, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							id_max, id_min, 0, &cur_id, NULL, NULL, menu_callback2);

		caption_split_by_code(cap_code[1], &cap,0);
#ifdef USE_GSMART_PLU
		if (cap_code[1] == 0x3282) cap.input_length = 5;	// Qty Symbol length (5chars*18ea) 
#else
		if (cap_code[1] == 0x3282) cap.input_length = 15;	// Qty Symbol length (15chars*8ea) 
#endif
		caption_adjust(&cap, NULL);
		Menu_InputCreate(MENU1_2ND, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, set_str, NULL, NULL);	
		Menu_SetProperties(MENU1_2ND, properties);

		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) 
		{
			menu_type1_str.code = cur_id;
			memcpy(menu_type1_str.text, set_str, str_size - 2);

			flash_addr = start_addr + (cur_id-1) * str_size;			
			Flash_swrite(flash_addr, (HUGEDATA INT8U *)&menu_type1_str, str_size);
		}
		else if (result == MENU_RET_ESC)
		{
			break;
		}
	}
}

void    menu1_delete(INT16U code, INT16U cap_code, INT32U _MENU_AREA, INT16U str_size)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
//	INT8U	menu_type1_str[100];
//	INT8U	menu_type1_str[150];
	INT8U ucTemp, nutri_type=0;

	INT32U	Flash_addr=0;//addr, 
	long 	ret_long=1;
	INT16S  	result, current_num,start_line;
#ifdef USE_PERSIA_DEFAULT
    INT32U value;
#endif
#ifdef USE_DIRECT_NUTRITION
	int ret, i;
	INT16U deptno;
	INT32U pluno, key;
#endif

	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)	
	nutri_type = ucTemp&0x0f; 
#ifdef COUNTRY_USA
	nutri_type = NUTRI_TYPE_2COL_USA;
#endif

	start_line = Startup.menu_type;			   //se-hyung 20071001

	Dsp_Fill_Buffer(0);	


	menu_display_menucode(CurMenuCode, 1);
	menu_displaytitle(code,0);   
	menu_writepage(1, 1);
    	
	caption_split_by_code(cap_code, &cap,0);
#if defined(USE_KOR_TRACE_999) || defined(USE_TRACE_1000QTY) //sewon 161219
	if (cap_code == 0x1af0)	// 개체인덱스
	{
		cap.input_max = MAX_INDIVIDUAL_NO - 1;
		cap.input_length = ilog(cap.input_max);
	}
#endif
#ifdef USE_DIRECT_NUTRITION
	NUTRIFACT_STRUCT4 nutrifact_str4;
	if(cap_code == 70) {
		deptno = status_scale.departmentid;
		pluno=1;
		ret = plu_input_department_only(&deptno, 0);
		if(ret==2)
			goto RET_ESC;
		if(ret==3) { //KP_SALE
			goto RET_ESC;
		}
		while(1) {
			key = plu_table_seq_search(deptno, &pluno, MAX_PLU_NUMBER);
			if(key == 0) {
				goto RET_ESC;
			}
			else {
				ret_long = pluno;
				break;
			}
		}

		cap.input_length = ilog(MAX_PLU_NUMBER);
		cap.input_dlength = cap.input_length;
	}
#endif
	caption_adjust(&cap,NULL);
#if defined(USE_SINGLE_LINE_GRAPHIC)
	display_clear(DISPLAY_UNITPRICE);
	display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);
#else
	disp_p = point(start_line*Startup.menu_ygap,0);
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
#endif
	Dsp_Diffuse();

#ifdef USE_DIRECT_NUTRITION
	if(cap_code == 70)
	{
		CallbackEditPosition_x = strlen((char *)cap.form);
		CallbackEditPosition_y = start_line;
		nutri_create_dept_no = deptno;
		keyin_setcallback(callback_nutrifact_description);
	}
#endif

REN1:	DspStruct.DirectDraw = 1;   
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
	result=keyin_ulong(KI_GRAPHIC, 
			   KI_FORM_ILNN,	
			   0,
			   (long*)&ret_long, 
			   cap.input_length, 
			   start_line*Startup.menu_ygap,  
			   cap.input_x*Startup.menu_xgap, 
			   0,
			   KI_NOPASS );
#else
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	result=keyin_ulong(KI_GRAPHIC,
			   KI_FORM_NORMAL,
			   0,
			   (long*)&ret_long,
			   cap.input_length,
			   start_line*Startup.menu_ygap,
			   cap.input_x*Startup.menu_xgap,
			   0,
			   KI_NOPASS );
#else
	result=keyin_ulong(KI_DEFAULT,
			   KI_FORM_NORMAL,	
			   0,
			   (long*)&ret_long, 
			   cap.input_length, 
			   start_line*Startup.menu_ygap,  
			   cap.input_x*Startup.menu_xgap, 
			   0,
			   KI_NOPASS );   
#endif
#endif
	switch (result) 
	    {	
		// <CODE> JHC, 040511, ADD
		// [
		// $1: JHC, 040528, Comments
		// case KP_SAVE:
		// ]
		case KP_ENTER:	
				if ((nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
				|| nutri_type == NUTRI_TYPE_6DIG_CZE)&& cap_code == 70) cap.input_max = MAX_NUTRIFACT2_NO;  
#ifdef USE_EU_NUTRITIONFACT
				else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT) cap.input_max = MAX_NUTRIFACT3_NO;
#endif
#ifdef USE_DIRECT_NUTRITION
				cap.input_max = MAX_PLU_NUMBER;
#endif
				if ((ret_long>cap.input_max) || (ret_long<cap.input_min)) 
				{  	BuzOn(2);	goto REN1;	}
				if (menu_data_from_zero)
				{
					current_num = ret_long;
				}
				else
				{
					current_num = ret_long - 1;
				}
				break;
		case KP_ESC:	goto RET_ESC; 
	     	default:	BuzOn(2);	
				goto REN1;		     	 	
	}

	//memset((INT8U*)&menu_type1_str, 0x00, str_size);
#ifdef USE_PERSIA_DEFAULT
    caption_split_by_code(0x9500,&cap,0);  //"Are you sure?(1=Y/0=N)" 데이터 삭제 전 확인 메세지 표시
    disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
    Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0);
    arab_codetemp[strlen(cap.form)] = 0;
    disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
    Dsp_Write_1stringLong(disp_p, arab_codetemp);

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
            goto    RET_ESC;
        default :
            BuzOn(2);
    }
#endif

#if defined(USE_TRACE_1000QTY) //sewon 161206
	if(code == 0x1373)
		Flash_addr =  EXT_MEMCARD_ADDR + _MENU_AREA;
	else
#elif defined(USE_10000_SCANNER_CONVTABLE)
	if(code == 0x1463)
		Flash_addr =  EXT_MEMCARD2_ADDR_X + _MENU_AREA;
	else
#elif defined(USE_DIRECT_NUTRITION)
	if(code == 0x1323)
		Flash_addr =  EXT_MEMCARD2_ADDR_X + _MENU_AREA;
	else
#endif
	Flash_addr =  DFLASH_BASE + _MENU_AREA;

#ifdef USE_DIRECT_NUTRITION
	if(cap_code == 70) {
		if(delete_plu_direct_nutrition(deptno, ret_long) < 0)
			goto RET_ESC;
	} else {
		Flash_addr += (INT32U)current_num * str_size;
		Flash_clear(Flash_addr, 0x00, str_size);
	}
#else
	Flash_addr += (INT32U)current_num * str_size;
	//Flash_swrite(Flash_addr, (HUGEDATA INT8U *)&menu_type1_str, str_size);
	Flash_clear(Flash_addr, 0x00, str_size);
#endif

RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
 	return;
}
