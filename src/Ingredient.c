/**
********************************************************************************
* Copyright (c) 2003 CAS
* @brief   Ingredient API Source File\n
* @file    Ingredient.c
* @version 1.0
* @date    2003/12/15
* @author  getwing (Jae-Ok Kim)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2003/12/15 created by getwing (Jae-Ok Kim) \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "initial.h"
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
#include "prt_list.h"

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
#include "Ingredient.h"
#include "menu_code_text1.h"
#include "pgm_app.h"
#include "mode_error.h"
#include "mode_commun.h"
#include "ethernet_comm.h"
#include "plu_manager.h"

#ifdef USE_DISCOUNT_MEMBERSHIP
#include "mode_sale.h"
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#ifndef __SUB_STRUCT_SIZE
#define __SUB_STRUCT_SIZE
INT16U INGREDIENT_STRUCT_SIZE;
INT16U MAX_INGREDIENT_NO;
INT16U MAX_INDIRECT_INGREDIENT_large;
INT16U MAX_INDIRECT_INGREDIENT_small;
#ifdef USE_DIRECT_INGREDIENT
INT16U MAX_INGREDIENT_TEXT_NUMBER;
INT16U MAX_DIRECT_INGREDIENT_large;
INT16U MAX_DIRECT_INGREDIENT_small;
INT32U FLASH_INGREDIENT_TEXT_AREA;
INT32U FLASH_INGREDIENT_TEXT_EXT_AREA;
#endif
#ifdef USE_9999_INDIRECT_INGREDIENT
INT32S MAX_INGREDIENT_INDEX_NUMBER;
INT32U FLASH_INGREDIENT_TABLE_EXT_AREA;
HUGEDATA INT8U *ingredient_reminder_number;
#endif
#ifdef USE_INGREDIENT_CODE_4DIGIT
INT32S MAX_INGREDIENT_INDEX_NUMBER;
TABLE_INDEX_STRUCT IngredientTable[INGREDIENT_BASIC_AREA_QTY];
INT16U IngTableLastId;
#endif
#endif

/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
#ifdef USE_DISCOUNT_MEMBERSHIP
#define MAX_DISC_CAP_SIZE 30
struct DISCOUNT_TEXT
{
	INT16U id;
	INT8U disc_text[MAX_DISC_CAP_SIZE];
};

  #ifdef USE_LOTTEMART_DEFAULT
#define MAX_DISC_CAP_NUM	10

struct DISCOUNT_TEXT default_cap_for_disc[MAX_DISC_CAP_NUM] = {	
	{DISC_ID_CAP_BASE+ MEMBERSHIP_DISC, "엘포인트"},
	{DISC_ID_CAP_BASE+ CARD_DISC, "행사카드"},
	{DISC_ID_CAP_BASE+ MEMBERSHIP_CARD_DISC, "엘포인트 +행사카드"},
	{CAP_BEFORE_MEMBERS_DISC, "비회원가"},
	{CAP_AFTER_MEMBERS_DISC, "회원가"},
	{CAP_BEFORE_MEMBERS_DISC, "비회원가"},
	{CAP_BEFORE_CARD_DISC, "판매가"},
	{CAP_AFTER_CARD_DISC, "카드가"},
	{CAP_BEFORE_MEBERS_AND_CARD_DISC, "비회원가"},
	{CAP_AFTER_MEBERS_AND_CARD_DISC, "최종 혜택가"},
};
  #endif
#endif

/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
extern INT32U FLASH_ING_EXT_AREA_PNT;
extern INT32U FLASH_ING_EXT2_AREA_PNT;
extern INT32U FLASH_ING_NOR_AREA_PNT;
extern INT16U EXT_INGREDIENT_NO;
extern INT8U menu_readmode;

extern INT32U ing_calc_addr_by_key(INT16U ingid);
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void ingredient_move(INT16U ing_number);
static void ingredient_w_restore(INT16U ing_number);
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
#ifdef USE_9999_INDIRECT_INGREDIENT
/**
********************************************************************************
* @brief    ingredient_get_id_by_table function\n
* @remark   \n
********************************************************************************
*/
INT32U ingredient_get_table_pos(INT16U ing_table_pos)
{
	INT32U ing_table_addr;	
	INT32U temp_table_id;	

	ing_table_addr = FLASH_INGREDIENT_TABLE_EXT_AREA;
	temp_table_id = Flash_lread(ing_table_addr+ing_table_pos*sizeof(INT32U));

	return temp_table_id;
}
#endif

#ifdef USE_INGREDIENT_CODE_4DIGIT
INT8U ingredient_search_index(INT16U *num, INT16U *tableId, INT16U *index, INT8U nextSearch)
{
	INT16S right;
	INT16S left;
	INT16S middle;

	if (IngTableLastId == 0)
	{
		*tableId = 0;
		*index = IngredientTable[0].index;
		return 0;
	}
	right = IngTableLastId - 1;
	left = 0;

	while (right >= left)
	{
		middle = (right + left) / 2;

//sprintf(MsgBuf, "(%d) %4d, l=%3ld, r=%3ld, m=%3ld\r\n", *num, SlaughtHouseTable[middle].no, left, right, middle);
//MsgOut(MsgBuf);
		if (*num > IngredientTable[middle].no)
		{
			left = middle + 1;
		}
		else if (*num < IngredientTable[middle].no)
		{
			right = middle - 1;
		}		
		else //if (*num == deptPluNo)
		{
			*tableId = middle;
			*index = IngredientTable[middle].index;
			return 1;
		}
	}

	if (left > IngTableLastId - 1)
	{
		*tableId = IngTableLastId;
	}
	else
	{
		*tableId = left;
	}

	*index = 0;
	if (nextSearch)
	{
		if (*tableId < IngTableLastId)
		{
			*index = IngredientTable[*tableId].index;
			*num = IngredientTable[*tableId].no;
//sprintf(MsgBuf, "  [%d %4d, lid=%d, tid=%3d]\r\n", *num, SlaughtHouseTable[*tableId].no, SlaughtTableLastId, *tableId);
//MsgOut(MsgBuf);
			return 1;
		}
	}
	return 0;
	
}

INT16U ingredient_no2index(INT16U num)
{
	INT16S right;
	INT16S left;
	INT16S middle;

	if (IngTableLastId == 0)
	{
		return 0;
	}
	right = IngTableLastId - 1;
	left = 0;

	while (right >= left)
	{
		middle = (right + left) / 2;

		if (num > IngredientTable[middle].no)
		{
			left = middle + 1;
		}
		else if (num < IngredientTable[middle].no)
		{
			right = middle - 1;
		}		
		else //if (*num == deptPluNo)
		{
			return IngredientTable[middle].index;
		}
	}
	return 0;
	
}

INT16U ingredient_index2no(INT16U index)
{
	INT16S i;

	for (i = 0; i < IngTableLastId; i++)
	{
		if (IngredientTable[i].index == index)
		{
			return IngredientTable[i].no;
		}
	}
	return 0;
	
}

void ingredient_sort(void)	// bubble sort
{
	TABLE_INDEX_STRUCT tempStr;
	int i, j;

	for (i = INGREDIENT_BASIC_AREA_QTY - 1; i > 0; i--)
	{
		for (j = 0; j < i; j++) 
		{
			if (IngredientTable[j].no > IngredientTable[j + 1].no)
			{
				tempStr = IngredientTable[j];
				IngredientTable[j] = IngredientTable[j + 1];
				IngredientTable[j + 1] = tempStr;
			}
		}
	}

}

void ingredient_init(void)
{
	INT16U rcode;
	INT16U i;
	INT32U addr;

	IngTableLastId = 0;

	addr = DFLASH_BASE;
	addr += FLASH_INGREDIENT_AREA;
	for (i = 0; i < INGREDIENT_BASIC_AREA_QTY; i++) 
	{
		rcode = Flash_wread(addr);
		if ((rcode > 0) && (rcode < MAX_INGREDIENT_INDEX_NUMBER)) 
		{
			IngTableLastId++;
		}
		else
		{
			rcode = 0xffff;
		}
		IngredientTable[i].no = rcode;
		IngredientTable[i].index  = i + 1;
		addr += INGREDIENT_STRUCT_SIZE;
	}
	ingredient_sort();
}

#endif
/**
********************************************************************************
* @brief    ingredient_get_id function\n
* @remark   \n
********************************************************************************
*/
INT16U ingredient_get_id(INT16U ing_number)
{
	INT32U	ing_addr;
	INT16U	ing_check_id;	
#ifdef USE_9999_INDIRECT_INGREDIENT
	INT16U	ing_id;
	INT16U	ing_table_id;
#endif
#ifdef USE_INGREDIENT_CODE_4DIGIT
	INT8U being;
	INT16U tableId, index;
#endif

	if (ing_number==0) return 0;
#ifdef USE_9999_INDIRECT_INGREDIENT
	// Find Ingredient ID
	ing_table_id = ing_number;
	ing_id = ingredient_fine_table_set_value(ing_table_id,0,0);
	// Can't find same ingredient
	if(ing_id==0)return 0;
	ing_addr = ing_calc_addr_by_key(ing_id-1);
#elif defined(USE_INGREDIENT_CODE_4DIGIT)
	being = ingredient_search_index(&ing_number, &tableId, &index, OFF);
	if (being == 0)
	{
		return 0;
	}
	ing_addr = ing_calc_addr_by_key(index-1);
#else
	ing_addr = ing_calc_addr_by_key(ing_number-1);
#endif
	ing_check_id = Flash_wread(ing_addr);
#ifdef USE_INGREDIENT_CODE_4DIGIT
	if ((ing_check_id<1) || (ing_check_id > MAX_INGREDIENT_INDEX_NUMBER)) 
#else
	if ((ing_check_id<1) || (ing_check_id > MAX_INGREDIENT_NO)) 
#endif
	{
		ing_check_id=0;
	} 
	return ing_check_id;
}

/**
********************************************************************************
* @brief    ingredient_get_data function\n
* @remark   \n
********************************************************************************
*/
INT16U ingredient_get_data(INT16U ing_number,INT8U *data,INT16S size)
{
	INT32U	ing_addr;
	INT16U	ing_check_id;	
#ifdef USE_9999_INDIRECT_INGREDIENT
	INT16U	ing_id;
	INT16U	ing_table_id;
#endif
#ifdef USE_INGREDIENT_CODE_4DIGIT
	INT8U being;
	INT16U tableId, index;
#endif

	if (ing_number==0) 
	{
		data[0]=0;	
		return 0;
	}
#ifdef USE_9999_INDIRECT_INGREDIENT
	// Find Ingredient ID
	ing_table_id = ing_number;	
	ing_id = ingredient_fine_table_set_value(ing_table_id,0,0);
	// Can't find same ingredient
	if(ing_id==0)
	{
		data[0]=0;	
		return 0;
	}
	ing_addr = ing_calc_addr_by_key(ing_id-1);
#elif defined(USE_INGREDIENT_CODE_4DIGIT)
	being = ingredient_search_index(&ing_number, &tableId, &index, OFF);

	ing_addr = ing_calc_addr_by_key(index-1);
#else
	ing_addr = ing_calc_addr_by_key(ing_number-1);
#endif
	ing_check_id     = Flash_wread(ing_addr);
#ifdef USE_INGREDIENT_CODE_4DIGIT
	if ((ing_check_id<1) || (ing_check_id > MAX_INGREDIENT_INDEX_NUMBER)) 
#else
	if ((ing_check_id<1) || (ing_check_id > MAX_INGREDIENT_NO)) 
#endif
	{
		data[0]=0;
		ing_check_id=0;
	} 
	else 
	{
		if ((size==0) || (size>INGREDIENT_STRUCT_SIZE-2))size = INGREDIENT_STRUCT_SIZE-2;
		Flash_sread((ing_addr+2), (HUGEDATA INT8U *)data, (INT16U)size);
	}
	return ing_check_id;
}

/**
********************************************************************************
* @brief    callback_end_ingredient_no function\n
* @remark   \n
********************************************************************************
*/
void callback_end_ingredient_no(INT32U num)
{
	ingredient_move(num);

      	MenuRefresh = ON;
}

/**
********************************************************************************
* @brief    ingredient_create function\n
* @remark   \n
********************************************************************************
*/
void ingredient_create(void)		// command code  0x1311
{
	CAPTION_STRUCT cap;
	INT16U  ing_id;
	INT16U 	i, result;
	HUGEDATA INT8U *nv_pnt;
	INT32U cur_id;
	INT32U ing_max, ing_min;

	caption_split_by_code(25, &cap,0);
	ing_max = cap.input_max;
	ing_min = cap.input_min;
	while(1) {
#ifdef USE_INGREDIENT_CODE_4DIGIT
		for (i = 0; i < MAX_INGREDIENT_INDEX_NUMBER; i++) {
#else
		for (i = 0; i < MAX_INGREDIENT_NO; i++) {
#endif
			ing_id = ingredient_get_id(i + 1);
			if (ing_id == i + 1) {
				if (i + 1 > ing_max) {
					cur_id = ing_min;
					break;
				}
			} else {
				cur_id = i + 1;
		       		break;
			}
		}

		ingredient_move(cur_id);
		nv_pnt = NvramTempBuffer[0];
		nv_pnt += 2;

		Menu_Init();
	
		caption_split_by_code(0x1311, &cap,0);//""
		Menu_HeadCreate((HUGEDATA char *)cap.form);
	
		caption_split_by_code(25, &cap,0);//""	  
		//cap.input_min = 1;
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
		cap.input_max = (INT32U)(MAX_INGREDIENT_INDEX_NUMBER);
#else
		cap.input_max = (INT32U)(MAX_INGREDIENT_NO);
#endif
		cap.input_dlength = (INT16U)ilog(cap.input_max);
		cap.input_length = cap.input_dlength;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(1, (HUGEDATA char *)cap.form, MENU_TYPE_NUM, cap.input_x, cap.input_dlength, cap.input_length,
							cap.input_max, cap.input_min, 0, &cur_id, NULL, NULL, NULL);
		Menu_AddEndFunction(1, callback_end_ingredient_no);
	
		caption_split_by_code(0x131B, &cap,0);
		cap.input_dlength = Dsp_GetChars();
		cap.input_dlength -= strlen(cap.form);
		cap.input_dlength -= 2;
		caption_adjust(&cap,NULL);
		Menu_InputCreate(2, (HUGEDATA char *)cap.form, MENU_TYPE_STR, cap.input_x, cap.input_dlength, INGREDIENT_STRUCT_SIZE-2,
							cap.input_max, cap.input_min, 0, NULL, nv_pnt, NULL, NULL);						
		Menu_SetProperties(2, MENU_P_USE_LINEFEED|MENU_P_DSP_POSITION|MENU_P_PAGEUPDN_KEY|MENU_P_CHECK_BLANKS);
						
		result = Menu_InputDisplay();
		if(result == MENU_RET_SAVE) {
			ingredient_w_restore(cur_id);
			if (network_status.service_flag&0x20)	// send plu
			{
				commun_net_mask(2, (INT16U*)&cur_id, 0xff, 1);
			}
		} else if(result == MENU_RET_ESC) {
			break;
		}
	}
}

/**
********************************************************************************
* @brief    ingredient_list function\n
* @remark   \n
********************************************************************************
*/
#ifndef DISABLE_LIST_MENU
void 	ingredient_list(void)		// command code  0x1312
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	char   	string_buf[64];//, end_flag;
	long 	ret_long, i;
	INT16S	start_num, end_num;
	INT16S	j, result, min_num, max_num, lcd_size;	
	INT16S	line_ct, start_id;
	INT16U	read_code, reverse_num, cur_id;
	INT32U	addr, cap_addr;
	INT8U	linenum;			    //se-hyung 20071001	     list볼 범위 입력창 아래 쓰레기값 나오는거 수정
#ifdef USE_9999_INDIRECT_INGREDIENT
	INT32U  ingredient_id;
#endif

	lcd_size = Dsp_GetChars();
LIST_START:
	caption_split_by_code(25,&cap,0);
	cap.input_min = 1;
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
	cap.input_max = (INT32U)(MAX_INGREDIENT_INDEX_NUMBER);
#else
	cap.input_max = (INT32U)(MAX_INGREDIENT_NO);
#endif
    cap.input_dlength = ilog(cap.input_max);
	cap.input_length = cap.input_dlength;
	caption_adjust(&cap,NULL);
	min_num	= cap.input_min;				//
	max_num = cap.input_max; 				// 
	if(min_num <= 0)  {
		disp_p = point(0,0);
		message_error(disp_p);
	}
	Dsp_Fill_Buffer(0);
	display_title_page_change(0x1312,1,1);
	addr = caption_search_by_code(0x1E00);
	linenum = Startup.line_max;			      //se-hyung 20071001  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
	if (linenum > 3) linenum = 3;			      //se-hyung 20071001  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
	for(i = 0;i<linenum; i++)  {			      //se-hyung 20071001  list볼 범위 입력창 아래 쓰레기값 나오는거 수정
		cap_addr = addr + CAP_MSG_LENGTH * i;
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr, sizeof(cap));
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		cap.input_min = 1;
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
		cap.input_max = (INT32U)(MAX_INGREDIENT_INDEX_NUMBER);
#else
		cap.input_max = (INT32U)MAX_INGREDIENT_NO;
#endif
		cap.input_dlength = ilog(cap.input_max);
		cap.input_length = cap.input_dlength;
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
#if defined(USE_ARAB_CONVERT) || defined(HEBREW_FONT)
	start_num = cap.input_min;	//Min Number 변경 값 반영
	end_num = cap.input_max;	//Max Number 자리수 늘어나는 문제 수정
#else
	start_num = min_num;
	end_num = max_num;
#endif
	for(i=0; i<2; i++)  {
REN1:
		if(i == 0)	ret_long = start_num;		
		if(i == 1)	ret_long = end_num;
		cap_addr = addr + CAP_MSG_LENGTH * i;
		//memcpy((INT8U*)&cap,(INT8U *)cap_addr,sizeof(cap));
		Flash_sread(cap_addr, (INT8U*)&cap, sizeof(cap));
		cap.input_min = 1;
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
		cap.input_max = (INT32U)(MAX_INGREDIENT_INDEX_NUMBER);
#else
		cap.input_max = (INT32U)MAX_INGREDIENT_NO;
#endif
		cap.input_dlength = ilog(cap.input_max);
		cap.input_length = cap.input_dlength;
		caption_adjust(&cap,NULL);
		disp_p = point((i+Startup.menu_type)*Startup.menu_ygap,0);
#ifdef HEBREW_FONT
		Convert_HebrewPreview((INT8U *)cap.form, strlen(cap.form), 0, 0); 
		hebrew_codetemp[strlen(cap.form)] = 0;
		disp_p.x = 202 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, hebrew_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, cap.input_dlength, 
				   (i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#elif defined(USE_ARAB_CONVERT)
		Convert_ArabPreview((INT8U *)cap.form, strlen(cap.form), 0, 0, 0); 
		arab_codetemp[strlen(cap.form)] = 0;
		disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
		Dsp_Write_1stringLong(disp_p, arab_codetemp);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_ILNN,	0,(long*)&ret_long, cap.input_dlength, 
				   (i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#else
		Dsp_Write_String(disp_p,(HUGEDATA char*)cap.form);

		result=keyin_ulong(KI_GRAPHIC, KI_FORM_NORMAL,	0,(long*)&ret_long, cap.input_dlength, 
				   (i+Startup.menu_type)*Startup.menu_ygap,  cap.input_x*Startup.menu_xgap, 0,KI_NOPASS );
#endif
		switch (result){
			case KP_ARROW_UP:
					if(i > 0)	i--;
					else		BuzOn(2);
					goto REN1;
			case KP_ESC:
					goto RET_ESC;
			case KP_SAVE:
					start_num = min_num;
					end_num = max_num;
					i++;
					break;
			case KP_PAGE_DN:
			case KP_ARROW_DN:
			case KP_ENTER:	   
					if ( ret_long > max_num || ret_long < min_num ) 
					{
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
	line_ct    = Startup.line_max; // (Startup.menu_type?3:2);
	cur_id     = start_num;
	start_id   = start_num;

READ_START:
	Dsp_Fill_Buffer(0);
	if (Startup.menu_type) {
		caption_split_by_code(0x71FF, &cap,0);
		disp_p = point((Startup.menu_type-1) * Startup.menu_ygap, 0); //se-hyung 20071001     no. reference(descript.) 한줄 띄우기
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
#ifdef USE_9999_INDIRECT_INGREDIENT	
	while (cur_id <= MAX_INGREDIENT_NO) {
		ingredient_id = ingredient_get_table_pos(cur_id-1);
		if(!(ingredient_id & INGREDIENT_EXIST_MASK))
		{
			read_code = ingredient_get_id(ingredient_id);
			if(ingredient_id <= end_num && ingredient_id >= start_id)
			{
				if(read_code != 0)
				{
					j++;
					sprintf(string_buf,"%4d %4d:",cur_id,ingredient_id);
					ingredient_get_data((INT16U)ingredient_id,(INT8U *)&string_buf[10],28);
					string_buf[lcd_size] = 0;
					disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);
#ifdef HEBREW_FONT
					Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0);
					hebrew_codetemp[strlen(string_buf)] = 0;
					disp_p.x = 202 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#elif defined(USE_ARAB_FONT)	//기존 아랍ROM 적용되어 있음
					Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 0); 
					arab_codetemp[strlen(string_buf)] = 0;
					disp_p.x = LCD_Y_MARGIN_REAL - DspStruct.Hbits1 - disp_p.x;
					Dsp_Write_1stringLong(disp_p, arab_codetemp);
#else
					Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
#endif
					if (j == 1)	start_id = cur_id;
					if (j == line_ct) break;
				}
			}
		}
		cur_id++;
	}
	Dsp_Diffuse();
	if (cur_id >= MAX_INGREDIENT_NO && j == 0)
	{
		error_display_caption(0xE00C);
		goto  LIST_START;
	}
#else
	while (cur_id <= end_num) {
		read_code = ingredient_get_id(cur_id);
		if(read_code != 0)
		{
			if(read_code == cur_id /*>= min_num && read_code <= max_num*/)  
			{
				j++;
				sprintf(string_buf,"%4d ",read_code);
				ingredient_get_data((INT16U)read_code,(INT8U *)&string_buf[5],32);
#ifdef USE_ARAB_FONT//list
				//sprintf(string_buf, "%2d %1d", dept_str._code, dept_str._speedkey);
 #ifdef USE_ENG_BASED_ARAB_SUPPORT
				Convert_ArabPreview((INT8U *)string_buf, strlen(string_buf), 0, 0, 1); 
				eng_codetemp[strlen(string_buf)] = 0;
				Dsp_Write_1stringLong(disp_p, eng_codetemp);
 #else
                Convert_ArabPreview((INT8U *)&string_buf[5], strlen(string_buf)-5, 0, 0, 0);//ingredient 내용만 arab_codetemp에 저장.
				arab_codetemp[25-5] = 0;// LCD 한 줄에 표현 가능한 최대 표현 글자 - 5자리(ingredient No. 제외)

				string_buf[5] = 0; //ingredient No. 만 버퍼에 남김
				disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, LCD_Y_MARGIN_REAL);
                disp_p.x = disp_p.x - (DspStruct.Hbits1*strlen(string_buf));
				Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);

                disp_p.x -= DspStruct.Hbits1;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
                
                /*
				Convert_ArabPreview((INT8U *)&string_buf[5], strlen(string_buf)-5, 0, 0, 0);
				arab_codetemp[25-5] = 0;

				string_buf[lcd_size] = 0;
				disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);
				Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
				disp_p.x += (25-1)*8;
				Dsp_Write_1stringLong(disp_p, arab_codetemp);
				*/
 #endif
#elif defined(HEBREW_FONT)
			string_buf[lcd_size] = 0;
			disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0); 			 

			Convert_HebrewPreview((INT8U *)string_buf, strlen(string_buf), 0, 0);
			hebrew_codetemp[strlen(string_buf)] = 0;
			disp_p.x = 202 - disp_p.x;
			Dsp_Write_1stringLong(disp_p, hebrew_codetemp);
#else			
				string_buf[lcd_size] = 0;

				disp_p = point(((j-1)+Startup.menu_type)*Startup.menu_ygap, 0);
				Dsp_Write_String(disp_p, (HUGEDATA char *)string_buf);
#endif
				if (j == 1)	start_id = cur_id;	
				if (j == line_ct) break;
			}
		}
		cur_id++;
	}
	Dsp_Diffuse();
	if (cur_id > end_num && j == 0)
	{
		error_display_caption(0xE00C);
		goto  LIST_START;
	}
#endif

	cur_id = start_id;
KEY_INPUT:
	while(!KEY_Read());
	BuzOn(1);
	
	switch(KeyDrv.CnvCode) {
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
			read_code = ingredient_get_id(cur_id);
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
			read_code = ingredient_get_id(cur_id);
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
			read_code = ingredient_get_id(cur_id);
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
			read_code = ingredient_get_id(reverse_num);
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
		ingredient_print_list(start_num,end_num);
		goto KEY_INPUT;	
	case KP_FEED:
		keyapp_feed(OFF);
		goto KEY_INPUT;	
	default :	
		BuzOn(2);
		goto KEY_INPUT;
	}
	RET_ESC:    return;
}
#endif //#ifndef DISABLE_LIST_MENU

#ifdef USE_DIRECT_INGREDIENT//DEL080826#if (ROM_COUNTRY == ROM_RUS)
extern INT16U AutoFlushCache2Time;

/**
********************************************************************************
* @brief    ingredientTextDeleteAll function\n
* @remark   \n
********************************************************************************
*/
void ingredientTextDeleteAll(void)
{
	INT32U i;
	INT32U fl_xpnt;
	char temp[8];

	memset(temp,0,sizeof(temp));

	fl_xpnt = FLASH_INGREDIENT_TEXT_AREA;
	for(i=0; i< MAX_DIRECT_INGREDIENT_small; i++)
	{
		Flash_swrite_c2(fl_xpnt,(HUGEDATA INT8U *)temp,4);
		fl_xpnt += DEFAULT_INGREDIENT_TEXT_SIZE;	
	}

	fl_xpnt = FLASH_INGREDIENT_TEXT_EXT_AREA;
	for(i=MAX_DIRECT_INGREDIENT_small; i< MAX_INGREDIENT_TEXT_NUMBER ; i++)
	{
		Flash_swrite_c2(fl_xpnt,(HUGEDATA INT8U *)temp,4);
		fl_xpnt += DEFAULT_INGREDIENT_TEXT_SIZE;	
	}
	AutoFlushCache2Time = SysTimer100ms; 
} 
#endif

/**
********************************************************************************
* @brief    ingredient_delete function\n
* @remark   \n
********************************************************************************
*/
void    ingredient_delete(void)		// command code  0x1313
{
	CAPTION_STRUCT 		cap;
	POINT 			disp_p;
	long 	ret_long=1;
	INT16S  	result, current_num,start_line;
	INT16U ing_id;

	start_line = Startup.menu_type;			   //se-hyung 20071001

	Dsp_Fill_Buffer(0);	
	menu_displaytitle(0x1313,0);
	menu_writepage(1,1);   
   	
	caption_split_by_code(25,&cap,0);
	cap.input_min = 1;
	//cap.input_min =0; // 0 입력 후 Delete 시 "direct ingredient" all delete .SG070827 TEST ONLY
#if defined(USE_9999_INDIRECT_INGREDIENT) || defined(USE_INGREDIENT_CODE_4DIGIT)
	cap.input_max = (INT32U)(MAX_INGREDIENT_INDEX_NUMBER);
#else
	cap.input_max = MAX_INGREDIENT_NO;
#endif
	cap.input_dlength = ilog(cap.input_max);
	cap.input_length = cap.input_dlength;
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
	switch (result) {
		case KP_ENTER:	if ((ret_long>cap.input_max) || (ret_long<cap.input_min)) 
				{
				  	BuzOn(2);	
				  	goto REN1;	
				}
				current_num = ret_long - 1;	  	
				break;
		case KP_ESC:	goto RET_ESC; 
  #ifdef USE_KOR_INGREDIENT_EXPAND
		case KP_TEST:
				ingredient_clear_expended_area_for_korea();
				break;
  #endif
	     	default:	BuzOn(2);	
				goto REN1;		     	 	
	}
	ingredient_del_id(current_num+1);
	ing_id = current_num + 1;
	if (network_status.service_flag&0x20)	// send plu
	{
		commun_net_mask(2, (INT16U*)&ing_id, 0xff, 1);
	}
RET_ESC:
	Dsp_Fill_Buffer(0);
	Dsp_Diffuse();
 	return;
}

/**
********************************************************************************
* @brief    ingredient_delete_all function\n
* @remark   \n
********************************************************************************
*/
void ingredient_delete_all(void)
{
	INT32U p;
	char temp[4];
	INT16U i;
	memset(temp,0,4);
	p=FLASH_ING_NOR_AREA_PNT;
	for (i=0; i<MAX_INDIRECT_INGREDIENT_small; i++)
	{
		Flash_swrite(p,(HUGEDATA INT8U *)temp,2);
		p+=INGREDIENT_STRUCT_SIZE;
	}
	p=FLASH_ING_EXT_AREA_PNT;
	for (i=MAX_INDIRECT_INGREDIENT_small; i<MAX_INGREDIENT_NO;i++)
	{
		Flash_swrite(p,(HUGEDATA INT8U *)temp,2);
		p+=INGREDIENT_STRUCT_SIZE;
	}
	if (network_status.service_flag&0x20)	// send plu
	{
		commun_net_mask(2, NULL, 0xff, 99);
	}
#ifdef USE_9999_INDIRECT_INGREDIENT
	ingredient_delete_table_all();
#endif
#ifdef USE_INGREDIENT_CODE_4DIGIT
	ingredient_init();
#endif
}

#ifdef USE_KOR_INGREDIENT_EXPAND
/**
********************************************************************************
* @brief    Clear_Expended_Ingredient_Area_For_Korea function\n
* @remark   \n
********************************************************************************
*/
void ingredient_clear_expended_area_for_korea(void)
{
	INT32U p;
	char temp[4];
	INT16U i;
	INT8U data[20];
	
	sprintf(data,"  Init");
	display_string(1,(INT8U *)data);
	sprintf(data,"ING-EX");
	display_string(2,(INT8U *)data);
	VFD7_Diffuse();
	memset(temp,0,4);
	p=FLASH_ING_EXT2_AREA_PNT;
	for (i=MAX_INDIRECT_INGREDIENT_small+INGREDIENT_PREV_EXPAND_EXTER_AREA_QTY; i<MAX_INGREDIENT_NO;i++)
	{
		Flash_swrite(p,(HUGEDATA INT8U *)temp,2);
		p+=INGREDIENT_STRUCT_SIZE;
	}
}
#endif
/**
********************************************************************************
* @brief    ingredient_check_number function\n
* @remark   \n
********************************************************************************
*/
INT8U ingredient_check_number(INT16U ing_no, INT16U ing_block_no)
{
	INT16U read_code;
	INT16U i;
	INT16U block_count;
	INT32U addr;
#ifdef USE_INGREDIENT_CODE_4DIGIT
	INT8U being;
	INT16U tableId, index;
#endif
	//read_code = ingredient_get_id(ing_no);
#ifdef USE_INGREDIENT_CODE_4DIGIT
	being = ingredient_search_index(&ing_no, &tableId, &index, OFF);
	if (being == 0)
	{
		return OFF;
	}
	addr = ing_calc_addr_by_key(index-1);
#else
	addr = ing_calc_addr_by_key(ing_no-1);
#endif
	read_code = Flash_wread(addr);
	if(read_code == ing_no)
	{
		for (i = 0; i < INGREDIENT_STRUCT_SIZE - 2; i++)
		{
			if(Flash_bread(addr + 2 + i) == 0x00) break;
		}
	        block_count = i / INGREDIENT_COM_BLOCK_SIZE;
		if (i % INGREDIENT_COM_BLOCK_SIZE) block_count++;
		if ((block_count-1) >= ing_block_no) return ON;
		else return OFF;
	}
	return OFF;
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

/**
********************************************************************************
* @brief    ingredient_move function\n
* @remark   \n
********************************************************************************
*/
void ingredient_move(INT16U ing_number)
{
	INT32U   ing_addr;
	HUGEDATA INT8U *ing_data;	
	INT16U	 ing_check_id;
	INT16U	 ing_id;
#ifdef USE_9999_INDIRECT_INGREDIENT
	INT16U	 ing_table_id;
#endif
#ifdef USE_INGREDIENT_CODE_4DIGIT
	INT8U being;
	INT16U tableId, index;
#endif

	if (ing_number==0) return;
	ing_data  = NvramTempBuffer[0];	
#ifdef USE_9999_INDIRECT_INGREDIENT
	// Find Ingredient ID by Ingredient table
	ing_table_id = ing_number;
	ing_id = ingredient_get_id_by_table(ing_table_id);
	if(ing_id == 0)// empty
	{
		memset(ing_data,0,INGREDIENT_STRUCT_SIZE);	
	}
	else
	{
		ing_addr = ing_calc_addr_by_key(ing_id-1);
		ing_check_id = Flash_wread(ing_addr);
		if (ing_check_id != ing_table_id) 
		{
			memset(ing_data,0,INGREDIENT_STRUCT_SIZE);
		} 
		else 
		{
			memcpy(ing_data, (INT8U*)&ing_id, 2);
			Flash_sread((ing_addr+2), (HUGEDATA INT8U *)(ing_data+2), (INT16U)INGREDIENT_STRUCT_SIZE-2);
		}
	}
#else
#ifdef USE_INGREDIENT_CODE_4DIGIT
	being = ingredient_search_index(&ing_number, &tableId, &index, OFF);
	ing_addr = ing_calc_addr_by_key(index-1);
#else
	ing_addr = ing_calc_addr_by_key(ing_number-1);
#endif
	ing_id = Flash_wread(ing_addr);
	if (ing_id != ing_number) 
	{
		memset(ing_data,0,INGREDIENT_STRUCT_SIZE);
	} 
	else 
	{
		memcpy(ing_data, (INT8U*)&ing_id, 2);
		Flash_sread((ing_addr+2), (HUGEDATA INT8U *)(ing_data+2), (INT16U)INGREDIENT_STRUCT_SIZE-2);
	}
#endif
}

/**
********************************************************************************
* @brief    ingredient_w_restore function\n
* @remark   \n
********************************************************************************
*/
void ingredient_w_restore(INT16U ing_number)
{
	INT32U   ing_addr;
	HUGEDATA INT8U *ing_data;
#ifdef USE_9999_INDIRECT_INGREDIENT
	INT16U	 ing_id;
	INT16U	 ing_table_id;
#endif
#ifdef USE_INGREDIENT_CODE_4DIGIT
	INT8U being;
	INT16U i, tableId, index, newIndex;
#endif
	
	if (ing_number==0) return;
	ing_data  = NvramTempBuffer[0];
	
#ifdef USE_9999_INDIRECT_INGREDIENT
	// find same Ingredient ID
	ing_table_id = ing_number;
	ing_id = ingredient_fine_table_set_value(ing_table_id,0,0);	
	if(ing_id == 0)// empty
	{
		// Create Ingredient ID	
		ing_id = ingredient_create_table(ing_table_id);
		// Memory full
		if(ing_id==0)return;
	}
	ing_addr = ing_calc_addr_by_key(ing_id-1);
	Flash_wwrite(ing_addr,ing_table_id);
#elif defined(USE_INGREDIENT_CODE_4DIGIT)
	being = ingredient_search_index(&ing_number, &tableId, &index, OFF);

	if (being == 0)
	{
		if (IngTableLastId >= INGREDIENT_BASIC_AREA_QTY) return;
		newIndex = IngredientTable[IngTableLastId].index;
		for (i = IngTableLastId; i > tableId; i--)
		{
			IngredientTable[i] = IngredientTable[i - 1];
		}
		IngredientTable[tableId].no = ing_number;
		IngredientTable[tableId].index = newIndex;
		IngTableLastId++;
		index = IngredientTable[tableId].index;
	}
	ing_addr = ing_calc_addr_by_key(index-1);
#else
	ing_addr = ing_calc_addr_by_key(ing_number-1);
#endif
	Flash_wwrite(ing_addr,ing_number);	
	ing_addr+= 2;
	ing_data += 2;
	Flash_swrite(ing_addr,ing_data,INGREDIENT_STRUCT_SIZE-2);
}

#ifdef USE_INGREDIENT_CODE_4DIGIT
/**
********************************************************************************
* @brief    ingredient_insert function\n
* @remark   \n
********************************************************************************
*/
INT32U ingredient_insert(INT16U ing_number, INT8U *ing_data, INT16U size)
{
	INT32U   ing_addr;
	INT8U being;
	INT16U i, tableId, index, newIndex;

	//ing_data  = (HUGEDATA INT8U*)NVRAM_LOCAL_TEMP_AREA_PNT;
	
	being = ingredient_search_index(&ing_number, &tableId, &index, OFF);

	if (being == 0)
	{
		if (IngTableLastId >= INGREDIENT_BASIC_AREA_QTY) return 0;
		newIndex = IngredientTable[IngTableLastId].index;
		for (i = IngTableLastId; i > tableId; i--)
		{
			IngredientTable[i] = IngredientTable[i - 1];
		}
		IngredientTable[tableId].no = ing_number;
		IngredientTable[tableId].index = newIndex;
		IngTableLastId++;
		index = IngredientTable[tableId].index;
	}
	ing_addr = ing_calc_addr_by_key(index-1);
	Flash_wwrite(ing_addr,ing_number);	

	ing_addr+= 2;
	//ing_data += 2;
	Flash_swrite(ing_addr,ing_data,INGREDIENT_STRUCT_SIZE-2);
	
	return ing_addr;
}
#endif	//USE_INGREDIENT_CODE_4DIGIT

/**
********************************************************************************
* @brief    ingredient_del_id function\n
* @remark   \n
********************************************************************************
*/
void ingredient_del_id(INT16U ing_number)
{
	INT32U	ing_addr;
#ifdef USE_9999_INDIRECT_INGREDIENT
	INT16U	ing_id;
	INT16U	ing_table_id;
#endif
#ifdef USE_INGREDIENT_CODE_4DIGIT
	INT8U being;
	INT16U i, tableId, index, newIndex;
#endif

	if (ing_number==0) return;
#ifdef USE_9999_INDIRECT_INGREDIENT
	// Find Ingredient ID
	ing_table_id = ing_number;
	ing_id = ingredient_fine_table_set_value(ing_table_id,1,INGREDIENT_EXIST_MASK);
	// Can't find same ingredient
	if(ing_id==0)return;
	ing_addr = ing_calc_addr_by_key(ing_id-1);
#elif defined(USE_INGREDIENT_CODE_4DIGIT)
	being = ingredient_search_index(&ing_number, &tableId, &index, OFF);

	if (being)
	{
		newIndex = IngredientTable[tableId].index;
		for (i=tableId;i<IngTableLastId-1;i++)
		{
			IngredientTable[i] = IngredientTable[i + 1];
		}
		IngTableLastId--;
		IngredientTable[IngTableLastId].no = 0xffff;
		IngredientTable[IngTableLastId].index = newIndex;
	}
	ing_addr = ing_calc_addr_by_key(index-1);
#else
	ing_addr = ing_calc_addr_by_key(ing_number-1);
#endif
	ing_number   = 0;
	Flash_wwrite(ing_addr,ing_number);
}

#ifdef USE_9999_INDIRECT_INGREDIENT
/**
********************************************************************************
* @brief    ingredient_get_id_by_table function\n
* @remark   \n
********************************************************************************
*/
INT32U ingredient_get_id_by_table(INT16U ing_table_id)
{
	INT32U ing_table_addr;	
	INT32U temp_table_id;	
	INT32U pnt;		
	INT16U i;	
	ing_table_addr = FLASH_INGREDIENT_TABLE_EXT_AREA;
	// Find exist Igredient ID
	for(i=0;i<MAX_INGREDIENT_NO;i++)
	{
		temp_table_id = Flash_lread(ing_table_addr+i*sizeof(INT32U));
		if(temp_table_id == ing_table_id)
		{
			// return ingredient ID
			return (i+1);
		}
	}
	// 없음
	return 0;
}

/**
********************************************************************************
* @brief    ingredient_fine_table_set_value function\n
* @remark   \n
********************************************************************************
*/
INT32U ingredient_fine_table_set_value(INT16U ing_table_id, INT8U set_option, INT32U set_value)
{
	INT32U temp_table_id;
	INT32U ingredient_index_table_addr;	
	INT16U i;	
	ingredient_index_table_addr = FLASH_INGREDIENT_TABLE_EXT_AREA;
	// Find exist Igredient ID
	for(i=0;i<MAX_INGREDIENT_NO;i++)
	{
		temp_table_id = Flash_lread(ingredient_index_table_addr+i*sizeof(INT32U));
		if(!(temp_table_id&INGREDIENT_EXIST_MASK))// 상위비트 1번째 비트로 존재유무 확인
		{
			if(temp_table_id == ing_table_id) 
			{
				// set-option 이 0이 아니면 setvalue값을 저장
				if(set_option)
				{
					Flash_lwrite(ingredient_index_table_addr+i*sizeof(INT32U), set_value);
				}
				return (i+1);
			}
		}
	}
	// Can't find same ingredient table id
	return 0;
}

/**
********************************************************************************
* @brief    ingredient_create_table function\n
* @remark   \n
********************************************************************************
*/
INT32U ingredient_create_table(INT16U ing_table_id)
{
	INT32U pnt;
	INT32U ingredient_index_table_addr;	
	INT16U i;	
	ingredient_index_table_addr = FLASH_INGREDIENT_TABLE_EXT_AREA;
	// Create Igredient ID	
	for(i=0;i<MAX_INGREDIENT_NO;i++)
	{
		pnt = Flash_lread(ingredient_index_table_addr+i*sizeof(INT32U));
		if(pnt&INGREDIENT_EXIST_MASK)// 상위비트 1번째 비트로 존재유무 확인
		{
			// Save Ingredient Number
			Flash_lwrite(ingredient_index_table_addr+i*sizeof(INT32U), ing_table_id);
			// return ingredient ID
			return (i+1);
		}
	}
	// no empty space
return 0;
}

/**
********************************************************************************
* @brief    ingredient_delete_table_all function\n
* @remark   \n
********************************************************************************
*/
void ingredient_delete_table_all()
{
	INT32U ingredient_index_table_addr;	
	INT16U i;	
	ingredient_index_table_addr = FLASH_INGREDIENT_TABLE_EXT_AREA;
	// Create Igredient ID	
	for(i=0;i<MAX_INGREDIENT_NO;i++)
	{
		Flash_lwrite(ingredient_index_table_addr+i*sizeof(INT32U), INGREDIENT_EXIST_MASK);
	}
}


#endif

#ifdef USE_DISCOUNT_MEMBERSHIP
/**
********************************************************************************
* @brief    할인 정보 초기화\n
* @remark   \n
********************************************************************************
*/
void ingredient_disc_text_default(void)
{
  #ifdef USE_LOTTEMART_DEFAULT
	INT8U i;
	INT16U ing_number;
	INT32U   ing_addr;

	for(i = 0; i < MAX_DISC_CAP_NUM; i++)
	{
		ing_number = default_cap_for_disc[i].id;
		ing_addr = ing_calc_addr_by_key(ing_number - 1);
		Flash_wwrite(ing_addr,ing_number);	
		ing_addr+= 2;
		Flash_swrite(ing_addr, (HUGEDATA INT8U *)&default_cap_for_disc[i].disc_text, MAX_DISC_CAP_SIZE);	
	}
  #endif
}
#endif