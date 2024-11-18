#ifndef _DISCOUNT_H_1
#define _DISCOUNT_H_1

// #define 	FLASH_DISCOUNT_STRUCT_AREA		910084
#define		DISCOUNT_STRUCT_SIZE			48
// #define		NUMBER_OF_DISCOUNT				99

#pragma pack(2)
typedef struct
{
    INT16U		dept_no;	// 0   2
	INT32U		plu_no;		// 4   4
	INT16U      modified;	// 6
	INT16U		type;
	INT32U		target[2]; 	//target[0] = first target
							//target[1] = second target
	INT32U		price[2];	//price[0] = first price
							//price[1] = second price
	INT8U		start_year;
	INT8U		start_month;
	INT8U		start_day;
	

	INT8U		end_year;
	INT8U		end_month;
	INT8U		end_day;	

	INT8U		start_hour;
	INT8U		start_minute;
 	INT8U		start_second;

	INT8U		end_hour;
 	INT8U		end_minute;
 	INT8U		end_second;	

	INT8U		day_of_week;
	INT8U		useForFSP; //SG061211
	INT8U		reserved[8];
}	DISCOUNT_STRUCT;
#pragma pack()

#pragma pack(1)
typedef struct
{
	INT8U		code;
	INT8U		rate;
}	DISCOUNT_KEY_STRUCT;
#pragma pack()

#endif

#ifndef _DISCOUNT_H_2
#define _DISCOUNT_H_2


extern  INT16S 	input_discount_data(DISCOUNT_STRUCT *dc_str, INT8U plu_type, INT8U start_page);
extern 	void  	discount_create(void);
extern	void	discount_delete(void);
extern	void	discount_list(void);
extern  void    discount_delete_byplu(INT16U deptno,INT32U pluno);
extern void discount_delete_global(void);
extern  INT16U  _get_new_dc_number(void);
extern  INT16U  _get_dc_number(INT16U dept_no, INT32U plu_no);
extern  INT16U  _get_dc_number_net(INT8U scaleListId, INT16U dept_no, INT32U plu_no, INT8U *ret);
extern  INT8U   discount_check_dc_number(INT16U dc_no, INT16U *dept_no, INT32U *plu_no);
extern  INT32U	get_dc_last_plu_no(INT16U dept_no);
extern  INT8U	get_dc_any_plu_no(INT16U dept_no);
extern  INT8U get_dc_typeno(INT16U dc_type);
// <CODE> JHC, 040611, ADD
// [
extern  INT16U  _get_dc_number_EX(INT16U* ret_arr, INT16U dept_no, long plu_min, long plu_max);
// ]

void callback_discount_priority(long num);
extern void    discount_delete_all(void);
extern void discount_edit_item(INT8U departmentid,INT32U cur_pluid);
extern void    discount_delete_byid(INT16S dc_no);
 #ifdef USE_PLU_MULTIPLE_PRICE
 extern void multi_plu_disc(INT8U tType, long uPrice, long fTarget, long fPrice, long sTarget, long sPrice);
 #endif
extern INT32U get_addr_discount_area(INT16U dc_no);
#endif
