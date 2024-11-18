/*****************************************************************************
|*			
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	cal.c
|*  Version			:	0.1
|*  Programmer(s)	:	
|*  Created			:	2003/03/20
|*  Modified			:	2004/02/12
|*  Description		:	CLP Cal mode routine    
|*				
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/kmem.h>
#include "globals.h"

#include "adm.h"
#include "adm_app.h"

#include "commbuf.h"
#include "prtdrv.h"
#include "timer.h"
#include "serial.h"
#include "flash.h"
#include "flash_app.h"
#include "sensor.h"
#include "ram.h"
#include "prt_render.h"
#include "bar_common.h"
#include "prt_font.h"
#include "dsp_common.h"
#include "prt_common.h"
#include "prt_interpreter.h"
#include "dsp_vfd7.h"
#include "dsp_app.h"
#include "initial.h"
#include "initial_glb.h"
#include "common.h"

#include "key.h"
#include "key_typedef.h"
#include "key_app.h"
#include "key_function.h"
#include "main.h"
#include "cal.h"
#include "debug.h"			  
#include "menu.h"

#include "caption_app.h"
#include "label_caption.h"
#include "global_set.h"
#include "plu_device.h"
#include "discount.h"
#include "pgm_app.h"
#include "ethernet.h"
#include "cursor.h"
#include "mode_calibration.h"
#include "mode_main.h"
#include "mode_commun.h"
#include "prt_list.h"
#include "cashdrawer.h"
#include "clerk_app.h"
#include "login.h"
#include "wlan_set_lantronix.h"
#include "department.h"
#include "license.h"
#include "sqi.h"
#ifdef COUNTRY_USA
#include "barcode_file.h"
#endif
#ifdef DISPLAY_REVISION_INFO
#include "revision_info.h"
#endif

extern void ad_set_fulltare(INT8U fulltare);	//se-hyung 20080325
INT8U Cal_flagAdmBackup;
extern WLAN_CARD_STRUCT EthCardStruct;
extern char	gmsg[];
INT8U cal_mode_plu_init;
extern INT8U adm_save_tarelimit(INT32U val, INT8U wunit);
INT8U BootVersion[4] = {0,};
INT8U BootVer = 0;

void CalRun_CalErr_Dislay(INT16U cap_code, INT8U error)
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char string_buf[50];

	BuzOnAdd(1);
	Dsp_SetPage(DSP_ERROR_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);

	Dsp_Fill_Buffer(0);
	caption_split_by_code(cap_code,&cap,0);
	sprintf(string_buf, "%s:%02x", (HUGEDATA char*)cap.form, error);

	disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
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

#if !defined(USE_SINGLE_LINE_GRAPHIC)
	caption_split_by_code(0x9120,&cap,0);
	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)&cap.form, strlen(cap.form), 0, 0); 
	hebrew_codetemp[strlen(cap.form)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
	arab_codetemp[strlen((char *)cap.form)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p,(HUGEDATA char*)&cap.form);
#endif
#endif

	Dsp_Diffuse();

	while(1) {
		if(KEY_Read()) {
			BuzOn(1);
			break;
		}
	}

	Dsp_SetPage(DSP_DEFAULT_PAGE);
	DspLoadFont1(DSP_MENU_FONT_ID);
	return;
}


#define RATIO_INPUT_UNIT 1000 // ratio 입력 시 입력 단위. 3자리 보다 큰 수에 대해 입력 가능. ex) 100.00kg 일경우 10kg~100kg 범위에서 10kg 단위로 설정 가능
INT32U CalRun_ADM_RatioSet(INT16U head_cap_code, INT32U ratio)
{
	INT32U set_val;
	INT8U result, len, i, backup_weightunit;
	CAPTION_STRUCT cap;
	INT8U PercentMax[2][10] = { 
					{ 6, 15, 30,  6, 15, 30,  6, 15, 30,  6},
					{15, 30, 60, 15, 30, 60, 15, 30, 60, 15}
				 };
	INT8U PercentLen[2][10] = { 	
					{1, 2, 2, 1, 2, 2, 1, 2, 2, 1},
					{2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
				 };
	char ad_unit_str[5], temp_str[20];
	INT8U weight_decimal_point, weight_decimal_sym;
	INT32U max_weight;
	INT32U percentMax, percentLen;

	weight_decimal_sym   = get_global_bparam(GLOBAL_DECIMAL_SIGN);
	weight_decimal_point = get_decimal_pt(2, 0);

	if(ADM_Status.CalUnit == WEIGHT_UNIT_KG) {
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)ad_unit_str, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)ad_unit_str, 4);
	}
	ad_unit_str[4] = 0;

	max_weight = ad_get_max_capa(ADM_Status.Capa, ADM_Status.CalUnit, 0);
	backup_weightunit = CurPluWeightUnit;
	CurPluWeightUnit = ADM_Status.CalUnit;
	max_weight = ad_cnv_adm2main(max_weight);
	CurPluWeightUnit = backup_weightunit;


	if (ADM_Status.Capa == ADM_CUSTOM_CAPA_ID)
	{
		percentMax = max_weight/RATIO_INPUT_UNIT;
		percentLen = ilog(max_weight/RATIO_INPUT_UNIT);
	}
	else
	{
		percentMax = (INT32U)PercentMax[ADM_Status.CalUnit][ADM_Status.Capa];
		percentLen = (INT32U)PercentLen[ADM_Status.CalUnit][ADM_Status.Capa];
	}
	set_val = percentMax;
	set_val *= (ratio/0x100);
	set_val /= (ratio%0x100);
				   
	Menu_Init();

	caption_split_by_code(head_cap_code, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x9172, &cap,0);
	cap.input_length = percentLen;
	cap.input_max = percentMax;
	cap.input_min = 1;
	caption_adjust(&cap,NULL);

	strcpy(temp_str, "");
	if (ADM_Status.Capa == ADM_CUSTOM_CAPA_ID)
	{
		for(i = 0; i < ilog(RATIO_INPUT_UNIT-1) - weight_decimal_point; i++) {
			strcat(temp_str, "0");
		}
	}
	else
	{
		for(i = 0; i < ADM_Status.Capa/3; i++) {	// 60kg, 150kg, 300kg, 600kg, 1500kg, 3000kg, 6000kg
			strcat(temp_str, "0");
		}
	}
	strcat(temp_str, " ");
	strcat(temp_str, ad_unit_str);
	strcat((char*)cap.form, temp_str);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val, NULL, NULL, NULL);

#if !defined(USE_SINGLE_LINE_GRAPHIC)
	caption_split_by_code(0x9173, &cap,0);
	format_value((HUGEDATA INT8U *)temp_str, max_weight, weight_decimal_point, weight_decimal_sym);
	len = strlen(temp_str);
	temp_str[len++] = ' ';
	temp_str[len] = 0;
	strcat(temp_str, ad_unit_str);
	strcat((char*)cap.form, temp_str);
	cap.input_dlength = 0;
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_FIX, 0, cap.input_dlength, cap.input_dlength, 
						0, 0, 0, NULL, NULL, NULL, NULL);
#endif
	
	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		ratio = set_val * 0x100 + percentMax;
		return ratio;
	} else {
		return 0;
	}
}

#define ADM_CAL_STATE_DISPLAY 1
#define ADM_CAL_STATE_WAIT_KEY 2
#define ADM_CAL_STATE_SEND_CMD 3
#define ADM_CAL_STATE_SAVE 4
#define ADM_CAL_STATE_ESC 5
#define ADM_CAL_STATE_END 6
#define ADM_CAL_STATE_WAIT_CAL 7

void CalRun_SpanCal(INT8U mode)
{
	INT8U cal_pt;
	INT8U state;
	INT32S zerobuf;
	INT32S weight;
	INT16U wait_cnt;
	INT32U cal_weight, max_weight;
	INT32U ratio;
	CAPTION_STRUCT cap;
	POINT disp_p;
	char disp_string_buf[50];
	char weight_string[8];
	char string[20];
	INT8U weight_decimal_point, weight_decimal_sym;
	INT16U cur_time;
	char ad_unit_str[5];
	INT32U backup_calzero;
	INT32U gravity;

	weight = 0;
	zerobuf = 0;
	wait_cnt = 0;
	state = ADM_CAL_STATE_DISPLAY;
	cal_pt = 0;

	ratio = 0x6464;	// 100%: ratio = numerator * 0x100 + denominator
	if(mode == CAL_SPAN_PERCENT) {
		ratio = ADM_WaitData(ADM_CMD_GET_CAL_RATIO, 0, ADM_PARAM_OFF);		// Get Percent Cal Ratio
		ratio = CalRun_ADM_RatioSet(0x8150, ratio);
		if(ratio == 0) return;		// Cancel
	}
#ifdef USE_SINGLE_LINE_GRAPHIC
	else if(mode == CAL_SPAN_NORMAL) {
		DspStruct.DirectDraw = 0;
		Dsp_Fill_Buffer(0);
		menu_display_menucode(CurMenuCode, 1);
		menu_displaytitle(0x8110,0);
		Dsp_Diffuse();
	}
#endif
	ADM_WaitData(ADM_CMD_SET_CAL_RATIO, ratio, ADM_PARAM_ON);			// Set Percent Cal Ratio

	max_weight = ad_get_max_capa(ADM_Status.Capa, ADM_Status.CalUnit, 0);
	max_weight = ad_cnv_adm2main(max_weight);

	cal_weight = max_weight *10L;
	cal_weight *= (INT32U)(ratio/0x100);
	cal_weight /= (INT32U)(ratio%0x100);
	cal_weight += 5;
	cal_weight /= 10;

	backup_calzero = ADM_WaitData(ADM_CMD_GET_CAL_ZERO, 0, ADM_PARAM_OFF);	// Get CalZero

	weight_decimal_point = get_decimal_pt(2, 0);
	weight_decimal_sym   = get_global_bparam(GLOBAL_DECIMAL_SIGN);

	if(ADM_Status.CalUnit == WEIGHT_UNIT_KG) {
			get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)ad_unit_str, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)ad_unit_str, 4);
	}
	ad_unit_str[4] = 0;

	format_value((HUGEDATA INT8U *)weight_string, cal_weight, weight_decimal_point, weight_decimal_sym);

	ADM_WaitData(ADM_CMD_SET_OUT_AD4, 0, ADM_PARAM_OFF);		// Set Linearized Ad Value(AD4)
	ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);			// Start Weight

	while(state != ADM_CAL_STATE_END)
	{
		if(KEY_Read())
		{
			switch(KeyDrv.CnvCode)
			{
				case KP_ENTER :		// ENTER
					BuzOn(1);
					cur_time = SysTimer100ms;
					wait_cnt = 0;
					state = ADM_CAL_STATE_SEND_CMD;
					break;
				case KP_ESC : 		// CANCEL
					BuzOn(1);
					state = ADM_CAL_STATE_ESC;
					break;
				case KP_ZERO :
					BuzOn(1);
					zerobuf = ADM_GetData.RecvData;
					break;
				default :
					BuzOn(2);
					break;
			}
		}

		switch(state)
		{
			case ADM_CAL_STATE_DISPLAY :	// Display State
				state = ADM_CAL_STATE_WAIT_KEY;
				Dsp_Fill_Buffer(0);
				DspLoadFont1(DSP_MENU_FONT_ID);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
				if(!cal_pt) {
					menu_displaytitle(0x9111,0);
					menu_writepage(2, 1);

					disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
					caption_split_by_code(0x9112, &cap,0);
#ifdef HEBREW_FONT
					Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
					hebrew_codetemp[strlen(cap.form)] = 0;
					disp_p.x = 202 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
					Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
					arab_codetemp[strlen((char *)cap.form)] = 0;
					disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif

					disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
					caption_split_by_code(0x9113, &cap,0);
#ifdef HEBREW_FONT
					Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
					hebrew_codetemp[strlen(cap.form)] = 0;
					disp_p.x = 202 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
					Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
					arab_codetemp[strlen((char *)cap.form)] = 0;
					disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
				} else {
					menu_displaytitle(0x9114,0);
					menu_writepage(2, 2);

					disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
					caption_split_by_code(0x9115, &cap,0);
					sprintf(string,"%s %s",weight_string, ad_unit_str);
					sprintf(disp_string_buf,(const char *)cap.form, string);
#ifdef HEBREW_FONT
					Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
					hebrew_codetemp[strlen(disp_string_buf)] = 0;
					disp_p.x = 202 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
					Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
					arab_codetemp[strlen(disp_string_buf)] = 0;
					disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p,disp_string_buf);
#endif

					disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
					caption_split_by_code(0x9113, &cap,0);
#ifdef HEBREW_FONT
					Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
					hebrew_codetemp[strlen(cap.form)] = 0;
					disp_p.x = 202 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
					Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
					arab_codetemp[strlen((char *)cap.form)] = 0;
					disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
				}
				Dsp_Diffuse();
#endif
				break;
			case ADM_CAL_STATE_WAIT_KEY :	// Wait State
				break;
			case ADM_CAL_STATE_SEND_CMD :	// Wait and Send State
				if((INT16U)(SysTimer100ms - cur_time) > 5) {
					cur_time = SysTimer100ms;
					wait_cnt++;
				}
				if(wait_cnt >= 5) {
					if(!cal_pt) {
						ADM_SendCommand(ADM_CMD_SET_CAL_ZERO, 0, ADM_PARAM_OFF);	// Calibaration CalZero
					} else {
						ADM_SendCommand(ADM_CMD_SET_CAL_SPAN, 0, ADM_PARAM_OFF);	// Calibaration CalSpan
					}
					state = ADM_CAL_STATE_WAIT_CAL;
					cur_time = SysTimer100ms;
				}
				break;
			case ADM_CAL_STATE_WAIT_CAL:
				if((INT16U)(SysTimer100ms - cur_time) > 120) {	// 시간 초과
					//BuzOn(1);
					CalRun_CalErr_Dislay(0x911f, 0xff);
					state = ADM_CAL_STATE_ESC;
				}
				break;
			case ADM_CAL_STATE_SAVE :	// Save State
				// Gravity 일치
				gravity = ADM_WaitData(ADM_CMD_GET_GRVTY_CAL, 0, ADM_PARAM_OFF);	// Get Gravity at Cal
				ADM_WaitData(ADM_CMD_SET_GRVTY_USE, gravity, ADM_PARAM_ON);		// Set Gravity at Use
				///////////////
				ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);		// Save Settings
				Cal_flagAdmBackup = 1;
				state = ADM_CAL_STATE_END;
				backup_calzero=ADM_WaitData(ADM_CMD_GET_CAL_ZERO,0, ADM_PARAM_OFF);	// Calibaration CalZero
				history_save(HISTORY_ADZERO,backup_calzero);
				backup_calzero=ADM_WaitData(ADM_CMD_GET_CAL_SPAN,0, ADM_PARAM_OFF);	// Calibaration CalZero
				history_save(HISTORY_ADSPAN,backup_calzero);
				history_save(HISTORY_ADMIDUP,0);
				history_save(HISTORY_ADMIDDN,0);
				break;
			case ADM_CAL_STATE_ESC :	// Esc
				if(cal_pt) {	// Zero Cal이 수행되었으면, 복구.
					ADM_WaitData(ADM_CMD_SET_CAL_ZERO, backup_calzero, ADM_PARAM_ON);	// Calibaration CalZero
				}
				state = ADM_CAL_STATE_END;
				break;
				
		}
		
		if(ADM_RecvProc() && state != ADM_CAL_STATE_END) {
			if(ADM_GetData.DataMode == ADM_DATAMODE_AD4 || ADM_GetData.DataMode == ADM_DATAMODE_INIT_ERR) {
				weight = ADM_GetData.RecvData;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
				display_clear(DISPLAY_WEIGHT);
#endif
				if(wait_cnt){
					sprintf(disp_string_buf,"Wait%d", 5-wait_cnt); 
					display_string(DISPLAY_WEIGHT, (INT8U*)disp_string_buf);
				} else {
					if(!cal_pt) {
						sprintf(disp_string_buf,"ULOAD");
					} else {
						sprintf(disp_string_buf,"LOAD ");
					}
					display_string(DISPLAY_WEIGHT, (INT8U*)disp_string_buf);
				}
				display_ulong(DISPLAY_UNITPRICE, weight - zerobuf, 0);
				display_ulong(DISPLAY_PRICE, weight, 0);
				VFD7_Diffuse();
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_OK) {
				if(ADM_GetData.RecvCmd == ADM_CMD_SET_CAL_ZERO) {
					ADM_WaitData(ADM_CMD_GET_CAL_ZERO, 0, ADM_PARAM_OFF);	// Get CalZero
					zerobuf = ADM_GetData.RecvData;		
					state = ADM_CAL_STATE_DISPLAY;
					cal_pt = 1;
					wait_cnt = 0;
				} else if(ADM_GetData.RecvCmd == ADM_CMD_SET_CAL_SPAN) {
					state = ADM_CAL_STATE_SAVE;
					wait_cnt = 0;
				}
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_ERROR) {
				switch(ADM_GetData.RecvData) {
					case 0x01 :	// Cal Unstable
						CalRun_CalErr_Dislay(0x911d, (INT8U)ADM_GetData.RecvData);
						state = ADM_CAL_STATE_ESC;
						break;
					case 0x07 :	// Cal Range Over
						CalRun_CalErr_Dislay(0x911e, (INT8U)ADM_GetData.RecvData);
						state = ADM_CAL_STATE_ESC;
						break;
					case 0x60 :	// A/D Conversion Error
						CalRun_CalErr_Dislay(0x911c, (INT8U)ADM_GetData.RecvData);
						state = ADM_CAL_STATE_ESC;
						break;
					default :
						CalRun_CalErr_Dislay(0x911f, (INT8U)ADM_GetData.RecvData);
						state = ADM_CAL_STATE_ESC;
						break;
				}
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_EEP_ERR) {
				CalRun_CalErr_Dislay(0x911b, 0);
				state = ADM_CAL_STATE_ESC;
			}
		}
	}
	ADM_WaitData(ADM_CMD_SET_OUT_WT, 0, ADM_PARAM_OFF);		// Set Weight Value(C0)

	display_alloff_primary_indication();
	VFD7_Diffuse();
}

void CalRun_Linearize(void) // Modified by CJK 20050927
{
	INT8U cal_pt;
	INT8U state;
	INT32S zerobuf;
	INT32S weight;
	INT16U wait_cnt;
	INT32U cal_weight, max_weight;
	INT32U ratio;
	CAPTION_STRUCT cap;
	POINT disp_p;
	char disp_string_buf[50];
	char weight_string[8];
	char string[20];
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	INT8U weight_decimal_point, weight_decimal_sym;
#endif
	INT16U cur_time;
	char ad_unit_str[5];
	INT32U backup_calzero, backup_calspan, backup_cuvzero, backup_cuvmid, backup_cuvspan, cuvspan, cuvzero, cuvmid;
	INT32U gravity;

	weight = 0;
	zerobuf = 0;
	wait_cnt = 0;
	state = 1;
	cal_pt = 0;

	ratio = ADM_WaitData(ADM_CMD_GET_CURVE_RATIO, 0, ADM_PARAM_OFF);		// Get Percent Cal Ratio
	ratio = CalRun_ADM_RatioSet(0x8160, ratio);
	if(ratio == 0) return;		// Cancel
	ADM_WaitData(ADM_CMD_SET_CURVE_RATIO, ratio, ADM_PARAM_ON);			// Set Percent Cal Ratio

	max_weight = ad_get_max_capa(ADM_Status.Capa, ADM_Status.CalUnit, 0);
	max_weight = ad_cnv_adm2main(max_weight);

	cal_weight = max_weight * 10L;
	cal_weight *= (INT32U)(ratio/0x100);
	cal_weight /= (INT32U)(ratio%0x100);
	cal_weight += 5;
	cal_weight /= 10;

#if !defined(USE_SINGLE_LINE_GRAPHIC)
	weight_decimal_point = get_decimal_pt(2, 0);
	weight_decimal_sym   = get_global_bparam(GLOBAL_DECIMAL_SIGN);
#endif

	if(ADM_Status.CalUnit == WEIGHT_UNIT_KG) {
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)ad_unit_str, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)ad_unit_str, 4);
	}
	ad_unit_str[4] = 0;

	backup_cuvzero = ADM_WaitData(ADM_CMD_GET_CURVE_ZERO, 0, ADM_PARAM_OFF);	// Get CurveZero
	backup_cuvmid  = ADM_WaitData(ADM_CMD_GET_CURVE_MID,  0, ADM_PARAM_OFF);	// Get CurveMid
	backup_cuvspan = ADM_WaitData(ADM_CMD_GET_CURVE_SPAN, 0, ADM_PARAM_OFF);	// Get CurveSpan

	backup_calzero = ADM_WaitData(ADM_CMD_GET_CAL_ZERO, 0, ADM_PARAM_OFF);		// Get CalZero
	backup_calspan = ADM_WaitData(ADM_CMD_GET_CAL_SPAN, 0, ADM_PARAM_OFF);		// Get CalSpan


	ADM_WaitData(ADM_CMD_SET_INI_ZR_FLAG, 1, ADM_PARAM_ON);	// Pass Initial Zero Check
	ADM_WaitData(ADM_CMD_SET_OUT_AD5, 0, ADM_PARAM_OFF);		// Set Raw Ad Value(AD5)
	ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);			// Start Weight

	while(state != ADM_CAL_STATE_END) {
		if(KEY_Read()) {
			switch(KeyDrv.CnvCode) {
				case KP_ENTER :		// ENTER
					BuzOn(1);
					cur_time = SysTimer100ms;
					wait_cnt = 0;
					state = ADM_CAL_STATE_SEND_CMD;
					break;
				case KP_ESC : 		// CANCEL
					BuzOn(1);
					state = ADM_CAL_STATE_ESC;
					break;
				default :
					BuzOn(2);
					break;
			}
		}

		switch(state) {
			case ADM_CAL_STATE_DISPLAY :	// Display State
				state = ADM_CAL_STATE_WAIT_KEY;
				Dsp_Fill_Buffer(0);
				DspLoadFont1(DSP_MENU_FONT_ID);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
				switch(cal_pt) {
					case 0 :
						menu_displaytitle(0x9111,0);
						menu_writepage(3, cal_pt+1);
						disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9112, &cap,0);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
						hebrew_codetemp[strlen(cap.form)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
						arab_codetemp[strlen((char *)cap.form)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
						disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9113, &cap,0);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
						hebrew_codetemp[strlen(cap.form)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
						arab_codetemp[strlen((char *)cap.form)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
						break;
					case 1 :
						menu_displaytitle(0x9117,0);
						menu_writepage(3, cal_pt+1);
						format_value((HUGEDATA INT8U *)weight_string, cal_weight, weight_decimal_point, weight_decimal_sym);
						disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9115, &cap,0);
						sprintf(string,"%s %s",weight_string, ad_unit_str);
						sprintf(disp_string_buf,(const char *)cap.form, string);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
						hebrew_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
						arab_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,disp_string_buf);
#endif

						disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9113, &cap,0);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
						hebrew_codetemp[strlen(cap.form)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
						arab_codetemp[strlen((char *)cap.form)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
						break;
					case 2 :
						menu_displaytitle(0x9114,0);
						menu_writepage(3, cal_pt+1);
						format_value((HUGEDATA INT8U *)weight_string, max_weight, weight_decimal_point, weight_decimal_sym);
						disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9115, &cap,0);
						sprintf(string,"%s %s",weight_string, ad_unit_str);
						sprintf(disp_string_buf,(const char *)cap.form, string);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
						hebrew_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
						arab_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,disp_string_buf);
#endif

						disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9113, &cap,0);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
						hebrew_codetemp[strlen(cap.form)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
						arab_codetemp[strlen((char *)cap.form)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
						break;	      					
				}
				Dsp_Diffuse();
#endif
				break;
			case ADM_CAL_STATE_WAIT_KEY :	// Wait State
				break;
			case ADM_CAL_STATE_SEND_CMD :	// Wait and Send State
				if((INT16U)(SysTimer100ms - cur_time) > 5) {
					cur_time = SysTimer100ms;
					wait_cnt++;
				}
				if(wait_cnt >= 5) {
					switch(cal_pt) {
						case 0 :
							ADM_SendCommand(ADM_CMD_SET_CURVE_ZERO, 0, ADM_PARAM_OFF);	// Cal Curve Zero
							break;
						case 1 :
							ADM_SendCommand(ADM_CMD_SET_CURVE_MID, 0, ADM_PARAM_OFF);	// Cal Curve Mid
							break;
						case 2 :
							ADM_SendCommand(ADM_CMD_SET_CURVE_SPAN, 0, ADM_PARAM_OFF);	// Cal Curve Span
							break;
					}
					state = ADM_CAL_STATE_WAIT_CAL;
					cur_time = SysTimer100ms;
				}
				break;
			case ADM_CAL_STATE_WAIT_CAL:
				if((INT16U)(SysTimer100ms - cur_time) > 120) {	// 시간 초과
					CalRun_CalErr_Dislay(0x911f, 0xff);
					state = ADM_CAL_STATE_ESC;
				}
				break;
			case ADM_CAL_STATE_SAVE :	// Save State
				// Gravity 일치
				gravity = ADM_WaitData(ADM_CMD_GET_GRVTY_CAL, 0, ADM_PARAM_OFF);	// Get Gravity at Cal
				ADM_WaitData(ADM_CMD_SET_GRVTY_USE, gravity, ADM_PARAM_ON);		// Set Gravity at Use
				///////////////
				ADM_WaitData(ADM_CMD_SET_CURVE_CALC, 0, ADM_PARAM_OFF); 	// Calc Curve Parameter
				ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);		// Save Settings
				cuvzero = ADM_WaitData(ADM_CMD_GET_CURVE_ZERO, 0, ADM_PARAM_OFF);	// Get CurveZero
				cuvmid  = ADM_WaitData(ADM_CMD_GET_CURVE_MID, 0, ADM_PARAM_OFF);	// Get CurveMid
				cuvspan = ADM_WaitData(ADM_CMD_GET_CURVE_SPAN, 0, ADM_PARAM_OFF);	// Get CurveSpan
				history_save(HISTORY_ADZERO, cuvzero);	       //se-hyung 080109
				history_save(HISTORY_ADSPAN, cuvspan);	       //se-hyung 080109
				history_save(HISTORY_ADMIDUP, 0);
				history_save(HISTORY_ADMIDDN, cuvmid);
				state = ADM_CAL_STATE_END;
				break;
			case ADM_CAL_STATE_ESC :	// Esc
				if(zerobuf) {	// Zero Cal이 수행되었으면, 복구.
					ADM_WaitData(ADM_CMD_SET_CURVE_ZERO, backup_cuvzero, ADM_PARAM_ON);	// Restore CuvZero
					ADM_WaitData(ADM_CMD_SET_CURVE_MID,  backup_cuvmid,  ADM_PARAM_ON);	// Restore CuvSpan
					ADM_WaitData(ADM_CMD_SET_CURVE_SPAN, backup_cuvspan, ADM_PARAM_ON);	// Restore CuvSpan
					ADM_WaitData(ADM_CMD_SET_CURVE_CALC, 0, ADM_PARAM_OFF); 		// Calc Curve Parameter
					ADM_WaitData(ADM_CMD_SET_CAL_ZERO, backup_calzero, ADM_PARAM_ON);	// Restore CalZero
					ADM_WaitData(ADM_CMD_SET_CAL_SPAN, backup_calspan, ADM_PARAM_ON);	// Restore CalSpan
				}
				state = ADM_CAL_STATE_END;
				break;
		}
		
		if(ADM_RecvProc()) {
			if(ADM_GetData.DataMode == ADM_DATAMODE_AD5) {
				weight = ADM_GetData.RecvData;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
				display_clear(DISPLAY_WEIGHT);
#endif
				if(wait_cnt){
					sprintf(disp_string_buf,"Wait%d", 5-wait_cnt);
					display_string(DISPLAY_WEIGHT, (INT8U*)disp_string_buf);
				} else {
					switch(cal_pt) {
						case 0 :
							sprintf(disp_string_buf,"ULOAD");
							break;
						case 1 :
							sprintf(disp_string_buf,"MID  ");
							break;
						case 2 :
							sprintf(disp_string_buf,"LOAD ");
							break;
					}
					display_string(DISPLAY_WEIGHT, (INT8U*)disp_string_buf);
				}
				display_ulong(DISPLAY_UNITPRICE, weight - zerobuf, 0);
				display_ulong(DISPLAY_PRICE, weight, 0);
				VFD7_Diffuse();
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_OK) {
				switch(ADM_GetData.RecvCmd) {
					case ADM_CMD_SET_CURVE_ZERO :
						ADM_WaitData(ADM_CMD_GET_CAL_ZERO, 0, ADM_PARAM_OFF);	// Get CalZero
						zerobuf = ADM_GetData.RecvData;		
						state = ADM_CAL_STATE_DISPLAY;
						cal_pt++;
						wait_cnt = 0;
						break;
					case ADM_CMD_SET_CURVE_MID :
						state = ADM_CAL_STATE_DISPLAY;
						cal_pt++;
						wait_cnt = 0;
						break;
					case ADM_CMD_SET_CURVE_SPAN :
						state = ADM_CAL_STATE_SAVE;
						cal_pt++;
						wait_cnt = 0;
						break;
				}
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_ERROR) {
				switch(ADM_GetData.RecvData) {
					case 0x01 :	// Cal Unstable
						CalRun_CalErr_Dislay(0x911d, (INT8U)ADM_GetData.RecvData);
						state = ADM_CAL_STATE_ESC;
						break;
					case 0x07 :	// Cal Range Over
						CalRun_CalErr_Dislay(0x911e, (INT8U)ADM_GetData.RecvData);
						state = ADM_CAL_STATE_ESC;
						break;
					case 0x60 :	// A/D Conversion Error
						CalRun_CalErr_Dislay(0x911c, (INT8U)ADM_GetData.RecvData);
						state = ADM_CAL_STATE_ESC;
						break;
					default :
						CalRun_CalErr_Dislay(0x911f, (INT8U)ADM_GetData.RecvData);
						state = ADM_CAL_STATE_ESC;
						break;
				}
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_EEP_ERR) {
				CalRun_CalErr_Dislay(0x911b, 0);
				state = ADM_CAL_STATE_ESC;
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_INIT_ERR) {
				ADM_WaitData(ADM_CMD_SET_INI_ZR_FLAG, 1, ADM_PARAM_ON);	// Pass Initial Zero Check
			}
		}
	}
	ADM_WaitData(ADM_CMD_SET_OUT_WT, 0, ADM_PARAM_OFF);		// Set Weight Value(C0)
	display_alloff_primary_indication();
	VFD7_Diffuse();
}

void CalRun_Hysteresis(void) // Modified by CJK 20050927
{
	INT8U cal_pt;
	INT8U state;
	INT32S zerobuf;
	INT32S weight;
	INT16U wait_cnt;
	INT32U cal_weight, max_weight;
	INT32U ratio;
	CAPTION_STRUCT cap;
	POINT disp_p;
	char disp_string_buf[50];
	char weight_string[8];
	char string[20];
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	INT8U weight_decimal_point, weight_decimal_sym;
#endif
	INT16U cur_time;
	char ad_unit_str[5];
	INT32U backup_hystzero, backup_hystmid_up, backup_hystmid_dn, backup_hystspan, hystzero, hystspan, hystmidup, hystmiddn;
	INT8U backup_hysten;
	INT32U backup_calzero, backup_calspan, backup_cuvzero, backup_cuvmid, backup_cuvspan;
	INT32U gravity;

	weight = 0;
	zerobuf = 0;
	wait_cnt = 0;
	state = 1;
	cal_pt = 0;

	if(ADM_Status.Version < 100) {
		BuzOn(3);
		return;
	}
	ratio = ADM_WaitData(ADM_CMD_GET_CURVE_RATIO, 0, ADM_PARAM_OFF);		// Get Percent Cal Ratio
	ratio = CalRun_ADM_RatioSet(0x8185, ratio);
	if(ratio == 0) return;		// Cancel
	ADM_WaitData(ADM_CMD_SET_CURVE_RATIO, ratio, ADM_PARAM_ON);			// Set Percent Cal Ratio

	max_weight = ad_get_max_capa(ADM_Status.Capa, ADM_Status.CalUnit, 0);
	max_weight = ad_cnv_adm2main(max_weight);

	cal_weight = max_weight * 10L;
	cal_weight *= (INT32U)(ratio/0x100);
	cal_weight /= (INT32U)(ratio%0x100);
	cal_weight += 5;
	cal_weight /= 10;

#if !defined(USE_SINGLE_LINE_GRAPHIC)
	weight_decimal_point = get_decimal_pt(2, 0);
	weight_decimal_sym   = get_global_bparam(GLOBAL_DECIMAL_SIGN);
#endif

	if(ADM_Status.CalUnit == WEIGHT_UNIT_KG) {
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)ad_unit_str, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)ad_unit_str, 4);
	}
	ad_unit_str[4] = 0;

	backup_hystzero   = ADM_WaitData(ADM_CMD_GET_HYST_ZERO, 0, ADM_PARAM_OFF);	// Get HystZero
	backup_hystmid_up = ADM_WaitData(ADM_CMD_GET_HYST_MID_UP,  0, ADM_PARAM_OFF);	// Get HystMidUp
	backup_hystmid_dn = ADM_WaitData(ADM_CMD_GET_HYST_MID_DN,  0, ADM_PARAM_OFF);	// Get HystMidDn
	backup_hystspan   = ADM_WaitData(ADM_CMD_GET_HYST_SPAN, 0, ADM_PARAM_OFF);	// Get HystSpan
	backup_hysten     = (INT8U)ADM_WaitData(ADM_CMD_GET_HYST_ENABLE, 0, ADM_PARAM_OFF); 	// Get HYST Parameter

	backup_cuvzero = ADM_WaitData(ADM_CMD_GET_CURVE_ZERO, 0, ADM_PARAM_OFF);	// Get CurveZero
	backup_cuvmid  = ADM_WaitData(ADM_CMD_GET_CURVE_MID,  0, ADM_PARAM_OFF);	// Get CurveMid
	backup_cuvspan = ADM_WaitData(ADM_CMD_GET_CURVE_SPAN, 0, ADM_PARAM_OFF);	// Get CurveSpan

	backup_calzero = ADM_WaitData(ADM_CMD_GET_CAL_ZERO, 0, ADM_PARAM_OFF);		// Get CalZero
	backup_calspan = ADM_WaitData(ADM_CMD_GET_CAL_SPAN, 0, ADM_PARAM_OFF);		// Get CalSpan

	ADM_WaitData(ADM_CMD_SET_INI_ZR_FLAG, 1, ADM_PARAM_ON);	// Pass Initial Zero Check
	ADM_WaitData(ADM_CMD_SET_OUT_AD5, 0, ADM_PARAM_OFF);		// Set Raw Ad Value(AD5)
	ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);			// Start Weight

	while(state != ADM_CAL_STATE_END) {
		if(KEY_Read()) {
			switch(KeyDrv.CnvCode) {
				case KP_ENTER :		// ENTER
					BuzOn(1);
					cur_time = SysTimer100ms;
					wait_cnt = 0;
					state = ADM_CAL_STATE_SEND_CMD;
					break;
				case KP_ESC : 		// CANCEL
					BuzOn(1);
					state = ADM_CAL_STATE_ESC;
					break;
				default :
					BuzOn(2);
					break;
			}
		}

		switch(state) {
			case ADM_CAL_STATE_DISPLAY :	// Display State
				state = ADM_CAL_STATE_WAIT_KEY;
				Dsp_Fill_Buffer(0);
				DspLoadFont1(DSP_MENU_FONT_ID);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
				switch(cal_pt) {
					case 0 :
						menu_displaytitle(0x9111,0);
						menu_writepage(4, cal_pt+1);
						disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9112, &cap,0);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
						hebrew_codetemp[strlen(cap.form)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
						arab_codetemp[strlen((char *)cap.form)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif

						disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9113, &cap,0);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
						hebrew_codetemp[strlen(cap.form)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
						arab_codetemp[strlen((char *)cap.form)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
						break;
					case 1 :
						menu_displaytitle(0x9117,0);
						menu_writepage(4, cal_pt+1);
						format_value((HUGEDATA INT8U *)weight_string, cal_weight, weight_decimal_point, weight_decimal_sym);
						disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9115, &cap,0);
						sprintf(string,"%s %s",weight_string, ad_unit_str);
						sprintf(disp_string_buf,(const char *)cap.form, string);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
						hebrew_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
						arab_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,disp_string_buf);
#endif

						disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9113, &cap,0);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
						hebrew_codetemp[strlen(cap.form)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
						arab_codetemp[strlen((char *)cap.form)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
						break;
					case 2 :
						menu_displaytitle(0x9114,0);
						menu_writepage(4, cal_pt+1);
						format_value((HUGEDATA INT8U *)weight_string, max_weight, weight_decimal_point, weight_decimal_sym);
						disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9115, &cap,0);
						sprintf(string,"%s %s",weight_string, ad_unit_str);
						sprintf(disp_string_buf,(const char *)cap.form, string);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
						hebrew_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
						arab_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,disp_string_buf);
#endif

						disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9113, &cap,0);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
						hebrew_codetemp[strlen(cap.form)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
						arab_codetemp[strlen((char *)cap.form)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
						break;
					case 3 :
						menu_displaytitle(0x9117,0);
						menu_writepage(4, cal_pt+1);
						format_value((HUGEDATA INT8U *)weight_string, cal_weight, weight_decimal_point, weight_decimal_sym);
						disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9115, &cap,0);
						sprintf(string,"%s %s",weight_string, ad_unit_str);
						sprintf(disp_string_buf,(const char *)cap.form, string);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
						hebrew_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
						arab_codetemp[strlen(disp_string_buf)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else	
						Dsp_Write_String(disp_p,disp_string_buf);
#endif

						disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
						caption_split_by_code(0x9113, &cap,0);
#ifdef HEBREW_FONT
						Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
						hebrew_codetemp[strlen(cap.form)] = 0;
						disp_p.x = 202 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
						Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
						arab_codetemp[strlen((char *)cap.form)] = 0;
						disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
						Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
						Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
						break;	      					
				}
				Dsp_Diffuse();
#endif
				break;
			case ADM_CAL_STATE_WAIT_KEY :	// Wait State
				break;
			case ADM_CAL_STATE_SEND_CMD :	// Wait and Send State
				if((INT16U)(SysTimer100ms - cur_time) > 5) {
					cur_time = SysTimer100ms;
					wait_cnt++;
				}
				if(wait_cnt >= 5) {
					switch(cal_pt) {
						case 0 :
							ADM_SendCommand(ADM_CMD_SET_HYST_ZERO, 0, ADM_PARAM_OFF);	// Cal HYST Zero
							break;
						case 1 :
							ADM_SendCommand(ADM_CMD_SET_HYST_MID_UP, 0, ADM_PARAM_OFF);	// Cal HYST Mid
							break;
						case 2 :
							ADM_SendCommand(ADM_CMD_SET_HYST_SPAN, 0, ADM_PARAM_OFF);	// Cal HYST Span
							break;
						case 3 :
							ADM_SendCommand(ADM_CMD_SET_HYST_MID_DN, 0, ADM_PARAM_OFF);	// Cal HYST Mid
							break;
					}
					state = ADM_CAL_STATE_WAIT_CAL;
					cur_time = SysTimer100ms;
				}
				break;
			case ADM_CAL_STATE_WAIT_CAL:
				if((INT16U)(SysTimer100ms - cur_time) > 120) {	// 시간 초과
					//BuzOn(1);
					CalRun_CalErr_Dislay(0x911f, 0xff);
					state = ADM_CAL_STATE_ESC;
				}
				break;
			case ADM_CAL_STATE_SAVE :	// Save State
				// Gravity 일치
				gravity = ADM_WaitData(ADM_CMD_GET_GRVTY_CAL, 0, ADM_PARAM_OFF);	// Get Gravity at Cal
				ADM_WaitData(ADM_CMD_SET_GRVTY_USE, gravity, ADM_PARAM_ON);		// Set Gravity at Use
				///////////////
				ADM_WaitData(ADM_CMD_SET_HYST_ENABLE, 1, ADM_PARAM_ON); 	// Calc HYST Parameter
				ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);		// Save Settings
				hystzero = ADM_WaitData(ADM_CMD_GET_HYST_ZERO, 0, ADM_PARAM_OFF); //se-hyung 080109
				hystspan = ADM_WaitData(ADM_CMD_GET_HYST_SPAN, 0, ADM_PARAM_OFF); //se-hyung 080109
				hystmidup = ADM_WaitData(ADM_CMD_GET_HYST_MID_UP, 0, ADM_PARAM_OFF);
				hystmiddn = ADM_WaitData(ADM_CMD_GET_HYST_MID_DN, 0, ADM_PARAM_OFF);
				history_save(HISTORY_ADZERO, hystzero);	       //se-hyung 080109
				history_save(HISTORY_ADSPAN, hystspan);	       //se-hyung 080109
				history_save(HISTORY_ADMIDUP, hystmidup);
				history_save(HISTORY_ADMIDDN, hystmiddn);
				state = ADM_CAL_STATE_END;
				break;
			case ADM_CAL_STATE_ESC :	// Esc
				if(zerobuf) {	// Zero Cal이 수행되었으면, 복구.
					ADM_WaitData(ADM_CMD_SET_HYST_ZERO, backup_hystzero, ADM_PARAM_ON);	// Restore CalZero
					ADM_WaitData(ADM_CMD_SET_HYST_MID_UP,  backup_hystmid_up,  ADM_PARAM_ON);	// Restore CalSpan
					ADM_WaitData(ADM_CMD_SET_HYST_MID_DN,  backup_hystmid_dn,  ADM_PARAM_ON);	// Restore CalSpan
					ADM_WaitData(ADM_CMD_SET_HYST_SPAN, backup_hystspan, ADM_PARAM_ON);	// Restore CalSpan
					ADM_WaitData(ADM_CMD_SET_HYST_ENABLE, (INT32S)backup_hysten, ADM_PARAM_ON); 	// Calc HYST Parameter
					ADM_WaitData(ADM_CMD_SET_CURVE_ZERO, backup_cuvzero, ADM_PARAM_ON);	// Restore CuvZero
					ADM_WaitData(ADM_CMD_SET_CURVE_MID,  backup_cuvmid,  ADM_PARAM_ON);	// Restore CuvSpan
					ADM_WaitData(ADM_CMD_SET_CURVE_SPAN, backup_cuvspan, ADM_PARAM_ON);	// Restore CuvSpan
					ADM_WaitData(ADM_CMD_SET_CURVE_CALC, 0, ADM_PARAM_OFF); 		// Calc Curve Parameter
					ADM_WaitData(ADM_CMD_SET_CAL_ZERO, backup_calzero, ADM_PARAM_ON);	// Restore CalZero
					ADM_WaitData(ADM_CMD_SET_CAL_SPAN, backup_calspan, ADM_PARAM_ON);	// Restore CalSpan
				}
				state = ADM_CAL_STATE_END;
				break;
		}
		
		if(ADM_RecvProc()) {
			if(ADM_GetData.DataMode == ADM_DATAMODE_AD5) {
				weight = ADM_GetData.RecvData;
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
				display_clear(DISPLAY_WEIGHT);
#endif
				if(wait_cnt){
					sprintf(disp_string_buf,"Wait%d", 5-wait_cnt);
					display_string(DISPLAY_WEIGHT, (INT8U*)disp_string_buf);
				} else {
					switch(cal_pt) {
						case 0 :
							sprintf(disp_string_buf,"ULOAD   ");
							break;
						case 1 :
							sprintf(disp_string_buf,"MID UP  ");
							break;
						case 2 :
							sprintf(disp_string_buf,"LOAD    ");
							break;
						case 3 :
							sprintf(disp_string_buf,"MID DOWN");
							break;
					}
					display_string(DISPLAY_WEIGHT, (INT8U*)disp_string_buf);
				}
				display_ulong(DISPLAY_UNITPRICE, weight - zerobuf, 0);
				display_ulong(DISPLAY_PRICE, weight, 0);
				VFD7_Diffuse();
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_OK) {
				switch(ADM_GetData.RecvCmd) {
					case ADM_CMD_SET_HYST_ZERO :
						ADM_WaitData(ADM_CMD_GET_CAL_ZERO, 0, ADM_PARAM_OFF);	// Get CalZero
						zerobuf = ADM_GetData.RecvData;		
						state = ADM_CAL_STATE_DISPLAY;
						cal_pt++;
						wait_cnt = 0;
						break;
					case ADM_CMD_SET_HYST_MID_UP :
						state = ADM_CAL_STATE_DISPLAY;
						cal_pt++;
						wait_cnt = 0;
						break;
					case ADM_CMD_SET_HYST_SPAN :
						state = ADM_CAL_STATE_DISPLAY;
						cal_pt++;
						wait_cnt = 0;
						break;
					case ADM_CMD_SET_HYST_MID_DN :
						state = ADM_CAL_STATE_SAVE;
						cal_pt++;
						wait_cnt = 0;
						break;
				}
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_ERROR) {
				switch(ADM_GetData.RecvData) {
					case 0x01 :	// Cal Unstable
						CalRun_CalErr_Dislay(0x911d, (INT8U)ADM_GetData.RecvData);	//"Cal Error - Unstable"
						state = ADM_CAL_STATE_ESC;
						break;
					case 0x07 :	// Cal Range Over
						CalRun_CalErr_Dislay(0x911e, (INT8U)ADM_GetData.RecvData);	//"Cal Error - Range Over"
						state = ADM_CAL_STATE_ESC;
						break;
					case 0x60 :	// A/D Conversion Error
						CalRun_CalErr_Dislay(0x911c, (INT8U)ADM_GetData.RecvData);	//"A/D - Conversion Error"
						state = ADM_CAL_STATE_ESC;
						break;
					default :
						CalRun_CalErr_Dislay(0x911f, (INT8U)ADM_GetData.RecvData);	//"Cal Error - Wrong ADM"
						state = ADM_CAL_STATE_ESC;
						break;
				}
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_EEP_ERR) {
				CalRun_CalErr_Dislay(0x911b, 0);
				state = ADM_CAL_STATE_ESC;
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_INIT_ERR) {
				ADM_WaitData(ADM_CMD_SET_INI_ZR_FLAG, 1, ADM_PARAM_ON);	// Pass Initial Zero Check
			}
		}
	}
	ADM_WaitData(ADM_CMD_SET_OUT_WT, 0, ADM_PARAM_OFF);		// Set Weight Value(C0)
	display_alloff_primary_indication();
	VFD7_Diffuse();
}


void CalRun_Gravity(void)
{
	INT32U set_val[2];
	INT8U result;
	CAPTION_STRUCT cap;

	set_val[0] = 90000 + ADM_WaitData(ADM_CMD_GET_GRVTY_CAL, 0, ADM_PARAM_OFF);		// Get Gravity at Cal
	set_val[1] = 90000 + ADM_WaitData(ADM_CMD_GET_GRVTY_USE, 0, ADM_PARAM_OFF);		// Get Gravity at Use

	
	Menu_Init();

	caption_split_by_code(0x8140, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x9132, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 4, &set_val[0], NULL, NULL, NULL);


	caption_split_by_code(0x9133, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 4, &set_val[1], NULL, NULL, NULL);

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		ADM_WaitData(ADM_CMD_SET_GRVTY_CAL, set_val[0]-90000, ADM_PARAM_ON);		// Set Gravity at Cal
		ADM_WaitData(ADM_CMD_SET_GRVTY_USE, set_val[1]-90000, ADM_PARAM_ON);		// Set Gravity at Cal
		ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);			// Save Settings
		Cal_flagAdmBackup = 1;
		ADM_ChangeWordFlash(GLOBAL_AD_GRAVITY_FACTORY, set_val[0]-90000);
		ADM_ChangeWordFlash(GLOBAL_AD_GRAVITY_LOCAL,   set_val[1]-90000);
	}
}


#define M_ID_USEUNIT	1
#define M_ID_CAPA	2
#define M_ID_INTERVAL	3
#define M_ID_CALUNIT	4
#define M_ID_CUSTOM_MAX	5
#define M_ID_CUSTOM_D	6
#define M_ID_DECIMAL    7

void CalRun_DispCapa_callback(long val)
{
	CAPTION_STRUCT cap;
	POINT disp_p;
	char string_buf[32];
	INT8U len, i;
	char backup_directdraw;
	INT8U unit_id;
			//Capa: 0  1   2   3   4    5    6    7     8     9     10
	INT16U disp_cap[11] = { 6, 15, 30, 60, 150, 300, 600, 1500, 3000, 6000, 15000};

	unit_id = Menu_GetMenuId(1);

	backup_directdraw = DspStruct.DirectDraw;
	caption_split_by_code(0x9144, &cap,0);
	caption_adjust(&cap,NULL);
	disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, (cap.input_x+cap.input_dlength+2)*Startup.menu_xgap); 
	if(val < 0 || val > ADM_MAX_CAPA_ID) {
		strcpy(string_buf, global_message[MSG_INVALID_VALUE]);
	} else if(val == ADM_MAX_CAPA_ID) {
		strcpy(string_buf, "CUSTOM CAPA");
	} else {
		if(*Menu[unit_id].pval == 1) { // Capa LB
			val += 1;
			sprintf(string_buf, "%dlb", disp_cap[val]);
		} else {
			sprintf(string_buf, "%dkg", disp_cap[val]);
		}
	}
	len = strlen((char *)string_buf);
	for(i = len; i < 31; i++) {
		string_buf[i] = 0x20;
	}
	string_buf[31] = 0x00;
	
	DspStruct.DirectDraw = 1;
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
	Dsp_Write_String(disp_p, string_buf);
#endif
	DspStruct.DirectDraw = backup_directdraw;

}


void CalRun_CustomCapa_callback(long val)
{
	INT8U custom_max_id, custom_d_id, decimal_id;
	CAPTION_STRUCT cap;

	custom_max_id  = Menu_GetMenuId(M_ID_CUSTOM_MAX);
	custom_d_id = Menu_GetMenuId(M_ID_CUSTOM_D);
	decimal_id = Menu_GetMenuId(M_ID_DECIMAL);
	if(custom_max_id == 0xff || custom_d_id == 0xff) return;

	if (val == ADM_CUSTOM_CAPA_ID)
	{
		Menu[custom_max_id].inhibit = OFF;	// Custom max
		Menu[custom_d_id].inhibit = OFF;	// Custom d
		Menu[decimal_id].inhibit = OFF;		// decimal
	}
	else
	{
		Menu[custom_max_id].inhibit = ON;	// Custom max
		Menu[custom_d_id].inhibit = ON;		// Custom d
		Menu[decimal_id].inhibit = ON;		// decimal
	}
	MenuRefresh = ON;
}

void CalRun_CapaUnit(void)
{
	INT32U set_val[4];
	INT8U result;
	CAPTION_STRUCT cap;
	CAPTION_STRUCT cap_sub;
	INT8U tempbyte,temp_wunit;
	INT8U len;
	INT8U temp; //se-hyung 20080325
	INT8U wtDec;
	INT32U custom_capa;
	INT32U custom_d;
	INT32U decimal;
	
	set_val[0] = ADM_WaitData(ADM_CMD_GET_W_UNIT_CAL, 0, ADM_PARAM_OFF);
	set_val[1] = ADM_WaitData(ADM_CMD_GET_W_UNIT_USE, 0, ADM_PARAM_OFF);
	set_val[2] = ADM_WaitData(ADM_CMD_GET_CAPACITY, 0, ADM_PARAM_OFF);			// Get Capa
	set_val[3] = ADM_WaitData(ADM_CMD_GET_DUAL, 0, ADM_PARAM_OFF);				// Get Dual
	if (ADM_Status.Version >= ADM_CUSTOM_CAPA_VER) {// CL5200J 기준
		custom_capa = ADM_WaitData(ADM_CMD_GET_MAX_4_CUST, 0, ADM_PARAM_OFF);		// Get Custom Max
		custom_d    = ADM_WaitData(ADM_CMD_GET_D1_4_CUST, 0, ADM_PARAM_OFF);		// Get Custom d
		decimal     = get_global_bparam(GLOBAL_DISPLAY_WEIGHTDECIMAL);
	}
	
	if (set_val[1]>WEIGHT_UNIT_G) set_val[1]=WEIGHT_UNIT_KG;
	if (set_val[1]==WEIGHT_UNIT_KG) {
		len=get_base_bparam(FLASH_ADC_WUNIT);
		if (len==WEIGHT_UNIT_G) set_val[1]=WEIGHT_UNIT_G;
	}
	Menu_Init();

	caption_split_by_code(0x8130, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x9143, &cap,0);
	caption_adjust(&cap,NULL);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	len = strlen((char *)cap.form);
	cap.form[len++] = ' ';
	cap.form[len] = 0;
	caption_split_by_code(0x9150, &cap_sub,0);
	strcat((char *)cap.form,(char *)cap_sub.form);
#endif
	Menu_InputCreate(M_ID_USEUNIT, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);

	caption_split_by_code(0x9144, &cap,0);
	if(ADM_Status.Version >= ADM_CUSTOM_CAPA_VER) {// CL5200J 기준
		cap.input_length = 2;
		cap.input_max = ADM_CUSTOM_CAPA_ID;
	}
	caption_adjust(&cap,NULL);
	Menu_InputCreate(M_ID_CAPA, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[2], NULL, NULL, CalRun_DispCapa_callback);
	if (ADM_Status.Version >= ADM_CUSTOM_CAPA_VER) {// CL5200J 기준
		Menu_AddEndFunction(M_ID_CAPA, CalRun_CustomCapa_callback);
	}

	caption_split_by_code(0x9145, &cap,0);
	caption_adjust(&cap,NULL);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	len = strlen((char *)cap.form);
	cap.form[len++] = ' ';
	cap.form[len] = 0;
	caption_split_by_code(0x914c, &cap_sub,0);
	strcat((char *)cap.form,(char *)cap_sub.form);
#endif
	Menu_InputCreate(M_ID_INTERVAL, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[3], NULL, NULL, NULL);

	caption_split_by_code(0x9142, &cap,0);
	caption_adjust(&cap,NULL);
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	len = strlen((char *)cap.form);
	cap.form[len++] = ' ';
	cap.form[len] = 0;
	caption_split_by_code(0x9151, &cap_sub,0);
	strcat((char *)cap.form,(char *)cap_sub.form);
#endif
	Menu_InputCreate(M_ID_CALUNIT, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);

	if(ADM_Status.Version >= ADM_CUSTOM_CAPA_VER) {	// CL5200J 기준
		caption_split_by_code(0x9142, &cap,0); // custom capa.
		sprintf(cap.form, "Cus capa");
		cap.input_length = 5;
		cap.input_max = 99999;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_CUSTOM_MAX, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
							cap.input_max, cap.input_min, 0, &custom_capa, NULL, NULL, NULL);

		caption_split_by_code(0x9142, &cap,0); // custom d
		sprintf(cap.form, "Cus d");
		cap.input_length = 5;
		cap.input_max = 99999;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_CUSTOM_D, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
							cap.input_max, cap.input_min, 0, &custom_d, NULL, NULL, NULL);

		caption_split_by_code(0x9142, &cap,0); // decimal
		sprintf(cap.form, "Dec");
		cap.input_length = 1;
		cap.input_min = 0;
		cap.input_max = 6;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(M_ID_DECIMAL, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
							cap.input_max, cap.input_min, 0, &decimal, NULL, NULL, NULL);
	}

	while(1) {
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) {
			if(set_val[1] == WEIGHT_UNIT_G && set_val[2] >= 3) {	// 60kg 이상이고 g이면
				BuzOn(2);
				continue;
			}
			temp_wunit = (INT8U)set_val[1];
			set_base_bparam(FLASH_ADC_WUNIT, temp_wunit);
			ADM_WaitData(ADM_CMD_SET_W_UNIT_CAL, set_val[0], ADM_PARAM_ON);		// Get CalUnit
			if(set_val[1] == WEIGHT_UNIT_G) {
				set_val[1] = WEIGHT_UNIT_KG;	// gram(2) 일때, kg(0)으로 변경(ADM에는 2가 존재하지 않음)
			}
			ADM_WaitData(ADM_CMD_SET_W_UNIT_USE, set_val[1], ADM_PARAM_ON);		// Get UseUnit
			if(ADM_Status.Version >= ADM_CUSTOM_CAPA_VER) {// CL5200J 기준
				// capa가 10일 경우 max, d1이 capa보다 먼저 설정되어야 함
				ADM_WaitData(ADM_CMD_SET_MAX_4_CUST, custom_capa, ADM_PARAM_ON);	// Get Custom Max
				ADM_WaitData(ADM_CMD_SET_D1_4_CUST, custom_d, ADM_PARAM_ON);		// Get Custom d
				//////
			}
			ADM_WaitData(ADM_CMD_SET_CAPACITY, set_val[2], ADM_PARAM_ON);		// Get Capa
			ADM_WaitData(ADM_CMD_SET_DUAL, set_val[3], ADM_PARAM_ON);			// Get Capa
			ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);				// Save Settings
			Cal_flagAdmBackup = 1;
			tempbyte = Flash_bread(DFLASH_BASE + FLASH_ADC_AREA + GLOBAL_AD_MIX_1);
			tempbyte &= 0x3f;
			if(set_val[0]) tempbyte |= 0x40;
			if(set_val[1]) tempbyte |= 0x80;
			ADM_ChangeByteFlash(GLOBAL_AD_MIX_1, tempbyte);

			tempbyte = Flash_bread(DFLASH_BASE + FLASH_ADC_AREA + GLOBAL_AD_MIX_2);
			tempbyte &= 0xe0;
			tempbyte |= (INT8U)(set_val[2]&0x0000000f);
			if(set_val[3]) tempbyte |= 0x10;
			ADM_ChangeByteFlash(GLOBAL_AD_MIX_2, tempbyte);
			ADM_Status.CalUnit = set_val[0];
			ADM_Status.UseUnit = set_val[1];
			ADM_GetData.CurUnit = ADM_Status.UseUnit;
			CurPluWeightUnit = ADM_GetData.CurUnit;	// Added by CJK 20060221
			ADM_Status.Capa    = set_val[2];
			ADM_Status.Dual    = set_val[3];

			if(ADM_Status.Version >= ADM_CUSTOM_CAPA_VER) {// CL5200J 기준
				ADM_Status.Max1[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_MAX1_KG, ADM_PARAM_ON)%1000000L;
				ADM_Status.Max2[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_MAX2_KG, ADM_PARAM_ON)%1000000L;
				ADM_Status.d1[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_D1_KG, ADM_PARAM_ON)%1000000L;
				ADM_Status.d2[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_D2_KG, ADM_PARAM_ON)%1000000L;

				ADM_Status.Max1[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_MAX1_LB, ADM_PARAM_ON)%1000000L;
				ADM_Status.Max2[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_MAX2_LB, ADM_PARAM_ON)%1000000L;
				ADM_Status.d1[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_D1_LB, ADM_PARAM_ON)%1000000L;
				ADM_Status.d2[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_D2_LB, ADM_PARAM_ON)%1000000L;
			} else {
			}

			history_save(HISTORY_CAPA,ad_get_max_capa(set_val[2],ADM_Status.UseUnit,0));

			if (ADM_Status.Capa == ADM_CUSTOM_CAPA_ID)
			{
				wtDec = decimal;
			}
			else
			{
				wtDec = get_weight_decimal(ADM_Status.Capa, ADM_Status.Dual, temp_wunit);
			}
			change_weight_decimal(wtDec);
			save_weight_decimal(wtDec);

			temp = get_base_bparam(FLASH_ADC_USE_FULLTARE);	  //se-hyung 20080325
			temp = temp&0x01;
			ad_set_fulltare(temp);
			break;
		} else {
			break;
		}
	}
}

#ifdef USE_DEBUG_PRINT_AD_VALUE
extern INT8U ad_test_print_list(long weight, long zerobuf, char *prev_str);
#endif

void CalRun_TestAD(void)
{
	INT8U loop;
	INT8U flagDisplay, w_mode;
	INT32S zerobuf;
	INT32S weight;
	CAPTION_STRUCT cap;
	POINT disp_p;
	char disp_string_buf[50];
	INT8U first;
	INT8U errcode, prev_errcode;
	INT8U backup_cur_unit, PrevCurUnit, backup_hyst_on;
	INT8U state = 0;
	char string_buf[50];
#ifdef USE_DEBUG_PRINT_AD_VALUE
	INT16U prev_print_100ms, cur_100ms;
	INT8U test_state = 0;
#endif
	INT8U backup_directdraw;

	backup_directdraw = DspStruct.DirectDraw;

	first = 1;
	zerobuf = 0;
	weight = 0;
	errcode = 0;
	
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();

	backup_cur_unit = (INT8U)ADM_WaitData(ADM_CMD_GET_W_UNIT_CUR, 0, ADM_PARAM_OFF);
	ADM_WaitData(ADM_CMD_SET_OUT_AD2, 0, ADM_PARAM_OFF);		// Set Normalized A/D Value(AD2)
	if(ADM_Status.Version >= 99) {	// Hystereis 사용
		backup_hyst_on = (INT8U)ADM_WaitData(ADM_CMD_GET_HYST_ENABLE, 0, ADM_PARAM_OFF);
		ADM_WaitData(ADM_CMD_SET_ZERO, 0, ADM_PARAM_OFF);
	}
	w_mode = 2;
	ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);		// Start Weight

	loop = TRUE;
	flagDisplay = 1;

	Dsp_Fill_Buffer(0);
	DspLoadFont1(DSP_MENU_FONT_ID);

	menu_display_menucode(CurMenuCode, 1);
	menu_displaytitle(0x8520,0);
	menu_writepage(1, 1);  
	
	while(loop)
	{
		if(flagDisplay) {
			flagDisplay = 0;
			DspStruct.DirectDraw = 1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			disp_p = point(Startup.menu_type*Startup.menu_ygap, 0);
#else
			disp_p = point(0, 0);
#endif
			memset((INT8U *)disp_string_buf, ' ', 32);
			disp_string_buf[32] = 0;
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
			hebrew_codetemp[strlen(disp_string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
			arab_codetemp[strlen(disp_string_buf)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)disp_string_buf);
#endif

			switch(w_mode) {
				case 0 :
					caption_split_by_code(0x9192, &cap,0);	
					if(ADM_GetData.CurUnit == WEIGHT_UNIT_KG) {
						strcat((char*)cap.form, "(kg)");
					} else if(ADM_GetData.CurUnit == WEIGHT_UNIT_G) {
						strcat((char*)cap.form, "(g)");
					} else {
						strcat((char*)cap.form, "(lb)");
					}
					break;
				case 1 :
					caption_split_by_code(0x9193, &cap,0);
					break;
				case 2 :
					caption_split_by_code(0x9194, &cap,0);
					break;
				case 3 :
					caption_split_by_code(0x9195, &cap,0);
					break;
				case 4 :
					caption_split_by_code(0x9196, &cap,0);
					break;
				case 5 :
					caption_split_by_code(0x9197, &cap,0);
					break;
			}
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
			hebrew_codetemp[strlen(cap.form)] = 0;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
			arab_codetemp[strlen((char *)cap.form)] = 0;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
			DspStruct.DirectDraw = 0;
		}
		if(errcode != prev_errcode) {
			DspStruct.DirectDraw = 1;
			DspLoadFont1(DSP_MENU_FONT_ID);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, Startup.menu_xgap);
#else
			disp_p = point(0, 0);
#endif
			memset((INT8U *)disp_string_buf, ' ', 32);
			disp_string_buf[32] = 0;
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
			hebrew_codetemp[strlen(disp_string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
			arab_codetemp[strlen(disp_string_buf)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)disp_string_buf);
#endif

			switch(errcode) {
				case 0 :
					cap.form[0] = 0;
					break;
				case 0xf1 :	// A/D Conversion Error
					caption_split_by_code(0x911c, &cap,0);
					break;
				case 0xf2 :	// EEPROM Error
					caption_split_by_code(0x911b, &cap,0);
					break;
				case 0xf3 :	// Init Status Error
					caption_split_by_code(0x911a, &cap,0);
					break;
				default :
					caption_split_by_code(0x9125, &cap,0);
					sprintf(disp_string_buf, "(0x%02x)", (INT8U)ADM_GetData.RecvData);
					strcat(cap.form, disp_string_buf);
					break;
			}
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
			hebrew_codetemp[strlen(cap.form)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
			arab_codetemp[strlen((char *)cap.form)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
			DspStruct.DirectDraw = 0;
		}

		if(KEY_Read())
		{
			if(KeyDrv.PressedModeKey) {
				switch(KeyDrv.CnvCode) {
					case KEY_NUM_7 :
						BuzOn(1);
						if(state == 0) state = 1;
						else state = 0;
						break;
					case KEY_NUM_5 :
						BuzOn(1);
						if(state == 1) state = 2;
						else state = 0;
						break;
					case KEY_NUM_3 :
						BuzOn(1);
						if(state == 2) state = 3;
						else state = 0;
						break;
					case KP_ENTER :
						BuzOn(1);
						if(state == 3) {
						#ifdef PRINT_AD_SETTINGS
							adm_env_print_list();
						#endif
						}
						state = 0;
						break;
					default :
						BuzOn(1);
						state = 0;
						break;
				}
			} else {
				state = 0;
				switch(KeyDrv.CnvCode)
				{
					case KP_ESC ://0x000c :		// CANCEL
						BuzOn(1);
						loop = FALSE;
						break;
					case KEY_NUM_0:
					case KEY_NUM_1:
					case KEY_NUM_2:
					case KEY_NUM_3:
					case KEY_NUM_4:
					case KEY_NUM_5:
						BuzOn(1);
						flagDisplay = 1;
						zerobuf = 0;
						w_mode = KeyDrv.CnvCode;
						ADM_WaitDataCnt(ADM_CMD_SET_OUT_WT + w_mode, 0, ADM_PARAM_OFF,1);		// Set Weight Mode(C0,C1,D2,D3,D4,C5)
						ADM_WaitDataCnt(ADM_CMD_START_WT, 0, ADM_PARAM_OFF,1);
						break;
					case KP_ARROW_LF:	// kg mode
						if(w_mode == 0) {
							BuzOn(1);
							flagDisplay = 1;
							zerobuf = 0;
							ADM_WaitDataCnt(ADM_CMD_SET_W_UNIT_CUR, 0, ADM_PARAM_ON, 1);
						} else {
							BuzOn(2);
						}
						break;
					case KP_ARROW_RI:	// lb mode
						if(w_mode == 0) {
							BuzOn(1);
							flagDisplay = 1;
							zerobuf = 0;
							ADM_WaitDataCnt(ADM_CMD_SET_W_UNIT_CUR, 1, ADM_PARAM_ON, 1);
						} else {
							BuzOn(2);
						}
						break;
					case KP_PAGE_UP:	// Hysteresis Off							
						if(ADM_Status.Version >= 99) {
							BuzOn(1);
							ADM_WaitDataCnt(ADM_CMD_SET_HYST_ENABLE, 1, ADM_PARAM_ON, 1);
						} else {
							BuzOn(2);
						}
						break;
					case KP_PAGE_DN:	// Hysteresis On
						if(ADM_Status.Version >= 99) {
							BuzOn(1);
							ADM_WaitDataCnt(ADM_CMD_SET_HYST_ENABLE, 0, ADM_PARAM_ON, 1);
						} else {
							BuzOn(2);
						}
						break;
				#ifdef USE_DEBUG_PRINT_AD_VALUE
					case KP_ARROW_UP:	// Start Creep Test 
						BuzOn(1);
						prev_print_100ms = SysTimer100ms - 3000;
						test_state = 1;
						break;
					case KP_ARROW_DN:	// Start Creep Test 
						BuzOn(1);
						test_state = 0;
						break;
				#endif
					case KP_FEED:
						BuzOn(1);
						keyapp_feed(OFF);
						break;
					case KP_ZERO :
						BuzOn(1);
						if(w_mode != 0) {
							zerobuf = ADM_GetData.RecvData;	 
						}
						if(ADM_Status.Version >= 99) {	// Hystereis 사용
							ADM_SendCommand(ADM_CMD_SET_ZERO, 0, ADM_PARAM_OFF);
						}
						break;						
					default :
						BuzOn(2);
						break;
				}
			}
		}

		if(ADM_RecvProc()) {
			prev_errcode = errcode;
			errcode = 0;
			if(ADM_GetData.DataMode <= ADM_DATAMODE_WEIGHT_LB) {
				weight = ADM_GetData.RecvData;
				if(first) {
					first = 0;
					zerobuf = ADM_GetData.RecvData;
				}
				if(w_mode == 0) {
					display_ulong(DISPLAY_UNITPRICE, ad_cnv_weight_value(weight-zerobuf, 1), 0);
				} else {
					display_ulong(DISPLAY_UNITPRICE, weight - zerobuf, 0);
				}

				display_ulong(DISPLAY_PRICE, weight, 0);

				VFD7_Diffuse();
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_ERROR) {
				switch(ADM_GetData.RecvData) {
					case 0x60 :	// A/D Conversion Error
						BuzOnAdd(1);
						errcode = 0xf1;
						break;
					default :
						BuzOnAdd(1);
						errcode = (INT8U)ADM_GetData.RecvData;
						break;
				}
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_INIT_ERR) {
				errcode = 0xf3;
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_EEP_ERR) {
				errcode = 0xf2;
			}
			if(ADM_GetData.CurUnit != PrevCurUnit) flagDisplay = 1;
			PrevCurUnit = ADM_GetData.CurUnit;
		}
#ifdef USE_DEBUG_PRINT_AD_VALUE
		if(test_state) {
			cur_100ms = SysTimer100ms;
			if(cur_100ms - prev_print_100ms >= 3000) {
				prev_print_100ms = cur_100ms;
				sprintf(string_buf, "");
				ad_test_print_list(weight, zerobuf, string_buf);
			}
		}
#endif
	}
	ADM_WaitData(ADM_CMD_SET_W_UNIT_CUR, (INT32S)backup_cur_unit, ADM_PARAM_ON);
	if(ADM_Status.Version >= 99) {
		ADM_WaitData(ADM_CMD_SET_HYST_ENABLE, (INT32S)backup_hyst_on, ADM_PARAM_ON);
	}
	ADM_WaitData(ADM_CMD_SET_OUT_WT, 0, ADM_PARAM_OFF);		// Set Weight Value(C0)
	display_alloff_primary_indication();
	VFD7_Diffuse();
	DspStruct.DirectDraw = backup_directdraw;
}


void CalRun_TestAD_forUser(void)
{
	INT8U loop;
	INT8U flagDisplay, w_mode;
	INT32S zerobuf;
	INT32S weight;
	CAPTION_STRUCT cap;
	POINT disp_p;
	char disp_string_buf[50];
	INT8U first;
	INT8U backup_cur_unit;
	INT8U backup_directdraw;

	backup_directdraw = DspStruct.DirectDraw;

	first = 1;
	zerobuf = 0;
	weight = 0;
	
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();

	backup_cur_unit = (INT8U)ADM_WaitData(ADM_CMD_GET_W_UNIT_CUR, 0, ADM_PARAM_OFF);
	ADM_WaitData(ADM_CMD_SET_OUT_AD2, 0, ADM_PARAM_OFF);		// Set Normalized A/D Value(AD2)
	w_mode = 2;
	ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);		// Start Weight

	loop = TRUE;
	flagDisplay = 1;

	Dsp_Fill_Buffer(0);
	
	while(loop)
	{
		if(flagDisplay)
		{
			flagDisplay = 0;
			DspStruct.DirectDraw = 1;
			DspLoadFont1(DSP_MENU_FONT_ID);

			menu_display_menucode(CurMenuCode, 1);
			menu_displaytitle(0x1862,0);
			menu_writepage(1, 1);  

#if !defined(USE_SINGLE_LINE_GRAPHIC)
			disp_p = point(Startup.menu_type*Startup.menu_ygap, 0);
			memset((INT8U *)disp_string_buf, ' ', 32);
			disp_string_buf[32] = 0;
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
			hebrew_codetemp[strlen(disp_string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
			arab_codetemp[strlen(disp_string_buf)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)disp_string_buf);
#endif

 			caption_split_by_code(0x97af, &cap,0);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
			hebrew_codetemp[strlen(cap.form)] = 0;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
			arab_codetemp[strlen((char *)cap.form)] = 0;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,(HUGEDATA char *)cap.form);
#endif
#endif
			DspStruct.DirectDraw = 0;
		}

		if(KEY_Read())
		{
			switch(KeyDrv.CnvCode)
			{
				case KP_ESC :
					BuzOn(1);
					loop = FALSE;
					break;
				case KP_ARROW_LF:	// kg mode
					if(w_mode == 0) {
						BuzOn(1);
						flagDisplay = 1;
						zerobuf = 0;
							ADM_WaitDataCnt(ADM_CMD_SET_W_UNIT_CUR, 0, ADM_PARAM_ON, 1);
					} else {
						BuzOn(2);
					}
					break;
				case KP_ARROW_RI:	// lb mode
					if(w_mode == 0) {
						BuzOn(1);
						flagDisplay = 1;
						zerobuf = 0;
						ADM_WaitDataCnt(ADM_CMD_SET_W_UNIT_CUR, 0, ADM_PARAM_ON, 1);
					} else {
						BuzOn(2);
					}
					break;
				case KP_ZERO :
					BuzOn(1);
					zerobuf = ADM_GetData.RecvData;	 
					break;
				default :
					BuzOn(2);
					break;
			}
		}

		if(ADM_RecvProc()) {
			if(ADM_GetData.DataMode <= ADM_DATAMODE_WEIGHT_LB 
				|| ADM_GetData.DataMode == ADM_DATAMODE_INIT_ERR
				|| ADM_GetData.DataMode == ADM_DATAMODE_EEP_ERR) {
				weight = ADM_GetData.RecvData;
				if(first) {
					first = 0;
					zerobuf = ADM_GetData.RecvData;
				}
				display_ulong(DISPLAY_UNITPRICE, weight - zerobuf, 0);
				display_ulong(DISPLAY_PRICE, weight, 0);
				VFD7_Diffuse();
			} else if(ADM_GetData.DataMode == ADM_DATAMODE_ERROR) {
				switch(ADM_GetData.RecvData) {
					case 0x60 :	// A/D Conversion Error
						BuzOn(1);
						CalRun_CalErr_Dislay(0x97ad, (INT8U)ADM_GetData.RecvData);
						loop = OFF;
						break;
					default :
						BuzOn(1);
						CalRun_CalErr_Dislay(0x97ae, (INT8U)ADM_GetData.RecvData);
						loop = OFF;
						break;
				}
			}
		}
	}
	ADM_WaitData(ADM_CMD_SET_W_UNIT_CUR, (INT32S)backup_cur_unit, ADM_PARAM_ON);
	ADM_WaitData(ADM_CMD_SET_OUT_WT, 0, ADM_PARAM_OFF);		// Set Weight Value(C0)
	display_alloff_primary_indication();
	VFD7_Diffuse();
	DspStruct.DirectDraw = backup_directdraw;
}

void CalRun_ZeroTare(void)
{
	INT32U set_val[10], ltemp;
	INT8U result;
	CAPTION_STRUCT cap;
	INT8U use_unit;
	char ad_unit_str[5];


	set_val[0] = ADM_WaitData(ADM_CMD_GET_INIT_ZR_R, 0, ADM_PARAM_OFF);				// Get Init Zero Range
	set_val[1] = ADM_WaitData(ADM_CMD_GET_REZERO_R, 0, ADM_PARAM_OFF);				// Get Rezero Range
	set_val[3] = ADM_WaitData(ADM_CMD_GET_OVERLD_R, 0, ADM_PARAM_OFF);				// Get Overload Range

	set_val[4] = (INT32U)ad_get_accumul_tare();
	set_val[5] = (INT32U)ad_get_subtract_tare();

	ltemp      = ADM_WaitData(ADM_CMD_GET_ETC, 0, ADM_PARAM_OFF);					// Get ETC(Successive Tare)	

	if(ltemp&0x20) set_val[6] = 1;	// Enable Gross Zero Mark
	else 	       set_val[6] = 0;

	if(ltemp&0x10) set_val[7] = 1;	// Enabel Net Zero Mark
	else 	       set_val[7] = 0;

	if(ltemp&0x08) set_val[8] = 1;	// Enable Gross Zero Tracking
	else 	       set_val[8] = 0;

	if(ltemp&0x04) set_val[9] = 1;	// Enabel Net Zero Tracking
	else 	       set_val[9] = 0;

	use_unit = adm_get_unit_weigh();

	// 이곳에선 UseUnit=CurUnit로 가정
	if(use_unit == WEIGHT_UNIT_KG) {
		get_global_sparam(GLOBAL_WEIGHT_SYMBOL_1, (INT8U *)ad_unit_str, 4);
	} else {
		get_global_sparam(GLOBAL_WEIGHT_LBSYMBOL_1, (INT8U *)ad_unit_str, 4);
	}
	ad_unit_str[4] = 0;

	Menu_Init();

	caption_split_by_code(0x8170, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x9152, &cap,0);
	caption_adjust(&cap, NULL);
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);

	caption_split_by_code(0x9153, &cap,0);
	caption_adjust(&cap, NULL);
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);

	caption_split_by_code(0x9155, &cap,0);
	caption_adjust(&cap, NULL);
	Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[3], NULL, NULL, NULL);

	caption_split_by_code(0x9157, &cap,0);
	caption_adjust(&cap, NULL);
	Menu_InputCreate(5, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[4], NULL, NULL, NULL);

	caption_split_by_code(0x9158, &cap,0);
	caption_adjust(&cap, NULL);
	Menu_InputCreate(6, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[5], NULL, NULL, NULL);

	caption_split_by_code(0x9159, &cap,0);
	caption_adjust(&cap, NULL);
	Menu_InputCreate(7, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[6], NULL, NULL, NULL);

	caption_split_by_code(0x915a, &cap,0);
	caption_adjust(&cap, NULL);
	Menu_InputCreate(8, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[7], NULL, NULL, NULL);

	caption_split_by_code(0x915b, &cap,0);
	caption_adjust(&cap, NULL);
	Menu_InputCreate(9, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[8], NULL, NULL, NULL);

	caption_split_by_code(0x915c, &cap,0);
	caption_adjust(&cap, NULL);
	Menu_InputCreate(10, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[9], NULL, NULL, NULL);

#ifdef USE_CANADA_ZERO_MARK_SETTING
	Menu_SetDisable(7, ON);
	Menu_SetDisable(8, ON);
#endif

	while(1) {
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) {
			ADM_WaitData(ADM_CMD_SET_INIT_ZR_R, set_val[0], ADM_PARAM_ON);			// Set Init Zero Range
			ADM_WaitData(ADM_CMD_SET_REZERO_R, set_val[1], ADM_PARAM_ON);			// Set Rezero Range
			ADM_WaitData(ADM_CMD_SET_OVERLD_R, set_val[3], ADM_PARAM_ON);			// Set Overload Range
			ad_set_accumul_tare((INT8U)set_val[4]);
			ad_set_subtract_tare((INT8U)set_val[5]);
			ltemp = 0xc0;	// CJK080324 : ad module은 +tare, -tare를 허용. main에서 체크하도록 수정
			if(set_val[6]) ltemp += 0x20; 
			if(set_val[7]) ltemp += 0x10;
			if(set_val[8]) ltemp += 0x08; 
			if(set_val[9]) ltemp += 0x04;
			ADM_WaitData(ADM_CMD_SET_ETC, ltemp, ADM_PARAM_ON);				// Set ETC(Successive Tare)
			ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);					// Save Settings
			Cal_flagAdmBackup = 1;
			ADM_ChangeByteFlash(GLOBAL_AD_INIT_ZERO_RANGE, (INT8U)set_val[0]);
			ADM_ChangeByteFlash(GLOBAL_AD_REZERO_RANGE, (INT8U)set_val[1]);
			ADM_ChangeByteFlash(GLOBAL_AD_OVER_RANGE, (INT8U)set_val[3]);
			ADM_ChangeByteFlash(GLOBAL_AD_TARE_ZERO, (INT8U)ltemp);
			return;
		} else if(result == MENU_RET_ESC) break;
	}
}
            
void CalRun_InitAdFilter(INT8U scale_type)
{
	adm_set_filter(scale_type, 1);//default
	set_base_bparam(FLASH_ADC_FILTER_LEVEL, 1);
}


void CalRun_InitADM(INT8U debug_mode, INT8U backup)// Modified by CJK 20050816
// debug_mode = 0 : Question(X) , Message (X)
// debug_mode = 1 : Question(O) , Message (O)
// debug_mode = 2 : Question(X) , Message (O)
{
	INT32U set_val;
	CAPTION_STRUCT cap;
	INT8U result;
	INT8U scale_type;
	INT32U ltemp;
	INT8U initZero;
	INT8U reZero;
    
 
	if(debug_mode == 1 ) {
		Menu_Init();
		caption_split_by_code(0x8183, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);

		caption_split_by_code(0x9500, &cap,0);
		caption_adjust(&cap,NULL);
		set_val=0;
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val, NULL, NULL, NULL);

		result = Menu_InputDisplay();
	} else {
		result = MENU_RET_SAVE;
		set_val = 1;
	}
	if(result == MENU_RET_SAVE) {
		if(set_val != 1) return; 
		if(debug_mode) {
			VertScrollMsg_Init(DSP_MENU_FONT_ID);
		}

		ADM_ScanBaudrate(BAUD_RATE_57600, BAUD_RATE_57600);
		ADM_WaitData(ADM_CMD_STOP_WT, 0, ADM_PARAM_OFF);

		if(debug_mode) {
			VertScrollMsg_Add("Init ADM");
			VertScrollMsg_Add("Init EEPROM");
		}

		ADM_WaitData(ADM_CMD_SET_EEP_ERASE, 83419, ADM_PARAM_ON);
		ADM_WaitData(ADM_CMD_INIT, 83419, ADM_PARAM_ON);

		if(debug_mode) {
			VertScrollMsg_Add("Restart ADM");
		}

		ADM_SendCommand(ADM_CMD_RESTART, 0, ADM_PARAM_OFF);	// Adm Restart
		wait_sleep(0x4ffff);

		if(debug_mode) {
			VertScrollMsg_Add("Setting Conf");
		}
		ADM_WaitData(ADM_CMD_SET_INIT_ZR_R, INIT_ZERO_VALUE, ADM_PARAM_ON);	//Set init zero value to ADM
		ADM_WaitData(ADM_CMD_SET_REZERO_R, REZERO_VALUE, ADM_PARAM_ON);	//Set rezero value to ADM
		initZero = INIT_ZERO_VALUE;
		reZero = REZERO_VALUE;
		ADM_ChangeByteFlash(GLOBAL_AD_INIT_ZERO_RANGE, (INT8U)initZero);	//Set init zero value to Main
		ADM_ChangeByteFlash(GLOBAL_AD_REZERO_RANGE, (INT8U)reZero );	//Set rezero value to Main
		ADM_WaitData(ADM_CMD_SET_TARESET_R, 23992, ADM_PARAM_ON);	// Set Tare Limit // Proper Tare(5.998kg)

		ltemp = ADM_WaitData(ADM_CMD_GET_ETC, 0, ADM_PARAM_OFF);		// Get ETC(Successive Tare)
		ltemp |= 0xc0;	// accumulation/subtraction tare
		ltemp &= 0xff;
		ADM_WaitData(ADM_CMD_SET_ETC, ltemp, ADM_PARAM_ON);		// Set ETC(Successive Tare)
			
		scale_type = get_global_bparam(GLOBAL_AREA_SCALETYPE);
		CalRun_InitAdFilter(scale_type);

		if(debug_mode) {
			VertScrollMsg_Add("Saving Conf");
		}

		ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);

		if(backup) {
			if(debug_mode) {
				VertScrollMsg_Add("Backup Conf");
			}
			ADM_BackupEeprom();
		}

		if(debug_mode) {
			VertScrollMsg_Add("End Init");

			VertScrollMsg_Add("Restart ADM");

		}

		ADM_SendCommand(ADM_CMD_RESTART, 0, ADM_PARAM_OFF);	// Restart
		Cal_flagAdmBackup = 1;

		if(debug_mode == 1) {
			VertScrollMsg_Add("Press Key");

			while(!KEY_Read());
			BuzOn(1);
		}
		ADM_WaitData(ADM_CMD_START_WT, 0, ADM_PARAM_OFF);  	// Start Weight
	}

	ADM_Status.Version = ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF) % 1000L;

	ADM_GetData.CurUnit = adm_get_unit_cur();
	CurPluWeightUnit = ADM_GetData.CurUnit;

	ADM_Status.CalUnit = ad_get_unit_cal();
	ADM_Status.UseUnit = ad_get_unit_weigh();
	ADM_Status.Capa    = ad_get_capa_idx();
	ADM_Status.Dual    = ad_get_multi_interval();

}

void CalRun_KeyTest(void)
{
	char string_buf[50];
	POINT disp_p;
	CAPTION_STRUCT cap;
	INT8U esc_count;
	INT8U backup_directdraw;


	esc_count = 0;
	backup_directdraw = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;

	while(1) {
		Dsp_Fill_Buffer(0);
		DspLoadFont1(DSP_MENU_FONT_ID);

#if defined(USE_SINGLE_LINE_GRAPHIC)
		menu_display_menucode(CurMenuCode, 1);
		menu_displaytitle(CurMenuCode, 0);
		menu_writepage(1, 1);

		if (status_run.run_mode==RUN_CALIBRATION)
		{
			caption_split_by_code(0x9431, &cap,0);
		}
		else
		{
			caption_split_by_code(0x9431, &cap,0);
		}
		caption_adjust(&cap, NULL);
		
		display_clear(DISPLAY_UNITPRICE);
		display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);

		display_clear(DISPLAY_PRICE);
		sprintf(string_buf, "%04X", KeyDrv.RawCode);
		display_string(DISPLAY_PRICE, (INT8U*)string_buf);
#else
#ifdef _USE_LCD32_        
		menu_display_menucode(CurMenuCode, 1);

		menu_displaytitle(0x1863,0);
		menu_writepage(1, 1);

		caption_split_by_code(0x9431, &cap,0);
		caption_adjust(&cap,"%04X");
		disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
		sprintf(string_buf, (char *)cap.form, KeyDrv.RawCode);
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
		Dsp_Write_String(disp_p, string_buf);
#endif
#endif //#ifdef _USE_LCD32_        
		if (status_run.run_mode==RUN_CALIBRATION) {
			menu_displaytitle(0x8530,0);
			menu_writepage(1, 1);

			caption_split_by_code(0x9431, &cap,0);
			caption_adjust(&cap,"%04X");
			disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
			sprintf(string_buf, (char *)cap.form, KeyDrv.RawCode);
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
			Dsp_Write_String(disp_p, string_buf);
#endif

			caption_split_by_code(0x9436, &cap,0);
			caption_adjust(&cap,"%d");
			disp_p = point((Startup.menu_type)*Startup.menu_ygap, 16*Startup.menu_xgap);
			sprintf(string_buf, (char *)cap.form, KeyDrv.PressedModeKey);
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
			Dsp_Write_String(disp_p, string_buf);
#endif

			caption_split_by_code(0x9432, &cap,0);
			caption_adjust(&cap,"%04X");
			disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
			sprintf(string_buf, (char *)cap.form, KeyDrv.CnvCode);
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
			Dsp_Write_String(disp_p, string_buf);
#endif

			caption_split_by_code(0x9433, &cap,0);
			caption_adjust(&cap,"%d");
			disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 16*Startup.menu_xgap);
			sprintf(string_buf, (char *)cap.form, KeyDrv.keytype);
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
			Dsp_Write_String(disp_p, string_buf);
#endif

			if(Startup.menu_type) {
				if(esc_count == 1) {
					caption_split_by_code(0x9434, &cap,0);
				} else if(esc_count == 2) {
					caption_split_by_code(0x9435, &cap,0);
				} else {
					cap.form[0] = 0;
				}
				disp_p = point((Startup.menu_type+2)*Startup.menu_ygap, 0*Startup.menu_xgap);
#ifdef HEBREW_FONT
				Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
				hebrew_codetemp[strlen(cap.form)] = 0;
				disp_p.x = 202 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
				Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
				arab_codetemp[strlen((char *)cap.form)] = 0;
				disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
				Dsp_Write_String(disp_p, cap.form);
#endif
			}
		} else {
			menu_displaytitle(0x1863,0);
			menu_writepage(1, 1);

			caption_split_by_code(0x9431, &cap,0);
			caption_adjust(&cap,"%04X");
			disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
			sprintf(string_buf, (char *)cap.form, KeyDrv.RawCode);
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
			Dsp_Write_String(disp_p, string_buf);
#endif
		}
#endif //#if defined(USE_SINGLE_LINE_GRAPHIC)

		Dsp_Diffuse();

		while(!KEY_Read()) {
		}
		if (status_run.run_mode==RUN_CALIBRATION) {
			switch(KeyDrv.CnvCode) {
				case KP_ESC :
				case KS_MULTI :
					BuzOn(1);
					if(esc_count == 0) {
						esc_count = 1;
					} else if(esc_count == 1) {
						KEY_SetMode(1);
						DspStruct.DirectDraw = backup_directdraw;
						return;
					} else esc_count = 0;
					break;
				case KP_ENTER :
					BuzOn(1);
					if(esc_count == 1) esc_count = 2;
					else esc_count = 0;
					break;
				case KEY_NUM_0 :
				case KEY_NUM_1 :
					BuzOn(1);
					if(esc_count == 2) KEY_SetMode(KeyDrv.CnvCode - KEY_NUM_0);
					esc_count = 0;
					break;
				default :
					BuzOn(1);
					esc_count = 0;
					break;
			}
		} else {
			switch(KeyDrv.CnvCode) {
				case KP_ESC :
				case KS_MULTI :
					BuzOn(1);
					if(esc_count == 0) {
						esc_count = 1;
					} else if(esc_count == 1) {
						KEY_SetMode(1);
						DspStruct.DirectDraw = backup_directdraw;
						return;
					} else esc_count = 0;
					break;
				default :
					BuzOn(1);
					esc_count = 0;
					break;
			}
		}
	}
}

INT8U DisableSignInPgmMode = OFF;
void CalRun_DisplayTest(void)
{
	INT8U num, one_time;
	INT8U disp_on;
	INT8U i, temp;
	INT8U bright,bright_backup;
	INT8U cc;
	char temp_buf[12];
	POINT disp_p;

	//HYP 20041007
	//Insert current bright value (in flash)
	bright = get_global_bparam(GLOBAL_DISPLAY_BRIGHT);
	if ((bright < 1) || (bright > 9)) bright = 5;
	//
	bright_backup=bright;
	one_time = 1;
	num = 0;
	disp_on = 1;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	LCD_Check(0x00, 0x00);
#else
	cc = 0xff;
	disp_p = point(0,0);
	memset((INT8U *)temp_buf, 0x20, sizeof(temp_buf));
	sprintf(temp_buf,"CONTRAST:%d", bright);
	DisplayMsg(temp_buf);
	Delay_100ms(10);
#endif
	DisableSignInPgmMode = ON; // 프로그램모드에서 sign(LINK,USBMEM) 표시 안되도록 함

	while(1) {
		if(!(SysTimer100ms%5) && disp_on) {
			if(one_time) {
				one_time = 0;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
				if(num >= 18) {
					display_allon_primary_indication();
					num = 0;
				} else {
					display_alloff_primary_indication();
					if(num < 8) {
						temp = num%8;
						for (i=0; i<DSP_MAX_DIGIT/8; i++) {
							VFD7_display_char(temp+i*8,'8');
							VFD7_display_triangle(temp+i*8, 1);
							VFD7_display_char(temp+i*8,',');
							VFD7_display_ucomma(temp+i*8, 1);
						}
					} else {
						for (i=0; i<DSP_MAX_DIGIT; i++) {
							VFD7_display_char(i,num-8+'0');
							VFD7_display_triangle(i,num%2);
							if ((num-8)%2) {
								VFD7_display_char(i,',');
								VFD7_display_ucomma(i, 1);
							}
						}
					}
					num++;
				}
#ifdef _USE_LCD32_ 
                VFD7_Diffuse();
#else                
				Dsp_Diffuse();
#endif                
				if(!(num%4)) {
				    LCD_Check(0x00, 0x00);
				    if(num == 16) ;
				    else init_lcdchess();
				} else {
				}
#else
				display_alloff_primary_indication();
				if(num < 5) {
					display_allon_primary_indication();
				} else if(num < 10) {
					temp = (num-5)%5;
					for (i=0; i<DSP_MAX_DIGIT/5; i++) {
						VFD7_display_char(temp+i*5,'8');
						VFD7_display_char(temp+i*5,',');
						VFD7_display_triangle(temp+i*5, 1);
					}
				} else {
					for (i=0; i<DSP_MAX_DIGIT; i++) {
						VFD7_display_char(i,num-10+'0');
						if ((num-10)%2) {
							VFD7_display_char(i,',');
						}
					}
					VFD7_display_triangle_all(num%2);
				}
				if(!(num%5)) {
					Dsp_Fill_Buffer(cc);
					cc ^= 0xff;
				} else {
				}
				VFD7_Diffuse();
				num++;
				num %= 20;
#endif
			}
		} else {
			one_time = 1;
		}
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_ESC) {
				VFD7_display_bright(bright_backup);
				BuzOn(1);
				break;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			} else if (KeyDrv.CnvCode <= KEY_NUM_9) {	
#else
			} else if ((KeyDrv.CnvCode >= KEY_NUM_0) && (KeyDrv.CnvCode <= KEY_NUM_9)) {
#endif
				BuzOn(1);
				num = KeyDrv.CnvCode - KEY_NUM_0 + 8;
			} else if ((KeyDrv.CnvCode >= KP_ASC_A) && (KeyDrv.CnvCode <= KP_ASC_H)) {
				BuzOn(1);
				num = KeyDrv.CnvCode - KP_ASC_A;
			} else if ((KeyDrv.CnvCode >= KP_ASC_A_L) && (KeyDrv.CnvCode <= KP_ASC_H_L)) {
				BuzOn(1);
				num = KeyDrv.CnvCode - KP_ASC_A_L;
			} else if (KeyDrv.CnvCode == KP_ENTER || KeyDrv.CnvCode == KP_SAVE) {
				//HYP 20041007
				//Insert save bright value (in flash)
				set_global_bparam(GLOBAL_DISPLAY_BRIGHT,bright);
				//
				BuzOn(1);
				break;
			} else if(KeyDrv.CnvCode == KP_ARROW_UP) {
				bright++;
				if(bright > 9) {
					BuzOn(2);
					bright = 9;
				} else {
					BuzOn(1);	
				}
				VFD7_display_bright(bright);
				display_lcd_clear();
				sprintf(temp_buf,"CONTRAST:%d", bright);
				DisplayMsg(temp_buf);
			} else if(KeyDrv.CnvCode == KP_ARROW_DN) {
				bright--;
				if(bright < 1) {
					BuzOn(2);
					bright=1;
				} else {
					BuzOn(1);
				}
				VFD7_display_bright(bright);
				display_lcd_clear();
				sprintf(temp_buf,"CONTRAST:%d", bright);
				DisplayMsg(temp_buf);
			} else if(KeyDrv.CnvCode == KP_CLEAR) {
				BuzOn(1);
				disp_on ^= 1;
			} else {
				BuzOn(2);				
			}
		}
	}
    DisableSignInPgmMode = OFF;
	display_alloff_primary_indication();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	LCD_Check(0x00, 0x00);
#else
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
#endif
}


void CalRun_CashDrawerTest(void)
{
	char string_buf[50];
#if !defined(USE_SINGLE_LINE_GRAPHIC)
	POINT disp_p;
#endif
	CAPTION_STRUCT cap;
	INT8U cash_state, prev_cash_state, backup_directdraw;
	INT8U key_pressed = 0;

	backup_directdraw = DspStruct.DirectDraw;

	cash_state = Cash_check();
	prev_cash_state = cash_state^0x01;

	while(1) {
		DspStruct.DirectDraw = 0;
		Dsp_Fill_Buffer(0);
		DspLoadFont1(DSP_MENU_FONT_ID);

		menu_display_menucode(CurMenuCode, 1);
		menu_displaytitle(0x8580,0);
		menu_writepage(1, 1);

		caption_split_by_code(0x9480, &cap,0);
		caption_adjust(&cap,NULL);
#if defined(USE_SINGLE_LINE_GRAPHIC)
		display_clear(DISPLAY_UNITPRICE);
		display_string(DISPLAY_UNITPRICE,(INT8U *)cap.form);
#else
		disp_p = point((Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen((char *)cap.form), 0, 0, 0); 
		arab_codetemp[strlen((char *)cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p, (HUGEDATA char *)cap.form);
#endif
#endif

		Dsp_Diffuse();
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		disp_p = point((Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap);
#endif
		DspStruct.DirectDraw = 1;
#ifdef CPU_PIC32MZ
		while(!KEY_Read())
		{
			cash_state = Cash_check();
			if (cash_state != prev_cash_state || key_pressed == 1) {
				key_pressed = 0;
				sprintf(string_buf, "%c", (cash_state?'O':'X'));
#if defined(USE_SINGLE_LINE_GRAPHIC)
				display_clear(DISPLAY_PRICE);
				display_string(DISPLAY_PRICE,(INT8U *)string_buf);
				VFD7_Diffuse();
#else
				disp_p = point((Startup.menu_type)*Startup.menu_ygap, cap.input_x*Startup.menu_xgap);
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
				Dsp_Write_String(disp_p, string_buf);
#endif
#endif
				prev_cash_state = cash_state;
			}
		}
#else
		while(1) 
		{
			cash_state = Cash_check();
			if(cash_state)
			{
				if (cash_state != prev_cash_state || key_pressed == 1) 
				{
					key_pressed = 0;
					sprintf(string_buf, "%c", ((cash_state-'0')?'O':'X'));
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
					Dsp_Write_String(disp_p, string_buf);
#endif
					prev_cash_state = cash_state;
				}
				if( KEY_Read() ) break;	
			}
		}
#endif		
		
		switch(KeyDrv.CnvCode) {
			case KP_TEST :
				BuzOn(1);
				Cash_open();
				key_pressed = 1;
				break;
			case KP_ESC :
				BuzOn(1);
				DspStruct.DirectDraw = backup_directdraw;
				return;
			default :
				key_pressed = 1;
				BuzOn(2);
				break;
		}
	}
}

extern void barcodefile_default_us(void);
extern void barcodefile_default_KR_trace(void);
void CalRun_TableDefault(void)	//20090508 Added by JJANG 	
{
	CAPTION_STRUCT	cap;
	INT32U pnt;
	DEPARTMENT_STRUCT dept;
	
#ifdef COUNTRY_USA
	barcodefile_default_us();
#else
	//Department 
	memset((INT8U *)&dept,0x00,sizeof(DEPARTMENT_STRUCT));
	dept._code     = 1;
	dept._speedkey = 1;
#ifdef USE_PERSIA_DEFAULT
	caption_split_by_code(0x9280, &cap,0);	//"Default"
	sprintf((char *)dept._description, cap.form);
#else
	sprintf((char *)dept._description,"Default");
#endif
	pnt  =  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	pnt +=  DEPARTMENT_STRUCT_SIZE * (dept._code-1);
	Flash_swrite(pnt, (HUGEDATA INT8U *)&dept, sizeof(dept));

	//Barcode
	if (Startup.country == COUNTRY_KR) barcodefile_default_KR_trace();
#endif
}

INT8U CalRun_ClearMemory(INT8U mode)		// mode 1:NVRam, 2:PLU 4:Table, 8:Flash All, 16:print/adm default
{
	CAPTION_STRUCT cap;
	INT16U cap_code;
	INT8U  result;
	INT32U set_val;
	INT8U buffer[HISTORY_BUF_SIZE];

	Dsp_Fill_Buffer(0);
	DspLoadFont1(DSP_MENU_FONT_ID);
	switch(mode) {
		case 0x01 : // Report
			cap_code=0x8211;
			break;
		case 0x02 : // PLU
			cap_code=0x8212;
			break;
		case 0x04 : // Table
			cap_code=0x8213;
			break;
		case 0x07 : // Report&PLU,Table
			cap_code=0x8214;
			break;
		case 0x08 :
			//cap_code=0x8214;
			cap_code=0x8215;
			break;
		default :
			return MENU_MOVE_TO_UP;
	}
	Menu_Init();
	caption_split_by_code(cap_code, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x9500, &cap,0);
	caption_adjust(&cap,NULL);
	set_val=0;
	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
				cap.input_max, cap.input_min, 0, &set_val, NULL, NULL, NULL);

	while(1) {
		result = Menu_InputDisplay();
		if(mode == 8 && result == MENU_RET_SAVE && set_val == 1) {
			status_scale.level = 0;
			if (permission_check(PMS_DEALER,0))	break;
		} else {
			break;
		}
	}

	if(result == MENU_RET_SAVE) {
		if(set_val != 1) return MENU_MOVE_TO_UP;
		Dsp_Fill_Buffer(0);
		Dsp_Diffuse();
		VertScrollMsg_Init(DSP_MENU_FONT_ID);
		if (mode&0x01) {
			init_factory_nvram(CLEAR_CAL);
			plu_init(0);	//se-hyung  080123  sort table 삭제 되는 현상 수정
		}
		if (mode&0x02) {
			VertScrollMsg_Add("init PLU");
			if (cal_mode_plu_init) plu_init(1);
			cal_mode_plu_init=0;
			VertScrollMsg_Add("Del Disc");
			discount_delete_all();
			VertScrollMsg_Add("Del All PLU");
			plu_all_delete(NULL,0);
		}
		if (mode&0x04) {
			CalRun_ClearTable();
			CalRun_TableDefault();
		}
		if (mode&0x08) {
			history_flash2ram(buffer);
			Flash_init_nvbuffer();
			if(FlashDChipErase() == FLASH_SUCCESS) {
				sprintf(gmsg, "Clear Flash");
			} else {
				sprintf(gmsg, "Flash Failed");
			}
			history_ram2flash(buffer);
			Flash_flush_nvbuffer();
			VertScrollMsg_Add(gmsg);
			
		_SOFTWARE_RESET;
		}
		if (mode&0x10) {
			VertScrollMsg_Add("Printer Init");
			PrtFlashSetDefault();
			VertScrollMsg_Add("ADM Init");
			CalRun_InitADM(0, ON);// Modified by CJK 20050816
		}
	}
	return MENU_MOVE_TO_UP;
}

void CalRun_InitPrinter(INT8U debug_mode)
{
//	char disp_string_buf[50];
	INT32U set_val;
	CAPTION_STRUCT cap;
	INT8U result;

	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
	if(debug_mode == ON) {
		Menu_Init();
		caption_split_by_code(0x8380, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);
		caption_split_by_code(0x9500, &cap,0);
		caption_adjust(&cap,NULL);
		set_val=0;
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length,
					cap.input_max, cap.input_min, 0, &set_val, NULL, NULL, NULL);

		result = Menu_InputDisplay();
	} else {
		result = MENU_RET_SAVE;
		set_val = 1;
	}
	if(result == MENU_RET_SAVE) {
		if(set_val != 1) return;
		VertScrollMsg_Init(DSP_MENU_FONT_ID);
		VertScrollMsg_Add("Printer Init");
		PrtFlashSetDefault();
	}
}

#define CHECKSUM_BOOT_START	0x1FC01000
#define CHECKSUM_BOOT_RESERVE_START  0x1FC14000
#define CHECKSUM_BOOT_RESERVE_END    0x1FC20000
#define CHECKSUM_BOOT_END	0x1FC34000
#define CHECKSUM_FW_START	0x1D000000
#define CHECKSUM_FW_END		0x1D200000

extern INT8U history_print_list(void);

//const char VERSION_STRING[]={"C51507BTCD00100"};
				// 0~1  : Model(C5:CL5200J)
				// 2~3  : Year
				// 4~5  : Month	
				// 6~8  : boot code
				// 9    : Type
				// 10~11: Sub-Version(not use)
				// 12~13: Version
				// 14:	  Dummy
INT8U GetBootVersion(INT8U *str)
{
	INT8U *ptr;
	INT32U val;
	INT16U char_cnt;
	INT8U state = 0;
	
	str[0] = 0;
	
	for (ptr = (INT8U*)PA_TO_KVA0(CHECKSUM_BOOT_START); ptr < (INT8U*)PA_TO_KVA0(CHECKSUM_BOOT_END); ptr++)
	{
		if (ptr == (INT8U*)PA_TO_KVA0(CHECKSUM_BOOT_RESERVE_START)) {
			/* boot 펌웨어 체크섬 동작 방식과 동일하게 boot 메모리 주소 영역 체크함 */
			ptr = (INT8U*)PA_TO_KVA0(CHECKSUM_BOOT_RESERVE_END);
		}

		switch(state)
		{
		case 0:
			if (*ptr == 'C') state = 1;
			break;
		case 1:
			if (*ptr == '4')        // ( C5->C4 CL5200N Model 코드로 변경 )
			{
				state = 2;
				val = 0;
				char_cnt = 0;
			}
			else state = 0;
			break;
		case 2: // made year 2 digits
			val *= 10;
			val += ctod(*ptr);
			if (++char_cnt >= 2)
			{
				if (val >= 15 && val <= 25) // year 2015 ~ 2025
				{
					state = 3; 
					val = 0;
					char_cnt = 0;
				}
				else state = 0;
			}
			break;
		case 3: // made month 2 digits
			val *= 10;
			val += ctod(*ptr);
			if (++char_cnt >= 2)
			{
				if (val >= 1 && val <= 12) // month 1 ~ 12
				{
					state = 4; 
					val = 0;
					char_cnt = 0;
				}
				else state = 0;
			}
			break;
		case 4:
			if (*ptr == 'B') state = 5;
			else state = 0;
			break;
		case 5:
			if (*ptr == 'T') state = 6;
			else state = 0;
			break;
		case 6:
			if (*ptr == 'C') state = 7;
			else state = 0;
			break;
		case 7: // type
			state = 8;
			break;
		case 8: // sub-version1
			state = 9;
			break;
		case 9: // sub-version2
			state = 10;
			break;
		case 10: // version 1
			str[0] = *ptr;
			state = 11;
			break;
		case 11: // version 2
			str[1] = *ptr;
			str[2] = 0;
			return ON;
			break;
		}
	}
	return OFF;
}

#if defined(USE_SINGLE_LINE_GRAPHIC) || defined(_USE_LCD20848_) || defined(_USE_LCD32_)
void CalRun_FirmwareVersion(void)
{
	POINT disp_p;
	INT16U i,nth;//,j;
	INT16S cur_id, page_first_id;
	char str_buf[50], str[32];
	CAPTION_STRUCT cap;
	INT32U start_id, end_id;
	INT16U version;
	char ver_str[32], des_str[20];
	char backupDirect;
	ROMDATA INT8U *cp;
	INT8U *fcp;
	static INT16U boot_checksum=0, fw_checksum=0, cap_checksum=0;
	static INT8U calc_checksum = OFF;
	INT32U k, max_cap;
	FW_VERSION fversion;
	FW_VERSION2 fversion2;
	char subversion[3];
	INT8U bootversion[4];
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	char char_w,char_h;
	INT8U start_line;
#endif
	char old_font;
	char dealer_str[4];
	char country_str[4];
	char cs_str[10];
	INT8U w_version[17];
#ifdef USE_USB_ZIG_TEST
	INT8U usb_write_ok=0;
	INT8U usb_read_ok=0;
	INT8U usb_comm_ok=0;
#endif
#ifdef USE_DATETIME_ZIG_TEST
	INT16U  year,month,day;
	INT8U rtc_Batt_Err;
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	MenuNumLinePerPage = 3;
#else
	MenuNumLinePerPage = 1;
#endif
    INT8U ip_l[4] = {10,10,4,33};
    INT8U gw[4] = {10,10,0,1};
    INT8U sn[4] = {255,0,0,0};
#ifdef USE_ADM_AUTO_RESTORE
	INT8U init_err_cnt, eep_err_cnt;
#endif

	version_get(&fversion);
	version2_get(&fversion2);

	memcpy(dealer_str, fversion2.dealer, 3);
	dealer_str[3] = 0;
	str_trim(dealer_str);
	memcpy(country_str, fversion2.country, 2);
	country_str[2] = 0;
	str_trim(country_str);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	if (country_str[0]) strcat(country_str, " ");	// country_str이 있을 때, 스페이스 추가
#endif
	backupDirect = DspStruct.DirectDraw;
	start_id = 0;
#ifdef USE_CTS_FUNCTION
  #ifdef USE_ADM_AUTO_RESTORE
	end_id = 11;
  #else
    #ifdef USE_DATETIME_ZIG_TEST
	end_id = 11;
    #else
	end_id = 8;
    #endif
  #endif
#else
  #ifdef USE_ADM_AUTO_RESTORE
	end_id = 11;
  #else
    #ifdef USE_DATETIME_ZIG_TEST
	end_id = 8;
    #else
	end_id = 7;
    #endif
  #endif
#endif

	cur_id = start_id;
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_SMALL_FONT_ID);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	char_w = display_font_get_width();
	char_h = display_font_get_height();
	start_line = 0;
	if (Startup.menu_type == 2) start_line = 1;
#endif

#ifdef USE_DATETIME_ZIG_TEST
	if(status_run.run_mode == RUN_CALIBRATION)
	{		
		RTC_CLK_Burst_Read();
		if(RTCStruct.hour==0xFF || RTCStruct.min==0xFF || RTCStruct.sec==0xFF) rtc_Batt_Err = 1;
		else rtc_Batt_Err = 0;

		if(!rtc_Batt_Err)
		{
			RTC_CLK_Burst_Read();
			if(RTCStruct.year<19 || RTCStruct.year>39)	//RTC 셋팅 안 된 경우(향후 20년 동안 문제없도록)
			{
				RTCStruct.year =hex2bcd(0x01);
				RTCStruct.month=hex2bcd(0x01);
				RTCStruct.date =hex2bcd(0x01);
				RTCStruct.hour=hex2bcd(0x01);
				RTCStruct.min=hex2bcd(0x01);
				RTCStruct.sec=hex2bcd(0x01);
				RTC_CLK_Burst_Write();
  #ifdef USE_MORNITORING_CNT
				rtc_set_pre_minute();
  #endif 
			}
		}
	}
#endif
	EthVariableInit();
	if (Startup.global_com&0x01) {
		if(EthInit())
		{
		}
		else
		{
			Startup.global_com &= ~0x01;
		}
	}
	while(1) {
		DspStruct.DirectDraw = 0;
		Dsp_Fill_Buffer(0);
		menu_display_menucode(CurMenuCode, 1);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		DspLoadFont1(DSP_SMALL_FONT_ID);

#ifdef USE_ARAB_FONT
		sprintf(ver_str, "DESC.  VERSION");
#else
  #ifdef USE_CTS_FUNCTION
		sprintf(ver_str, "DESC.      VERSION");
  #else
		sprintf(ver_str, "DESC.      VERSION (NO CTS)");
  #endif
#endif
		disp_p = point(start_line*char_h, 0);
		Dsp_Write_String(disp_p, (HUGEDATA char *)ver_str);
#else
		menu_displaytitle(0x8570,0);
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
		DspLoadFont1(DSP_6_BY_8_ENG_FONT_ID);
#endif
#endif
		nth = 0;
		page_first_id = cur_id;
		memset(ver_str,0x00,20);
		memset(des_str,0x00,20);
#if !defined(_USE_LCD20848_) && !defined(_USE_LCD32_)
		memset(cs_str,0x00,10);
#endif	// To be checked - ckkim
		while(1) {
			switch(cur_id) {
				case 0 :	// Main
					i = 0;
					if(fversion.subversion[0] != '0') subversion[i++] = fversion.subversion[0];
					subversion[i++] = fversion.subversion[1];
					subversion[i] = 0;
/*
#ifdef USE_KOREA_VERSION_DISPLAY
 #if defined(USE_CTS_FUNCTION)
  #ifdef USE_KOREA_CL5200 // 국내 CTS 버전 파생 시 -C 표시 필요
                    if (country_str[0])
                    {
                        sprintf(ver_str, "%s%c.%c%c.%s-C%s", country_str, fversion.version, fversion.reversion[0], fversion.reversion[1]
                                         , subversion, dealer_str);
                    }
                    else
                    {
                        sprintf(ver_str, "V%c.%c%c.%s-C%s", fversion.version, fversion.reversion[0], fversion.reversion[1]
                                         , subversion, dealer_str);
                    }
  #else
					if (country_str[0])
					{
						sprintf(ver_str, "%s%c.%c%c.%sC%s", country_str, fversion.version, fversion.reversion[0], fversion.reversion[1]
									     , subversion, dealer_str);
					}
					else
					{
						sprintf(ver_str, "V%c.%c%c.%sC%s", fversion.version, fversion.reversion[0], fversion.reversion[1]
									     , subversion, dealer_str);
					}
  #endif
 #else
*/
                    if (country_str[0])
                    {
                        sprintf(ver_str, "%sV%c.%c%c.%s%s", country_str, fversion.version, fversion.reversion[0], fversion.reversion[1]
                                         , subversion, dealer_str);
                    }
                    else
                    {
                        sprintf(ver_str, "V%c.%c%c.%s%s", fversion.version, fversion.reversion[0], fversion.reversion[1]
                                         , subversion, dealer_str);
                    }
// #endif
//#endif

					if (calc_checksum)
					{
#ifdef DISPLAY_REVISION_INFO
  #ifdef USE_ARAB_FONT
    #ifdef _USE_LCD32_ // 아랍폰트를 사용하는 CL5500 Display 제품
						sprintf(cs_str, "-%04X,r%04d", fw_checksum, GetRevision());
						ver_str[8]='\0';	//체크섬, 리비전 표시 시작 위치
    #else
						sprintf(cs_str, "[%04X,r%04d]", fw_checksum, GetRevision());
						ver_str[11]='\0';	//체크섬, 리비전 표시 시작 위치
    #endif
  #else
						sprintf(cs_str, "[%04X,r%04d]", fw_checksum, GetRevision());
						ver_str[11]='\0';	//체크섬, 리비전 표시 시작 위치
  #endif
#else
						sprintf(cs_str, "(%04X)", fw_checksum);
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						strcat(ver_str, cs_str);
#endif
					}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_FONT
					sprintf(des_str, "Scale");
	#else
					sprintf(des_str, "Scale Main");
	#endif
#endif	// #ifdef _USE_LCD20848_
					break;
				case 1 :	// ADM
					version = (INT16U)(ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF) % 1000);	// Version = "1023" + VER[3]
					sprintf(ver_str, "V%d.%02d", version/100, version%100);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_FONT
					sprintf(des_str, "AD");
	#else
					sprintf(des_str, "AD Module");
	#endif
#endif	// #ifdef _USE_LCD20848_
					break;
				case 2 :
					sprintf(ver_str, "N/A     ");
					if (Startup.global_com&0x01) {
						for(i=0;i<8;i++) {
							ver_str[i] = EthCardStruct.Ver[i];
						}
						ver_str[i]=0;
					}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_FONT
					sprintf(des_str, "LAN");
	#else
					sprintf(des_str, "Ethernet");
	#endif
#endif	// #ifdef _USE_LCD20848_
					break;
				case 3 :caption_split_by_code(0x0fff, &cap, 0);
					sprintf(ver_str, "%s", cap.form);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_FONT
					sprintf(des_str, "Cap.");
	#else
					sprintf(des_str, "Caption");
	#endif
#endif	// #ifdef _USE_LCD20848_
					if (calc_checksum)
					{
						sprintf(cs_str, "(%04X)", cap_checksum);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						strcat(ver_str, cs_str);
#endif
					}
					break;
				case 4 :sprintf(ver_str, "V%c.%c",Startup.version[0],Startup.version[1]);
					sprintf(des_str, "Data");
					break;
				case 5 :	// boot checksum
					sprintf(ver_str, "");
					if (GetBootVersion(bootversion))
					{
						sprintf(ver_str, "V%c.%c", bootversion[0], bootversion[1]);
						if (calc_checksum)
						{
							sprintf(cs_str, "(%04X)", boot_checksum);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
							strcat(ver_str, cs_str);
#endif
						}
					}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_FONT
					sprintf(des_str, "Boot");
	#else
					sprintf(des_str, "Scale Boot");
	#endif
#endif	// #ifdef _USE_LCD20848_
					break;
				case 6:
#ifdef USE_CHN_IO_BOARD
					i2c_readVersionOfBoard(ver_str);
					ver_str[6] = 0; // 앞 6자리만 표시 "V1.00CN"
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_FONT
					sprintf(des_str, "I/O");
	#else
					sprintf(des_str, "I/O Board");
	#endif
#endif	// #ifdef _USE_LCD20848_
					break;
#else
					cur_id = 7;
#endif //#ifdef USE_CHN_IO_BOARD
				case 7:
					sprintf(ver_str, "N/A");
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_FONT
					sprintf(des_str, "WL B/G");
	#else
					sprintf(des_str, "WL BRIDGE");
	#endif
#else	// #ifdef _USE_LCD20848_
					sprintf(des_str, "WLAN");
#endif	// #ifdef _USE_LCD20848_
                    wlan_SetNetwork(ip_l, gw, sn);  // change local IP for w/l setting
                    if (wlan_CheckVer_Bridge(Startup.wireless_type, w_version, 0))
					{
						w_version[16] = 0;
						sprintf(ver_str, (char *)w_version);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_FONT
						sprintf(des_str, "WLAN(M/P)");
	#else
						sprintf(des_str, "WLAN");
	#endif 
#endif	// #ifdef _USE_LCD20848_
						if (w_version[0] == 'T')
						{
							Startup.wireless_type = WLTYPE_TOBNET;
						}
						else
						{
							Startup.wireless_type = WLTYPE_LANTRONIX;
						}
					}
					Ethernet_RestoreNetwork(); // 무선랜 버전 확인 후 IP 복구 안 시키는 오류 수정 (다른 모델도 동일, CN1 r3378 Merge)
#ifdef USE_USB_ZIG_TEST
					APP_Initialize ();
					usb_write_ok = zig_usb_write();
					usb_read_ok = zig_usb_read();
					if(usb_write_ok && usb_read_ok) usb_comm_ok = 1;
					else usb_comm_ok = 0;

					if (usb_comm_ok)
					{
						sprintf(cs_str, "-USB OK");
						strcat(ver_str, cs_str);
					}
					else
					{
						if(status_run.run_mode == RUN_CALIBRATION)
						{
							sprintf(cs_str, "-USB FAIL");
						}
						else
						{
							sprintf(cs_str, "-NO USB");
						}
						strcat(ver_str, cs_str);
					}
  #if !defined(USE_CTS_FUNCTION) && defined(USE_ADM_AUTO_RESTORE)
					cur_id = 8; // 8건너 뛰고 9로 가려고
  #endif
#endif
					break;
#ifdef USE_CTS_FUNCTION
    			case 8 :
					version = CtsStruct2.BoardId;
					sprintf(ver_str, "%08d", version); // Board ID 10자리중 8자리만 출력(수정예정)
					sprintf(des_str, "BOARD ID");
					break;
#endif
#ifdef USE_DATETIME_ZIG_TEST
				case 9 :
					RTC_CLK_Burst_Read();
					if(RTCStruct.hour==0xFF || RTCStruct.min==0xFF || RTCStruct.sec==0xFF) rtc_Batt_Err = 1;
					else rtc_Batt_Err = 0;
					
					if(!rtc_Batt_Err)
					{	
						year  =bcd2hex(RTCStruct.year);
						month =bcd2hex(RTCStruct.month);
						day   =bcd2hex(RTCStruct.date);

						if(year>99) year=99;
						sprintf(ver_str,"20%02d.%02d.%02d-%02X:%02X:%02X",year,month,day,RTCStruct.hour,RTCStruct.min,RTCStruct.sec);
					}
					else
					{
						sprintf(ver_str,"RTC ERROR");
					}
					sprintf(des_str,"RTC CHECK");
					break;
#endif
#ifdef USE_ADM_AUTO_RESTORE
				case 10 :
					init_err_cnt = Flash_bread(DFLASH_BASE + FLASH_ADM_INIT_ERROR_COUNT);
					sprintf(ver_str, "%3d", init_err_cnt);
					sprintf(des_str, "Init Err");
					break;
				case 11 :
					eep_err_cnt  = Flash_bread(DFLASH_BASE + FLASH_ADM_EEP_ERROR_COUNT);
					sprintf(ver_str, "%3d", eep_err_cnt);
  					sprintf(des_str, "EEP Err");
					break;
#endif
				default: goto END;
			}
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	#ifdef USE_ARAB_FONT
			sprintf(str_buf, "%-5s %s", des_str, ver_str);
	#else
			sprintf(str_buf, "%-10s %s", des_str, ver_str);
	#endif
			disp_p = point((nth+start_line+1)*char_h, 0);
			Dsp_Write_String(disp_p, str_buf);
#else	// #ifdef _USE_LCD20848_
			display_clear(DISPLAY_UNITPRICE);
			display_string(DISPLAY_UNITPRICE,(INT8U *)des_str);
			display_clear(DISPLAY_PRICE);
			display_string_pos(DISPLAY_START_POS_PRICE, (INT8U*)ver_str);
			disp_p = point(0, 0);
			Dsp_Write_String(disp_p,(HUGEDATA char*)cs_str);
#endif	// #ifdef _USE_LCD20848_
			nth++;
			cur_id++;
			if (cur_id > end_id || nth >= MenuNumLinePerPage) break;
		}
	END:
		Dsp_Diffuse();
		while(!KEY_Read());
		switch(KeyDrv.Type) {
			case KEY_TYPE_FUNCTION :
				switch(KeyDrv.CnvCode) {
					case KP_ARROW_DN :
					case KP_PAGE_DN :
						if(cur_id > end_id) {
							cur_id = page_first_id;
							BuzOn(2);
						} else BuzOn(1);
						break;
					case KP_ARROW_UP :
					case KP_PAGE_UP :
						if(page_first_id <= start_id) {
							cur_id = page_first_id;
							BuzOn(2);
						} else {
#ifndef USE_CTS_FUNCTION
							if(page_first_id == 11) page_first_id = 10;
#endif
#ifndef USE_CHN_IO_BOARD
							if(page_first_id == 7) page_first_id = 6;
#endif
							cur_id = page_first_id-MenuNumLinePerPage;
							BuzOn(1);
						}
						break;
					case KP_ESC :
						BuzOn(1);
						DspLoadFont1(old_font);
						DspStruct.DirectDraw = backupDirect;
						return;
					case KP_TEST :
						if (status_run.run_mode==RUN_CALIBRATION) {
							BuzOn(1);
							history_print_list();
							//parameter_print_list(FACTORY_MODE);
						} else {
							BuzOn(2);
						}
						cur_id = page_first_id;
						break;
					case KP_FEED:
						keyapp_feed(OFF);
						cur_id = page_first_id;
						break;
#ifdef USE_WHITESCREEEN_DEBUG
					case KP_TARE :
						BuzOn(1);
						set_nvram_wparam(NVRAM_BOOT_COUNT_FOR_DEBUG, 0);
						cur_id = page_first_id;
						break;
#endif
					case KP_ZERO :
						BuzOn(1);

						Dsp_SetPage(DSP_ERROR_PAGE);
#ifdef USE_CHN_FONT
						DspLoadFont1(3);
#else
						DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
						Dsp_Fill_Buffer(0);

						disp_p = point( Startup.menu_type*Startup.menu_ygap, 0);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
						sprintf(str, "Wait for Check!");
#else
						sprintf(str, "Wait for Chk!");
#endif
						Dsp_Write_String(disp_p,(HUGEDATA char*)str);
						Dsp_Diffuse();

						boot_checksum = 0;
						/* boot 펌웨어 체크섬 동작 방식과 동일하게 boot 메모리 주소 영역 체크함 */
						cp = (ROMDATA INT8U*)PA_TO_KVA0(CHECKSUM_BOOT_START);
						for(k = CHECKSUM_BOOT_START; k < CHECKSUM_BOOT_END; k++) {
							if (k == CHECKSUM_BOOT_RESERVE_START)
							{
								k = CHECKSUM_BOOT_RESERVE_END;
								cp = (ROMDATA INT8U*)PA_TO_KVA0(CHECKSUM_BOOT_RESERVE_END);
							}
							boot_checksum += *cp++;
						}

						fw_checksum = 0;
						cp = (ROMDATA INT8U*)PA_TO_KVA0(CHECKSUM_FW_START);
						for(k = CHECKSUM_FW_START; k < CHECKSUM_FW_END; k++) {
							fw_checksum += *cp++;
						}

						Flash_flush_nvbuffer();
						/*
						max_cap = caption_search_by_code(0xffff);
						max_cap -= (DFLASH_BASE + FLASH_CAPTION_AREA);
						max_cap /= CAP_MSG_LENGTH;
						max_cap ++;
						*/
						max_cap = MAX_CAPTION_NO;
						fcp = (HUGEDATA INT8U*)(DFLASH_BASE + FLASH_CAPTION_AREA);	// Caption
						cap_checksum = 0;
						for(k = 0; k < (max_cap*(INT32U)CAP_MSG_LENGTH); k++) {
  #ifdef USE_EXTEND_CAPNTION_AREA
							if (fcp == (HUGEDATA INT8U*)(DFLASH_BASE + FLASH_CAPTION_AREA + ((MID_CAPTION_NO) * CAP_MSG_LENGTH)))
								fcp = (HUGEDATA INT8U*)(DFLASH_BASE + FLASH_CAPTION_EXT_AREA);
  #endif
							//cap_checksum += *fcp++;
							cap_checksum += Flash_bread((INT32U)fcp);
							fcp++;
						}
						max_cap = plu_struct_get_nfield();
						fcp = (HUGEDATA INT8U*)(DFLASH_BASE + FLASH_PLU_STRUCT_CAPTION_AREA);	// Plu Name Caption
						for(k = 0; k < (max_cap*(INT32U)PLUSTR_NAME_SIZE); k++) {
							//cap_checksum += *fcp++;
							cap_checksum += Flash_bread((INT32U)fcp);
							fcp++;
						}

						Dsp_SetPage(DSP_DEFAULT_PAGE);
						DspLoadFont1(DSP_SMALL_FONT_ID);
						Dsp_Diffuse();
						BuzOn(1);
						calc_checksum = ON;

						cur_id = page_first_id;
						break;
					default :
						BuzOn(2);
						cur_id = page_first_id;
						break;
				}
				break;
			default :
				BuzOn(2);
				cur_id = page_first_id;
				break;
		}
	}
}
#elif !defined(_USE_LCD20848_)//#if defined(USE_SINGLE_LINE_GRAPHIC)
void CalRun_FirmwareVersion(void)
{
	POINT disp_p;
	INT16U i,nth;//,j;
	INT16S cur_id, page_first_id;
	char str_buf[50], str[32];
	CAPTION_STRUCT cap;
	INT32U start_id, end_id;
	INT16U version;
	char ver_str[32], des_str[20];
	char backupDirect;
	ROMDATA INT8U *cp;
	INT8U *fcp;
	static INT16U boot_checksum=0, fw_checksum=0, cap_checksum=0;
	static INT8U calc_checksum = OFF;
	INT32U k, max_cap;
	FW_VERSION fversion;
	FW_VERSION2 fversion2;
	char subversion[3];
	INT8U bootversion[4];
	char char_w, char_h;
	INT8U start_line;
	char old_font;
	char dealer_str[4];
	char country_str[4];
	char cs_str[10];
	INT8U w_version[17];

	MenuNumLinePerPage = 3;

	version_get(&fversion);
	version2_get(&fversion2);

	memcpy(dealer_str, fversion2.dealer, 3);
	dealer_str[3] = 0;
	str_trim(dealer_str);
	memcpy(country_str, fversion2.country, 2);
	country_str[2] = 0;
	str_trim(country_str);
	if (country_str[0]) strcat(country_str, " ");	// country_str이 있을 때, 스페이스 추가
	backupDirect = DspStruct.DirectDraw;
	start_id = 0;
	end_id = 8;

	cur_id = start_id;
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_SMALL_FONT_ID);
	char_w = display_font_get_width();
	char_h = display_font_get_height();
	start_line = 0;
	if (Startup.menu_type == 2) start_line = 1;
	EthVariableInit();
	if (Startup.global_com&0x01) {
		if(EthInit())
		{
		}
		else
		{
			Startup.global_com &= ~0x01;
		}
	}
	while(1) {	
		DspStruct.DirectDraw = 0;
		Dsp_Fill_Buffer(0);
		menu_display_menucode(CurMenuCode, 1);
#ifdef USE_ARAB_FONT
		sprintf(ver_str, "DEV DESC. VERSION");
#else
		sprintf(ver_str, "DESC.      VERSION");
#endif
		disp_p = point(start_line*char_h, 0);
		Dsp_Write_String(disp_p, (HUGEDATA char *)ver_str);
		nth = 0;
		page_first_id = cur_id;
		memset(ver_str,0x00,20);
		memset(des_str,0x00,20);
		while(1) {
			switch(cur_id) {
				case 0 :	// Main
					i = 0;
					if(fversion.subversion[0] != '0') subversion[i++] = fversion.subversion[0];
					subversion[i++] = fversion.subversion[1];
					subversion[i] = 0;
					sprintf(ver_str, "%sV%c.%c%c.%s%s", country_str, fversion.version, fversion.reversion[0], fversion.reversion[1]
									     , subversion, dealer_str);
					if (calc_checksum)
					{
						sprintf(cs_str, "(%04X)", fw_checksum);
						strcat(ver_str, cs_str);
					}
#ifdef USE_ARAB_FONT
					sprintf(des_str, "Scale");
#else
					sprintf(des_str, "Scale Main");
#endif
					break;
				case 1 :	// ADM
					version = (INT16U)(ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF) % 1000);	// Version = "1023" + VER[3]
					sprintf(ver_str, "V%d.%02d", version/100, version%100);
#ifdef USE_ARAB_FONT
					sprintf(des_str, "AD");
#else
					sprintf(des_str, "AD Module");
#endif
					break;
				case 2 :
					sprintf(ver_str, "N/A     ");
					if (Startup.global_com&0x01) {
						for(i=0;i<8;i++) {
							ver_str[i] = EthCardStruct.Ver[i];
						}
						ver_str[i]=0;
					}
#ifdef USE_ARAB_FONT
					sprintf(des_str, "LAN");
#else
					sprintf(des_str, "Ethernet");
#endif
					break;
				case 3 :caption_split_by_code(0x0fff, &cap, 0);
					//sprintf(ver_str, "V%s(%04Xh)", &cap.form[8], cap_checksum);
					//sprintf(des_str, "Caption(%.5s)", cap.form);
					sprintf(ver_str, "%s", cap.form);
#ifdef USE_ARAB_FONT
					sprintf(des_str, "Cap.");
#else
					sprintf(des_str, "Caption");
#endif
					if (calc_checksum)
					{
						sprintf(cs_str, "(%04X)", cap_checksum);
						strcat(ver_str, cs_str);
					}
					break;
				case 4 :sprintf(ver_str, "V%c.%c",Startup.version[0],Startup.version[1]);
					sprintf(des_str, "Data");
					break;
				case 5 :	// boot checksum
					bootversion[0] = ((ROMDATA INT8U*)(FW_BOOT_VERSION_ROM_ADDR))[10];
					bootversion[1] = ((ROMDATA INT8U*)(FW_BOOT_VERSION_ROM_ADDR))[11];
					bootversion[2] = ((ROMDATA INT8U*)(FW_BOOT_VERSION_ROM_ADDR))[12];
					bootversion[3] = ((ROMDATA INT8U*)(FW_BOOT_VERSION_ROM_ADDR))[13];
					sprintf(ver_str, "V%c%c.%c%c", bootversion[2],bootversion[3],bootversion[0],bootversion[1]);
					if (calc_checksum)
					{
						sprintf(cs_str, "(%04X)", boot_checksum);
						strcat(ver_str, cs_str);
					}
#ifdef USE_ARAB_FONT
					sprintf(des_str, "Boot");
#else
					sprintf(des_str, "Scale Boot");
#endif
					break;
				case 6:
#ifdef USE_CHN_IO_BOARD
					i2c_readVersionOfBoard(ver_str);
					ver_str[6] = 0; // 앞 6자리만 표시 "V1.00CN"
#ifdef USE_ARAB_FONT
					sprintf(des_str, "I/O");
#else
					sprintf(des_str, "I/O Board");
#endif
					break;
#else
					cur_id = 7;
#endif //#ifdef USE_CHN_IO_BOARD
				case 7:
					sprintf(ver_str, "N/A");
#ifdef USE_ARAB_FONT
					sprintf(des_str, "WL B/G");
#else
					sprintf(des_str, "WL BRIDGE");
#endif
					if (wlan_CheckVer_Bridge(Startup.wireless_type, w_version, 0))
					{
						w_version[16] = 0;
						sprintf(ver_str, (char *)w_version);
#ifdef USE_ARAB_FONT
						sprintf(des_str, "WLAN(M/P)");
#else
						sprintf(des_str, "WLAN");
#endif                        
						if (w_version[0] == 'T')
						{
							Startup.wireless_type = WLTYPE_TOBNET;
						}
						else
						{
							Startup.wireless_type = WLTYPE_LANTRONIX;
						}
					}
					break;
				case 8 :
					version = Key_Check_Version();
					sprintf(ver_str, "V%02d", version);
					sprintf(des_str, "KEY");
					break;
				default: goto END;
			}
#ifdef USE_ARAB_FONT
			sprintf(str_buf, "#%d  %-5s %s",cur_id+1,des_str,ver_str);
#else
			sprintf(str_buf, "%-10s %s", des_str, ver_str);
#endif
			disp_p = point((nth+start_line+1)*char_h, 0);
			Dsp_Write_String(disp_p, str_buf);
			nth++;
			cur_id++;
			if (cur_id > end_id || nth >= MenuNumLinePerPage) break;
		}
	END:
		Dsp_Diffuse();
		while(!KEY_Read());
		switch(KeyDrv.Type) {
			case KEY_TYPE_FUNCTION :
				switch(KeyDrv.CnvCode) {
					case KP_ARROW_DN :
					case KP_PAGE_DN :
						if(cur_id > end_id) {
							cur_id = page_first_id;
							BuzOn(2);
						} else BuzOn(1);
						break;
					case KP_ARROW_UP :
					case KP_PAGE_UP :
						if(page_first_id <= start_id) {
							cur_id = page_first_id;
							BuzOn(2);
						} else {
							cur_id = page_first_id-3;
							BuzOn(1);
						}
						break;
					case KP_ESC :
						BuzOn(1);
						DspLoadFont1(old_font);
						DspStruct.DirectDraw = backupDirect;
						return;
					case KP_TEST :
						if (status_run.run_mode==RUN_CALIBRATION) {
							BuzOn(1);
							history_print_list();
						} else {
							BuzOn(2);
						}
						cur_id = page_first_id;
						break;
					case KP_FEED:
						keyapp_feed(OFF);
						cur_id = page_first_id;
						break;
					case KP_ZERO :
						BuzOn(1);

						Dsp_SetPage(DSP_ERROR_PAGE);
#ifdef USE_CHN_FONT
						DspLoadFont1(3);
#else						
						DspLoadFont1(DSP_LARGE_FONT_ID);
#endif
						Dsp_Fill_Buffer(0);

						disp_p = point( Startup.menu_type*Startup.menu_ygap, 0);
						sprintf(str, "Wait for Check!");
						Dsp_Write_String(disp_p,(HUGEDATA char*)str);
						Dsp_Diffuse();

						cp = (ROMDATA INT8U*)CHECKSUM_BOOT_START;
						boot_checksum = 0;
						for(k = CHECKSUM_BOOT_START; k < CHECKSUM_BOOT_END; k++) {
							boot_checksum += *cp++;
						}

						cp = (ROMDATA INT8U*)CHECKSUM_FW_START;
						fw_checksum = 0;
						for(k = CHECKSUM_FW_START; k < CHECKSUM_FW_END; k++) {
							fw_checksum += *cp++;
						}

						Flash_flush_nvbuffer();
						max_cap = caption_search_by_code(0xffff);
						max_cap -= (DFLASH_BASE + FLASH_CAPTION_AREA);
						max_cap /= CAP_MSG_LENGTH;
						max_cap ++;
						fcp = (HUGEDATA INT8U*)(DFLASH_BASE + FLASH_CAPTION_AREA);	// Caption
						cap_checksum = 0;
						for(k = 0; k < (max_cap*(INT32U)CAP_MSG_LENGTH); k++) {
							cap_checksum += *fcp++;
						}
						max_cap = plu_struct_get_nfield();
						fcp = (HUGEDATA INT8U*)(DFLASH_BASE + FLASH_PLU_STRUCT_CAPTION_AREA);	// Plu Name Caption
						for(k = 0; k < (max_cap*(INT32U)PLUSTR_NAME_SIZE); k++) {
							cap_checksum += *fcp++;
						}

						Dsp_SetPage(DSP_DEFAULT_PAGE);
						DspLoadFont1(DSP_SMALL_FONT_ID);
						Dsp_Diffuse();
						BuzOn(1); 
						calc_checksum = ON;

						cur_id = page_first_id;
						break;
					default :
						BuzOn(2);
						cur_id = page_first_id;
						break;
				}
				break;
			default :
				BuzOn(2);
				cur_id = page_first_id;
				break;
		}
	}
}
#endif //#if defined(USE_SINGLE_LINE_GRAPHIC)

#if defined (CL5200_PROJECT) || defined (CL5500_PROJECT)
#ifdef CPU_PIC32MZ
#define MAX_FLASH_CHIP 8
//#define MAX_FLASH_CHIP 6
#endif

#ifdef USE_MORNITORING_CNT
void CalRun_Scale_Info(void)
{
	POINT disp_p;
	INT16U i,nth;//,j;
	INT16S cur_id, page_first_id;
	INT8U cur_page = 1;
	char str_buf[32];
	CAPTION_STRUCT cap;
	INT32U start_id, end_id;
	char ver_str[32], des_str[20];
	char backupDirect;
	INT32U k, max_cap;
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	char char_w,char_h;
	INT8U start_line;
#endif
	char old_font;

	INT16U  year = 0;
	INT16U  month = 0;
	INT16U  day = 0;
	INT8U rtc_Batt_Err;
	INT32S set_val[5];
	INT32U init_date;
	INT32S cmd_param;
	INT32U temp_date;
	
	memset(str_buf,0x00,sizeof(str_buf));
	MenuNumLinePerPage = 3;

	backupDirect = DspStruct.DirectDraw;
	start_id = 0;
	end_id = 11;

	cur_id = start_id;
	old_font = DspStruct.Id1;
	DspLoadFont1(DSP_SMALL_FONT_ID);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
	char_w = display_font_get_width();
	char_h = display_font_get_height();
	start_line = 0;
	if (Startup.menu_type == 2) start_line = 1;
#endif
	
	set_val[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_OVERLOAD_SUM_CNT, ADM_PARAM_ON)%1000000L;
	set_val[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_OVERLOAD_CNT, ADM_PARAM_ON)%1000000L;
	set_val[2] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_CAL_DATE, ADM_PARAM_ON)%1000000L;
	set_val[3] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_WEIGHING_SUM_CNT, ADM_PARAM_ON)%1000000L;
	set_val[4] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_WEIGHING_CNT, ADM_PARAM_ON)%1000000L;

	while(1) 
	{
		DspStruct.DirectDraw = 0;
		Dsp_Fill_Buffer(0);
		menu_display_menucode(CurMenuCode, 1);
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
		DspLoadFont1(DSP_SMALL_FONT_ID);
		sprintf(ver_str, "DESC.                VALUE");
		disp_p = point(start_line*char_h, 0);
		Dsp_Write_String(disp_p, (HUGEDATA char *)ver_str);
#else
		menu_displaytitle(0x8570,0);
#endif
#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
#if defined(USE_ARAB_FONT) || defined(HEBREW_FONT)
		DspLoadFont1(DSP_6_BY_8_ENG_FONT_ID);
#endif
#endif
		nth = 0;
		page_first_id = cur_id;
		cur_page = (INT8U)((cur_id + 1) / MenuNumLinePerPage);
		memset(ver_str,0x00,20);
		memset(des_str,0x00,20);
		while(1) {
			switch(cur_id) {
				case 0 : //Print Lenght(SUM)
					sprintf(des_str, "Print Len(SUM)");
					sprintf(ver_str, "%-4ld", TphUseData.TotalCnt);
					break;
				case 1 : //Print Lenght
					sprintf(des_str, "Print Len");
					sprintf(ver_str, "%-4ld",  TphUseData.AfterCnt);
					break;
				case 2 : // Inspection Date
					sprintf(des_str, "Inspection Date");
					temp_date = TphUseData.InspectDate;
					year = temp_date / 10000L;
					temp_date %= 10000L;
					month = temp_date / 100L;
					temp_date %= 100L;
					day = temp_date;                    
					sprintf(ver_str,"20%02d.%02d.%02d",year ,month ,day);
					break;
				case 3 : // Overload Count(SUM)
					sprintf(des_str, "Overload Count(SUM)");
					if (set_val[0] == -1) {
						sprintf(ver_str, "Disconnected!");
					}
					else	{
						sprintf(ver_str, "%-4ld", set_val[0]);
					}
					break;
				case 4 : // Overload Count
					sprintf(des_str, "Overload Count");
					if (set_val[1] == -1) {
						sprintf(ver_str, "Disconnected!");
					}
					else	{
						sprintf(ver_str, "%-4ld", set_val[1]);
					}
					break;
				case 5 : // Inspection Date
				case 8:
					sprintf(des_str, "Inspection Date");
					if (set_val[2] == -1) {
						sprintf(ver_str, "Disconnected!");
					}
					else	{
						temp_date = set_val[2];
						year = temp_date / 10000L;
						temp_date %= 10000L;
						month = temp_date / 100L;
						temp_date %= 100L;
						day = temp_date;
						sprintf(ver_str,"20%02d.%02d.%02d",year,month,day);
					}
					break;
				case 6 : // Weighing Count(SUM)
					sprintf(des_str, "Weighing Count(SUM)");
					if (set_val[3] == -1) {
						sprintf(ver_str, "Disconnected!");
					}
					else	{
						sprintf(ver_str, "%-4ld", set_val[3]);
					}
					break;
				case 7 : // Weighing Count
					sprintf(des_str, "Weighing Count");
					if (set_val[4] == -1) {
						sprintf(ver_str, "Disconnected!");
					}
					else	{
						sprintf(ver_str, "%-4ld", set_val[4]);
					}
					break;
				case 9 : // Using Hour(SUM)
					sprintf(des_str, "Using Hour(SUM)");
					//OperTime.TotalCnt = get_nvram_lparam(NVRAM_MORNITORING_USING_TIME);
					sprintf(ver_str, "%-4ld", OperTime.TotalCnt);
					break;
				case 10 : // Using Hour
					sprintf(des_str, "Using Hour");
					sprintf(ver_str, "%-4ld", OperTime.AfterCnt);
					break;
				case 11 : // Inspection Date
					sprintf(des_str, "Inspection Date");
					temp_date = OperTime.InspectDate;
					year = temp_date / 10000L;
					temp_date %= 10000L;
					month = temp_date / 100L;
					temp_date %= 100L;
					day = temp_date;
					sprintf(ver_str,"20%02d.%02d.%02d",year ,month ,day);
					break;
				default: goto END;
			}

#if defined(_USE_LCD20848_) || defined(_USE_LCD32_)
			sprintf(str_buf, "%-20s %s", des_str, ver_str);
			disp_p = point((nth+start_line+1)*char_h, 0);
			Dsp_Write_String(disp_p, str_buf);
#endif	// #ifdef _USE_LCD20848_
			nth++;
			cur_id++;
			if (cur_id > end_id || nth >= MenuNumLinePerPage) break;
		}
	END:
		Dsp_Diffuse();
		while(!KEY_Read());
		switch(KeyDrv.Type) {
			case KEY_TYPE_FUNCTION :
				switch(KeyDrv.CnvCode) {
					case KP_ARROW_DN :
					case KP_PAGE_DN :
						if(cur_id > end_id) {
							cur_id = page_first_id;
							BuzOn(2);
						} else BuzOn(1);
						break;
					case KP_ARROW_UP :
					case KP_PAGE_UP :
						if(page_first_id <= start_id) {
							cur_id = page_first_id;
							BuzOn(2);
						} else {
							cur_id = page_first_id - MenuNumLinePerPage;
							BuzOn(1);
						}
						break;
					case KP_ESC :
						BuzOn(1);
						DspLoadFont1(old_font);
						DspStruct.DirectDraw = backupDirect;
						display_alloff_primary_indication();
						return;
					case KP_TEST :
						if (status_run.run_mode==RUN_CALIBRATION) {
							BuzOn(1);
							history_print_list();
						} else {
							BuzOn(2);
						}
						cur_id = page_first_id;
						break;
					case KP_FEED:
						keyapp_feed(OFF);
						cur_id = page_first_id;
						break;
					case KP_ZERO : // Erase Count
						BuzOn(1);
						RTC_CLK_Burst_Read();
						if(RTCStruct.hour==0xFF || RTCStruct.min==0xFF || RTCStruct.sec==0xFF) {
							rtc_Batt_Err = 1;   
							year = 0;
							month = 0;
							day = 0;
						} else {
							rtc_Batt_Err = 0;
							year  =bcd2hex(RTCStruct.year);
							month =bcd2hex(RTCStruct.month);
							day   =bcd2hex(RTCStruct.date);

							if(year>99) year=99;

							init_date = year * 10000L;
							init_date += month * 100L;
							init_date += day;
						}
						if(!rtc_Batt_Err) 
						{
							switch (cur_page) 
							{
								case 0://Print Lenght data
									if (permission_check_sub(PMS_DEALER,0)) 
									{
										PrtClearTphUseData(init_date);
									}
									else
										goto END;
									break;
								case 1: // Overload data
								case 2: // weighg data
									if (status_run.run_mode == RUN_CALIBRATION) 
									{
										// Cal Date init
										cmd_param = init_date;
										cmd_param += ADM_SUB_CAL_DATE;
										ADM_WaitData(ADM_CMD_SET_PARAMETER, cmd_param, ADM_PARAM_ON);
										// ADM Data Init
										ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_OVERLOAD_CNT, ADM_PARAM_ON);
										ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_WEIGHING_CNT, ADM_PARAM_ON);
										// read data
										set_val[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_OVERLOAD_SUM_CNT, ADM_PARAM_ON)%1000000L;
										set_val[1] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_OVERLOAD_CNT, ADM_PARAM_ON)%1000000L;
										set_val[2] = init_date;
										set_val[3] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_WEIGHING_SUM_CNT, ADM_PARAM_ON)%1000000L;
										set_val[4] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_WEIGHING_CNT, ADM_PARAM_ON)%1000000L;
									}
									break;
								case 3:  // Using Hour(SUM)
									if (status_run.run_mode == RUN_CALIBRATION) 
									{
										// init data
                                        rtc_clear_operated_time(init_date);
										// read data
										rtc_load_operated_time();
									}
									break;
							}
						}
						
						Dsp_Fill_Buffer(0);
						Dsp_Diffuse();
						BuzOn(1);

						cur_id = page_first_id;
						break;
					default :
						BuzOn(2);
						cur_id = page_first_id;
						break;
				}
				break;
			default :
				BuzOn(2);
				cur_id = page_first_id;
				break;
		}
	}
}
#endif

void CalRun_MemoryInformation(void)
{
	INT16U result;
	INT8U mem_size;
	char disp_string_buf[50], chip_status[20], chip_number[20];
	POINT disp_p;
	CAPTION_STRUCT cap;
	INT8U i;
	INT16U input_x;
	INT16U cap_code[3] = {0x1866, 0x97aa, 0x97ab};
	INT8U cap_id;

	if (status_run.run_mode==RUN_CALIBRATION) {
		cap_id = 1;
	} else {
		cap_id = 0;
	}

	input_x = 14;

	while(1) {
		DspStruct.DirectDraw = 0;
#ifdef CPU_PIC32MZ
		result = 0xff;
#else
		result = FlashCheckMem();
		result >>= 4;
#endif
		mem_size = 0;
#ifdef USE_CHECK_MEMORY_SIZE
		Dsp_Fill_Buffer(0);
		DspLoadFont1(DSP_MENU_FONT_ID);
		
		if( Flash0ExtFlag == FLASH_SIZE_8MB ) 			mem_size = 8;
		else if( Flash0ExtFlag == FLASH_SIZE_4MB )		mem_size = 4;
		else											mem_size = 0;
		
		if( mem_size )	sprintf(disp_string_buf,"FLASH-1(MB): %d", mem_size);
		else			sprintf(disp_string_buf,"FLASH-1(MB): N/A");
		disp_p = point(0, 0);
		Dsp_Write_String(disp_p,disp_string_buf);

		if( Flash1ExtFlag == FLASH_SIZE_8MB ) 			mem_size = 8;
		else if( Flash1ExtFlag == FLASH_SIZE_4MB )		mem_size = 4;
		else											mem_size = 0;
		
		if( mem_size )	sprintf(disp_string_buf,"FLASH-2(MB): %d", mem_size);
		else			sprintf(disp_string_buf,"FLASH-2(MB): N/A");
		disp_p = point(Startup.menu_ygap, 0);
		Dsp_Write_String(disp_p,disp_string_buf);
#else // USE_CHECK_MEMORY_SIZE
		for(i = 0; i < MAX_FLASH_CHIP; i++) {
			if((result&0x0001) == 1) {
				mem_size++;
				chip_status[i] = 'O';
			} else {
				chip_status[i] = 'X';
			}
			chip_number[i] = i + '0';
			result >>= 1;
		}
		chip_status[i] = 0x00;
		chip_number[i] = 0x00;

		Dsp_Fill_Buffer(0);
		DspLoadFont1(DSP_MENU_FONT_ID);
		if (Startup.menu_type != 0) {
			menu_display_menucode(CurMenuCode, 1);
			menu_displaytitle(cap_code[0], 0);//0x8560,0);
			menu_writepage(1, 1);
			disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, 0);
			caption_split_by_code(cap_code[1], &cap, 0);//0x9560, &cap,0);	// "Check Flash Chip"
			cap.input_dlength = MAX_FLASH_CHIP;
			cap.input_length  = MAX_FLASH_CHIP;
			caption_adjust(&cap, NULL);
			input_x = cap.input_x;
			sprintf(disp_string_buf,(const char *)cap.form);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
			hebrew_codetemp[strlen(disp_string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
			arab_codetemp[strlen(disp_string_buf)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,disp_string_buf);
#endif
			disp_p = point(Startup.menu_type*Startup.menu_ygap, input_x*Startup.menu_xgap);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)chip_number, strlen(chip_number), 0, 0); 
			hebrew_codetemp[strlen(chip_number)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)chip_number, strlen(chip_number), 0, 0, 0); 
			arab_codetemp[strlen(chip_number)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p,chip_number);
#endif
			disp_p = point((Startup.menu_type+1)*Startup.menu_ygap, input_x*Startup.menu_xgap);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)chip_status, strlen(chip_status), 0, 0); 
			hebrew_codetemp[strlen(chip_status)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#else
			Dsp_Write_String(disp_p,chip_status);
#endif
		}
		
		disp_p = point((Startup.menu_type!=0?(Startup.menu_type+2):0)*Startup.menu_ygap, 0);
		caption_split_by_code(cap_code[2], &cap, 0);//0x9561, &cap,0);	// "Total Flash Size :"
		sprintf(disp_string_buf,"%s %d", (char*)cap.form, mem_size);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0); 
		hebrew_codetemp[strlen(disp_string_buf)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)disp_string_buf, strlen(disp_string_buf), 0, 0, 0); 
		arab_codetemp[strlen(disp_string_buf)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p,disp_string_buf);
#endif
#endif // USE_CHECK_MEMORY_SIZE

		Dsp_Diffuse();

		while(!KEY_Read());

		switch(KeyDrv.CnvCode) {
			case KP_ESC :
				BuzOn(1);
				return;
			default :
				BuzOn(2);
				break;
		}
	}
}
#endif	//CL5200_PROJECT
//Scale - Type 설정 시 Description 
void CalRun_ScaleTypee_callback(long type32)	//Added by JJANG 20070621
{
	CAPTION_STRUCT cap;
	POINT disp_p;
	char string_buf[MAX_MENU_CHAR_BUF];
	INT8U len, i;
	INT16U w;
	INT8U type;	
#if defined(USE_CL5200_KEY) || defined(USE_CL3000_KEY)
	char typename[3][14]={"STANDARD TYPE", "POLE TYPE", "HANGING TYPE"};
#else
 	char typename[5][14]={"STANDARD TYPE","POLE TYPE","HANGING TYPE","SELF-SERVICE","DOUBLE-B TYPE"};
#endif
 	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);		
	w = Dsp_GetChars();
	
	type = (INT8U)type32;
#if defined(USE_CL5200_KEY) || defined(USE_CL3000_KEY)
	if (type>0 && type<4) strcpy(cap.form,typename[type-1]);

#else
	if (type>0 && type<6)  //SG100107
	{
		strcpy(cap.form,typename[type-1]);
	}	
#endif
	else 	strcpy(cap.form,global_message[MSG_INVALID_VALUE]); // Invalid Value

	len = strlen((char *)cap.form);
	memcpy((INT8U *)string_buf, (INT8U*)cap.form, len);
	for(i = len; i < w; i++) 
	{
		string_buf[i] = 0x20;
	}
	string_buf[w] = 0x00;	
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
	Dsp_Write_String(disp_p, string_buf);
#endif
}

extern void salemode_reset(void);

void SetScaleType(INT8U scaleType, INT8U display, INT8U alwaysChange)
{
	INT8U oldType;
	INT8U ret;

	oldType = get_global_bparam(GLOBAL_AREA_SCALETYPE);
	set_global_bparam(GLOBAL_AREA_SCALETYPE, scaleType);
	Startup.scale_type = scaleType;

	if(scaleType == 4) set_global_bparam(GLOBAL_SALERUN_MODE, 7);	// self type
	else set_global_bparam(GLOBAL_SALERUN_MODE, 2);
	salemode_reset();

	ret = 0;
	if (oldType != scaleType || alwaysChange){
		if (display)
		{
			VertScrollMsg_Init(DSP_MENU_FONT_ID);
	
			VertScrollMsg_Add("Change Type");
			VertScrollMsg_Add("Conv Key TBL");
		}
	#ifdef _USE_UNDER_SELFKEY_
		if(scaleType != 4)	  ret = KEY_CnvTable(0, scaleType, 0x3f);
		else	 	  ret=1;
	#else
		ret = KEY_CnvTable(0, scaleType, 0x3f);
	#endif
		
		if (ret)	// if key table version is old (under 0xa001)					    
		{
			if (display)
			{
				VertScrollMsg_Add("Init Key TBL");
			}
			KEY_CopyKeyTable(0xffff);
		}

		if (display)
		{
			VertScrollMsg_Add(" Init A/D Fil");
		}
		CalRun_InitAdFilter(scaleType);	// AD Filter settings
		ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);

		if (display)
		{
//			VertScrollMsg_Add("Press Any Key");
			VertScrollMsg_Add("Press Key");//change to ERROR msg

			while(!KEY_Read());
			BuzOn(1);
		}

	}
}

void CalRun_ScaleType(void)
{
	INT32U ret_long;
	INT8U result;
	CAPTION_STRUCT cap;

	result= get_global_bparam(GLOBAL_AREA_SCALETYPE);
	ret_long=(INT32U)result;
#if defined(USE_CL5200_KEY)
	if(ret_long == SCALE_TYPE_BENCH_CL5200)			ret_long = SCALE_TYPE_BENCH;
	else if(ret_long == SCALE_TYPE_HANGING_CL5200)	ret_long = SCALE_TYPE_HANGING;
#elif defined(USE_CL3000_KEY)
	if(ret_long == SCALE_TYPE_BENCH_CL3000)		ret_long = SCALE_TYPE_BENCH;
#endif 

	Menu_Init();

	caption_split_by_code(0x8220, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	caption_split_by_code(0x9171, &cap,0);
	caption_adjust(&cap,NULL);

#if defined(USE_CL5200_KEY) || defined(USE_CL3000_KEY)
	cap.input_max = 3;	//scale type max 3(P, B, H)
#endif 

	Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &ret_long, NULL, NULL, CalRun_ScaleTypee_callback );
	
	Menu_SetCallbackMode(1, 1);	// below

	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) 
	{
#if defined(USE_CL5200_KEY)
		if(ret_long == SCALE_TYPE_BENCH)		ret_long = SCALE_TYPE_BENCH_CL5200;
		else if(ret_long == SCALE_TYPE_HANGING)	ret_long = SCALE_TYPE_HANGING_CL5200;
#elif defined(USE_CL3000_KEY)
		if(ret_long == SCALE_TYPE_BENCH)		ret_long = SCALE_TYPE_BENCH_CL3000;
#endif
		SetScaleType(ret_long, ON, OFF);
	}
}

void CalRun_ClearTable(void)	// mode 0:confirm, 1:no confirm
{
	char disp_string_buf[50];
	CAPTION_STRUCT	cap;
#ifdef COUNTRY_USA
	INT16U default_department;
	INT32U pnt;
	DEPARTMENT_STRUCT dept;
	INT8U default_barcode[9];
	BARCODE_STRUCT default_barsetting[9];
	INT8U i;
	INT8U struct_size;	
#endif

	VertScrollMsg_Init(DSP_MENU_FONT_ID);
	sprintf(disp_string_buf,"Discount");
	VertScrollMsg_Add(disp_string_buf);
	discount_delete_all();

	sprintf(disp_string_buf,"Department");
	VertScrollMsg_Add(disp_string_buf);
#ifdef COUNTRY_USA
	// Get Default Department
	default_department=(INT32U)get_net_wparam(NETWORK_DEPARTMENT_ID);
	plu_get_display_info(PTYPE_DEPARTMENT_NO,&cap,0);
	if ((default_department<cap.input_min) || (default_department>cap.input_max)) 
	{
		default_department = 1;
	}
	struct_size = sizeof(DEPARTMENT_STRUCT);
	pnt  =  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	pnt +=  DEPARTMENT_STRUCT_SIZE * (default_department-1);
	Flash_sread(pnt, (HUGEDATA INT8U *)&dept, struct_size);

	// Reset Department
	table1_delete_any(1,0,1);

	// Restore Default Department
	Flash_swrite(pnt, (HUGEDATA INT8U *)&dept, struct_size);	

	// Init Department# 1
	memset((INT8U *)&dept,0x00,sizeof(DEPARTMENT_STRUCT));
	dept._code     = 1;
	dept._speedkey = 1;
#ifdef USE_PERSIA_DEFAULT
	caption_split_by_code(0x9280, &cap,0);	//"default"
	sprintf((char *)dept._description, cap.form);
#else
	sprintf((char *)dept._description,"Default");
#endif
	pnt  =  DFLASH_BASE + FLASH_DEPARTMENT_AREA;
	pnt +=  DEPARTMENT_STRUCT_SIZE * (dept._code-1);
	Flash_swrite(pnt, (HUGEDATA INT8U *)&dept, struct_size);
#else
	table1_delete_any(1,0,1);
#endif

	sprintf(disp_string_buf,"Group");
	VertScrollMsg_Add(disp_string_buf);
	table1_delete_any(2,0,1);

	sprintf(disp_string_buf,"Sales MSG");
	VertScrollMsg_Add(disp_string_buf);
	table1_delete_any(3,0,1);

#ifndef COUNTRY_USA
	//미국일 때 Origin 삭제 안함
	sprintf(disp_string_buf,"Origin");
	VertScrollMsg_Add(disp_string_buf);
	table1_delete_any(4,0,1);
#endif

	sprintf(disp_string_buf,"Unit Symbol");
	VertScrollMsg_Add(disp_string_buf);
	table1_delete_any(5,0,1);

	sprintf(disp_string_buf,"TAX");
	VertScrollMsg_Add(disp_string_buf);
	table1_delete_any(6,0,1);

	sprintf(disp_string_buf,"Tare");
	VertScrollMsg_Add(disp_string_buf);
	table1_delete_any(7,0,1);

	sprintf(disp_string_buf,"Barcode Form");
	VertScrollMsg_Add(disp_string_buf);
#ifdef COUNTRY_USA
	// Get Default Barcode
	struct_size = sizeof(BARCODE_STRUCT);
	for(i=0;i<9;i++)
	{
		default_barcode[i] = get_global_bparam(GLOBAL_BARCODE_STD_WID+i);
		if(default_barcode[i] != 0 && default_barcode[i] <= MAX_BARCODE_NO)
		{
			pnt  =  DFLASH_BASE + FLASH_BARCODE_FORMAT_AREA;
			pnt +=  BARCODE_STRUCT_SIZE * (default_barcode[i]-1);
			Flash_sread(pnt, (HUGEDATA INT8U *)&default_barsetting[i], struct_size);
		}
		else
		{
			default_barcode[i] = 0;	
		}
	}
	// Reset Barcode table
	table1_delete_any(8,0,1);
	// Restore Default Barcode
	for(i=0;i<9;i++)
	{
		set_global_bparam(GLOBAL_BARCODE_STD_WID+i, default_barcode[i]);
		if(default_barcode[i] != 0)
		{
			pnt  =  DFLASH_BASE + FLASH_BARCODE_FORMAT_AREA;
			pnt +=  BARCODE_STRUCT_SIZE * (default_barcode[i]-1);
			Flash_swrite(pnt, (HUGEDATA INT8U *)&default_barsetting[i], struct_size);
		}
	}
#else
	table1_delete_any(8,0,1);
#endif	

	sprintf(disp_string_buf,"Ingredients");
	VertScrollMsg_Add(disp_string_buf);
	table2_delete_any(TABLE2_INGREDIENT,0,1);

	sprintf(disp_string_buf,"Nutri Facts");
	VertScrollMsg_Add(disp_string_buf);
	table2_delete_any(TABLE2_NUTRITION_FACT,0,1);

	sprintf(disp_string_buf,"Traceability");
	VertScrollMsg_Add(disp_string_buf);
	table2_delete_any(TABLE2_TRACEABILITY,0,1);

	sprintf(disp_string_buf,"Country");
	VertScrollMsg_Add(disp_string_buf);
	table2_delete_any(TABLE2_COUNTRY,0,1);

	sprintf(disp_string_buf,"Slaughter");
	VertScrollMsg_Add(disp_string_buf);
	table2_delete_any(TABLE2_SLAUGHTERHOUSE,0,1);

	sprintf(disp_string_buf,"Cut Hall");
	VertScrollMsg_Add(disp_string_buf);
	table2_delete_any(TABLE2_CUTTINGHALL,0,1);

	sprintf(disp_string_buf,"Store");
	VertScrollMsg_Add(disp_string_buf);
	table3_delete_any(1,0,1);

	sprintf(disp_string_buf,"Customer");
	VertScrollMsg_Add(disp_string_buf);
	table3_delete_any(2,0,1);

	sprintf(disp_string_buf,"Scroll MSG");
	VertScrollMsg_Add(disp_string_buf);
	table3_delete_any(3,0,1);

	sprintf(disp_string_buf,"Clerk ");
	VertScrollMsg_Add(disp_string_buf);
	table3_delete_any(4,0,1);

	sprintf(disp_string_buf,"Currency");
	VertScrollMsg_Add(disp_string_buf);
	table3_delete_any(5,0,1);
}




void CalRun_SetCreep(void)
{
	INT32S cmd_param;
	CAPTION_STRUCT cap;
	INT32U set_val[3];
	INT8U result;

	if(ADM_Status.Version < 100) {
		BuzOn(2);
		return;
	}

	if(ADM_Status.Version >= 101) {
		set_val[0] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_CREEP_ON, ADM_PARAM_ON);			// Creep Compensation ON
		set_val[0] %= 1000000L;
	}
	set_val[2] = ADM_WaitData(ADM_CMD_GET_PARAMETER, ADM_SUB_CREEP_NUM, ADM_PARAM_ON);			// Creep Value & sign
	set_val[2] %= 1000000L;

	if(set_val[2] > 1000) {
		set_val[1] = 0;	// '-'
		set_val[2] = set_val[2] - 1000;
	} else {
		set_val[1] = 1;	// '+'
	}

	
	Menu_Init();

	caption_split_by_code(0x8186, &cap,0);
	Menu_HeadCreate((HUGEDATA char *)cap.form);

	if(ADM_Status.Version >= 101) {
		caption_split_by_code(0x91c1, &cap,0);
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_YN, cap.input_x, cap.input_dlength, cap.input_length, 
							cap.input_max, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);
	}

	caption_split_by_code(0x91c2, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);

	caption_split_by_code(0x91c3, &cap,0);
	caption_adjust(&cap,NULL);
	Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length, 
						cap.input_max, cap.input_min, 0, &set_val[2], NULL, NULL, NULL);


	result = Menu_InputDisplay();
	if(result == MENU_RET_SAVE) {
		if(set_val[1] == 1) {
			cmd_param = set_val[2];
		} else {
			cmd_param = set_val[2] + 1000;	// negative creep
		}
		cmd_param += ADM_SUB_CREEP_NUM;//"01"
		ADM_WaitData(ADM_CMD_SET_PARAMETER, cmd_param, ADM_PARAM_ON);
		if(ADM_Status.Version >= 101) {
			cmd_param = set_val[0];
			cmd_param += ADM_SUB_CREEP_ON;//"02"
			ADM_WaitData(ADM_CMD_SET_PARAMETER, cmd_param, ADM_PARAM_ON);
		}
		ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);				// Save Settings
		Cal_flagAdmBackup = 1;
	} 
}

void CalRun_AdFirmware(void)
{
	POINT disp_p;
	char str_buf[50];
	CAPTION_STRUCT cap;
	INT16U version, old_version;
	char old;
	INT8U scale_type;
	char char_w, char_h;

	old = DspStruct.DirectDraw;
	DspStruct.DirectDraw = 0;
	Dsp_Fill_Buffer(0);

#if defined(USE_SINGLE_LINE_GRAPHIC)
	menu_display_menucode(CurMenuCode, 1);
	menu_displaytitle(0x8187, 0);
	char_h = 0;
#else
	if (Startup.menu_type == 2)
	{
		DspLoadFont1(DSP_MENU_FONT_ID);
		char_w = display_font_get_width();
		char_h = display_font_get_height();
		menu_display_menucode(CurMenuCode, 1);
		menu_displaytitle(0x8187, 0);
		menu_writepage(1, 1);
	}
	else
	{
		DspLoadFont1(DSP_SYSTEM_FONT_ID);
		char_w = display_font_get_width();
		char_h = display_font_get_height();
		caption_split_by_code(0x8187, &cap,0);
		_strupr((char *)cap.form);
		disp_p = point(0, 2*char_w);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form+3, strlen(cap.form+3), 0, 0); 
		hebrew_codetemp[strlen(cap.form+3)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form+3, strlen(cap.form+3), 0, 0, 0); 
		arab_codetemp[strlen(cap.form+3)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
		Dsp_Write_String(disp_p, (HUGEDATA char *)cap.form+3);
#endif
	}
#endif
	Dsp_Diffuse();

	DspStruct.DirectDraw = 1;

	old_version = (INT16U)(ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF) % 1000);	// Version = "1023" + VER[3]
	#ifdef USE_ARAB_CONVERT
	sprintf(str_buf, "V%d.%02d : CUR", old_version/100, old_version%100);
#else
	sprintf(str_buf, "CUR : V%d.%02d", old_version/100, old_version%100);
#endif
	disp_p = point(Startup.menu_type*char_h, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)str_buf, strlen(str_buf), 0, 0); 
	hebrew_codetemp[strlen(str_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)str_buf, strlen(str_buf), 0, 0, 0); 
	arab_codetemp[strlen(str_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, str_buf);
#endif

	while(1) {
		if(KEY_Read()) {
			if(KeyDrv.CnvCode == KP_TEST) {
				BuzOn(1);
				break;
			} else if(KeyDrv.CnvCode == KP_ESC) {
				BuzOn(1);
				return;
			} else {
				BuzOn(2);
			}
		}
	}
		
//	sprintf(str_buf, "[A/D Debug Mode]");
	sprintf(str_buf, "[A/D Debug]");
	disp_p = point((Startup.menu_type+2)*char_h, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)str_buf, strlen(str_buf), 0, 0); 
	hebrew_codetemp[strlen(str_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)str_buf, strlen(str_buf), 0, 0, 0); 
	arab_codetemp[strlen(str_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, str_buf);
#endif

	ad_firmware_download();

	version = (INT16U)(ADM_WaitData(ADM_CMD_GET_VERSION, 0, ADM_PARAM_OFF) % 1000);	// Version = "1023" + VER[3]
	ADM_Status.Version = version;
#ifdef USE_ARAB_CONVERT
	sprintf(str_buf, "V%d.%02d :NEW", version/100, version%100);
#else
	sprintf(str_buf, "NEW : V%d.%02d", version/100, version%100);
#endif
	disp_p = point((Startup.menu_type+1)*char_h, 0);
#ifdef HEBREW_FONT
	Convert_HebrewPreview((INT8U *)str_buf, strlen(str_buf), 0, 0); 
	hebrew_codetemp[strlen(str_buf)] = 0;
	disp_p.x = 202 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
	Convert_ArabPreview((INT8U *)str_buf, strlen(str_buf), 0, 0, 0); 
	arab_codetemp[strlen(str_buf)] = 0;
	disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
	Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
	Dsp_Write_String(disp_p, str_buf);
#endif

	while(1)
	{
		if(KEY_Read()) {
			BuzOn(1);
			if(version != old_version && version != 0xffff) {
				scale_type = get_global_bparam(GLOBAL_AREA_SCALETYPE);
				CalRun_InitAdFilter(scale_type);
				ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);
			}
			break;
		}		
	}
	DspStruct.DirectDraw = old;
	DspLoadFont1(DSP_MENU_FONT_ID);
}

extern INT8U KeyCheckNetwork;
void CalRun_ComTest(void)
{
	POINT disp_p;
	INT8U str[10];
	INT8U old;
	INT16U disp_max;
	char char_w, char_h, x, y;
	COMM_BUF *CBuf;
	INT8U loop;
	INT16U cap_id;
	INT8U send_flag, disp_flag;
	INT8U com_mode = 0;	// 0:RS232C, 1:Client, 2:Server
	INT8U change_mode;
	CAPTION_STRUCT cap;
	char disp_str[34];
	INT8U term_start_line;
	char old_font;

	send_flag = OFF;
	disp_flag = OFF;
	
	if (status_run.run_mode == RUN_CALIBRATION) {
		cap_id = 0x8590;
	} else {
		cap_id = 0x1868;
	}

	old_font = DspStruct.Id1; 
#ifdef USE_CHN_FONT
	DspLoadFont1(DSP_MEDIUM_FONT_ID);
#else
	DspLoadFont1(DSP_SMALL_FONT_ID);
#endif
	char_w = display_font_get_width();
	char_h = display_font_get_height();

	old = DspStruct.DirectDraw;

	disp_max = Dsp_GetChars();

	CBuf = &CommBuf;
#if defined(USE_SINGLE_LINE_GRAPHIC)
	term_start_line = 1;
	disp_max -= 1;
#else
	term_start_line = Startup.menu_type;
	if(Startup.menu_type == 0) term_start_line = 1;
#endif

	loop = ON;
	change_mode = ON;
	KeyCheckNetwork = OFF;
	while (loop)
	{
		if (change_mode)
		{
			change_mode = OFF;
			DspStruct.DirectDraw = 0;
			Dsp_Fill_Buffer(0);
			menu_display_menucode(CurMenuCode, 1);
			caption_split_by_code(cap_id, &cap, 0);
#if defined(USE_SINGLE_LINE_GRAPHIC)
			sprintf(disp_str, cap.form);
#else
#ifdef HEBREW_FONT
			sprintf(disp_str, "%s)%d(", cap.form, com_mode);
#else
			sprintf(disp_str, "%s(%d)", cap.form, com_mode);
#endif
#endif
			menu_writetitle_str(&disp_str[0]);
			menu_writepage(1, 1);
			Dsp_Diffuse();
			DspStruct.DirectDraw = 1;
			x = 0;
			y = 0;
			switch (com_mode)
			{
				case 0:
					CBuf = &CommBuf;
#if defined(USE_SINGLE_LINE_GRAPHIC)
					display_clear(DISPLAY_UNITPRICE);
					display_string(DISPLAY_UNITPRICE, (INT8U*)"SERIAL");
#endif
					break;
				case 1:
					CBuf = &(CommBufEthData[0]);
					if (Startup.global_com&0x01) Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL);
#if defined(USE_SINGLE_LINE_GRAPHIC)
					display_clear(DISPLAY_UNITPRICE);
					display_string(DISPLAY_UNITPRICE, (INT8U*)"CLIENT");
#endif
					break;
				case 2:
					CBuf = &(CommBufEthData[1]);
#if defined(USE_SINGLE_LINE_GRAPHIC)
					display_clear(DISPLAY_UNITPRICE);
					display_string(DISPLAY_UNITPRICE, (INT8U*)"SERVER");
#endif
					break;
			}
			Dsp_Diffuse();
		}
		if (KEY_Read()) 
		{
			BuzOn(1);
			switch (KeyDrv.Type) 
			{
				case KEY_TYPE_ASCII:
					str[0] = (KeyDrv.CnvCode - KP_ASC_00) & 0xff;
					str[1] = 0x00;
					send_flag = ON;
					disp_flag = ON;
					break;
				case KEY_TYPE_NUMERIC:
					str[0] = KeyDrv.CnvCode - KEY_NUM_0 + '0';
					str[1] = 0x00;
					send_flag = ON;
					disp_flag = ON;
					break;
				default:
				case KEY_TYPE_FUNCTION:
					switch (KeyDrv.CnvCode) 
					{
						case KP_ESC: 
							loop = OFF;
							break;
						case KP_CLEAR:
							change_mode = ON;
							break;
						case KP_PAGE_UP:
							change_mode = ON;
							if(com_mode == 2) com_mode = 0;
							else com_mode++;
							break;
						case KP_PAGE_DN:
							change_mode = ON;
							if(com_mode == 0) com_mode = 2;
							else com_mode--;
							break;
						default:
							BuzOn(2);
							break;
					}
					break;
			}
		}
		else 
		{
			EthDataProc((HUGEDATA COMM_BUF *)CBuf);	
			if (CheckRxBuf((HUGEDATA COMM_BUF *)CBuf))
			{
				str[0] = GetCharRxBuf((HUGEDATA COMM_BUF *)CBuf);
				str[1] = 0x00;
				disp_flag = ON;
			}
		}

		if (send_flag)
		{
			send_flag = OFF;
			if (com_mode == 1 && (Startup.global_com&0x01)) Ethernet_Server(CLIENT_OPEN, 0, CLIENT_NORMAL);
			commun_outleng((HUGEDATA COMM_BUF *)CBuf, (char *)str, 1);
		}
		if (disp_flag)
		{
			disp_flag = OFF;
			disp_p.x = char_w * x;
			disp_p.y = char_h * (y + term_start_line);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)str, strlen(str), 0, 0); 
			hebrew_codetemp[strlen(str)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_CONVERT)
			Convert_ArabPreview((INT8U *)str, strlen((char *)str), 0, 0, 0); 
			arab_codetemp[strlen((char *)str)] = 0;
			disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
			Dsp_Write_String(disp_p, (HUGEDATA char *)str);
#endif
			x++;
			if (x > disp_max)
			{
				x = 0;
				y++;
				if (y >= Startup.line_max)
				{
					y = 0;
				}
				if (Startup.menu_type == 0) y = 0;	// 1line만 사용
			}
		}
	}
	if (Startup.global_com&0x01) Ethernet_Server(CLIENT_CLOSE, 0, CLIENT_NORMAL);

	KeyCheckNetwork = ON;

	DspStruct.DirectDraw = old;
	DspLoadFont1(old_font);
	
}
