
#ifndef _PLU_EDIT_H
#define _PLU_EDIT_H

//#define MAX_PLU_NUMBER	99999
#ifdef USE_CHN_CART_SCALE
#define MAX_PLU_NUMBER	999
#else
#define MAX_PLU_NUMBER	999999    //SG070315  
#endif

#endif

extern void clear_temp_inhibit_list(void);
extern INT8U plu_input_department(INT16U *deptno, INT32U _PACKED_DEF *pluno, INT8U dc);
extern INT16U plu_input_department_only(INT16U *deptno, INT8U allow);
extern INT8U plu_input(INT32U key,INT16U deptno,INT32U pluno, INT16S maxid, INT16S *enable_list, INT16S page_offset, INT16S saveflag);
extern void plu_copy_move_manager(INT16U code, char flag);
extern void plu_save_deptid(INT32U pnt,INT32U data,INT8U stype,INT16U usize);
//extern long plu_get_last_id(unsigned char deptno);
extern void plu_del_all(void);
extern void plu_new(void);
extern void plu_list(void);
extern void plu_copy(void);
extern void plu_move(void);
//extern void plu_copy_move_manager(INT16U code, char flag);
extern void plu_del_by_plu_no(void);	  // function code : 1161
extern void plu_del_by_department(void);		// function code : 1162
extern INT16S  plu_pageview(long pluno,INT16S cur_id,INT8U flag, INT16S *enable_list, INT16S maxid, INT16S page_offset);
extern void plu_inhibit_all(void);
extern void plu_inhibit_step(void);
extern void plu_set_usefield(INT16U index, INT8U use);
extern INT8U plu_inhibit_bycode(INT16U code,INT8U onoff);
extern void plu_inhibit_callback(INT16S num);
extern void plu_unit_price_change(void);
extern void plu_edit_item(INT8U deptno,INT32U pluno);
extern void plu_edit_origin(INT8U deptno,INT32U pluno);
extern void plu_edit_sellbydate(INT8U deptno,INT32U pluno);
extern void plu_edit_tax(PRICE_TYPE *pPrice,INT8U deptno,INT32U pluno);
extern void plu_apply_global_tax(PRICE_TYPE *pPrice,INT8U deptno,INT32U pluno);
extern void plu_edit_traceability(INT8U deptno,INT32U pluno);
extern void plu_edit_delete(INT8U departmentid,INT32U cur_pluid);
extern void plu_edit_speedkey(INT32U cur_pluid);
extern INT8U plu_edit_price_sub(INT8U dept,INT32U plu,INT8U mode);
extern void plu_edit_price(INT8U deptno,INT32U pluno);
extern void plu_edit_ingredient(INT8U deptno,INT32U pluno);
extern void plu_edit_date_flag(INT8U deptno,INT32U pluno);
extern INT16S	read_enable_list(INT16S *enable_list);
extern void	plu_speed_key_define(void);
//extern void	temp_number_of_linked_plu(INT16S linked_num);
extern void save_override_unit_price(void);
extern void save_changed_unitprice(INT16U deptno,INT32U pluno,INT16U pcs, long unitprice);
extern void save_plu_type_change(void);
extern void save_plu_type_changed_data(INT8U plu_type, INT16U deptno,INT32U pluno,INT16U pcs);
extern INT8U plu_inhibit_bycode(INT16U code,INT8U onoff);
extern void plu_mgr_trancnt(void);
extern INT8U plu_set_speedkey(INT16U setNo, INT16U keyNo, INT32U pluNo);
extern INT32U plu_get_speedkey(INT16U setNo, INT16U keyNo);
extern void display_remain_plu(void);

