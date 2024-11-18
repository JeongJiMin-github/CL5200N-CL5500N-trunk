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
#include "adm_app.h"
#include "key.h"
#include "key_typedef.h"
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
#include "key_app.h"
#include "dsp_app.h"
#include "ingredient.h"
#include "discount.h"
#include "tare_file.h"
#include "plu_auxcall.h"
#include "barcode_file.h"

extern DISCOUNT_STRUCT	discount_str;

//INT32U get_tare_fromid(INT16U id,INT8U *tare_type)
INT8U get_tare_fromid(INT16U id,INT8U *tare_type, INT32U *tare_value, INT8U *tare_unit)
{
	INT32U addr;
//DEL060829	INT8U ret;
	TARE_STRUCT 	tare_str;

	*tare_type  = 0;
	*tare_value = 0;
	*tare_unit  = 0;

	if (id==0) return OFF;
	addr = id-1;
	addr*=TARE_STRUCT_SIZE;
	addr+=DFLASH_BASE;
	addr+=FLASH_TARE_AREA;

	Flash_sread(addr, (HUGEDATA INT8U *)&tare_str, TARE_STRUCT_SIZE);

	if(id != tare_str._code) return OFF;
	if(tare_str._type != 1 && tare_str._type != 2) return OFF;

	*tare_type  = tare_str._type;
	*tare_value = tare_str._value;
	*tare_unit  = tare_str._weightunit;

	if(!GlbOper.EnableKgLb) {
		*tare_unit = ADM_Status.UseUnit;	// disable kg/lb
		if(*tare_unit == WEIGHT_UNIT_KG) {
			if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) *tare_unit = WEIGHT_UNIT_G;
		}
	}
	return ON;
}

INT32U get_defaultunit(INT8U w_unit)	// Default : 1kg, 100g, 1lb,......1/2lb(X), 1/4lb(X)
{
	INT32U ret;

	ret=(INT32U)get_global_wparam(GLOBAL_DEFAULTUNIT);

	if(w_unit == WEIGHT_UNIT_LB)
	{
		ret = 1;	// Only 1lb
	}
#ifdef USE_WEIGHT_UNIT_500G
	else if(ret > 3)// 1:1kg, 2:100g, 3:500g(china)
#else
	else if(ret > 2)
#endif
	{
		ret = 1;	// 1:1kg, 2:100g
	}

	ret= get_unitweight(ret, w_unit);

	return ret;
}

//////////////////////
// Input Parameter
//  weight unit   id    unit weight
//    (wunit)	 (id)
//       kg        1      1kg
//                 2      100g
//       lb        1      1lb
//                 2      1/2lb
//                 3      1/4lb
INT32U get_unitweight(INT8U id, INT8U wunit) // Modified by CJK 20050927
{
	INT32U wu;

	if (wunit == WEIGHT_UNIT_LB) {
// Deleted by CJK 20060220
//		switch (id) {
//			case 1: wu=100l; break;	// 1 lb
//			case 2: wu=50l;  break;	// 1/2 lb
//			case 3: wu=25l;  break;	// 1/4 lb
//			default: wu=100l; break;
//		}
		switch (id) {
			case 1: wu=1000l; break;	// 1 lb
			case 2: wu=500l;  break;	// 1/2 lb
			case 3: wu=250l;  break;	// 1/4 lb
			default: wu=1000l; break;
		}
	} else {
		switch (id) {
			case  1: wu=1000l; break;	// 1 kg
			case  2: wu=100l;  break;	// 100 g
#ifdef USE_WEIGHT_UNIT_500G
			case  3: wu=500l;  break;	// 500 g (china)
#endif
			default: wu=1000l; break;
		}
	}
	return wu;
}

//////////////////////
// Input Parameter
//  weight unit  unit weight   id    
//    (wunit)                 (id)
//       kg       1000	       1    
//                100	       2    
//       lb       1000	       1    
//                500	       2    
//                250	       3    
INT8U get_unitweight_id(INT32U unitweight, INT8U wunit)
{
	INT8U id;

	if (wunit == WEIGHT_UNIT_LB) {
		switch (unitweight) {
			case 1000: id=1; break;	// 1 lb
			case 500:  id=2; break;	// 1/2 lb
			case 250:  id=3; break;	// 1/4 lb
			default:   id=1; break;
		}
	} else {
		switch (unitweight) {
			case 1000: id=1; break;	// 1 kg
			case 100:  id=2; break;	// 100 g
#ifdef USE_WEIGHT_UNIT_500G
			case 500:  id=3; break; // 500 g(china)
#endif
			default:   id=1; break;
		}
	}
	return id;
}

///////////////////////////
// 무게단위와 UnitWeight를 반환
//     UseUnit  PLU Field   plu_unit
//       KG         1        1kg or 100g
//                  2        1lb
//       LB         1        1lb
//                  2        1kg or 100g 
//
//    -. PLU의 UnitWeight을 가져올 때 만 사용해야한다.

void get_weightunit_and_id_fromid(INT8U id, INT8U *new_id, INT8U *plu_unit)
{
	
	*plu_unit = CurPluWeightUnit;
	*new_id = id;

// Misc. Plu의 구분 불필요
//CJK080214	if(status_scale.cur_pluid >= MISC_PLU_WEIGHT)
//CJK080214	{
//CJK080214		*new_id = 0;	// default id로 선택하기 위함
//CJK080214		//if(ADM_Status.UseUnit == WEIGHT_UNIT_LB) *new_id = 1;	//Modified by JJANG 20070712
//CJK080214	}

	if(GlbOper.EnableKgLb) {
		if(ADM_Status.UseUnit == WEIGHT_UNIT_LB) 	// 1:lb, 2:kg
		{
			if(*new_id == 2)	// KG
			{
				*plu_unit = WEIGHT_UNIT_KG;
				*new_id = (INT8U)get_global_wparam(GLOBAL_DEFAULTUNIT);	// 1:1kg, 2:100g
			} 
			else			// LB  
			{
				*plu_unit = WEIGHT_UNIT_LB;
				*new_id = 1;// dummy
			}
		} 
		else						// 1:kg, 2:lb 
		{
			if(*new_id == 2)	// LB
			{
				*plu_unit = WEIGHT_UNIT_LB;
				*new_id = 1;
			}
			else			// KG  
			{
				*plu_unit = WEIGHT_UNIT_KG;
				*new_id = (INT8U)get_global_wparam(GLOBAL_DEFAULTUNIT);	// 1:1kg, 2:100g
			}
		}
	}
	if(*plu_unit == WEIGHT_UNIT_LB) {
		if(!GlbOper.EnableMultiLbKey || !*new_id || *new_id > 1) {
			*new_id = 1;
		}
	} else {
		if(get_base_bparam(FLASH_ADC_WUNIT) == WEIGHT_UNIT_G) *plu_unit = WEIGHT_UNIT_G;
#ifdef USE_WEIGHT_UNIT_500G
		if(!GlbOper.EnableMultiKgKey || !*new_id || *new_id > 3)
#else
		if(!GlbOper.EnableMultiKgKey || !*new_id || *new_id > 2)
#endif
		{
			*new_id = (INT8U)get_global_wparam(GLOBAL_DEFAULTUNIT);
		}
	}

	return;
}





INT8U get_clerk_fromid(INT16S id)
{
	return id;
}

//INT8U get_discount(PLU_BASE *Plu,INT16U id)
//{
//	INT32U Flash_addr;
//
//	discount_str.code=id;
//	Flash_addr =  DFLASH_BASE + FLASH_DISCOUNT_AREA;
//	Flash_addr += DISCOUNT_STRUCT_SIZE * discount_str.code;
//	Flash_sread(Flash_addr, (HUGEDATA INT8U *)&discount_str, sizeof(DISCOUNT_STRUCT));
//	Plu->dc_type = discount_str.type;
///*
//	Plu->dc_firstprice  =discount_str.first_price;
//	Plu->dc_secondprice =discount_str.second_price;
//	Plu->dc_firsttarget =discount_str.first_target;
//	Plu->dc_secondtarget=discount_str.second_target;
// Check Usage
//*/
//	return 1;
//}

INT8U get_barcodeformat(INT16U barid, char *str, INT8U *barcode_type)
{
	BARCODE_STRUCT 	bc_str;
	INT32U start_addr;

	if(barid != 0) {	// barid가 있을 경우
		start_addr  = barid-1;
		start_addr *= BARCODE_STRUCT_SIZE;
		start_addr += DFLASH_BASE + FLASH_BARCODE_FORMAT_AREA;

		Flash_sread(start_addr,(HUGEDATA INT8U *)&bc_str,BARCODE_STRUCT_SIZE);
		if(barid == bc_str.barcode_num) {	// barid가 존재하는 바코드인경우
			*barcode_type = bc_str.barcode_type;
			memcpy((INT8U *)str, bc_str.barcode_format, BARCODE_FORMAT_LEN);
			str[BARCODE_FORMAT_LEN] = 0;
			return ON;
		} else {	// barid가 존재하지 않는 바코드인경우
			return OFF;
		}
	} else { // barid가 0인 바코드인경우
		bc_str.barcode_type = BARCODE_TYPE_EAN13;	// EAN-13
		barcode_make_format(1, bc_str.barcode_type, bc_str.barcode_format);
		*barcode_type = bc_str.barcode_type;
		memcpy((INT8U *)str, bc_str.barcode_format, BARCODE_FORMAT_LEN);
		str[BARCODE_FORMAT_LEN] = 0;
		return ON;
	}
}

INT8U get_barcodetype(INT32U a)
{
	BARCODE_STRUCT 	bc_str;
	INT32U start_addr;
	INT16U barid;
	INT8U barcode_type;	
	
	barid = get_global_bparam(a);
	if (barid<1 || barid>99) barid=0;
	
	if(barid != 0) {	// barid가 있을 경우
		start_addr  = barid-1;
		start_addr *= BARCODE_STRUCT_SIZE;
		start_addr += DFLASH_BASE + FLASH_BARCODE_FORMAT_AREA;

		Flash_sread(start_addr,(HUGEDATA INT8U *)&bc_str,BARCODE_STRUCT_SIZE);
		if(barid == bc_str.barcode_num) {	// barid가 존재하는 바코드인경우
			barcode_type = bc_str.barcode_type;
		} else {	// barid가 존재하지 않는 바코드인경우
			barcode_type = BARCODE_TYPE_EAN13;	// EAN-13
		}
	} else { // barid가 0인 바코드인경우
		barcode_type = BARCODE_TYPE_EAN13;	// EAN-13
	}
	
	return barcode_type;
}
/*
void calc_discount(INT16S id)
{
	INT32U Flash_addr;
	long   dc_price;
	INT16U dtype;
	double f_dc;

	if (id==0) return;

	discount_str.code=id-1;
	Flash_addr =  DFLASH_BASE + FLASH_DISCOUNT_AREA;
	Flash_addr += DISCOUNT_STRUCT_SIZE * discount_str.code;
	Flash_sread(Flash_addr, (HUGEDATA INT8U *)&discount_str, sizeof(DISCOUNT_STRUCT));
	dtype = discount_str.type;
 	status_scale.dc_totalprice=status_scale.TotalPrice;
 	switch (dtype) {
		case 0x1C91:
			     status_scale.TotalPrice = discount_str.target[0];
		             break;
		case 0x1C92:
			     status_scale.TotalPrice = status_scale.TotalPrice - discount_str.target[0];
		             break;
		case 0x1C93:
//			     dc_price=status_scale.TotalPrice*discount_str.first_target/1000;
			     f_dc=status_scale.TotalPrice*discount_str.target[0]/1000.0;
			     f_dc/=100.0;
			     dc_price=(long)(f_dc+0.5);
			     status_scale.TotalPrice = status_scale.TotalPrice - dc_price;
			     break;
		case 0x1C94:
		             if ((status_scale.Weight>=discount_str.target[0]) && (status_scale.Weight<discount_str.target[1])) {
				status_scale.TotalPrice -= discount_str.price[0];
			     } else if (status_scale.Weight>=discount_str.target[1]) {
				status_scale.TotalPrice -= discount_str.price[0];
			     }
			     break;
		default    : status_scale.dc_totalprice=status_scale.TotalPrice;
	}
}

void calc_global_discount(INT8U dtype)
{
	long   dc_price;
	if (dtype==0) return;
 	status_scale.dc_totalprice=status_scale.TotalPrice;
 	switch (dtype) {
		case 3:// 0x1C91:
			     status_scale.TotalPrice = status_scale.sale_value;
		             break;
		case 2:// 0x1C92:
			     status_scale.TotalPrice = status_scale.TotalPrice - status_scale.sale_value;
		             break;
		case 1:// 0x1C93:
			     dc_price=status_scale.TotalPrice*status_scale.sale_value/100;
			     status_scale.TotalPrice = status_scale.TotalPrice - dc_price;
			     break;
	}	
}

*/
