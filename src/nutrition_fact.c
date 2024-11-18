/*****************************************************************************
|*		       
|*  Copyright		:	(c) 2003 CAS
|*  Filename		:	nutition.c
|*  Version			:	v 0.5
|*  Programmer(s)	:	getwing (Jae-Ok Kim)
|*  Created			:	2003/12/04
|*  Description		:	Nutition Facts   
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
#include "nutrition_fact.h"
#include "menu_code_text1.h"
#include "pgm_app.h"
#include "prt_list.h"
#include "mode_error.h"
#ifdef USE_EU_NUTRITIONFACT
#include "nutrition_fact_caption.h"
#endif
#ifdef USE_DIRECT_NUTRITION
#include "plu_edit.h"
#endif

#ifdef USE_DIRECT_NUTRITION
void direct_nutri_menu_delete(void);
#endif

////NUTRIFACT_CAP_STRUCT  nutrifact_cap_str[] =
////	{// 	{no, caption  	,  		unit, decP, input_min, input_max }
////		{100, "Serving Size",			" "},		//0
////		{101, "Serving Per",			" "},		//1
////		{  1, "Calories", 			" "},		//2 	 0
////		{  2, "Calories From Fat", 		" "},		//3 	 1
////		{  3, "Total Fat",			"g"},		//4 	 2
////		{  4, "Saturated Fat",			"g"},		//5 	 3
////		{  5, "Trans Fat",			"g"},		//6 	 4	// Added by CJK 20051011
////		{  6, "Cholesterol",			"mg"},		//7 	 5
////		{  7, "Sodium",				"mg"},		//8 	 6
////		{  8, "Total Carbohydrates", 		"g"},		//9 	 7
////		{  9, "Dietary Fibers",			"g"},		//10	 8
////		{ 10, "Sugars",				"g"},		//11	 9
////		{ 11, "Protein",	    		"g"},		//12	 10
////		{ 12, "Vitamin A",			"%"},		//13	 11
////		{ 13, "Calcium",	    		"%"},		//14	 12
////		{ 14, "Vitamin C",			"%"},		//15	 13
////		{ 15, "Iron",				"%"}, 		//16	 14
//////		{200, "Etc", 				" "}		//17	 15
////
////	};

/*			 ****** Nutrifact ******	
Index 		Items
--------------------------------------------------
.		Serving Size (Text)    :
.		Serving Per Con 	   :
0.		Base Unit (0=g/1=ml)   :	- CHL(display) 
1.		Serving Size (Num)	   :	- CHL(display)
2.		Base Size			   :	- CHL(display)
3.		Calories			   :	
4.		Calories From Fat	   :
5.		Total Fat(g)		   :
6.		Saturated Fat(g)	   :
7.		Trans Fat(g)		   :
8.		Cholesterol(mg) 	   :
9.		Sodium(mg)			   :
10. 	Total Carbohydrates(g) :
11. 	Dietary Fibers(g)	   :
12. 	Sugars(g)			   :
13. 	Protein(g)			   :
14. 	Vitamin A(%)		   :
15. 	Calcium(%)			   :
16. 	Vitamin C(%)		   :
17. 	Iron(%) 			   :
*/

/*
	********************************************************************************
	* @brief	nutri type 과 index에 맞게 decimal point 값 반환함.
	* @param	index: nutrifact테이블index를 참조하여 만듦. nutriType: 국가별type 받음
	* @return   국가별 nutrifact index에 맞는 소수점 반환.
	* @remark	prt_cmdtest에서도 참조
	********************************************************************************
*/
#ifdef USE_NUTRITION_FACT

INT8U ReturnDecimalPointNutriTable(INT8U nutriType, INT8U index)
{
	//국가별 소수점 세팅, 위 nutrifact 테이블의 index를 참조한다. 
	INT8U decimalPointAUS[MAX_NUTRIFACT_ITEM]={0,0,0,0,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2,2};		//호주 nutri decimal point
	INT8U decimalPointCHL[MAX_NUTRIFACT_ITEM]={0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};		//칠레 nutri decimal point
	INT8U decimalPointEUR[MAX_NUTRIFACT_ITEM]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};		//영공 nutri decimal point
	INT8U decimalPointCAN[MAX_NUTRIFACT_ITEM]={0,0,0,0,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0};		//캐나다 nutri decimal point
	INT8U decimalPointNZL[MAX_NUTRIFACT_ITEM]={0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1};		//뉴질랜드 nutri decimal point	
	INT8U decimalPointCZE[MAX_NUTRIFACT_ITEM]={0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};		//체코 nutri decimal point
	INT8U decimalPointGBR[MAX_NUTRIFACT_ITEM]={0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,2,0};		//영국 nutri decimal point
#ifdef USE_EU_NUTRITIONFACT
	INT8U decimalPointEU[MAX_NUTRIFACT_ITEM_EU]={0,0,0,0,2,2,2,2,1,1,1,1,1,2,2,1,2,1,1,1,2,2,2,2,1,2,1,2,1,1,1,1,1,2,2,2,2,1,1,1,1,1};
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	INT8U decimalPointUSA[MAX_NUTRIFACT_ITEM]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	//미국 nutri decimal point
	//INT8U decimalPointCAN[MAX_NUTRIFACT_ITEM]={0,0,0,0,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0};	//캐나다 nutri decimal point
#endif
	
	if (nutriType == NUTRI_TYPE_2COL_CHL) 
	{
		return decimalPointCHL[index];
	}
	else if (nutriType == NUTRI_TYPE_TOTALFAT_CAN) 
	{
		return decimalPointCAN[index];
	}
	else if (nutriType == NUTRI_TYPE_2COL_6DIG_AUS || nutriType == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT)
	{	
		return decimalPointAUS[index];
	}
	else if (nutriType == NUTRI_TYPE_2COL_NZL || nutriType == NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
	{	
		return decimalPointNZL[index];
	}
	else if (nutriType == NUTRI_TYPE_6DIG_CZE)
	{
		return decimalPointCZE[index];
	}
#ifdef USE_EU_NUTRITIONFACT
	else if (nutriType == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
	{
		return decimalPointEU[index];
	}
#else
	else if (nutriType == NUTRI_TYPE_GBR)
	{
		return decimalPointGBR[index];
	}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	else if (nutriType == NUTRI_TYPE_2COL_USA)
	{
		return decimalPointUSA[index];
	}
	else if (nutriType == NUTRI_TYPE_TOTALFAT_CAN) 
	{
		return decimalPointCAN[index];
	}
#endif
	else // NUTRI_TYPE_NORMAL
	{
		return decimalPointEUR[index];
	}
	
}


INT16U Nutri_DV_refer_table[6] = {
	65,	// Total Fat
	20,	// Saturated Fat
	300,	// Cholesterol
	2400,	// Sodium
	300,	// Total Carbohydrates
	25,	// Dietary Fiber
};

#ifdef USE_EU_NUTRITIONFACT
INT32U Nutri_DV_refer_table_EU[39] = {
	8400,	// Energy
	7000,	// FAT
	2000,	// saturates
	0,		// mono unsaturates
	0,		// polyunsaturates
	2600,	// Carbohydrate
	900,	// sugars
	0,		// polyols
	0,		// starch
	0,		// Fibre
	5000,	// Protein
	600,	// Salt

	8000,	// Vitamin A
	500,	// Vitamin D
	120,	// Vitamin E
	750,	// Vitamin K
	800,	// Vitamin C
	110,	// Thiamin
	140,	// Riboflavin
	1600,	// Niacin
	140,	// Vitamin B6
	2000,	// Folic acid
	250,	// Vitamin B12
	500,	// Biotin
	600,	// Pantothenic acid
	20000,	// Potassium
	8000,	// Chloride
	8000,	// Calcium
	7000,	// Phosphorus
	3750,	// Magnesium
	1400,	// Iron
	1000,	// Zinc
	100,	// Copper
	200,	// Manganese
	35,		// Fluoride
	550,	// Selenium
	400,	// Chromium
	500,	// Molybdenum
	1500	// Iodine
};
#endif

#ifdef USE_ADDITIONAL_NUTRIENT
INT16U Nutri_DV_refer_table_USA[12] = {
	78,	// Total Fat
	20,	// Saturated Fat
	300,	// Cholesterol
	2300,	// Sodium
	275,	// Total Carbohydrates
	28,	// Dietary Fiber
	20, // Vitamin D
	1300,// Calcium
	18,// Iron
	4700,// Potassium
	50,// Added Sugars
	100,// Sugars for CANADA
};
#endif

#ifdef USE_DIRECT_NUTRITION
INT8U nutri_create_dept_no;
void nutrifact_dataUpdate(INT32U num)
#else
void nutrifact_dataUpdate(INT32U num)
#endif
{
	NUTRIFACT_STRUCT nutriStr;
	NUTRIFACT_STRUCT2 nutriStr2;
#ifdef USE_EU_NUTRITIONFACT
	NUTRIFACT_STRUCT3 nutriStr3;
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	NUTRIFACT_STRUCT4 nutriStr4;
#endif
	INT16U  strSize;
	INT32U	addr;
	INT16U  nutriIndex;
	INT8U   i, k = 0;
	INT8U	nutri_type;
#ifdef USE_DIRECT_NUTRITION
	INT16U  idx;
#endif

	nutri_type = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	nutri_type = nutri_type&0x0f;
#ifdef COUNTRY_USA
	nutri_type = NUTRI_TYPE_2COL_USA;
#endif
	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
	{
		strSize = sizeof(nutriStr2);
	}
#ifdef USE_EU_NUTRITIONFACT
	else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
	{
		strSize = sizeof(nutriStr3);
	}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		strSize = sizeof(nutriStr4);
#endif
	else strSize = sizeof(nutriStr); 
#ifdef USE_NORTH_AMERICA_INGREDIENT
	addr = EXT_MEMCARD2_ADDR_X;
#else
	addr = DFLASH_BASE;
#endif
	addr += FLASH_NUTRIFACT_AREA;
	addr += ((num - 1) * strSize);
	nutriIndex = Flash_wread(addr);

	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
	{
		if(nutriIndex == num) Flash_sread(addr, (HUGEDATA INT8U *)&nutriStr2, strSize );
		else
		{
			memset(&nutriStr2, 0, strSize );
			nutriStr2.code = num;		
		}
		*Menu[k++].pval = nutriStr2.code; 
		memcpy(Menu[k++].pstr, nutriStr2.description, Menu[1].length); 
		memcpy(Menu[k++].pstr, nutriStr2.serving_size, Menu[2].length); 
		memcpy(Menu[k++].pstr, nutriStr2.serving_per, Menu[3].length);
		/*for (i = 0; i < 18; i++) 
		{
			*Menu[k++].pval = nutriStr2.nutri_data[i];
		}*/
		if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT) 
		{
			for (i = 0; i < 3; i++) {
				*Menu[k++].pval = nutriStr2.nutri_data[i];
			}
		}
		for (i = 3; i < MAX_NUTRIFACT_ITEM; i++)
		{
			*Menu[k++].pval = nutriStr2.nutri_data[i];
		}
	}
#ifdef USE_EU_NUTRITIONFACT
	else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
	{
		if(nutriIndex == num) Flash_sread(addr, (HUGEDATA INT8U *)&nutriStr3, strSize );
	else
	{
			memset(&nutriStr3, 0, strSize );
			nutriStr3.code = num;		
		}
		*Menu[k++].pval = nutriStr3.code; 
		memcpy(Menu[k++].pstr, nutriStr3.description, Menu[1].length); 
		memcpy(Menu[k++].pstr, nutriStr3.serving_size, Menu[2].length); 
		memcpy(Menu[k++].pstr, nutriStr3.serving_per, Menu[3].length);
		for (i = 0; i < MAX_NUTRIFACT_ITEM_EU; i++) 
		{
			if (nutriStr3.nutri_data[i] < Menu[k].min || nutriStr3.nutri_data[i] > Menu[k].max)
				nutriStr3.nutri_data[i] = Menu[k].min;
			*Menu[k++].pval = nutriStr3.nutri_data[i];
		}
	}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
		else if(nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		{
  #ifdef USE_DIRECT_NUTRITION
		plu_table_search(nutri_create_dept_no, num, &idx, 0);
		if (idx == 0) {
			BuzOn(3);
			return;
			///return FALSE; // return FALSE 존재의 이유를 모르겠음
		}
		get_direct_nutrition(nutri_create_dept_no, num, &nutriStr4);
		*Menu[k++].pval = num;
  #else // USE_DIRECT_NUTRITION
			if(nutriIndex == num) 
				Flash_sread(addr, (INT8U *)&nutriStr4, strSize );
			else
			{
				memset(&nutriStr4, 0, strSize );
				nutriStr4.code = num;
			}
			*Menu[k++].pval = nutriStr4.code; 
  #endif // USE_DIRECT_NUTRITION
			memcpy(Menu[k++].pstr, nutriStr4.description, Menu[1].length); 
			memcpy(Menu[k++].pstr, nutriStr4.serving_size, Menu[2].length); 
			memcpy(Menu[k++].pstr, nutriStr4.serving_per, Menu[3].length);
			
			for (i = 3; i < MAX_NUTRIFACT_ITEM; i++)
			{
				*Menu[k++].pval = nutriStr4.nutri_data[i];
			}
		}
#endif
	else
	{
		if(nutriIndex == num) Flash_sread(addr, (HUGEDATA INT8U *)&nutriStr, strSize );
		else
		{
			memset(&nutriStr, 0, strSize );
			nutriStr.code = num;		
		}
		*Menu[k++].pval = nutriStr.code; 
		memcpy(Menu[k++].pstr, nutriStr.description, Menu[1].length); 
		memcpy(Menu[k++].pstr, nutriStr.serving_size, Menu[2].length); 
		memcpy(Menu[k++].pstr, nutriStr.serving_per, Menu[3].length);
		if (nutri_type == NUTRI_TYPE_2COL_CHL || nutri_type == NUTRI_TYPE_2COL_NZL || nutri_type == NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT) {
			for (i = 0; i < 3; i++) {
				*Menu[k++].pval = nutriStr.nutri_data[i];
			}
		}
		for (i = 3; i < MAX_NUTRIFACT_ITEM; i++)
		{
			*Menu[k++].pval = nutriStr.nutri_data[i];
		}
	}
	MenuRefresh = ON;
}

#ifdef USE_DIRECT_NUTRITION
void callback_nutrifact_description(INT32U num)
{
	NUTRIFACT_STRUCT4 nutriStr4;
	INT16U  strSize, idx;
	INT32U	addr;
	INT16U  nutriIndex, w;
	int   i, k = 0, j = 0;
	INT8U	nutri_type;
	CAPTION_STRUCT 	  cap;
	POINT disp_p;
	char buffer[MAX_MENU_CHAR_BUF];

	disp_p = point(CallbackEditPosition_y*Startup.menu_ygap, CallbackEditPosition_x*Startup.menu_xgap);
	w = Dsp_GetChars() + 1;
	w -= CallbackEditPosition_x;
	memset((INT8U *)buffer,0x20, w);
	buffer[w] = 0;
	Dsp_Write_String(disp_p,(HUGEDATA char *)buffer);	

	nutri_type = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	nutri_type = nutri_type&0x0f;
#ifdef COUNTRY_USA
	nutri_type = NUTRI_TYPE_2COL_USA;
#elif defined(USE_CANADA_DEFAULT)
	nutri_type = NUTRI_TYPE_TOTALFAT_CAN;
#endif
	strSize = sizeof(nutriStr4);

	DspStruct.Reverse = 1;

	plu_table_search(nutri_create_dept_no, num, &idx, 0);
	if (idx == 0) {
		BuzOn(3);
		
		caption_split_by_code(0xE00C,&cap,0);
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		DspStruct.Reverse = 0;
		
		return;
	}
	if(get_direct_nutrition(nutri_create_dept_no, num, &nutriStr4) < 0) {
		caption_split_by_code(0xE00C,&cap,0);
		safe_strncpy(nutriStr4.description,cap.form, sizeof(nutriStr4.description));
	}
	
	Dsp_Write_String(disp_p,(HUGEDATA char*)nutriStr4.description);
	DspStruct.Reverse = 0;
	
	MenuRefresh = ON;
	return;
}
#endif // USE_DIRECT_NUTRITION

void	nutri_create(void)
{
	NUTRIFACT_STRUCT  nutrifact_str;
	NUTRIFACT_STRUCT2  nutrifact_str2;
#ifdef USE_EU_NUTRITIONFACT
	NUTRIFACT_STRUCT3  nutrifact_str3;
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	NUTRIFACT_STRUCT4  nutrifact_str4;
	INT16U deptno;
#endif
	CAPTION_STRUCT 	  cap;	
 	INT16U	str_size; 
	INT16U	result, old_font, key_mode;	    
	 
	INT32U	addr_start; 
	INT32U	addr_flash;
	INT8U k, ucTemp, nutri_type=0;
#ifdef USE_DIRECT_NUTRITION
    INT32U max_nutri_no;
#else
	INT16U max_nutri_no;
#endif
	INT16U i;
	INT8U set_str[3][31];
#ifdef USE_EU_NUTRITIONFACT
	INT32U set_val[MAX_NUTRIFACT_ITEM_EU+1];
	INT8U eu_nutri_str[30];
	INT8U form_len;
#else
	INT32U set_val[MAX_NUTRIFACT_ITEM+1];
#endif
#ifdef USE_DIRECT_NUTRITION
	INT8U old_page, ret;
	INT32U pluno, plu_addr, key;
#endif

	INT8U decimal_point=0;

	key_mode=KEY_GetMode();
	KEY_SetMode(1);
	old_font=DspStruct.Id1;
	DspLoadFont1(DSP_MENU_FONT_ID);

	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
	nutri_type = ucTemp&0x0f;
#ifdef COUNTRY_USA
	nutri_type = NUTRI_TYPE_2COL_USA;
#endif
	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
	{
		max_nutri_no = MAX_NUTRIFACT2_NO;
		str_size = sizeof(nutrifact_str2);
	}
#ifdef USE_EU_NUTRITIONFACT
	else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
	{
		max_nutri_no = MAX_NUTRIFACT3_NO;
		str_size = sizeof(nutrifact_str3);
	}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	else if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
	{
  #ifdef USE_DIRECT_NUTRITION
		max_nutri_no = MAX_PLU_NUMBER; 
  #else
		max_nutri_no = MAX_NUTRIFACT4_NO;
  #endif
		str_size = sizeof(nutrifact_str4);
	}
#endif
	else
	{
		max_nutri_no = MAX_NUTRIFACT_NO;
		str_size = sizeof(nutrifact_str);
	}

	set_val[0] = 1;
  #ifdef USE_DIRECT_NUTRITION
	Dsp_Fill_Buffer(0);
	deptno = status_scale.departmentid;
	pluno=1;
	ret = plu_input_department_only(&deptno, 0);
	if(ret==2) {
		goto RET_ESC;
	}
	if(ret==3) { //KP_SALE
//		flagGotoSale = ON;
		goto RET_ESC;
	}
	if (deptno<1) {
		goto RET_ESC;
	}
	while(1) {
		key = plu_table_seq_search(deptno, &pluno, MAX_PLU_NUMBER);
		if(key == 0) {
			goto RET_ESC;
		}
		else {
			set_val[0] = pluno;
			break;
		}
	}
  #else
	addr_start =  DFLASH_BASE + FLASH_NUTRIFACT_AREA;
	// Get Last Number
	caption_split_by_code(70, &cap,0);
	for (i=0; i<max_nutri_no; i++) {
		addr_flash = addr_start + (str_size * i);
		set_val[0] = Flash_wread(addr_flash);
		if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
		{
			cap.input_max = MAX_NUTRIFACT2_NO;
		}
#ifdef USE_EU_NUTRITIONFACT
		else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
		{
			cap.input_max = MAX_NUTRIFACT3_NO;
		}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
		else if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		{
			cap.input_max = MAX_NUTRIFACT4_NO;
		}
#endif
		if ((set_val[0]<cap.input_min) || (set_val[0]>cap.input_max)) 
		{
			set_val[0]=i+1;
			break;
		}
	}
	if ((set_val[0]<cap.input_min) || (set_val[0]>cap.input_max)) set_val[0]=cap.input_min;
#endif
	while(1)
	{
#ifdef USE_DIRECT_NUTRITION
		get_direct_nutrition(deptno, set_val[0], &nutrifact_str4);
		safe_strncpy((char*)set_str[0], (char*)nutrifact_str4.description, sizeof(set_str[0]));
		safe_strncpy((char*)set_str[1], (char*)nutrifact_str4.serving_size, sizeof(set_str[1]));   //serving size
		safe_strncpy((char*)set_str[2], (char*)nutrifact_str4.serving_per, sizeof(set_str[2]));    //serving per container
		for (k = 0; k < MAX_NUTRIFACT_ITEM; k++)
			set_val[k+1] = nutrifact_str4.nutri_data[k];
#else // USE_DIRECT_NUTRITION

		addr_start =  DFLASH_BASE + FLASH_NUTRIFACT_AREA;
		addr_flash = addr_start + (str_size * (set_val[0]-1));

		if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)
		{
			Flash_sread(addr_flash, (HUGEDATA INT8U *)&nutrifact_str2, str_size);
			if(nutrifact_str2.code != set_val[0])  {
				memset((INT8U*)&nutrifact_str2, 0x00, str_size);
			}
			nutrifact_str2.code = set_val[0];
			strcpy((HUGEDATA char*)set_str[0], (HUGEDATA char*)nutrifact_str2.description);
			strcpy((HUGEDATA char*)set_str[1], (HUGEDATA char*)nutrifact_str2.serving_size);   //serving size
			strcpy((HUGEDATA char*)set_str[2], (HUGEDATA char*)nutrifact_str2.serving_per);    //serving per container
			for (k = 0; k < MAX_NUTRIFACT_ITEM; k++)
			{
				set_val[k+1] = nutrifact_str2.nutri_data[k];
			}
		}
#ifdef USE_EU_NUTRITIONFACT
		else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
		{
			Flash_sread(addr_flash, (HUGEDATA INT8U *)&nutrifact_str3, str_size);
			if(nutrifact_str3.code != set_val[0])  {
				memset((INT8U*)&nutrifact_str3, 0x00, str_size);
			}
			nutrifact_str3.code = set_val[0];
			strcpy((HUGEDATA char*)set_str[0], (HUGEDATA char*)nutrifact_str3.description);
			strcpy((HUGEDATA char*)set_str[1], (HUGEDATA char*)nutrifact_str3.serving_size);   //serving size
			strcpy((HUGEDATA char*)set_str[2], (HUGEDATA char*)nutrifact_str3.serving_per);    //serving per container
			for (k = 0; k < MAX_NUTRIFACT_ITEM_EU; k++)
			{
				set_val[k+1] = nutrifact_str3.nutri_data[k];
			}
		}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
		else if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		{
			Flash_sread(addr_flash, (INT8U *)&nutrifact_str4, str_size);
			if(nutrifact_str4.code != set_val[0])  {
				memset((INT8U*)&nutrifact_str4, 0x00, str_size);
			}
			nutrifact_str4.code = set_val[0];
			strcpy((char*)set_str[0], (char*)nutrifact_str4.description);
			strcpy((char*)set_str[1], (char*)nutrifact_str4.serving_size);   //serving size
			strcpy((char*)set_str[2], (char*)nutrifact_str4.serving_per);    //serving per container
			for (k = 0; k < MAX_NUTRIFACT_ITEM; k++)
			{
				set_val[k+1] = nutrifact_str4.nutri_data[k];
			}
		}
#endif
		else
		{
			Flash_sread(addr_flash, (HUGEDATA INT8U *)&nutrifact_str, str_size);
			if(nutrifact_str.code != set_val[0])  {
				memset((INT8U*)&nutrifact_str, 0x00, str_size);
			}
			nutrifact_str.code = set_val[0];
			strcpy((HUGEDATA char*)set_str[0], (HUGEDATA char*)nutrifact_str.description);
			strcpy((HUGEDATA char*)set_str[1], (HUGEDATA char*)nutrifact_str.serving_size);   //serving size
			strcpy((HUGEDATA char*)set_str[2], (HUGEDATA char*)nutrifact_str.serving_per);    //serving per container
			for (k = 0; k < MAX_NUTRIFACT_ITEM; k++)
			{
				set_val[k+1] = nutrifact_str.nutri_data[k];
			}
		}
#endif // USE_DIRECT_NUTRITION

		Menu_Init();
		caption_split_by_code(0x1321, &cap,0);
		Menu_HeadCreate((HUGEDATA char *)cap.form);
  #ifdef USE_DIRECT_NUTRITION
		caption_split_by_code(0x9266, &cap,0);	// "PLU No"
		cap.input_dlength = ilog(MAX_PLU_NUMBER);
		cap.input_length = cap.input_dlength;
		cap.input_min = 1;
		nutri_create_dept_no = deptno;
  #else
		caption_split_by_code(70, &cap,0);
  #endif
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							max_nutri_no, cap.input_min, 0, &set_val[0], NULL, NULL, NULL);
		Menu_AddEndFunction(1, nutrifact_dataUpdate);

//		input_max = cap.input_max;

		caption_split_by_code(0x132B, &cap,0);	//"Description"
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, set_str[0], NULL, NULL);

		caption_split_by_code(0x97c0, &cap,0);	//"Serving Size (Text)    :"
		caption_adjust(&cap,NULL);
#ifdef USE_DIRECT_NUTRITION
		cap.input_dlength = sizeof(nutrifact_str4.serving_size);
		cap.input_length = sizeof(nutrifact_str4.serving_size);
#endif
		Menu_InputCreate(3, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, set_str[1], NULL, NULL);

		caption_split_by_code(0x97c1, &cap,0);	//"Serving Per Con        :"
		caption_adjust(&cap,NULL);
		Menu_InputCreate(4, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, NULL, set_str[2], NULL, NULL);

#ifdef USE_EU_NUTRITIONFACT
		if(nutri_type == NUTRI_TYPE_2COL_CHL || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_NZL
		|| nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT || nutri_type == NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT || nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
#else
		if(nutri_type == NUTRI_TYPE_2COL_CHL || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_NZL\
		|| nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT || nutri_type == NUTRI_TYPE_2COL_NZL_ACTUAL_WEIGHT)
#endif
		{

			caption_split_by_code(0x97c2, &cap,0);	//"Base Unit (0=g/1=ml)   :"
			caption_adjust(&cap,NULL);
			Menu_InputCreate(5, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, 0, &set_val[1], NULL, NULL, NULL);

			caption_split_by_code(0x97c3, &cap,0);	//"Serving Size (Num)     :"
			caption_adjust(&cap,NULL);
			Menu_InputCreate(6, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, 0, &set_val[2], NULL, NULL, NULL);

			caption_split_by_code(0x97c4, &cap,0);	//"Base Size              :"
			caption_adjust(&cap,NULL);
			Menu_InputCreate(7, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, 0, &set_val[3], NULL, NULL, NULL);
		}
		
#ifdef USE_EU_NUTRITIONFACT
		if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
		{
			for (k = 3; k < MAX_NUTRIFACT_ITEM_EU; k++)
			{
				//caption_split_by_code(0x97D2+k, &cap,0);
				decimal_point = ReturnDecimalPointNutriTable(nutri_type, k);
				//if(decimal_point) cap.input_length++;
				//caption_adjust(&cap,NULL);
				strcpy((char *)eu_nutri_str, eu_nutrition_menu_caption[k-3]); 
				strcat((char *)eu_nutri_str,":[");
				form_len = strlen((char *)eu_nutri_str);
				for (i = 0; i < 6; i++)
					strcat((char *)eu_nutri_str," ");
				strcat((char *)eu_nutri_str,"]");
				Menu_InputCreate(k+5, (HUGEDATA char *)eu_nutri_str, MENU_TYPE_NUM, form_len, 6, 6,
									99999, 0, decimal_point, &set_val[k+1], NULL, NULL, NULL);
			}
		}
		else
		{
			for (k = 3; k < MAX_NUTRIFACT_ITEM; k++)
			{
				caption_split_by_code(0x97c2+k, &cap,0);
				decimal_point = ReturnDecimalPointNutriTable(nutri_type, k);

				if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
					|| nutri_type == NUTRI_TYPE_6DIG_CZE) 
				{
					cap.input_max = 999999l;
					cap.input_length = 6;
				}
				if(decimal_point) cap.input_length++;
				caption_adjust(&cap,NULL);
				Menu_InputCreate(k+5, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
									cap.input_max, cap.input_min, decimal_point, &set_val[k+1], NULL, NULL, NULL);
			}
		}
#else // USE_EU_NUTRITIONFACT
		for (k = 3; k < MAX_NUTRIFACT_ITEM; k++)
		{
			caption_split_by_code(0x97c2+k, &cap,0);
			decimal_point = ReturnDecimalPointNutriTable(nutri_type, k);

			if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
				|| nutri_type == NUTRI_TYPE_6DIG_CZE) 
			{
				cap.input_max = 999999l;
				cap.input_length = 6;
			}
			if(decimal_point) cap.input_length++;
			caption_adjust(&cap,NULL);
			Menu_InputCreate(k+5, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
								cap.input_max, cap.input_min, decimal_point, &set_val[k+1], NULL, NULL, NULL);
  #ifdef COUNTRY_USA
			if((0x97c2 + k) == 0x97c6 || (0x97c2 + k) == 0x97d0 || (0x97c2 + k) == 0x97d2 
				|| (0x97c2 + k) == 0x97d4 || (0x97c2 + k) == 0x97d5 ) {
				Menu[Menu_GetMenuId(k+5)].inhibit = ON;
			}
  #endif
		}
 #ifdef COUNTRY_USA
			//menu reordering
			MenuReorder = ON;
			int j = 0;
			for (i = 0; i < MAX_MENU_INDEX; i++) {
				if(Menu[i].inhibit) 
					continue;
				else
					Menu_order_table[j++] = i;
			}
			Menu_order_table[13] = 21;
			Menu_order_table[14] = 14;
			Menu_order_table[15] = 16;
			Menu_order_table[16] = 18;
			Menu_order_table[17] = 22;
			Menu_order_table[18] = 23;
			Menu_order_table[19] = 0xff;
			for(i = 20; i < MAX_MENU_INDEX; i++)
				Menu_order_table[i] = 0;			
 #endif // COUNTRY_USA		
#endif // USE_EU_NUTRITIONFACT
		result = Menu_InputDisplay();
#ifdef COUNTRY_USA
		MenuReorder = OFF;
#endif				
		if(result == MENU_RET_SAVE) 
		{
			addr_flash = addr_start + (str_size * (set_val[0]-1));
			if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
				|| nutri_type == NUTRI_TYPE_6DIG_CZE)
			{
				memset((INT8U*)&nutrifact_str2, 0x00, str_size);
				nutrifact_str2.code = set_val[0];
				strcpy((HUGEDATA char*)nutrifact_str2.description, (HUGEDATA char*)set_str[0]);
				strcpy((HUGEDATA char*)nutrifact_str2.serving_size,(HUGEDATA char*)set_str[1]);   //serving size
				strcpy((HUGEDATA char*)nutrifact_str2.serving_per, (HUGEDATA char*)set_str[2]);    //serving per container
				for (k = 0; k < MAX_NUTRIFACT_ITEM; k++)
				{
					nutrifact_str2.nutri_data[k] = set_val[k+1];
				}
				Flash_swrite(addr_flash, (HUGEDATA INT8U *)&nutrifact_str2, str_size);
			}
#ifdef USE_EU_NUTRITIONFACT
			else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
			{
				memset((INT8U*)&nutrifact_str3, 0x00, str_size);
				nutrifact_str3.code = set_val[0];
				strcpy((HUGEDATA char*)nutrifact_str3.description, (HUGEDATA char*)set_str[0]);
				strcpy((HUGEDATA char*)nutrifact_str3.serving_size,(HUGEDATA char*)set_str[1]);   //serving size
				strcpy((HUGEDATA char*)nutrifact_str3.serving_per, (HUGEDATA char*)set_str[2]);    //serving per container
				for (k = 0; k < MAX_NUTRIFACT_ITEM_EU; k++)
				{
					nutrifact_str3.nutri_data[k] = set_val[k+1];
				}
				Flash_swrite(addr_flash, (HUGEDATA INT8U *)&nutrifact_str3, str_size);
			}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
			else if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
			{
				memset((INT8U*)&nutrifact_str4, 0x00, str_size); 
  #ifdef USE_DIRECT_NUTRITION
			
  #else
				nutrifact_str4.code = set_val[0]; 
  #endif
				strcpy((char*)nutrifact_str4.description, (char*)set_str[0]);
				strcpy((char*)nutrifact_str4.serving_size,(char*)set_str[1]);   //serving size
				strcpy((char*)nutrifact_str4.serving_per, (char*)set_str[2]);    //serving per container
				for (k = 0; k < MAX_NUTRIFACT_ITEM; k++)
				{
					nutrifact_str4.nutri_data[k] = set_val[k+1];
				}
  #ifdef USE_DIRECT_NUTRITION
				nutrifact_str4.dept_no = nutri_create_dept_no;
				nutrifact_str4.plu_num = set_val[0];
				if(insert_direct_nutrition(&nutrifact_str4) < 0) {
					caption_split_by_code(0xca05, &cap,0);
					display_message_page_mid_delay(cap.form, 30);
					goto RET_ESC;
				}
  #else
				Flash_swrite(addr_flash, (INT8U *)&nutrifact_str4, str_size); 
  #endif
			}
#endif
			else
			{			
				memset((INT8U*)&nutrifact_str, 0x00, str_size);
				nutrifact_str.code = set_val[0];
				strcpy((HUGEDATA char*)nutrifact_str.description, (HUGEDATA char*)set_str[0]);
				strcpy((HUGEDATA char*)nutrifact_str.serving_size,(HUGEDATA char*)set_str[1]);   //serving size
				strcpy((HUGEDATA char*)nutrifact_str.serving_per, (HUGEDATA char*)set_str[2]);    //serving per container
				for (k = 0; k < MAX_NUTRIFACT_ITEM; k++)
				{
					nutrifact_str.nutri_data[k] = set_val[k+1];
				}
				Flash_swrite(addr_flash, (HUGEDATA INT8U *)&nutrifact_str, str_size);
			}
#ifdef USE_DIRECT_NUTRITION
			while(1) {
				if(set_val[0] == MAX_PLU_NUMBER)
					set_val[0] = 1;
				set_val[0]++;
				key = plu_table_seq_search(deptno, &set_val[0], MAX_PLU_NUMBER);
				if(key == 0) {
					goto RET_ESC;
				} else {
					break;
				}
			}
#else
			if(set_val[0] <max_nutri_no) set_val[0]++;
			else   set_val[0] = max_nutri_no; 
#endif // USE_DIRECT_NUTRITION
		}
		else if(result ==MENU_RET_ESC) break;
	}
#ifdef USE_DIRECT_NUTRITION
RET_ESC:
#endif
	Dsp_Fill_Buffer(0);						// LCD Buffer clear
	DspLoadFont1(old_font);
	Dsp_Diffuse();
	KEY_SetMode(key_mode);
	sale_display_update(0x0fff);
}

void    nutri_delete(void)		// command code  0x1252
{	
	INT8U ucTemp, nutri_type=0;
	
	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
	nutri_type = ucTemp&0x0f;
#ifdef COUNTRY_USA
	nutri_type = NUTRI_TYPE_2COL_USA;
#endif		
	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)	
	{
		menu1_delete(0x1323, 70, FLASH_NUTRIFACT_AREA, sizeof(NUTRIFACT_STRUCT2));
	}
#ifdef USE_EU_NUTRITIONFACT
	else if (nutri_type == NUTRI_TYPE_42ITEM_5DIG_EU_ACTUAL_WEIGHT)
	{
		menu1_delete(0x1323, 70, FLASH_NUTRIFACT_AREA, sizeof(NUTRIFACT_STRUCT3));
	}
#endif
#ifdef USE_ADDITIONAL_NUTRIENT
	else if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)	
	{
		menu1_delete(0x1323, 70, FLASH_NUTRIFACT_AREA, sizeof(NUTRIFACT_STRUCT4));
	}
#endif
	else
	{
		menu1_delete(0x1323, 70, FLASH_NUTRIFACT_AREA, sizeof(NUTRIFACT_STRUCT));
	}
}

#ifndef DISABLE_LIST_MENU
void  nutri_list(void)			// command code  0x1253
{		
	CAPTION_STRUCT 	   cap;
	POINT 		   disp_p;
	NUTRIFACT_STRUCT	nutrifact_str;
	NUTRIFACT_STRUCT2	nutrifact_str2;
#ifdef USE_ADDITIONAL_NUTRIENT
	NUTRIFACT_STRUCT4	nutrifact_str4;
#endif	
//	INT8U	y_gab=8;				// 폰트의 y 크기
//	INT8U	x_gab=6;				// 폰트의 x 크기

	char   	string_buf[34];//, end_flag = 0;
	long 	ret_long, i;
	INT16S	reverse_num;
	INT16S	j = 0, result, min_num, str_size;
	INT16S	line_ct, start_id, lcd_size;

	INT16U	read_code;
	INT32U	addr, addr_flash, cap_addr, max_num, start_num, end_num, cur_id;
	INT16U	code = 0x1322, cap_code = 0x1E00;
	INT8U	linenum;			   //se-hyung 20071001

	INT8U ucTemp, nutri_type=0;
  #ifdef USE_DIRECT_NUTRITION
	INT16U dept_no, line;
  	INT32U key, plu_id, line_id[4];
	INT32U page_num = 1;
	INT32U list_count = 0;
  #endif
  	INT8U input_digit;

	ucTemp = get_global_bparam(GLOBAL_SALE_SETUP_FLAG9);
	// nutri_type (0:normal, 1:CHL, 2:CAN, 3:AUS, 4:NZL, 5: AUS for actual weight, 6: NZL for actual weight)
	nutri_type = ucTemp&0x0f; 
#ifdef COUNTRY_USA
	nutri_type = NUTRI_TYPE_2COL_USA;
#endif
	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT\
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)	
	{
		str_size = sizeof(nutrifact_str2);
	}
#ifdef USE_ADDITIONAL_NUTRIENT
	else if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		str_size = sizeof(nutrifact_str4);
#endif		
	else str_size = sizeof(nutrifact_str); 

LIST_START:

  #ifdef USE_DIRECT_NUTRITION
	Dsp_Fill_Buffer(0);
	dept_no = 1;
	result=plu_input_department_only((INT16U *)&dept_no, 0);
	if (result==2) goto RET_ESC;
	if(result==3) {
		goto RET_ESC;
	}
  #endif
	caption_split_by_code(70,&cap,0);
	caption_adjust(&cap,NULL);
	min_num	= cap.input_min;					//
//	max_num = cap.input_max; 					// 
	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS || nutri_type == NUTRI_TYPE_2COL_6DIG_AUS_ACTUAL_WEIGHT\
		|| nutri_type == NUTRI_TYPE_6DIG_CZE)	
	{
		max_num = MAX_NUTRIFACT2_NO;
	}
#ifdef USE_ADDITIONAL_NUTRIENT
	else if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
		 max_num = MAX_NUTRIFACT4_NO;
#endif	
	else max_num = MAX_NUTRIFACT_NO;
#ifdef USE_DIRECT_NUTRITION
	max_num = MAX_PLU_NUMBER;
	input_digit = 6;
#else
	input_digit = 4;
#endif
  


	if(min_num <= 0)  {
		disp_p = point(0,0);
		message_error(disp_p);
	}

	Dsp_Fill_Buffer(0);
	display_title_page_change(code,1,1);
			
	addr = caption_search_by_code(cap_code);		
//	for(i = 0; i<Startup.line_max; i++)  {
	linenum = Startup.line_max;			      //se-hyung 20071001
	if (linenum > 3) linenum = 3;			      //se-hyung 20071001
	for(i = 0;i<linenum; i++)  {			      //se-hyung 20071001
		cap_addr = addr + CAP_MSG_LENGTH * i;
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		cap.input_length = input_digit;
		cap.input_dlength = cap.input_length;
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
REN1:
		if(i == 0)	ret_long = start_num;		
		if(i == 1)	ret_long = end_num;

		cap_addr = addr + CAP_MSG_LENGTH * i;
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
 		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, input_digit, 
				   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, input_digit, 
				   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, input_digit, 
				   				(i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
						 
			case KP_ARROW_UP:  if(i > 0)	i--;
					   else 	BuzOn(2);
					   goto REN1;    					  
			case KP_ESC     :  goto RET_ESC;

			case KP_SAVE    :  start_num = min_num;
					   end_num = max_num;
					   i++;
					   break;
			case KP_PAGE_DN :
			case KP_ARROW_DN:
			case KP_ENTER   :  if ( ret_long > max_num || ret_long < min_num ) {
						error_display_form1(0, min_num, max_num);
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
#ifdef USE_DIRECT_NUTRITION
	//cur_id = start_num;
	key = plu_table_seq_search(dept_no,(INT32U *)&start_num, end_num);
	if (key == 0)
	{
		error_display_caption(0xE00C);
		Dsp_Fill_Buffer(0);
		goto LIST_START;
	}
	cur_id = start_num;
#else
	addr_flash = DFLASH_BASE + FLASH_NUTRIFACT_AREA;
	cur_id     = start_num;
	start_id   = start_num;
#endif
READ_START:
	Dsp_Fill_Buffer(0);
#ifdef USE_DIRECT_NUTRITION
	if (Startup.menu_type)
	{
		sprintf(cap.form, "DEPT. PLU NO. DESCRIOPT.");
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071001
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);
	}

	line = 0;
	while (line < 3)
	{
		key = plu_table_seq_search(dept_no,(INT32U *)&cur_id,end_num);
		if(key > 0)
		{
			if (get_direct_nutrition(dept_no, cur_id, &nutrifact_str4) < 0)
			{
				cur_id++;
				continue;
			}
			if (line == 0) line_ct = cur_id;
			
			sprintf(string_buf, "%-5d %-7d %-s", dept_no, cur_id, nutrifact_str4.description);

			string_buf[lcd_size] = 0;				
			disp_p = point((line+Startup.menu_type)*Startup.menu_ygap, 0);
			Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
			Dsp_Diffuse();
			line += 1;
		}
		
		if (cur_id>=end_num)
		{
			break;
		}
		cur_id++;
	}
#else

	if (Startup.menu_type)
	{
		caption_split_by_code(0x71FF, &cap,0);
//		disp_p = point(0,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071001
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
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

		 	if (nutri_type == NUTRI_TYPE_2COL_6DIG_AUS)
			{
				Flash_sread(addr, (HUGEDATA INT8U*)&nutrifact_str2, str_size ); 
			 	sprintf(string_buf, "%3d %s", nutrifact_str2.code, nutrifact_str2.description);
			}
#ifdef USE_ADDITIONAL_NUTRIENT
			else if (nutri_type == NUTRI_TYPE_2COL_USA || nutri_type == NUTRI_TYPE_TOTALFAT_CAN)
			{
				Flash_sread(addr, (INT8U*)&nutrifact_str4, str_size ); 
  #ifdef USE_DIRECT_NUTRITION
  				sprintf(string_buf, "%6d %s", nutrifact_str4.plu_num, nutrifact_str4.description);
  #else
				sprintf(string_buf, "%3d %s", nutrifact_str4.code, nutrifact_str4.description);
  #endif
			}
#endif
			else
			{
				Flash_sread(addr, (HUGEDATA INT8U*)&nutrifact_str, str_size ); 
			 	sprintf(string_buf, "%3d %s", nutrifact_str.code, nutrifact_str.description);
				
			}
			string_buf[lcd_size] = 0;	       		
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
			Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0);
			hebrew_codetemp[strlen(string_buf)] = 0;
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
			Dsp_Diffuse();

			if (j == 1)
				start_id = cur_id;	
			
			if (j == line_ct)
				break;
		}
		
		cur_id++;	
	}
	if (cur_id >= end_num && j == 0) 
	{
		error_display_caption(0xE00C);
		goto  LIST_START;
	}
	cur_id = start_id;
#endif	
KEY_INPUT:
	while(!KEY_Read());
	BuzOn(1);
	
	switch(KeyDrv.CnvCode) 
	{
	case KP_ESC:
		goto RET_ESC;
	case KP_ARROW_UP:
#ifdef USE_DIRECT_NUTRITION // 1개씩 
		cur_id = line_ct;
		cur_id -= 1;
		while (1)
		{
			if(cur_id <= start_num)
			{
				cur_id = start_num;
				break;
			}
			
			key = plu_table_prev_search((INT16S)dept_no,(INT32U *)&cur_id,(INT32U)start_num);
			if (key)
			{
				if (get_direct_nutrition(dept_no, cur_id, &nutrifact_str4) < 0)
				{
					cur_id -= 1;
					continue;
				}
				else
				{
					goto READ_START;
				}
			}
			else
			{
				BuzOn(2);
				break;
			}
			cur_id -= 1;
		}
		goto KEY_INPUT;
#else
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
#endif
	case KP_PAGE_UP :
#ifdef USE_DIRECT_NUTRITION // 3개씩
		cur_id = line_ct;
		line = 0;
		while (line < Startup.line_max)
		{
			if(cur_id <= start_num)
			{
				cur_id = start_num;
				goto READ_START;
			}
			key = plu_table_prev_search((INT16S)dept_no,(INT32U *)&cur_id,(INT32U)start_num);
			if (key)
			{
				if (get_direct_nutrition(dept_no, cur_id, &nutrifact_str4) < 0)
				{
					cur_id -= 1;
					continue;
				}
				else
				{
					line += 1;
				}
			}
			cur_id -= 1;
		}

		if(line >= Startup.line_max) goto READ_START;
		else
		{
			BuzOn(2);
		}
		goto KEY_INPUT;
#else
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
#endif
	case KP_PAGE_DN :
#ifdef USE_DIRECT_NUTRITION
		while (1)
		{
			if(cur_id >= end_num)
			{
				cur_id = end_num;
				goto READ_START;
			}
			
			key = plu_table_seq_search((INT16S)dept_no,(INT32U *)&cur_id,(INT32U)end_num);
			if (key)
			{
				if (get_direct_nutrition(dept_no, cur_id, &nutrifact_str4) < 0)
				{
					cur_id += 1;
					continue;
				}
				else
				{
					goto READ_START;
				}
			}
			cur_id += 1;
		}
		goto KEY_INPUT;
#else
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
#endif
	case KP_ENTER:		
	case KP_ARROW_DN:
#ifdef USE_DIRECT_NUTRITION
		cur_id = line_ct;
		cur_id += 1; 
		while (1)
		{
			if(cur_id >= end_num)
			{
				cur_id = end_num;
				goto READ_START;
			}
			
			key = plu_table_seq_search((INT16S)dept_no,(INT32U *)&cur_id,(INT32U)end_num);
			if (key)
			{
				if (get_direct_nutrition(dept_no, cur_id, &nutrifact_str4) < 0)
				{
					cur_id += 1;
					continue;
				}
				else
				{
					goto READ_START;
				}
			}
			else
			{
				BuzOn(2);
				break;
			}
			cur_id += 1;
		}
		goto KEY_INPUT;
#else
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
#endif
	case KP_TEST:
  #ifdef USE_DIRECT_NUTRITION
  		nutri_print_list(start_num, end_num, dept_no);
  #else
		nutri_print_list(start_num, end_num);
  #endif
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

#ifdef USE_DIRECT_NUTRITION
void direct_nutri_menu_delete(void)
{
	CAPTION_STRUCT cap;
	INT16U deptno;
	INT32U pluno;
	INT8U result;
	char backup_directdraw;

	menu_display_menucode(CurMenuCode, 1);
	menu_displaytitle(0x1323,0);   
	menu_writepage(1, 1);
    	
	deptno = 1;
	pluno = 1;
	result = plu_input_department((INT16U *)&deptno, (INT32U *)&pluno, 0);
	if (result==2) goto RET_ESC;
	if(result==3) {
		goto RET_ESC;
	}
  switch (result) {
		case KP_ENTER:
		case KP_SAVE:
				delete_plu_direct_nutrition(deptno, pluno);
			break;
		default: BuzOn(2); break;
	}
RET_ESC:
	return;
}
int delete_plu_direct_nutrition(INT16U deptno, INT32U pluno)
{
	int i;
	NUTRIFACT_STRUCT4 nutrifact_str4;
	INT16U str_size = sizeof(NUTRIFACT_STRUCT4);
	for(i = 0; i < MAX_NUTRIFACT4_NO; i++) {
		Flash_sread(FLASH_DIRECT_NUTRIFACT_AREA + i * str_size, (INT8U *)&nutrifact_str4, str_size);
		if(nutrifact_str4.dept_no == deptno && nutrifact_str4.plu_num == pluno)
			break;
	}
	if(i == MAX_NUTRIFACT4_NO)
		return -1;
	else
		Flash_clear(FLASH_DIRECT_NUTRIFACT_AREA + i * str_size, 0, str_size);
	return 0;
}
int delete_all_plu_direct_nutrition(void)
{
	Flash_clear(FLASH_DIRECT_NUTRIFACT_AREA, 0, sizeof(NUTRIFACT_STRUCT4) * MAX_NUTRIFACT4_NO);
}
int get_direct_nutrition(INT16U deptno, INT32U pluno, NUTRIFACT_STRUCT4 * ret_nutriStr4)
{
	int i;
	NUTRIFACT_STRUCT4 nutriStr4;
	int strSize = sizeof(NUTRIFACT_STRUCT4);
	for(i = 0; i < MAX_NUTRIFACT4_NO; i++) {
		Flash_sread(FLASH_DIRECT_NUTRIFACT_AREA + i * strSize, (INT8U *)&nutriStr4, strSize);
		if(nutriStr4.dept_no == deptno && nutriStr4.plu_num == pluno)
			break;
	}
	if(i == MAX_NUTRIFACT4_NO) {
		memset(ret_nutriStr4, 0x00, strSize);
		return -1;
	}
	memcpy(ret_nutriStr4, &nutriStr4, strSize);
	return 0;
}
int insert_direct_nutrition(NUTRIFACT_STRUCT4 *nutriStr4)
{
	int i;
	NUTRIFACT_STRUCT4 nutri_fact4_temp;
	int sz = sizeof(NUTRIFACT_STRUCT4);
	for(i = 0; i < MAX_NUTRIFACT4_NO; i++) {
		//insert
		Flash_sread(FLASH_DIRECT_NUTRIFACT_AREA + i * sz, (INT8U *)&nutri_fact4_temp, sz);
		if(nutri_fact4_temp.dept_no == 0 || nutri_fact4_temp.plu_num == 0 || nutri_fact4_temp.dept_no == 0xff || nutri_fact4_temp.plu_num == 0xffffffff) 
			break;
		//overwrite exist
		if(nutri_fact4_temp.dept_no == nutriStr4->dept_no && nutri_fact4_temp.plu_num == nutriStr4->plu_num)
			break;
	}
	//memmory full
	if(i == MAX_NUTRIFACT4_NO) 
		return -1;
	else
		Flash_swrite(FLASH_DIRECT_NUTRIFACT_AREA + i * sz, (INT8U *)nutriStr4, sz);
	return 0;
}
#endif

#endif
