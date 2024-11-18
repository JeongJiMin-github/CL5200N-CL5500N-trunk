/*****************************************************************************
|*			
|*  Copyright		:	(c) 2015 CAS
|*  Filename		:	Chn_dahua_interpreter.c
|*  Version			:	0.1
|*  Programmer(s)	:	LEE YOUNG HOON (LYH)
|*  Created			:	2015/09/06
|*  Description		:	CHINA Dahua scale protocol interpreter    
|*				
****************************************************************************/
#include <string.h>
#include "globals.h"
#include "commbuf.h"
#include "common.h"
#include "initial.h"
#include "mode_commun.h"
#include "plu_device.h"
#include "plu_manager.h"
#include "chn_dahua_interpreter.h"
#ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜 

// Global variable
DAHUA_STATE dahuaprotocol;
// Static Prototype
static void CHN_quweima_to_GB2312(INT8U *str);
//static void CHN_GB2312_to_quweima(INT8U *str);
//static INT8U CHN_dhbz_plu_save(HUGEDATA COMM_BUF *CBuf, DAHUA_DHBZ_PLU *dahua_plu);
//static INT8U CHN_tma_plu_save(HUGEDATA COMM_BUF *CBuf, DAHUA_TMA_PLU *dahua_plu);
//static void CHN_dhbz_interpreter_init(DAHUA_DHBZ_PLU *temp_dahua_dhbz_plu);
//static void CHN_tma_interpreter_init(DAHUA_TMA_PLU *temp_dahua_tma_plu);
//static void CHN_dahua_plu_input(INT32U *inputdata, INT8U maxdigit, INT8U r_data);
// Global Function

// DAHUA DHBZ5,DHBZ6 PROTOCOL
//INT8U CHN_dhbz_plu_interpreter(HUGEDATA COMM_BUF *CBuf, INT8U r_data, DAHUA_DHBZ_PLU *temp_dahua_dhbz_plu)
//{
//	switch(dahuaprotocol.state)
//	{
//		case 0 : // INIT
//			CHN_dhbz_interpreter_init(temp_dahua_dhbz_plu);
//			dahuaprotocol.state++;
//		case 1: //"!0Waaaa" 4digit PLU num
//			CHN_dahua_plu_input((INT32U*)temp_dahua_dhbz_plu->PLUnum, DAHUA_DHBZ_PLUNUM_DIGIT, r_data);
//			break;
//		case 2: //"!0WaaaaA" Field seperator
//			if(r_data=='A') dahuaprotocol.state++;
//			else dahuaprotocol.state=0;				
//			break;
//		case 3: //"bbbbbb" 6digit itemcode
//			CHN_dahua_plu_input((INT32U*)temp_dahua_dhbz_plu->itemcode, DAHUA_DHBZ_ITMECODE_DIGIT, r_data);
//			break;
//		case 4: //"bbbbbbB" Field seperator
//			if(r_data=='B')	dahuaprotocol.state++;
//			else dahuaprotocol.state=0;
//			break;
//		case 5: //"ccccccC" 5,6digit unitprice
//			if(r_data=='C')
//			{
//				dahuaprotocol.state++;
//				dahuaprotocol.length= 0;
//				break;
//			}
//			else
//			{
//				temp_dahua_dhbz_plu->unitprice = temp_dahua_dhbz_plu->unitprice * 10;
//				temp_dahua_dhbz_plu->unitprice += ctod(r_data);
//			}
//			dahuaprotocol.length++;
//			if(dahuaprotocol.length > DAHUA_DHBZ_UNPRICE_DIGIT) dahuaprotocol.state = 0;			
//			break;
//		case 6: //"d" 1digit PLU unit 0: weight 1: non-weight 2: fixedprice
//			temp_dahua_dhbz_plu->type += ctod(r_data);
//			dahuaprotocol.state++;
//			break;
//		case 7: //"dD"  Field seperator
//			if(r_data=='D')	dahuaprotocol.state++;
//			else dahuaprotocol.state=0;
//			break;
//		case 8: //"eee" 3digit valid date
//			CHN_dahua_plu_input((INT32U*)temp_dahua_dhbz_plu->validdate, DAHUA_DHBZ_VALIDDATE_DIGIT, r_data);
//			break;
//		case 9: //"eeeE"  Field seperator
//			if(r_data=='E')	dahuaprotocol.state++;
//			else dahuaprotocol.state=0;
//			break;
//		case 10: //"ff" 2digit Barcode index number
//			CHN_dahua_plu_input((INT32U*)temp_dahua_dhbz_plu->barcodenum, DAHUA_DHBZ_BARCODE_DIGIT, r_data);
//			break;
//		case 11: //"ffF"  Field seperator
//			if(r_data=='E')	dahuaprotocol.state++;
//			else dahuaprotocol.state=0;
//			break;
//		case 12: //"ggggg"  5 digit tare/fixed price,  unit:g
//			CHN_dahua_plu_input((INT32U*)temp_dahua_dhbz_plu->tare, DAHUA_DHBZ_TARE_DIGIT, r_data);
//			break;
//		case 13: //"gggggG"  Field seperator
//			if(r_data=='G')	dahuaprotocol.state++;
//			else dahuaprotocol.state=0;
//			break;
//		case 14: //"h" 1digit PLU special
//			CHN_dahua_plu_input((INT32U*)temp_dahua_dhbz_plu->specialnum, DAHUA_DHBZ_SPECIAL_DIGIT, r_data);
//			break;
//		case 15: //"hH"  Field seperator
//			if(r_data=='H')	dahuaprotocol.state++;
//			else dahuaprotocol.state=0;
//			break;
//		case 16: //"iiiiiiiiiiiiiiiiiiI" 18digit PLU name
//			if(r_data=='I')
//			{
//				dahuaprotocol.state++;
//				dahuaprotocol.length = 0;
//				break;
//			}
//			if(dahuaprotocol.length%2 == 0)
//			{
//				temp_dahua_dhbz_plu->PLUname[dahuaprotocol.length/2] = ctod(r_data);
//			}
//			else
//			{
//				temp_dahua_dhbz_plu->PLUname[dahuaprotocol.length/2] =\
//				temp_dahua_dhbz_plu->PLUname[dahuaprotocol.length/2] * 10;
//				temp_dahua_dhbz_plu->PLUname[dahuaprotocol.length/2] += ctod(r_data);
//			}
//			dahuaprotocol.length++;
//			if(dahuaprotocol.length > DAHUA_DHBZ_PLUNAME_SIZE)	dahuaprotocol.state = 0;
//			break;
//		case 17: //"jjjjjjjjjjjjjjjjjjjjjjjjjjJ" 26digit ingredient
//			if(r_data=='J')
//			{
//				dahuaprotocol.state=0;				
//				dahuaprotocol.length = 0;
//				CHN_dhbz_plu_save(CBuf, temp_dahua_dhbz_plu);
//				break;
//			}
//			if(dahuaprotocol.length%2 == 0)
//			{
//				temp_dahua_dhbz_plu->ingredient[dahuaprotocol.length/2] = ctod(r_data);
//			}
//			else
//			{
//				temp_dahua_dhbz_plu->ingredient[dahuaprotocol.length/2] =\
//				temp_dahua_dhbz_plu->ingredient[dahuaprotocol.length/2] * 10;
//				temp_dahua_dhbz_plu->ingredient[dahuaprotocol.length/2] += ctod(r_data);
//			}
//			dahuaprotocol.length++;
//			if(dahuaprotocol.length > DAHUA_DHBZ_INGREDIENT_SIZE)	dahuaprotocol.state = 0;
//			return RETURN_END;
//	}
//	return RETURN_STAY;
//}

// DAHUA TMA07 PROTOCOL
//INT8U CHN_tma_plu_interpreter(HUGEDATA COMM_BUF *CBuf, INT8U r_data, DAHUA_TMA_PLU *temp_dahua_tma_plu)
//{
//	switch(dahuaprotocol.state)
//	{
//		case 0 : // INIT
//			CHN_tma_interpreter_init(temp_dahua_tma_plu);
//			dahuaprotocol.state++;
//		case 1: //"!0Waaaa" 4digit PLU num
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->PLUnum, DAHUA_TMA_PLUNUM_DIGIT, r_data);
//			break;
//		case 2: //"!0VaaaaA" Field seperator
//			if(r_data=='A') dahuaprotocol.state++;
//			else dahuaprotocol.state=0;
//			break;
//		case 3: //"bbbbbbb" 7digit itemcode
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->itemcode, DAHUA_TMA_ITMECODE_DIGIT, r_data);			
//			break;
//		case 4: //"bbbbbbbcccccc" 6digit Unit Price
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->unitprice, DAHUA_TMA_UNPRICE_DIGIT, r_data);
//			break;
//		case 5: //"bbbbbbbccccccd" 1digit PLU type
//			temp_dahua_tma_plu->type += ctod(r_data);
//			dahuaprotocol.state++;
//			break;
//		case 6: //"bbbbbbbccccccdee" 2digit ingredient1 index number
//		case 7: //"bbbbbbbccccccdeeff" 2digit ingredient2 index number
//		case 8: //"bbbbbbbccccccdeeffgg" 2digit ingredient3 index number
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->ingredientNo[dahuaprotocol.state-6], DAHUA_TMA_INGREDIENT_DIGIT, r_data);
//			break;
//		case 9: //"bbbbbbbccccccdeeffgghhh" 3digit sellbydate
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->validdate, DAHUA_TMA_VALIDDATE_DIGIT, r_data);
//			break;
//		case 10: //"bbbbbbbccccccdeeffgghhhii" 2digit shop number
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->shopNo, DAHUA_TMA_SHOP_DIGIT, r_data);			
//			break;
//		case 11: //"bbbbbbbccccccdeeffgghhhiijj" 2digit department
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->department, DAHUA_TMA_DEPARTMENT_DIGIT, r_data);
//			break;
//		case 12: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkk" 13digit ext.barcode
//			temp_dahua_tma_plu->extbarcode[dahuaprotocol.length] = r_data;
//			dahuaprotocol.length++;
//			if(dahuaprotocol.length==DAHUA_TMA_BARCODE_SIZE)
//			{
//				dahuaprotocol.state++;
//				dahuaprotocol.length = 0;
//			}
//			break;
//		case 13: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllll" 5digit tare
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->tarevalue, DAHUA_TMA_TARE_DIGIT, r_data);
//			break;
//		case 14: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmm" 2digit label no
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->labelNo, DAHUA_TMA_LABEL_DIGIT, r_data);
//			break;
//		case 15: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnn" 2digit discounttype
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->discounttype, DAHUA_TMA_DICOUNT_DIGIT, r_data);			
//			break;
//		case 16: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnoo" 2digit 1st discount start hour
//		case 19: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrr" 2digit 2nd discount start hour		
//		case 22: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrrssttuu" 2digit 3rd discount start hour		
//		case 25: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrrssttuuvvwwxx" 2digit 4rd discount start hour		
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->discount[(dahuaprotocol.state-16)/3].start_hour, DAHUA_TMA_DICOUNT_DIGIT, r_data);
//			break;
//		case 17: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnoopp" 2digit 1st discount end hour
//		case 20: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrrss" 2digit 2nd discount end hour
//		case 23: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrrssttuuvv" 2digit 3rd discount end hour
//		case 26: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrrssttuuvvwwxxyy" 2digit 4rd discount end hour
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->discount[(dahuaprotocol.state-17)/3].end_hour, DAHUA_TMA_DICOUNT_DIGIT, r_data);
//			break;
//		case 18: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqq" 2digit 1st discount rate
//		case 21: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrrsstt" 2digit 2nd discount rate
//		case 24: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrrssttuuvvww" 2digit 3rd discount rate
//		case 27: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrrssttuuvvwwxxyyzz" 2digit 4rd discount rate
//			CHN_dahua_plu_input((INT32U*)temp_dahua_tma_plu->discount[(dahuaprotocol.state-18)/3].rate, DAHUA_TMA_DICOUNT_DIGIT, r_data);
//			break;
//		case 28: //"bbbbbbbccccccdeeffgghhhiijjkkkkkkkkkkkkkklllllmmnnooppqqrrssttuuvvwwxxyyzzB" Field seperator
//			// Seperator
//			if(r_data=='B') dahuaprotocol.state++;
//			else dahuaprotocol.state=0;
//			break;
//		case 29: //"111111111111111111111111C" PLU name 1
//		case 30: //"222222222222222222222222D" PLU name 2
//		case 31: //"333333333333333333333333E" PLU name 3
//			if(r_data=='C' || r_data=='D' || r_data=='E')
//			{
//				dahuaprotocol.state++;
//				dahuaprotocol.length = 0;
//				break;
//			}
//			if(dahuaprotocol.length%2 == 0)
//			{
//				temp_dahua_tma_plu->PLUname[dahuaprotocol.state-26][dahuaprotocol.length/2] = ctod(r_data);
//			}
//			else
//			{
//				temp_dahua_tma_plu->PLUname[dahuaprotocol.state-26][dahuaprotocol.length/2] =\
//				temp_dahua_tma_plu->PLUname[dahuaprotocol.state-26][dahuaprotocol.length/2] * 10;
//				temp_dahua_tma_plu->PLUname[dahuaprotocol.state-26][dahuaprotocol.length/2] += ctod(r_data);
//			}
//			dahuaprotocol.length++;
//			if(dahuaprotocol.length > DAHUA_TMA_PLUNAME_SIZE)	dahuaprotocol.state = 0;
//			break;
//		case 32: //0x0d end sequance1
//	        if(r_data==0x0d) dahuaprotocol.state++;
//			else dahuaprotocol.state=0;
//			break;
//		case 33: //0x0a end sequance2
//			dahuaprotocol.state=0;
//	        if(r_data==0x0a)
//			{
//				CHN_tma_plu_save(CBuf,temp_dahua_tma_plu);
//			}
//			return RETURN_END;
//	}
//	return RETURN_STAY;
//}

// Static Fucntion
void CHN_quweima_to_GB2312(INT8U *str)
{
	INT8U i=0;
	while(*(str+i))
	{
		*(str+i) += 0xa0;	
		i++;		
	}
}
/*
void CHN_GB2312_to_quweima(INT8U *str)
{
	INT8U i=0;
	while(*(str+i))
	{
		*(str+i) -= 0xa0;	
		i++;
	}
}
*/
void CHN_dhbz_plu_save(DAHUA_DHBZ_PLU *dahua_plu)
{
	INT8U  deptno;
	INT16U plu_addr;
	INT32U pluno,key;
	INT32U temp_value;

	deptno = (INT8U)1;
	pluno  = (INT32U)dahua_plu->PLUnum;

	// Search PLU
	if (!plu_table_search(1,pluno,&plu_addr,0)) {
		// New PLU
		plu_addr = plu_address_get_new();
		plu_temp_clear(COMM_TYPE_ETHDATA);
	}
	else // exist PLU
	{
		key = (INT32U)plu_addr;
		plu_move_temp(COMM_TYPE_ETHDATA,key-1);
	
	}
//	xpos_base=get_temp_area(COMM_TYPE_ETHDATA);	
	// Department : Always 1
	temp_value = 1;	
	plu_set_field(plu_addr-1, 1, (INT8U *)&temp_value);

	// PLU num
	plu_set_field(plu_addr-1, 2, (INT8U *)&(dahua_plu->PLUnum)); 		
	// PLU type : Always weight
	temp_value = 1;		
	plu_set_field(plu_addr-1,  4, (INT8U*)&temp_value);
	// Unit Price
	plu_set_field(plu_addr-1,  6, (INT8U*)&(dahua_plu->unitprice));
	// PLU unit
	switch(dahua_plu->unit)
	{
		case 0: // Weight PLU
			temp_value = 1;
			plu_set_field(plu_addr-1,  4, (INT8U*)&temp_value);
			temp_value = 0;
			plu_set_field(plu_addr-1,  26, (INT8U*)&temp_value);
			break;
		case 1: // Count PLU
			temp_value = 2;
			plu_set_field(plu_addr-1,  4, (INT8U*)&temp_value);
			temp_value = 0;
			plu_set_field(plu_addr-1,  26, (INT8U*)&temp_value);
			break;
		case 2: // Fixed price PLU
			temp_value = 1;
			plu_set_field(plu_addr-1,  4, (INT8U*)&temp_value);
			temp_value = 1;
			plu_set_field(plu_addr-1,  26, (INT8U*)&temp_value);
			break;
	}

	// PLU itemcode
	plu_set_field(plu_addr-1,11,(INT8U *)&(dahua_plu->itemcode));

#ifndef USE_DAHUA_EXCLUSIVE_PROTOCOL	// DAHUA 마트 전용 프로그램 사용 시 Barcode ID / Sale Msg 사용 X
	// Barcode Id
	plu_set_field(plu_addr-1,85,(INT8U *)&(dahua_plu->barcodenum));

	// Sale Msg
	plu_set_field(plu_addr-1,90,(INT8U *)&(dahua_plu->specialnum));
#endif
	// Tare
	plu_set_field(plu_addr-1, 13, (INT8U*)&(dahua_plu->tarevalue));	
	
	// PLU name
	CHN_quweima_to_GB2312(dahua_plu->PLUname);
	plu_set_field(plu_addr-1, 10, (INT8U*)(dahua_plu->PLUname));
	// PLU name2
	CHN_quweima_to_GB2312(dahua_plu->ingredient);
	plu_set_field(plu_addr-1, 30, (INT8U*)(dahua_plu->ingredient));	
	
	// ETC setting
	temp_value = 0;		
	// Valid date
	plu_set_field(plu_addr-1, 16, (INT8U*)&(dahua_plu->validdate));		
	// TAX
	plu_set_field(plu_addr-1, 8, (INT8U*)&temp_value);
	plu_set_field(plu_addr-1, 100, (INT8U*)&temp_value);			
//	plu_set_field(plu_addr-1, 26, (INT8U*)&temp_value);			
	plu_set_field(plu_addr-1, 55, (INT8U*)&temp_value);	
	plu_set_field(plu_addr-1, 9,(INT8U *)&(temp_value));
	plu_set_field(plu_addr-1,  25, (INT8U*)&(temp_value));
	plu_set_field(plu_addr-1,  14, (INT8U*)&(temp_value));
	plu_set_field(plu_addr-1,  15, (INT8U*)&(temp_value));	
	plu_set_field(plu_addr-1,  3, (INT8U*)&(temp_value));	
	plu_table_save(deptno,pluno);
}

void CHN_tma_plu_save(DAHUA_TMA_PLU *dahua_plu)
{
	INT8U  deptno;
	INT16U plu_addr;
	INT32U pluno,key;
	INT32U temp_value;

	deptno = (INT8U)dahua_plu->department;
	if(deptno == 0)deptno = 1;
	pluno  = (INT32U)dahua_plu->PLUnum;

	// Search PLU
	if (!plu_table_search(1,pluno,&plu_addr,0)) {
		// New PLU
		plu_addr = plu_address_get_new();
		plu_temp_clear(COMM_TYPE_ETHDATA);
	}
	else // exist PLU
	{
		key = (INT32U)plu_addr;
		plu_move_temp(COMM_TYPE_ETHDATA,key-1);
	
	}
	// Department : Always 1
	plu_set_field(plu_addr-1, 1, (INT8U *)&deptno);

	// PLU number
	plu_set_field(plu_addr-1, 2, (INT8U *)&(dahua_plu->PLUnum)); 		
	// PLU type : Always weight
	switch(dahua_plu->type)
	{
		case 0: // Weight PLU
			temp_value = 1;
			plu_set_field(plu_addr-1,  4, (INT8U*)&temp_value);
			temp_value = 0;
			plu_set_field(plu_addr-1,  26, (INT8U*)&temp_value);
			break;
		case 1: // Count PLU
			temp_value = 2;
			plu_set_field(plu_addr-1,  4, (INT8U*)&temp_value);
			temp_value = 0;
			plu_set_field(plu_addr-1,  26, (INT8U*)&temp_value);
			break;
		case 2: // Fixed price PLU
			temp_value = 1;
			plu_set_field(plu_addr-1,  4, (INT8U*)&temp_value);
			temp_value = 1;
			plu_set_field(plu_addr-1,  26, (INT8U*)&temp_value);
			break;
	}
	
	// Unit Price
	plu_set_field(plu_addr-1,  6, (INT8U*)&(dahua_plu->unitprice));
	
	// IngredientNo
	plu_set_field(plu_addr-1,  25, (INT8U*)&(dahua_plu->ingredientNo));

	// labelNo
//	plu_set_field(plu_addr-1,  80, (INT8U*)&(dahua_plu->labelNo));

	// PLU itemcode
	plu_set_field(plu_addr-1,11,(INT8U *)&(dahua_plu->itemcode));

	// Group
//	plu_set_field(plu_addr-1, 9,(INT8U *)&(dahua_plu->storeNo));

	// Tare
	plu_set_field(plu_addr-1, 13, (INT8U*)&(dahua_plu->tarevalue));	

	// Ext.barcode
//	plu_set_field(plu_addr-1, 49, (INT8U*)&(dahua_plu->extbarcode));	
		
	// Valid date
	plu_set_field(plu_addr-1, 16, (INT8U*)&(dahua_plu->validdate));	
	// PLU name
	CHN_quweima_to_GB2312(dahua_plu->PLUname);
	plu_set_field(plu_addr-1, 10, (INT8U*)(dahua_plu->PLUname));
	// PLU name2
	CHN_quweima_to_GB2312(dahua_plu->PLUname2);
//	plu_set_field(plu_addr-1, 30, (INT8U*)(dahua_plu->PLUname2));	
	// PLU name3
	CHN_quweima_to_GB2312(dahua_plu->PLUname3);
//	plu_set_field(plu_addr-1, 31, (INT8U*)(dahua_plu->PLUname3));		
	// ETC setting
	temp_value = 0;		
	// TAX
	plu_set_field(plu_addr-1, 8, (INT8U*)&temp_value);
	plu_set_field(plu_addr-1, 100, (INT8U*)&temp_value);			
	plu_set_field(plu_addr-1, 55, (INT8U*)&temp_value);		
	// Barcode Id
	plu_set_field(plu_addr-1, 85,(INT8U *)&(temp_value));
	// PLU unit
	plu_set_field(plu_addr-1,  5, (INT8U*)&(temp_value));
	// Sale Msg
	plu_set_field(plu_addr-1,90,(INT8U *)&(temp_value));
	
	plu_table_save(deptno,pluno);
}

//void CHN_dhbz_interpreter_init(DAHUA_DHBZ_PLU *temp_dahua_dhbz_plu)
//{
//	dahuaprotocol.state=0;
//	dahuaprotocol.length=0;
//	memset(temp_dahua_dhbz_plu, 0, sizeof(DAHUA_DHBZ_PLU));
//}

//void CHN_tma_interpreter_init(DAHUA_TMA_PLU *temp_dahua_tma_plu)
//{
//	dahuaprotocol.state=0;
//	dahuaprotocol.length=0;
//	memset(temp_dahua_tma_plu, 0, sizeof(DAHUA_TMA_PLU));
//}

//void CHN_dahua_plu_input(INT32U *inputdata, INT8U maxdigit, INT8U r_data)
//{
//	dahuaprotocol.length++;
//	*inputdata = (*inputdata) * 10;
//	*inputdata += ctod(r_data);
//	if(dahuaprotocol.length==maxdigit)
//	{
//		dahuaprotocol.state++;
//		dahuaprotocol.length = 0;
//	}
//}


#endif // #ifdef USE_PROTOCOL_CHINA_DAHUA	// 중국 전용 다화 프로토콜 
