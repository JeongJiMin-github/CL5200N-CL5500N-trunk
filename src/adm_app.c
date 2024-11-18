/*****************************************************************************
|*			
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	adm_app.c
|*  Version			:	0.1
|*  Programmer(s)	:	Chung jong kwon (CJK)
|*  Created			:	2003/03/20
|*  Description		:	CLP ad module routine    
|*				
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

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
#include "prt_interpreter.h"

#include "adm.h"
#include "main.h"
#include "initial.h"
#include "adm_app.h"
#include "initial_glb.h"
#include "common.h"
#include "dsp_app.h"

INT32U ad_max_capa[5]={ 3000,6000,15000,30000,60000 };
INT8U  ad_interval[6]={ 1,2,5,10,20,50 };
// div_fact_kg : 0.001kg 기준에서 display에 맞게 digit 조정 
//              ex) 60kg,dual일 때, "12.34kg"
//                  무게값이 12340이며, 10으로 나눠서 1234로 표시하고 decimal을 2번째 자리에 표시
// div_fact_lb : 0.001lb 기준에서 display에 맞게 digit 조정 
//              ex) 30lb,single일 때, "12.45lb
//                  무게값이 12450이면, 10으로 나눠서 1245로 표시하고 decimal을 2번째 자리에 표시
INT8S div_fact_kg[2][10]={ { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
INT8S div_fact_lb[2][10]={ { 0, 1, 1, 0, 1, 1, 0, 1, 1, 0}, { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}};
INT8S div_fact_g[2][10] ={ { 0, 0, 0, 0,-1,-1,-2,-2,-2,-3}, { 0, 0, 0, 0,-1,-1,-2,-2,-2,-3}};
INT8U decimal_kg[2][10] ={ { 3, 3, 2, 2, 2, 1, 1, 1, 0, 0}, { 3, 3, 3, 2, 2, 2, 1, 1, 1, 0}};
INT8U decimal_lb[2][10] ={ { 3, 2, 2, 2, 1, 1, 1, 0, 0, 0}, { 3, 3, 2, 2, 2, 1, 1, 1, 0, 0}};
extern char	gmsg[];
extern void VertScrollMsg_Add(const char *str);

INT8U get_ad_bparam(INT32U addr)
{
	INT32U pnt;
	INT8U cc;
	pnt = DFLASH_BASE + FLASH_ADC_AREA;
	pnt+=addr;
	cc=Flash_bread(pnt);
	return cc;
}

INT16U get_ad_wparam(INT32U addr)
{
	INT16U val;
	val = get_ad_bparam(addr+1);
	val <<= 8;
	val |= get_ad_bparam(addr); 			
	return val;
}

INT32U get_ad_lparam(INT32U addr)
{
	INT32U val;
	
//endian 수정 
	//val = get_ad_bparam(addr);
	//val <<= 8;
	//val |= get_ad_bparam(addr+1);
	//val <<= 8;
	//val |= get_ad_bparam(addr+2);
	//val <<= 8;
	//val |= get_ad_bparam(addr+3);
	val = get_ad_bparam(addr+3);
	val <<= 8;	
	val |= get_ad_bparam(addr+2);
	val <<= 8;
	val |= get_ad_bparam(addr+1);
	val <<= 8;
	val |= get_ad_bparam(addr);

	return val;
}


INT32S ad_cnv_main2adm(INT32S val)
{
	if (ADM_Status.Capa == ADM_CUSTOM_CAPA_ID) return val; // custom capa일 때 조정없음
	if(CurPluWeightUnit == WEIGHT_UNIT_LB) {
		if(div_fact_lb[ADM_Status.Dual][ADM_Status.Capa] == 1)       val *= 10L;
		else if(div_fact_lb[ADM_Status.Dual][ADM_Status.Capa] == -1) val /= 10L;
	} else {
		if(div_fact_kg[ADM_Status.Dual][ADM_Status.Capa] == 1)       val *= 10L;
		else if(div_fact_kg[ADM_Status.Dual][ADM_Status.Capa] == -1) val /= 10L;
	}
	return val;
}

INT32S ad_cnv_adm2main(INT32S val)
{
	if (ADM_Status.Capa == ADM_CUSTOM_CAPA_ID) return val; // custom capa일 때 조정없음
	if(CurPluWeightUnit == WEIGHT_UNIT_LB) {
		if(div_fact_lb[ADM_Status.Dual][ADM_Status.Capa] == 1)       val /= 10L;
		else if(div_fact_lb[ADM_Status.Dual][ADM_Status.Capa] == -1) val *= 10L;
	} else {
		if(div_fact_kg[ADM_Status.Dual][ADM_Status.Capa] == 1)       val /= 10L;
		else if(div_fact_kg[ADM_Status.Dual][ADM_Status.Capa] == -1) val *= 10L;
	}
	return val;
}


INT8U  adm_set_weighttare(long *v) 
{
	INT8U rdata;
	rdata = ADM_WaitResult(ADM_CMD_SET_TARE, 0, ADM_PARAM_OFF);	// Set Tare
	switch (rdata) {
		case 0x00 : 
			*v=ADM_WaitData(ADM_CMD_GET_TARE_EXT, 0, ADM_PARAM_OFF);
			*v = ad_cnv_adm2main(*v);
			if (*v<0) return 9;
			if (*v>9999l) {
				ADM_WaitResult(ADM_CMD_SET_PRESET_TARE, 0, ADM_PARAM_ON);
				*v = 0;
			    	return 1;	// Added by CJK 20060220
			}
			return 0;	// OK
		case 0x02 : return 1;	// Tare Overrange(+,-)
		case 0x03 : return 2;	// Invalid Value	// not used at here
		case 0x04 : return 3;	// Unstable
		case 0x05 : return 4;	// Already Tare
		case 0x06 : return 5;	// No Tare
		default :
		case 0xff : return 9;	// Communication Error
	}
}

INT8U  ad_set_zero(void)
{
	INT8U rdata;
#ifdef USE_SRILANKA_ZERO
	if(!ad_check_rezero(status_scale.Weight)) return 1;	// Srilanka zero range 적용(인증전용) : by CJK 20060927
#endif
	rdata = ADM_WaitResult(ADM_CMD_SET_ZERO, 0, ADM_PARAM_OFF);	// Set Zero
	switch(rdata) {
		case 0x00 : return 0;	// OK
		case 0x02 : return 1;	// Zero Overrange(+,-)
		//case 0x03 : return 2;	// Invalid Value
		case 0x04 : return 3;	// Unstable
		case 0x05 : return 4;	// Already Tare
		//case 0x06 : return 5;	// No Tare
		default :
		case 0xff : return 9;	// Communication Error
	}
}

/////////////////////////////
// return value 
//	0:Success, 
//	1:tarelimit over, 
//	2:invalid value
//	3:communication(?)
//	4:unstable
INT8U  ad_set_presettare(long v)			// Key-in Tare , Preset(PLU) Tare
{
	INT8U result;

	result = ad_check_tarevalue(v);	// result  0:Success, 1:tarelimit over, 2:invalid value, 3:over display length

	if (result == 0) {

		v = ad_cnv_main2adm(v);
		result = ADM_WaitResult(ADM_CMD_SET_PRESET_TARE, v, ADM_PARAM_ON);
		switch(result) {
			case 0x00 : return 0;	// OK
			case 0x02 : return 1;	// Tare Overrange(+,-)
			case 0x03 : return 2;	// Invalid Value
			case 0x04 : return 3;	// Unstable			// not used at here
			case 0x05 : return 4;	// Already Tare
			case 0x06 : return 5;	// No Tare
			default :
			case 0xff : return 9;	// Communication Error
		}
	} else {	// result : 1 or 2
		if(result == 3) result = 1;
		return result;
	}
}

INT8U ad_get_multi_interval(void)
{	
	INT8U num_int;

	num_int = get_ad_bparam(GLOBAL_AD_MIX_2);
	num_int = (num_int&0x10)>>4;   	// 0,1
	return num_int;
}

INT8U ad_get_unit_cal(void)
{	
	INT8U cal_unit;

	cal_unit = get_ad_bparam(GLOBAL_AD_MIX_1);
	cal_unit = (cal_unit&0x40)>>6;
	return cal_unit;
}

INT8U ad_get_unit_weigh(void)
{	
	INT8U use_unit;

	use_unit = get_ad_bparam(GLOBAL_AD_MIX_1);
	use_unit = (use_unit&0x80)>>7;
	return use_unit;
}

INT8U  ad_get_kglb(void)		// return  0:kg, 1:lb, 2:g
{

	return ADM_GetData.CurUnit;
}

INT8U adm_get_unit_cal(void)		// return  0:kg(g), 1:lb
{
	ADM_Status.CalUnit = (INT8U)ADM_WaitData(ADM_CMD_GET_W_UNIT_CAL, 0, ADM_PARAM_OFF);

	return ADM_Status.CalUnit;
}

void adm_set_unit_cal(INT8U unit)	// return  0:kg(g), 1:lb
{
	ADM_WaitData(ADM_CMD_SET_W_UNIT_CAL, (INT32U)unit, ADM_PARAM_ON);		// Set CalUnit(New)
	ADM_Status.CalUnit = unit;
}

INT8U adm_get_unit_weigh(void)		// return  0:kg(g), 1:lb
{
     	ADM_Status.UseUnit = (INT8U)ADM_WaitData(ADM_CMD_GET_W_UNIT_USE, 0, ADM_PARAM_OFF);
	return ADM_Status.UseUnit;
}

void adm_set_unit_weigh(INT8U unit)	// return  0:kg(g), 1:lb
{
	ADM_GetData.CurUnit = unit;
	if(unit >= WEIGHT_UNIT_G) unit = WEIGHT_UNIT_KG;
 	ADM_WaitData(ADM_CMD_SET_W_UNIT_USE, (INT32U)unit, ADM_PARAM_ON);		// Set UseUnit
	ADM_Status.UseUnit = unit;
	CurPluWeightUnit = ADM_GetData.CurUnit;	// Added by CJK 20060221
}


INT8U adm_get_unit_cur(void)		// return  0:kg, 1:lb, 2:g
{
	INT8U cur_unit;

	cur_unit = (INT8U)ADM_WaitData(ADM_CMD_GET_W_UNIT_CUR, 0, ADM_PARAM_OFF);
	if(cur_unit == WEIGHT_UNIT_KG) {
		if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) cur_unit = WEIGHT_UNIT_G;
	}

	return cur_unit;
}

INT8U adm_set_unit_cur(INT8U mode)	// mode 0:kg, 1:lb, 2:g
{
	long rdata;
	INT8U temp;

	temp = mode;
	if(mode < WEIGHT_UNIT_KG || mode > WEIGHT_UNIT_LB) {
		temp = WEIGHT_UNIT_KG;
	}

	rdata = ADM_WaitData(ADM_CMD_SET_W_UNIT_CUR, temp, ADM_PARAM_ON);	// 0:kg, 1:lb

	if(rdata == -1) return FALSE;
	ADM_GetData.CurUnit = mode;
	CurPluWeightUnit = ADM_GetData.CurUnit;
	return TRUE;
}

INT8U adm_get_capa_id(void)		// return  0:kg(g), 1:lb
{
     	ADM_Status.Capa = (INT8U)ADM_WaitData(ADM_CMD_GET_CAPACITY, 0, ADM_PARAM_OFF);
	return ADM_Status.Capa;
}
    
INT8U ad_get_capa_idx(void)
{	
	INT8U capa_idx;

	capa_idx  = get_ad_bparam(GLOBAL_AD_MIX_2);//GLOBAL_AD_MIX_2
	capa_idx  = (capa_idx&0x0f);	  // 0,1,2,3				
	// kg = 0:6kg(15lb), 1:15kg(30lb), 2:30kg(60lb)
	// lb = 0:15lb(6kg), 1:30lb(15kg), 2:60lb(30kg)
	// 10:custom capa
	if(capa_idx > ADM_MAX_CAPA_ID) {
		capa_idx = 1;	// 15kg
	}
	return capa_idx;
}


INT32U ad_get_max_capa(INT8U adm_capa, INT8U unit, INT8U is_mid_pt)
{
	INT8U capa_id;
	INT32U ret;

	if(unit >= WEIGHT_UNIT_G) unit = WEIGHT_UNIT_KG;

	if (ADM_Status.Version >= ADM_CUSTOM_CAPA_VER) {
		if (is_mid_pt == 1) return ADM_Status.Max1[unit];
		else return ADM_Status.Max2[unit];
	}

	capa_id = adm_capa%3;
	capa_id++;
	capa_id += unit;
	capa_id -= is_mid_pt;

	ret = ad_max_capa[capa_id];

	return ret;	
}


INT8U ad_get_interval(INT8U range_pt)	// range_pt - 0: low range, 1: high range
{
	INT8U capa_id;
	INT8U ret, cur_unit;

	cur_unit = CurPluWeightUnit;
	if(cur_unit >= WEIGHT_UNIT_G) cur_unit = WEIGHT_UNIT_KG;

	if (ADM_Status.Version >= ADM_CUSTOM_CAPA_VER) {
		if (range_pt == 0) return ADM_Status.d1[cur_unit];
		else return ADM_Status.d2[cur_unit];
	}

	capa_id = (ADM_Status.Capa%3);
	capa_id++;
	capa_id += cur_unit;
	if(ADM_Status.Dual) {
		capa_id -= (1-range_pt);
	} else {
	}
	ret = ad_interval[capa_id];

	return ret;	
}


INT32U ad_get_capa(void)	// Adjust digit for display
{
	INT32U ret;

	ret = ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 0);
	ret = ad_cnv_adm2main(ret);

	return ret; //ad_max_capa[capa_idx];
}

INT32S ad_cnv_to_internal(INT32S ext_val)
{
	INT32S int_val;//, int_resol;
	INT32S max_weight;

	max_weight = ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 0);
	max_weight = ad_cnv_adm2main(max_weight);

	int_val = ADM_INT_RESOLUTION;			// 외부값->내부값
	int_val /= max_weight;//ad_max_capa[capa_idx];
	int_val *= ext_val;
	return int_val;
}


INT32S ad_cnv_to_external(INT32S int_val)
{
	INT32S ext_val, mid_weight, interval;
	INT8U range_pt;

	mid_weight = ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 1);
	mid_weight = ad_cnv_adm2main(mid_weight);

	range_pt = 1;
	if(ADM_Status.Dual == 1) {
		if(int_val < ad_cnv_to_internal(mid_weight)) {
			range_pt = 0;
		}
	}

	interval = (INT32S)ad_get_interval(range_pt);
	interval = ad_cnv_adm2main(interval);

	ext_val = int_val*10L / ad_cnv_to_internal(interval);
	if(ext_val < 0) {
		ext_val -= 5;
	} else {
		ext_val += 5;
	}
	ext_val /= 10L;
	ext_val *= interval;

	return ext_val;
}


INT8U ad_check_tarelimit(long val)		// 15.333... : PLU Edit,Key Tare,
{
	INT32U tarelimit, inter_val;

	inter_val = ad_cnv_to_internal(val);		// 외부값->내부값

	tarelimit = get_ad_lparam(GLOBAL_AD_TARE_LIMIT);//ad_get_tarelimit();

	if(ADM_Status.Version >= 106) {	// TarelimitLB
		if(CurPluWeightUnit == WEIGHT_UNIT_LB) {	// Added by CJK 20060223
			tarelimit = get_ad_lparam(GLOBAL_AD_TARE_LIMIT_LB);//ad_get_tarelimit();
		}
	}

	if(inter_val > tarelimit){
		return FALSE;			// over range
	} else return TRUE;  			// Success
}


INT8U ad_check_weight_value(long val)
{
	INT8U range_pt;
	INT32U mid_weight, interval;

	mid_weight = ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 1);
	mid_weight = ad_cnv_adm2main(mid_weight);

	range_pt = 1;
	if(ADM_Status.Dual == 1) {
		if(val < mid_weight) {	// dual 일 경우 max1보다크면 다음 interval 적용
			range_pt = 0;	// Lower Range
		}
	}

	interval = ad_get_interval(range_pt);
	interval = ad_cnv_adm2main(interval);

	if(!interval) return FALSE;
	if(val%interval) return FALSE;		// invalid value
	else return TRUE;					// Success
}

INT8U ad_check_tare_displen(INT32U val)
{
	
	if(ilog(val) > display_parameter.mode_len[DISPLAY_WEIGHT] - 1) return FALSE;
	else return TRUE;
}

INT8U ad_check_tarevalue(long v)
{
	if(!ad_check_tarelimit(v)) return 1;		// over tarelimit
	else if(!ad_check_weight_value(v)) return 2; 	// invalid value
	else if(!ad_check_tare_displen(v)) return 3;	// over display length
	else return 0;					// success
}

INT8U ad_get_accumul_tare(void)
{
	INT8U flag;

	flag = get_base_bparam(FLASH_ADC_TARE_CONFIG);
	if(flag & 0x80)	return ON;
	return OFF;
}

void ad_set_accumul_tare(INT8U val)
{
	INT8U flag;

	flag = get_base_bparam(FLASH_ADC_TARE_CONFIG);

	if(val) flag |= 0x80;
	else flag &= 0x7f;

	set_base_bparam(FLASH_ADC_TARE_CONFIG, flag);
}

INT8U ad_get_subtract_tare(void)
{
	INT8U flag;

	flag = get_base_bparam(FLASH_ADC_TARE_CONFIG);
	if(flag & 0x40)	return ON;
	return OFF;
}


void ad_set_subtract_tare(INT8U val)
{
	INT8U flag;

	flag = get_base_bparam(FLASH_ADC_TARE_CONFIG);

	if(val) flag |= 0x40;
	else flag &= 0xbf;

	set_base_bparam(FLASH_ADC_TARE_CONFIG, flag);
}

INT8U adm_save_tarelimit(INT32U val, INT8U wunit)
{
	INT32U ltemp, interval;
	INT8U range_pt;
	INT32U mid_weight;
	INT8U backup_weightunit;

	backup_weightunit = CurPluWeightUnit;
	CurPluWeightUnit = wunit;//ADM_Status.UseUnit;

	if(ad_check_weight_value(val) == FALSE || ad_get_capa() < val) {
		CurPluWeightUnit = backup_weightunit;
		return FALSE;
	}
	ltemp = ad_cnv_to_internal(val);

	mid_weight = ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 1);
	mid_weight = ad_cnv_adm2main(mid_weight);
	range_pt = 1;
	if(ADM_Status.Dual == 1) {
		if(val < mid_weight) {
			range_pt = 0;	// Lower Range
		}
	}
	interval = ad_get_interval(range_pt);
	interval = ad_cnv_adm2main(interval);	// Added by CJK 20060221
	interval = ad_cnv_to_internal(interval);

	ltemp += (interval/2-1);

	if(ADM_Status.Version >= 106) {	// TarelimitLB
		if(wunit == WEIGHT_UNIT_LB) {	// Added by CJK 20060223
			ADM_WaitData(ADM_CMD_SET_TARESET_LB_R, ltemp, ADM_PARAM_ON);
			ADM_ChangeLongFlash(GLOBAL_AD_TARE_LIMIT_LB, ltemp);
		} else {
			ADM_WaitData(ADM_CMD_SET_TARESET_R, ltemp, ADM_PARAM_ON);
			ADM_ChangeLongFlash(GLOBAL_AD_TARE_LIMIT, ltemp);
		}
	} else {
		if(wunit == ADM_Status.UseUnit) {
			ADM_WaitData(ADM_CMD_SET_TARESET_R, ltemp, ADM_PARAM_ON);
			ADM_ChangeLongFlash(GLOBAL_AD_TARE_LIMIT, ltemp);
		}
	}

	ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);			// Save Settings

	CurPluWeightUnit = backup_weightunit;
	return TRUE;
}

void ad_set_default(INT8U country)
{
	INT8U err;
	INT8U initZero;
	INT8U reZero;
    
	VertScrollMsg_Add("Chk ADM");
	err = ADM_CheckModuleAndGetVersion();
	if (err == ADM_CHK_ERR_COM || err == ADM_CHK_ERR_MODULE)
	{
		VertScrollMsg_Add("AD not exist");
		return;
	}
	ADM_CheckCsLocation();

	VertScrollMsg_Add("Default ADM");
	Uart1Init(BAUD_RATE_57600); 	// ADM
	switch(country) {
		case COUNTRY_UN  :
			adm_set_unit_cal(WEIGHT_UNIT_KG);
			adm_set_unit_weigh(WEIGHT_UNIT_KG);
			set_base_bparam(FLASH_ADC_WUNIT, WEIGHT_UNIT_KG);
			break;
		case COUNTRY_KR  :
			adm_set_unit_cal(WEIGHT_UNIT_KG);
			adm_set_unit_weigh(WEIGHT_UNIT_KG);
			if (adm_get_capa_id() <= 2)	// CJK 090313
			{
				set_base_bparam(FLASH_ADC_WUNIT, WEIGHT_UNIT_G);
			}
			else
			{
				set_base_bparam(FLASH_ADC_WUNIT, WEIGHT_UNIT_KG);
			}
			break;
		case COUNTRY_US  :
#ifdef USE_CANADA_CALUNIT
			adm_set_unit_cal(WEIGHT_UNIT_LB);
			adm_set_unit_weigh(WEIGHT_UNIT_KG);
			set_base_bparam(FLASH_ADC_WUNIT, WEIGHT_UNIT_KG);
#else
			adm_set_unit_cal(WEIGHT_UNIT_LB);
			adm_set_unit_weigh(WEIGHT_UNIT_LB);
			set_base_bparam(FLASH_ADC_WUNIT, WEIGHT_UNIT_LB);
#endif
			break;
		case COUNTRY_EU  :
			adm_set_unit_cal(WEIGHT_UNIT_KG);
			adm_set_unit_weigh(WEIGHT_UNIT_KG);
			set_base_bparam(FLASH_ADC_WUNIT, WEIGHT_UNIT_KG);
			break;
		case COUNTRY_EUT :
			adm_set_unit_cal(WEIGHT_UNIT_KG);
			adm_set_unit_weigh(WEIGHT_UNIT_KG);
			set_base_bparam(FLASH_ADC_WUNIT, WEIGHT_UNIT_KG);
			break;
		case COUNTRY_RU  :
		case COUNTRY_ETC :
		default :
			adm_set_unit_cal(WEIGHT_UNIT_KG);
			adm_set_unit_weigh(WEIGHT_UNIT_KG);
			set_base_bparam(FLASH_ADC_WUNIT, WEIGHT_UNIT_KG);
			break;
	}
	ADM_WaitData(ADM_CMD_SET_INIT_ZR_R, INIT_ZERO_VALUE, ADM_PARAM_ON);	//Set init zero value to ADM
	ADM_WaitData(ADM_CMD_SET_REZERO_R, REZERO_VALUE, ADM_PARAM_ON);	//Set rezero value to ADM
	initZero = INIT_ZERO_VALUE;
	reZero = REZERO_VALUE;
	ADM_ChangeByteFlash(GLOBAL_AD_INIT_ZERO_RANGE, (INT8U)initZero);	//Set init zero value to Main
	ADM_ChangeByteFlash(GLOBAL_AD_REZERO_RANGE, (INT8U)reZero );	//Set rezero value to Main
    
	VertScrollMsg_Add("Save ADM Set");
	ADM_WaitData(ADM_CMD_SAVE_SETS, 83419, ADM_PARAM_ON);
	VertScrollMsg_Add("Backup ADM");
	ADM_BackupEeprom();
}

INT32U ad_select_tarelimit(INT8U tarelimit_mode, INT32U tarelimit_value, INT8U wunit)
{
	INT8U mode;
	INT8U dual_mode;
	INT32U limit = 0;
	INT8U capa_idx;
	
	capa_idx = ad_get_capa_idx();
	mode = wunit;
	if(mode == WEIGHT_UNIT_G) mode = WEIGHT_UNIT_KG;
	mode *=3;	// 0:kg, 1:lb
	mode += (capa_idx%3);
	// 0 : 6kg,   60kg,  600kg, 6000kg
	// 1 : 15kg, 150kg, 1500kg
	// 2 : 30kg, 300kg, 3000kg
	// 3 : 15lb, 150lb, 1500lb
	// 4 : 30lb, 300lb, 3000lb
	// 5 : 60lb, 600lb, 6000lb
	if (capa_idx == ADM_CUSTOM_CAPA_ID)
	{
		limit = ad_get_capa(); // full tare
		return limit;
	}

	dual_mode = ad_get_multi_interval();

	switch(tarelimit_mode) {
		case AD_FULL_TARE :			      //se-hyung 20080325
			switch(mode) {
				case AD_CAPA_6KG :
				case AD_CAPA_60LB :
					if(dual_mode == 1) {// Dual
						limit = 6000;	// 2.999/29.99	(capa 6/60)
					} else {		// Single
						limit = 6000;	// 2.998/29.98	(capa 6/60)
					}

					break;
				case AD_CAPA_15KG :
				case AD_CAPA_15LB :
					if(dual_mode == 1) {// Dual
						limit = 9995;	// 5.998	(capa 15)
					} else {		// Single
						limit = 9995;	// 5.995	(capa 15)
					}
					break;
				case AD_CAPA_30KG :
				case AD_CAPA_30LB :
					if(dual_mode == 1) {// Dual
						limit = 9995;	// 5.998	(capa 15)
					} else {		// Single
						limit = 3000;	// 5.995	(capa 15)
					}
					break;
			}
			break;
		case AD_HALF_TARE :
			switch(mode) {
				case AD_CAPA_6KG :
				case AD_CAPA_60LB :
					limit = 2999;	// 2.999/29.99	(capa 6/60)
					break;
				case AD_CAPA_15KG :
				case AD_CAPA_15LB :
					limit = 5998;	// 5.998	(capa 15)
					break;
				case AD_CAPA_30KG :
				case AD_CAPA_30LB :
					limit = 14995;	// 9.995	(capa 30)
					break;
			}
			break;
		case AD_PROPER_TARE :
			switch(mode) {
				case AD_CAPA_6KG :
				case AD_CAPA_60LB :
					if(dual_mode == 1) {// Dual
						limit = 2999;	// 2.999/29.99	(capa 6/60)
					} else {		// Single
						limit = 2998;	// 2.998/29.98	(capa 6/60)
					}
					break;
				case AD_CAPA_15KG :
				case AD_CAPA_15LB :
					if(dual_mode == 1) {// Dual
						limit = 5998;	// 5.998	(capa 15)
					} else {		// Single
						limit = 5995;	// 5.995	(capa 15)
					}
					break;
				case AD_CAPA_30KG :
				case AD_CAPA_30LB :
					if(dual_mode == 1) {// Dual
						limit = 9995;	// 9.995	(capa 30)
					} else {		// Single
						limit = 1499;	// 14.99	(capa 30)
					}
					break;
			}
			break;
		case AD_CUSTOM_TARE :
			limit = tarelimit_value;
			break;
	}
	return limit;
}


void ad_set_tarelimit_default(INT8U tarelimit_mode, INT32U tarelimit_value, INT8U wunit)
{
	INT32U limit;

	limit = ad_select_tarelimit(tarelimit_mode, tarelimit_value, wunit);

	adm_save_tarelimit(limit, wunit);
}


long ad_cnv_weight_value(long val, INT8U round_type)	// 무게값을 Interval 에 맞게 표현하도록 변환
{
	INT8U range_pt;
	INT32S mid_weight, interval;

	mid_weight = ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 1);
	mid_weight = ad_cnv_adm2main(mid_weight);

	range_pt = 1;
	if(ADM_Status.Dual == 1) {
		if(labs(val) < mid_weight) {
			range_pt = 0;	// Lower Range
		}
	}

	interval = ad_get_interval(range_pt);
	interval = ad_cnv_adm2main(interval);

	if(round_type == 0) return val;	// No Round

	val *= 10L;
	val = val/interval;
	switch(round_type) {
		default :
		case 1 :	// Round Off	-1:-1.4~-0.5, 0:-0.4~0.4, 1:0.5~1.4
			val += 5;
			if(val < 0) val -= 10;
			break;
		case 2 :	// Round Down
			//val = val;
			break;
		case 3 :	// Round Up
			val += 9;
			if(val < 0) val -= 18;
			break;
	}
	val /= 10L;
	val = val*interval;

	return val;
}

#ifdef USE_SRILANKA_ZERO
INT8U ad_get_rezero_range(void)	// Srilanka zero range 적용(인증전용) : by CJK 20060927
{
	return get_ad_bparam(GLOBAL_AD_REZERO_RANGE);
}

INT8U ad_check_rezero(INT32S weight)	// Srilanka zero range 적용(인증전용) : by CJK 20060927
{
	INT32S max_capa_weight, reference_weight;
	INT8U rezero_range;

	max_capa_weight = ad_get_max_capa(ADM_Status.Capa, CurPluWeightUnit, 0);
	max_capa_weight = ad_cnv_adm2main(max_capa_weight);
	rezero_range = ad_get_rezero_range();
	reference_weight = (max_capa_weight * rezero_range + 50) / 100;

	if(weight > reference_weight || weight < -reference_weight) return FALSE;

	return TRUE;
}
#endif

long ad_cnv_weight_unit(long val, INT8U unit1, INT8U unit2)
{
	INT8U backup_weightunit;

	backup_weightunit = CurPluWeightUnit;
	if(unit1 == WEIGHT_UNIT_KG && unit2 == WEIGHT_UNIT_LB){			// unit1:kg -> unit2:lb
		//val *= 1.102311;					// = 2.204622 / 2.0
		CurPluWeightUnit = unit1;
		val = ad_cnv_main2adm(val);
		val = longmuldiv(val, 2204622, 1000000l);
		CurPluWeightUnit = unit2;
		val = ad_cnv_adm2main(val);
		val = ad_cnv_weight_value(val, 1);
	} else if(unit1 == WEIGHT_UNIT_LB && unit2 == WEIGHT_UNIT_KG){		// unit1:lb -> unit2:kg
		//val *= 0.907185;					// = 0.4535925 * 2.0
		CurPluWeightUnit = unit1;
		val = ad_cnv_main2adm(val);
		val = longmuldiv(val, 453593, 1000000l);
		CurPluWeightUnit = unit2;
		val = ad_cnv_adm2main(val);
		val = ad_cnv_weight_value(val, 1);
	}
	CurPluWeightUnit = backup_weightunit;
	
	return val;
}

INT8U ADM_FilterLevel[2][5][8] = 	// [scaletype][level][paramter]
{
// normal-type	
	{//	motion	filton	filtoff	ft_size	update1	update2	update3	prefilt
		{8,	4,	70,	10,	16,	8,	5,	0},
		{8,	4,	70,	10,	16,	8,	4,	0},	// default
		{8,	4,	70,	10,	16,	8,	3,	0},
		{8,	4,	70,	10,	16,	8,	2,	0},
		{8,	4,	70,	10,	16,	8,	1,	0}
	},
// hanging-type
	{
		{3,	20,	70,	24,	20,	4,	10,	1},
		{3,	20,	70,	24,	20,	4,	8,	1},	// default
		{3,	20,	70,	24,	20,	4,	6,	1},
		{3,	20,	70,	24,	20,	4,	4,	1},
		{3,	20,	70,	24,	20,	4,	2,	1}
	}
};


void adm_set_filter(INT8U scale_type, INT8U level)
{
	INT8U stype;

	stype = 0;
	if (scale_type == 3) stype = 1;

	ADM_WaitData(ADM_CMD_SET_MOTION_BAND, (INT32S)ADM_FilterLevel[stype][level][0], ADM_PARAM_ON);		// Set Motion Band
	ADM_WaitData(ADM_CMD_SET_FILTON_BAND, (INT32S)ADM_FilterLevel[stype][level][1], ADM_PARAM_ON);	 	// Set Filter On Band
	ADM_WaitData(ADM_CMD_SET_FILTOFF_BAND, (INT32S)ADM_FilterLevel[stype][level][2], ADM_PARAM_ON);	// Set Filter Off Band
	ADM_WaitData(ADM_CMD_SET_FILTER_SIZE, (INT32S)ADM_FilterLevel[stype][level][3], ADM_PARAM_ON);	// Set Filter Buffer Size

	ADM_WaitData(ADM_CMD_SET_UPDATETIME1, (INT32S)ADM_FilterLevel[stype][level][4], ADM_PARAM_ON);	// Set Update Times1
	ADM_WaitData(ADM_CMD_SET_UPDATETIME2, (INT32S)ADM_FilterLevel[stype][level][5], ADM_PARAM_ON);		// Set Update Times2
	ADM_WaitData(ADM_CMD_SET_UPDATETIME3, (INT32S)ADM_FilterLevel[stype][level][6], ADM_PARAM_ON);		// Set Update Times3

	if(ADM_Status.Version >= 110) {
		ADM_WaitData(ADM_CMD_SET_PARAMETER, ADM_SUB_HANG_TYPE + (INT32S)ADM_FilterLevel[stype][level][7], ADM_PARAM_ON);	// Set Pre-Filter Enable
	}
}



INT32S ad_cnv_weight_by_gram(INT32S weight)
{
	INT8U id;
	INT32S mul;
	
	id = ad_get_capa_idx();
	mul = 1;
	if (id <= 2) mul = 1; // 6kg, 15kg, 30kg
	else if (id <= 5) mul = 10; // 60kg, 150kg, 300kg
	else if (id <= 8) mul = 100; // 600kg, 1500kg, 3000kg
	else if (id == 9) mul = 1000; // 6000kg

	weight *= mul;
	return weight;
}

