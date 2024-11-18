/**
********************************************************************************
* Copyright (c) 2003 CAS
* @brief   Ingredient Header File\n
* @file    Ingredient.h
* @date    2003/12/15
* @author  getwing (Jae-Ok Kim)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2003/12/15 created by getwing (Jae-Ok Kim) \n
********************************************************************************
*/
#ifndef _INGREDIENT_H
#define _INGREDIENT_H
/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#ifdef USE_9999_INDIRECT_INGREDIENT
#define INGREDIENT_EXIST_MASK	0x8000
#endif
/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
#ifdef USE_INGREDIENT_CODE_4DIGIT
typedef struct
{
	INT16U no;
	INT16U index;
} TABLE_INDEX_STRUCT;
#endif
#ifdef CPU_PIC32MZ
typedef struct {
	INT16U	no;
	INT8U	text[1022];	
}INGREDIENT_STRUCT;
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
#ifdef USE_DIRECT_INGREDIENT
extern INT16U MAX_DIRECT_INGREDIENT_large;
extern INT16U MAX_DIRECT_INGREDIENT_small;
extern INT32U FLASH_INGREDIENT_TEXT_AREA;
extern INT32U FLASH_INGREDIENT_TEXT_EXT_AREA;
#endif

extern INT16U INGREDIENT_STRUCT_SIZE;
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void ingredient_create(void);		// command code 0x1311
extern void ingredient_delete(void);		// command code 0x1313
extern void ingredient_list(void);			// command code 0x1312
extern INT16U ingredient_get_id(INT16U ingid);
extern INT16U ingredient_get_data(INT16U ingid,INT8U *data,INT16S size);
extern void ingredient_del_id(INT16U ing_number);
extern void ingredient_delete_all(void);
extern void ingredient_clear_expended_area_for_korea(void);
extern INT8U ingredient_check_number(INT16U ing_no, INT16U ing_block_no);

#ifdef USE_9999_INDIRECT_INGREDIENT
INT32U ingredient_get_id_by_table(INT16U ing_table_id);
INT32U ingredient_create_table(INT16U ing_table_id);
extern INT32U ingredient_fine_table_set_value(INT16U ing_table_id, INT8U set_option, INT32U set_value);
void ingredient_delete_table_all();
INT32U ingredient_get_table_pos(INT16U ing_table_pos);
#endif

#ifdef USE_INGREDIENT_CODE_4DIGIT
extern INT8U ingredient_table_insert(INT16U ingNo, INT8U *ingData);
extern INT8U ingredient_search_index(INT16U *num, INT16U *tableId, INT16U *index, INT8U nextSearch);
extern INT16U ingredient_no2index(INT16U num);
extern INT16U ingredient_index2no(INT16U num);
extern void ingredient_init(void);
extern INT32U ingredient_insert(INT16U ing_number, INT8U *ing_data, INT16U size);
#endif

#ifdef USE_DISCOUNT_MEMBERSHIP
extern void ingredient_disc_text_default(void);
#endif

#endif /* _INGREDIENT_H */

