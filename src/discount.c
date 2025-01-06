/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	discount.c
|*  Version		:	1.0
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created		:	2003/11/24
|*  Modified		:	2004/04/26 (bmlim)
|*  Description		:	Discount Rule    
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
#include "clerk_app.h"
#include "login.h"
#include "rtc.h"

#include "adm.h"
#include "main.h"
#include "dsp_vfd7.h"
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
#include "mode_program.h"
#include "mode_main.h"
#include "discount.h"
#include "ingredient.h"
#include "math.h"
#include "cursor.h"
#include "menu_code_text1.h"
#include "pgm_app.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "plu_edit.h"
#include "department.h"	
#include "mode_sale.h"
#include "network_command.h"
#include "prt_list.h"
#include "ethernet_comm.h"
#include "adm_app.h"
#include "plu_auxcall.h"
#include "mode_commun.h"

extern NETWORK_STATUS network_status;
INT8U temp_callback_discount_flag=0;
#ifdef USE_PLU_MULTIPLE_PRICE
extern DISC_SALE_TYPE 	DiscSale;
#endif

//extern INT16S 		plucall_department_position;
//extern INT16S		plucall_plu_position_x, plucall_plu_position_y;
extern INT16U		callback_dept_no;

INT8U	g_i8uInputDeptReturn = 0;
// Discount Type	0,      1,      2,      3,      4,      5,      6,      7,      8,      9, 	10,	   11
//INT16U weight_type[12] = { 0x0000, 0x1C97, 0x1CA0, 0x1C96, 0x1C99, 0x1C9E, 0x1C9F, 0x1C98, 0x1C93, 0x1C92, 0x1C95, 0x1C93};
INT16U	weight_type[14] = { 0x0000, 0x1C97, 0x1CA0, 0x1C96, 0x1C99, 0x1C9E, 0x1C9F, 0x1C98, 0x1C91, 0x1C95, 0x1C92, 0x1C93,0x1CA2,0x1CA4};
//INT16U pcs_type[12]    = { 0x0000, 0x1C9A, 0x1CA1, 0x1C9B, 0x1C9D, 0x1C9E, 0x1C9f, 0x1C9C, 0x1C93, 0x1C92, 0x1C95, 0x1C94};
INT16U	pcs_type[14]    = { 0x0000, 0x1C9B, 0x1CA1, 0x1C9A, 0x1C9D, 0x1C9E, 0x1C9f, 0x1C9C, 0x1C91, 0x1C95, 0x1C92, 0x1C94,0x1CA3,0x1CA5};
INT16U	max_dc_type = 13;

INT16U	discount_type_read(INT16U cap_code);
INT8U	dc_type1(DISCOUNT_STRUCT *dc_str, INT16U  caption_code, INT8U first_decimal);
INT8U	dc_type2(DISCOUNT_STRUCT *dc_str, INT8U view_line, INT8U input_line, INT16U  caption_code, INT8U first_decimal, INT8U second_decimal, INT8U plu_type);
//DELvoid 	discount_selecttype(INT16S num);
void 	input_day_format(INT8U day_type, char *str, INT8U year, INT8U month, INT8U date);
void 	input_time_format(char *str, INT16S hour, INT16S min, INT16S sec);
INT32U	get_dc_last_plu_no(INT16U dept_no);
//extern INT32U plu_get_display_info(INT16U id,CAPTION_STRUCT *cap,INT8U x);
extern  void  discount_delete_department(INT16U deptno);
extern  void  discount_delete_all(void);

/**
 * @brief discount 번호에 대한 플래시 시작 주소얻는 함수 
 * 
 * @param dc_no
 *        - discount 번호 (1번부터 시작함, index 아님)
 * @return INT32U
 *         - 성공 시 : discount 번호에 해당하는 Flash 시작 주소 
 *         - 실패 시 : 0
 * @remark
 *         - 주소 범위 체크하고 에러 처리할 것 
 *         - inline 처리 고려
 */
INT32U get_addr_discount_area(INT16U dc_no)
{
	INT32U offset; /* 인덱스 계산을 위한 오프셋 */
	INT32U base_addr = DFLASH_BASE;

	if (dc_no == 0 || dc_no > MAX_DISCOUNT_NO) {
		BuzOn(2);
		error_display_caption(0xF03D);
		return 0;
	}

#ifdef USE_DISCOUNT_QTY_EXT
	offset     = dc_no > DISCOUNT_QTY_2M ? dc_no - DISCOUNT_QTY_2M - 1  : dc_no - 1;
	base_addr += dc_no > DISCOUNT_QTY_2M ? FLASH_DISCOUNT_EXT_AREA_ADDR : FLASH_DISCOUNT_AREA;
#else
	offset     = dc_no - 1;
	base_addr += FLASH_DISCOUNT_AREA;
#endif /* USE_DISCOUNT_QTY_EXT */
	return (base_addr + offset * DISCOUNT_STRUCT_SIZE);
}

void discount_edit_item(INT8U departmentid,INT32U cur_pluid)
{
	DISCOUNT_STRUCT	 disc_str;
	INT8U	key_mode, plu_type,fixed_price;
	INT16U	dc_no, result;
	INT32U	addr;
	INT16U  plu_address;
	INT16S     usize;
	char    old_font;

	if (departmentid==0) return;
	if (cur_pluid==0) return;
//SG070315	if (cur_pluid>=100000) return;
	if (cur_pluid>MAX_PLU_NUMBER) return;
	if( !permission_check(PMS_KEY_EDIT_DISC,0)) return;
	
	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);
	
FUNCTION_IN:
	dc_no    = _get_dc_number(departmentid, cur_pluid);
	plu_type =plu_get_plu_type(departmentid, cur_pluid);
	plu_type&=0x03;
	if(dc_no) {
		addr = get_addr_discount_area(dc_no);
		if (!addr) { /* 에러 처리 */
			goto RET_ESC;
		}
		Flash_sread(addr, (HUGEDATA INT8U*)&disc_str, DISCOUNT_STRUCT_SIZE);
	}
//	plu_address = plu_table_search(departmentid,cur_pluid);
	plu_table_search(departmentid,cur_pluid,&plu_address,0);
 	plu_get_field(plu_address-1, PTYPE_FIXED_PRICE,&fixed_price,&usize,1);
	if (fixed_price) {
		error_display_caption(0xF03D);
		result = 0;
		goto RET_ESC;
	} else {
		result = input_discount_data(&disc_str, plu_type, 0);
	}
	switch(result) {
		case KP_ESC:
			goto RET_ESC;
		case KP_SAVE:
		case KP_ARROW_DN:
			goto RET_END;

		default :
		case KP_ARROW_UP:
			BuzOn(2);
			goto FUNCTION_IN;
	}

RET_END:
	disc_str.modified=0xffff;
	Flash_swrite(addr, (HUGEDATA INT8U *)&disc_str, DISCOUNT_STRUCT_SIZE);
	if (network_status.service_flag&0x20)	// send plu
	{
		//set_nvram_wparam(NVRAM_DC_SYNC_FLAG+2*(dc_no-1), 0xffff);
		commun_net_mask(1, &dc_no, 0xff, 1);
	}

RET_ESC:
//	Dsp_Fill_Buffer(0);						// LCD Buffer clear
//	Dsp_Diffuse();
	DspLoadFont1(old_font);

	KEY_SetMode(key_mode);
	sale_display_update(UPDATE_ALL|UPDATE_CLEARALL);
	return;
}

void discount_del_all(void)
{
	INT32U value;
	CAPTION_STRUCT cap;
	INT8U result;

	Menu_Init();
	caption_split_by_code(0x1139, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);
	       
	value=0;
	caption_split_by_code(0x9500, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &value, NULL, NULL, NULL);
	result = Menu_InputDisplay();

	if(result != MENU_RET_SAVE) goto RET_ESC;
	if (value) {
		discount_delete_all();
	}
RET_ESC:;
// -------------------
}

void    discount_delete_all(void)
{		
	INT32U flash_addr;
	INT16S sz;
	INT16U deptno;
	INT16U i;

	for (i=0; i<MAX_DISCOUNT_NO; i++) {
		flash_addr = get_addr_discount_area(i + 1);
		if (!flash_addr) { /* 에러 처리 */
			return;
		}
		deptno = Flash_wread(flash_addr);
		deptno |= 0x8000;
		Flash_wwrite(flash_addr, deptno);
		//Flash_clear(flash_addr,0,6);
	}

//	Flash_clear(flash_addr, 0, sz*MAX_DISCOUNT_NO);	// CJK080222
	if (network_status.service_flag&0x20)	// send plu
	{
		if (ethernet_list.status == 1)
		{
			//set_nvram_wparam(NVRAM_DC_SYNC_FLAG+2*i, 0xffff);
			//memset((void *)(NVRAM_BASE+NVRAM_DC_SYNC_FLAG), 0xff, 2*MAX_DISCOUNT_NO);
			commun_net_mask(1, NULL, 0xff, 99);
		}
	}
}

void discount_delete_byid(INT16S dc_no)
{
	INT32U addr;
	INT16U deptno;

	if (dc_no<=0) return;
	if (dc_no>=MAX_DISCOUNT_NO) return;
	addr = get_addr_discount_area(dc_no);
	if (!addr) { /* 에러 처리 */
		return;
	}
	//Flash_clear(addr, 0x00, DISCOUNT_STRUCT_SIZE);
	deptno = Flash_wread(addr);
	deptno |= 0x8000;
	Flash_wwrite(addr, deptno);
	if (network_status.service_flag&0x20)	// send plu
	{
		if (ethernet_list.status == 1)
		{
			//set_nvram_wparam(NVRAM_DC_SYNC_FLAG+2*(dc_no-1), 0xffff);
			commun_net_mask(1, (INT16U *)&dc_no, 0xff, 1);
		}
	}
}

void discount_del_by_department(void)		// function code : 1162
{	
	INT16U ret;
//	INT32U ret_long;
	INT16U deptno;

	if (Startup.disable_dept) return;
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	deptno=1;

PAGE_KI_IN:
	display_title_page_change(0x1138,1,1);
	g_i8uInputDeptReturn = 1;
	ret = plu_input_department_only(&deptno, 0);
	g_i8uInputDeptReturn = 0;
//	ret_long = (INT32U)deptno;
	if(ret==2) {
		goto RET_ESC;
	}
	else if (ret == KP_ARROW_DN ||  ret == KP_PAGE_DN) {
		goto PAGE_KI_IN;
	}
	if (deptno<1) {
		goto RET_ESC;
	}
	discount_delete_department(deptno);
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
	return;	
}

void	discount_delete(void)
{	
	INT16S	result;//, input_line=0, view_line=0;
	INT16U 	dept_no=status_scale.departmentid;
	INT32U	plu_no=1;
PAGE1_KI_IN:
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1137,1,1);
	g_i8uInputDeptReturn = 1;
	result=(INT16S)plu_input_department(&(dept_no), &(plu_no), 1);
	g_i8uInputDeptReturn = 0;

	if (result==2) goto RET_ESC;
	else if (result==KP_ARROW_DN) goto PAGE1_KI_IN;

	discount_delete_byplu(dept_no,plu_no);
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
 	return;
}

void    discount_delete_byplu(INT16U deptno,INT32U pluno)
{
	INT16S dc_no;
	dc_no = _get_dc_number(deptno, pluno);
	discount_delete_byid(dc_no);	
}

void    discount_delete_department(INT16U deptno)
{
	INT32U addr;
	INT16S    i;
	INT16U d_no;

	for (i = 1; i <= MAX_DISCOUNT_NO; i++) {
		//Flash_sread(addr, (HUGEDATA INT8U*)&disc_str, DISCOUNT_STRUCT_SIZE);
		addr = get_addr_discount_area(i);
		if (!addr) { /* 에러 처리 */
			return;
		}
		
		d_no = Flash_wread(addr);
		if (d_no==deptno) {
			//memset(&disc_str,0,sizeof(disc_str));
			//Flash_swrite(addr, (HUGEDATA INT8U*)&disc_str, DISCOUNT_STRUCT_SIZE);
			d_no |= 0x8000;
			Flash_wwrite(addr, d_no);
			if (network_status.service_flag&0x20)	// send plu
			{
				if (ethernet_list.status == 1)
				{
					//commun_plu_net_mask((INT16U *)&key, 0xff, 1);
					set_nvram_wparam(NVRAM_DC_SYNC_FLAG+2*(i-1), 0xffff);
					commun_net_mask(1, (INT16U *)&i, 0xff, 1);
				}
			}
		}
	}
}



void discount_delete_global(void)
{
	INT32U	addr;

	addr = DFLASH_BASE + FLASH_DISCOUNT_GLOBAL_WEIGHT;
	Flash_clear(addr, 0x00, DISCOUNT_STRUCT_SIZE);
	addr = DFLASH_BASE + FLASH_DISCOUNT_GLOBAL_COUNT;
	Flash_clear(addr, 0x00, DISCOUNT_STRUCT_SIZE);
	addr = DFLASH_BASE + FLASH_DISCOUNT_GLOBAL_NONFOOD;
	Flash_clear(addr, 0x00, DISCOUNT_STRUCT_SIZE);
}

void discount_selecttype(long num)
{
	CAPTION_STRUCT cap;
	POINT disp_p;
	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	memset((INT8U*)&cap.form, 0x20, sizeof(cap.form));
	cap.form[31]=0;
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
//	if(num==0) return;

//	addr = caption_search_by_code(0x1B51);
	if(input_range_check(num, 1, max_dc_type)) {
		if(num==12)
		{
			caption_split_by_code(0x1B51, &cap,0);
			strcat((char *)cap.form, "+");
		}
		else if(num==13)
		{
			caption_split_by_code(0x1B56, &cap,0);
			strcat((char *)cap.form, "+");
		}
		else
		{
			caption_split_by_code(0x1B50+num, &cap,0);
		}
	} else {
		strcpy(cap.form,global_message[MSG_INVALID_VALUE]);
	}
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
}

#ifndef DISABLE_LIST_MENU
void	discount_list(void)
{
	DISCOUNT_STRUCT	discount_str;
	CAPTION_STRUCT 	cap;

	POINT   disp_p;
	INT8U   lcd_size;
	char	buffer[64];
	long 	ret_long, 
		i, 
		plu_num[2];		// start_num = plu_num[0], end_num=plu_num[1];	
	INT16S 	result, 
		dept_no=status_scale.departmentid, 
		cur_id,
		line_ct;
//	INT8U	end_flag, 
    INT8U	input_line, m;
	INT16U	dc_no, 		data_pos[MAX_DISCOUNT_NO];
//		input_cap, 
//		list_title_cap, 
//		data_pos[MAX_DISCOUNT_NO];
	INT32U addr_flash;
	INT16U cap_id[3]={0x3151,0x3152,0x1e02};
//	input_cap = 0x1C10;
	// <CODE> JHC, 040414, -Code change
	// [
	// $1: JHC, 040414, Modify, -0x1239 to 0x1139
//	list_title_cap = 0x1132;
	// ]

	lcd_size = Dsp_GetChars();
		
LIST_START:
	// <CODE> JHC, 040623, Modify
	// [
	// JHC, 040630, Modify, -plu_num[1]
	plu_num[0]  = 1;
//SG070315   plu_num[1]  = 99999;
	plu_num[1]  = MAX_PLU_NUMBER;
//	end_flag=1;
	// ]

PAGE1_VIEW:
	// <CODE> JHC, 040414, -Code change
	// [
	// $1: JHC, 040414, Modify, -0x1232 to 0x1132
	// $2: JHC, 040611, Modify, -data type
	// $3: JHC, 040624, Modify, -display title
	// display_title_page_change(0x1132, 1, 2);
	//display_title_page(0x1132);
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1132,1,2);
	result=plu_input_department_only((INT16U*)&dept_no, 0);
	// ]

	if (result==2) goto RET_ESC;

PAGE2_VIEW:	
	Dsp_Fill_Buffer(0);

	// <CODE> JHC, 040624, Modify, -display title
	// [
	// display_title_page_change(0x1132, 2, 2);
	//display_title_page(0x1132);
	display_title_page_change(0x1132,2,2);
	// ]
			
	for(i = 0; i<3; i++)  {	
		caption_split_by_code(cap_id[i], &cap,0);
		if (i!=2) caption_adjust(&cap,NULL);
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

	input_line = 0;
PAGE2_KI_IN:
	for(i=input_line; i<2; i++)  
	{
		ret_long = plu_num[i];
		caption_split_by_code(0x3151+i, &cap,0);
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
		if(result == KP_ESC)
			goto RET_ESC;
		if ( ret_long > cap.input_max || ret_long < cap.input_min ) {
			BuzOn(2);
			error_display_form1(0,cap.input_min,cap.input_max);
			goto PAGE2_KI_IN;	
		}
		plu_num[i] = ret_long;

		switch (result){
						 
			case KP_ARROW_UP:  
				if(i == 0)
					goto PAGE1_VIEW;
				input_line = 0;
				goto PAGE2_VIEW;

			case KP_PAGE_UP:
				goto PAGE1_VIEW;

			case KP_SAVE:	
				i = 2;
				break;

			case KP_ENTER:
			case KP_ARROW_DN:	   
				break;
					   				  				
		     	default:	
		     		BuzOn(2);	goto PAGE2_KI_IN;		     	 	
		} //switch (result)
	} //for(i=1; i<3; i++)
	ret_long=get_dc_last_plu_no(dept_no);
	if(ret_long <= plu_num[1])
		plu_num[1] = ret_long;

	cur_id 	= 0;
	line_ct = Startup.line_max; // (Startup.menu_type?3:2);
	dc_no	= _get_dc_number_EX(data_pos, dept_no, plu_num[0], plu_num[1]);
	if (!dc_no) {
		error_display_caption(0xE00C);
		goto  LIST_START;
	}
READ_START:
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type)
	{
		//caption_split_by_code(list_title_cap, &cap,0);
		caption_split_by_code(0x71FF, &cap,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 2 * Startup.menu_xgap);
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

	for (i = 0; i < line_ct; i++)
	{
		if (cur_id+i >= dc_no) break;

		addr_flash = get_addr_discount_area(data_pos[cur_id+i]);
		if (!addr_flash) { /* 에러 처리 */
			break;
		}
		Flash_sread(addr_flash,(HUGEDATA INT8U*)&discount_str, DISCOUNT_STRUCT_SIZE);
		for(m=0; m<=max_dc_type; m++) 
		{
			if( discount_str.type == weight_type[m] )
				break;
			if( discount_str.type == pcs_type[m] )
				break;
		}
		caption_split_by_code(0x1B50+m, &cap,0);
		sprintf(buffer, "%5ld %s", discount_str.plu_no, &cap.form[3]);
		buffer[lcd_size] = 0;

		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap, 0);
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
		Dsp_Write_String(disp_p,(HUGEDATA char *)&buffer);
#endif
	}
	Dsp_Diffuse();

KEY_INPUT:
	while(!KEY_Read());
	BuzOn(1);
	
	switch(KeyDrv.CnvCode) 
	{
	case KP_ESC:
		goto RET_ESC;
	case KP_ARROW_UP:
		if (cur_id > 0)
		{
			cur_id--;
			goto READ_START;
		}
		else
		{
			BuzOn(2);
			goto KEY_INPUT;
		}
	case KP_PAGE_UP :
		if (cur_id == 0)
		{
			BuzOn(2);
			goto KEY_INPUT;
		}
		else
		{
			if (cur_id >= line_ct)
			{
				cur_id -= line_ct;
				goto READ_START;
			}
			else
			{
				cur_id = 0;
				BuzOn(2);
				goto READ_START;
			}
		}
	case KP_PAGE_DN :
		if ((cur_id+line_ct) < dc_no)
		{
			cur_id += line_ct;
			goto READ_START;
		}
		else
		{
			BuzOn(2);
			error_display_caption(0xE00E);
			goto KEY_INPUT;
		}
	case KP_ENTER:		
	case KP_ARROW_DN:
		if (cur_id < dc_no-1)
		{
			cur_id++;
			goto READ_START;
		}
		else
		{
			BuzOn(2);
			error_display_caption(0xE00E); 
			goto KEY_INPUT;
		}
	case KP_TEST:
		discount_print_list(dept_no, plu_num[0], plu_num[1], data_pos, dc_no);
		goto KEY_INPUT;	
	case KP_FEED:
		keyapp_feed(OFF);
		goto KEY_INPUT;	
	default :	
		BuzOn(2);
		goto KEY_INPUT;
	}
RET_ESC:
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
}
#endif //#ifndef DISABLE_LIST_MENU


void callback_discount_priority(long num)
{
 	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char	buffer[MAX_MENU_CHAR_BUF];
	INT16U 	w;

	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
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
	Convert_ArabPreview((INT8U *)buffer, strlen(buffer), 0, 0, 0); 
	arab_codetemp[strlen(buffer)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);
#endif

	if(num <0 ||num>3) {
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
		caption_split_by_code((0x1B21+num), &cap,0);	  
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

void input_day_format(INT8U day_type, char *str, INT8U year, INT8U month, INT8U date) 
{
	INT8U date_sep;
	
#if defined(USE_SINGLE_LINE_GRAPHIC)
	date_sep = '.'; //7-seg display용
#else
	date_sep = get_global_bparam(GLOBAL_DATE_SEPARATOR);
//	time_sep = get_global_bparam(GLOBAL_TIME_SEPARATOR);
#endif

	if (year>99) year=0;
	if (month>12) month=0;
	if (date>31) date=0;
	switch(day_type) {
		default:
		case 0: 	sprintf(str,"%02d%c%02d%c%02d", year,date_sep, month,date_sep, date );
				str[10] =0;
				return;

		case 1: 	sprintf(str,"%02d%c%02d%c%02d", month,date_sep, date,date_sep, year );
				str[10] =0;
				return;
		case 2:
		case 3: 	sprintf(str,"%02d%c%02d%c%02d", date,date_sep, month,date_sep, year );
				str[10] =0;
				return;
	}
}

void input_time_format(char *str, INT16S hour, INT16S min, INT16S sec) 
{
	INT8U time_sep;
	
#if defined(USE_SINGLE_LINE_GRAPHIC)
	time_sep = '.'; //7-seg display용
#else
//	date_sep = get_global_bparam(GLOBAL_DATE_SEPARATOR);
	time_sep = get_global_bparam(GLOBAL_TIME_SEPARATOR);
#endif
       	sprintf((char *)str,"%02d%c%02d%c%02d", hour,time_sep, min,time_sep, sec );
}


INT16U _get_dc_number(INT16U dept_no, INT32U plu_no)
{	
	INT32U 	flash_addr;
	INT16U	dc_no,disc_no;

	disc_no=0;
	if (!(status_scale.restrict&FUNC_DISCOUNT)) goto IGNORE;
   	for(dc_no=0; dc_no<MAX_DISCOUNT_NO; dc_no++) {
		flash_addr = get_addr_discount_area(dc_no + 1);
		if (!flash_addr) { /* 에러 처리 */
			goto IGNORE;
		}
		if (dept_no == Flash_wread(flash_addr)) {
			if(plu_no == Flash_lread(flash_addr+2)) 
			{
				disc_no=dc_no+1;
				break;
			}
		} 

	}
IGNORE:
	return disc_no;
}
/*
INT16U _get_dc_number_net(INT16S scaleid,INT16U dept_no, INT32U plu_no,INT8U *ret)
{	
	INT32U 	addr_origin, flash_addr;
	INT16U	dc_no,disc_no;
	INT16U  xmask,wflag;
	disc_no=0;
	if (!(status_scale.restrict&FUNC_DISCOUNT)) goto IGNORE;
	addr_origin =  DFLASH_BASE + FLASH_DISCOUNT_AREA;
	wflag=0;
   	for(dc_no=0; dc_no<MAX_DISCOUNT_NO; dc_no++) {
		flash_addr = addr_origin + DISCOUNT_STRUCT_SIZE*dc_no;
		if (dept_no == Flash_wread(flash_addr)) {
			if (plu_no == Flash_lread(flash_addr+2)) {
				wflag=Flash_wread(flash_addr+6);
				disc_no=dc_no+1;
				break;
			}
		} 

	}
	if (disc_no) {
		scaleid=scaleid-1;
		xmask = 0x8000;
		xmask >>= scaleid;
		*ret=0;
		if ((xmask&wflag)) *ret=1;
	}
IGNORE:
	return disc_no;
}
*/

INT16U _get_dc_number_net(INT8U scaleListId, INT16U dept_no, INT32U plu_no, INT8U *ret)
{	
	INT32U 	flash_addr;
	INT16U	dc_no,disc_no;
//	INT16U  wflag;
	disc_no=0;
	if (!(status_scale.restrict&FUNC_DISCOUNT)) goto IGNORE;
//	wflag=0;
   	for(dc_no=0; dc_no<MAX_DISCOUNT_NO; dc_no++) {
		flash_addr = get_addr_discount_area(dc_no + 1);
		if (!flash_addr) { /* 에러 처리 */
			goto IGNORE;
		}
		if (dept_no == Flash_wread(flash_addr)) {
			if (plu_no == Flash_lread(flash_addr+2)) {
				//wflag=Flash_wread(flash_addr+6);
				//wflag = get_nvram_wparam(NVRAM_DC_SYNC_FLAG+2*dc_no);
				disc_no=dc_no+1;
				break;
			}
		} 

	}
	if (disc_no) {
#ifdef USE_DISCOUNT_QTY_EXT
		if (disc_no > DISCOUNT_QTY_2M) {
			goto IGNORE;
		}
#endif /* USE_DISCOUNT_QTY_EXT */
		//xmask = 0x0001;
		//xmask <<= (scaleListId - 1);
		*ret=0;
		if (scaleListId == 0 || scaleListId > MAX_SLAVE_SCALE) *ret=1;	// 해당id(IP)가 리스트상에 없으면 전송
		//if ((xmask&wflag)) *ret=1;	// dc가 변경되었으면 전송
		if (commun_net_mask(1, &disc_no, scaleListId, 10)) *ret = 1;
	}
IGNORE:
	return disc_no;
}

// <FUNC> JHC, 040611, ADD, -_get_dc_number_EX()
// [
// $Arguments	: INT16U* ret_arr   	- return variable of SAVE DATA POSITION
//		  INT16U  dept_no	- select department number
// 		  INT32U plu_min	- plu minimum
//		  INT32U plu_max	- plu maximum
// $Return 	: INT16U            	- SAVED DATA COUNT
// ]
INT16U _get_dc_number_EX(INT16U* ret_arr, INT16U dept_no, long plu_min, long plu_max)
{	
	INT32U flash_addr,
		plu_temp,
		plu_num[MAX_DISCOUNT_NO];
	INT16U	i16u_i,
		i16u_j,
		i16u_rtn,
		ret_temp, 
		dc_no;

	for (i16u_i = 0; i16u_i < MAX_DISCOUNT_NO; i16u_i++)
		ret_arr[i16u_i] = 0;

	i16u_rtn = 0;

   	for(dc_no=0; dc_no<MAX_DISCOUNT_NO; dc_no++) 
   	{
		flash_addr = get_addr_discount_area(dc_no + 1);
		if (!flash_addr) { /* 에러 처리 */
			break;
		}

		if (dept_no == Flash_wread(flash_addr)) 
		{
			plu_temp = Flash_lread(flash_addr+2);
			// <CODE> JHC, 040614, Modify
			// [
			if (plu_temp >= plu_min && plu_temp <= plu_max && plu_temp != 0)
			{
				ret_arr[i16u_rtn] = dc_no + 1;
				plu_num[i16u_rtn] = plu_temp;
				i16u_rtn++;
			}
			// ]
		} 
	}

	plu_temp = 0;
	for (i16u_i = 0; i16u_i < i16u_rtn-1; i16u_i++)
	{
		for (i16u_j = i16u_i+1; i16u_j < i16u_rtn; i16u_j++)
		{
			if (plu_num[i16u_i] > plu_num[i16u_j])
			{
				plu_temp = plu_num[i16u_i];
				plu_num[i16u_i] = plu_num[i16u_j];
				plu_num[i16u_j] = plu_temp;

				ret_temp = ret_arr[i16u_i];
				ret_arr[i16u_i] = ret_arr[i16u_j];
				ret_arr[i16u_j] = ret_temp;				
			}
		}
	}

	return i16u_rtn;
}

/**
 * @brief 
 * 
 * @return INT16U 
 *         - discount 영역에서 비어있는 영역의 번호
 *         - ex) discount index 0번 영역 -> 1 return
 *         - 즉, discount no는 1번부터 시작
 */
INT16U _get_new_dc_number(void)
{
	CAPTION_STRUCT	cap;

	INT32U 	flash_addr;
	INT16U	dept_no, dc_no,disc_no;

//	caption_split_by_code(0x121A, &cap,0);
	plu_get_display_info(PTYPE_DEPARTMENT_NO,&cap,0);
	disc_no=MAX_DISCOUNT_NO-1;

	/* 여기서 dc_no : index */
   	for(dc_no=0; dc_no<MAX_DISCOUNT_NO; dc_no++) {
		flash_addr = get_addr_discount_area(dc_no + 1);
		if (!flash_addr) { /* 에러 처리 */
			break;
		}
		dept_no = Flash_wread(flash_addr);
		/* 비어있는 discount 영역인지를 판단하기 위해 앞의 2바이트(Department 번호)를 기준으로 판별 */
		/* 지울 때 0x8000로 지움 */
		if( !input_range_check((long)dept_no, cap.input_min, cap.input_max) ) {
			// Not available data = New data
			disc_no = dc_no+1;
			break;
		}
	}
	return disc_no;
}

INT8U discount_check_dc_number(INT16U dc_no, INT16U *dept_no, INT32U *plu_no)
{
	INT32U 	flash_addr;

	flash_addr = get_addr_discount_area(dc_no);
	if (!flash_addr) { /* 에러 처리 */
		return OFF;
	}
	*dept_no = Flash_wread(flash_addr);
	*plu_no = Flash_lread(flash_addr+2);
	if(*dept_no < MAX_DEPT_NO && *dept_no > 0) 
	{
		if(*plu_no <= MAX_PLU_NUMBER && *plu_no > 0) 
		{
			return ON;
		}
	}
	return OFF;
}

INT32U	get_dc_last_plu_no(INT16U dept_no)
{

	INT32U 	flash_addr, plu_no=0, temp_no;
	INT16U	dc_no;

   	for(dc_no=0; dc_no<MAX_DISCOUNT_NO; dc_no++) {
//		cur_dept_no = Flash_wread(flash_addr);
//		if(cur_dept_no<cap.input_min || cur_dept_no>cap.input_max) {
//			continue;
//		}
		flash_addr = get_addr_discount_area(dc_no+1);
		if (!flash_addr) { /* 에러 처리 */
			break;
		}

		if(dept_no == Flash_wread(flash_addr)) {
			//plu_no = Flash_lread(flash_addr+2);
			temp_no = Flash_lread(flash_addr+2);
			if(temp_no > plu_no) plu_no = temp_no;
		}
	}
	if(plu_no == 0) {
		plu_no = 0xffffffff;
	}
	return plu_no;
}

INT8U	get_dc_any_plu_no(INT16U dept_no)
{

	INT32U 	flash_addr;
	INT16U	dc_no,temp_no;

   	for(dc_no=0; dc_no<MAX_DISCOUNT_NO; dc_no++) {
		flash_addr = get_addr_discount_area(dc_no+1);
		if (!flash_addr) { /* 에러 처리 */
			break;
		}
		temp_no=Flash_wread(flash_addr);
		if (dept_no) {
			if (temp_no==dept_no) return 1;
		} else {
			if ((temp_no>0) && (temp_no<100)) return 1;
		}
	}
	return 0;
}

//ret_long=plu_get_last_id(dept_no)-1;

void discount_create(void)
{
	DISCOUNT_STRUCT	disc_str;
//	CAPTION_STRUCT cap;
//	POINT 	disp_p;
//	INT8U	y_gab, x_gab; 	// 폰트의 x, y 크기
//	char 	form_string[34], daytype_str[15];
//	char	ret_string[32];
	INT8U	plu_type,fixed_price;
//	INT8U   input_line;
	INT16U  plu_address;
	INT16S   	result,usize;
 	INT16U	dc_no;
		
// 	long 	ret_long=0;
//	char	page_view[10];
   	INT32U	flash_addr;
	INT8U wunit_id, new_wunit_id, backup_weightunit;
//	INT8U   nPage=0;
//	INT8U   ucTemp;
//	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
//	nPage = (ucTemp>>4)&0x01; //use FSP  SG061211
	INT16U dept_no;
	INT32U plu_no;

DISCOUNT_START:
	dc_no = _get_new_dc_number();
	flash_addr = get_addr_discount_area(dc_no);
	
	if (dc_no>=MAX_DISCOUNT_NO)
	{	// data full
		error_display_caption(0xF035);
		flash_addr = get_addr_discount_area(1);
		if (!flash_addr) { /* 에러 처리 */
			goto RET_ESC;
		}
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&disc_str, DISCOUNT_STRUCT_SIZE); 
	} 
	else 
	{
		memset((INT8U*)&disc_str,0x00,DISCOUNT_STRUCT_SIZE);
		disc_str.dept_no = status_scale.departmentid;
		disc_str.plu_no = 1;
		disc_str.day_of_week = 0xff;
	}
//	input_line=0;
PAGE1_VIEW:
	// <CODE> JHC, 040701, ADD
	// [
//@@	if (Startup.menu_type==0) {
//@@		display_clear(2);
//@@		sprintf(page_view,"01-07");
//@@		display_string(2,(INT8U *)page_view);
//@@		// Dsp_Diffuse();
//@@		VFD7_Diffuse();
//@@	}
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1131,1,7);
	// packed된 structure member 를 pointer로 전달시 오류 방어(CL5500에서는 _PACKED_DEF)
	dept_no = disc_str.dept_no;
	plu_no = disc_str.plu_no;
	result=(INT16S)plu_input_department((INT16U *)&dept_no, (INT32U *)&plu_no, 0);
	disc_str.dept_no = dept_no;
	disc_str.plu_no = plu_no;
// <CODE> JHC, 040610, Modify
//	switch(result) {
//		case 2 : 	goto RET_ESC;
//		case KP_SAVE: 	goto RET_ESC;
//	}
	if (result==2) goto RET_ESC;

//	plu_address = plu_table_search((INT8U)disc_str.dept_no,disc_str.plu_no);
	plu_table_search((INT8U)disc_str.dept_no,disc_str.plu_no,&plu_address,0);
	if (!plu_address) {
		BuzOn(2);
		goto PAGE1_VIEW;
	}
 	plu_get_field(plu_address-1, PTYPE_FIXED_PRICE,&fixed_price,&usize,1);
	if (fixed_price) {
		error_display_caption(0xF03D);
		result = 0;
		goto PAGE1_VIEW;
	}

	plu_type=plu_get_plu_type(disc_str.dept_no, disc_str.plu_no);

	if(plu_type<1 || plu_type>3) {
		error_display_caption(0xF039);
		goto RET_ESC;
	}
	dc_no = _get_dc_number(disc_str.dept_no, disc_str.plu_no);
	if(dc_no) {   // edit_dc data
		flash_addr = get_addr_discount_area(dc_no);
		if (!flash_addr) { /* 에러 처리 */
			goto RET_ESC;
		}
		Flash_sread(flash_addr, (HUGEDATA INT8U*)&disc_str, DISCOUNT_STRUCT_SIZE);
	} else {
//		memset((INT8U*)&disc_str,0x00,DISCOUNT_STRUCT_SIZE);
	}

	backup_weightunit = CurPluWeightUnit;
	plu_get_field(plu_address-1, PTYPE_UNIT_WEIGHT,(INT8U *)&wunit_id, &usize, sizeof(wunit_id));	// UnitWeight
	get_weightunit_and_id_fromid(wunit_id, &new_wunit_id, &CurPluWeightUnit);
	result = input_discount_data(&disc_str, plu_type, 1);
	CurPluWeightUnit = backup_weightunit;	// 복귀
	switch(result) {
		case  KP_ESC:
			goto RET_ESC;
		case  KP_ARROW_DN:
		case  KP_SAVE:
			goto RET_END;
		case  KP_ARROW_UP:
//			input_line = 0;
			goto PAGE1_VIEW;
	}
RET_END:
	disc_str.modified=0xffff;
	Flash_swrite(flash_addr, (HUGEDATA INT8U *)&disc_str, DISCOUNT_STRUCT_SIZE);
	if (network_status.service_flag&0x20)	// send plu
	{
		dc_no = _get_dc_number(disc_str.dept_no, disc_str.plu_no);
		if (dc_no)
		{
			//set_nvram_wparam(NVRAM_DC_SYNC_FLAG+2*(dc_no-1), 0xffff);
			commun_net_mask(1, &dc_no, 0xff, 1);
		}
	}
	goto DISCOUNT_START;
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
 	return;
}

INT8U get_dc_typeno(INT16U dc_type)
{
	INT8U i;

	for (i = 0; i <= max_dc_type; i++) 
	{
 		if(dc_type == weight_type[i])
			break;
		if(dc_type == pcs_type[i])
			break;
	}
	return i;
}

void get_dc_input_type(INT16U dc_type, INT8U plu_type, INT8U *target_type, INT8U *value_type)
{
	switch(plu_type) {
		case PLU_BY_WEIGHT: // 1: weight, 2: price, 3: count(PCS), 4: Percent(%)
			*target_type = 1;
			break;
		case PLU_BY_COUNT :
		case PLU_BY_PCS   : // 1: weight, 2: price, 3: count(PCS), 4: Percent(%)
			*target_type = 3;
			break;
		default:
			*target_type = 0;
			break;
	}
	switch(dc_type)
	{
		case 0x1C91:	//8-1. 8-2. Total price by Total price
		case 0x1C92:	//10-1. 10-2. unit price by total price
		case 0x1C93:	//11-1. Weight by total price
		case 0x1C94:	//11-2. Quantity by total price
		case 0x1C95:	//9-1. 9-2. %Total price by total price
			// 1: weight, 2: price, 3: count(PCS), 4: Percent(%)
			*target_type = 2;
			break;
	}

	*value_type = 2;	// default : price
	switch(dc_type) {
		case 0x1C91:	//8-1. 8-2. Total price by Total price
		case 0x1C92:	//10-1. 10-2. unit price by total price
		case 0x1C97:	//1-1. Unit price by weight
		case 0x1C9A:	//1-2. Unit price by quantity
		case 0x1CA2:	//12-1. Unit price by weight+
		case 0x1CA3:	//12-2. Unit price by quantity+		
		case 0x1C96:	//3-1. Total price by weight
		case 0x1C9B:	//3-2. Total price by quantity
			// 1: weight, 2: price, 3: count(PCS), 4: Percent(%)
			*value_type = 2;
			break;
		case 0x1C95:	//9-1. 9-2. %Total price by total price
		case 0x1CA0:	//2-1. %Unit price by quantity
		case 0x1CA1:	//2-2. %Unit price by quantity
		case 0x1C99:	//4-1. %total price by weight
		case 0x1C9D:	//4-2. %total price by quantity
			// 1: weight, 2: price, 3: count(PCS), 4: Percent(%)
			*value_type = 4;
			break;
		case 0x1C93:	//11-1. Weight by total price
		case 0x1C98:	//7-1. Free addition by  weight
			// 1: weight, 2: price, 3: count(PCS), 4: Percent(%)
			*value_type = 1;
			break;
		case 0x1C94:	//11-2. Quantity by total price
		case 0x1C9C:	//7-2. Free addition by quantity
			// 1: weight, 2: price, 3: count(PCS), 4: Percent(%)
			*value_type = 3;
			break;
		case 0x1C9E:	//5-1, 5-2. Fixed Price
			// 1: weight, 2: price, 3: count(PCS), 4: Percent(%)
			*value_type = 2;
			break;
		case 0x1C9F:	//6-1, 6-2, 6-3 Free Item
			// 1: weight, 2: price, 3: count(PCS), 4: Percent(%)
			*value_type = 0;
			break;
		case 0x1CA4:	//13-1. N + 1 by weight
		case 0x1CA5:	//13-2. N + 1 by quantity
			*target_type = 3;		
			*value_type = 2;
			break;
			
	}
}


void get_dc_input_condition(INT8U dc_type, INT32U *input_max, INT32U *input_min, INT16U *length, INT8U *dec, INT8U *sym)
{
	INT8U	wei_dec, pri_dec;

	wei_dec = get_decimal_pt(6, 0);
	pri_dec  = get_global_bparam(GLOBAL_AREA_PRICE_DECIMALPOINT);

	*input_min = 0;
	switch(dc_type) 
	{
		default:
		case 0: // default
//SYM			memset(&cap,0,sizeof(cap));
			*sym = 0;
			*length = 1;
			*input_max = 0;
			*dec = 0;
			break;
		case 1: // weight
//SYM			caption_split_by_code(0x2035, &cap,0);
//SYM			sprintf(temp[i], " %s", (char*)cap.form);
			*sym = CurPluWeightUnit+1;	//1:kg, 2:lb, 3:g, 11:$, 21:pcs, 31:%
			*length = get_global_bparam(GLOBAL_DISPLAY_WEIGHTLENGTH);
			if (wei_dec) (*length)++;
			*input_max = ad_get_capa();	// Modified by CJK 20051027
			*dec = wei_dec;
			break;
		case 2: // price
//SYM			caption_split_by_code(0x2036, &cap,0);
//SYM			sprintf(temp[i], " %s", (char*)cap.form);
			*sym = 11;	//1:kg, 2:lb, 3:g, 11:$, 21:pcs, 31:%
			*length = get_global_bparam(GLOBAL_DISPLAY_UNITPRICELENGTH);
			if (pri_dec) (*length)++;
			*input_max = power10(*length)-1;
			*dec = pri_dec;
			break;
		case 3: // count(PCS)
//SYM			caption_split_by_code(0x2038, &cap,0);
//SYM			sprintf(temp[i], " %s", (char*)cap.form);
			*sym = 21;	//1:kg, 2:lb, 3:g, 11:$, 21:pcs, 31:%
			*length = get_global_bparam(GLOBAL_INPUT_BC_PCSLENGTH);;
			*input_max = power10(*length)-1;
			*dec = 0;
			break;
		case 4: // percent(%)
//SYM			caption_split_by_code(0x2037, &cap,0);
//SYM			sprintf(temp[i], " %s", (char*)cap.form);
			*sym = 31;	//1:kg, 2:lb, 3:g, 11:$, 21:pcs, 31:%
			*length = 5;
			*input_max = 9999;
			*dec = 2;
			break;
	}
}

//DEL071119void callback_end_dc_checkdate(void)
//DEL071119{
//DEL071119	
//DEL071119		//if( input_day_check(dc_str->start_year, dc_str->start_month, dc_str->start_day) ) {
//DEL071119		//if( input_day_check(dc_str->end_year, dc_str->end_month, dc_str->end_day) ) {
//DEL071119
//DEL071119		//if( !input_time_check(starttime_str,2) )  
//DEL071119		//if( !input_time_check(endtime_str,2) )
//DEL071119}
//DEL071119
//DEL071119void callback_end_dc_checktime(void)
//DEL071119{
//DEL071119	
//DEL071119		//if( input_day_check(dc_str->start_year, dc_str->start_month, dc_str->start_day) ) {
//DEL071119		//if( input_day_check(dc_str->end_year, dc_str->end_month, dc_str->end_day) ) {
//DEL071119
//DEL071119		//if( !input_time_check(starttime_str,2) )  
//DEL071119		//if( !input_time_check(endtime_str,2) )
//DEL071119}

INT8U Menu_DiscPluType;
void callback_end_disc_type(INT32U dc_typenum)
{
	INT8U	target_type;
	INT8U	target_dec;
	INT32U	target_input_max;
	INT32U	target_input_min;
	INT16U	target_length;
	INT8U   target_sym;
	INT8U	value_type;
	INT8U	value_dec;
	INT32U	value_input_max;
	INT32U	value_input_min;
	INT16U	value_length;
	INT8U   value_sym;

	INT16U  dc_type;

	INT16U	id_fix_v;
	INT16U  id_1st_t;
	INT16U  id_1st_v;
	INT16U  id_2nd_t;
	INT16U  id_2nd_v;
	
	switch(Menu_DiscPluType) {
		case PLU_BY_WEIGHT: dc_type = weight_type[dc_typenum]; break;
		case PLU_BY_COUNT :
		case PLU_BY_PCS   : dc_type = pcs_type[dc_typenum]; break;
	}
	get_dc_input_type(dc_type, Menu_DiscPluType, &target_type, &value_type);
	get_dc_input_condition(target_type, &target_input_max, &target_input_min, &target_length, &target_dec, &target_sym);
	get_dc_input_condition(value_type, &value_input_max, &value_input_min, &value_length, &value_dec, &value_sym);	

	id_fix_v = Menu_GetMenuId(4);
	id_1st_t = Menu_GetMenuId(5);
	id_1st_v = Menu_GetMenuId(6);
	id_2nd_t = Menu_GetMenuId(7);
	id_2nd_v = Menu_GetMenuId(8);

	if (id_fix_v != 0xff)
	{
		Menu[id_fix_v].decimal = value_dec;
		Menu[id_fix_v].length = value_length;
		Menu[id_fix_v].max = value_input_max;
		Menu[id_fix_v].min = value_input_min;
		Menu[id_fix_v].symbol = value_sym;
		Menu[id_fix_v].w_length = value_length;
		if (dc_type == 0x1C9E)	//5-1, 5-2. Fixed Price
		{
			Menu[id_fix_v].inhibit = OFF;
		}
		else             
		{
			Menu[id_fix_v].inhibit = ON;
		}
	}

	if (id_1st_t != 0xff)
	{
		Menu[id_1st_t].decimal = target_dec;
		Menu[id_1st_t].length = target_length;
		Menu[id_1st_t].max = target_input_max;
		Menu[id_1st_t].min = target_input_min;
		Menu[id_1st_t].symbol = target_sym;
		Menu[id_1st_t].w_length = target_length;
		if (dc_type == 0x1C9E || dc_type == 0x1C9F)	//5-1, 5-2. Fixed Price,	6-1, 6-2, 6-3 Free Item
		{
			Menu[id_1st_t].inhibit = ON;
		}
		else             
		{
			Menu[id_1st_t].inhibit = OFF;
		}
	}

	if (id_1st_v != 0xff)
	{
		Menu[id_1st_v].decimal = value_dec;
		Menu[id_1st_v].length = value_length;
		Menu[id_1st_v].max = value_input_max;
		Menu[id_1st_v].min = value_input_min;
		Menu[id_1st_v].symbol = value_sym;
		Menu[id_1st_v].w_length = value_length;
		if (dc_type == 0x1C9E || dc_type == 0x1C9F)	//5-1, 5-2. Fixed Price,	6-1, 6-2, 6-3 Free Item
		{
			Menu[id_1st_v].inhibit = ON;
		}
		else             
		{
			Menu[id_1st_v].inhibit = OFF;
		}
	}

	if (id_2nd_t != 0xff)
	{
		Menu[id_2nd_t].decimal = target_dec;
		Menu[id_2nd_t].length = target_length;
		Menu[id_2nd_t].max = target_input_max;
		Menu[id_2nd_t].min = target_input_min;
		Menu[id_2nd_t].symbol = target_sym;
		Menu[id_2nd_t].w_length = target_length;
		if (dc_type == 0x1C9E || dc_type == 0x1C9F)	//5-1, 5-2. Fixed Price,	6-1, 6-2, 6-3 Free Item
		{
			Menu[id_2nd_t].inhibit = ON;
		}
		else             
		{
			Menu[id_2nd_t].inhibit = OFF;
		}
	}

	if (id_2nd_v != 0xff)
	{
		Menu[id_2nd_v].decimal = value_dec;
		Menu[id_2nd_v].length = value_length;
		Menu[id_2nd_v].max = value_input_max;
		Menu[id_2nd_v].min = value_input_min;
		Menu[id_2nd_v].symbol = value_sym;
		Menu[id_2nd_v].w_length = value_length;
		if (dc_type == 0x1C9E || dc_type == 0x1C9F)	//5-1, 5-2. Fixed Price,	6-1, 6-2, 6-3 Free Item
		{
			Menu[id_2nd_v].inhibit = ON;
		}
		else             
		{
			Menu[id_2nd_v].inhibit = OFF;
		}
	}


	MenuRefresh = ON;
	MenuCapAdjust = ON;
}

//////////////////////////////  21종
////		case 0x1C97:	//1-1. Unit price by weight
////		case 0x1C9A:	//1-2. Unit price by quantity
////		case 0x1CA0:	//2-1. %Unit price by quantity
////		case 0x1CA1:	//2-2. %Unit price by quantity
////		case 0x1C96:	//3-1. Total price by weight
////		case 0x1C9B:	//3-2. Total price by quantity
////		case 0x1C99:	//4-1. %total price by weight
////		case 0x1C9D:	//4-2. %total price by quantity
////		case 0x1C9E:	//5-1, 5-2. Fixed Price
////		case 0x1C9F:	//6-1, 6-2, 6-3 Free Item
////		case 0x1C98:	//7-1 Free addition by  weight
////		case 0x1C9C:	//7-2. Free addition by quantity
////		case 0x1C91:	//8-1, 8-2, Total price by Total price
////		case 0x1C95:	//9-1, 9-2 %Total price by total price
////		case 0x1C92:	//10-1, 10-2. unit price by total price
////		case 0x1C93:	//11-1. Weight by total price
////		case 0x1C94:	//11-2. Quantity by total price
////		case 0x1CA2:	//12-1. Unit price by weight+
////		case 0x1CA3:	//12-2. Unit price by quantity+
////		case 0x1CA4:	//13-1. N+1 by weight
////		case 0x1CA5:	//13-2. N+1 by quantity
//////////////////////////////

INT16S input_discount_data(DISCOUNT_STRUCT *dc_str, INT8U plu_type, INT8U start_page)
{
	CAPTION_STRUCT cap;

	char 	form_string[34];
#ifndef USE_SINGLE_LINE_GRAPHIC
	char 	daytype_str[15],timetype_str[15];
	char    date_form[14],time_form[14];
#endif
	INT8U	day_type;
	INT8U	date_sep, time_sep;
	INT16S   	result;
	INT8U 	ucTemp;

	INT16U  head_cap_code;
	INT8U	target_type;
	INT8U	target_dec;
	INT32U	target_input_max;
	INT32U	target_input_min;
	INT16U	target_length;
	INT8U   target_sym;
	INT8U	value_type;
	INT8U	value_dec;
	INT32U	value_input_max;
	INT32U	value_input_min;
	INT16U	value_length;
	INT8U   value_sym;

	INT32U	use_fsp;
	INT32U	dc_typenum;
	INT32U	first_target;
	INT32U	first_value;
	INT32U	second_target;
	INT32U	second_value;
	char dayofweek_str[10];
	char startdate_str[10];
	char enddate_str[10];
	char starttime_str[10];
	char endtime_str[10];
	 
	Menu_DiscPluType = plu_type;

#ifndef USE_SINGLE_LINE_GRAPHIC
	caption_split_by_code(0x1C19,&cap,0);	// "(0..23 Hour)"
	cap.form[14]=0;
	strcpy((char *)timetype_str,(char *)cap.form);
#endif

	day_type = rtc_get_date_format();
#ifndef USE_SINGLE_LINE_GRAPHIC
	switch(day_type) {
		case 0:	caption_split_by_code(0x384A, &cap,0); 	break;
		case 1:	caption_split_by_code(0x384B, &cap,0);	break;
		case 2:
		case 3: caption_split_by_code(0x384D, &cap,0);	break;
	}
	cap.form[14]=0;
	sprintf(daytype_str, "%s", cap.form);
	daytype_str[14]=0;
#endif

#if defined(USE_SINGLE_LINE_GRAPHIC)
	date_sep = '.'; //7-seg display용
	time_sep = '.'; //7-seg display용
#else
	date_sep = get_global_bparam(GLOBAL_DATE_SEPARATOR);
	time_sep = get_global_bparam(GLOBAL_TIME_SEPARATOR);
#endif
#ifndef USE_SINGLE_LINE_GRAPHIC
#ifdef HEBREW_FONT
	sprintf(date_form,"]  %c  %c  [",date_sep,date_sep);
	sprintf(time_form,"]  %c  %c  [",time_sep,time_sep);
#else
	sprintf(date_form,"[  %c  %c  ]",date_sep,date_sep);
	sprintf(time_form,"[  %c  %c  ]",time_sep,time_sep);
#endif
#endif
	switch(plu_type) {
		default:
		case PLU_BY_WEIGHT: 
			head_cap_code = 0x1532;
			break;
		case PLU_BY_PCS:
			head_cap_code = 0x1534;
			break;
		case PLU_BY_COUNT:
			head_cap_code = 0x1533;
			break; 
	}

	use_fsp = dc_str->useForFSP;
	dc_typenum = get_dc_typeno(dc_str->type);
	if (dc_typenum > max_dc_type || dc_typenum == 0)
	{
		dc_typenum = 1;
		switch(plu_type) {
			default:
			case PLU_BY_WEIGHT: dc_str->type = weight_type[dc_typenum]; break;
			case PLU_BY_COUNT :
			case PLU_BY_PCS   : dc_str->type = pcs_type[dc_typenum]; break;
		}
	}
	get_dc_input_type(dc_str->type, plu_type, &target_type, &value_type);
	get_dc_input_condition(target_type, &target_input_max, &target_input_min, &target_length, &target_dec, &target_sym);
	get_dc_input_condition(value_type, &value_input_max, &value_input_min, &value_length, &value_dec, &value_sym);	
	first_target = dc_str->target[0];
	first_value = dc_str->price[0];
	second_target = dc_str->target[1];
	second_value = dc_str->price[1];
	if(!input_range_check(first_target, target_input_min, target_input_max)) first_target = 0;
	if(!input_range_check(first_value, value_input_min, value_input_max)) first_value = 0;
	if(!input_range_check(second_target, target_input_min, target_input_max)) second_target = 0;
	if(!input_range_check(second_value, value_input_min, value_input_max)) second_value = 0;
	binary2str(dc_str->day_of_week,(INT8U *)dayofweek_str);
	input_day_format(day_type, startdate_str, dc_str->start_year, dc_str->start_month, dc_str->start_day);
	input_day_format(day_type, enddate_str, dc_str->end_year, dc_str->end_month, dc_str->end_day);
	if (dc_str->start_hour > 24) {
		dc_str->start_hour=0;
		dc_str->start_minute=0;
		dc_str->start_second=0;
	}
	if (dc_str->end_hour > 24) {
		dc_str->end_hour=0;
		dc_str->end_minute=0;
		dc_str->end_second=0;
	}
	input_time_format(starttime_str, dc_str->start_hour, dc_str->start_minute, dc_str->start_second);
	input_time_format(endtime_str, dc_str->end_hour, dc_str->end_minute, dc_str->end_second);
	
////////////// Menu 시작 //////////////////
///////////////////////////////////////////
	Menu_Init_WithPage(start_page);
#if !defined(USE_SINGLE_LINE_GRAPHIC)//3LINE
	MenuNumLinePerPage = 2;
#endif

	caption_split_by_code(head_cap_code, &cap, 0);
	sprintf(form_string, "%s", cap.form);
	Menu_HeadCreate(form_string);


	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG6);
	ucTemp = (ucTemp>>4)&0x01; //use FSP  SG061211
	if (ucTemp)
	{
		caption_split_by_code(0x1C11,&cap,0);	// "Use for FSP (Y/N) :"
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length, 
							cap.input_max, cap.input_min, 0, &use_fsp, NULL, NULL, NULL);
	}


	caption_split_by_code(0x1C12,&cap,0);	// "Discount Type :"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max+2, cap.input_min, 0, &dc_typenum, NULL, NULL, discount_selecttype);
	Menu_AddEndFunction(2, callback_end_disc_type);
	Menu_SetCallbackMode(2, 2); // bottom


//5-1, 5-2. Fixed Price 전용
	caption_split_by_code(0x1C20,&cap,0);	// "Fixed Price :"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, target_length, target_length, 
						cap.input_max, cap.input_min, value_dec, &first_value, NULL, NULL, NULL);
	Menu_SetSymbol(4, value_sym);
	Menu_SetStrAdjustable(4, ON);

/////////////////////////////////////////


	caption_split_by_code(0x1C44,&cap,0);	// "First Target :"
	cap.input_length  = target_length;
	cap.input_dlength = target_length;
	cap.input_max = target_input_max;
	cap.input_min = target_input_min;
	caption_adjust(&cap,NULL);
	Menu_InputCreate(5, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, target_length, target_length, 
						cap.input_max, cap.input_min, target_dec, &first_target, NULL, NULL, NULL);
	Menu_SetSymbol(5, target_sym);
	Menu_SetStrAdjustable(5, ON);


	caption_split_by_code(0x1C45,&cap,0);	// "First Value  :"
	cap.input_length  = value_length;
	cap.input_dlength = value_length;
	cap.input_max = value_input_max;
	cap.input_min = value_input_min;
	caption_adjust(&cap,NULL);
	Menu_InputCreate(6, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, value_length, value_length, 
						cap.input_max, cap.input_min, value_dec, &first_value, NULL, NULL, NULL);
	Menu_SetSymbol(6, value_sym);
	Menu_SetStrAdjustable(6, ON);


	caption_split_by_code(0x1C46,&cap,0);	// "Second Target :"
	cap.input_length  = target_length;
	cap.input_dlength = target_length;
	cap.input_max = target_input_max;
	cap.input_min = target_input_min;
	caption_adjust(&cap,NULL);
	Menu_InputCreate(7, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, target_length, target_length, 
						cap.input_max, cap.input_min, target_dec, &second_target, NULL, NULL, NULL);
	Menu_SetSymbol(7, target_sym);
	Menu_SetStrAdjustable(7, ON);


	caption_split_by_code(0x1C47,&cap,0);	// "Second Value  :"
	cap.input_length  = value_length;
	cap.input_dlength = value_length;
	cap.input_max = value_input_max;
	cap.input_min = value_input_min;
	caption_adjust(&cap,NULL);
	Menu_InputCreate(8, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, value_length, value_length, 
						cap.input_max, cap.input_min, target_dec, &second_value, NULL, NULL, NULL);
	Menu_SetSymbol(8, value_sym);
	Menu_SetStrAdjustable(8, ON);

#ifndef USE_SINGLE_LINE_GRAPHIC
	caption_split_by_code(0x1C13,&cap,0);	// "Input '1' or '0'  [SMTWTFS]"
	Menu_InputCreate(9, (HUGEDATA char *)cap.form, MENU_TYPE_FIX, cap.input_x, cap.input_dlength, cap.input_length, 
						0, 0, 0, NULL, NULL, NULL, NULL);
#endif

	strcpy(form_string,"1111111");
	caption_split_by_code(0x1C14,&cap,0);	// "Day of Week      :"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(10, (HUGEDATA char *)cap.form, MENU_TYPE_FORM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)dayofweek_str, (HUGEDATA INT8U *)form_string, NULL);

	sprintf(form_string,"99%c99%c99", date_sep, date_sep);
	caption_split_by_code(0x1C15, &cap, 0);	// "Start Date :"
#ifdef USE_THAI_FONT
	cap.input_x=Cvt_ThaiStrLength((INT8U*)cap.form)+1;
#else  
	cap.input_x=strlen((char *)cap.form)+1;
#endif
#ifndef USE_SINGLE_LINE_GRAPHIC
	strcat((char *)cap.form, date_form);
	strcat((char *)cap.form, daytype_str);
#endif
	//caption_adjust(&cap,NULL);
	Menu_InputCreate(11, (HUGEDATA char *)cap.form, MENU_TYPE_DATE, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)startdate_str, (HUGEDATA INT8U *)form_string, NULL);
//	Menu_AddEndFunction(11, callback_end_dc_checkdate);
	Menu_SetGeneralFlag(11, day_type);
	Menu_SetFixedCharacter(11, date_sep);

	caption_split_by_code(0x1C17, &cap, 0);	// "End   Date : "
#ifdef USE_THAI_FONT
    cap.input_x=Cvt_ThaiStrLength((INT8U*)cap.form)+1;
#else  
	cap.input_x=strlen((char *)cap.form)+1;
#endif
#ifndef USE_SINGLE_LINE_GRAPHIC
	strcat((char *)cap.form, date_form);
	strcat((char *)cap.form, daytype_str);
#endif
	//caption_adjust(&cap,NULL);
	Menu_InputCreate(12, (HUGEDATA char *)cap.form, MENU_TYPE_DATE, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)enddate_str, (HUGEDATA INT8U *)form_string, NULL);
//	Menu_AddEndFunction(12, callback_end_dc_checkdate);
	Menu_SetGeneralFlag(12, day_type);
	Menu_SetFixedCharacter(12, date_sep);

	sprintf(form_string,"99%c99%c99", time_sep, time_sep);
	caption_split_by_code(0x1C16, &cap, 0);	// "Start Time :"
#ifndef USE_SINGLE_LINE_GRAPHIC
  #ifdef USE_THAI_FONT
  	cap.input_x=Cvt_ThaiStrLength((INT8U*)cap.form)+1;
  #else
	cap.input_x=strlen((char *)cap.form)+1;
  #endif
	strcat((char *)cap.form,time_form);
	strcat((char *)cap.form,timetype_str);
#endif
	//caption_adjust(&cap,NULL);
	Menu_InputCreate(13, (HUGEDATA char *)cap.form, MENU_TYPE_TIME, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)starttime_str, (HUGEDATA INT8U *)form_string, NULL);
//	Menu_AddEndFunction(13, callback_end_dc_checktime);
	Menu_SetFixedCharacter(13, time_sep);


	caption_split_by_code(0x1C18, &cap, 0);	// "End   Time :"
#ifndef USE_SINGLE_LINE_GRAPHIC
  #ifdef USE_THAI_FONT
    cap.input_x=Cvt_ThaiStrLength((INT8U*)cap.form)+1;
  #else 
	cap.input_x=strlen((char *)cap.form)+1;
  #endif
	strcat((char *)cap.form,time_form);
	strcat((char *)cap.form,timetype_str);
#endif
	//caption_adjust(&cap,NULL);
	Menu_InputCreate(14, (HUGEDATA char *)cap.form, MENU_TYPE_TIME, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, (HUGEDATA INT8U *)endtime_str, (HUGEDATA INT8U *)form_string, NULL);
//	Menu_AddEndFunction(14, callback_end_dc_checktime);
	Menu_SetFixedCharacter(14, time_sep);


	//MenuDateType = day_type;
	Menu_return_last_item_arrow_dn = 1;
	Menu_return_first_item_arrow_up = 1;							
	result = Menu_InputDisplay();
	Menu_return_last_item_arrow_dn = 0;
	Menu_return_first_item_arrow_up = 0;
	if(result == MENU_RET_SAVE || result == MENU_RET_DN) 
	{
		dc_str->useForFSP = use_fsp;
		switch(plu_type) {
			case PLU_BY_WEIGHT: dc_str->type = weight_type[dc_typenum]; break;
			case PLU_BY_COUNT :
			case PLU_BY_PCS   : dc_str->type = pcs_type[dc_typenum]; break;
		}
		dc_str->target[0] = first_target;
		if(dc_typenum==13 && dc_str->target[0]!=0)
		{
			dc_str->price[0] = 1;
		}
		else
		{
			dc_str->price[0] = first_value;
		}
		dc_str->target[1] = second_target;
		dc_str->price[1] = second_value;	
		dc_str->day_of_week = str2binary((INT8U *)dayofweek_str);
       		input_convert_date(day_type, (INT8U *)startdate_str, &dc_str->start_year, &dc_str->start_month, &dc_str->start_day);
		input_convert_date(day_type, (INT8U *)enddate_str, &dc_str->end_year, &dc_str->end_month, &dc_str->end_day);
		input_convert_time((INT8U*)starttime_str, &dc_str->start_hour, &dc_str->start_minute, &dc_str->start_second);
		input_convert_time((INT8U*)endtime_str, &dc_str->end_hour, &dc_str->end_minute, &dc_str->end_second);

		//if( input_day_check(dc_str->start_year, dc_str->start_month, dc_str->start_day) ) {
		//if( input_day_check(dc_str->end_year, dc_str->end_month, dc_str->end_day) ) {

		//if( !input_time_check(starttime_str,2) )  
		//if( !input_time_check(endtime_str,2) )  
		return KP_SAVE;  
	}
	else if (result == MENU_RET_UP)
	{
		return KP_ARROW_UP;
	}
	else if (result == MENU_RET_ESC)
	{
		return KP_ESC;
	}
	return result;

}

#ifdef USE_PLU_MULTIPLE_PRICE
void multi_plu_disc(INT8U tType, long uPrice, long fTarget, long fPrice, long sTarget, long sPrice)
{
	INT8U	flag_tmp;
    char	old;
	INT32U	tempFPrice, tempSPrice;

	tempFPrice = uPrice - fPrice;
	tempSPrice = uPrice - sPrice;
        
    old=DspStruct.Id1;
		
	DiscSale.saleGlobalOk = ON;
	DiscSale.saleDiscountType = DISCOUNT_GLOBAL;
	if(status_scale.cur_pluid < MISC_PLU_WEIGHT)
	{
		Operation.override = OFF;
		SetMaxRide(PluType());
		SetCurRide(PluType());
	}

	DiscSale.discIndexData.discTargetType = tType;
	//flag_tmp=get_global_bparam(GLOBAL_SALE_SETUP_FLAG2); // Parameter 585
	DiscSale.discIndexData.discPriceType = DISC_PRICE_UPRICE;

	DiscSale.discIndexData.discTarget[0] = fTarget;//tPrice;//Price.TotalPrice[PRICE_RESULT];
	DiscSale.discIndexData.discTarget[1] = sTarget;//tPrice;//Price.TotalPrice[PRICE_RESULT];
	DiscSale.discIndexData.discPrice[0] = tempFPrice;
	DiscSale.discIndexData.discPrice[1] = tempSPrice;

	keyapp_callback_salemode();
	if (fPrice==0l && sPrice==0l) {
		DiscSale.saleGlobalOk = OFF;
		DiscSale.saleDiscountType = DISCOUNT_NO;
	}
	DspLoadFont1(old);
}
#endif
