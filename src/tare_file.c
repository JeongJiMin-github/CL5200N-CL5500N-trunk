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
#include "mode_error.h"
#include "sale_report.h"

extern INT16S plucall_tareid_x;
void tare_selecttype(long num);
 
void    tarefile_delete(void)		// command code  0x1282
{	
	// <CODE> JHC, 040414, -change code
	// [
	//taretype_delete(0x1273, 0x3271, FLASH_TARE_AREA, TARE_STRUCT_SIZE);		
	taretype_delete(0x1273, 12, FLASH_TARE_AREA, TARE_STRUCT_SIZE);
	// ]
}

#ifndef DISABLE_LIST_MENU
void 	tarefile_list(void)		// command code  0x1283
{	
	// <CODE> JHC, 040414, -change code
	// [
	// $1: JHC, 040414, Modify, -0x1283 to 0x1273
	taretype_list(0x1272, 0x1E00, FLASH_TARE_AREA, TARE_STRUCT_SIZE);
	// ]
}
#endif

#define M_ID_TARE_NO		1
#define M_ID_TARE_DESCRIPTION	2
#define M_ID_TARE_WEIGHTUNIT	3
#define M_ID_TARE_TYPE		4
#define M_ID_TARE_VALUE		5

void callback_end_tare_weightunit(INT32U num)
{
	INT8U id_tare_value;

	id_tare_value = Menu_GetMenuId(M_ID_TARE_VALUE);
	if(id_tare_value == 0xff) return;

	Menu[id_tare_value].decimal = get_decimal_pt(3, (INT16U)num);
	Menu[id_tare_value].symbol = num + 1;

	ADM_WaitData(ADM_CMD_SET_W_UNIT_CUR, num, ADM_PARAM_ON);
	CurPluWeightUnit = num;

      	MenuRefresh = ON;
}

void callback_end_tare_type(INT32U num)
{
	INT8U id_tare_value;

	id_tare_value = Menu_GetMenuId(M_ID_TARE_VALUE);
	if(id_tare_value == 0xff) return;
	
	if (num == 1) 
	{
		Menu[id_tare_value].type = MENU_TYPE_NUM;
	}
	else if (num == 2)
	{
		Menu[id_tare_value].type = MENU_TYPE_WEIGHT;
	}
	else 
	{
		return;
	}

      	//MenuRefresh = ON;
}



void callback_end_tare_no(INT32U num)
{
	TARE_STRUCT tare_str;
	INT16U  strSize;
	INT32U	addr;
	INT8U id_tare_descr;
	INT8U id_tare_wunit;
	INT8U id_tare_type; 
	INT8U id_tare_value;

	id_tare_descr = Menu_GetMenuId(M_ID_TARE_DESCRIPTION);
	id_tare_wunit = Menu_GetMenuId(M_ID_TARE_WEIGHTUNIT);
	id_tare_type  = Menu_GetMenuId(M_ID_TARE_TYPE);
	id_tare_value = Menu_GetMenuId(M_ID_TARE_VALUE);

	
	strSize = TARE_STRUCT_SIZE;		      
	addr = DFLASH_BASE;
	addr += FLASH_TARE_AREA;
	addr += ((num-1) * (INT32U)strSize);

	Flash_sread(addr,(HUGEDATA INT8U *)&tare_str, strSize);
	if (tare_str._code != num) {
		memset((INT8U *)&tare_str, 0, strSize);
		tare_str._code = num;
		tare_str._weightunit = ADM_Status.UseUnit;
		tare_str._type = 1;
	}

 	memcpy(Menu[id_tare_descr].pstr, tare_str._description, Menu[id_tare_descr].length);
	if(id_tare_wunit != 0xff) *Menu[id_tare_wunit].pval = tare_str._weightunit;
	*Menu[id_tare_type].pval = tare_str._type;
	*Menu[id_tare_value].pval = tare_str._value;

      	MenuRefresh = ON;
}

INT8U callback_check_tarevalue(INT32U num)
{
	INT8U temp_value;
	INT8U tare_val_type;
	INT8U result;
	char string_buf[30];
	POINT disp_p;
	INT8U id_tare_value;
	INT8U backup_directdraw;

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
	id_tare_value = Menu_GetMenuId(M_ID_TARE_VALUE);
	//// Assign Flash Memeory. 3 bits
	//    0. Error
	//    1. No Round
	//    2. Round Off
	//    3. Round Down
	//    4. Round Up
	tare_val_type = get_global_bparam(GLOBAL_TAREINPUT_FLAG);

	temp_value = ad_check_tarevalue(num);
	if (temp_value == 0) {
		result = ON;
	} else if (temp_value == 2) {
		if (tare_val_type == 0) {	// error
			BuzOn(2);
			result = OFF;
		} else if (tare_val_type == 1) {	// No round
			result = ON;
		} else {
			*Menu[id_tare_value].pval = ad_cnv_weight_value(num, tare_val_type-1);
			//keyin_value(4, KI_GRAPHIC, KI_FORM_NORMAL, Menu[view_idx].decimal, (INT32U)Menu[view_idx].pval, Menu[view_idx].w_length, (i+start_line)*y_gab, Menu[view_idx].start*x_gab, 0, KI_PASS);
			disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, (CallbackEditPosition_x+2)*Startup.menu_xgap);	// 2digits : kg/lb symbol
			sprintf(string_buf, "Rounded Tare");
			Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
			Key_DelaySec(10, 0);
			result = ON;
		}
	} else {
		BuzOn(2);
		result = OFF;
	}

	DspStruct.DirectDraw = backup_directdraw;
	return result;
}

void	tarefile_create(void)
{
	CAPTION_STRUCT 	cap;	
	TARE_STRUCT 	tare_str;
 	INT8U	wei_dec;
	INT16S 	i, result=0;
 	INT32U	start_addr, flash_addr;
	INT8U	back_weight_unit;
	INT32U cur_id, type_no, tare_val, weightunit;
	INT8U  description[17];

	back_weight_unit = CurPluWeightUnit;

	start_addr = DFLASH_BASE + FLASH_TARE_AREA;
	wei_dec = get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);

	while(1)
	{	
		caption_split_by_code(12,&cap,0);
		i = 0;
		while (1)  
		{
			flash_addr = start_addr + TARE_STRUCT_SIZE * (INT32U)i;
			tare_str._code = Flash_wread(flash_addr);
			if (tare_str._code == i + 1) {
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

		flash_addr = start_addr + (TARE_STRUCT_SIZE * (cur_id - 1));
		Flash_sread(flash_addr, (HUGEDATA INT8U *)&tare_str, sizeof(tare_str));
		if(tare_str._code != cur_id)  {
			memset((INT8U*)&tare_str, 0x00, sizeof(tare_str));
			tare_str._code = cur_id;
			tare_str._weightunit = ADM_Status.UseUnit;
			tare_str._type = 1;
		}
		tare_str._code = cur_id;
		memcpy(description, tare_str._description, sizeof(tare_str._description));
		type_no = tare_str._type;
		tare_val = tare_str._value;
		weightunit = tare_str._weightunit;


		Menu_Init();

		caption_split_by_code(0x1271, &cap,0);//""
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(12, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TARE_NO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &cur_id, NULL, NULL, NULL);
		Menu_AddEndFunction(M_ID_TARE_NO, callback_end_tare_no);

		caption_split_by_code(0x3272, &cap,0);//"Description"
		cap.input_length = sizeof(tare_str._description)-1;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TARE_DESCRIPTION, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, description, NULL, NULL);
		Menu_SetProperties(M_ID_TARE_DESCRIPTION, MENU_P_CHECK_BLANKS);

	
		if(GlbOper.EnableKgLb) {
			caption_split_by_code(0x3276, &cap,0);//"Weight Unit(0=kg,1=lb):"
			caption_adjust(&cap,NULL);
			Menu_InputCreate(M_ID_TARE_WEIGHTUNIT, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, 0, &weightunit, NULL, NULL, NULL);
			Menu_AddEndFunction(M_ID_TARE_WEIGHTUNIT, callback_end_tare_weightunit);
		} else {
			weightunit = (INT32U)ADM_Status.UseUnit;
			if(weightunit == WEIGHT_UNIT_KG) {
				if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) weightunit = WEIGHT_UNIT_G;
			}
		}
		wei_dec = get_decimal_pt(3, (INT16U)weightunit);

		caption_split_by_code(0x3273, &cap,0);//"Tare Type"
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TARE_TYPE, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &type_no, NULL, NULL, tare_selecttype);
		Menu_AddEndFunction(M_ID_TARE_TYPE, callback_end_tare_type);

		caption_split_by_code(0x3274, &cap,0);//"Tare"
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TARE_VALUE, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, wei_dec, &tare_val, NULL, NULL, NULL);
		Menu_SetSymbol(M_ID_TARE_VALUE, weightunit+1);
		Menu_AddCheckFunction(M_ID_TARE_VALUE, callback_check_tarevalue);
		//MENU_TYPE_WEIGHT
						
		result = Menu_InputDisplay();
		if (result == MENU_RET_SAVE) 
		{
			tare_str._code = cur_id;
			memcpy(tare_str._description, description, sizeof(tare_str._description));
			tare_str._type = type_no;
			tare_str._value = tare_val;
			tare_str._weightunit = weightunit;

			flash_addr = start_addr + (TARE_STRUCT_SIZE * (cur_id - 1));
			Flash_swrite(flash_addr, (HUGEDATA INT8U *)&tare_str, sizeof(tare_str));
		}
		else if (result == MENU_RET_ESC)
		{
			break;
		}
	}
	if(GlbOper.EnableKgLb) {
		ADM_WaitData(ADM_CMD_SET_W_UNIT_CUR, back_weight_unit, ADM_PARAM_ON);	// Added by CJK 20060502
	}
	ADM_GetData.CurUnit = back_weight_unit;
	CurPluWeightUnit    = back_weight_unit;
}

void    taretype_delete(INT16U code, INT16U cap_code, INT32U _MENU_AREA, INT16U str_size)
{
	menu1_delete(code, cap_code, _MENU_AREA, str_size);
}

#ifndef DISABLE_LIST_MENU
void taretype_list(INT16U code, INT16U cap_code, INT32U _MENU_AREA, INT16U str_size)		
{		
	CAPTION_STRUCT 	cap;
	POINT 		disp_p;
	TARE_STRUCT 	tare_str;
	char   	string_buf[34], string_buf1[34], end_flag = 0;
	long 	ret_long, i;
	INT16S	start_num, end_num, cur_id, reverse_num;
	INT16S	j = 0, result, min_num, max_num;
	INT16S	lcd_size, line_ct, start_id,r;
	char    unit_str[6];
	INT8U   v8_1,v8_2;
	INT16U	read_code;
	INT32U	addr, addr_flash;	

LIST_START:
//	_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));		//
	//caption_split_by_code(0x3271,&cap,0);
	caption_split_by_code(12,&cap,0);
	min_num	= cap.input_min;					//
	max_num = cap.input_max; 					// 
	if(min_num <= 0)  {
//		sprintf(string_buf, "caption error");
		disp_p = point(0,0);
//		Dsp_Write_String(disp_p,(HUGEDATA char*)string_buf);
		message_error(disp_p);
	}

	Dsp_Fill_Buffer(0);
	//display_title_page(code);
	display_title_page_change(code,1,1);
	
//	addr = caption_search_by_code(cap_code);		
	for(i = 0; i<3; i++)  {
//		cap_addr = addr + CAP_MSG_LENGTH * i;
//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		caption_split_by_code(cap_code+i,&cap,0);
		//if(i != 2) caption_adjust_ref(&cap,NULL,0x3271);
		if(i != 2) caption_adjust_ref(&cap,NULL,12);
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
		caption_split_by_code(cap_code+i,&cap,0);
//		cap_addr = addr + CAP_MSG_LENGTH * i;
//		_memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));
		//caption_adjust_ref(&cap,NULL,0x3271);
		caption_adjust_ref(&cap,NULL,12);
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
						 
			case KP_ARROW_UP:  // <CODE> JHC, 040531, Modify
					   // [
					   if(i > 0)	i--;
					   else		BuzOn(2);
					   // ]
					   goto REN1;    					   
			// <CODE> JHC, 040511, Comment
			// case KP_PAGE_UP:
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
	addr_flash = DFLASH_BASE + _MENU_AREA;
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
			Flash_sread(addr, (HUGEDATA INT8U*)&tare_str, str_size ); 
//#ifdef USE_ARAB_FONT//list
//			v8_1=get_decimal_pt(3, (INT16U)tare_str._weightunit);
//			v8_2=get_global_bparam(GLOBAL_DECIMAL_SIGN);
//			//GetWeightSymbol(ADM_Status.UseUnit, unit_str);
//			GetWeightSymbol(tare_str._weightunit, unit_str);
//			format_value((HUGEDATA INT8U *)string_buf1,tare_str._value,v8_1,v8_2);
//			sprintf(string_buf,"%3d",tare_str._code);
//			string_buf[3]=0x20;
//
//			strcat(string_buf,unit_str);
//			r=strlen((char *)string_buf);
//			sprintf(&string_buf[r], " %s",tare_str._description);
//
//			//sprintf(string_buf,"%3d %6s%3s %.14s",tare_str._code,string_buf1,unit_str,tare_str._description);
//			sprintf(string_buf,"%3d %6s%3s",tare_str._code,string_buf1,unit_str);
//			string_buf[lcd_size] = 0;
//			
//			Convert_ArabPreview((INT8U *)tare_str._description, strlen(tare_str._description), 0, 0, 0);
//			arab_codetemp[25-14] = 0;
//			
//			disp_p.x += (25-1)*8;
//			Dsp_Write_1stringLong(disp_p, arab_codetemp);
//#else			             
			tare_str._description[12] = 0;
		 	if(tare_str._type !=3) {
				//v8_1=get_decimal_pt(3, (INT16U)ADM_Status.UseUnit);
				v8_1=get_decimal_pt(3, (INT16U)tare_str._weightunit);
				v8_2=get_global_bparam(GLOBAL_DECIMAL_SIGN);
				//GetWeightSymbol(ADM_Status.UseUnit, unit_str);
				GetWeightSymbol(tare_str._weightunit, unit_str);
				format_value((HUGEDATA INT8U *)string_buf1,tare_str._value,v8_1,v8_2);
				sprintf(string_buf,"%3d",tare_str._code);
				string_buf[3]=0x20;

				strcat(string_buf,unit_str);
				r=strlen((char *)string_buf);
				sprintf(&string_buf[r], " %s",tare_str._description);

				sprintf(string_buf,"%3d %6s%3s %.14s",tare_str._code,string_buf1,unit_str,tare_str._description);
			} 
			else  
			{
				sprintf(string_buf, 
					"%3d %6.2f%% %s", 
			                tare_str._code, 
			                (float)tare_str._value/100,
			                tare_str._description);
			}		
			string_buf[lcd_size] = 0;	       		
			
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);	       		 
#ifdef USE_ARAB_CONVERT
			Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0);
			arab_codetemp[strlen(string_buf)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
#endif
//#endif            
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
		tarefile_print_list(start_num, end_num);
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


//void discount_selecttype(INT16S num)
void tare_selecttype(long num)
{
	CAPTION_STRUCT cap;
	POINT disp_p;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT16U  w;

	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars();
#ifndef USE_ARAB_CONVERT // w < caption length 인경우 마지막 글자 잔상으로 남음. 
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
	Dsp_Write_String(disp_p, buffer);
#endif

	if(num < 1 || num > 2){
		if (Startup.menu_type==0) {
			//memset((INT8U*)cap.form,0x20,32);
			//cap.form[32] = 0;
		} else {
			//strcpy(buffer, global_message[MSG_INVALID_VALUE]);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0); 
			hebrew_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)global_message[MSG_INVALID_VALUE], strlen(global_message[MSG_INVALID_VALUE]), 0, 0, 0);
			arab_codetemp[strlen(global_message[MSG_INVALID_VALUE])] = 0;
            disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p, global_message[MSG_INVALID_VALUE]);
#endif
		}
	} else {
		caption_split_by_code(0x1B11 + num - 1,&cap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
        disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p, cap.form);
#endif
	}
}


//extern INT8U ad_check_tarelimit(long val);
//INT16U display_weight(INT8U dsp_mode, INT8U format, INT8U decimal, long *ret_val, INT8U leng, INT8U x, INT16U y, INT8U use_sec, INT8U pass)
INT16U weighing_tare_input(INT8U dsp_mode, INT8U decimal, long *ret_val, INT8U leng, INT8U y, INT16U x)
{
	long temp_val;
	char buffer[20];
	char temp_buffer[20];
	INT8U win_size, overlength, buf_size;
	POINT disp_p;
	INT8U i;
	INT8U backup_directdraw;
	INT16U l_space;
	INT8U  ch;
//CJK061012	INT16U zero_code;

//SG061009	zero_code = keyin_getrawcode_bytype(0, KS_ZERO);
//CJK061012	zero_code = keyin_getrawcode_bytype(1, KP_ZERO);

	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 1;
	//return
	overlength = 0;
	switch(dsp_mode) {
		case KI_TARE:
			win_size = display_parameter.mode_len[DISPLAY_TARE];
			break;
		case KI_WEIGHT:
			win_size = display_parameter.mode_len[DISPLAY_WEIGHT];
			break;
		case KI_UNIT :
			win_size = display_parameter.mode_len[DISPLAY_UNITPRICE];
			break;
		case KI_TOTAL :
			win_size = display_parameter.mode_len[DISPLAY_PRICE];
			break;
		case KI_GRAPHIC :
		default :
			win_size = leng;
			break;
	}
	l_space = 0;
	if(leng < win_size)
	{
		l_space = win_size - leng;
		if (dsp_mode == KI_GRAPHIC) l_space = 0; // graphic lcd에서는 왼쪽 띄어쓰기 필요없음
		win_size = leng;
	}
	if(decimal >= win_size) decimal = 0;	// 잘못된 deciaml일 경우 decimal을 0

	temp_val = 0;

	if(decimal) {
		overlength = 1;
		buf_size = win_size + 1;
	} else {
		overlength = 0;
		buf_size = win_size;
	}

	ADM_WaitData(ADM_CMD_SET_OUT_WT, 0, ADM_PARAM_OFF);	// Set Normalized Value(C1)
	ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);  	// Start Weight
	while(1) {
		while(!KEY_Read()) {
			if(ADM_RecvProc())
			{
				temp_val = ad_cnv_adm2main(ADM_GetData.RecvData);
				if(ADM_GetData.Overload || !ad_check_tarelimit(temp_val)) {
					for(i = 0; i < buf_size; i++) buffer[i] = '-';
				} else {
					format_fixed((HUGEDATA INT8U *)buffer, temp_val, decimal, buf_size, KeyInSet.char_decimal);
				}
				buffer[buf_size] = '\0';
				if (dsp_mode != KI_GRAPHIC)	overlength = 0; //7-Segment에서는 overlength 불필요
				temp_buffer[0] = 0;
				if (l_space+overlength) // 7-seg 용 dsp_mode에서 오른쪽 정렬이 필요할 때, 앞쪽에 띄어야하는 칸 수
				{
					for (i = 0; i < l_space+overlength; i++) temp_buffer[i] = 0x20;
					temp_buffer[i] = 0;
				}
				if (overlength)
				{
					for(i = 0; i < buf_size; i++)
					{
						ch = buffer[i+1];
						buffer[i] = ch;
					}		
					buffer[buf_size  - 1] = 0;
				}
				strcat(temp_buffer, buffer);
				switch(dsp_mode) {
					case KI_TARE:
						display_string(DISPLAY_TARE, (INT8U*)temp_buffer);
						VFD7_Diffuse();
						break;
					case KI_WEIGHT:
						display_string(DISPLAY_WEIGHT, (INT8U*)temp_buffer);
						VFD7_Diffuse();
						break;
					case KI_UNIT :
						display_string(DISPLAY_UNITPRICE, (INT8U*)temp_buffer);
						VFD7_Diffuse();
						break;
					case KI_TOTAL :
						display_string(DISPLAY_PRICE, (INT8U*)temp_buffer);
						VFD7_Diffuse();
						break;
					case KI_GRAPHIC :
					default :
						disp_p = point(y, x);
#ifdef USE_ARAB_CONVERT
						Convert_ArabPreview((INT8U *)buffer+overlength, strlen(buffer+overlength), 0, 0, 0);
						arab_codetemp[strlen(buffer+overlength)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p, buffer+overlength);
#endif
						break;
				}
			}
		}
		switch(KeyDrv.Type) {
			case KEY_TYPE_FUNCTION :
				switch(KeyDrv.CnvCode) {
					case KP_PAGE_UP :
						BuzOn(1);
						DspStruct.DirectDraw = backup_directdraw;
						return KeyDrv.CnvCode;
					//case KP_PAGE_DN :
					case KP_ARROW_DN :
						
					case KP_ARROW_UP :
						BuzOn(1);
						DspStruct.DirectDraw = backup_directdraw;
						return KeyDrv.CnvCode;
					//case KP_ARROW_RI:
					//case KP_ARROW_LF:
					case KP_ENTER :
					case KP_SAVE  :
						if(temp_val < 0) {
							BuzOn(2);
						} else {
							BuzOn(1);
							//cursor_clear(&cursor);
							*ret_val = temp_val;
							DspStruct.DirectDraw = backup_directdraw;
							return KeyDrv.CnvCode;
						}
						break;
					case KP_ESC :
						BuzOn(1);
						//cursor_clear(&cursor);
						return KeyDrv.CnvCode;
					case KP_CLEAR :
						if(ad_set_zero() == 0) BuzOn(1);
						else BuzOn(2);
						break;
//					case KP_BS:
//						BuzOn(1);
//						temp_val = temp_val/10;
//						*ret_val = temp_val;
//						break;
					case KP_ZERO :
						if(ad_set_zero() == 0) BuzOn(1);
						else BuzOn(2);
						break;
					default :
						BuzOn(2);
						break;
				}
				break;
			case KEY_TYPE_NUMERIC :
			default :
				BuzOn(2);
		}
	}
	//DspStruct.DirectDraw = backup_directdraw;
}


#define M_ID_TAREKEY_KNO	1
#define M_ID_TAREKEY_TNO	2

void callback_end_tarekey(INT32U num)
{
	INT32U addr_key;
	INT8U  id_tare_no;
	INT32U tare_no;

	id_tare_no = Menu_GetMenuId(M_ID_TAREKEY_TNO);

	addr_key = FLASH_KEY_CDTC_AREA + DFLASH_BASE + 128;
	addr_key += (num - 1) * 2;
	tare_no = Flash_wread(addr_key);

	if(id_tare_no != 0xff) *Menu[id_tare_no].pval = tare_no;

//      	MenuRefresh = ON;
}


void	tare_key_define(void)
{
	CAPTION_STRUCT cap;
	INT16S 	result;
	INT32U  addr_key;
	INT32U  tare_key_no, tare_no;
	INT32U  tare_key_max;

	tare_key_no = 1;
	tare_no = 1;

	while(1)
	{
		addr_key = FLASH_KEY_CDTC_AREA + DFLASH_BASE + 128;
		addr_key += (tare_key_no-1) * 2;
		tare_no = Flash_wread(addr_key);

		Menu_Init();

		caption_split_by_code(0x1274, &cap,0);//""
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x3275, &cap,0);//""
		tare_key_max = cap.input_max;
		if (tare_key_max > MAX_TARE_KEY) tare_key_max = MAX_TARE_KEY;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TAREKEY_KNO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &tare_key_no, NULL, NULL, NULL);
		Menu_AddEndFunction(M_ID_TAREKEY_KNO, callback_end_tarekey);
		Menu_SetKeyMode(M_ID_TAREKEY_KNO, 3);

		caption_split_by_code(12, &cap,0);//""
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_TAREKEY_TNO, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &tare_no, NULL, NULL, plucall_tareid);						

		result = Menu_InputDisplay();
		if (result == MENU_RET_SAVE) 
		{
			addr_key = FLASH_KEY_CDTC_AREA + DFLASH_BASE + 128;
			addr_key += (tare_key_no - 1) * 2;
			Flash_wwrite(addr_key, tare_no);
			tare_key_no++;
			if (tare_key_no > tare_key_max) tare_key_no = tare_key_max;
		}
		else if (result == MENU_RET_ESC)
		{
			break;
		}
	}
}

#ifdef USE_INDIA_TATA_DEFAULT
void tareTable_default_for_indiaTATA(void)
{
	INT8U i;
	TARE_STRUCT IndiaTATA_TareTabel[] = {
	//{ tareNo,	Description,		weightUnit,				Type,		Value }
		{ 1,		"L",				WEIGHT_UNIT_KG,			1,			24 }, // 0.024 kg
		{ 2,		"M",				WEIGHT_UNIT_KG,			1,			16 }, // 0.016 kg
		{ 3,		"S",				WEIGHT_UNIT_KG,			1,			12 }, // 0.012 kg
	};

	for( i=0; i<3; i++)
	{
		set_base_sparam(FLASH_TARE_AREA + (TARE_STRUCT_SIZE * (INT32U)i), (INT8U *)&IndiaTATA_TareTabel[i], sizeof(TARE_STRUCT));
	}
}
#endif