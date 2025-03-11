/*****************************************************************************
|*  Copyright		:	(c) 2004 CAS
|*  Filename		:	pgm_global_set.c
|*  Version		:	v 0.9
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Modified		:	2004.05.20
|*  Description		:	Global setting       				
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
#include "barcode_file.h"
#include "initial.h"
#include "pgm_app.h"
#include "pgm_global_set.h"
#include "tax.h"
#include "discount.h"
#include "mode_sale.h"
#include "clerk_app.h"
#include "key_function.h"
#include "ethernet.h"
#include "adm_app.h"
#include "global_set.h"
#include "ethernet_comm.h"
#include "initial_glb.h"

extern INT8U temp_callback_discount_flag;
extern INT8U Prt_uprice_Symbol_flag;
extern INT8U Prt_weight_Symbol_flag;
extern INT8U permission_check(INT8U func_code, INT8U login_chk);
extern NETWORK_STATUS network_status;
extern INT8U barcodetype_codevalue[];
//extern INT16S  plucall_taxcode_pos_y;		//JJANG 20070612

//INT8U _gi8u_input_type=0;

#ifdef USE_TAX_RATE
void global_set_tax_type(void);
void global_set_tax_code(void);
#endif
void callback_initial_screen(INT16S num);
extern void callback_default_mode(long num);
// void global_set_change(INT16U tcode,INT16 icode,INT16U value_point,INT8U xtype);
extern INT8U	OnOff(INT8U	tmp);

INT8U global_printingoperation(INT16U fcode)
{
	INT8U ret;
	CAPTION_STRUCT 	cap;
	INT8U	key_label,
		report_mode,
		origin_line,
		sym_in_qty,
		pcs_in_unit,
		uctemp,
		uctemp1,
		uctemp2,	// Added by CJK
		uctemp3,
		uctemp4,
		uctemp5,
		uctemp6,
		prt_all_report,	// Added by CJK
		swedish_barcode, // SG060306
		prepackweek,	// LIM060320
		printReturn,	//SG070215
		labelPrintMode,
		prt_total_conti;
	INT16U  result, i;
	INT32U  set_val[11];
		
	ret=(INT8U)fcode;
	uctemp  = get_global_bparam(GLOBAL_PRINT_OPER_FLAG);
#ifndef _CL3000_PRINTER_MECH
	key_label    = (uctemp>>1) & 0x01;
#endif
	report_mode  = (uctemp>>2) & 0x03;
	origin_line  = (uctemp>>4) & 0x03;

	uctemp1  = get_global_bparam(GLOBAL_LABEL_FLAG);
	
	sym_in_qty  = (uctemp1>>0) & 0x01;
	pcs_in_unit = (uctemp1>>1) & 0x01;

	// Added by CJK
	uctemp2  = get_global_bparam(GLOBAL_USE_REPORT);
	
	prt_all_report  = (uctemp2>>0) & 0x01;

	//SG060306
	uctemp3 = get_global_bparam(GLOBAL_SALE_SETUP_FLAG2); 
	swedish_barcode = (uctemp3>>6) & 0x03;
	uctemp4 = get_global_bparam(GLOBAL_SALE_SETUP_FLAG5);
	prepackweek = (uctemp4>>1)&0x03;
	printReturn = (uctemp4>>5)&0x01; //SG070215

	uctemp5 = get_global_bparam(GLOBAL_SALE_SETUP_FLAG11);
	labelPrintMode = OnOff(uctemp5 >>7);
	
	uctemp6  = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
	prt_total_conti = (uctemp6 >> 5) & 0x01;
	/////////////////


	set_val[0]=(INT32U)prt_total_conti;
#ifndef _CL3000_PRINTER_MECH
	set_val[1]=(INT32U)key_label;
#endif
	set_val[2]=(INT32U)report_mode;
	set_val[3]=(INT32U)origin_line;
	set_val[4]=(INT32U)sym_in_qty;
	set_val[5]=(INT32U)pcs_in_unit;
	set_val[6]=(INT32U)prt_all_report;
	set_val[7]=(INT32U)swedish_barcode;
	set_val[8]=(INT32U)prepackweek;
	set_val[9]=(INT32U)printReturn;
	set_val[10]=(INT32U)labelPrintMode;

	Menu_Init();
	caption_split_by_code(0x1872, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

////	caption_split_by_code(0x20f3, &cap);
////	caption_adjust(&cap,NULL);
////	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
////						cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);

#ifndef _CL3000_PRINTER_MECH
	caption_split_by_code(0x3871, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);
#endif

	caption_split_by_code(0x3872, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[2], NULL, NULL, NULL);
	// <CODE> JHC, 040510, ADD, -Print origin for comm. line
	// [
	if ((status_scale.restrict&FUNC_ORG_TBL))
	{
		caption_split_by_code(0x3873, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[3], NULL, NULL, NULL);
	}
	// ]
	if (!Prt_weight_Symbol_flag)
	{
		caption_split_by_code(0x3874, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[4], NULL, NULL, NULL);
	}
	
	if (status_scale.use_pcs) {	// Added by CJK 20050728
		caption_split_by_code(0x3875, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &set_val[5], NULL, NULL, NULL);
	}

	caption_split_by_code(0x3876, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[6], NULL, NULL, NULL);

	//SG060306
	if (Startup.country!=COUNTRY_KR) {
		caption_split_by_code(0x3877, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[7], NULL, NULL, NULL);
	}
	caption_split_by_code(0x3878, &cap,0);		//가공요일 인쇄
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[8], NULL, NULL, NULL);

#ifndef USE_ONLY_LABEL
	//SG070215
	caption_split_by_code(0x3879, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[9], NULL, NULL, NULL);
#endif	

	caption_split_by_code(0x387A, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[10], NULL, NULL, NULL);

	caption_split_by_code(0x387B, &cap, 0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);
	
	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		prt_total_conti = (INT8U)set_val[0];
#ifndef _CL3000_PRINTER_MECH
		key_label   =(INT8U)set_val[1];
#endif
		report_mode =(INT8U)set_val[2];
		origin_line =(INT8U)set_val[3];
		sym_in_qty  =(INT8U)set_val[4];
		pcs_in_unit =(INT8U)set_val[5];
		prt_all_report =(INT8U)set_val[6];	// Added by CJK
		swedish_barcode = (INT8U)set_val[7];    //SG060306
		prepackweek = (INT8U)set_val[8];
		printReturn = (INT8U)set_val[9];
		labelPrintMode = (INT8U)set_val[10];

////		if(double_print) {
////			uctemp |= 0x01;// 0x40
////		} else {
////			uctemp &= (~0x01);
////		}
		// <CODE> JHC, 040513, Modify
		// [
#ifndef _CL3000_PRINTER_MECH
		if(key_label) {
			uctemp |= 0x02;// 0x40
		} else {
			uctemp &= (~0x02);
		}
#endif
		// ]
		uctemp &= (~0x0c);
		uctemp |= ((report_mode << 2) & 0x0c);

////		if(report_mode) {
////			uctemp |= 0x08;// 0x40
////		} else {
////			uctemp &= (~0x08);
////		}
		// <CODE> JHC, 040510, ADD
		// [
		for( i = 0; i < 2; i++)
		{
			if ((origin_line>>i) & 0x01)
				uctemp |= (0x01<<(4+i));
			else
				uctemp &= (~(0x01<<(4+i)));
		}
		// ]
		set_global_bparam(GLOBAL_PRINT_OPER_FLAG, uctemp);

		if(sym_in_qty) {
			uctemp1 |= 0x01;
		} else {
			uctemp1 &= (~0x01);
		}
		if(pcs_in_unit) {
			uctemp1 |= 0x02;
		} else {
			uctemp1 &= (~0x02);
		}

		set_global_bparam(GLOBAL_LABEL_FLAG, uctemp1);

		// Added by CJK
		if(prt_all_report) {
			uctemp2 |= 0x01;
		} else {
			uctemp2 &= (~0x01);
		}

		set_global_bparam(GLOBAL_USE_REPORT, uctemp2);
		//////////////////////

		ConfirmPrintOnLabel = report_mode;

		//SG060306
		uctemp3 &= (~0xc0);
		uctemp3 |= ((swedish_barcode << 6) & 0xc0);
		set_global_bparam(GLOBAL_SALE_SETUP_FLAG2, uctemp3);
		//-[lim060320
		uctemp4 &= (~0x06);
		uctemp4 |= ((prepackweek<<1)&0x06);

		uctemp4 &= (~0x20);
		uctemp4 |= ((printReturn<<5)&0x20);
		
		set_global_bparam(GLOBAL_SALE_SETUP_FLAG5, uctemp4);
		
		
		uctemp5 &= (~0x80);
		uctemp5 |= ((labelPrintMode<<7)&0x80);
		set_global_bparam(GLOBAL_SALE_SETUP_FLAG11, uctemp5);

		if(prt_total_conti)	//set
		{
			uctemp6 |= 0x20;
		}
		else	//reset
		{
			uctemp6 &= (~0x20);
		}
		set_global_bparam(GLOBAL_PRINT_OPER_FLAG2, uctemp6);
		
		//-[end
	}
/*
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
*/
	return ret;
}




INT8U global_function(INT16U fcode)
{
	INT8U ret;
	ret=TRUE;
//	rfcode=fcode&0xff00;
	if (!permission_check(PMS_GLOBAL,0)) return FALSE;
	switch (fcode) {
		case 0x1510: global_set_label();	 break;
		case 0x1520: global_set_barcode();	 break;
		case 0x1531: global_set_disc_priority(); break;
		case 0x1532: global_set_disc_weight();   break;
		case 0x1533: global_set_disc_count();	 break;
		case 0x1534: global_set_disc_pcs(); 	 break;
#ifdef USE_FIXED_PERCENT_KEY_SET
		case 0x1535: global_set_disc_key(); 	 break;
#endif
#ifdef USE_TAX_RATE        
		case 0x1541: global_set_tax_type();	 break;
		case 0x1542: global_set_tax_code();	 break;
#endif        
#ifdef USE_GSMART_PLU
		case 0x1550: global_set_PLU_default();	 break;
#endif
		default    : ret=FALSE; break;
	}
	if (ret) LoadGlobalSetting();
	return ret;
}

void callback_clear(long num)
{	
	POINT 	disp_p;
	INT8U 	buffer[64];
	INT8U 	backup_directdraw;
	long  	ltemp = num;

	backup_directdraw = DspStruct.DirectDraw;

	if (Startup.menu_type==0) return;
	disp_p = point((Startup.menu_type+2)*Startup.menu_ygap, 3*Startup.menu_xgap);
	if (Startup.menu_type == 2)	disp_p = point((CALLBACK_DISPLAY)*Startup.menu_ygap, 3*Startup.menu_xgap); //se-hyung 20071006
	memset((INT8U *)&buffer,0x20, sizeof(buffer) );
	buffer[63] = 0;
	DspStruct.DirectDraw = 1;
	Dsp_Write_String(disp_p,(HUGEDATA char *)&buffer);

	DspStruct.DirectDraw = backup_directdraw;
}

//extern INT8U LabelIdSetPosition;
extern void set_bit_on_byte(INT8U *data, INT8U set_val, INT8U nth);
void global_set_label(void)
{
	CAPTION_STRUCT	cap;	
	INT16S	result;
#ifdef USE_SPAR_MEMBER_PRICE
    INT32U 	memberprice_label;
#endif
	INT32U 	input_data[7];
	INT32U 	label2;
	INT8U	cnt;
	INT8U   uctemp, common;
	INT8U 	rFormName[7];
#ifdef USE_LOTTEMART_DEFAULT
	INT32U	onlinelabel;
	onlinelabel = get_global_bparam(GLOBAL_SALE_ONLINE_LABEL);
#endif
#ifdef USE_DISCOUNT_MEMBERSHIP
	INT32U dsclabel;
	dsclabel = get_global_bparam(GLOBAL_DISCOUNT_LABEL);
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	INT32U	setlabel;
	setlabel = get_global_lparam(GLOBAL_SALE_SET_LABEL);
#endif
	input_data[0] = get_global_bparam(GLOBAL_LABEL_PRIORITY);//Flash_bread(addr);
	input_data[1] = get_global_wparam(GLOBAL_LABEL_STANDARD);
	input_data[2] = get_global_wparam(GLOBAL_LABEL_MISC);
	input_data[3] = get_global_wparam(GLOBAL_LABEL_ADDUP);
	input_data[4] = get_global_wparam(GLOBAL_LABEL_FSP);
	label2 = get_global_bparam(GLOBAL_LABEL_2);

	common = get_global_bparam(GLOBAL_SALE_SETUP_FLAG3)&0x80;
	uctemp = get_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2);
	input_data[5] = (INT32U)((uctemp>>2) & 0x01); // 00100 = 0x04
	input_data[6] = (INT32U)((uctemp>>3) & 0x01);
#ifdef USE_SPAR_MEMBER_PRICE
    memberprice_label = get_global_wparam(GLOBAL_LABEL_MEMBER_PRICE);
#endif

	get_base_sparam(FLASH_EXT_PRT_LABEL_NAME, rFormName, 7);

	Menu_Init();
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	MenuNumLinePerPage = 2;
	MenuDefaultCallbackMode = 2;	// 2:bottom
#endif

	caption_split_by_code(0x1510, &cap, 0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);
	
	caption_split_by_code(0x2011, &cap,0);	// "Use global label(1=Y/0=N):"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &input_data[0], NULL, NULL, NULL);

	if (common) 
	{
		caption_split_by_code(0x2018,  &cap,0);	// "Label #:"
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &input_data[1], NULL, NULL, NULL);
		cnt = 3;
	}
	else
	{	
		caption_split_by_code(0x2012, &cap,0);	// "PLU Label No.    : "
		caption_adjust(&cap,NULL);
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &input_data[1], NULL, NULL, plucall_label_id);
		
		caption_split_by_code(0x2013, &cap,0);	// "Misc. Label No.  :"
		caption_adjust(&cap,NULL);
		Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &input_data[2], NULL, NULL, plucall_label_id);
		
		caption_split_by_code(0x2014, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(5, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &input_data[3], NULL, NULL, plucall_label_id);

		if(GlbOper.UseFSP)
		{		
			caption_split_by_code(0x2015, &cap,0);
			caption_adjust(&cap,NULL);
			Menu_InputCreate(6, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, 0, &input_data[4], NULL, NULL, plucall_label_id);
		}
		
		caption_split_by_code(0x2016, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(7, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &input_data[5], NULL, NULL, NULL);
		
		caption_split_by_code(0x2017, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(8, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &input_data[6], NULL, NULL, NULL);
		cnt = 9;
	}
#ifdef USE_LOTTEMART_DEFAULT
	sprintf(cap.form,"온라인라벨:");
	cap.input_length = 2;
	cap.input_dlength = 2;
	cap.input_itype = 'N';
	cap.input_max = 99;
	cap.input_min = 0;
	cap.inputtype = 0;
	cap.input_x = 13;

	caption_adjust(&cap,NULL);
	Menu_InputCreate(cnt, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &onlinelabel, NULL, NULL, NULL);
#endif
#ifdef USE_DISCOUNT_MEMBERSHIP
	sprintf(cap.form,"할인라벨:");
	cap.input_length = 2;
	cap.input_dlength = 2;
	cap.input_itype = 'N';
	cap.input_max = 99;
	cap.input_min = 0;
	cap.inputtype = 0;
	cap.input_x = strlen((char *)cap.form) + 1;

	caption_adjust(&cap,NULL);
	Menu_InputCreate(cnt, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &dsclabel, NULL, NULL, NULL);
#endif
#ifdef USE_SETPRODUCT_SALE
	sprintf(cap.form,"세트상품 라벨:");
	cap.input_length = 2;
	cap.input_dlength = 2;
	cap.input_itype = 'N';
	cap.input_max = 99;
	cap.input_min = 0;
	cap.inputtype = 0;
	cap.input_x = 13;

	caption_adjust(&cap,NULL);
	Menu_InputCreate(cnt, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &input_data[3], NULL, NULL, plucall_label_id);
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
	sprintf(cap.form,"세트라벨:");
	cap.input_length = 2;
	cap.input_dlength = 2;
	cap.input_itype = 'N';
	cap.input_max = 99;
	cap.input_min = 0;
	cap.inputtype = 0;
	cap.input_x = 8;

	caption_adjust(&cap,NULL);
	Menu_InputCreate(cnt, ( char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &setlabel, NULL, NULL, NULL);
#endif
	caption_split_by_code(0x2019, &cap,0);//라벨2 No:[ ]
	caption_adjust(&cap,NULL);
	Menu_InputCreate(cnt, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &label2, NULL, NULL, NULL);

	caption_split_by_code(0x201A, &cap,0);//ext. label name[]
	caption_adjust(&cap,NULL);
	Menu_InputCreate(10, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, NULL, rFormName, NULL, NULL);

#ifdef USE_SPAR_MEMBER_PRICE
    caption_split_by_code(0x201B, &cap,0);//member price
    caption_adjust(&cap,NULL);
    Menu_InputCreate(11, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
                        cap.input_max, cap.input_min, 0, &memberprice_label, NULL, NULL, plucall_label_id);
#endif

    result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) 
	{
		set_global_bparam(GLOBAL_LABEL_PRIORITY, input_data[0]);
#ifdef USE_LOTTEMART_DEFAULT
		set_global_bparam(GLOBAL_SALE_ONLINE_LABEL,(INT8U)onlinelabel);
#endif
#ifdef USE_DISCOUNT_MEMBERSHIP
		set_global_bparam(GLOBAL_DISCOUNT_LABEL,(INT8U)dsclabel);
#endif
#ifdef USE_HYUNDAI_SET_FUNCTION
		set_global_lparam(GLOBAL_SALE_SET_LABEL,(INT8U)setlabel);
#endif
		set_global_bparam(GLOBAL_LABEL_2, (INT8U)label2);
		set_base_sparam(FLASH_EXT_PRT_LABEL_NAME, rFormName, 7);
		if (common)
		{
			input_data[2] = input_data[1];
#ifndef USE_SETPRODUCT_SALE
			input_data[3] = input_data[1];
#endif
			input_data[5] = 0;
			input_data[6] = 0;
		}
		set_global_wparam(GLOBAL_LABEL_STANDARD, input_data[1]);
		set_global_wparam(GLOBAL_LABEL_MISC, input_data[2]);
		set_global_wparam(GLOBAL_LABEL_ADDUP, input_data[3]);
	 	if(GlbOper.UseFSP)
		{
			set_global_wparam(GLOBAL_LABEL_FSP, input_data[4]);
		}
		set_bit_on_byte(&uctemp, input_data[5], 2);
		set_bit_on_byte(&uctemp, input_data[6], 3);
		set_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2, uctemp);
#ifdef USE_SPAR_MEMBER_PRICE
        set_global_wparam(GLOBAL_LABEL_MEMBER_PRICE, memberprice_label);
#endif
		label_load(input_data[1]);
		global_set_pluinhibit(VERIFY_FUNC,0);
 	}
}


#ifdef USE_TAX_RATE 
void global_set_tax_code(void)
{	
	CAPTION_STRUCT 	cap;
	INT32U  input_data[2];
	INT16U	result;

	input_data[0] = get_global_bparam(GLOBAL_TAX_VALUE);
	input_data[1] = get_global_bparam(GLOBAL_MISC_TAX_VALUE);
	
    
	Menu_Init();
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	MenuNumLinePerPage = 2;
	MenuDefaultCallbackMode = 2;	// 2:bottom
#endif

	caption_split_by_code(0x1542, &cap, 0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);
	

	caption_split_by_code(0x2042, &cap,0);	// "Select Tax No. :"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &input_data[0], NULL, NULL, plucall_taxcode);
	
	caption_split_by_code(0x2043, &cap,0);	// "Misc Tax No. :"
	caption_adjust(&cap,NULL);
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &input_data[1], NULL, NULL, plucall_taxcode);
		
	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) 
	{
		set_global_bparam(GLOBAL_TAX_VALUE, input_data[0]);
		set_global_bparam(GLOBAL_MISC_TAX_VALUE, input_data[1]);
 	}
}
#endif


void global_set_barcode(void)	//program code = 0x1520
{
	CAPTION_STRUCT	cap;	
	INT32U input_data[10];
	INT32U bar2;
	INT16U xcode;
	INT16S i,menu_cnt;
	INT8U result,common;
#ifdef USE_QR_ADDTOTAL
	INT8U ttlbarcode_type;
	INT8U flag_tmp;
	INT8U bartype;
#endif
#ifdef USE_LOTTESUPER_DEFAULT
	// 롯데마트-슈퍼 통합 펌웨어에서 아래 코드는 고려하지 않기로 함
	INT8U barcodefmtNo_forWeightPLU;
	INT8U barcodefmtNo_forCountPLU;
	INT32U addr;
	INT8U structSize;
#endif

	input_data[0] = get_global_bparam(GLOBAL_BARCODE_PRIORITY);
	menu_cnt = 10;
	for(i=0; i<menu_cnt-1; i++) {
		input_data[i+1] = (INT32U)get_global_bparam(GLOBAL_BARCODE_STD_WID+i);
	}
	bar2 = (INT32U)get_global_bparam(GLOBAL_BARCODE_2);

    	Menu_Init();
	caption_split_by_code(0x1520, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);
	common=get_global_bparam(GLOBAL_SALE_SETUP_FLAG3)&0x40;
	if (common) {
		menu_cnt=2;
	}
	for (i=0; i<menu_cnt; i++) {
		xcode=0x3521+i;
		if ((i==1) && (common)) xcode=0x352B;
		if (!status_scale.use_pcs) {
			if ((xcode==0x3524) || (xcode==0x3528)) continue;
		}
		if (!status_scale.use_bycount) {
			if ((xcode==0x3523) || (xcode==0x3527)) continue;
		}
		caption_split_by_code(xcode, &cap,0);
		caption_adjust(&cap,NULL);
		if (xcode==0x3521)
			Menu_InputCreate(i+1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &input_data[i], NULL, NULL, NULL);
		else {
#ifdef USE_QR_ADDTOTAL
			flag_tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG14);
			flag_tmp = OnOff(flag_tmp & 0x08);	//param 726
			if((xcode==0x352A) && (flag_tmp)); // param 726 Flag가 On 일 ? Add up Total Barcode 2 사용안함
			else
#endif
			Menu_InputCreate(i+1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &input_data[i], NULL, NULL,callback_barcode);
		}
	}
	caption_split_by_code(0x352C, &cap, 0);//바코드2 No:[ ]
	caption_adjust(&cap,NULL);
	Menu_InputCreate(i+1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
				cap.input_max, cap.input_min, 0, &bar2, NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		set_global_bparam(GLOBAL_BARCODE_PRIORITY,(INT8U)input_data[0]);
		set_global_bparam(GLOBAL_BARCODE_2,(INT8U)bar2);
		if (common) {
			menu_cnt=10;
		}
		for(i=0; i<menu_cnt-1; i++)  {
			if (common) {
				if (i) input_data[i+1]=input_data[1];
			}
			set_global_bparam(GLOBAL_BARCODE_STD_WID+i,(INT8U)input_data[i+1]);
#ifdef USE_LOTTESUPER_DEFAULT
			// 롯데마트-슈퍼 통합 펌웨어에서 아래 코드는 고려하지 않기로 함
			if(i == 0)
			{
				barcodefmtNo_forWeightPLU = (INT8U)input_data[i+1];	//무게상품 입력 값 
			}
			else if(i == 1)
			{
				barcodefmtNo_forCountPLU =  (INT8U)input_data[i+1]; // 개수상품 입력 값
			}
#endif
		}
#ifdef USE_LOTTESUPER_DEFAULT
		// 롯데마트-슈퍼 통합 펌웨어에서 아래 코드는 고려하지 않기로 함
		addr = DFLASH_BASE + FLASH_LABEL_KEY_TABLE_AREA;
		structSize = sizeof(LABEL_KEY_STRUCT); // 라벨변경 정보 저장용 구조체 크기 : 6
		addr += structSize; // 롯데슈퍼 보관라벨 주소
		addr += structSize; // 롯데슈퍼 일반라벨 (무게 상품) 주소
		Flash_wwrite(addr+2, (INT16U)barcodefmtNo_forWeightPLU);
		addr += structSize; // 롯데슈퍼 일반라벨 (개수 상품) 주소
		Flash_wwrite(addr+2, (INT16U)barcodefmtNo_forCountPLU);
#endif
		global_set_pluinhibit(VERIFY_FUNC,0); //se-hyung 070802
#ifdef USE_QR_ADDTOTAL
		flag_tmp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG14);
		flag_tmp = OnOff(flag_tmp & 0x08);	//param 726
		if (flag_tmp)
		{
			ttlbarcode_type = get_barcodetype(GLOBAL_BARCODE_TOTAL_ID);
			bartype = barcodetype_codevalue[ttlbarcode_type];
			if (bartype == QRCODE) // Add up Total barcode type이 QR이면 QrAddTotalFlag On
				QrAddTotalFlag = 1;
			else
				QrAddTotalFlag = 0;
		}
#endif
	}
	return;	
}


#ifdef USE_TAX_RATE
void global_set_tax_type(void)
{
	CAPTION_STRUCT 	cap;
	INT32U	tax_priority;
	INT16U  result;

	tax_priority = (INT32U)get_global_bparam(GLOBAL_TAX_FLAG);


    	Menu_Init();

	caption_split_by_code(0x1541, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x2041, &cap, 0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
				cap.input_max, cap.input_min, 0, &tax_priority, NULL, NULL, callback_tax_type);
	Menu_SetCallbackMode(1, 1);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		set_global_bparam(GLOBAL_TAX_FLAG, (INT8U)tax_priority);
		global_set_pluinhibit(VERIFY_FUNC,0);
	}
}
#endif

void global_set_disc_priority(void)
{
	CAPTION_STRUCT 	cap;
	INT32U	dc_priority;
	INT8U   result;

	dc_priority = (INT32U)get_global_bparam(GLOBAL_DISCOUNT_PRIORITY);


    	Menu_Init();

	caption_split_by_code(0x1531, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x203A, &cap, 0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
				cap.input_max, cap.input_min, 0, &dc_priority, NULL, NULL, callback_discount_priority);
	Menu_SetCallbackMode(1, 1);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		set_global_bparam(GLOBAL_DISCOUNT_PRIORITY, (INT8U)dc_priority);
	}
}


void global_set_disc_weight(void)
{
	DISCOUNT_STRUCT disc_str;
	
	INT8U result;
	INT32U	addr;
//	char str[32];

	addr = DFLASH_BASE  + FLASH_DISCOUNT_GLOBAL_WEIGHT;
	Flash_sread(addr, (HUGEDATA INT8U *)&disc_str, DISCOUNT_STRUCT_SIZE);
	result = input_discount_data(&disc_str, PLU_BY_WEIGHT, 0);
	switch(result) {
		case KP_ESC:
		case KP_ARROW_UP:
			goto RET_ESC;
		case KP_SAVE:
		case KP_ARROW_DN:
			goto RET_END;
	}

RET_END:
	Flash_swrite(addr, (HUGEDATA INT8U *)&disc_str, DISCOUNT_STRUCT_SIZE);
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
 	return;
}


void global_set_disc_pcs(void)
{
	DISCOUNT_STRUCT disc_str;
	
	INT8U result;
	INT32U	addr;

	addr = DFLASH_BASE  + FLASH_DISCOUNT_GLOBAL_COUNT;

	Flash_sread(addr, (HUGEDATA INT8U *)&disc_str, DISCOUNT_STRUCT_SIZE);

	result = input_discount_data(&disc_str, PLU_BY_PCS, 0);

	switch(result) {
		case KP_ESC:
		case KP_ARROW_UP:
			goto RET_ESC;
		case KP_SAVE:
		case KP_ARROW_DN:
			goto RET_END;
	}
RET_END:
	Flash_swrite(addr, (HUGEDATA INT8U *)&disc_str, DISCOUNT_STRUCT_SIZE);
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
 	return;
}

void global_set_disc_count(void)
{
	DISCOUNT_STRUCT disc_str;
	
	INT8U result;
	INT32U	addr;

	addr = DFLASH_BASE  + FLASH_DISCOUNT_GLOBAL_NONFOOD;
	Flash_sread(addr, (HUGEDATA INT8U *)&disc_str, DISCOUNT_STRUCT_SIZE);
	result = input_discount_data(&disc_str, PLU_BY_COUNT, 0);
	switch(result) {
		case KP_ESC:
		case KP_ARROW_UP:
			goto RET_ESC;
		case KP_SAVE:
		case KP_ARROW_DN:
			goto RET_END;
	}

RET_END:
	Flash_swrite(addr, (HUGEDATA INT8U *)&disc_str, DISCOUNT_STRUCT_SIZE);
RET_ESC:
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	Dsp_Diffuse();
 	return;
}

#ifdef USE_FIXED_PERCENT_KEY_SET
void global_set_disc_key(void)
{
	CAPTION_STRUCT	cap;	
	INT32U disc_rate[5];
	INT8U i, result, codeErr, discNum[5];
	char discTemp[10];
	POINT disp_p;
	DISCOUNT_KEY_STRUCT discKey;
	INT32U	addr;

	codeErr = 0;
	
    Menu_Init();
	MenuNumLinePerPage = 2;
	caption_split_by_code(0x1535, &cap,0);	// Discount key setting
	Menu_HeadCreate((HUGEDATA char *)cap.form);
	
	for(i=0;i<5;i++)
	{	
		addr = DFLASH_BASE + FLASH_DISCOUNT_RATE_AREA + (i*DISCOUNT_KEY_STRUCT_SIZE);
		Flash_sread(addr, (HUGEDATA INT8U*)&discKey, sizeof(DISCOUNT_KEY_STRUCT));
		
		caption_split_by_code(0x2047, &cap, 0);	//Discount key 1~5:[  ]%
		sprintf(discTemp, " %d :", i+1);
		strcat(cap.form, discTemp);
		caption_adjust(&cap,NULL);
		if(discKey.code != i+1)
		{
			discNum[i] = discKey.code;
			codeErr = 1;
		}
		else codeErr = 0;
		disc_rate[i] = discKey.rate;

		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
					cap.input_max, cap.input_min, 0, &disc_rate[i], NULL, NULL, NULL);
	}
	result = Menu_InputDisplay();
	
	if(result == MENU_RET_SAVE)
	{
		for(i=0;i<5;i++)
		{
			if(codeErr) 
			{
				addr = DFLASH_BASE + FLASH_DISCOUNT_RATE_AREA + (i*DISCOUNT_KEY_STRUCT_SIZE);
				Flash_swrite(addr,(HUGEDATA INT8U *)&discNum[i],1);
			}
			addr = DFLASH_BASE + FLASH_DISCOUNT_RATE_AREA + (i*DISCOUNT_KEY_STRUCT_SIZE+1);
			Flash_swrite(addr,(HUGEDATA INT8U *)&disc_rate[i],1);
		}
	}
	return;
}
#endif

//DEL071122void callback_sale_mode(INT16S num)
//DEL071122{
//DEL071122	CAPTION_STRUCT 		cap;
//DEL071122	POINT 			disp_p;
//DEL071122
//DEL071122//	INT8U	y_gab=8, x_gab=6;				// 폰트의 x 크기
//DEL071122	char buffer[64];
//DEL071122
//DEL071122	disp_p = point((1+Startup.menu_type)*Startup.menu_ygap, 0);
//DEL071122	memset((INT8U *)&buffer,0x20, 64);
//DEL071122	buffer[63] = 0;
//DEL071122
//DEL071122	DspStruct.DirectDraw = 1;
//DEL071122	Dsp_Write_String(disp_p,(HUGEDATA char *)&buffer);
//DEL071122
//DEL071122	disp_p = point((1+Startup.menu_type)*Startup.menu_ygap, 3*Startup.menu_xgap);
//DEL071122
//DEL071122	caption_split_by_code(0x20D1, &cap,0);
//DEL071122
//DEL071122	if ((num<cap.input_min) || (num>cap.input_max)) {
//DEL071122		DspStruct.DirectDraw = 1;
//DEL071122		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
//DEL071122		return;
//DEL071122	} else {
//DEL071122		caption_split_by_code(0x20D9+num, &cap,0);
//DEL071122		DspStruct.DirectDraw = 1;
//DEL071122		Dsp_Write_String(disp_p,(HUGEDATA char *)&cap.form[3]);
//DEL071122	}
//DEL071122
//DEL071122}
//DEL071122
//DEL071122void callback_operation_mode(INT16S num)
//DEL071122{
//DEL071122	CAPTION_STRUCT 		cap;
//DEL071122	POINT 			disp_p;
//DEL071122
//DEL071122//	INT8U	y_gab=8, x_gab=6;				// 폰트의 x 크기
//DEL071122	char buffer[64];
//DEL071122
//DEL071122	disp_p = point((1+Startup.menu_type)*Startup.menu_ygap, 0);
//DEL071122	memset((INT8U *)&buffer,0x20, 64);
//DEL071122	buffer[63] = 0;
//DEL071122
//DEL071122	DspStruct.DirectDraw = 1;
//DEL071122	Dsp_Write_String(disp_p,(HUGEDATA char *)&buffer);
//DEL071122
//DEL071122	disp_p = point((1+Startup.menu_type)*Startup.menu_ygap, 3*Startup.menu_xgap);
//DEL071122
//DEL071122	caption_split_by_code(0x20D2, &cap,0);
//DEL071122
//DEL071122	if ((num<cap.input_min) || (num>cap.input_max)) {
//DEL071122		DspStruct.DirectDraw = 1;
//DEL071122		Dsp_Write_String(disp_p,(HUGEDATA char *)global_message[MSG_INVALID_VALUE]);
//DEL071122		return;
//DEL071122	} else {
//DEL071122		caption_split_by_code(0x20DC+num, &cap,0);
//DEL071122		DspStruct.DirectDraw = 1;
//DEL071122		Dsp_Write_String(disp_p,(HUGEDATA char *)&cap.form[3]);
//DEL071122	}
//DEL071122
//DEL071122}


void set_bit_on_byte(INT8U *data, INT8U set_val, INT8U nth)
{
	if(set_val) {
		*data = *data | (0x01<<nth);
	} else {
		*data = *data & (~(0x01<<nth) );
	}
}
		
		
// Modified by CJK 20050726		       
void global_set_clerk_print_ticket_item(void)
{
	CAPTION_STRUCT	cap;	
	char  uctemp, uctemp1;
	INT16S	result;
	INT32U 	input_data[5];	

	uctemp = get_global_bparam(GLOBAL_PRINT_CLERK_FLAG1);
//	bit0:   prt. clerk name
//	bit1:	Prt. Store name
//	bit2:	Prt. List
//	bit3:	Prt. Total Barcode
//	bit4:	Prt. Total Label
//	bit5:	Prt. Item  barcode
//	bit6:	Ticket Head Form No.
//	bit7:	Ticket Tail Form No.
	uctemp1 = get_global_bparam(GLOBAL_PRINT_CLERK_FLAG2);
//	bit0:   prt. label
//	bit1:	Prt. tax information
//	bit2:	Prt. traceability
//	bit3:	Prt. Double Ticket
//	bit4:	Prt. List(D.Ticket)			 

	input_data[0] = (INT32U)((uctemp>>2) & 0x01);
	input_data[1] = (INT32U)((uctemp>>3) & 0x01);
	input_data[2] = (INT32U)((uctemp>>4) & 0x01);
	input_data[3] = (INT32U)((uctemp1>>3) & 0x01);
	input_data[4] = (INT32U)((uctemp1>>4) & 0x01);

	Menu_Init();
	caption_split_by_code(0x1761, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x38C1, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[0], NULL, NULL, NULL);

	caption_split_by_code(0x38C2, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[1], NULL, NULL, NULL);

	caption_split_by_code(0x38C3, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[2], NULL, NULL, NULL);

	caption_split_by_code(0x38C5, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[3], NULL, NULL, NULL);

	caption_split_by_code(0x38C6, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[4], NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		set_bit_on_byte((INT8U*)&uctemp, (INT8U)input_data[0], 2);
		set_bit_on_byte((INT8U*)&uctemp, (INT8U)input_data[1], 3);
		set_bit_on_byte((INT8U*)&uctemp, (INT8U)input_data[2], 4);
		set_global_bparam(GLOBAL_PRINT_CLERK_FLAG1, uctemp);
		set_bit_on_byte((INT8U*)&uctemp1, (INT8U)input_data[3], 3);
		set_bit_on_byte((INT8U*)&uctemp1, (INT8U)input_data[4], 4);
		set_global_bparam(GLOBAL_PRINT_CLERK_FLAG2, uctemp1);
	}
}

void global_set_clerk_print_list_item(void)
{
	CAPTION_STRUCT	cap;	
	char  uctemp, uctemp1;
	INT16S	result;
    INT32U  flash_addr;
    INT8U   strTemp1[64];
    INT8U   strTemp2[64];
    
	INT32U 	input_data[7];	//fix size
    
	uctemp = get_global_bparam(GLOBAL_PRINT_CLERK_FLAG1);
//      bit0:   prt. clerk name
//	bit1:	Prt. Store name
//	bit2:	Prt. List
//	bit3:	Prt. Total Barcode
//	bit4:	Prt. Total Label
//	bit5:	Prt. Item  barcode
//	bit6:	Ticket Head Form No.
//	bit7:	Ticket Tail Form No.
	uctemp1 = get_global_bparam(GLOBAL_PRINT_CLERK_FLAG2);
//      bit0:   prt. label
//	bit1:	Prt. tax information
//	bit2:	Prt. traceability
//	bit3:	Prt. Double Ticket
//	bit4:	Prt. List(D.Ticket)
	 
	input_data[0] = (INT32U)((uctemp>>1) & 0x01);
	input_data[1] = (INT32U)((uctemp>>0) & 0x01);
	input_data[2] = (INT32U)((uctemp>>5) & 0x01);
	input_data[3] = (INT32U)((uctemp1>>1) & 0x01);
	input_data[4] = (INT32U)((uctemp1>>2) & 0x01);
	input_data[5] = (INT32U)get_global_wparam(GLOBAL_TICKET_MESSAGE_HEAD); //label form #
	input_data[6] = (INT32U)get_global_wparam(GLOBAL_TICKET_MESSAGE_TAIL); //label form #
    
	flash_addr = DFLASH_BASE + FLASH_TICKET_HEAD_MESSAGE_AREA;
	Flash_sread(flash_addr, (HUGEDATA INT8U *)strTemp1, 64);

	flash_addr = DFLASH_BASE + FLASH_TICKET_TAIL_MESSAGE_AREA;
	Flash_sread(flash_addr, (HUGEDATA INT8U *)strTemp2, 64);

	//////////////////////////////

	Menu_Init();
	caption_split_by_code(0x1762, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x38B1, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &input_data[0], NULL, NULL, NULL);
	}
	caption_split_by_code(0x38B2, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[1], NULL, NULL, NULL);
	}
	caption_split_by_code(0x38B3, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[2], NULL, NULL, NULL);
	}
	caption_split_by_code(0x38B4, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[3], NULL, NULL, NULL);
	}
#ifdef USE_TRACEABILITY
	caption_split_by_code(0x38B5, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[4], NULL, NULL, NULL);
	}
#endif
	caption_split_by_code(0x38B6, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[5], NULL, NULL, NULL);
	}

	caption_split_by_code(0x38B7, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[6], NULL, NULL, NULL);
	}

	caption_split_by_code(0x38B8, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, NULL, strTemp1, NULL, NULL);
	}
    
	caption_split_by_code(0x38B9, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, NULL, strTemp2, NULL, NULL);
	}

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		set_bit_on_byte((INT8U*)&uctemp, (INT8U)input_data[0], 1);
		set_bit_on_byte((INT8U*)&uctemp, (INT8U)input_data[1], 0);
		set_bit_on_byte((INT8U*)&uctemp, (INT8U)input_data[2], 5);
		set_global_bparam(GLOBAL_PRINT_CLERK_FLAG1, uctemp);

		set_bit_on_byte((INT8U*)&uctemp1, (INT8U)input_data[3], 1);
		set_bit_on_byte((INT8U*)&uctemp1, (INT8U)input_data[4], 2);
		set_global_bparam(GLOBAL_PRINT_CLERK_FLAG2, uctemp1);

		set_global_wparam(GLOBAL_TICKET_MESSAGE_HEAD ,(INT8U)input_data[5]);
		set_global_wparam(GLOBAL_TICKET_MESSAGE_TAIL ,(INT8U)input_data[6]);
        
        flash_addr = DFLASH_BASE + FLASH_TICKET_HEAD_MESSAGE_AREA;
        Flash_swrite(flash_addr, (HUGEDATA INT8U *)strTemp1, 64);
    
        flash_addr = DFLASH_BASE + FLASH_TICKET_TAIL_MESSAGE_AREA;
        Flash_swrite(flash_addr, (HUGEDATA INT8U *)strTemp2, 64);        
	}
}

void global_set_clerk_print_font_size(void)
{
	CAPTION_STRUCT	cap;	
	char  uctemp;
	INT16S	result;
	INT32U 	input_data[4];	

//초기화 하기
	uctemp = get_global_bparam(GLOBAL_PRT_TICKET_FONT_SIZE);
	 
	input_data[0] = (INT32U)((uctemp) & 0x03);      // plu name
	input_data[1] = (INT32U)((uctemp>>2) & 0x03);	// transaction data
	input_data[2] = (INT32U)((uctemp>>4) & 0x03);	// total value
	input_data[3] = (INT32U)((uctemp>>6) & 0x03);	// ticket serial number


	Menu_Init();
	caption_split_by_code(0x1763, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x38E1, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &input_data[0], NULL, NULL, NULL);
	}
	caption_split_by_code(0x38E2, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[1], NULL, NULL, NULL);
	}
	caption_split_by_code(0x38E3, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[2], NULL, NULL, NULL);
	}
	caption_split_by_code(0x38E4, &cap,0);
	caption_adjust(&cap,NULL);
	if (caption_run_check(cap.code,cap.runflag)) {
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[3], NULL, NULL, NULL);
	}

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		
		uctemp =   ((INT8U)input_data[0]& 0x03)      |           
		           ((INT8U)(input_data[1]<<2) & 0x0C)|  	 	
			   ((INT8U)(input_data[2]<<4) & 0x30)|
			   ((INT8U)(input_data[3]<<6) & 0xC0);	

		set_global_bparam(GLOBAL_PRT_TICKET_FONT_SIZE, uctemp);
	}
}


// Modified by CJK 20040712
void global_set_addup_print_condition(void)
{
	CAPTION_STRUCT	cap;	
	char  uctemp, uctemp1;
	INT16S	result;
	INT32U 	input_data[2];	
		  
	uctemp  = get_global_bparam(GLOBAL_PRINT_ADDUP_FLAG1);
//	bit0:	Prt. Store name
//	bit1:	Prt. List
//	bit2:	Prt. Barcode
//	bit3:	Prt. total use label form 
//	bit4:	Prt. barcode for each list
//	bit5:	Prt. Head label message
//	bit6:	Prt. Tail label message
	uctemp1 = get_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2);

	input_data[0] = (uctemp>>3) & 0x01;
	input_data[1] = (uctemp1>>0) & 0x01;

	Menu_Init();
	caption_split_by_code(0x1750, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x38C3, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[0], NULL, NULL, NULL);

	caption_split_by_code(0x38C4, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &input_data[1], NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		set_bit_on_byte((INT8U*)&uctemp, (INT8U)input_data[0], 3);
		set_global_bparam(GLOBAL_PRINT_ADDUP_FLAG1, uctemp);

		set_bit_on_byte((INT8U*)&uctemp1, (INT8U)input_data[1], 0);
		set_global_bparam(GLOBAL_PRINT_ADDUP_FLAG2, uctemp1);
	}
}

void print_markdown_set(void)
{
	CAPTION_STRUCT 	cap;
	INT8U	total, 
		unit,
		weight,
		quantity,
		uctemp;
	INT16U  result;
	INT32U set_val[4];
	
	uctemp  = get_global_bparam(GLOBAL_LABEL_MARKDOWNFLAG);

	total 	 = (uctemp>>0) & 0x01;
	unit 	 = (uctemp>>1) & 0x01;
	weight   = (uctemp>>2) & 0x01;
	quantity = (uctemp>>3) & 0x01;
	set_val[0] = total;
	set_val[1] = unit;
	set_val[2] = weight;
	set_val[3] = quantity;
	Menu_Init();
	caption_split_by_code(0x1720, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x1721, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);

	caption_split_by_code(0x1722, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);
	caption_split_by_code(0x1723, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[2], NULL, NULL, NULL);
	caption_split_by_code(0x1724, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &set_val[3], NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		total    = set_val[0];
		unit	 = set_val[1];
		weight	 = set_val[2];
		quantity = set_val[3];

		set_bit_on_byte(&uctemp, set_val[0], 0);
		set_bit_on_byte(&uctemp, set_val[1], 1);
		set_bit_on_byte(&uctemp, set_val[2], 2);
		set_bit_on_byte(&uctemp, set_val[3], 3);
		set_global_bparam(GLOBAL_LABEL_MARKDOWNFLAG, uctemp);
	}
}

void plu_test_label_flag(void)
{
	CAPTION_STRUCT	cap;
	INT8U uctemp;
	INT32U set_val;
	INT8U result;
	
	uctemp = get_global_bparam(GLOBAL_PRINT_OPER_FLAG2);
	set_val = (INT32U)(uctemp&0x01);

	Menu_Init();

	//caption_split_by_code(0x9131, &cap,0);
	caption_split_by_code(0x1170, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x3170, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val, NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		set_bit_on_byte((INT8U*)&uctemp, (INT8U)set_val, 0);
		set_global_bparam(GLOBAL_PRINT_OPER_FLAG2, uctemp);
	}

}


#ifdef USE_GSMART_PLU	 
void global_set_PLU_default(void)	//Added by JJANG 20081014 menu 1550, GS전용
{
	CAPTION_STRUCT	cap;	
	INT8U 	menu_cnt;
	INT16S	i, result;
	INT16S	j;
	//INT16U  cap_id[16]={39, 40, 41, 42, 43, 44, 45, 46, 78, 18, 79, 19, 76, 16, 77, 17};
#if (PLU_COMMODITY_LINE_NO == 8)
	INT32U 	input_data[12];
	INT16U  cap_id[12]={39, 40, 41, 42, 43, 44, 45, 46, 78, 79, 76, 77};
#else
	INT32U 	input_data[9];
	INT16U  cap_id[9]={39, 40, 41, 42, 43, 78, 79, 76, 77};
#endif

	for (j = 0; j < PLU_COMMODITY_LINE_NO; j++)
	{
		input_data[j] = get_base_bparam(FLASH_PLU_FONT_SIZE + j);
	}
	input_data[j++] = get_base_bparam(FLASH_PACKED_DATE_FLAG);	// 8
	//input_data[9] = get_base_wparam(FLASH_PACKED_DATE);
	input_data[j++] = get_base_bparam(FLASH_PACKED_TIME_FLAG);	// 9
	//input_data[11] = get_base_wparam(FLASH_PACKED_TIME);
	input_data[j++] = get_base_bparam(FLASH_SELL_BY_DATE_FLAG);	// 10
	//input_data[13] = get_base_lparam(FLASH_SELL_BY_DATE);
	input_data[j++] = get_base_bparam(FLASH_SELL_BY_TIME_FLAG);	// 11
	//input_data[15] = get_base_wparam(FLASH_SELL_BY_TIME);

	menu_cnt = j;
    	Menu_Init();
	caption_split_by_code(0x1550, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_continuous_search = ON;
	caption_continuous_search_addr = 0;
	for (i = 0; i<menu_cnt; i++) 
	{
		caption_split_by_code(cap_id[i], &cap,0);
		caption_adjust(&cap,NULL);
		cap.input_min = 0;		//'0'도 입력할 수 있게 하기위함
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &input_data[i], NULL, NULL, NULL);
	}
	caption_continuous_search = OFF;
PAGE_VIEW:
       	result = Menu_InputDisplay();
	switch(result)
	{
		case MENU_RET_SAVE:
			for (j = 0; j < PLU_COMMODITY_LINE_NO; j++)
			{
				set_base_bparam(FLASH_PLU_FONT_SIZE+j, input_data[j]);
			}
			set_base_bparam(FLASH_PACKED_DATE_FLAG,input_data[j++]); 
			//set_base_wparam(FLASH_PACKED_DATE, input_data[9]); 
			set_base_bparam(FLASH_PACKED_TIME_FLAG, input_data[j++]);
			//set_base_wparam(FLASH_PACKED_TIME, input_data[11]);
			set_base_bparam(FLASH_SELL_BY_DATE_FLAG, input_data[j++]); 
			//set_base_lparam(FLASH_SELL_BY_DATE, input_data[13]);
			set_base_bparam(FLASH_SELL_BY_TIME_FLAG, input_data[j++]); 
			//set_base_wparam(FLASH_SELL_BY_TIME, input_data[15]);
		case MENU_RET_ESC:
			goto RET_ESC;
		default :
			goto PAGE_VIEW;
	}
RET_ESC:
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse(); 
	return;
}

INT8U PrtPluNameFontSize[8] = 	{2, 2, 1, 1, 1, 1, 1, 1};
void global_PLU_default_init(void)	//Added by JJANG 20081014, 초기화루틴
{
	INT16S j;
	for (j = 0; j<8; j++)
	{
		set_base_bparam(FLASH_PLU_FONT_SIZE+j, PrtPluNameFontSize[j]);
	}
	set_base_bparam(FLASH_PACKED_DATE_FLAG,2); 
	set_base_wparam(FLASH_PACKED_DATE,0); 
#ifdef USE_EMART_DEFAULT
	set_base_bparam(FLASH_PACKED_TIME_FLAG,1);
#else
	set_base_bparam(FLASH_PACKED_TIME_FLAG,2);
#endif
	set_base_wparam(FLASH_PACKED_TIME,0);
	set_base_bparam(FLASH_SELL_BY_DATE_FLAG,2); 
	set_base_lparam(FLASH_SELL_BY_DATE,0);
#ifdef USE_EMART_DEFAULT
	set_base_bparam(FLASH_SELL_BY_TIME_FLAG,1); 
#else
	set_base_bparam(FLASH_SELL_BY_TIME_FLAG,2); 
#endif
	set_base_wparam(FLASH_SELL_BY_TIME,0);
}

#endif

#ifdef USE_REMOTE_FUNCTION
 #define MAX_APP_MENULIST		9
#else
 #define MAX_APP_MENULIST		8
#endif
void global_application_set(void)
{
	CAPTION_STRUCT 	cap;
	INT32U  flag[MAX_APP_MENULIST];
	INT16U  result,mask,i,
		uitemp;
	INT8U   uctemp;

	uitemp = get_net_wparam(NETWORK_SERVICE_FLAG);
	flag[0] = OnOff(uitemp&0x01); // Time Sync B0
	flag[1] = OnOff(uitemp&0x02); // PLU Delete B1
	flag[2] = OnOff(uitemp&0x04); // D/C Compare B2
	flag[3] = OnOff(uitemp&0x08); // Com Error
	flag[4] = OnOff(uitemp&0x10); // Continuous Sale
	flag[5] = OnOff(uitemp&0x20); // PLU Send
	flag[6] = OnOff(uitemp&0x40); // Remote PLU Call
	uctemp = get_net_bparam(NETWORK_SERVICE_FLAG2);
	flag[7] = uctemp;
#ifdef USE_REMOTE_FUNCTION
	flag[8] = OnOff(uitemp & 0x80);
#endif    

	Menu_Init();
	caption_split_by_code(0x1920, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x3921, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &flag[0], NULL, NULL, NULL);

	caption_split_by_code(0x3922, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &flag[1], NULL, NULL, NULL);
	caption_split_by_code(0x3923, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &flag[2], NULL, NULL, NULL);
#ifndef USE_SINGLE_LINE_GRAPHIC
	caption_split_by_code(0x3924, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &flag[3], NULL, NULL, NULL);
	caption_split_by_code(0x3925, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(5, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
						cap.input_max, cap.input_min, 0, &flag[4], NULL, NULL, NULL);
#endif
	caption_split_by_code(0x3926, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(6, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
		cap.input_max, cap.input_min, 0, &flag[5], NULL, NULL, NULL);
	caption_split_by_code(0x3927, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(7, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
		cap.input_max, cap.input_min, 0, &flag[6], NULL, NULL, NULL);
	if (caption_split_by_code(0x3928, &cap,0)) {
		caption_adjust(&cap,NULL);
		Menu_InputCreate(8, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &flag[7], NULL, NULL, NULL);
#ifdef USE_REMOTE_FUNCTION
		caption_split_by_code(0x3929, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(9, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &flag[8], NULL, NULL, NULL);
#endif
	} else {
		flag[7] = 0;
#ifdef USE_REMOTE_FUNCTION
		caption_split_by_code(0x3929, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(8, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
			cap.input_max, cap.input_min, 0, &flag[8], NULL, NULL, NULL);
#endif
	}
	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
	        mask=0x01;
		for (i=0; i<7; i++) {
			if (flag[i]) {
				uitemp|=mask;
			} else {
				uitemp&=~mask;
			}
			mask<<=1;
		}
#ifdef USE_REMOTE_FUNCTION
		if (flag[8]) {
			uitemp |= 0x80;
		} else {
			uitemp &= ~0x80;
		}
		network_status.remote_control = 0;
		if (flag[8]) network_status.remote_control = 1; // Remote Control
#endif            
		set_net_wparam(NETWORK_SERVICE_FLAG, uitemp);
		operation_load();
		network_status.allow_sale=0;
		network_status.remote_plu=0;
		if (flag[4]) network_status.allow_sale=1; // Continuouse Sale
		if (flag[6]) network_status.remote_plu=1; // Remote PLU Call
		network_status.service_flag=uitemp;
		uctemp=(INT8U)flag[7];
		//set_net_wparam(NETWORK_SERVICE_FLAG2, uctemp);
		set_net_bparam(NETWORK_SERVICE_FLAG2, uctemp);	// Modified by CJK 20060519
		network_status.service_bflag2=uctemp;
		if (flag[0]) ethernet_list.timesync=0; // time sync
	}
}
